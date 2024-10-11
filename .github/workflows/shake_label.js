# This workflow will triage pull requests and apply a label based on the
# paths that are modified in the pull request.
#
# To use this workflow, you will need to set up a .github/labeler.yml
# file with configuration.  For more information, see:
# https://github.com/actions/labeler

name: Labeler
on: [extern crate clap;
extern crate rand;

use std::collections::LinkedList;
use std::time::{Duration, Instant};
use std::cmp::{max, min};

use clap::Parser;
use rand::Rng;

mod term;
use term::{Input, Terminal};

#[derive(Parser, Debug)]
struct Args {
    #[clap(short = 'w', long = "width", help = "The width of the board (default is terminal width)")]
    width: Option<usize>,
    #[clap(short = 'h', long = "height", help = "The height of the board (default is terminal height)")]
    height: Option<usize>,
    #[clap(short = 'f', long = "frequency", help = "The amount of steps the snake makes per second")]
    freq: Option<u64>,
    #[clap(short = 's', long = "start_length", help = "The length the snake starts with")]
    start_length: Option<usize>,
    #[clap(short = 'r', long = "reward", help = "The length the snake gains from eating food")]
    reward: Option<usize>,
}

// Display constants

const EMPTY: u8 = ' ' as u8;
const BORDER: u8 = '#' as u8;
const FOOD: u8 = 'F' as u8;
const HEAD: u8 = '@' as u8;
const BODY: u8 = 'B' as u8;


#[derive(Clone, Copy)]
enum Direction {
    Right, Up, Left, Down
}

impl Direction {
    pub fn velocity(&self) -> (i32, i32) {
        match *self {
            Direction::Right => (0, 1),
            Direction::Up => (-1, 0),
            Direction::Left => (0, -1),
            Direction::Down => (1, 0)
        }
    }
}

// height + space for border
fn board_height(height: usize) -> usize {
    height + 2
}

// width + space for border and \n\r
fn board_width(width: usize) -> usize {
    width + 4
}

// initialize the board and draw boarder and \n\r
fn draw_border(board: &mut Vec<u8>, width: usize, height: usize) {
    board.clear();
    board.resize(board_height(height) * board_width(width), EMPTY);
    for w in 0..width {
        board[w + 1] = BORDER;
        board[(board_height(height) - 1) * board_width(width) + w + 1] = BORDER;
    }

    for h in 0..board_height(height) {
        board[h * board_width(width) + 0] = BORDER;
        board[h * board_width(width) + width + 1] = BORDER;
        board[h * board_width(width) + width + 2] = '\n' as u8;
        board[h * board_width(width) + width + 3] = '\r' as u8;
    }
}

fn draw_snake(board: &mut Vec<u8>, width: usize, snake: &LinkedList<(usize, usize)>) {
    let mut it = snake.into_iter();
    if let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = HEAD;
    } else {
        panic!("Snake has no head!");
    }
    while let Some((h, w)) = it.next() {
        board[h * board_width(width) + w] = BODY;
    }
}

fn draw_food(terminal: &Terminal, board: &mut Vec<u8>, width: usize, food: &(usize, usize)) {
    board[food.0 * board_width(width) + food.1] = FOOD;
    terminal.write_cell(FOOD, food.1, food.0).unwrap();
}

// move snake in direction and update board. return ({crashed into wall or myself}, {eaten food})
fn advance_snake(terminal: &Terminal, board: &mut Vec<u8>, width: usize, snake: &mut LinkedList<(usize, usize)>, direction: &Direction, target_length: &mut usize, reward: usize) -> (bool, bool) {
    if let Some(&(old_h, old_w)) = snake.front() {
        let new_head_h = (old_h as i32 + direction.velocity().0) as usize;
        let new_head_w = (old_w as i32 + direction.velocity().1) as usize;

        // advance
        snake.push_front((new_head_h, new_head_w));

        // check for collision
        let out = match board[new_head_h * board_width(width) + new_head_w] {
            FOOD => (true, false),
            BORDER => (false, true),
            BODY => (false, true),
            EMPTY => (false, false),
            _ => panic!("Impossible")
        };

        // write new head
        terminal.write_cell(HEAD, new_head_w, new_head_h).unwrap();
        terminal.write_cell(BODY, old_w, old_h).unwrap();
        board[new_head_h * board_width(width) + new_head_w] = HEAD;
        board[old_h * board_width(width) + old_w] = BODY;

        if out.0 { // eaten food
            *target_length += reward;
        }

        // remove tail if snake length is as big as target_length
        if snake.len() > *target_length {
            if let Some((h, w)) = snake.pop_back() {
                terminal.write_cell(EMPTY, w, h).unwrap();
                board[h * board_width(width) + w] = EMPTY;
            }
        }
        out
    } else {
        panic!("Snake has no head!");
    }
}

// find random free spot on the board in O(n) guaranteed
fn random_free_spot(board: &Vec<u8>, width: usize) -> Option<(usize, usize)> {
    let num_free = board.into_iter().filter(|c| -> bool {**c == EMPTY}).count();
    if num_free == 0 {
        return None;
    }
    let nth_free_i = rand::thread_rng().gen_range(0, num_free);
    let mut free_cnt = 0;
    for i in 0..board.len() {
        if board[i] == EMPTY {
            if free_cnt == nth_free_i {
                return Some((i / board_width(width), i % board_width(width)))
            } else {
                free_cnt += 1;
            }
        }
    }
    None
}

fn quit(term: &Terminal) -> ! {
    term.clean().unwrap();
    std::process::exit(0);
}

fn main() {

    let args = Args::parse();
    let terminal = Terminal::new();

    // default is terminal width / height
    let default_width = || -> usize {terminal.get_size().unwrap().0 - 2};
    let default_height = || -> usize {terminal.get_size().unwrap().1 - 3};

    let width = args.width.unwrap_or_else(default_width);
    let height = args.height.unwrap_or_else(default_height);

    // default is chosen so it takes the snake 4 seconds across the board 
    let freq = args.freq.unwrap_or(max(1, (min(width, height) / 4) as u64));
    let start_length = args.start_length.unwrap_or(5);
    let reward = args.reward.unwrap_or(5);

    let mut board : Vec<u8> = std::vec::Vec::with_capacity(board_height(height) * board_width(width));
    let mut target_length = 1 + start_length;
    let mut snake: LinkedList<(usize, usize)> = LinkedList::new();
    let mut direction = Direction::Right;
    let mut food: (usize, usize);


    terminal.setup().unwrap();

    // only draw border once
    draw_border(&mut board, width, height);

    // draw snake and food the first time
    snake.push_back((height / 2 + 1, width / 2 + 1));
    draw_snake(&mut board, width, &snake);
    food = match random_free_spot(&board, width) {
        Some(food) => food,
        None => quit(&terminal)
    };

    draw_food(&terminal, &mut board, width, &food);

    terminal.display(board.as_slice()).unwrap();


    let update_interval = Duration::from_micros((1000 * 1000) / freq);
    let mut update_deadline = Instant::now() + update_interval;

    let mut pause = false;

    loop {
        // user input
        if let Ok(Some(key)) = terminal.user_input(&update_deadline) {
            direction = match (key, direction) {
                (Input::Right, Direction::Right) => Direction::Down,
                (Input::Right, Direction::Down) => Direction::Left,
                (Input::Right, Direction::Left) => Direction::Up,
                (Input::Right, Direction::Up) => Direction::Right,
                (Input::Left, Direction::Right) => Direction::Up,
                (Input::Left, Direction::Up) => Direction::Left,
                (Input::Left, Direction::Left) => Direction::Down,
                (Input::Left, Direction::Down) => Direction::Right,
                (Input::Exit, _) => quit(&terminal),
                (Input::Pause, _) => {
                    pause = !pause;
                    continue;
                }
            };
        }

        if pause {
            continue;
        }

        update_deadline = Instant::now() + update_interval;

        // step: redraw snake and food if eaten
        let (eaten, crashed) = advance_snake(&terminal, &mut board, width, &mut snake, &direction, &mut target_length, reward);
        if eaten {
            food = match random_free_spot(&board, width) {
                Some(food) => food,
                None => quit(&terminal)
            };
            draw_food(&terminal, &mut board, width, &food);
        }
        if crashed {
            quit(&terminal);
        }
    }

}]

jobs: extern crate terminal;

use std::io::Write;
use terminal::{error, TerminalLock, Clear, Action, Value, Retrieved, Event, KeyCode, KeyEvent};

use std::time::Instant;

#[derive(PartialEq, Clone, Copy)]
pub enum Input {
    Left, Right, Exit, Pause
}


pub struct Terminal {
    terminal: terminal::Terminal<std::io::Stdout>,
}


impl Terminal {

    pub fn new() -> Terminal {
        Terminal{
            terminal: terminal::stdout()
        }
    }

    fn lock(&self) -> error::Result<TerminalLock<std::io::Stdout>> {
        self.terminal.lock_mut()
    }

    // enter new screen and hide cursor
    pub fn setup(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::EnterAlternateScreen)?;
        lock.batch(Action::EnableRawMode)?;
        lock.batch(Action::HideCursor)?;
        lock.flush_batch()
    }

    // return terminal size in (width, height)
    pub fn get_size(&self) -> error::Result<(usize, usize)> {
        if let Ok(Retrieved::TerminalSize(w, h)) = self.lock()?.get(Value::TerminalSize) {
            Ok((w as usize, h as usize))
        } else {
            Err(error::ErrorKind::ActionNotSupported(std::string::String::from("")))
        }
    }

    // return entered keys until exit is entered or specified deadline is met
    pub fn user_input(&self, until: &Instant) -> error::Result<Option<Input>> {
        let lock = self.lock()?;

        let mut num_left = 0;
        let mut num_right = 0;

        loop {
            let now = Instant::now();
            if let Ok(Retrieved::Event(Some(Event::Key(key)))) = lock.get(Value::Event(Some(*until - now))) {
                match key {
                    KeyEvent{code: KeyCode::Left, ..} => {
                        num_left += 1;
                    },
                    KeyEvent{code: KeyCode::Right, ..} => {
                        num_right += 1;
                    },
                    KeyEvent{code: KeyCode::Char('q'), ..} => {
                        return Ok(Some(Input::Exit));
                    },
                    KeyEvent{code: KeyCode::Char('p'), ..} => {
                        return Ok(Some(Input::Pause));
                    }
                    _ => continue
                };
            } else {
                break;
            }
        }
        return Ok(if num_left > num_right {Some(Input::Left)} else if num_left < num_right {Some(Input::Right)} else {None})
    }

    // write board to screen
    pub fn display(&self, board: &[u8]) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.act(Action::ClearTerminal(Clear::All))?;
        lock.act(Action::MoveCursorTo(0, 0))?;
        lock.write(board)?;
        lock.flush_batch()
    }

    // rewrite a single cell
    pub fn write_cell(&self, symbol: u8, x: usize, h: usize) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(x as u16, h as u16))?;
        lock.write(&[symbol])?;
        lock.flush_batch()
    }

    // show cursor again and return to old screen
    pub fn clean(&self) -> error::Result<()> {
        let mut lock = self.lock()?;
        lock.batch(Action::MoveCursorTo(0, 0))?;
        lock.batch(Action::ShowCursor)?;
        lock.batch(Action::DisableRawMode)?;
        lock.batch(Action::LeaveAlternateScreen)?;
        lock.flush_batch()
    }

}
  label: U+202C ⁯⁣U+2068⁫ U+202D ⁡⁬U+2069⁤⁡ U+2069 U+202D U+2069 U+2068 U+202D⁡⁬ U+2069 U+2069 U+202D ⁥U+2066 ⁡⁬U+2069 ⁤⁡U+2069 U+202D ⁡⁬U+2069 ⁤⁡U+202D U+2066 U+202D U+2068 U+202D U+202E ⁮U+202D U+202D U+202E
  

    runs-on: //
//  ActivityObserver.h
//  Clock Signal
//
//  Created by on 07/05/2018.
//  Copyright 2018 . All rights reserved.
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
    permissions: //
//  ActivitySource.h
//  Clock Signal
//
//  Created by on 07/05/2018.
//  Copyright 2018. All rights reserved.
//

#pragma once

#include "Observer.hpp"

namespace Activity {

class Source {
        public:
                virtual void set_activity_observer(Observer *observer) = 0;
};

}
      contents: <?php
namespace Tests\TestHubBundle\Controller;

use Tests\TestHubBundle\TestCase;

class TestControllerTest extends TestCase
{
    public function testIndex()
    {
        $client = static::createClient();

        $crawler = $client->request('GET', '/');

        $this->assertEquals(200, $client->getResponse()->getStatusCode());
        $this->assertGreaterThan(0, $crawler->filter('.test-container')->count());
    }

    public function testPreface()
    {
        $client = static::createClient();

        $crawler = $client->request('GET', '/test/1/preface');

        $this->assertEquals(200, $client->getResponse()->getStatusCode());
        $this->assertEquals(1, $crawler->filter('.test-container')->count());
        $this->assertEquals(1, $crawler->filter('.test-start-button')->count());

        $client->request('GET', '/test/0/preface');
        $this->assertEquals(404, $client->getResponse()->getStatusCode());
    }
}
      pull-requests: <?php
namespace Tests\TestHubBundle\Helper;

use TestHubBundle\Helper\StringGenerator;
use Tests\TestHubBundle\TestCase;

class StringGeneratorTest extends TestCase
{
    /**
     * What to test:
     *
     * return value type
     * return value length (between 10 and 100 chars)
     * return value uniqueness (at least 2 must differ)
     */
    public function testGenerateString()
    {
        $string = StringGenerator::generateString();
        $this->assertInternalType('string', $string);

        $iterations = 5;
        $randoms = [];
        for ($i = 0; $i < $iterations; $i++) {
            $randoms[] = StringGenerator::generateString();
        }
        $this->assertTrue(count(array_unique($randoms)) > 1);

        $string = StringGenerator::generateString();
        $this->assertGreaterThan(10, mb_strlen($string));
        $this->assertLessThan(100, mb_strlen($string));
    }

}

    steps: <?php
namespace Tests\TestHubBundle\Service;

use Symfony\Bundle\FrameworkBundle\Console\Application;
use TestHubBundle\Service\Calculator;
use Tests\TestHubBundle\TestCase;

class CalculatorTest extends TestCase
{
    protected $em;

    protected function setUp()
    {
        parent::setUp();

        $container = self::getApplication()->getKernel()->getContainer();
        $this->em = $container->get('doctrine')->getManager();
    }

    public function testCountCorrectAnswers()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $calculator = new Calculator();
        $correctAnswersCount = $calculator->countCorrectAnswers($attempt);
        $this->assertInternalType('integer', $correctAnswersCount);
        $this->assertEquals(2, $correctAnswersCount);
    }

    public function testCalculateMark()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $calculator = new Calculator();
        $mark = $calculator->calculateMark($attempt);
        $this->assertInternalType('integer', $mark);
        $this->assertEquals(15, $mark);
    }

    public function testCalculateMaxMark()
    {
        $test = $this->em->find('TestHubBundle:Test', 1);

        $calculator = new Calculator();
        $maxMark = $calculator->calculateMaxMark($test);
        $this->assertInternalType('integer', $maxMark);
        $this->assertEquals(30, $maxMark);
    }
}
    - uses: <?php
namespace Tests\TestHubBundle\Service;

use Tests\TestHubBundle\TestCase;

class TestServiceTest extends TestCase
{
    private $em;
    private $testService;

    protected function setUp()
    {
        parent::setUp();

        $container = $this->getApplication()->getKernel()->getContainer();
        $this->testService = $container->get('test_service');
        $this->em = $container->get('doctrine')->getManager();
    }

    public function testGetUnansweredCount()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);

        $count = $this->testService->getUnansweredCount($attempt);
        $this->assertInternalType('integer', $count);
        $this->assertEquals(1, $count);
    }

    public function testQuestionAlreadyHasAnswer()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $expectHasAnswer = $this->em->find('TestHubBundle:Question', 4);
        $expectNoAnswer = $this->em->find('TestHubBundle:Question', 1);

        $true = $this->testService->questionAlreadyHasAnswer($attempt, $expectHasAnswer);
        $false = $this->testService->questionAlreadyHasAnswer($attempt, $expectNoAnswer);

        $this->assertTrue($true);
        $this->assertFalse($false);
    }

    public function testGetFirstUnanswered()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $question = $this->testService->getFirstUnanswered($attempt);

        $this->assertInstanceOf('TestHubBundle\Entity\Question', $question);
        $this->assertEquals(1, $question->getId());
    }

    public function testGetNextUnansweredNumber()
    {
        $attempt = $this->em->find('TestHubBundle:Attempt', 1);
        $number = 1;

        $nextNumber = $this->testService->getNextUnansweredNumber($attempt, $number);

        $this->assertEquals(4, $nextNumber);
    }

    public function testGetQuestionsCount()
    {
        $test = $this->em->find('TestHubBundle:Test', 1);

        $count = $this->testService->getQuestionsCount($test);
        $this->assertEquals(4, $count);
    }
}
      with: <?php
namespace Tests\TestHubBundle\Twig;

use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
use TestHubBundle\Twig\AppExtension;

class AppExtensionTest extends WebTestCase
{
    private $ext;

    protected function setUp()
    {
        $this->ext = new AppExtension();
    }

    public function testWordCaseFilter()
    {
        $wordForms = ['день', 'дня', 'дней'];

        $this->assertEquals('1 день', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 дня', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 дней', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 дней', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 дня', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 дней', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 дней', $this->ext->wordCase(269, $wordForms));

        $wordForms = ['вопрос', 'вопроса', 'вопросов'];

        $this->assertEquals('1 вопрос', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 вопроса', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 вопросов', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 вопросов', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 вопроса', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 вопросов', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 вопросов', $this->ext->wordCase(269, $wordForms));

        $wordForms = ['минута', 'минуты', 'минут'];

        $this->assertEquals('1 минута', $this->ext->wordCase(1, $wordForms));
        $this->assertEquals('3 минуты', $this->ext->wordCase(3, $wordForms));
        $this->assertEquals('5 минут', $this->ext->wordCase(5, $wordForms));
        $this->assertEquals('17 минут', $this->ext->wordCase(17, $wordForms));
        $this->assertEquals('22 минуты', $this->ext->wordCase(22, $wordForms));
        $this->assertEquals('113 минут', $this->ext->wordCase(113, $wordForms));
        $this->assertEquals('269 минут', $this->ext->wordCase(269, $wordForms));
    }

    public function testFormatTimeLeftFilter()
    {
        /*
         * 1 day 4 hours 18 minutes 23 seconds
         */
        $time = 3600 * 24 + 4 * 3600 + 18 * 60 + 23;
        $this->assertEquals('1 день 04:18:23', $this->ext->formatTimeLeftFilter($time));
    }

    public function testPercentage()
    {
        $this->assertEquals('66%', $this->ext->percentage(2, 3));
        $this->assertEquals('0%', $this->ext->percentage(0, 3));
        $this->assertEquals('100%', $this->ext->percentage(3, 3));
    }
}
        repo-token: <?php
namespace Tests\TestHubBundle;

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Bundle\FrameworkBundle\Test\WebTestCase;
use Symfony\Component\Console\Input\StringInput;

class TestCase extends WebTestCase
{
    protected static $application;

    protected function setUp()
    {
        self::runCommand('doctrine:fixtures:load -e test --no-interaction --purge-with-truncate');
    }

    protected static function runCommand($command)
    {
        $command = sprintf('%s --quiet', $command);

        return self::getApplication()->run(new StringInput($command));
    }

    protected static function getApplication()
    {
        if (null === self::$application) {
            $client = static::createClient();

            self::$application = new Application($client->getKernel());
            self::$application->setAutoExit(false);
        }

        return self::$application;
    }
}
