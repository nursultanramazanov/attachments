# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: ARG base_image
ARG python_version=base

FROM {
  "variant": {
    "label": "Debug",
    "keywordSettings": {
      "buildType": "debug"
    },
    "description": "Emit debug information without performing optimizations"
  },
  "activeEnvironments": []
} AS base

RUN yum install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    bison \
    make \
    gcc \
    gcc-c++ \
    flex \
    autoconf \
    zip \
    git \
    ninja-build

WORKDIR /root

FROM {
    "version": "0.2.0",
    "configurations": [
        
        {
            "name": "C++ Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/dramcoremain",
            "args": ["-c", "./../configs/ddr4_8G_x8_config.ini", "-n", "100000", "--trace-cpu", "--trace-file", "./../sample_trace.txt"],
            "stopAtEntry": true,
            "cwd": "${workspaceRoot}/build",
            "environment": [],
            "externalConsole": true,
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        },
        {
            "name": "C++ Attach",
            "type": "cppdbg",
            "request": "attach",
            "program": "enter program name, for example ${workspaceRoot}/a.out",
            "processId": "${command.pickProcess}",
            "linux": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            },
            "osx": {
                "MIMode": "lldb"
            },
            "windows": {
                "MIMode": "gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": true
                    }
                ]
            }
        }
    ]
}
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade urllib3==1.26.16  # temporary to avoid https://github.com/urllib3/urllib3/issues/2168 (TODO remove when the AL2 image updates to support OpenSSL 1.1.1+)
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"] 
  push: ARG base_image
ARG python_version=base

FROM {
    "files.associations": {
        "array": "cpp",
        "initializer_list": "cpp"
    }
} AS base

RUN dnf install -y \
    boost-devel \
    jemalloc-devel \
    libxml2-devel \
    libxslt-devel \
    bison \
    make \
    gcc \
    gcc-c++ \
    flex \
    autoconf \
    zip \
    git \
    ninja-build

WORKDIR /root

FROM {
    // See https://go.microsoft.com/fwlink/?LinkId=733558
    // for the documentation about the tasks.json format
    "version": "0.1.0",
    "command": "make",
    "isShellCommand": true,
    "args": ["-j5"],
    "showOutput": "always",
    "options": {
        "cwd": "${workspaceRoot}/build"
    }
} 
COPY pyproject.toml poetry.lock ./

RUN pip3 install --upgrade pip wheel
RUN pip3 install --upgrade six cython==3.0.8 cmake hypothesis poetry
RUN poetry config virtualenvs.create false --local && poetry install --no-root --only main

RUN rm -f pyproject.toml poetry.lock

ENTRYPOINT ["/bin/sh"] 
    branches: [ "main" ]
  pull_request: #!/usr/bin/env bash
set -ex

cp ../../pyproject.toml .
cp ../../poetry.lock .

export DOCKER_BUILDKIT=1

PYTHON_VERSION=${1:-ALL}

# Python 3.8
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.8" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py38 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.8 \
    .
fi

# Python 3.9
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.9" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py39 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.9 \
    .
fi

# Python 3.10
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.10" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py310 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.10 \
    .
fi

# Python 3.11
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.11" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py311 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.11 \
    .
fi

# Python 3.12
if [[ $PYTHON_VERSION == "ALL" || $PYTHON_VERSION == "3.12" ]]
then
  docker build \
    --pull \
    --tag awswrangler-build-py312 \
    --build-arg base_image=public.ecr.aws/lambda/python:3.12 \
    --file Dockerfile.al2023 \
    .
fi

rm -rf pyproject.toml poetry.lock 
    branches: [ "main" ]
  schedule: #!/usr/bin/env bash
set -ex

FILENAME="awswrangler-layer-${1}.zip"
NINJA=${2}

pushd /aws-sdk-pandas
rm -rf python dist/pyarrow_files "dist/${FILENAME}" "${FILENAME}"
popd

rm -rf dist arrow

export ARROW_HOME=$(pwd)/dist
export LD_LIBRARY_PATH=$(pwd)/dist/lib:$LD_LIBRARY_PATH
export CMAKE_PREFIX_PATH=$ARROW_HOME:$CMAKE_PREFIX_PATH

git clone \
  --depth 1 \
  --branch apache-arrow-14.0.1 \
  --single-branch \
  https://github.com/apache/arrow.git

mkdir $ARROW_HOME
mkdir arrow/cpp/build
pushd arrow/cpp/build

cmake \
    -DCMAKE_INSTALL_PREFIX=$ARROW_HOME \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DARROW_PYTHON=ON \
    -DARROW_PARQUET=ON \
    -DARROW_DATASET=ON \
    -DARROW_WITH_SNAPPY=ON \
    -DARROW_WITH_ZLIB=ON \
    -DARROW_FLIGHT=OFF \
    -DARROW_GANDIVA=OFF \
    -DARROW_ORC=OFF \
    -DARROW_CSV=ON \
    -DARROW_JSON=ON \
    -DARROW_COMPUTE=ON \
    -DARROW_FILESYSTEM=ON \
    -DARROW_PLASMA=OFF \
    -DARROW_WITH_BZ2=OFF \
    -DARROW_WITH_ZSTD=OFF \
    -DARROW_WITH_LZ4=OFF \
    -DARROW_WITH_BROTLI=OFF \
    -DARROW_BUILD_TESTS=OFF \
    -GNinja \
    ..

eval $NINJA
eval "${NINJA} install"

popd

pushd arrow/python

export CMAKE_PREFIX_PATH=${ARROW_HOME}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}
export ARROW_PRE_0_15_IPC_FORMAT=0
export PYARROW_WITH_HDFS=0
export PYARROW_WITH_FLIGHT=0
export PYARROW_WITH_GANDIVA=0
export PYARROW_WITH_ORC=0
export PYARROW_WITH_CUDA=0
export PYARROW_WITH_PLASMA=0
export PYARROW_WITH_PARQUET=1
export PYARROW_WITH_DATASET=1
export PYARROW_WITH_FILESYSTEM=1
export PYARROW_WITH_CSV=1
export PYARROW_WITH_JSON=1
export PYARROW_WITH_COMPUTE=1

python3 setup.py build_ext \
  --build-type=release \
  --bundle-arrow-cpp \
  bdist_wheel

pip3 install dist/pyarrow-*.whl -t /aws-sdk-pandas/dist/pyarrow_files

popd

pushd /aws-sdk-pandas

pip3 install . -t ./python ".[redshift,mysql,postgres,gremlin,opensearch,openpyxl]"

rm -rf python/pyarrow*
rm -rf python/boto*
rm -rf python/urllib3*
rm -rf python/s3transfer*

cp -r /aws-sdk-pandas/dist/pyarrow_files/pyarrow* python/

# Removing nonessential files
find python -name '*.so' -type f -exec strip "{}" \;
find python -wholename "*/tests/*" -type f -delete
find python -regex '^.*\(__pycache__\|\.py[co]\)$' -delete

zip -r9 "${FILENAME}" ./python
mv "${FILENAME}" dist/

rm -rf python dist/pyarrow_files "${FILENAME}"

popd

rm -rf dist arrow 
    - cron: # Use the front controller as index file. It serves as a fallback solution when
# every other rewrite/redirect fails (e.g. in an aliased environment without
# mod_rewrite). Additionally, this reduces the matching process for the
# start page (path "/") because otherwise Apache will apply the rewriting rules
# to each configured DirectoryIndex file (e.g. index.php, index.html, index.pl).
DirectoryIndex app.php

# By default, Apache does not evaluate symbolic links if you did not enable this
# feature in your server configuration. Uncomment the following line if you
# install assets as symlinks or if you experience problems related to symlinks
# when compiling LESS/Sass/CoffeScript assets.
# Options FollowSymlinks

# Disabling MultiViews prevents unwanted negotiation, e.g. "/app" should not resolve
# to the front controller "/app.php" but be rewritten to "/app.php/app".
<IfModule mod_negotiation.c>
    Options -MultiViews
</IfModule>

<IfModule mod_rewrite.c>
    RewriteEngine On

    # Determine the RewriteBase automatically and set it as environment variable.
    # If you are using Apache aliases to do mass virtual hosting or installed the
    # project in a subdirectory, the base path will be prepended to allow proper
    # resolution of the app.php file and to redirect to the correct URI. It will
    # work in environments without path prefix as well, providing a safe, one-size
    # fits all solution. But as you do not need it in this case, you can comment
    # the following 2 lines to eliminate the overhead.
    RewriteCond %{REQUEST_URI}::$1 ^(/.+)/(.*)::\2$
    RewriteRule ^(.*) - [E=BASE:%1]

    # Sets the HTTP_AUTHORIZATION header removed by Apache
    RewriteCond %{HTTP:Authorization} .
    RewriteRule ^ - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization}]

    # Redirect to URI without front controller to prevent duplicate content
    # (with and without `/app.php`). Only do this redirect on the initial
    # rewrite by Apache and not on subsequent cycles. Otherwise we would get an
    # endless redirect loop (request -> rewrite to front controller ->
    # redirect -> request -> ...).
    # So in case you get a "too many redirects" error or you always get redirected
    # to the start page because your Apache does not expose the REDIRECT_STATUS
    # environment variable, you have 2 choices:
    # - disable this feature by commenting the following 2 lines or
    # - use Apache >= 2.3.9 and replace all L flags by END flags and remove the
    #   following RewriteCond (best solution)
    RewriteCond %{ENV:REDIRECT_STATUS} ^$
    RewriteRule ^app\.php(?:/(.*)|$) %{ENV:BASE}/$1 [R=301,L]

    # If the requested filename exists, simply serve it.
    # We only want to let Apache serve files and not directories.
    RewriteCond %{REQUEST_FILENAME} -f
    RewriteRule ^ - [L]

    # Rewrite all other queries to the front controller.
    RewriteRule ^ %{ENV:BASE}/app.php [L]
</IfModule>

<IfModule !mod_rewrite.c>
    <IfModule mod_alias.c>
        # When mod_rewrite is not available, we instruct a temporary redirect of
        # the start page to the front controller explicitly so that the website
        # and the generated links can still be used.
        RedirectMatch 302 ^/$ /app.php/
        # RedirectTemp cannot be used instead
    </IfModule>
</IfModule> 

env: <?php

use Symfony\Component\HttpFoundation\Request;

/**
 * @var Composer\Autoload\ClassLoader
 */
$loader = require __DIR__.'/../app/autoload.php';
include_once __DIR__.'/../var/bootstrap.php.cache';

// Enable APC for autoloading to improve performance.
// You should change the ApcClassLoader first argument to a unique prefix
// in order to prevent cache key conflicts with other applications
// also using APC.
/*
$apcLoader = new Symfony\Component\ClassLoader\ApcClassLoader(sha1(__FILE__), $loader);
$loader->unregister();
$apcLoader->register(true);
*/

$kernel = new AppKernel('prod', false);
$kernel->loadClassCache();
//$kernel = new AppCache($kernel);

// When using the HttpCache, you need to call the method in your front controller instead of relying on the configuration parameter
//Request::enableHttpMethodParameterOverride();
$request = Request::createFromGlobals();
$response = $kernel->handle($request);
$response->send();
$kernel->terminate($request, $response); 
  # Path to the CMake build directory.
  build: <?php

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Debug\Debug;

// If you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#checking-symfony-application-configuration-and-setup
// for more information
//umask(0000);

// This check prevents access to debug front controllers that are deployed by accident to production servers.
// Feel free to remove this, extend it, or make something more sophisticated.
if (isset($_SERVER['HTTP_CLIENT_IP'])
    || isset($_SERVER['HTTP_X_FORWARDED_FOR'])
    || !(in_array(@$_SERVER['REMOTE_ADDR'], ['127.0.0.1', 'fe80::1', '::1']) || php_sapi_name() === 'cli-server')
) {
    header('HTTP/1.0 403 Forbidden');
    exit('You are not allowed to access this file. Check '.basename(__FILE__).' for more information.');
}

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';
Debug::enable();

$kernel = new AppKernel('dev', true);
$kernel->loadClassCache();
$request = Request::createFromGlobals();
$response = $kernel->handle($request);
$response->send();
$kernel->terminate($request, $response); 

permissions: <?php

/*
 * ************** CAUTION **************
 *
 * DO NOT EDIT THIS FILE as it will be overridden by Composer as part of
 * the installation/update process. The original file resides in the
 * SensioDistributionBundle.
 *
 * ************** CAUTION **************
 */

if (!isset($_SERVER['HTTP_HOST'])) {
    exit('This script cannot be run from the CLI. Run it from a browser.');
}

if (!in_array(@$_SERVER['REMOTE_ADDR'], array(
    '127.0.0.1',
    '::1',
))) {
    header('HTTP/1.0 403 Forbidden');
    exit('This script is only accessible from localhost.');
}

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$symfonyRequirements = new SymfonyRequirements();

$majorProblems = $symfonyRequirements->getFailedRequirements();
$minorProblems = $symfonyRequirements->getFailedRecommendations();

?>
<!DOCTYPE html>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
        <meta name="robots" content="noindex,nofollow" />
        <title>Symfony Configuration Checker</title>
        <link rel="stylesheet" href="bundles/framework/css/structure.css" media="all" />
        <link rel="stylesheet" href="bundles/framework/css/body.css" media="all" />
        <style type="text/css">
            /* styles copied from bundles/sensiodistribution/webconfigurator/css/install.css */
            body {
                font-size: 14px;
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
            }
            .sf-reset h1.title {
                font-size: 45px;
                padding-bottom: 30px;
            }
            .sf-reset h2 {
                font-weight: bold;
                color: #FFFFFF;
                /* Font is reset to sans-serif (like body) */
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
                margin-bottom: 10px;
                background-color: #aacd4e;
                padding: 2px 4px;
                display: inline-block;
                text-transform: uppercase;
            }
            .sf-reset ul a,
            .sf-reset ul a:hover {
                background: url(../images/blue-arrow.png) no-repeat right 6px;
                padding-right: 10px;
            }
            .sf-reset ul, ol {
                padding-left: 20px;
            }
            .sf-reset li {
                padding-bottom: 18px;
            }
            .sf-reset ol li {
                list-style-type: decimal;
            }
            .sf-reset ul li {
                list-style-type: none;
            }
            .sf-reset .symfony-blocks-install {
                overflow: hidden;
            }
            .sf-reset .symfony-install-continue {
                font-size: 0.95em;
                padding-left: 0;
            }
            .sf-reset .symfony-install-continue li {
                padding-bottom: 10px;
            }
            .sf-reset .ok {
                color: #fff;
                font-family: "Lucida Sans Unicode", "Lucida Grande", Verdana, Arial, Helvetica, sans-serif;
                background-color: #6d6;
                padding: 10px;
                margin-bottom: 20px;
            }
            .sf-reset .ko {
                background-color: #d66;
            }
            .version {
                text-align: right;
                font-size: 10px;
                margin-right: 20px;
            }
            .sf-reset a,
            .sf-reset li a {
                color: #08C;
                text-decoration: none;
            }
            .sf-reset a:hover,
            .sf-reset li a:hover {
                color: #08C;
                text-decoration: underline;
            }
            .sf-reset textarea {
                padding: 7px;
            }
        </style>
    </head>
    <body>
        <div id="content">
            <div class="header clear-fix">
                <div class="header-logo">
                    <img src="bundles/framework/images/logo_symfony.png" alt="Symfony" />
                </div>

                <div class="search">
                  <form method="get" action="http://symfony.com/search">
                    <div class="form-row">

                      <label for="search-id">
                          <img src="bundles/framework/images/grey_magnifier.png" alt="Search on Symfony website" />
                      </label>

                      <input name="q" id="search-id" type="search" placeholder="Search on Symfony website" />

                      <button type="submit" class="sf-button">
                          <span class="border-l">
                            <span class="border-r">
                                <span class="btn-bg">OK</span>
                            </span>
                        </span>
                      </button>
                    </div>
                   </form>
                </div>
            </div>

            <div class="sf-reset">
                <div class="block">
                    <div class="symfony-block-content">
                        <h1 class="title">Configuration Checker</h1>
                        <p>
                            This script analyzes your system to check whether is
                            ready to run Symfony applications.
                        </p>

                        <?php if (count($majorProblems)): ?>
                            <h2 class="ko">Major problems</h2>
                            <p>Major problems have been detected and <strong>must</strong> be fixed before continuing:</p>
                            <ol>
                                <?php foreach ($majorProblems as $problem): ?>
                                    <li><?php echo $problem->getHelpHtml() ?></li>
                                <?php endforeach; ?>
                            </ol>
                        <?php endif; ?>

                        <?php if (count($minorProblems)): ?>
                            <h2>Recommendations</h2>
                            <p>
                                <?php if (count($majorProblems)): ?>Additionally, to<?php else: ?>To<?php endif; ?> enhance your Symfony experience,
                                it’s recommended that you fix the following:
                            </p>
                            <ol>
                                <?php foreach ($minorProblems as $problem): ?>
                                    <li><?php echo $problem->getHelpHtml() ?></li>
                                <?php endforeach; ?>
                            </ol>
                        <?php endif; ?>

                        <?php if ($symfonyRequirements->hasPhpIniConfigIssue()): ?>
                            <p id="phpini">*
                                <?php if ($symfonyRequirements->getPhpIniConfigPath()): ?>
                                    Changes to the <strong>php.ini</strong> file must be done in "<strong><?php echo $symfonyRequirements->getPhpIniConfigPath() ?></strong>".
                                <?php else: ?>
                                    To change settings, create a "<strong>php.ini</strong>".
                                <?php endif; ?>
                            </p>
                        <?php endif; ?>

                        <?php if (!count($majorProblems) && !count($minorProblems)): ?>
                            <p class="ok">All checks passed successfully. Your system is ready to run Symfony applications.</p>
                        <?php endif; ?>

                        <ul class="symfony-install-continue">
                            <?php if (count($majorProblems) || count($minorProblems)): ?>
                                <li><a href="config.php">Re-check configuration</a></li>
                            <?php endif; ?>
                        </ul>
                    </div>
                </div>
            </div>
            <div class="version">Symfony Standard Edition</div>
        </div>
    </body>
</html> 
  contents: # www.robotstxt.org/
# www.google.com/support/webmasters/bin/answer.py?hl=en&answer=156449

User-agent: *
Disallow: 

jobs:
  analyze:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ env.build }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v4
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
 
