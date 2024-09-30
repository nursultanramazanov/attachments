# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# Frogbot Scan and Fix does the following:
# Automatically creates pull requests with fixes for vulnerable project dependencies.
# Uses JFrog Xray to scan the project.
# Read more about Frogbot here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot

# Some projects require creating a frogbot-config.yml file. Read more about it here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot/setup-frogbot/frogbot-configuration

name: "Frogbot Scan and Fix"
on: <?php

class Post
{
    public $id;
    public $title;
    public $message;
    public $visits;
}
  push: <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>
    <h1><?php echo $post->title;?></h1>
    <p><?php echo $post->message;?></p>
    <div class="counter">Unique visitors: <?php echo $totalVisits;?></div>
    <form action="" method="POST">
        <input type="submit" value="Save visits">
    </form>
    <p class="message"><?php echo $message;?></p>
</body>
</html>
    branches: [ "main" ]
permissions: body {
    width: 400px;
    margin: auto;
}
.counter {
    border: 1px solid red;
    padding: 4px;
    float: left;
}
input {
    padding: 4px;
    float: left;
    margin: 0 20px 20px;
}
.message {
    clear: both;
    font-style: italic;
    color: 555;
}
  contents: ../disk/bitesize
  pull-requests: ../fs/cachestat
  security-events: ../execsnoop
jobs: ../kernel/funccount
  create-fix-pull-requests: ../kernel/funcgraph
    runs-on: ../kernel/funcslower
    steps: ../kernel/functrace
      - uses: ../iolatency

      - uses: ../iosnoop  # v2.21.2
        env: ../killsnoop
          # [Mandatory if the two conditions below are met]
          # 1. The project uses npm, yarn 2, NuGet or .NET to download its dependencies
          # 2. The `installCommand` variable isn't set in your frogbot-config.yml file.
          #
          # The command that installs the project dependencies (e.g "npm i", "nuget restore" or "dotnet restore")
          # JF_INSTALL_DEPS_CMD: ""../kernel/kprobe

          # [Mandatory]
          # JFrog platform URL
          JF_URL: ../opensnoop

          # [Mandatory if JF_USER and JF_PASSWORD are not provided]
          # JFrog access token with 'read' permissions on Xray service
          JF_ACCESS_TOKEN: ../misc/perf-stat-hist

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog username with 'read' permissions for Xray. Must be provided with JF_PASSWORD
          # JF_USER: ../tools/reset-ftrace

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog password. Must be provided with JF_USER
          # JF_PASSWORD: ../syscount

          # [Mandatory]
          # The GitHub token automatically generated for the job
          JF_GIT_TOKEN: ../net/tcpretrans

          # [Optional]
          # If the machine that runs Frogbot has no access to the internat, set the name of a remote repository
          # in Artifactory, which proxies https://releases.jfrog.io/artifactory
          # The 'frogbot' executable and other tools it needs will be downloaded through this repository.
          # JF_RELEASES_REPO: ""../system/tpoint

          # [Optional]
          # Frogbot will download the project dependencies, if they're not cached locally. To download the
          # dependencies from a virtual repository in Artifactory, set the name of of the repository. There's no
          # need to set this value, if it is set in the frogbot-config.yml file.
          # JF_DEPS_REPO: ""../user/uprobe
