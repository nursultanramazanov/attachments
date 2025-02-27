# This workflow will install Python dependencies, run tests and lint with a variety of Python versions
# For more information see: https://docs.github.com/en/actions/automating-builds-and-tests/building-and-testing-python

name: Python package

on: keyword = "apache"
template = """
parse "* - * [*] \\"* * *\\" * *" as ip, name, timestamp, method, url, protocol, status, contentlength
""" 
  push: keyword = "k8singressnginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" * * [*] [*] * * * * *" as remote_addr, remote_user, timestamp, method, url, protocol, status, body_bytes_sent, http_referer, http_user_agent, request_length, request_time, proxy_upstream_name, proxy_alternative_upstream_name, upstream_addr, upstream_response_length, upstream_response_time, upstream_status, req_id
""" 
    branches: [ "main" ]
  pull_request: keyword = "testmultioperator"
template = """
json | count
""" 
    branches: [ "main" ]

jobs: keyword = "nginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" \\"*\\"" as addr, user, timestamp, method, url, protocol, status, bytes_sent, http_referer, http_user_agent, gzip_ratio
""" 
  build: <?php

use Doctrine\ORM\Tools\Console\ConsoleRunner;

require "../app/bootstrap.php";

return ConsoleRunner::createHelperSet($em); 

    runs-on: <?php

$settings['displayErrorDetails'] = true;

$settings['db'] = [
    'dbname' => 'memtext',
    'user' => 'root',
    'pass' => '',
    'host' => 'localhost',
    'driver' => 'pdo_mysql',
];

$settings['sphinx'] = [
    'host' => '127.0.0.1',
    'port' => 9306,
    'indexName' => 'ix_dictionary',
];

$settings['pager'] = [
    'perPage' => 20,
    'maxLinksCount' => 6,
];

$settings['purifier'] = [
    'AutoFormat.AutoParagraph' => true,
    'AutoFormat.RemoveEmpty' => true,
    'HTML.Doctype' => 'HTML 4.01 Transitional',
    'HTML.AllowedElements' =>
        ['p','h1','h2','h3','h4','h5','h6','br','em','b','i','strong'],
];

$settings['yandex'] = [
    'key' => 'trnsl.1.1.20160330T163001Z.d161a299772702fe.'.
             '0d436c4c1cfc1713dea2aeb9d9e3f2bebae02844',
    'api' => 'https://translate.yandex.net/api/v1.5/tr.json/translate',
]; 
    strategy: source memtext_base
{
    type            = mysql
    sql_host        = localhost
    sql_user        = root
    sql_pass        =
    sql_db          = memtext
    sql_port        = 3306 

    sql_query_pre   = SET NAMES utf8
}

source memtext_dictionary: memtext_base
{
    sql_range_step          = 1000
    sql_query_range         = SELECT MIN(id), MAX(id) FROM dictionary 
    sql_query       = \
        SELECT id, keyword, keyword as word, type \
        FROM dictionary WHERE id BETWEEN $start AND $end

    sql_attr_string = type
    sql_attr_string = word
}

index ix_dictionary
{
    morphology = stem_ru, stem_en
    source = memtext_dictionary
    ### path =
    ### stopwords =
    docinfo = extern
    charset_type = utf-8
}

indexer
{
    mem_limit       = 40M
}

searchd
{
    listen          = 9312
    listen          = 9306:mysql41

    ### log         =
    ### query_log       =

    read_timeout        = 5
    max_children        = 30
    ### pid_file        =
    max_matches     = 10000
    seamless_rotate     = 1
    preopen_indexes     = 1
    unlink_old      = 1
    workers         = threads
    ### binlog_path     = 
    compat_sphinxql_magics = 0
} 
      fail-fast: CREATE TABLE `user`
(
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `login` VARCHAR(100) NOT NULL,
    `email` VARCHAR(255) NOT NULL,
    `salt` VARCHAR(255) NOT NULL,
    `salted_hash` VARCHAR(255) NOT NULL
) ENGINE=INNODB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `dictionary`
(
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `keyword` VARCHAR(255) NOT NULL,
    `definition` TEXT NOT NULL,
    `type` ENUM('short', 'full') NOT NULL DEFAULT 'full',
    KEY (`keyword`)
) ENGINE=INNODB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `user_dictionary`
(
    `user_id` INT UNSIGNED NOT NULL,
    `word_id` INT UNSIGNED NOT NULL,
    PRIMARY KEY (`user_id`, `word_id`),
    FOREIGN KEY (`user_id`) REFERENCES `user` (`id`)
      ON UPDATE CASCADE ON DELETE CASCADE,
    FOREIGN KEY (`word_id`) REFERENCES `dictionary` (`id`)
      ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=INNODB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `text`
(
    `id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `title` VARCHAR(255) NOT NULL,
    `content` TEXT NOT NULL,
    `author_id` INT UNSIGNED NOT NULL,
    FOREIGN KEY (`author_id`) REFERENCES `user`(`id`)
        ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=INNODB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

CREATE TABLE `text_dictionary`
(
    `text_id` INT UNSIGNED NOT NULL,
    `word_id` INT UNSIGNED NOT NULL,
    PRIMARY KEY (`text_id`, `word_id`),
    FOREIGN KEY (`text_id`) REFERENCES `text` (`id`)
        ON UPDATE CASCADE ON DELETE CASCADE,
    FOREIGN KEY (`word_id`) REFERENCES `dictionary` (`id`)
        ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=INNODB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci; 
      matrix: cp vendor/components/jquery/jquery.min.js web/vendor-js/jquery.min.js

cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css web/vendor-css/bootstrap.min.css
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js web/vendor-js/bootstrap.min.js

cp vendor/twbs/bootstrap/dist/fonts/* web/fonts/

cp -r vendor/ckeditor/ckeditor/adapters web/ckeditor/adapters
cp -r vendor/ckeditor/ckeditor/lang web/ckeditor/lang
cp -r vendor/ckeditor/ckeditor/plugins web/ckeditor/plugins
cp -r vendor/ckeditor/ckeditor/skins web/ckeditor/skins
cp vendor/ckeditor/ckeditor/ckeditor.js web/ckeditor/ckeditor.js
cp vendor/ckeditor/ckeditor/config.js web/ckeditor/config.js
cp vendor/ckeditor/ckeditor/contents.css web/ckeditor/contents.css
cp vendor/ckeditor/ckeditor/styles.js web/ckeditor/styles.js 
        python-version: ["3.9", "3.10", "3.11"]

    steps:
    - uses: actions/checkout@v4
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
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
