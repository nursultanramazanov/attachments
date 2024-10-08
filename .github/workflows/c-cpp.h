name: C/C++ CI

on: // ---------------
// Basic benchmark
// ---------------

// Benchmark file's have to have <file_name>_test.go and use the Benchmark functions like below.
// The goal is to know what perform better and what allocate more or less between Sprint and Sprintf.

// Our guess is that Sprint is gonna be better because it doesn't have any overhead doing the
// formatting. However, this is not true. Remember we have to optimize for correctness so we don't
// want to guess.

// Run benchmark:
// go test -run none -bench . -benchmem -benchtime 3s

// Sample output:
// BenchmarkSprintBasic-8       50000000                78.7 ns/op             5 B/op          1 allocs/op
// BenchmarkSprintfBasic-8      100000000               60.5 ns/op             5 B/op          1 allocs/op

package main

import (
        "fmt"
        "testing"
)

var gs string

// BenchmarkSprint tests the performance of using Sprint.
// All the code we want to benchmark need to be inside the b.N for loop.
// The first time the tool call it, b.N is equal to 1. It will keep increasing the value of N and
// run long enough based on our bench time.
// fmt.Sprint returns a value and we want to capture this value so it doesn't look like dead code.
// We assign it to the global variable gs.
func BenchmarkSprintBasic(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprint("hello")
        }

        gs = s
}

// BenchmarkSprint tests the performance of using Sprintf.
func BenchmarkSprintfBasic(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprintf("hello")
        }

        gs = s
}
  push: // -------------
// Sub benchmark
// -------------

// Like sub test, we can also do sub benchmark.

// Sample available commands:
// go test -run none -bench . -benchtime 3s -benchmem
// go test -run none -bench BenchmarkSprintSub/none -benchtime 3s -benchmem
// go test -run none -bench BenchmarkSprintSub/format -benchtime 3s -benchmem

package main

import (
        "fmt"
        "testing"
)

var gs string

// BenchmarkSprint tests all the Sprint related benchmarks as sub benchmarks.
func BenchmarkSprintSub(b *testing.B) {
        b.Run("none", benchSprint)
        b.Run("format", benchSprintf)
}

// benchSprint tests the performance of using Sprint.
func benchSprint(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprint("hello")
        }

        gs = s
}

// benchSprintf tests the performance of using Sprintf.
func benchSprintf(b *testing.B) {
        var s string

        for i := 0; i < b.N; i++ {
                s = fmt.Sprintf("hello")
        }

        gs = s
}
    branches: [ "main" ]
  pull_request: //
//  ActivityObserver.h
//  Clock Signal
//
//  Created by 07/05/2018.
//  Copyright 2018.All rights reserved.
//  
//

#pragma once

#include <cstdint>
#include <string>

namespace Activity {

/*!
        Provides a purely virtual base class for anybody that wants to receive notifications of
        'activity': any feedback from an emulated system which a user could perceive other than
        through the machine's native audio and video outputs.

        So: status LEDs, drive activity, etc. A receiver may choose to make appropriate noises
        and/or to show or unshow status indicators.
*/
class Observer {
        public:
                virtual ~Observer() = default;

                /// Provides hints as to the sort of information presented on an LED.
                enum LEDPresentation: uint8_t {
                        /// This LED informs the user of some sort of persistent state, e.g. scroll lock.
                        /// If this flag is absent then the LED describes an ephemeral state, such as media access.
                        Persistent = (1 << 0),
                };

                /// Announces to the receiver that there is an LED of name @c name.
                virtual void register_led([[maybe_unused]] const std::string &name, [[maybe_unused]] uint8_t presentation = 0) {}

                /// Announces to the receiver that there is a drive of name @c name.
                ///
                /// If a drive has the same name as an LED, that LED goes with this drive.
                virtual void register_drive([[maybe_unused]] const std::string &name) {}

                /// Informs the receiver of the new state of the LED with name @c name.
                virtual void set_led_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool lit) {}

                enum class DriveEvent {
                        StepNormal,
                        StepBelowZero,
                        StepBeyondMaximum
                };

                /// Informs the receiver that the named event just occurred for the drive with name @c name.
                virtual void announce_drive_event([[maybe_unused]] const std::string &name, [[maybe_unused]] DriveEvent event) {}

                /// Informs the receiver of the motor-on status of the drive with name @c name.
                virtual void set_drive_motor_status([[maybe_unused]] const std::string &name, [[maybe_unused]] bool is_on) {}
};

}
    branches: [ "main" ]

jobs: //
//  ActivitySource.h
//  Clock Signal
//
//  Created by on 07/05/2018.
//  Copyright 2018. All rights reserved.
//  
//

#pragma once

#include "Observer.hpp"

namespace Activity {

class Source {
        public:
                virtual void set_activity_observer(Observer *observer) = 0;
};

}
  build: 
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

    runs-on: #pragma once

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

    steps: 
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
    - uses: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
};
    - name: 
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
      run: ./#pragma once

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
    - name: 
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
      run: #pragma once

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
    - name: #pragma once

struct Result
{
    bool error;
    bool quit;
};
      run: make check
    - name: make distcheck
      run: make distcheck
