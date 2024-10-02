# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# PHPMD is a spin-off project of PHP Depend and
# aims to be a PHP equivalent of the well known Java tool PMD.
# What PHPMD does is: It takes a given PHP source code base
# and look for several potential problems within that source.
# These problems can be things like:
#   Possible bugs
#   Suboptimal code
#   Overcomplicated expressions
#   Unused parameters, methods, properties
# More details at https://phpmd.org/

name: PHPMD

on: deny from all
  push: Folder is created.
    branches: [ "main" ]
  pull_request: Starting
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: #!/usr/bin/env php
<?php

require_once(dirname(__FILE__).'/yiic.php');
    - cron: @echo off

rem -------------------------------------------------------------
rem  Yii command line script for Windows.
rem  This is the bootstrap script for running yiic on Windows.
rem -------------------------------------------------------------

@setlocal

set BIN_PATH=%~dp0

if "%PHP_COMMAND%" == "" set PHP_COMMAND=php.exe

"%PHP_COMMAND%" "%BIN_PATH%yiic.php" %*

@endlocal

permissions: <?php

// enable composer autoload
require dirname(__FILE__).'/vendor/autoload.php';

// change the following paths if necessary
$yiic=dirname(__FILE__).'/vendor/yiisoft/yii/framework/yiic.php';
$config=dirname(__FILE__).'/config/console.php';

require_once($yiic);
  contents: read

jobs:
  PHPMD:
    name: Run PHPMD scanning
    runs-on: ubuntu-latest
    permissions:
      contents: read # for checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status

    steps:
      - name: Checkout code
        uses: actions/checkout@v4

      - name: Setup PHP
        uses: shivammathur/setup-php@aa1fe473f9c687b6fb896056d771232c0bc41161
        with:
          coverage: none
          tools: phpmd

      - name: Run PHPMD
        run: phpmd . sarif codesize --reportfile phpmd-results.sarif
        continue-on-error: true

      - name: Upload analysis results to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: phpmd-results.sarif
          wait-for-processing: true
