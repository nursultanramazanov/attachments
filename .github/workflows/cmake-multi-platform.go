# This starter workflow is for a CMake project running on multiple platforms. There is a different starter workflow if you just want a single platform.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-single-platform.yml
name: CMake on multiple platforms

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
  pull_request: // ------------------
// Language Mechanics
// ------------------

// Channels are for orchestration. They allow us to have 2 Goroutines participate in some sort of
// workflow and give us the ability to orchestrate in a predictable way.

// The one thing that we really need to think about is not that a channel is a queue, even though
// it seems it be implemented like queue, first in first out. We will have a difficult time if we
// think that way. What we want to think about instead is a channel as a way of signaling events to
// another Goroutine. What is nice here is that we can signal an event with data or without data.
// If everything we do have signaling in mind, we are going to use channel in a proper way.

// Go has 2 types of channels: unbuffered and buffered. They both allow us to signal with data.
// The big difference is that, when we use unbuffered channel, we are signaling and getting a
// guarantee the signal was received. We are not gonna be sure if that Goroutine is done whatever
// work we assign it to do but we do have the guarantee. The trade off for the guarantee that the
// signal was received is higher latency because we have to wait to make sure that the Goroutine on
// the other side of that unbuffered channel receive the data.

// This is how the unbuffered channel going to work.
// There is a Goroutine coming to the channel. The Goroutine wants to signal with some piece of
// data. It is gonna put the data right there in the channel. However, the data is locked in and
// cannot move because channel has to know if there is another Goroutine is on the other side to
// receive it. Eventually a Goroutine come and say that it want to receive the data. Both of
// Goroutines are not putting their hands in the channel. The data now can be transferred.
// Here is the key to why that unbuffered channel gives us that guarantee: the receive happens
// first. When the receive happens, we know that the data transfer has occurred and we can walk
// away.

//  G                      G
//  |        Channel       |
//  |      ----------      |
//  |     |   D  D   |     |
//  |-----|--->  <---|-----|
//  |     |          |     |
//  |      ----------      |
//  |                      |

// The unbuffered channel is a very powerful channel. We want to leverage that guarantee as much as
// possible. But again, the cost of the guarantee is higher latency because we have to wait for
// this.

// The buffered channel is a bit different: we do not get the guarantee but we get to reduce the
// amount of latencies on any given send or receive.

// Back to the previous example, we replace the unbuffered channel with a buffered channel. We are
// having a buffered channel of just 1 in size. It means there is a space in this channel for 1 piece of
// data that we are using the signal and we don't have to wait for the other side to get it. So
// now a Goroutine comes in, put the data in and then move away immediately. In other word, the
// sending is happening before the receiving. All the sending Goroutine know is that it issues the
// signal, put that data but has no clue when the signal is going to be received. Now hopefully a
// Goroutine comes in. It sees that there is a data there, receive it and move on.

//  G                      G
//  |      Channel (1)     |
//  |      ----------      |
//  |---->|    D     |<----|
//  |      ----------      |
//  |                      |

// We use a buffered channel of 1 when dealing with these type of latency. We may need buffers that are
// larger but there are some design rules that we are gonna learn later on we use buffers that are
// greater than 1. But if we are in a situation where we can have these sends coming in and they
// could potentially be locked then we have to think again: if the channel of 1 is fast enough to
// reduce the latency that we are dealing with. Because what's gonna happen is the following:
// What we are hoping is, the buffered channel is always empty every time we perform a send.

// Buffered channels are not for performance. What the buffered channel need to be used for is
// continuity, to keep the wheel moving. One thing we have to understand is that, everybody can
// write a piece of software that works when everything is going well. When things are going
// bad, it's where the architecture and engineer really come in. Our software doesn't enclose and
// it doesn't cost stress. We need to be responsible.

// Back to the example, it's not important that we know exactly the signaling data was received but
// we do have to make sure that it was. The buffered channel of 1 gives us almost guarantee because
// what happen is: it performs a send, puts the data in there, turns around and when it comes back,
// it sees that the buffered is empty. Now we know  that it was received. We don't know immediately
// at the time that we sent but by using a buffer of 1, we do know that is empty when we come back.
// Then it is okay to put another piece of data in there and hopefully when we come back again, it
// is gone. If it's not gone, we have a problem. There is a problem upstream. We cannot move
// forward until the channel is empty. This is something that we want to report immediately because
// we want to know why the data is still there. That's how we can build systems that are reliable.
// We don't take more work at any give time. We identify upstream when there is a problem so we
// don't put more stress on our systems. We don't take more responsibilities for things that we
// shouldn't be.

package main

import (
        "fmt"
        "time"
)

func main() {
        fmt.Printf("\n=> Basics of a send and receive\n")
        basicSendRecv()

        fmt.Printf("\n=> Close a channel to signal an event\n")
        signalClose()
}

// ---------------------------------------
// Unbuffered channel: Signaling with data
// ---------------------------------------

// basicSendRecv shows the basics of a send and receive.
// We are using make function to create a channel. We have no other way of creating a channel that
// is usable until we use make.
// Channel is also based on type, a type of data that we are gonna do the signaling. In this case,
// we use string. That channel is a reference type. ch is just a pointer variable to larger data
// structure underneath.
func basicSendRecv() {
        // This is an unbuffered channel.
        ch := make(chan string)

        go func() {
                // This is a send: a binary operation with the arrow pointing into the channel.
                // We are signaling with a string "hello".
                ch <- "hello"
        }()

        // This is a receive: also an arrow but it is a unary operation where it is attached to the
        // left hand side of the channel to show that is coming out.
        // We are now have an unbuffered channel where the send and receive have to come together. We
        // also know that the signal has been received because the receive happens first.
        // Both are gonna block until both come together so the exchange can happen.
        fmt.Println(<-ch)
}

// ------------------------------------------
// Unbuffered channel: Signaling without data
// ------------------------------------------

// signalClose shows how to close a channel to signal an event.
func signalClose() {
        // We are making a channel using an empty struct. This is a signal without data.
        ch := make(chan struct{})

        // We are gonna launch a Goroutine to do some work. Suppose that it's gonna take 100
        // millisecond. Then, it wants to signal another Goroutine that it's done.
        // It's gonna close the channel to report that it's done without the need of data.

        // When we create a channel, buffered or unbuffered, that channel can be in 2 different states.
        // All channels start out in open state so we can send and receive data. When we change the
        // state to be closed, it cannot be opened. We also cannot close the channel twice because that
        // is an integrity issue. We cannot signal twice without data twice.

        go func() {
                time.Sleep(100 * time.Millisecond)
                fmt.Println("signal event")
                close(ch)
        }()

        // When the channel is closed, the receive will immediately return.
        // When we receive on a channel that is open, we cannot return until we receive the data
        // signal. But if we receive on a channel that is closed, we are able to receive the signal
        // without data. We know that event is occurred. Every receive on that channel will immediately
        // return.
        <-ch

        fmt.Println("event received")
}
    branches: [ "main" ]

jobs: // ------------------
// Language Mechanics
// ------------------

package main

import (
        "fmt"
        "math/rand"
        "time"
)

func main() {
        fmt.Printf("\n=> Double signal\n")
        signalAck()

        fmt.Printf("\n=> Select and receive\n")
        selectRecv()

        fmt.Printf("\n=> Select and send\n")
        selectSend()

        fmt.Printf("\n=> Select and drop\n")
        selectDrop()
}

// ---------------------------------
// Unbuffered channel: Double signal
// ---------------------------------

// signalAck shows how to signal an event and wait for an acknowledgement it is done
// It does not only want to guarantee that a signal is received but also want to know when that
// work is done. This is gonna like a double signal.
func signalAck() {
        ch := make(chan string)

        go func() {
                fmt.Println(<-ch)
                ch <- "ok done"
        }()

        // It blocks on the receive. This Goroutine can no longer move on until we receive a signal.
        ch <- "do this"
        fmt.Println(<-ch)
}

// ---------------------------------
// Buffered channel: Close and range
// ---------------------------------

// closeRange shows how to use range to receive value and using close to terminate the loop.
func closeRange() {
        // This is a buffered channel of 5.
        ch := make(chan int, 5)

        // Populate with value
        for i := 0; i < 5; i++ {
                ch <- i
        }

        // Close the channel.
        close(ch)

        // Every iteration on the range is a receive.
        // When the range notices that the channel is closed, the loop will terminate.
        for v := range ch {
                fmt.Println(v)
        }
}

// --------------------------------------
// Unbuffered channel: select and receive
// --------------------------------------

// Select allows a Goroutine to work with multiple channel at a time, including send and receive.
// This can be great when creating an event loop but not good for serializing shared state.

// selectRecv shows how to use the select statement to wait for a specific amount of time to
// receive a value.
func selectRecv() {
        ch := make(chan string)

        // Wait for some amount of time and perform a send.
        go func() {
                time.Sleep(time.Duration(rand.Intn(200)) * time.Millisecond)
                ch <- "work"
        }()

        // Perform 2 different receives on 2 different channels: one above and one for time.
        // time.After returns a channel that will send the current time after that duration.
        // We want to receive the signal from the work sent but we are not willing to wait forever. We
        // only wait 100 milliseconds then we will move on.
        select {
        case v := <-ch:
                fmt.Println(v)
        case <-time.After(100 * time.Millisecond):
                fmt.Println("timed out")
        }

        // However, there is a very common bug in this code.
        // One of the biggest bug we are going to have and potential memory is when we write code like
        // this and we don't give the Goroutine an opportunity to terminate.
        // We are using an unbuffered channel and this Goroutine at some point, its duration will
        // finish and it will want to perform a send. But this is an unbuffered channel. This send
        // cannot be completed unless there is a corresponding receive. What if this Goroutine times
        // out and moves on? There is no more corresponding receive. Therefore, we will have a Goroutine
        // leak, which means it will never be terminated.

        // The cleanest way to fix this bug is to use the buffered channel of 1. If this send happens,
        // we don't necessarily have the guarantee. We don't need it. We just need to perform the
        // signal then we can walk away. Therefore, either we get the signal on the other side or we
        // walk away. Even if we walk away, this send can still be completed because there is room in
        // the buffer for that send to happen.
}

// -----------------------------------
// Unbuffered channel: select and send
// -----------------------------------

// selectSend shows how to use the select statement to attempt a send on a channel for a specific
// amount of time.
func selectSend() {
        ch := make(chan string)

        go func() {
                time.Sleep(time.Duration(rand.Intn(200)) * time.Millisecond)
                fmt.Println(<-ch)
        }()

        select {
        case ch <- "work":
                fmt.Println("send work")
        case <-time.After(100 * time.Millisecond):
                fmt.Println("timed out")
        }

        // Similar to the above function, Goroutine leak will occur.
        // Once again, a buffered channel of 1 will save us here.
}

// ---------------------------------
// Buffered channel: Select and drop
// ---------------------------------

// selectDrop shows how to use the select to walk away from a channel operation if it will
// immediately block.
// This is a really important pattern. Imagine a situation where our service is flushed with work
// to do or work is gonna coming. Something upstream is not functioning properly. We can't just
// back up the work. We have to throw it away so we can keep moving on.
// A Denial-of-service attack is a great example. We get a bunch of requests coming to our server.
// If we try to handle every single request, we are gonna implode. We have to handle what we can
// and drop other requests.
// Using this type of pattern (fanout), we are willing to drop some data. We can use buffer that
// are larger than 1. We have to measure what the buffer should be. It cannot be random.
func selectDrop() {
        ch := make(chan int, 5)

        go func() {
                // We are in the receive loop waiting for data to work on.
                for v := range ch {
                        fmt.Println("recv", v)
                }
        }()

        // This will send the work to the channel.
        // If the buffer fills up, which means it blocks, the default case comes in and drop things.
        for i := 0; i < 20; i++ {
                select {
                case ch <- i:
                        fmt.Println("send work", i)
                default:
                        fmt.Println("drop", i)
                }
        }

        close(ch)
}
  build: // ---------------------------------
// Unbuffered channel (Tennis match)
// ---------------------------------

// This program will put 2 Goroutines in a tennis match.
// We use an unbuffered channel because we need to guarantee that the ball is hit on both side or
// missed.

package main

import (
        "fmt"
        "math/rand"
        "sync"
        "time"
)

func init() {
        rand.Seed(time.Now().UnixNano())
}

func main() {
        // Create an unbuffered channel.
        court := make(chan int)

        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(2)

        // Launch two players.
        // Both are gonna start out in a receive mode. We are not really sure who is gonna get the ball
        // first. Imagine the main Goroutine is the judge. It depends on the judge to choose.
        go func() {
                player("Hoanh", court)
                wg.Done()
        }()

        go func() {
                player("Andrew", court)
                wg.Done()
        }()

        // Start the set.
        // The main Goroutine here is performing a send. Since both players are in receive mode, we
        // cannot predict which one will go first.
        court <- 1

        // Wait for the game to finish.
        wg.Wait()
}

// player simulates a person playing the game of tennis.
// We are asking for a channel value using value semantic.
func player(name string, court chan int) {
        for {
                // Wait for the ball to be hit back to us.
                // Notice that this is another form of receive. Instead of getting just the value, we can
                // get a flag indicating how the receive is returned. If the signal happens because of the
                // data, ok will be true. If the signal happens without data, in other word, the channel is
                // closed, ok will be false. In this case, we are gonna use that to determine who won.
                ball, ok := <-court
                if !ok {
                        // If the channel was closed we won.
                        fmt.Printf("Player %s Won\n", name)
                        return
                }

                // Pick a random number and see if we miss the ball (or we lose).
                // If we lose the game, we are gonna close the channel. It then causes the other player to
                // know that he is receiving the signal but without data. The channel is closed so he won.
                // They both return.
                n := rand.Intn(100)
                if n%13 == 0 {
                        fmt.Printf("Player %s Missed\n", name)

                        // Close the channel to signal we lost.
                        close(court)
                        return
                }

                // Display and then increment the hit count by one.
                // If the 2 cases above doesn't happen, we still have the ball. Increase the value of the
                // ball by one and perform a send. We know that the other player is still in receive mode,
                // therefore, the send and receive will eventually come together.
                // Again, in an unbuffered channel, the receive happens first because it gives us the
                // guarantee.
                fmt.Printf("Player %s Hit %d\n", name, ball)
                ball++

                // Hit the ball back to the opposing player.
                court <- ball
        }
}
    runs-on: ${{ // --------------------------------
// Unbuffered channel (Replay race)
// --------------------------------

// The program shows how to use an unbuffered channel to simulate a relay race between four Goroutines.
// Imagine we have 4 runners that are on the track. Only 1 can run at a time. We have the second
// runner on the track until the last one. The second one wait to be exchanged.

package main

import (
        "fmt"
        "sync"
        "time"
)

// wg is used to wait for the program to finish.
var wg sync.WaitGroup

func main() {
        // Create an unbuffered channel.
        track := make(chan int)

        // Add a count of one for the last runner.
        // We only add one because all we care about is the last runner in the race telling us that he
        // is done.
        wg.Add(1)

        // Create a first runner to his mark.
        go Runner(track)

        // The main Goroutine start the race (shoot the gun).
        // At this moment, we know that on the other side, a Goroutine is performing a receive.
        track <- 1

        // Wait for the race to finish.
        wg.Wait()
}

// Runner simulates a person running in the relay race.
// This Runner doesn't have a loop because it's gonna do everything from the beginning to end and
// then terminate. We are gonna keep adding Goroutines (Runners) in order to make this pattern
// work.
func Runner(track chan int) {
        // The number of exchanges of the baton.
        const maxExchanges = 4

        var exchange int

        // Wait to receive the baton with data.
        baton := <-track

        // Start running around the track.
        fmt.Printf("Runner %d Running With Baton\n", baton)

        // New runner to the line. Are we the last runner on the race?
        // If not, we increment the data by 1 to keep track which runner we are on.
        // We will create another Goroutine. It will go immediately into a receive. We are now having a
        // second Groutine on the track, in the receive waiting for the baton. (1)
        if baton < maxExchanges {
                exchange = baton + 1
                fmt.Printf("Runner %d To The Line\n", exchange)
                go Runner(track)
        }

        // Running around the track.
        time.Sleep(100 * time.Millisecond)

        // Is the race over.
        if baton == maxExchanges {
                fmt.Printf("Runner %d Finished, Race Over\n", baton)
                wg.Done()
                return
        }

        // Exchange the baton for the next runner.
        fmt.Printf("Runner %d Exchange With Runner %d\n", baton, exchange)

        // Since we are not the last runner, perform a send so (1) can receive it.
        track <- exchange
} }}

    strategy: // -------------------------
// Buffered channel: Fan Out
// -------------------------

// This is a classic use of a buffered channel that is greater than 1.
// It is called a Fan Out Pattern.

// Idea: A Goroutine is doing its thing and decides to run a bunch of database operation. It is
// gonna create a bunch of Gouroutines, say 10, to do that. Each Goroutine will perform 2 database
// operations. We end up having 20 database operations across 10 Goroutines. In other word, the
// original Goroutine will fan 10 Goroutines out, wait for them all to report back.

// The buffered channel is fantastic here because we know ahead of time that there are 10
// Goroutines performing 20 operations, so the size of the buffer is 20. There is no reason for any
// of these operation signal to block because we know that we have to receive this at the end of
// the day.

package main

import (
        "fmt"
        "log"
        "math/rand"
        "time"
)

// result is what is sent back from each operation.
type result struct {
        id  int
        op  string
        err error
}

func init() {
        rand.Seed(time.Now().UnixNano())
}

func main() {
        // Set the number of Goroutines and insert operations.
        const routines = 10
        const inserts = routines * 2

        // Buffered channel to receive information about any possible insert.
        ch := make(chan result, inserts)

        // Number of responses we need to handle.
        // Instead of using a WaitGroup, since this Goroutine can maintain its stack space, we are
        // gonna use a local variable as our WaitGroup. We will decrement that as we go.
        // Therefore, we set it to 20 inserts right out the box.
        waitInserts := inserts

        // Perform all the inserts. This is the fan out.
        // We are gonna have 10 Goroutines. Each Goroutine performs 2 inserts. The result of the insert
        // is used in a ch channel. Because this is a buffered channel, none of these send blocks.
        for i := 0; i < routines; i++ {
                go func(id int) {
                        ch <- insertUser(id)

                        // We don't need to wait to start the second insert thanks to the buffered channel.
                        // The first send will happen immediately.
                        ch <- insertTrans(id)
                }(i)
        }

        // Process the insert results as they complete.
        for waitInserts > 0 {
                // Wait for a response from a Goroutine.
                // This is a receive. We are receiving one result at a time and decrement the waitInserts
                // until it gets down to 0.
                r := <-ch

                // Display the result.
                log.Printf("N: %d ID: %d OP: %s ERR: %v", waitInserts, r.id, r.op, r.err)

                // Decrement the wait count and determine if we are done.
                waitInserts--
        }

        log.Println("Inserts Complete")
}

// insertUser simulates a database operation.
func insertUser(id int) result {
        r := result{
                id: id,
                op: fmt.Sprintf("insert USERS value (%d)", id),
        }

        // Randomize if the insert fails or not.
        if rand.Intn(10) == 0 {
                r.err = fmt.Errorf("Unable to insert %d into USER table", id)
        }

        return r
}

// insertTrans simulates a database operation.
func insertTrans(id int) result {
        r := result{
                id: id,
                op: fmt.Sprintf("insert TRANS value (%d)", id),
        }

        // Randomize if the insert fails or not.
        if rand.Intn(10) == 0 {
                r.err = fmt.Errorf("Unable to insert %d into USER table", id)
        }

        return r
}
      # Set fail-fast to false to ensure that feedback is delivered for all matrix combinations. Consider changing this to true when your workflow is stable.
      fail-fast: // ------
// Select
// ------

// This sample program demonstrates how to use a channel to monitor the amount of time
// the program is running and terminate the program if it runs too long.

package main

import (
        "errors"
        "log"
        "os"
        "os/signal"
        "time"
)

// Give the program 3 seconds to complete the work.
const timeoutSeconds = 3 * time.Second

// There are 4 channels that we are gonna use: 3 unbuffered and 1 buffered of 1.
var (
        // sigChan receives operating signals.
        // This will allow us to send a Ctrl-C to shut down our program cleanly.
        sigChan = make(chan os.Signal, 1)

        // timeout limits the amount of time the program has.
        // We really don't want to receive on this channel because if we do, that means something bad
        // happens, we are timing out and we need to kill the program.
        timeout = time.After(timeoutSeconds)

        // complete is used to report processing is done.
        // This is the channel we want to receive on. When the Goroutine finish the job, it will signal
        // to us on this complete channel and tell us any error that occurred.
        complete = make(chan error)

        // shutdown provides system wide notification.
        shutdown = make(chan struct{})
)

func main() {
        log.Println("Starting Process")

        // We want to receive all interrupt based signals.
        // We are using a Notify function from the signal package, passing sigChan telling the channel
        // to look for anything that is os.Interrupt related and sending us a data signal on this
        // channel.
        // One important thing about this API is that, it won't wait for us to be ready to receive the
        // signal. If we are not there, it will drop it on the floor. That's why we are using a
        // buffered channel of 1. This way we guarantee to get at least 1 signal. When we are ready to
        // act on that signal, we can come over there and do it.
        signal.Notify(sigChan, os.Interrupt)

        // Launch the process.
        log.Println("Launching Processors")

        // This Goroutine will do the processing job, for example image processing.
        go processor(complete)

        // The main Goroutine here is in this event loop and it's gonna loop forever until the program
        // is terminated.
        // There are 3 cases in select, meaning that there are 3 channels we are trying to receive on
        // at the same time: sigChan, timeout, and complete.

ControlLoop:
        for {
                select {
                case <-sigChan:
                        // Interrupt event signaled by the operation system.
                        log.Println("OS INTERRUPT")

                        // Close the channel to signal to the processor it needs to shutdown.
                        close(shutdown)

                        // Set the channel to nil so we no longer process any more of these events.
                        // If we try to send on a closed channel, we are gonna panic. If we receive on a closed
                        // channel, that's gonna immediately return a signal without data. If we receive on a
                        // nil channel, we are blocked forever. Similar with send.
                        // Why do we want to do that?
                        // We don't want user to hold down Ctrl C or hit Ctrl C multiple times. If they do that
                        // and we process the signal, we have to call close multiple time. When we call close
                        // on a channel that is already closed, the code will panic. Therefore, we cannot have
                        // that.
                        sigChan = nil

                case <-timeout:
                        // We have taken too much time. Kill the app hard.
                        log.Println("Timeout - Killing Program")

                        // os.Exit will terminate the program immediately.
                        os.Exit(1)

                case err := <-complete:
                        // Everything completed within the time given.
                        log.Printf("Task Completed: Error[%s]", err)

                        // We are using a label break here.
                        // We put one at the top of the for loop so the case has a break and the for has a
                        // break.
                        break ControlLoop
                }
        }

        // Program finished.
        log.Println("Process Ended")
}

// processor provides the main program logic for the program.
// There is something interesting in the parameter. We put the arrow on the right hand side of the
// chan keyword. It means this channel is a send-only channel. If we try to receive on this
// channel, the compiler will give us an error.
func processor(complete chan<- error) {
        log.Println("Processor - Starting")

        // Variable to store any error that occurs.
        // Passed into the defer function via closures.
        var err error

        // Defer the send on the channel so it happens regardless of how this function terminates.
        // This is an anonymous function call like we saw with Goroutine. However, we are using the
        // keyword defer here.
        // We want to execute this function but after the processor function returns. This gives us an
        // guarantee that we can have certain things happen before control go back to the caller.
        // Also, defer is the only way to stop a panic. If something bad happens, say the image library
        // is blowing up, that can cause a panic situation throughout the code. In this case, we want
        // to recover from that panic, stop it and then control the shutdown.
        defer func() {
                // Capture any potential panic.
                if r := recover(); r != nil {
                        log.Println("Processor - Panic", r)
                }

                // Signal the Goroutine we have shutdown.
                complete <- err
        }()

        // Perform the work.
        err = doWork()

        log.Println("Processor - Completed")
}

// doWork simulates task work.
// Between every single call, we call checkShutdown. After complete every tasks, we are asking:
// Have we been asked to shutdown? The only way we know is that shutdown channel is closed. The
// only way to know if the shutdown channel is closed is to try to receive. If we try to receive on
// a channel that is not closed, it's gonna block. However, the default case is gonna save us here.
func doWork() error {
        log.Println("Processor - Task 1")
        time.Sleep(2 * time.Second)

        if checkShutdown() {
                return errors.New("Early Shutdown")
        }

        log.Println("Processor - Task 2")
        time.Sleep(1 * time.Second)

        if checkShutdown() {
                return errors.New("Early Shutdown")
        }

        log.Println("Processor - Task 3")
        time.Sleep(1 * time.Second)

        return nil
}

// checkShutdown checks the shutdown flag to determine if we have been asked to interrupt processing.
func checkShutdown() bool {
        select {
        case <-shutdown:
                // We have been asked to shutdown cleanly.
                log.Println("checkShutdown - Shutdown Early")
                return true

        default:
                // If the shutdown channel was not closed, presume with normal processing.
                return false
        }
}

// Output:
// -------
// - When we let the program run, since we configure the timeout to be 3 seconds, it will
// then timeout and be terminated.
// - When we hit Ctrl C while the program is running, we will see the OS INTERRUPT and the program
// is being shutdown early.
// - When we send a signal quit by hitting Ctrt \, we will get a full stack trace of all the
// Goroutines.

      # Set up a matrix to run the following 3 configurations:
      # 1. <Windows, Release, latest MSVC compiler toolchain on the default runner image, default generator>
      # 2. <Linux, Release, latest GCC compiler toolchain on the default runner image, default generator>
      # 3. <Linux, Release, latest Clang compiler toolchain on the default runner image, default generator>
      #
      # To add more build types (Release, Debug, RelWithDebInfo, etc.) customize the build_type list.
      matrix: // ----------------------------------------
// Store and retrieve values from a context
// ----------------------------------------

// Context package is the answer to cancellation and deadline in Go.

package main

import (
        "context"
        "fmt"
)

// user is the type of value to store in the context.
type user struct {
        name string
}

// userKey is the type of value to use for the key. The key is type specific and only values of the
// same type will match.
// When we store a value inside a context, what getting stored is not just a value but also a type
// associated with the storage. We can only pull a value out of that context if we know the type of
// value that we are looking for.
// The idea of this userKey type becomes really important when we want to store a value inside the
// context.
type userKey int

func main() {
        // Create a value of type user.
        u := user{
                name: "Hoanh",
        }

        // Declare a key with the value of zero of type userKey.
        const uk userKey = 0

        // Store the pointer to the user value inside the context with a value of zero of type userKey.
        // We are using context.WithValue because a new context value and we want to initialize that
        // with data to begin with. Anytime we work a context, the context has to have a parent
        // context. This is where the Background function comes in. We are gonna store the key uk to
        // its value (which is 0 in this case), and address of user.
        ctx := context.WithValue(context.Background(), uk, &u)

        // Retrieve that user pointer back by user the same key type value.
        // Value allows us to pass the key of the corrected type (in our case is uk of userKey type)
        // and returns an empty interface. Because we are working with an interface, we have to perform
        // a type assertion to pull the value that we store in there out the interface so we can work
        // with the concrete again.
        if u, ok := ctx.Value(uk).(*user); ok {
                fmt.Println("User", u.name)
        }

        // Attempt to retrieve the value again using the same value but of a different type.
        // Even though the key value is 0, if we just pass 0 into this function call, we are not gonna
        // get back that address to the user because 0 is based on integer type, not our userKey type.
        // It's important that when we store the value inside the context to not use the built-in type.
        // Declare our own key type. That way, only us and who understand that type can pull that out.
        // Because what if multiple partial programs want to use that value of 0, we are all being
        // tripped up on each other. That type extends an extra level of protection on being able to
        // store and retrieve value out of context.
        // If we are using this, we want to raise a flag because we have to ask twice why do we want to
        // do that instead of passing down the call stack. Because if we can pass it down the call
        // stack, it would be much better for readability and maintainability for our legacy code in
        // the future.
        if _, ok := ctx.Value(0).(*user); !ok {
                fmt.Println("User Not Found")
        }
}
        os: [ // ----------
// WithCancel
// ----------

// Different ways we can do cancellation, timeout in Go.

package main

import (
        "context"
        "fmt"
        "time"
)

func main() {
        // Create a context that is cancellable only manually.
        // The cancel function must be called regardless of the outcome.
        // WithCancel allows us to create a context and provides us a cancel function that can be
        // called in order to report a signal, a signal without data, that we want whatever that
        // Goroutine is doing to stop right away. Again, we are using Background as our parents context.
        ctx, cancel := context.WithCancel(context.Background())

        // The cancel function must be called regardless of the outcome.
        // The Goroutine that creates the context must always call cancel. These are things that have
        // to be cleaned up. It's the responsibility that the Goroutine creates the context the first
        // time to make sure to call cancel after everything is done.
        // The use of the defer keyword is perfect here for this use case.
        defer cancel()

        // We launch a Goroutine to do some work for us.
        // It is gonna sleep for 50 milliseconds and then call cancel. It is reporting that it want to
        // signal a cancel without data.
        go func() {
                // Simulate work.
                // If we run the program using 50 ms, we expect the work to be complete. But if it is 150
                // ms, then we move on.
                time.Sleep(50 * time.Millisecond)

                // Report the work is done.
                cancel()
        }()

        // The original Goroutine that creates that channel is in its select case. It is gonna receive
        // after time.After. We are gonna wait 100 milliseconds for something to happen. We are also
        // waiting on context.Done. We are just gonna sit here, and if we are told to Done, we know
        // that work up there is complete.
        select {
        case <-time.After(100 * time.Millisecond):
                fmt.Println("moving on")
        case <-ctx.Done():
                fmt.Println("work complete")
        }
} ]
        build_type: [ // ------------
// WithDeadline
// ------------

package main

import (
        "context"
        "fmt"
        "time"
)

type data struct {
        UserID string
}

func main() {
        // Set a deadline.
        deadline := time.Now().Add(150 * time.Millisecond)

        // Create a context that is both manually cancellable and will signal
        // a cancel at the specified date/time.
        // We use Background as our parents context and set out deadline time.
        ctx, cancel := context.WithDeadline(context.Background(), deadline)
        defer cancel()

        // Create a channel to received a signal that work is done.
        ch := make(chan data, 1)

        // Ask a Goroutine to do some work for us.
        go func() {
                // Simulate work.
                time.Sleep(200 * time.Millisecond)

                // Report the work is done.
                ch <- data{"123"}
        }()

        // Wait for the work to finish. If it takes too long move on.
        select {
        case d := <-ch:
                fmt.Println("work complete", d)

        case <-ctx.Done():
                fmt.Println("work cancelled")
        }
} ]
        c_compiler: [ // -----------
// WithTimeout
// -----------

package main

import (
        "context"
        "fmt"
        "time"
)

type data struct {
        UserID string
}

func main() {
        // Set a duration.
        duration := 150 * time.Millisecond

        // Create a context that is both manually cancellable and will signal
        // a cancel at the specified duration.
        ctx, cancel := context.WithTimeout(context.Background(), duration)
        defer cancel()

        // Create a channel to received a signal that work is done.
        ch := make(chan data, 1)

        // Ask the goroutine to do some work for us.
        go func() {
                // Simulate work.
                time.Sleep(50 * time.Millisecond)

                // Report the work is done.
                ch <- data{"123"}
        }()

        // Wait for the work to finish. If it takes too long move on.
        select {
        case d := <-ch:
                fmt.Println("work complete", d)
        case <-ctx.Done():
                fmt.Println("work cancelled")
        }
} ]
        include: // ----------------
// Request/Response
// ----------------

// Sample program that implements a web request with a context that is
// used to timeout the request if it takes too long.

package main

import (
        "context"
        "io"
        "log"
        "net"
        "net/http"
        "os"
        "time"
)

func main() {
        // Create a new request.
        req, err := http.NewRequest("GET", "https://www.ardanlabs.com/blog/post/index.xml", nil)
        if err != nil {
                log.Println(err)
                return
        }

        // Create a context with a timeout of 50 milliseconds.
        ctx, cancel := context.WithTimeout(req.Context(), 50*time.Millisecond)
        defer cancel()

        // Declare a new transport and client for the call.
        tr := http.Transport{
                Proxy: http.ProxyFromEnvironment,
                DialContext: (&net.Dialer{
                        Timeout:   30 * time.Second,
                        KeepAlive: 30 * time.Second,
                        DualStack: true,
                }).DialContext,
                MaxIdleConns:          100,
                IdleConnTimeout:       90 * time.Second,
                TLSHandshakeTimeout:   10 * time.Second,
                ExpectContinueTimeout: 1 * time.Second,
        }

        client := http.Client{
                Transport: &tr,
        }

        // Make the web call in a separate Goroutine so it can be cancelled.
        ch := make(chan error, 1)
        go func() {
                log.Println("Starting Request")

                // Make the web call and return any error.
                // client.Do is going out and trying to hit the request URL. It's probably blocked right
                // now because it will need to wait for the entire document to comeback.
                resp, err := client.Do(req)

                // It the error occurs, we perform a send on the channel to report that we are done. We are
                // going to use this channel at some point to report back what is happening.
                if err != nil {
                        ch <- err
                        return
                }

                // If it doesn't fail, we close the response body on the return.
                defer resp.Body.Close()

                // Write the response to stdout.
                io.Copy(os.Stdout, resp.Body)

                // Then send back the nil instead of error.
                ch <- nil
        }()

        // Wait the request or timeout.
        // We perform a receive on ctx.Done saying that we want to wait 50 ms for that whole process
        // above to happen. If it doesn't, we signal back to that Goroutine to cancel the sending
        // request. We don't have to just walk away and let that eat up resources and finish because we
        // are not gonna need it. We are able to call CancelRequest and underneath, we are able to kill
        // that connection.
        select {
        case <-ctx.Done():
                log.Println("timeout, cancel work...")

                // Cancel the request and wait for it to complete.
                tr.CancelRequest(req)
                log.Println(<-ch)
        case err := <-ch:
                if err != nil {
                        log.Println(err)
                }
        }
}
          - os: windows-latest
            c_compiler: cl
            cpp_compiler: cl
          - os: ubuntu-latest
            c_compiler: gcc
            cpp_compiler: g++
          - os: ubuntu-latest
            c_compiler: clang
            cpp_compiler: clang++
        exclude: // --------------
// Race Detection
// --------------

// As soon as we add another Goroutine to our program, we add a huge amount of complexity. We can't
// always let the Goroutine run stateless. There has to be coordination. There are, in fact, 2
// things that we can do with multithread software.
// (1) We either have to synchronize access to share state like that WaitGroup is done with Add, Done
// and Wait.
// (2) Or we have to coordinate these Goroutines to behave in a predictable or responsible manner.
// Up until the use of channel, we have to use atomic function, mutex, to do both. The channel
// gives us a simple way to do orchestration. However, in many cases, using atomic function, mutex,
// and synchronizing access to shared state is the best way to go.

// Atomic instructions are the fastest way to go because deep down in memory, Go is synchronizaing
// 4-8 bytes at a time.
// Mutexes are the next fastest. Channels are very slow because not only they are mutexes, there
// are all data structure and logic that go with them.

// Data races is when we have multiple Goroutines trying to access the same memory location.
// For example, in the simplest case, we have a integer that is a counter. We have 2 Goroutines
// that want to read and write to that variable at the same time. If they are actually doing it at
// the same time, they are going to trash each other read and write. Therefore, this type of
// synchronizing access to the shared state has to be coordinated.

// The problem with data races is that they always appear random.

// Sample program to show how to create race conditions in our programs. We don't want to do this.
// To identify race condition : go run -race <file_name>

package main

import (
        "fmt"
        "runtime"
        "sync"
)

// counter is a variable incremented by all Goroutines.
var counter int

func main() {
        // Number of Goroutines to use.
        const grs = 2

        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(grs)

        // Create two Goroutines.
        // They loop twice: perform a read to a local counter, increase by 1, write it back to the shared state
        // Every time we run the program, the output should be 4.
        // The data races that we have here is that: at any given time, both Gooutines could be reading
        // and writing at the same time. However, we are very lucky in this case. What we are seeing it
        // that, each Goroutine is executing the 3 statements atomically completely by accident every
        // time this code run.
        // If we put the line runtime.Gosched(), it will tell the scheduler to be part of the
        // cooperation here and yield my time on that m. This will force the data race to happen. Once
        // we read the value out of that shared state, we are gonna force the context switch. Then we
        // come back, we are not getting 4 as frequent.
        for i := 0; i < grs; i++ {
                go func() {
                        for count := 0; count < 2; count++ {
                                // Capture the value of Counter.
                                value := counter

                                // Yield the thread and be placed back in queue.
                                // FOR TESTING ONLY! DO NOT USE IN PRODUCTION CODE!
                                runtime.Gosched()

                                // Increment our local value of Counter.
                                value++

                                // Store the value back into Counter.
                                counter = value
                        }
                        wg.Done()
                }()
        }

        // Wait for the goroutines to finish.
        wg.Wait()
        fmt.Println("Final Counter:", counter)
}
          - os: windows-latest
            c_compiler: gcc
          - os: windows-latest
            c_compiler: clang
          - os: ubuntu-latest
            c_compiler: cl

    steps: // ----------------
// Atomic Functions
// ----------------

package main

import (
        "fmt"
        "runtime"
        "sync"
        "sync/atomic"
)

// counter is a variable incremented by all Goroutines.
// Notice that it's not just an int but int64. We are being very specific about the precision
// because the atomic function requires us to do so.
var counter int64

func main() {
        // Number of Goroutines to use.
        const grs = 2

        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(grs)

        // Create two goroutines.
        for i := 0; i < grs; i++ {
                go func() {
                        for count := 0; count < 2; count++ {
                                // Safely add one to counter.
                                // Add the atomic functions that we have take an address as the first parameter and
                                // that is being synchronized, no matter many Goroutines they are. If we call one
                                // of these function on the same location, they will get serialized. This is the
                                // fastest way to serialization.
                                // We can run this program all day long and still get 4 every time.
                                atomic.AddInt64(&counter, 1)

                                // This call is now irrelevant because by the time AddInt64 function complete,
                                // counter is incremented.
                                runtime.Gosched()
                        }

                        wg.Done()
                }()
        }

        // Wait for the Goroutines to finish.
        wg.Wait()

        // Display the final value.
        fmt.Println("Final Counter:", counter)
}
    - uses: // -------
// Mutexes
// -------

// We don't always have the luxury of using of 4-8 bytes of memory as a shared data. This is where
// the muxtex comes in. Mutex allows us to have the API like the WaitGroup (Add, Done and Wait)
// where any Goroutine can execute one at a time.

package main

import (
        "fmt"
        "sync"
)

var (
        // counter is a variable incremented by all Goroutines.
        counter int

        // mutex is used to define a critical section of code.
        // Picture mutex as a room where all Goroutines have to go through. However, only one Goroutine
        // can go at a time. The scheduler will decide who can get in and which one is next. We cannot
        // determine what the scheduler is gonna do. Hopefully, it is gonna be fair. Just because one
        // Goroutine got to the door before another, it doesn't mean that Goroutine will get to the end first. Nothing
        // here is predictable.

        // The key here is, once a Goroutine is allowed in, it must report that it's out.
        // All the Goroutines come in will ask for a lock and unlock when it leave for other one to get
        // in.

        // Two different functions can use the same mutex which means only one Goroutine can execute
        // any of given functions at a time.
        mutex sync.Mutex
)

func main() {
        // Number of Goroutines to use.
        const grs = 2

        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(grs)

        // Create two Goroutines.
        for i := 0; i < grs; i++ {
                go func() {
                        for count := 0; count < 2; count++ {
                                // Only allow one Goroutine through this critical section at a time.
                                // Creating these artificial curly brackets gives readability. We don't have to do
                                // this but it is highly recommended.
                                // The Lock and Unlock function must always be together in line of sight.
                                mutex.Lock()
                                {
                                        // Capture the value of counter.
                                        value := counter

                                        // Increment our local value of counter.
                                        value++

                                        // Store the value back into counter.
                                        counter = value
                                }
                                mutex.Unlock()
                                // Release the lock and allow any waiting Goroutine through.
                        }

                        wg.Done()
                }()
        }

        // Wait for the Goroutines to finish.
        wg.Wait()
        fmt.Printf("Final Counter: %d\n", counter)
}

    - name: Set reusable strings
      # Turn repeated input strings (such as the build output directory) into step outputs. These step outputs can be used throughout the workflow file.
      id: // ----------------
// Read/Write Mutex
// ----------------

// There are times when we have a shared resource where we want many Goroutines reading it.
// Occasionally, one Goroutine can come in and make change to the resource. When that happens, everybody
// has to stop reading. It doesn't make sense to synchronize reads in this type of scenario
// because we are just adding latency to our software for no reason.

package main

import (
        "fmt"
        "math/rand"
        "sync"
        "sync/atomic"
        "time"
)

var (
        // data is a slice that will be shared.
        data []string

        // rwMutex is used to define a critical section of code.
        // It is a little bit slower than Mutex but we are optimizing the correctness first so we don't
        // care about that for now.
        rwMutex sync.RWMutex

        // Number of reads occurring at any given time.
        // As soon as we see int64 here, we should start thinking about using atomic instruction.
        readCount int64
)

// init is called prior to main.
func init() {
        rand.Seed(time.Now().UnixNano())
}

func main() {
        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(1)

        // Create a writer Goroutine that performs 10 different writes.
        go func() {
                for i := 0; i < 10; i++ {
                        time.Sleep(time.Duration(rand.Intn(100)) * time.Millisecond)
                        writer(i)
                }
                wg.Done()
        }()

        // Create eight reader Goroutines that runs forever.
        for i := 0; i < 8; i++ {
                go func(i int) {
                        for {
                                reader(i)
                        }
                }(i)
        }

        // Wait for the write Goroutine to finish.
        wg.Wait()
        fmt.Println("Program Complete")
}

// writer adds a new string to the slice in random intervals.
func writer(i int) {
        // Only allow one Goroutine to read/write to the slice at a time.
        rwMutex.Lock()
        {
                // Capture the current read count.
                // Keep this safe though we can due without this call.
                // We want to make sure that no other Goroutines are reading. The value of rc should always
                // be 0 when this code run.
                rc := atomic.LoadInt64(&readCount)

                // Perform some work since we have a full lock.
                fmt.Printf("****> : Performing Write : RCount[%d]\n", rc)
                data = append(data, fmt.Sprintf("String: %d", i))
        }
        rwMutex.Unlock()
}

// reader wakes up and iterates over the data slice.
func reader(id int) {
        // Any Goroutine can read when no write operation is taking place.
        // RLock has the corresponding RUnlock.
        rwMutex.RLock()
        {
                // Increment the read count value by 1.
                rc := atomic.AddInt64(&readCount, 1)

                // Perform some read work and display values.
                time.Sleep(time.Duration(rand.Intn(10)) * time.Millisecond)
                fmt.Printf("%d : Performing Read : Length[%d] RCount[%d]\n", id, len(data), rc)

                // Decrement the read count value by 1.
                atomic.AddInt64(&readCount, -1)
        }
        rwMutex.RUnlock()
}

// Lesson:
// -------
// The atomic functions and mutexes create latency in our software. Latency can be good when we
// have to coordinate orchestrating. However, if we can reduce latency using Read/Write Mutex, life
// is better.

// If we are using mutex, make sure that we get in and out of mutex as fast as possible. Do
// not anything extra. Sometimes just reading the shared state into a local variable is all we need
// to do. The less operation we can perform on the mutex, the better. We then reduce the latency to
// the bare minimum.
      shell: // ----------------------
// Go Scheduler Internals
// ----------------------

// Every time our Go's program starts up, it looks to see how many cores are available. Then it
// creates a logical processor.

// The operating system scheduler is considered a preemptive scheduler. It runs down there in the
// kernel. Its job is to look at all the threads that are in runnable states and gives them the
// opportunity to run on some cores. These algorithms are fairly complex: waiting, bouncing
// threads, keeping memory of threads, caching,... The operating system is doing all of that for
// us. The algorithm is really smart when it comes to multicore processor. Go doesn't want to
// reinvent the wheel. It wants to sit on top of the operating system and leverage it.

// The operating system is still responsible for operating system threads, scheduling operating
// system threads efficiently. If we have a 2 core machine and a thousands threads that the
// operating system has to schedule, that's a lot of work. A context switch on some operating
// system thread is expensive when the operating system have no clues of what that thread is doing.
// It has to save all the possible states in order to be able to restore that to exactly the way it
// was. If there are fewer threads, each thread can get more time to be rescheduled. If there are more
// threads, each thread has less time over a long period of time.

// "Less is more" is a really big concept here when we start to write concurrent software. We want to
// leverage the preemptive scheduler. So the Go's scheduler, the logical processor actually runs in
// user mode, the mode our application is running at. Because of that, we have to call the Go's
// scheduler a cooperating scheduler. What brilliant here is the runtime that coordinating the
// operation. It still looks and feels as a preemptive scheduler up in user land. We will see how "less
// is more" concept gets to present itself and we get to do a lot more work with less. Our goal needs
// to be how much work we get done with the less number of threads.

// Think about this in a simple way because processors are complex: hyperthreading, multiple threads
// per core, clock cycle. We can only execute one operating system thread at a time on any given
// core. If we only have 1 core, only 1 thread can be executed at a time. Anytime we have more
// threads in runnable states than we have cores, we are creating load, latency and we are getting
// more work done as we want. There needs to be this balance because not every thread is
// necessarily gonna be active at the same time. It all comes down to determining, understanding
// the workload for the software that we are writing.

// Back to the first idea, when our Go program comes up, it has to see how many cores that
// available. Let's say it found 1. It is going to create a logical processor P for that core.
// Again, the operating system is scheduling things around operating system threads. What this
// processor P will get is an m, where m stands for machine. It represents an operating system
// thread that the operating system is going to schedule and allows our code to run.

// The Linux scheduler has a run queue. Threads are placed in run queue in certain cores or
// family of cores and those are constantly bounded as threads are running. Go is gonna do the same
// thing. Go has its run queue as well. It has Global Run Queue (GRQ) and every P has a Local Run
// Queue (LRQ).

// Goroutine
// ---------
// What is a Goroutine? It is a path of execution. Threads are paths of execution. That path of
// execution needs to be scheduled. In Go, every function or method can be created to be a
// Goroutine, can become an independent path of execution that can be scheduled to run on some
// operating system threads against some cores.

// When we start our Go program, the first thing runtime gonna do is creating a Goroutine
// and putting that in some main LRQ for some P. In our case, we only have 1 P here so we can
// imagine that Goroutine is attached to P.

// A Goroutine, just like thread, can be in one of three major states: sleeping, executing or in
// runnable state asking to wait for some time to execute on the hardware. When the runtime creates
// a Goroutine, it is gonna placed in P and multiplex on this thread. Remember that it's the
// operating system that taking the thread, scheduling it, placing it on some core and doing
// execution. So Go's scheduler is gonna take all the code related to that Goroutine's path of
// execution, place it on a thread, tell the operating system that this thread is in runnable state
// and can we execute it. If the answer is yes, the operating system starts to execute on some
// cores there in the hardware.

// As the main Goroutine runs, it might want to create more paths of execution, more Goroutines.
// When that happens, those Goroutines might find themselves initially in the GRQ. These would be
// Goroutines that are in runnable state but haven't been assigned to some Ps yet. Eventually, they
// would end up in the LRQ where they're saying they would like some time to execute.

// This queue does not necessarily follow First-In-First-Out protocol. We have to understand that
// everything here is non-deterministic, just like the operating system scheduler. We cannot
// predict what the scheduler is gonna do when all things are equal. It is gonna make sure there is
// a balance. Until we get into orchestration, till we learn how to coordinate these execution of
// these Goroutines, there is no predictability.

// Here is the mental model of our example.
// GRQ
//      m
//      |
//    -----          LRQ
//   |  P  | ----------
//    -----           |
//      |             G1
//     Gm             |
//                    G2

// We have Gm executing on for this thread for this P, and we are creating 2 more Goroutines G1 and
// G2. Because this is a cooperating scheduler, that means that these Goroutines have to cooperate
// to be scheduled, to be swapped context switch on this operating system thread m.

// There are 4 major places in our code where the scheduler has the opportunity to make a
// scheduling decision.
// - The keyword go that we are going to create Goroutines. That is also an opportunity for the
// scheduler to rebalance when it has multiple P.
// - A system call. These system calls tend to happen all the time already.
// - A channel operation because there is mutex (blocking call) that we will learn later.
// - Garbage collection.

// Back to the example, says the scheduler might decide Gm has enough time to run, it will put Gm
// back to the run queue and allow G1 to run on that m. We are now having context switch.
//      m
//      |
//    -----      LRQ
//   |  P  | ------
//    -----       |
//      |         Gm
//      G1        |
//                G2

// Let's say G1 decides to open up a file. Opening up a file can take microsecond or 10 milliseconds. We
// don't really know. If we allow this Goroutine to block this operating system thread while we
// open up that file, we are not getting more work done. In this scenario here, having a single P,
// we are single threaded software application. All Goroutines only execute on the m attached to
// this P. What happen is this Goroutine is gonna block this m for potential a long time. We are
// basically be stalled while we still have works that need to get done. So the scheduler is not
// gonna allow that to happen, What actually happen is that the scheduler is gonna detach that m
// and G1. It is gonna bring a new m, say m2, then decide what G from the run queue should run
// next, say G2.
//            m2
//            |
//    m     -----      LRQ
//    |    |  P  | ------
//    G1    -----       |
//            |         Gm
//            G2

// We are now have 2 threads in a single threaded program. From our perspective, we are still
// single threading because the code that we are writing, the code associated with any G can only
// run against this P and this m. However, we don't know at any given time what m we are running on. m
// can get swapped out but we are still single threaded.

// Eventually, G1 will come back, the file will be opened. The scheduler is gonna take this G1 and
// put it back to the run queue so we can be executed against on this P for some m (m2 in this
// case). m is get placed on the side for later use. We are still maintaining these 2 threads. The
// whole process can happen again.
//            m2
//            |
//    m     -----      LRQ
//         |  P  | ------
//          -----       |
//            |         Gm
//            G2        |
//                      G1

// It is a really brilliant system of trying to leverage this thread to its fullest capability by
// doing more on 1 thread. Let's do so much on this thread we don't need another.

// There is something called a Network poller. It is gonna do all the low level networking
// asynchronous networking stuff. Our G, if it is gonna do anything like that, it might be moved out
// to the Network poller and then brought back in. From our perspective, here is what we have to
// remember:
// The code that we are writing always run on some P against some m. Depending on how many P we
// have, that's how many threads variables for us to run.

// Concurrency is about managing a lot of thing at once. This is what the scheduler is doing. It
// manages the execution of these 3 Goroutines against this one m for this P. Only 1 Goroutine can
// be executed at a single time.

// If we want to do something in parallel, which means doing a lot of things at once, then we would
// need to create another P that has another m, say m3.
//    m3              m2
//    |               |
//  -----            -----      LRQ
// |  P  | ------   |  P  | ------
//  -----       |    -----       |
//    |         Gx     |         Gm
//    Gx        |      G2        |
//              Gx               G1

// Both are scheduled by the operating system. So now we can have 2 Goroutines running at the
// same time in parallel.

// Let's try another example.
// --------------------------
// We have a multiple threaded software. The program launched 2 threads. Even if both threads end
// up on the same core, each want to pass a message to each other. What has to happen from the
// operating system point of view?
// We have to wait for thread 1 to get scheduled and placed on some cores - a context switch (CTX)
// has to happen here. While that's happening, thread is asleep so it's not running at all. From
// thread 1, we send a message over and want to wait to get a message back. In order to do that,
// there is another to context switch needs to be happened because we can put a different thread on
// that core (?). We are waiting for the operating system to schedule thread 2 so we are going to
// get another context switch, waking up and running, processing the message and sending the
// message back. On every single message that we are passing back and forth, thread is gonna from
// executable state to runnable state to asleep state. This is gonna cost a lot of context switches
// to occur.
//  T1                         T2
//  | CTX      message     CTX |
//  | -----------------------> |
//  | CTX                    | |
//  | CTX      message     CTX |
//  | <----------------------- |
//  | CTX                  CTX |

// Let's see what happen when we are using Goroutines, even on a single core.
// G1 wants to send a message to G2 and we perform a context switch. However, the context here
// is user's space switch. G1 can be taken out of the thread and G2 can be put on the thread. From the
// operating system point of view, this thread never go to sleep. This thread is always executing
// and never needed to be context switched out. It is the Go's scheduler that keeps the Goroutines
// context switched.
//               m
//               |
//             -----
//            |  P  |
//             -----
//  G1                         G2
//  | CTX      message     CTX |
//  | -----------------------> |
//  | CTX                    | |
//  | CTX      message     CTX |
//  | <----------------------- |
//  | CTX                  CTX |

// If a P for some m here has no work to do, there is no G, the runtime scheduler will try to spin
// that m for a little bit to keep it hot on the core. Because if that thread goes cold, the
// operating system will pull it off the core and put something else on. So it just spin a little
// bit to see if there will be another G comes in to get some work done.

// This is how the scheduler work underneath. We have a P, attached to thread m. The operating
// system will do the scheduling. We don't want any more than cores we have. We don't need any more
// operating system threads than cores we have. If we have more threads than cores we have, all we
// do is putting load on the operating system. We allow the Go's scheduler to make decisions on our
// Goroutines, keeping the least number of threads we need and hot all the time if we have work. The
// Go's scheduler is gonna look and feel preemptive even though we are calling a cooperating
// scheduler.

// However, let's not think about how the scheduler work. Think the following way makes it easier
// for future development.
// Every single G, every Goroutine that is in runnable state, is running at the same time.

package main

import "fmt"

func main() {
        fmt.Println("ok")
}
      run: |
        echo "build-output-dir=${{ // ------------------
// Language Mechanics
// ------------------

// One of the most important thing that we must do from day one is to write software that can
// startup and shutdown cleanly. This is very very important.

package main

import (
        "fmt"
        "runtime"
        "sync"
)

// init calls a function from the runtime package called GOMAXPROCS. This is also an environment
// variable, which is why is all capitalized.
// Prior to 1.5, when our Go program came up for the first time, it came up with just a single P,
// regardless of how many cores. The improvement that we made to the garbage collector and
// scheduler changed all that.
func init() {
        // Allocate one logical processor for the scheduler to use.
        runtime.GOMAXPROCS(1)
}

func main() {
        // wg is used to manage concurrency.
        // wg is set to its zero value. This is one of the very special types in Go that are usable in
        // its zero value state.
        // It is also called Asynchronous Counting Semaphore. It has three methods: Add, Done and Wait.
        // n number of Goroutines can call this method at the same time and it's all get serialized.
        // - Add keeps a count of how many Goroutines out there.
        // - Done decrements that count because some Goroutines are about to terminated.
        // - Wait holds the program until that count goes back down to zero.
        var wg sync.WaitGroup

        // We are creating 2 Gorouines.
        // We rather call Add(1) and call it over and over again to increment by 1. If we don't how
        // many Goroutines that we are going to create, that is a smell.
        wg.Add(2)

        fmt.Println("Start Goroutines")

        // Create a Goroutine from the uppercase function using anonymous function.
        // We have a function decoration here with no name and being called by the () in the end. We
        // are declaring and calling this function right here, inside of main. The big thing here is
        // the keyword go in front of func().
        // We don't execute this function right now in series here. Go schedules that function to be a
        // G, say G1, and load in some LRQ for our P. This is our first G.
        // Remember, we want to think that every G that is in runnable state is running at the same time.
        // Even though we have a single P, even though we have a single thread, we don't care.
        // We are having 2 Goroutines running at the same time: main and G1.
        go func() {
                lowercase()
                wg.Done()
        }()

        // Create a Goroutine from the lowercase function.
        // We are doing it again. We are now having 3 Goroutines running at the same time.
        go func() {
                uppercase()
                wg.Done()
        }()

        // Wait for the Goroutines to finish.
        // This is holding main from terminating because when the main terminates, our program
        // terminates, regardless of what any other Goroutine is doing.
        // There is a golden rule here: We are not allowed to create a Goroutine unless we can tell
        // when and how it terminates.
        // Wait allows us to hold the program until the two other Goroutines report that they are done.
        // It is gonna wait, count from 2 to 0. When it reaches 0, the scheduler will wake up the main
        // Goroutine again and allow it to be terminated.
        fmt.Println("Waiting To Finish")
        wg.Wait()

        fmt.Println("\nTerminating Program")
}

// lowercase displays the set of lowercase letters three times.
func lowercase() {
        // Display the alphabet three times
        for count := 0; count < 3; count++ {
                for r := 'a'; r <= 'z'; r++ {
                        fmt.Printf("%c ", r)
                }
        }
}

// uppercase displays the set of uppercase letters three times.
func uppercase() {
        // Display the alphabet three times
        for count := 0; count < 3; count++ {
                for r := 'A'; r <= 'Z'; r++ {
                        fmt.Printf("%c ", r)
                }
        }
}

// Sequence
// --------
// We call the uppercase after lowercase but Go's scheduler chooses to call the lowercase first.
// Remember we are running on a single thread so there is only one Goroutine is executed at a given
// time here. We can't see that we are running concurrently that the uppercase runs before the
// lowercase. Everything starts and completes cleanly.

// What if we forget to hold Wait?
// -------------------------------
// We would see no output of uppercase and lowercase. This is pretty much a data race. It's a race
// to see the program terminates before the scheduler stops it and schedules another Goroutine to
// run. By not waiting, these Goroutine never get a chance to execute at all.

// What if we forget to call Done?
// -------------------------------
// Deadlock!
// This is a very special thing in Go. When the runtime determines that all the Goroutines are
// there can no longer move forward, it's gonna panic. }}/build" >> "$GITHUB_OUTPUT"

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: >
        cmake -B ${{ // ----------------------
// Goroutine time slicing
// ----------------------

// How the Go's scheduler, even though it is a cooperating scheduler (not preemptive), it looks
// and feel preemptive because the runtime scheduler is making all the decisions for us. It is not
// coming for us.

// The program below will show us a context switch and how we can predict when the context switch
// is going to happen. It is using the same pattern that we've seen in the last file. The only
// difference is the printPrime function.

package main

import (
        "fmt"
        "runtime"
        "sync"
)

func init() {
        // Allocate one logical processor for the scheduler to use.
        runtime.GOMAXPROCS(1)
}

func main() {
        // wg is used to manage concurrency.
        var wg sync.WaitGroup
        wg.Add(2)

        fmt.Println("Create Goroutines")

        // Create the first goroutine and manage its lifecycle here.
        go func() {
                printPrime("A")
                wg.Done()
        }()

        // Create the second goroutine and manage its lifecycle here.
        go func() {
                printPrime("B")
                wg.Done()
        }()

        // Wait for the goroutines to finish.
        fmt.Println("Waiting To Finish")
        wg.Wait()

        fmt.Println("Terminating Program")
}

// printPrime displays prime numbers for the first 5000 numbers.
// printPrime is not special. It just requires a little bit more time to complete.
// When we run the program, what we will see are context switches at some point for some particular
// prime number. We cannot predict when the context switch happen. That's why we say the Go's
// scheduler looks and feels very preemptive even though it is a cooperating scheduler.
func printPrime(prefix string) {
next:
        for outer := 2; outer < 5000; outer++ {
                for inner := 2; inner < outer; inner++ {
                        if outer%inner == 0 {
                                continue next
                        }
                }

                fmt.Printf("%s:%d\n", prefix, outer)
        }

        fmt.Println("Completed", prefix)
} }}
        -DCMAKE_CXX_COMPILER=${{ // --------------------------
// Goroutines and parallelism
// --------------------------

// This programs show how Goroutines run in parallel.
// We are going to have 2 P with 2 m, and 2 Goroutines running in parallel on each m.
// This is still the same program that we are starting with. The only difference is that we are
// getting rid of the lowercase and uppercase function and putting their code directly inside Go's
// anonymous functions.

// Looking at the output, we can see a mix of uppercase of lowercase characters. These Goroutines
// are running in parallel now.

package main

import (
        "fmt"
        "runtime"
        "sync"
)

func init() {
        // Allocate 2 logical processors for the scheduler to use.
        runtime.GOMAXPROCS(2)
}

func main() {
        // wg is used to wait for the program to finish.
        // Add a count of two, one for each goroutine.
        var wg sync.WaitGroup
        wg.Add(2)

        fmt.Println("Start Goroutines")

        // Declare an anonymous function and create a goroutine.
        go func() {
                // Display the alphabet three times.
                for count := 0; count < 3; count++ {
                        for r := 'a'; r <= 'z'; r++ {
                                fmt.Printf("%c ", r)
                        }
                }

                // Tell main we are done.
                wg.Done()
        }()

        // Declare an anonymous function and create a goroutine.
        go func() {
                // Display the alphabet three times.
                for count := 0; count < 3; count++ {
                        for r := 'A'; r <= 'Z'; r++ {
                                fmt.Printf("%c ", r)
                        }
                }

                // Tell main we are done.
                wg.Done()
        }()

        // Wait for the goroutines to finish.
        fmt.Println("Waiting To Finish")
        wg.Wait()

        fmt.Println("\nTerminating Program")
} }}
        -DCMAKE_C_COMPILER=${{ // ---------------------
// Interface Conversions
// ---------------------

package main

import "fmt"

// Mover provides support for moving things.
type Mover interface {
        Move()
}

// Locker provides support for locking and unlocking things.
type Locker interface {
        Lock()
        Unlock()
}

// MoveLocker provides support for moving and locking things.
type MoveLocker interface {
        Mover
        Locker
}

// bike represents a concrete type for the example.
type bike struct{}

// Move can change the position of a bike.
func (bike) Move() {
        fmt.Println("Moving the bike")
}

// Lock prevents a bike from moving.
func (bike) Lock() {
        fmt.Println("Locking the bike")
}

// Unlock allows a bike to be moved.
func (bike) Unlock() {
        fmt.Println("Unlocking the bike")
}

func main() {
        // Declare variables of the MoveLocker and Mover interfaces set to their zero value.
        var ml MoveLocker
        var m Mover

        // Create a value of type bike and assign the value to the MoveLocker interface value.
        ml = bike{}

        // An interface value of type MoveLocker can be implicitly converted into
        // a value of type Mover. They both declare a method named move.
        m = ml

        //    ml                          m
        //  ------                      ------
        // | bike |        bike        | bike |
        //  ------        ------        ------
        // |  *   | ---> |      | <--- |  *   |
        //  ------        ------        ------

        // However, we cannot go in the other direction, like so:
        // ml = m

        // The compiler will say:
        // cannot use m (type Mover) as type MoveLocker in assignment: Mover does not
        // implement MoveLocker (missing Lock method).

        // --------------
        // Type assertion
        // --------------

        // Interface type Mover does not declare methods named lock and unlock. Therefore, the compiler
        // can't perform an implicit conversion to assign a value of interface type Mover to an
        // interface value of type MoveLocker. It is irrelevant that the concrete type value of
        // type bike that is stored inside of the Mover interface value implements the MoveLocker interface.

        // We can perform a type assertion at runtime to support the assignment.

        // Perform a type assertion against the Mover interface value to access a COPY of the concrete type
        // value of type bike that was stored inside of it. Then assign the COPY of the concrete type
        // to the MoveLocker interface.


        // This is the syntax for type assertion.
        // We are taking the interface value itself, dot (bike). We are using bike as an parameter.
        // If m is not nil and there is a bike inside of m, we will get a copy of it since we are using value semantic.
        // Or else, a panic occurs.
        // b is having a copy of bike value.
        b := m.(bike)

        // We can prevent panic when type assertion breaks by destructuring
        // the boolean value that represents type assertion result
        b, ok := m.(bike)
        fmt.Println("Does m has value of bike?:", ok)

        ml = b

        // It's important to note that the type assertion syntax provides a way to state what type
        // of value is stored inside the interface. This is more powerful from a language and readability
        // standpoint, than using a casting syntax, like in other languages.
} }}
        -DCMAKE_BUILD_TYPE=${{ // -----------------------
// Runtime Type Assertions
// -----------------------

package main

import (
        "fmt"
        "math/rand"
        "time"
)

// car represents something you drive.
type car struct{}

// String implements the fmt.Stringer interface.
func (car) String() string {
        return "Vroom!"
}

// cloud represents somewhere you store information.
type cloud struct{}

// String implements the fmt.Stringer interface.
func (cloud) String() string {
        return "Big Data!"
}

func main() {
        // Seed the number random generator.
        rand.Seed(time.Now().UnixNano())

        // Create a slice of the Stringer interface values.

        //  ---------------------
        // |   car   |   cloud   |
        //  ---------------------
        // |    *    |     *     |
        //  ---------------------
        //      A          A
        //      |          |
        //     car       cloud
        //    -----      -----
        //   |     |    |     |
        //    -----      -----

        mvs := []fmt.Stringer{
                car{},
                cloud{},
        }

        // Let's run this experiment ten times.
        for i := 0; i < 10; i++ {
                // Choose a random number from 0 to 1.
                rn := rand.Intn(2)

                // Perform a type assertion that we have a concrete type of cloud in the interface
                // value we randomly chose.
                // This shows us that this checking is at runtime, not compile time.
                if v, ok := mvs[rn].(cloud); ok {
                        fmt.Println("Got Lucky:", v)
                        continue
                }

                // We have to guarantee that variable in question (x in `x.(T)`) can always be asserted correctly as T type

                // Or else, We wouldn't want to use that ok variable because we want it to panic if there is an integrity
                // issue. We must shut it down immediately if that happens if we cannot recover from a
                // panic and guarantee that we are back at 100% integrity, the software has to be restarted. 
                // Shutting down means you have to call log.Fatal, os.exit, or panic for stack trace.
                // When we use type assertion, we need to understand when it is okay that whatever
                // we are asking for is not there.

                // Important note:
                // ---------------
                // If the type assertion is causing us to call the concrete value out, that should raise a big
                // flag. We are using interface to maintain a level of decoupling and now we are using type
                // assertion to go back to the concrete.
                // When we are in the concrete, we are putting our codes in the situation where cascading
                // changes can cause widespread refactoring. What we want with interface is the opposite,
                // internal changes minimize cascading changes.
                fmt.Println("Got Unlucky")
        }
} }}
        -S ${{ // ------------------
// Struct Composition
// ------------------

// Prototyping is important, as well as writing proof of concept and solving problem in the
// concrete first. Then we can ask ourselves: What can change? What change is coming? so we can
// start decoupling and refactor.

// Refactoring need to become a part of the development cycle.

// Here is the problem that we are trying to solve in this section.
// We have a system called Xenia that has a database.
// There is another system called Pillar, which is a web server with some front-end that consume
// it. It has a database too.
// Our goal is to move the Xenia's data into Pillar's system.

// How long is it gonna take?
// How do we know when a piece of code is done so we can move on the next piece of code?
// If you are a technical manager, 
// how do you know whether your debt is "wasting effort" or "not putting enough effort"?

// Being done has 2 parts:
// One is test coverage, 80% in general and 100% on the happy path.
// Second is about changes. By asking what can change, from technical perspective and business
// perspective, we make sure that we refactor the code to be able to handle that change.

// One example is, we can give you a concrete version in 2 days but we need 2 weeks to be able to
// refactor this code to deal with the change that we know it's coming.

// The plan is to solve one problem at a time. Don't be overwhelmed by everything.
// Write a little code, write some tests and refactor. Write layer of APIs that work on top of each
// other, knowing that each layer is a strong foundation to the next.

// Do not pay too much attention in the implementation detail. 
// It's the mechanics here that are important.
// We are optimizing for correctness, not performance. 
// We can always go back if it doesn't perform well enough to speed things up.

// Next step:
// ----------
// Decouple using interface.

package main

import (
        "errors"
        "fmt"
        "io"
        "math/rand"
        "time"
)

// The first problem that we have to solve is that we need a software that run on a timer. It need
// to connect to Xenia, read that database, identify all the data we haven't moved and pull it in.
func init() {
        rand.Seed(time.Now().UnixNano())
}

// Data is the structure of the data we are copying.
// For simplicity, just pretend it is a string data.
type Data struct {
        Line string
}

// Xenia is a system we need to pull data from.
type Xenia struct {
        Host    string
        Timeout time.Duration
}

// Pull knows how to pull data out of Xenia.
// We could do func (*Xenia) Pull() (*Data, error) that return the data and error. However, this
// would cost an allocation on every call and we don't want that.
// Using the function below, we know data is a struct type and its size ahead of time. Therefore
// they could be on the stack.
func (*Xenia) Pull(d *Data) error {
        switch rand.Intn(10) {
        case 1, 9:
                return io.EOF

        case 5:
                return errors.New("Error reading data from Xenia")

        default:
                d.Line = "Data"
                fmt.Println("In:", d.Line)
                return nil
        }
}

// Pillar is a system we need to store data into.
type Pillar struct {
        Host    string
        Timeout time.Duration
}

// Store knows how to store data into Pillar.
// We are using pointer semantics for consistency.
func (*Pillar) Store(d *Data) error {
        fmt.Println("Out:", d.Line)
        return nil
}

// System wraps Xenia and Pillar together into a single system.
// We have the API based on Xenia and Pillar. We want to build another API on top of this and use
// it as a foundation.
// One way is to have a type that have the behavior of being able to pull and store. We can do that
// through composition. System is based on the embedded value of Xenia and Pillar. And because of
// inner type promotion, System know how to pull and store.
type System struct {
        Xenia
        Pillar
}

// pull knows how to pull bulks of data from Xenia, leveraging the foundation that we have built.
// We don't need to add method to System to do this. There is no state inside System that we want
// the System to maintain. Instead, we want the System to understand the behavior.
// Functions are a great way of writing API because functions can be more readable than any method
// can. We always want to start with an idea of writing API from the package level with functions.
// When we write a function, all the input must be passed in. When we use a method, its signature
// doesn't indicate any level, what field or state that we are using on that value that we use to
// make the call.
func pull(x *Xenia, data []Data) (int, error) {
        // Range over the slice of data and share each element with the Xenial's Pull method.
        for i := range data {
                if err := x.Pull(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// store knows how to store bulks of data into Pillar.
// Similar to the function above.
// We might wonder if it is efficient. However, we are optimizing for correctness, not performance.
// When it is done, we will test it. If it is not fast enough, we will add more complexities to
// make it run faster.
func store(p *Pillar, data []Data) (int, error) {
        for i := range data {
                if err := p.Store(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// Copy knows how to pull and store data from the System.
// Now we can call the pull and store functions, passing Xenia and Pillar through.
func Copy(sys *System, batch int) error {
        data := make([]Data, batch)

        for {
                i, err := pull(&sys.Xenia, data)
                if i > 0 {
                        if _, err := store(&sys.Pillar, data[:i]); err != nil {
                                return err
                        }
                }

                if err != nil {
                        return err
                }
        }
}

func main() {
        sys := System{
                Xenia: Xenia{
                        Host:    "localhost:8000",
                        Timeout: time.Second,
                },
                Pillar: Pillar{
                        Host:    "localhost:9000",
                        Timeout: time.Second,
                },
        }

        if err := Copy(&sys, 3); err != io.EOF {
                fmt.Println(err)
        }
} }}

    - name: Build
      # Build your program with the given configuration. Note that --config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      run: cmake --build ${{ // -------------------------
// Decoupling With Interface
// -------------------------

// By looking at the API (functions), we need to decouple the API from the concrete implementation. The decoupling
// that we do must get all the way down into initialization. To do this right, the only piece of
// code that we need to change is initialization. Everything else should be able to act on the
// behavior that these types are gonna provide.

// pull is based on the concrete. It only knows how to work on Xenia. However, if we are able to
// decouple pull to use any system that know how to pull data, we can get the highest level of
// decoupling. Since the algorithm we have is already efficient, we don't need to add another level
// of generalization and destroy the work we did in the concrete. Same thing with store.

// It is nice to work from the concrete up. When we do this, not only we are solving problem
// efficiently and reducing technical debt but the contracts, they come to us. We already know what
// the contract is for pulling/storing data. We already validate that and this is what we need.

// Let's just decouple these 2 functions and add 2 interfaces. The Puller interface knows how to
// pull and the Storer knows how to store.
// Xenia already implemented the Puller interface and Pillar already implemented the Storer
// interface. Now we can come into pull/store, decouple this function from the concrete.
// Instead of passing Xenial and Pillar, we pass in the Puller and Storer. The algorithm doesn't
// change. All we doing is now calling pull/store indirectly through the interface value.

// Next step:
// ----------
// Copy also doesn't have to change because Xenia/Pillar already implemented the interfaces.
// However, we are not done because Copy is still bounded to the concrete. Copy can only work with
// pointer of type system. We need to decouple Copy so we can have a decoupled system that knows
// how to pull and store. We will do it in the next file.

package main

import (
        "errors"
        "fmt"
        "io"
        "math/rand"
        "time"
)

func init() {
        rand.Seed(time.Now().UnixNano())
}

// Data is the structure of the data we are copying.
type Data struct {
        Line string
}

// Puller declares behavior for pulling data.
type Puller interface {
        Pull(d *Data) error
}

// Storer declares behavior for storing data.
type Storer interface {
        Store(d *Data) error
}

// Xenia is a system we need to pull data from.
type Xenia struct {
        Host    string
        Timeout time.Duration
}

// Pull knows how to pull data out of Xenia.
func (*Xenia) Pull(d *Data) error {
        switch rand.Intn(10) {
        case 1, 9:
                return io.EOF

        case 5:
                return errors.New("Error reading data from Xenia")

        default:
                d.Line = "Data"
                fmt.Println("In:", d.Line)
                return nil
        }
}

// Pillar is a system we need to store data into.
type Pillar struct {
        Host    string
        Timeout time.Duration
}

// Store knows how to store data into Pillar.
func (*Pillar) Store(d *Data) error {
        fmt.Println("Out:", d.Line)
        return nil
}

// System wraps Xenia and Pillar together into a single system.
type System struct {
        Xenia
        Pillar
}

// pull knows how to pull bulks of data from any Puller.
func pull(p Puller, data []Data) (int, error) {
        for i := range data {
                if err := p.Pull(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// store knows how to store bulks of data from any Storer.
func store(s Storer, data []Data) (int, error) {
        for i := range data {
                if err := s.Store(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// Copy knows how to pull and store data from the System.
func Copy(sys *System, batch int) error {
        data := make([]Data, batch)

        for {
                i, err := pull(&sys.Xenia, data)
                if i > 0 {
                        if _, err := store(&sys.Pillar, data[:i]); err != nil {
                                return err
                        }
                }

                if err != nil {
                        return err
                }
        }
}

func main() {
        sys := System{
                Xenia: Xenia{
                        Host:    "localhost:8000",
                        Timeout: time.Second,
                },
                Pillar: Pillar{
                        Host:    "localhost:9000",
                        Timeout: time.Second,
                },
        }

        if err := Copy(&sys, 3); err != io.EOF {
                fmt.Println(err)
        }
} }} --config ${{ // ---------------------
// Interface Composition
// ---------------------

// Let's just add another interface. Let's use interface composition to do this.
// PullStorer has both behaviors: Puller and Storer. Any concrete type that implement both pull and
// store is a PullStorer. System is a PullStorer because it is embedded of these 2 types, Xenia and
// Pillar. Now we just need to go into Copy, replace the system pointer with PullStorer and no
// other code need to change.

// Looking closely at Copy, there is something that could potentially confuse us. We are passing
// the PullStorer interface value directly into pull and store respectively.
// If we look into pull and store, they don't want a PullStorer. One want a Puller and one want a
// Storer. Why does the compiler allow us to pass a value of different type value while it didn't
// allow us to do that before?
// This is because Go has what is called: implicit interface conversion.
// This is possible because:
// - All interface values have the exact same model (implementation details).
// - If the type information is clear, the concrete type that exists in one interface has enough
// behaviors for another interface. It is true that any concrete type that is stored inside of a
// PullStorer must also implement the Storer and Puller.

// Let's further look into the code.
// In the main function, we are creating a value of our System type. As we know, our System type 
// value is based on the embedding of two concrete types: Xenia and Pillar, where Xenia knows how
// to pull and Pillar knows how to store. 
// Because of inner type promotion, System knows how to pull and store both inherently.
// We are passing the address of our System to Copy. Copy then creates the PullStorer interface.
// The first word is a System pointer and the second word point to the original value. This
// interface now knows how to pull and store. When we call pull off of ps, we call pull off of
// System, which eventually call pull off of Xenia.
// Here is the kicker: the implicit interface conversion.
// We can pass the interface value ps to pull because the compiler knows that any concrete type
// stored inside the PullStorer must also implement Puller. We end up with another interface called
// Puller. Because the memory models are the same for all interfaces, we just copy those 2 words so
// they are all sharing the same interface type. Now when we call pull off of Puller, we call pull
// off of System. Similar to Storer.
// All using value semantic for the interface value and pointer semantic to share.

//        System                       ps
//  ------------------              ---------
// |  _______         |-pull       |         |-pull
// | |        |       |-store      | *System |-store
// | | Xenia  |-pull  |            |         |
// | |        |       |             ---------
// |  -------         |            |         |
// |  _______         |<-----------|    *    |
// | |        |       |            |         |
// | | Pillar |-store |             ---------               p                   s
// | |        |       |                                 ---------           ---------
// |  -------         |                                |         |-pull    |         |-store
// |                  |                                | *System |         | *System |
//  ------------------                                 |         |         |         |
//          A                                           ---------           ---------
//          |                                          |         |         |         |
//           ------------------------------------------|    *    | ------- |    *    |
//                                                     |         |         |         |
//                                                      ---------           ---------

// Next step:
// ----------
// Our system type is still concrete system type because it is still based on two concrete types,
// Xenial and Pillar. If we have another system, say Alice, we have to change in type System
// struct. This is not good. We will solve the last piece in the next file.

package main

import (
        "errors"
        "fmt"
        "io"
        "math/rand"
        "time"
)

func init() {
        rand.Seed(time.Now().UnixNano())
}

// Data is the structure of the data we are copying.
type Data struct {
        Line string
}

// Puller declares behavior for pulling data.
type Puller interface {
        Pull(d *Data) error
}

// Storer declares behavior for storing data.
type Storer interface {
        Store(d *Data) error
}

// PullStorer declares behaviors for both pulling and storing.
type PullStorer interface {
        Puller
        Storer
}

// Xenia is a system we need to pull data from.
type Xenia struct {
        Host    string
        Timeout time.Duration
}

// Pull knows how to pull data out of Xenia.
func (*Xenia) Pull(d *Data) error {
        switch rand.Intn(10) {
        case 1, 9:
                return io.EOF

        case 5:
                return errors.New("Error reading data from Xenia")

        default:
                d.Line = "Data"
                fmt.Println("In:", d.Line)
                return nil
        }
}

// Pillar is a system we need to store data into.
type Pillar struct {
        Host    string
        Timeout time.Duration
}

// Store knows how to store data into Pillar.
func (*Pillar) Store(d *Data) error {
        fmt.Println("Out:", d.Line)
        return nil
}

// System wraps Xenia and Pillar together into a single system.
type System struct {
        Xenia
        Pillar
}

// pull knows how to pull bulks of data from any Puller.
func pull(p Puller, data []Data) (int, error) {
        for i := range data {
                if err := p.Pull(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// store knows how to store bulks of data from any Storer.
func store(s Storer, data []Data) (int, error) {
        for i := range data {
                if err := s.Store(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// Copy knows how to pull and store data from any System.
func Copy(ps PullStorer, batch int) error {
        data := make([]Data, batch)

        for {
                i, err := pull(ps, data)
                if i > 0 {
                        if _, err := store(ps, data[:i]); err != nil {
                                return err
                        }
                }

                if err != nil {
                        return err
                }
        }
}

func main() {
        sys := System{
                Xenia: Xenia{
                        Host:    "localhost:8000",
                        Timeout: time.Second,
                },
                Pillar: Pillar{
                        Host:    "localhost:9000",
                        Timeout: time.Second,
                },
        }

        if err := Copy(&sys, 3); err != io.EOF {
                fmt.Println(err)
        }
} }}

    - name: Test
      working-directory: ${{ // -------------------------------------
// Decoupling With Interface Composition
// -------------------------------------

// We change our concrete type System. Instead of using two concrete types Xenia and Pillar, we
// use 2 interface types Puller and Storer. Our concrete type System where we can have concrete
// behaviors is now based on the embedding of 2 interface types. It means that we can inject any
// data, not based on the common DNA but on the data that providing the capability, the behavior
// that we need.
// Now our code can be fully decouplable because any value that implements the Puller interface can be stored
// inside the System (same with Storer interface). We can create multiple Systems and that data can
// be passed in Copy.
// We don't need method here. We just need one function that accept data and its behavior will
// change based on the data we put in.

// Now System is not based on Xenia and Pillar anymore. It is based on 2 interfaces, one that
// stores Xenia and one that stores Pillar. We get the extra layer of decoupling.
// If the system change, no big deal. We replace the system as we need to during the program
// startup.

// We solve this problem. We put this in production. Every single refactoring that we did went into
// production before we did the next one. We keep minimizing technical debt.

//        System                                ps
//  --------------------                      ---------
// |  _________         |-pull               |         |-pull
// | |         |        |-store              | *System |-store
// | | *Xenia  |-pull   |                    |         |
// | |         |        | <------------------ ---------
// |  ---------         |      p             |         |
// | |         |        |    -----           |    *    |
// | |    *    |------- |-> |     |-pull     |         |
// | |         |        |    -----            ---------
// |  ---------         |
// |
// |  __________        |
// | |          |       |
// | | * Pillar |-store |
// | |          |       |
// |  ----------        |      s
// | |          |       |    -----                         p                   s
// | |    *     |------ |-> |     |-store               ---------           ---------
// | |          |       |    -----                     |         |-pull    |         |-store
// |  ----------        |                              | *System |         | *System |
//  --------------------                               |         |         |         |
//          A                                           ---------           ---------
//          |                                          |         |         |         |
//           ------------------------------------------|    *    | ------- |    *    |
//                                                     |         |         |         |
//                                                      ---------           ---------

package main

import (
        "errors"
        "fmt"
        "io"
        "math/rand"
        "time"
)

func init() {
        rand.Seed(time.Now().UnixNano())
}

// Data is the structure of the data we are copying.
type Data struct {
        Line string
}

// Puller declares behavior for pulling data.
type Puller interface {
        Pull(d *Data) error
}

// Storer declares behavior for storing data.
type Storer interface {
        Store(d *Data) error
}

// PullStorer declares behaviors for both pulling and storing.
type PullStorer interface {
        Puller
        Storer
}

// Xenia is a system we need to pull data from.
type Xenia struct {
        Host    string
        Timeout time.Duration
}

// Pull knows how to pull data out of Xenia.
func (*Xenia) Pull(d *Data) error {
        switch rand.Intn(10) {
        case 1, 9:
                return io.EOF

        case 5:
                return errors.New("Error reading data from Xenia")

        default:
                d.Line = "Data"
                fmt.Println("In:", d.Line)
                return nil
        }
}

// Pillar is a system we need to store data into.
type Pillar struct {
        Host    string
        Timeout time.Duration
}

// Store knows how to store data into Pillar.
func (*Pillar) Store(d *Data) error {
        fmt.Println("Out:", d.Line)
        return nil
}

// System wraps Pullers and Stores together into a single system.
type System struct {
        Puller
        Storer
}

// pull knows how to pull bulks of data from any Puller.
func pull(p Puller, data []Data) (int, error) {
        for i := range data {
                if err := p.Pull(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// store knows how to store bulks of data from any Storer.
func store(s Storer, data []Data) (int, error) {
        for i := range data {
                if err := s.Store(&data[i]); err != nil {
                        return i, err
                }
        }

        return len(data), nil
}

// Copy knows how to pull and store data from any System.
func Copy(ps PullStorer, batch int) error {
        data := make([]Data, batch)

        for {
                i, err := pull(ps, data)
                if i > 0 {
                        if _, err := store(ps, data[:i]); err != nil {
                                return err
                        }
                }

                if err != nil {
                        return err
                }
        }
}

func main() {
        sys := System{
                Puller: &Xenia{
                        Host:    "localhost:8000",
                        Timeout: time.Second,
                },
                Storer: &Pillar{
                        Host:    "localhost:9000",
                        Timeout: time.Second,
                },
        }

        if err := Copy(&sys, 3); err != io.EOF {
                fmt.Println(err)
        }
} }}
      # Execute tests defined by the CMake configuration. Note that --build-config is needed because the default Windows generator is a multi-config generator (Visual Studio generator).
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest --build-config ${{ // --------------------
// Default error values
// --------------------

// Integrity matters. Nothing trumps integrity. Therefore, part of integrity is error handling.
// It is a big part of what we do everyday. It has to be a part of the main code.

// First, let's look at the language mechanic first on how the default error type is implemented.

package main

import "fmt"

// http://golang.org/pkg/builtin/#error
// This is pre-included in the language so it looks like an unexported type. It has one active behavior,
// which is Error returned a string.
// Error handling is decoupled because we are always working with error interface when we are
// testing our code.
// Errors in Go are really just values. We are going to valuate these through the decoupling of
// the interface. Decoupling error handling means that cascading changes will bubble up through the
// user application, causes cascading wide effect through the code base. It's important that we
// leverage the interface here as much as we can.
type error interface {
        Error() string
}

// http://golang.org/src/pkg/errors/errors.go
// This is the default concrete type that comes from the error package. It is an unexported type
// that has an unexported field. This gives us enough context to make us form a decision.
// We have responsibility around error handling to give the caller enough context to make them form
// a decision so they know how to handle this situation.
type errorString struct {
        s string
}

// http://golang.org/src/pkg/errors/errors.go
// This is using a pointer receiver and returning a string.
// If the caller must call this method and parse a string to see what is going on then we fail.
// This method is only for logging information about the error.
func (e *errorString) Error() string {
        return e.s
}

// http://golang.org/src/pkg/errors/errors.go
// New returns an error interface that formats as the given text.
// When we call New, what we are doing is creating errorString value, putting some sort of string
// in there.. Since we are returning the address of a concrete type, the user will get an error
// interface value where the first word is a *errorString and the second word points to the
// original value. We are going to stay decoupled during the error handling.
//       error
//  --------------
// | *errorString |      errorString
//  --------------       -----------
// |      *       | --> |   "Bad"   |
//  --------------       -----------
func New(text string) error {
        return &errorString{text}
}

func main() {
        // This is a very traditional way of error handling in Go.
        // We are calling webCall and return the error interface and store that in a variable.
        // nil is a special value in Go. What "error != nil" actually means is that we are asking if
        // there is a concrete type value that is stored in error type interface. Because if error is
        // not nil, there is a concrete value stored inside. If it is the case, we've got an error.
        // Now do we handle the error, do we return the error up the call stack for someone else to
        // handle? We will talk about this latter.
        if err := webCall(); err != nil {
                fmt.Println(err)
                return
        }

        fmt.Println("Life is good")
}

// webCall performs a web operation.
func webCall() error {
        return New("Bad Request")
} }}
