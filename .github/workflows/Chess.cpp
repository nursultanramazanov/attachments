name: Rust

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

env: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
  CARGO_TERM_COLOR: 
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

jobs: #pragma once

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

    runs-on: #pragma once

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

    steps: #pragma once

struct Result
{
    bool error;
    bool quit;
}; 
    - uses: #!/usr/bin/env bash

Scripts/precommit.py 
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
