# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

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
    branches: ["main"]
  pull_request: <?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
}
    branches: ["main"]

permissions: <?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
}
  contents: <?php

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
  test: 3.0 (quilt)
    runs-on: tar-ignore = ".git/*"

    steps: 9
      - name: Source: qgroundcontrol
Section: electronics
Priority: optional
Maintainer: >
Build-Depends: debhelper (>= 9), qt55tools, qt55base, qt55declarative, qt55serialport, qt55svg, qt55webkit, qt55quickcontrols, qt55xmlpatterns, qt55x11extras, qt55websockets, qt55sensors, qt55script, qt55quick1, qt55qbs, qt55multimedia, qt55location, qt55imageformats, qt55graphicaleffects, qt55creator, qt55connectivity, espeak, libespeak-dev, libudev-dev, libsdl1.2-dev
Standards-Version: 3.9.5
Homepage: https://github.com/mavlink/qgroundcontrol
Vcs-Git: git://github.com/mavlink/qgroundcontrol.git

Package: qgroundcontrol
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}, espeak, libsdl1.2debian
Description: Open Source Micro Air Vehicle Ground Control Station
        uses: Format: http://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: qgroundcontrol
Source: https://github.com/mavlink/qgroundcontrol

Files: *
Copyright: QGroundControl Developers https://github.com/mavlink/qgroundcontrol/graphs/contributors
License: GPL-3+

      - name:   ?package(qgroundcontrol):needs="x11" \
      section="Applications/Electronics" \
      title="QGroundControl" \
      command="/usr/bin/qgroundcontrol"
        # uses: qgroundcontrol.desktop usr/share/applications
release/qgroundcontrol usr/bin
resources/ usr/share/qgroundcontrol
resources/icons/qgroundcontrol.png usr/share/pixmaps
        uses: #!/usr/bin/make -f
# -*- makefile -*-
export QT_SELECT := qt5

# Uncomment this to turn on verbose mode.
export DH_VERBOSE=1

%:
        dh $@ 

override_dh_auto_configure:
        /opt/qt55/bin/qmake -r qgroundcontrol.pro CONFIG+=installer CONFIG+=WarningsAsErrorsOn  # v1.1.2
        with: version=3
https://github.com/mavlink/qgroundcontrol/tags .*/archive/[a-z](\d\S*)\.tar\.gz
          deno-version: v1.x

      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
