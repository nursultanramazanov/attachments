# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /${
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
} --name ${ #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; --privileged tizenrt/tizenrt:${
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
} /bin/bash
        docker cp ./. ${#pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};/${
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
}
        docker run -d --rm -it --name ${#pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};--volumes-from=${
#include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
} -w /${#pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
}; /os --privileged tizenrt/tizenrt:${#pragma once

struct Result
{
    bool error;
    bool quit;
}; /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${<IfModule mod_authz_core.c>
    Require all denied
</IfModule>
<IfModule !mod_authz_core.c>
    Order deny,allow
    Deny from all
</IfModule> arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${<?php

use Symfony\Bundle\FrameworkBundle\HttpCache\HttpCache;

class AppCache extends HttpCache
{
} bash -c "cd tools; ./configure.sh ${<?php

use Symfony\Component\HttpKernel\Kernel;
use Symfony\Component\Config\Loader\LoaderInterface;

mb_internal_encoding('utf8');

class AppKernel extends Kernel
{
    public function registerBundles()
    {
        $bundles = [
            new Symfony\Bundle\FrameworkBundle\FrameworkBundle(),
            new Symfony\Bundle\SecurityBundle\SecurityBundle(),
            new Symfony\Bundle\TwigBundle\TwigBundle(),
            new Symfony\Bundle\MonologBundle\MonologBundle(),
            new Symfony\Bundle\SwiftmailerBundle\SwiftmailerBundle(),
            new Doctrine\Bundle\DoctrineBundle\DoctrineBundle(),
            new Sensio\Bundle\FrameworkExtraBundle\SensioFrameworkExtraBundle(),
            new TestHubBundle\TestHubBundle(),
            new Symfony\Bundle\AsseticBundle\AsseticBundle(),
            new Doctrine\Bundle\MigrationsBundle\DoctrineMigrationsBundle(),
            new Doctrine\Bundle\FixturesBundle\DoctrineFixturesBundle(),
        ];

        if (in_array($this->getEnvironment(), ['dev', 'test'], true)) {
            $bundles[] = new Symfony\Bundle\DebugBundle\DebugBundle();
            $bundles[] = new Symfony\Bundle\WebProfilerBundle\WebProfilerBundle();
            $bundles[] = new Sensio\Bundle\DistributionBundle\SensioDistributionBundle();
            $bundles[] = new Sensio\Bundle\GeneratorBundle\SensioGeneratorBundle();
        }

        return $bundles;
    }

    public function getRootDir()
    {
        return __DIR__;
    }

    public function getCacheDir()
    {
        return dirname(__DIR__).'/var/cache/'.$this->getEnvironment();
    }

    public function getLogDir()
    {
        return dirname(__DIR__).'/var/logs';
    }

    public function registerContainerConfiguration(LoaderInterface $loader)
    {
        $loader->load($this->getRootDir().'/config/config_'.$this->getEnvironment().'.yml');
    }
}"
        docker exec -it ${ <?php

use Doctrine\Common\Annotations\AnnotationRegistry;
use Composer\Autoload\ClassLoader;

/**
 * @var ClassLoader $loader
 */
$loader = require __DIR__.'/../vendor/autoload.php';

AnnotationRegistry::registerLoader([$loader, 'loadClass']);

return $loader; } bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./

  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code 
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]
  schedule:
    - cron: '22 5 * * 5'

env:
  # Path to the CMake build directory.
  build: '${{ github.workspace }}/build'

permissions:
  contents: read

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
