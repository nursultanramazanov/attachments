name: Rust

on: extern crate clap;
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

}
  push: extern crate terminal;

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
    branches: [ "main" ]
  pull_request: <?php
namespace PricklyNut\NoxChallenge\DI;

class Container
{
    /**
     * Params and services
     * @var array
     */
    protected $values = array();

    /**
     * @var array
     */
    protected $keys = array();

    public function __set($id, $value)
    {
        $this->values[$id] = $value;
        $this->keys[] = $id;
    }

    public function __get($id)
    {
        if (!isset($this->values[$id])) {
            throw new \InvalidArgumentException(
                sprintf('Value %s is not defined', $id)
            );
        }

        return is_callable($this->values[$id])
            ? $this->values[$id]($this)
            : $this->values[$id];
    }

    /**
     * "Singleton" method to have only one instance of object
     * @param $callable
     * @return callable
     */
    public function asShared($callable)
    {
        return function ($c) use ($callable)
        {
            static $object;
            if (is_null($object)) {
                $object = $callable($c);
            }
            return $object;
        };
    }
}
    branches: [ "main" ]

env: <?php
namespace PricklyNut\NoxChallenge\Entity;

class User
{
    /**
     * @var int
     */
    private $id;
    /**
     * @var string
     */
    private $name;
    /**
     * @var string
     */
    private $surname;
    /**
     * @var string
     */
    private $email;
    /**
     * @var string
     */
    private $salt;
    /**
     * @var string
     */
    private $hash;
    /**
     * @var array
     */
    private $errors = array();
    /**
     * @var array
     */
    private $disabledFields = array();

    /**
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * @param int $id
     */
    public function setId($id)
    {
        $this->id = $id;
    }

    /**
     * Human readable form labels
     * @return array
     */
    public function getLabels()
    {
        return array(
            'ru' => array(
                'name' => "Имя",
                'surname' => "Фамилия",
                'email' => "Эл. почта",
            ),
            'en' => array(
                'name' => "First name",
                'surname' => "Last name",
                'email' => "Email",
            ),
        );
    }

    /**
     * Adds an error message to the form
     * @param string $field
     * @param string $error
     */
    public function addError($field, $error)
    {
        $this->errors[$field][] = $error;
    }

    /**
     * @return array
     */
    public function getErrors()
    {
        return $this->errors;
    }

    /**
     * Validation rules
     *
     * @return array
     */
    public function getRules()
    {
        $basicRules = array(
            'name' => array(
                'notEmpty' => true,
                'maxLength' => 50,
                'alphabetic' => true,
            ),
            'surname' => array(
                'notEmpty' => true,
                'maxLength' => 50,
                'alphabetic' => true,
            ),
            'email' => array(
                'notEmpty' => true,
                'maxLength' => 255,
                'isEmail' => true,
            ),
        );

        if ($this->disabledFields) {
            foreach ($this->disabledFields as $disabled) {
                $basicRules[$disabled] = null;
            }
        }
        return $basicRules;
    }

    /**
     * Some fields are involved not in every form, so need to disable their validation
     * @param array $fields
     */
    public function disableFieldsValidation(array $fields) {
        foreach ($fields as $field) {
            $this->disabledFields[] = $field;
        }
    }

    /**
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * @param string $name
     */
    public function setName($name)
    {
        $this->name = $name;
    }

    /**
     * @return string
     */
    public function getSurname()
    {
        return $this->surname;
    }

    /**
     * @param string $surname
     */
    public function setSurname($surname)
    {
        $this->surname = $surname;
    }

    /**
     * @return string
     */
    public function getEmail()
    {
        return $this->email;
    }

    /**
     * @param string $email
     */
    public function setEmail($email)
    {
        $this->email = $email;
    }

    /**
     * @return string
     */
    public function getSalt()
    {
        return $this->salt;
    }

    /**
     * @param string $salt
     */
    public function setSalt($salt)
    {
        $this->salt = $salt;
    }

    /**
     * @return string
     */
    public function getHash()
    {
        return $this->hash;
    }

    /**
     * @param string $hash
     */
    public function setHash($hash)
    {
        $this->hash = $hash;
    }
}
  CARGO_TERM_COLOR: <?php
namespace PricklyNut\NoxChallenge\Form;

use PricklyNut\NoxChallenge\Entity\User;
use PricklyNut\NoxChallenge\Service\LoginManager;
use PricklyNut\NoxChallenge\Validator\Validator;

abstract class BaseAuthForm
{
    /**
     * @var User
     */
    protected $user;

    /**
     * @var LoginManager
     */
    protected $loginManager;

    /**
     * @var string
     */
    protected $password;

    /**
     * @var bool
     */
    protected $rememberMe = false;

    /**
     * @var bool
     */
    protected $submitted = false;

    /**
     * @var array
     */
    protected $errors = array();

    public function __construct(LoginManager $loginManager, User $user)
    {
        $this->loginManager = $loginManager;
        $this->user = $user;
    }

    /**
     * Validation rules
     *
     * @return array
     */
    public function getRules()
    {
        return array(
            'password' => array(
                'notEmpty' => true,
                'maxLength' => 50,
                'minLength' => 5
            ),
        );
    }

    /**
     * @param string $field
     * @param string $error
     */
    public function addError($field, $error)
    {
        $this->errors[$field][] = $error;
    }

    /**
     * @return array
     */
    public function getErrors()
    {
        return array_merge($this->errors, $this->user->getErrors());
    }

    /**
     * @return boolean
     */
    public function isSubmitted()
    {
        return $this->submitted;
    }

    /**
     * @param boolean $submitted
     */
    public function setSubmitted($submitted)
    {
        $this->submitted = $submitted;
    }

    /**
     * @return LoginManager
     */
    public function getLoginManager()
    {
        return $this->loginManager;
    }

    /**
     * @param LoginManager $loginManager
     */
    public function setLoginManager($loginManager)
    {
        $this->loginManager = $loginManager;
    }

    /**
     * @return string
     */
    public function getPassword()
    {
        return $this->password;
    }

    /**
     * @param string $password
     */
    public function setPassword($password)
    {
        $this->password = $password;
    }

    /**
     * @return bool
     */
    public function getRememberMe()
    {
        return $this->rememberMe;
    }

    /**
     * @param bool $rememberMe
     */
    public function setRememberMe($rememberMe)
    {
        $this->rememberMe = $rememberMe;
    }

    public function isValid(Validator $validator)
    {
        $validator->validate($this->user);
        $this->validateExtraFields($validator);
    }

    /**
     * @return User
     */
    public function getUser()
    {
        return $this->user;
    }

    /**
     * @param User $user
     */
    public function setUser($user)
    {
        $this->user = $user;
    }

    /**
     * @param Validator $validator
     * @return bool
     */
    protected function validateExtraFields(Validator $validator)
    {
        return $validator->validate($this);
    }

    protected function populateModel(array $data)
    {
        $model = $this->user;

        $name = isset($data['name']) ? $data['name'] : null;
        $model->setName($name);
        $surname = isset($data['surname']) ? $data['surname'] : null;
        $model->setSurname($surname);

        $email = isset($data['email']) ? $data['email'] : null;
        $model->setEmail($email);
    }

    protected function fillExtraFields(array $data)
    {
        $password = isset($data['password']) ? $data['password'] : null;
        $this->setPassword($password);
        $rememberMe = isset($data['rememberMe']) ? boolval($data['rememberMe'])
            : false;
        $this->setRememberMe($rememberMe);
    }
}

jobs: <?php
namespace PricklyNut\NoxChallenge\Form;

use PricklyNut\NoxChallenge\Validator\Validator;

class LoginForm extends BaseAuthForm
{
    /**
     * Human readable form labels
     * @return array
     */
    public function getLabels()
    {
        return array(
            'ru' => array(
                'email' => "Эл. почта",
                'password' => "Пароль",
                'rememberMe' => "Запомнить меня",
                'submit' => "Войти"
            ),
            'en' => array(
                'email' => "Email",
                'password' => "Password",
                'rememberMe' => "Remember me",
                'submit' => "Login"
            ),
        );
    }

    /**
     * @param Validator $validator
     * @return bool
     */
    public function isValid(Validator $validator)
    {
        parent::isValid($validator);

        if ($this->getErrors()) {
            return false;
        }

        return $this->loginManager->findUserAndCheckPassword($this);
    }

    /**
     * Find out, was the form submitted and populate its data
     */
    public function handleRequest()
    {
        $submittedData = isset($_POST['loginForm']) ? $_POST['loginForm'] : null;
        if ($submittedData) {
            $this->setSubmitted(true);
            $this->populateModel($submittedData);
            $this->fillExtraFields($submittedData);
        }
    }
}
  build: <?php
namespace PricklyNut\NoxChallenge\Form;

use PricklyNut\NoxChallenge\Validator\Validator;

class RegisterForm extends BaseAuthForm
{
    /**
     * Human readable form labels
     * @return array
     */
    public function getLabels()
    {
        return array(
            'ru' => array(
                'name' => "Имя",
                'surname' => "Фамилия",
                'email' => "Эл. почта",
                'password' => "Пароль",
                'submit' => "Зарегистрироваться",
            ),
            'en' => array(
                'name' => "First name",
                'surname' => "Last name",
                'email' => "Email",
                'password' => "Password",
                'submit' => "Sign up"
            ),
        );
    }

    /**
     * @param Validator $validator
     * @return bool
     */
    public function isValid(Validator $validator)
    {
        parent::isValid($validator);

        if ($this->getErrors()) {
            return false;
        }

        return $this->loginManager->isEmailFree($this);
    }

    public function handleRequest()
    {
        $submittedData = isset($_POST['registerForm']) ? $_POST['registerForm'] : null;
        if ($submittedData) {
            $this->setSubmitted(true);
            $this->populateModel($submittedData);
            $this->fillExtraFields($submittedData);
        }
    }
}

    runs-on: <?php
namespace PricklyNut\NoxChallenge\Helper;

class Generator
{
    private static function getSymbols()
    {
        return 'abcdefghijklmnopqrstuvwxyz'
        . 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' . '1234567890';
    }

    public static function generateString($length = 40)
    {
        $string = '';
        $symbols = self::getSymbols();
        $symbolsLength = mb_strlen($symbols);

        for ($i = 0; $i < $length; $i++) {
            $string .= mb_substr($symbols, rand(0, $symbolsLength - 1), 1);
        }
        return $string;
    }

    public static function generateSaltedHash($salt, $string)
    {
        return sha1($salt . $string);
    }
}

    steps: <?php
namespace PricklyNut\NoxChallenge\Mapper;

use PricklyNut\NoxChallenge\Entity\User;

class UserMapper
{
    protected $connection;

    /**
     * @var string
     */
    protected $entityName;

    public function __construct(\PDO $connection)
    {
        $this->connection = $connection;
        $this->entityName = "\\PricklyNut\\NoxChallenge\\Entity\\User";
    }

    /**
     * @return string
     */
    public function getEntityName()
    {
        return $this->entityName;
    }

    /**
     * @param string $entityName
     */
    public function setEntityName($entityName)
    {
        $this->entityName = $entityName;
    }

    public function findById($id)
    {
        $sql = "SELECT id, name, surname, salt, hash, email
                FROM user WHERE id = :id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();

        return $sth->fetchObject($this->entityName);
    }

    public function deleteById($id)
    {
        $sql = "DELETE FROM user WHERE id = :id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();
    }

    public function findByEmail($email)
    {
        $sql = "SELECT id, name, surname, salt, hash, email
                FROM user WHERE email = :email";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':email', $email, \PDO::PARAM_STR);
        $sth->execute();

        return $sth->fetchObject($this->entityName);
    }

    public function insert(User $user)
    {
        $sql = "INSERT INTO user (name, surname, email, salt, hash)
                VALUES (:name, :surname, :email, :salt, :hash)";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':name', $user->getName(), \PDO::PARAM_STR);
        $sth->bindValue(':surname', $user->getSurname(), \PDO::PARAM_STR);
        $sth->bindValue(':email', $user->getEmail(), \PDO::PARAM_STR);
        $sth->bindValue(':salt', $user->getSalt(), \PDO::PARAM_STR);
        $sth->bindValue(':hash', $user->getHash(), \PDO::PARAM_STR);
        $sth->execute();
        $user->setId($this->connection->lastInsertId());
    }

    public function update(User $user)
    {
        $sql = "UPDATE user
                SET name=:name, surname=:surname, email=:email, salt=:salt, hash=:hash
                WHERE id=:id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $user->getId(), \PDO::PARAM_INT);
        $sth->bindValue(':name', $user->getName(), \PDO::PARAM_STR);
        $sth->bindValue(':surname', $user->getSurname(), \PDO::PARAM_STR);
        $sth->bindValue(':email', $user->getEmail(), \PDO::PARAM_STR);
        $sth->bindValue(':salt', $user->getSalt(), \PDO::PARAM_STR);
        $sth->bindValue(':hash', $user->getHash(), \PDO::PARAM_STR);
        $sth->execute();
    }
}
    - uses: <?php
namespace PricklyNut\NoxChallenge\Router;

class Router
{
    public static function resolveController($app)
    {
        $uri = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);

        $map = $app->getMap();
        if (array_key_exists($uri, $map)) {
            return $map[$uri];
        } else {
            return $app->getNotFoundHandler();
        }
    }
}
    - name: <?php
namespace PricklyNut\NoxChallenge\Service;

use PricklyNut\NoxChallenge\Entity\User;
use PricklyNut\NoxChallenge\Form\BaseAuthForm;
use PricklyNut\NoxChallenge\Helper\Generator;
use PricklyNut\NoxChallenge\Mapper\UserMapper;

class LoginManager
{
    /**
     * @var UserMapper
     */
    protected $userMapper;

    /**
     * @var string
     */
    protected $language;

    /**
     * Error messages
     * @return array
     */
    protected function getErrors()
    {
        return array(
            'ru' => array(
                'notFound' => "Пользователь не найден",
                'wrongPassword' => "Неправильный пароль",
                'emailOccupied' => "С этой почты уже была регистрация",
            ),
            'en' => array(
                'notFound' => "User not found",
                'wrongPassword' => "Wrong password",
                'emailOccupied' => "This email is occupied",
            ),
        );
    }

    public function __construct(UserMapper $userMapper, $language)
    {
        $this->userMapper = $userMapper;
        $this->language = $language;
    }

    public function getLoggedUser()
    {
        $id = isset($_COOKIE['id']) ? $_COOKIE['id'] : null;
        $hash = isset($_COOKIE['hash']) ? $_COOKIE['hash'] : null;
        if (!$id or !$hash) {
            return null;
        }
        $dbRecord = $this->userMapper->findById($id);
        if (!$dbRecord) {
            return null;
        }
        if ($dbRecord->getHash() === $hash) {
            return $dbRecord;
        }
        return null;
    }

    /**
     * @param BaseAuthForm $form
     * @return bool
     */
    public function findUserAndCheckPassword(BaseAuthForm $form)
    {
        $email = $form->getUser()->getEmail();
        $dbRecord = $this->userMapper->findByEmail($email);
        if (!$dbRecord) {
            $message = $this->getErrors()[$this->language]['notFound'];
            $form->addError('email', $message);
            return false;
        }

        $dbSalt = $dbRecord->getSalt();
        $dbHash = $dbRecord->getHash();
        $newSaltedHash = Generator::generateSaltedHash($dbSalt, $form->getPassword());

        if ($newSaltedHash != $dbHash) {
            $message = $this->getErrors()[$this->language]['wrongPassword'];
            $form->addError('password', $message);
            return false;
        }
        $form->getUser()->setId($dbRecord->getId());
        $form->getUser()->setHash($dbRecord->getHash());
        return true;
    }

    public function isEmailFree(BaseAuthForm $form)
    {
        $email = $form->getUser()->getEmail();
        $dbRecord = $this->userMapper->findByEmail($email);
        if ($dbRecord) {
            $message = $this->getErrors()[$this->language]['emailOccupied'];
            $form->addError('email', $message);
            return false;
        }
        return true;
    }

    /**
     * @param User $user
     * @param bool $remember
     */
    public function login(User $user, $remember = false)
    {
        if ($remember === true) {
            $time = time() + 3600*24*7;
        } else {
            $time = 0;
        }
        $path = '/';
        setcookie('id', $user->getId(), $time, $path);
        setcookie('hash', $user->getHash(), $time, $path);
    }
}
      run: <?php
namespace PricklyNut\NoxChallenge\Validator;

class Validator
{
    protected $language;

    public function __construct($language)
    {
        $this->language = $language;
    }

    public function errorMessages()
    {
        return array(
            'ru' => array(
                'notEmpty' => "Поле %s не может быть пустым",
                'maxLength' => "Максимальная длина поля %s - %s символов",
                'alphabetic' => "Поле %s может содержать только буквы",
                'isEmail' => "Поле %s должно содержать корректный электронный адрес",
                'minLength' => "Минимальная длина поля %s - %s символов",
            ),
            'en' => array(
                'notEmpty' => "%s cannot be empty",
                'maxLength' => "%s must be maximum %s symbols",
                'alphabetic' => "%s must contain only alphabetic characters",
                'isEmail' => "%s must be valid email address",
                'minLength' => "%s must be minimum %s symbols",
            ),
        );
    }

    public function validate($object)
    {
        $errorMessages = $this->errorMessages();
        $rulesPack = $object->getRules();

        foreach ($rulesPack as $field => $rules) {
            if ($rules == null) continue;
            foreach ($rules as $validator => $param) {
                $validatorName = 'validate' . ucfirst($validator);
                $fieldGetter = 'get' . ucfirst($field);
                if (!$this->$validatorName($object->$fieldGetter(), $param)) {
                    $message = $errorMessages[$this->language][$validator];
                    $label = $object->getLabels()[$this->language][$field];
                    $message = sprintf($message, $label, $param);
                    $object->addError($field, $message);
                }
            }
        }

        if (!$object->getErrors()) {
            return true;
        }
        return false;
    }

    public function validateNotEmpty($value)
    {
        return $value !== '';
    }

    public function validateMaxLength($value, $param)
    {
        return mb_strlen($value) <= $param;
    }

    public function validateMinLength($value, $param)
    {
        return mb_strlen($value) >= $param;
    }

    public function validateAlphabetic($value)
    {
        return boolval(preg_match('/^[a-zа-яё]+$/ui', $value));
    }

    public function validateIsEmail($value)
    {
        return boolval(preg_match('/^[^@.]+@[^@.]+\.[^@.]+$/u', $value));
    }
}
    - name: <?php
namespace PricklyNut\NoxChallenge;

use PricklyNut\NoxChallenge\DI\Container;

class Application
{
    /**
     * @var Container
     */
    protected $container;

    /**
     * Routes map
     * @var array
     */
    protected $map = array();

    /**
     * @var array
     */
    protected $config = array();

    public function __construct($config = array())
    {
        $this->container = new Container();
        $this->config = $config;

        $this->resolveLanguage();
    }

    /**
     * @return string
     */
    public function getLang()
    {
        return $this->config['lang'];
    }

    /**
     * @param $template
     * @param array $args
     */
    public function render($template, array $args)
    {
        extract($args);
        ob_start();
        require $this->config['templateDir'] . DIRECTORY_SEPARATOR . $template;
        $html = ob_get_clean();
        echo $html;
    }

    /**
     * @return Container
     */
    public function getContainer()
    {
        return $this->container;
    }

    /**
     * @param Container $container
     */
    public function setContainer($container)
    {
        $this->container = $container;
    }

    public function addRoute($uri, $handler)
    {
        $this->map[$uri] = $handler;
    }

    public function getMap()
    {
        return $this->map;
    }

    public function getNotFoundHandler()
    {
        return $this->map['notFoundHandler'];
    }

    public function setNotFoundHandler($handler)
    {
        return $this->map['notFoundHandler'] = $handler;
    }

    protected function resolveLanguage()
    {
        if (isset($_GET['lang'])) {
            $lang = $_GET['lang'];
            if (in_array($lang, $this->getSupportedLanguages())) {
                setcookie('lang', $lang, time() + 3600*24*90, '/');
            } else {
                $lang = 'ru';
            }
        } elseif (isset($_COOKIE['lang'])) {
            $lang = $_COOKIE['lang'];
        } else {
            $lang = 'ru';
        }
        $this->config['lang'] = $lang;
    }

    protected function getSupportedLanguages()
    {
        return array('ru', 'en');
    }
}
      run: cargo test --verbose
