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
  contents: <?php

require "../vendor/autoload.php";

$config = parse_ini_file('../config.ini');

$options = array(
    'servers' => array(
       array('host' => $config['redisHost'], 'port' => $config['redisPort']),
    )
);

$rediska = new Rediska($options);
$rediskaAdapter = new \VisitCounter\Redis\RediskaAdapter($rediska);
$vc = new \VisitCounter\VisitCounter($rediskaAdapter);
$pageID = '1';
$userIP = $_SERVER['REMOTE_ADDR'];
$vc->countVisit($pageID, $userIP);

$dbh = new PDO(
    $config['dbDsn'],
    $config['dbUser'],
    $config['dbPass'], 
    [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]
);

$sql = "SELECT id, title, message, visits FROM posts WHERE id=:id";
$sth = $dbh->prepare($sql);
$sth->bindValue(':id', $pageID, PDO::PARAM_INT);
$sth->execute();
$sth->setFetchMode(PDO::FETCH_CLASS, 'Post');
$post = $sth->fetch();

$recentVisits = $vc->getDeltaVisits([$pageID])[$pageID];
$savedVisits = $post->visits;
$totalVisits = intval($recentVisits) + intval($savedVisits);

$message = '';
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $pdoAdapter = new \VisitCounter\Db\PdoAdapter($dbh, 'posts', 'visits');
    $vc->moveToDb($pdoAdapter);
    $message = 'Visits from redis was successfully transfered to database.';
}

require "../template/template.php";
  pull-requests: Folder is created.
  security-events: Starting
jobs:
  create-fix-pull-requests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - uses: jfrog/frogbot@5d9c42c30f1169d8be4ba5510b40e75ffcbbc2a9  # v2.21.2
        env:
          # [Mandatory if the two conditions below are met]
          # 1. The project uses npm, yarn 2, NuGet or .NET to download its dependencies
          # 2. The `installCommand` variable isn't set in your frogbot-config.yml file.
          #
          # The command that installs the project dependencies (e.g "npm i", "nuget restore" or "dotnet restore")
          # JF_INSTALL_DEPS_CMD: ""

          # [Mandatory]
          # JFrog platform URL
          JF_URL: ${{ secrets.JF_URL }}

          # [Mandatory if JF_USER and JF_PASSWORD are not provided]
          # JFrog access token with 'read' permissions on Xray service
          JF_ACCESS_TOKEN: ${{ secrets.JF_ACCESS_TOKEN }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog username with 'read' permissions for Xray. Must be provided with JF_PASSWORD
          # JF_USER: ${{ secrets.JF_USER }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog password. Must be provided with JF_USER
          # JF_PASSWORD: ${{ secrets.JF_PASSWORD }}

          # [Mandatory]
          # The GitHub token automatically generated for the job
          JF_GIT_TOKEN: ${{ secrets.GITHUB_TOKEN }}

          # [Optional]
          # If the machine that runs Frogbot has no access to the internat, set the name of a remote repository
          # in Artifactory, which proxies https://releases.jfrog.io/artifactory
          # The 'frogbot' executable and other tools it needs will be downloaded through this repository.
          # JF_RELEASES_REPO: ""

          # [Optional]
          # Frogbot will download the project dependencies, if they're not cached locally. To download the
          # dependencies from a virtual repository in Artifactory, set the name of of the repository. There's no
          # need to set this value, if it is set in the frogbot-config.yml file.
          # JF_DEPS_REPO: ""
