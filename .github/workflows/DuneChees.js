# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: 
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
}
  push: #pragma once

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
};
    branches: [ "main" ]
  pull_request: 
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
}
    branches: [ "main" ]
  schedule: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
    - cron: 
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

env: #pragma once

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
};
  # Path to the CMake build directory.
  build: 
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
}

permissions: #pragma once

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
};
  contents: #pragma once

struct Result
{
    bool error;
    bool quit;
};

jobs: <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8"/>
        <title>DuneNvme test system Release Notes</title>
        <meta name="generator" content="LibreOffice 6.3.6.2 (Linux)"/>
        <meta name="created" content="00:00:00"/>
        <meta name="changedby" content="Terry Barnaby"/>
        <meta name="changed" content="2020-07-18T07:19:54.309394236"/>
        <meta name="created" content="00:00:00">
        <style type="text/css">
                h3.cjk { font-family: "Noto Sans CJK SC" }
                h3.ctl { font-family: "Lohit Devanagari" }
        </style>
</head>
<body lang="en-GB" dir="ltr"><h1 align="center">DuneNvme test system
Release Notes</h1>
<h3 class="western">Release 1.0.1: <font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>2020-0</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>7</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>-1</b></font></font><font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>8</b></font></font></h3>
<p>Minor bugfix release to aid debugging.</p>
<ol>
        <li><p>The bfpga_driver’s Makefile now finds the correct PCI
        device when loading the bfpga Linux driver.</p>
        <li><p>The bfpga driver’s udev file, bfpga.rules, now has the
        correct device name.</p>
        <li><p>Fix to the test code for correct bfpga device name string.</p>
        <li><p>Added NVMe PCIe phy_status and link_status signals to the
        NvmeStorageUnit’s status register (bits 30 and 31).</p>
</ol>
<h3 class="western">Release 1.0.0: <font face="Liberation Serif, serif"><font size="4" style="font-size: 14pt"><b>2020-06-16</b></font></font></h3>
<p>First release of Dune NvmeStorage system. This includes builds for
the AB17-M2FMC and Opsero OP47 NVMe boards.</p>
<p>NvmeStorage Utilisation:</p>
<table width="580" cellpadding="0" cellspacing="0" style="page-break-inside: avoid">
        <col width="78"/>

        <col width="79"/>

        <col width="75"/>

        <col width="37"/>

        <col width="40"/>

        <col width="64"/>

        <col width="64"/>

        <col width="49"/>

        <col width="95"/>

        <tr valign="top">
                <th width="78" style="border: none; padding: 0cm"><p>Total LUTs</p>
                </th>
                <th width="79" style="border: none; padding: 0cm"><p>Logic LUTs</p>
                </th>
                <th width="75" style="border: none; padding: 0cm"><p>LUTRAMs</p>
                </th>
                <th width="37" style="border: none; padding: 0cm"><p>SRLs</p>
                </th>
                <th width="40" style="border: none; padding: 0cm"><p>FFs</p>
                </th>
                <th width="64" style="border: none; padding: 0cm"><p>RAMB36</p>
                </th>
                <th width="64" style="border: none; padding: 0cm"><p>RAMB18</p>
                </th>
                <th width="49" style="border: none; padding: 0cm"><p>URAM</p>
                </th>
                <th width="95" style="border: none; padding: 0cm"><p>DSP48 Blocks</p>
                </th>
        </tr>
        <tr valign="top">
                <td width="78" style="border: none; padding: 0cm" sdval="10625" sdnum="2057;"><p>
                        10625</p>
                </td>
                <td width="79" style="border: none; padding: 0cm" sdval="8729" sdnum="2057;"><p>
                        8729</p>
                </td>
                <td width="75" style="border: none; padding: 0cm" sdval="1888" sdnum="2057;"><p>
                        1888</p>
                </td>
                <td width="37" style="border: none; padding: 0cm" sdval="8" sdnum="2057;"><p>
                        8</p>
                </td>
                <td width="40" style="border: none; padding: 0cm" sdval="13773" sdnum="2057;"><p>
                        13773</p>
                </td>
                <td width="64" style="border: none; padding: 0cm" sdval="24" sdnum="2057;"><p>
                        24</p>
                </td>
                <td width="64" style="border: none; padding: 0cm" sdval="26" sdnum="2057;"><p>
                        26</p>
                </td>
                <td width="49" style="border: none; padding: 0cm" sdval="0" sdnum="2057;"><p>
                        0</p>
                </td>
                <td width="95" style="border: none; padding: 0cm" sdval="0" sdnum="2057;"><p>
                        0</p>
                </td>
        </tr>
</table>
<p><br/>
<br/>

</p>
</body>
</html>
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
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
