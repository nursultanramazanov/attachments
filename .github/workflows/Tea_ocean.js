# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: #!/usr/bin/env php
<?php
/**
 * This file is part of the Tea programming language project
 * @copyright         (c)2019 tealang.org
 * For the full copyright and license information, please view the LICENSE file that was distributed with this source code.
 */

/// A script use to generate the loading file for PHP Unit

namespace Tea;

define('TEA_BASE_PATH', dirname(__DIR__) . '/');

const DEBUG = false;
const OPTION_KEYS = ['init'];
const USAGE = 'Usage: genphploader tea/tests/PHPDemoUnit';

require TEA_BASE_PATH . 'compiler/__public.php';

try {
        $opts = process_cli_options($argv, OPTION_KEYS);

        $dir = $opts[0] ?? null;
        if ($dir === null) {
                error("Missed target dir.");
                halt(USAGE);
        }

        $realpath = realpath($dir);
        if ($realpath === false) {
                error("Target dir '{$dir}' not found.");
                halt(USAGE);
        }

        $realpath = FileHelper::normalize_path($realpath . DS);
        $loader_file = $realpath . PUBLIC_LOADER_FILE_NAME;
        if (!file_exists($loader_file)) {
                halt(sprintf("The '%s' not found in '%s'.", PUBLIC_LOADER_FILE_NAME, $realpath));
        }

        $scanner = new PHPUnitScanner();
        $class_map = $scanner->scan($realpath);

        PHPLoaderMaker::generate_loader_file($realpath, $class_map);

        $count = count($class_map);
        halt("$count classes found.");
}
catch (Exception $e) {
        halt("Error: {$e->getMessage()}");
}

// end
  push: #!/usr/bin/env php
<?php
$target = $argv[1] ?? null;
if (empty($target)) {
        echo "\nPlease input a target file path...\n\n";
        exit;
}

$target_file = getcwd() . '/' . $target;
if (!file_exists($target_file)) {
        echo "\nTarget file not found.\n\n";
        exit;
}

// load the builtin libs
require dirname(__DIR__) . '/packages/builtin/__public.php';

// run target
include $target_file;

// end
    branches: ["main"]
  pull_request: #!/usr/bin/env php
<?php
/**
 * This file is part of the Tea programming language project
 * @copyright         (c)2019 tealang.org
 * For the full copyright and license information, please view the LICENSE file that was distributed with this source code.
 */

/// The complier script for the Tea programming language

namespace Tea;

define('TEA_BASE_PATH', dirname(__DIR__) . '/');

require TEA_BASE_PATH . 'compiler/__public.php';

// stats start
UsageTracer::start();

const DEBUG = 0;

const KEY_INIT = 'init';
const OPTION_KEYS = [KEY_INIT];

const USAGE = 'Usage: teac tea/tests/examples
Usage: teac --init myproject/hello';

try {
        $opts = process_cli_options($argv, OPTION_KEYS);

        $target = $opts[0] ?? null;
        if ($target === null) {
                error("Missed target for compile.");
                halt(USAGE);
        }

        // init
        if (isset($opts[KEY_INIT])) {
                $init = new TeaInitializer($target);
                $init->process();
                halt("Init '$target' success.");
        }

        // compile
        $realpath = realpath($target);
        if ($realpath === false) {
                error("Target '{$target}' not found.");
                halt(USAGE);
        }

        if (!is_dir($realpath)) {
                throw new Exception("The target path '{$realpath}' is not a valid Unit directory.");
        }

        echo LF;

        $compiler = new Compiler();
        $count = $compiler->make($realpath);

        echo "\n$count programs compiled.\n\n";
}
catch (\Exception $e) {
        echo $e->getTraceAsString(), LF, LF;
        halt(LF . $e->getMessage());
}

// stats end
echo join("\t", UsageTracer::end()), LF;

// end
    branches: ["main"]

permissions: <?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
}
  contents: <?php

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

jobs: <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
}
  test: <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
}
    runs-on: <?php

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

    steps: <?php

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
      - name: Setup repo
        uses: actions/checkout@v4

      - name: Setup Deno
        # uses: denoland/setup-deno@v1
        uses: denoland/setup-deno@61fe2df320078202e33d7d5ad347e7dcfa0e8f31  # v1.1.2
        with:
          deno-version: v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
