name: Rust

on: <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
}
  push: <?php

namespace VisitCounter\Db;

class PdoAdapter implements DbAdapterInterface
{
    private $connection;

    protected $pk;
    protected $tblName;
    protected $colName;

    public function __construct($connection, $tblName, $colName, $pk = 'id')
    {
        $this->connection = $connection;
        $this->tblName = $tblName;
        $this->colName = $colName;
        $this->pk = $pk;
    }

    public function save(array $visitsPages)
    {
        if (!$this->tblName or !$this->colName) {
            $message = "Properties tblName and colName are mandatory.";
            throw new \VisitCounter\Exception\RedisException($message);
        }
        try {
            foreach ($visitsPages as $visitCount => $pages) {
                $pageList = implode(',', $pages);
                $sql = "UPDATE {$this->tblName}
                        SET {$this->colName} = {$this->colName} + $visitCount
                        WHERE {$this->pk} IN ({$pageList})";
                $sth = $this->connection->prepare($sql);
                $sth->execute();
            }
        } catch (\PDOException $e) {
            throw new \VisitCounter\Exception\DbException($e->getMessage(), 0, $e);
        }
    }
}
    branches: [ "main" ]
  pull_request: <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
}
    branches: [ "main" ]

env: <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
}
  CARGO_TERM_COLOR: <?php

namespace VisitCounter\Redis;

interface RedisAdapterInterface
{
    public function setnx($keyName, $expire, $value = '');
    public function rpush($listName, $value);
    public function llen($listName);
    public function lrange($listName, $start = 0, $end = -1);
    public function ltrim($listName, $start, $end = -1);
    public function hincrby($hashName, $field, $count = 1);
    public function hmget($hashName, array $fields);
}

jobs: <?php

namespace VisitCounter\Redis;

class RediskaAdapter implements RedisAdapterInterface
{
    private $client;

    public function __construct(\Rediska $client)
    {
        $this->client = $client;
    }

    public function setnx($keyName, $expire, $value = '')
    {
        $command = new \Rediska_Command_Set(
            $this->client,
            'Set',
            array($keyName, $value, false)
        );
        try {
            if ( !$command->execute() ) return false;
            $key = new \Rediska_Key($keyName);
            $key->expire($expire);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function rpush($listName, $value)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->append($value);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function llen($listName)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $length = $key->getLength();
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $length;
    }

    public function lrange($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $result = $key->getValues($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $result;
    }

    public function ltrim($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->truncate($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hincrby($hashName, $field, $count = 1)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $key->increment($field, $count);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hmget($hashName, array $fields)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $result = $key->get($fields);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return array_combine($fields, $result);
    }
}
  build: <?php

class Post
{
    public $id;
    public $title;
    public $message;
    public $visits;
}

    runs-on: <!DOCTYPE html>
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

    steps: body {
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
    - uses: <?php

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
    - name: {
    "repositories": [
        {
            "type": "vcs",
            "url": "https://github.com/nsdvw/visit-counter"
        }
    ],
    "require": {
        "geometria-lab/rediska": "^0.5.10",
        "nsdvw/visit-counter": "dev-master"
    },
    "autoload": {
        "files": ["app/Post.php"]
    }
}
      run: {
    "_readme": [
        "This file locks the dependencies of your project to a known state",
        "Read more about it at https://getcomposer.org/doc/01-basic-usage.md#composer-lock-the-lock-file",
        "This file is @generated automatically"
    ],
    "hash": "826e2457fc05380510fd1d3db56917a5",
    "content-hash": "42055bc9b6d4d6c6cbfeb2a7c8f74ca5",
    "packages": [
        {
            "name": "geometria-lab/rediska",
            "version": "v0.5.10",
            "source": {
                "type": "git",
                "url": "https://github.com/Shumkov/Rediska.git",
                "reference": "5b33ba2f1526f9ad30864b42617e34ac370aeae0"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/Shumkov/Rediska/zipball/5b33ba2f1526f9ad30864b42617e34ac370aeae0",
                "reference": "5b33ba2f1526f9ad30864b42617e34ac370aeae0",
                "shasum": ""
            },
            "require-dev": {
                "phpunit/phpunit": ">=3.7.11",
                "zendframework/zendframework1": ">=1.12.1"
            },
            "type": "library",
            "autoload": {
                "psr-0": {
                    "Rediska": [
                        "library/",
                        "tests/library/"
                    ]
                }
            },
            "notification-url": "https://packagist.org/downloads/",
            "authors": [
                {
                    
                }
            ],
            "description": "Full-featured PHP client for key-value database Redis",
            "keywords": [
                "cache",
                "redis",
                "sessions",
                "zend"
            ],
            "time": "2014-08-12 15:02:35"
        },
        {
            "name": "nsdvw/visit-counter",
            "version": "dev-master",
            "source": {
                "type": "git",
                "url": "https://github.com/nsdvw/visit-counter.git",
                "reference": "7dc1ff516c1bb0855720a690d693e932ac2139a4"
            },
            "dist": {
                "type": "zip",
                "url": "https://api.github.com/repos/nsdvw/visit-counter/zipball/7dc1ff516c1bb0855720a690d693e932ac2139a4",
                "reference": "7dc1ff516c1bb0855720a690d693e932ac2139a4",
                "shasum": ""
            },
            "type": "library",
            "autoload": {
                "psr-4": {
                    "VisitCounter\\": "./"
                }
            },
            "description": "Simple visitor counter on redis, cross-client and extensible",
            "support": {
                "source": "https://github.com/nsdvw/visit-counter/tree/master",
                "issues": "https://github.com/nsdvw/visit-counter/issues"
            },
            "time": "2015-12-01 23:44:58"
        }
    ],
    "packages-dev": [],
    "aliases": [],
    "minimum-stability": "stable",
    "stability-flags": {
        "nsdvw/visit-counter": 20
    },
    "prefer-stable": false,
    "prefer-lowest": false,
    "platform": [],
    "platform-dev": []
}
    - name: redisHost="127.0.0.1"
redisPort="6379"
dbDsn="mysql:host=127.0.0.1;dbname=visit_counter"
dbUser="root"
dbPass=""
      run: CREATE DATABASE visit_counter;
USE visit_counter;
CREATE TABLE posts (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    title VARCHAR(255) NOT NULL,
    message TEXT NOT NULL,
    visits SMALLINT NOT NULL DEFAULT 0
);

INSERT INTO posts VALUES (
    1,
    'HELLO WORLD',
    'Lorem ipsum dolor sit amet, consectetur adipisicing elit. Ut nesciunt exercitationem eius nobis. Est in magnam velit laborum fugit alias pariatur quo eius tempora quisquam, facere tempore reiciendis distinctio culpa!',
    0
);
