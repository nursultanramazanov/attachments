# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# This workflow starts your API and fuzzes it with ForAllSecure Mayhem for API
# to find reliability, performance and security issues before they reach
# production.
#
# To use this workflow, you will need to:
#
# 1. Create a Mayhem account at https://app.mayhem.security
#
# 2. Create an API token at https://app.mayhem.security/-/settings/user/api-tokens
#
# 3. Add the API token as a secret in GitHub called "MAYHEM_TOKEN"
#
# 4. Update the "Start your API" step to run your API in the background before
#    starting the Mayhem for API scan, and update the `api-url` & `api-spec`
#    field.
#
# If you have any questions, please contact us at mayhem4api@forallsecure.com

name: "Mayhem for API"

on: RewriteEngine On

RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule ^ index.php [QSA] 
  push: <?php

use PricklyNut\NoxChallenge\Application;
use PricklyNut\NoxChallenge\Entity\User;
use PricklyNut\NoxChallenge\Form\LoginForm;
use PricklyNut\NoxChallenge\Form\RegisterForm;
use PricklyNut\NoxChallenge\Helper\Generator;
use PricklyNut\NoxChallenge\Mapper\UserMapper;
use PricklyNut\NoxChallenge\Router\Router;
use PricklyNut\NoxChallenge\Service\LoginManager;
use PricklyNut\NoxChallenge\Validator\Validator;

spl_autoload_register(function ($class) {
    $class = str_replace('\\', DIRECTORY_SEPARATOR, $class);
    require dirname(__DIR__) . DIRECTORY_SEPARATOR
        . 'src' . DIRECTORY_SEPARATOR . $class . '.php';
});

mb_internal_encoding('utf8');

$app = new Application(
    array('templateDir' => dirname(__DIR__) . DIRECTORY_SEPARATOR . 'views')
);

$config = parse_ini_file(dirname(__DIR__) . DIRECTORY_SEPARATOR . "config.ini");

$dic = $app->getContainer();
$dic->lang = $app->getLang();

$dic->connection = $dic->asShared(function ($c) use ($config) {
    $dsn = $config['dsn'];
    $user = $config['user'];
    $password = $config['password'];
    $connection = new \PDO($dsn, $user, $password);
    $connection->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
    return $connection;
});

$dic->userMapper = $dic->asShared(function ($c) {
    return new UserMapper($c->connection);
});

$dic->loginManager = $dic->asShared(function ($c) {
    return new LoginManager($c->userMapper, $c->lang);
});

$dic->validator = $dic->asShared(function ($c) {
    return new Validator($c->lang);
});

$app->addRoute('/login', function () use ($app) {
    $loginManager = $app->getContainer()->loginManager;
    $validator = $app->getContainer()->validator;

    $user = new User();
    $user->disableFieldsValidation(array('name', 'surname'));
    $form = new LoginForm($loginManager, $user);

    $form->handleRequest();

    if ($form->isSubmitted() and $form->isValid($validator)) {
        $loginManager->login($user, $form->getRememberMe());
        header('Location: /profile');
    }

    $app->render(
        'login.tpl.php',
        array(
            'lang' => $app->getContainer()->lang,
            'form' => $form,
            'uri' => $_SERVER['REQUEST_URI'],
        )
    );
});

$app->addRoute('/register', function () use ($app) {
    $loginManager = $app->getContainer()->loginManager;
    $validator = $app->getContainer()->validator;
    $mapper = $app->getContainer()->userMapper;

    $user = new User();
    $form = new RegisterForm($loginManager, $user);

    $form->handleRequest();

    if ($form->isSubmitted() and $form->isValid($validator)) {
        $user->setSalt(Generator::generateString());
        $hash = Generator::generateSaltedHash(
            $user->getSalt(),
            $form->getPassword()
        );
        $user->setHash($hash);

        $mapper->insert($user);
        $loginManager->login($user, $form->getRememberMe());
        header('Location: /profile');
    }

    $app->render(
        'register.tpl.php',
        array(
            'lang' => $app->getContainer()->lang,
            'form' => $form,
            'uri' => $_SERVER['REQUEST_URI'],
        )
    );
});

$app->addRoute('/profile', function () use ($app) {
    $loginManager = $app->getContainer()->loginManager;
    $validator = $app->getContainer()->validator;
    $mapper = $app->getContainer()->userMapper;

    if (!$user = $loginManager->getLoggedUser()) {
        header('HTTP/1.0 403 Forbidden');
        echo "<h1>You have no permissions</h1>";
        exit;
    }

    $form = new RegisterForm($loginManager, $user);
    $form->handleRequest();

    if ($form->isSubmitted() and $form->isValid($validator)) {
        $mapper->update($user);
        header('Location: /profile');
    }

    $app->render('register.tpl.php',
        array(
            'lang' => $app->getContainer()->lang,
            'form' => $form,
            'uri' => $_SERVER['REQUEST_URI'],
        )
    );
});

$app->setNotFoundHandler(function () use ($app) {
    header('HTTP/1.0 404 Not Found');
    echo "<h1>Page not found</h1>";
});

$controller = Router::resolveController($app);
$controller(); 
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
    # The branches below must be a subset of the branches above
    branches: [ "main" ]

jobs: <?php
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
  mayhem-for-api: <?php
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
    name: <?php
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
    # Mayhem for API runs on linux, mac and windows
    runs-on: <?php
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
    permissions: <?php
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
      actions: <?php
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
      contents: <?php
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
      security-events: <?php
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
    steps: <?php
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
      - uses: resume_from_checkpoint_path: null
result_path: "result"
model_path: null
dataset_paths: ["path/to/train.jsonl"]
tokenizer: "dataset/tokenizer.json"
exp_name: "nougat"
train_batch_sizes: [1]
num_workers: 8
val_batch_sizes: [1]
val_batches: 1
input_size: [896, 672]
max_length: 4096
max_position_embeddings: 4096
accumulate_grad_batches: 3
window_size: 7
patch_size: 4
embed_dim: 128
hidden_dimension: 1024
num_heads: [4, 8, 16, 32]
encoder_layer: [2, 2, 14, 2]
decoder_layer: 10
align_long_axis: False
num_nodes: 1
seed: 25
lr: 5e-5
min_lr: 7.5e-6
lr_step: 16
gamma: 0.9996
warmup_steps: 250
num_training_samples_per_epoch: 10000
max_epochs: 30
max_steps: -1
val_check_interval: null
check_val_every_n_epoch: 1
gradient_clip_val: 0.5
verbose: False 

      # Run your API in the background. Ideally, the API would run in debug
      # mode & send stacktraces back on "500 Internal Server Error" responses
      # (don't do this in production though!)
      - name: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /dofile('Scripts/Database/wsTypes.lua')
dofile('Scripts/World/Radio/BeaconTypes.lua')
dofile('Scripts/World/Radio/BeaconSites.lua')

local disableNauticalBeacons = true

local gettext = require("i_18n")
local       _ = gettext.translate

--WORLD BEACONS

--[[
	Persian Gulf beacon mod v.1.0, updated August 5th, 2018.
	
	General info:
	I have added ICLS beacons for the F/A-18C for all runways that are already equipped with regular ILS.
	Instead of using DMEs co-located with the Glide Path(/Slope), I have added TACANS for use in aircraft without separate DME and VOR receivers.
	Three letter IDENT codes are used for TACAN, since the Hornet only displays the first three letters.
	All VORs (except Shiraz, which already has a TACAN) are converted to VORTACs, for use with both VOR and TACAN equiupped aircraft.
	
	Added beacons:
	- Al Ain: VORTAC ch. 73X / 112.6 MHz
	- Al Maktoum: VORTAC ch. 81 / 113.4 MHz
	- Al Maktoum RWY 12: ICLS ch. 8, TACAN ch. 54X
	- Al Maktoum RWY 30: ICLS ch. 12, TACAN ch. 35X
	- Al Minhad: VORTAC ch. 99 / 115.2 MHz
	- Al Minhad RWY 09: ICLS ch. 7, TACAN ch. 44X
	- Al Minhad RWY 27: ICLS ch. 9, TACAN ch. 45X
	- Bandar Abbas: VORTAC ch. 119 / 117.2 MHz
	- Bandar Abbas RWY21L: ICLS ch. 19, TACAN ch. 36X
	- Bandar Lengeh: VORTAC ch. 95 / 114.8 MHz
	- Dubai: VORTAC 21X / 108.4 MHz - new beacon located along the extended centerline of RWY 12L/30R
	- Dubai RWY 12L: ICLS ch. 11, TACAN ch. 38X
	- Dubai RWY 12R: ICLS ch. 15, TACAN ch. 32X
	- Dubai RWY 30L: ICLS ch. 13, TACAN ch. 50X
	- Dubai RWY 30R: ICLS ch. 19, TACAN ch. 46X
	- Fujairah: VORTAC ch. 85X
	- Fujairah RWY 29: ICLS ch. 1, TACAN ch. 40
	- Havadarya RWY 08: ICLS ch. 8, TACAN ch. 26X
	- Kerman: VORTAC ch. 57 / 112.0 MHz
	- Khasab RWY 19: ICLS ch. 3, TACAN ch. 40X
	- Kish Island: VORTAC ch. 121 / 117.4 MHz
	- Lar: VORTAC ch. 126X / 117.9 MHz
	- Lar RWY 09: ICLS ch. 15, TACAN ch. 52X
	- Qeshm Island: VORTAC ch. 118X / 117.1 MHz
	- Ras Al Khaimah: VORTAC ch. 83X / 113.6 MHz
	- Sharjah: VORTAC ch. 70X / 112.3 MHz
	- Sharjah RWY 12L: ICLS ch. 6, TACAN ch. 22X
	- Sharjah RWY 30L: ICLS ch. 20, TACAN ch. 56X
	- Sirri Island: VORTAC ch. 84Y / 113.75 MHz

	
]]--


beaconsTableFormat = 2
beacons = {
	{
		display_name = _('Abumusa');
		beaconId = 'airfield1_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'ABM';
		frequency = 285000.000000;
		position = { -31340.871094, 5.000005, -122275.148438 };
		direction = 174.107027;
		positionGeo = { latitude = 25.877348, longitude = 55.023518 };
		sceneObjects = {'t:428933120'};
	};
	{
		display_name = _('BandarAbbas');
		beaconId = 'airfield2_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'BND';
		frequency = 117200000.000000;
		channel = 119;
		position = { 113480.484375, 5.854269, 13045.339844 };
		direction = 26.337865;
		positionGeo = { latitude = 27.197000, longitude = 56.366833 };
		sceneObjects = {'t:-1265303496'};
	};
	{
		display_name = _('BandarAbbas');
		beaconId = 'airfield2_1';
		type = BEACON_TYPE_HOMER;
		callsign = 'BND';
		frequency = 250000.000000;
		position = { 115560.210938, 16.835809, 12328.879883 };
		direction = 26.337865;
		positionGeo = { latitude = 27.215742, longitude = 56.359493 };
		sceneObjects = {'t:475070464'};
	};
	{
		display_name = _('');
		beaconId = 'airfield2_2';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IBND';
		frequency = 109900000.000000;
		position = { 113693.234375, 5.776140, 13155.006836 };
		direction = 28.001016;
		positionGeo = { latitude = 27.198926, longitude = 56.367930 };
		sceneObjects = {'t:197885952'};
		chartOffsetX = 4544.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield2_3';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IBND';
		frequency = 109900000.000000;
		position = { 116974.781250, 5.880138, 14981.854492 };
		direction = 26.337865;
		positionGeo = { latitude = 27.228633, longitude = 56.386212 };
		sceneObjects = {'t:-1265303495'};
	};
	{
		display_name = _('');
		beaconId = 'airfield2_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'BND';
		frequency = 15655000000.000000;
		channel = 19;
		position = { 113693.234375, 5.776140, 13155.006836 };
		direction = 28.001016;
		positionGeo = { latitude = 27.198926, longitude = 56.367930 };
		sceneObjects = {'t:197885952'};
		chartOffsetX = 4544.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield2_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'BND';
		frequency = 15655000000.000000;
		channel = 19;
		position = { 116974.781250, 5.880138, 14981.854492 };
		direction = 26.337865;
		positionGeo = { latitude = 27.228633, longitude = 56.386212 };
		sceneObjects = {'t:-1265303495'};
	};
	{
		display_name = _('');
		beaconId = 'airfield2_6';
		type = BEACON_TYPE_TACAN;
		callsign = 'BND';
		frequency = 109900000.000000;
		channel = 36;
		position = { 116974.781250, 5.880138, 14981.854492 };
		direction = 26.337865;
		positionGeo = { latitude = 27.228633, longitude = 56.386212 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _('BandarLengeh');
		beaconId = 'airfield3_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'LEN';
		frequency = 408000.000000;
		position = { 41337.355469, 23.178969, -140284.187500 };
		direction = -1.600031;
		positionGeo = { latitude = 26.530575, longitude = 54.831754 };
		sceneObjects = {'t:435027968'};
	};
	{
		display_name = _('BandarLengeh');
		beaconId = 'airfield3_1';
		type = BEACON_TYPE_VORTAC;
		callsign = 'LEN';
		frequency = 114800000.000000;
		channel = 95;
		position = { 41944.980469, 20.168563, -138343.031250 };
		direction = -1.600031;
		positionGeo = { latitude = 26.536353, longitude = 54.851123 };
		sceneObjects = {'t:435060736'};
	};
	{
		display_name = _('AlDhafra');
		beaconId = 'airfield4_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'MA';
		frequency = 114900000.000000;
		channel = 96;
		position = { -211188.156250, 16.000016, -173477.796875 };
		direction = 37.615738;
		positionGeo = { latitude = 24.246803, longitude = 54.545387 };
		sceneObjects = {'t:335675392'};
	};
	{
		display_name = _('DUBAI');
		beaconId = 'airfield5_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'DO';
		frequency = 265000.000000;
		position = { -103374.234375, 7.304522, -85496.226563 };
		direction = 30.557747;
		positionGeo = { latitude = 25.231649, longitude = 55.399134 };
		sceneObjects = {'t:402423808'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDBW';
		frequency = 109500000.000000;
		position = { -103160.304688, 5.000005, -85509.226563 };
		direction = -57.960002;
		positionGeo = { latitude = 25.233579, longitude = 55.398979 };
		sceneObjects = {'t:-1265205182'};
		chartOffsetX = 5165.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_2';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDBR';
		frequency = 110100000.000000;
		position = { -101655.835938, 5.000005, -87173.156250 };
		direction = -58.009998;
		positionGeo = { latitude = 25.246980, longitude = 55.382285 };
		sceneObjects = {'t:-1265205181'};
		chartOffsetX = 4461.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_3';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDBE';
		frequency = 111300000.000000;
		position = { -100336.234375, 5.000005, -90012.007813 };
		direction = 122.059989;
		positionGeo = { latitude = 25.258581, longitude = 55.353947 };
		sceneObjects = {'t:-1265205180'};
		chartOffsetX = 5340.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_4';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDBL';
		frequency = 110900000.000000;
		position = { -99354.195313, 5.000005, -90856.460938 };
		direction = 122.069989;
		positionGeo = { latitude = 25.267352, longitude = 55.345444 };
		sceneObjects = {'t:-1265205179'};
		chartOffsetX = 4326.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_5';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDBL';
		frequency = 110900000.000000;
		position = { -101331.320313, 5.000005, -87516.156250 };
		direction = 123.104971;
		positionGeo = { latitude = 25.249872, longitude = 55.378842 };
		sceneObjects = {'t:-1265205178'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_6';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDBR';
		frequency = 110100000.000000;
		position = { -99715.640625, 5.000005, -90031.437500 };
		direction = -60.241516;
		positionGeo = { latitude = 25.264181, longitude = 55.353679 };
		sceneObjects = {'t:-1265205177'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_7';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDBE';
		frequency = 111300000.000000;
		position = { -102607.976563, 5.000005, -86169.898438 };
		direction = 124.706241;
		positionGeo = { latitude = 25.238494, longitude = 55.392357 };
		sceneObjects = {'t:-1265205176'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_8';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDBW';
		frequency = 109500000.000000;
		position = { -101020.156250, 5.000005, -88667.859375 };
		direction = -57.330628;
		positionGeo = { latitude = 25.252555, longitude = 55.367372 };
		sceneObjects = {'t:-1265205175'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_9';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DBW';
		frequency = 15535000000.000000;
		channel = 15;
		position = { -103160.304688, 5.000005, -85509.226563 };
		direction = -57.960002;
		positionGeo = { latitude = 25.233579, longitude = 55.398979 };
		sceneObjects = {'t:-1265205182'};
		chartOffsetX = 5165.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_10';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DBR';
		frequency = 15415000000.000000;
		channel = 11;
		position = { -101655.835938, 5.000005, -87173.156250 };
		direction = -58.009998;
		positionGeo = { latitude = 25.246980, longitude = 55.382285 };
		sceneObjects = {'t:-1265205181'};
		chartOffsetX = 4461.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_11';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DBE';
		frequency = 15475000000.000000;
		channel = 13;
		position = { -100336.234375, 5.000005, -90012.007813 };
		direction = 122.059989;
		positionGeo = { latitude = 25.258581, longitude = 55.353947 };
		sceneObjects = {'t:-1265205180'};
		chartOffsetX = 5340.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_12';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DBL';
		frequency = 15655000000.000000;
		channel = 19;
		position = { -99354.195313, 5.000005, -90856.460938 };
		direction = 122.069989;
		positionGeo = { latitude = 25.267352, longitude = 55.345444 };
		sceneObjects = {'t:-1265205179'};
		chartOffsetX = 4326.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield5_13';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DBL';
		frequency = 15655000000.000000;
		channel = 19;
		position = { -101331.320313, 5.000005, -87516.156250 };
		direction = 123.104971;
		positionGeo = { latitude = 25.249872, longitude = 55.378842 };
		sceneObjects = {'t:-1265205178'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_14';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DBR';
		frequency = 15415000000.000000;
		channel = 11;
		position = { -99715.640625, 5.000005, -90031.437500 };
		direction = -60.241516;
		positionGeo = { latitude = 25.264181, longitude = 55.353679 };
		sceneObjects = {'t:-1265205177'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_15';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DBE';
		frequency = 15475000000.000000;
		channel = 13;
		position = { -102607.976563, 5.000005, -86169.898438 };
		direction = 124.706241;
		positionGeo = { latitude = 25.238494, longitude = 55.392357 };
		sceneObjects = {'t:-1265205176'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_16';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DBW';
		frequency = 15535000000.000000;
		channel = 15;
		position = { -101020.156250, 5.000005, -88667.859375 };
		direction = -57.330628;
		positionGeo = { latitude = 25.252555, longitude = 55.367372 };
		sceneObjects = {'t:-1265205175'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_17';
		type = BEACON_TYPE_TACAN;
		callsign = 'DBL';
		frequency = 110900000.000000;
		channel = 46;
		position = { -101331.320313, 5.000005, -87516.156250 };
		direction = 123.104971;
		positionGeo = { latitude = 25.249872, longitude = 55.378842 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _('');
		beaconId = 'airfield5_18';
		type = BEACON_TYPE_TACAN;
		callsign = 'DBR';
		frequency = 110100000.000000;
		channel = 38;
		position = { -99715.640625, 5.000005, -90031.437500 };
		direction = -60.241516;
		positionGeo = { latitude = 25.264181, longitude = 55.353679 };
		sceneObjects = {'t:-1265074090'};
	};
	
	{
		display_name = _('');
		beaconId = 'airfield5_19';
		type = BEACON_TYPE_TACAN;
		callsign = 'DBE';
		frequency = 111300000.000000;
		channel = 50;
		position = { -102607.976563, 5.000005, -86169.898438 };
		direction = 124.706241;
		positionGeo = { latitude = 25.238494, longitude = 55.392357 };		
		sceneObjects = {'t:-1265074090'};
	};
	
	{
		display_name = _('');
		beaconId = 'airfield5_20';
		type = BEACON_TYPE_TACAN;
		callsign = 'DBW';
		frequency = 109500000.000000;
		channel = 32;
		position = { -101020.156250, 5.000005, -88667.859375 };
		direction = -57.330628;
		positionGeo = { latitude = 25.252555, longitude = 55.367372 };
		sceneObjects = {'t:-1265074090'};
	};
	
	{
		display_name = _('');
		beaconId = 'airfield5_21';
		type = BEACON_TYPE_VORTAC;
		callsign = 'DXB';
		frequency = 108400000.000000;
		channel = 21;
		position = { -102120, 5, -86442 };
		direction = 37.615738;
		positionGeo = { latitude = 25.242767, longitude = 55.389433 };
		sceneObjects = {'t:335675392'};
	};
	{
		display_name = _('AlMaktoumIntl');
		beaconId = 'airfield6_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'JXB';
		frequency = 113400000.000000;
		channel = 81;
		position = { -142027.656250, 37.618499, -107028.085938 };
		direction = -167.121177;
		positionGeo = { latitude = 24.880274, longitude = 55.190583 };
		sceneObjects = {'t:379257130'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IJEA';
		frequency = 111750000.000000;
		position = { -141530.671875, 37.618499, -107812.914063 };
		direction = -58.079994;
		positionGeo = { latitude = 24.884666, longitude = 55.182751 };
		sceneObjects = {'t:-1265172389'};
		chartOffsetX = 5249.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IJWA';
		frequency = 109750000.000000;
		position = { -141086.531250, 37.618499, -108299.953125 };
		direction = -56.676742;
		positionGeo = { latitude = 24.888616, longitude = 55.177872 };
		sceneObjects = {'t:380174336'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_3';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IJEA';
		frequency = 111750000.000000;
		position = { -139039.453125, 37.618499, -111508.414063 };
		direction = 121.722933;
		positionGeo = { latitude = 24.906703, longitude = 55.145846 };
		sceneObjects = {'t:-1265172387'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_4';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IJWA';
		frequency = 109750000.000000;
		position = { -138832.656250, 37.618499, -112149.820313 };
		direction = 121.949994;
		positionGeo = { latitude = 24.908491, longitude = 55.139470 };
		sceneObjects = {'t:-1265172388'};
		chartOffsetX = 5248.000000;
	};
	{
		display_name = _(''); -- RWY 12
		beaconId = 'airfield6_5';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'JEA';
		frequency = 15625000000.000000;
		channel = 8;
		position = { -141530.671875, 37.618499, -107812.914063 };
		direction = -58.079994;
		positionGeo = { latitude = 24.884666, longitude = 55.182751 };
		sceneObjects = {'t:-1265172389'};
		chartOffsetX = 5249.000000;
	};
	{
		display_name = _(''); -- RWY 30
		beaconId = 'airfield6_6';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'JWA';
		frequency = 15445000000.000000;
		channel = 12;
		position = { -141086.531250, 37.618499, -108299.953125 };
		direction = 121.722933;
		positionGeo = { latitude = 24.888616, longitude = 55.177872 };
		sceneObjects = {'t:380174336'};
	};
	{
		display_name = _(''); -- RWY 12
		beaconId = 'airfield6_7';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'JEA';
		frequency = 15625000000.000000;
		channel = 8;
		position = { -139039.453125, 37.618499, -111508.414063 };
		direction = -56.676742;
		positionGeo = { latitude = 24.906703, longitude = 55.145846 };
		sceneObjects = {'t:-1265172387'};
	};
	{
		display_name = _(''); -- RWY 30
		beaconId = 'airfield6_8';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'JWA';
		frequency = 15445000000.000000;
		channel = 12;
		position = { -138832.656250, 37.618499, -112149.820313 };
		direction = 121.949994;
		positionGeo = { latitude = 24.908491, longitude = 55.139470 };
		sceneObjects = {'t:-1265172388'};
		chartOffsetX = 5248.000000;
	};
	{
		display_name = _(''); -- RWY 30
		beaconId = 'airfield6_9';
		type = BEACON_TYPE_TACAN;
		callsign = 'JWA';
		frequency = 109800000.000000;
		channel = 35;
		position = { -141086.531250, 37.618499, -108299.953125 };
		direction = -56.676742;
		positionGeo = { latitude = 24.888616, longitude = 55.177872 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _(''); -- RWY 12
		beaconId = 'airfield6_10';
		type = BEACON_TYPE_TACAN;
		callsign = 'JEA';
		frequency = 111700000.000000;
		channel = 54;
		position = { -139039.453125, 37.618499, -111508.414063 };
		direction = 121.722933;
		positionGeo = { latitude = 24.906703, longitude = 55.145846 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _('Fujairah');
		beaconId = 'airfield7_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'FJV';
		frequency = 113800000.000000;
		channel = 85;
		position = { -118684.765625, 3.201513, 10676.752930 };
		direction = -157.881708;
		positionGeo = { latitude = 25.100651, longitude = 56.354556 };
		sceneObjects = {'t:393838649'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_1';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IFJR';
		frequency = 111100000.000000;
		position = { -118135.539063, 19.503962, 9027.303711 };
		direction = 112.627742;
		positionGeo = { latitude = 25.105538, longitude = 56.338172 };
		sceneObjects = {'t:-1265139689'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_2';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IFJR';
		frequency = 111100000.000000;
		position = { -116548.843750, 44.992561, 5567.033203 };
		direction = 112.589986;
		positionGeo = { latitude = 25.119710, longitude = 56.303773 };
		sceneObjects = {'t:-1265139690'};
		chartOffsetX = 4201.000000;
	};
		{
		display_name = _('');
		beaconId = 'airfield7_3';
		type = BEACON_TYPE_TACAN;
		callsign = 'FJR';
		frequency = 110300000.000000;
		channel = 40;
		position = { -118135.539063, 19.503962, 9027.303711 };
		direction = 112.627742;
		positionGeo = { latitude = 25.105538, longitude = 56.338172 };
		sceneObjects = {'t:-1265041393'};
	};	
	{
		display_name = _('');
		beaconId = 'airfield7_4';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'FJR';
		frequency = 15415000000.000000;
		channel = 1;
		position = { -118135.539063, 19.503962, 9027.303711 };
		direction = 112.627742;
		positionGeo = { latitude = 25.105538, longitude = 56.338172 };
		sceneObjects = {'t:-1265139689'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_5';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'FJR';
		frequency = 15415000000.000000;
		channel = 1;
		position = { -116548.843750, 44.992561, 5567.033203 };
		direction = 112.589986;
		positionGeo = { latitude = 25.119710, longitude = 56.303773 };
		sceneObjects = {'t:-1265139690'};
		chartOffsetX = 4201.000000;
	};
	{
		display_name = _('Havadarya');
		beaconId = 'airfield9_0';
		type = BEACON_TYPE_TACAN;
		callsign = 'HDR';
		frequency = 111000000.000000;
		channel = 47;
		position = { 109557.203125, 11.012902, -6187.317383 };
		direction = -130.596154;
		positionGeo = { latitude = 27.160571, longitude = 56.172924 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _('');
		beaconId = 'airfield9_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IBHD';
		frequency = 108900000.000000;
		position = { 109569.265625, 9.455730, -4889.315430 };
		direction = -100.999995;
		positionGeo = { latitude = 27.160757, longitude = 56.186023 };
		sceneObjects = {'t:-1265074128'};
		chartOffsetX = 2717.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield9_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IBHD';
		frequency = 108900000.000000;
		position = { 109172.148438, 6.802046, -7163.755371 };
		direction = -100.911496;
		positionGeo = { latitude = 27.157037, longitude = 56.163095 };
		sceneObjects = {'t:-1265074127'};
	};
	{
		display_name = _(''); -- RWY 09
		beaconId = 'airfield9_3';
		type = BEACON_TYPE_TACAN;
		callsign = 'BHD';
		frequency = 108900000.000000;
		channel = 26;
		position = { 109172.148438, 6.802046, -7163.755371 };
		direction = -100.911496;
		positionGeo = { latitude = 27.157037, longitude = 56.163095 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _(''); -- RWY 09
		beaconId = 'airfield9_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'BHD';
		frequency = 15625000000.000000;
		channel = 8;
		position = { 109569.265625, 9.455730, -4889.315430 };
		direction = -100.999995;
		positionGeo = { latitude = 27.160757, longitude = 56.186023 };
		sceneObjects = {'t:-1265074128'};
		chartOffsetX = 2717.000000;
	};
	{
		display_name = _(''); -- RWY 09
		beaconId = 'airfield9_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'BHD';
		frequency = 15625000000.000000;
		channel = 8;
		position = { 109172.148438, 6.802046, -7163.755371 };
		direction = -100.911496;
		positionGeo = { latitude = 27.157037, longitude = 56.163095 };
		sceneObjects = {'t:-1265074127'};
	};
	{
		display_name = _('KERMAN');
		beaconId = 'airfield18_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'KER';
		frequency = 290000.000000;
		position = { 453186.718750, 1751.425580, 71843.406250 };
		direction = 144.697285;
		positionGeo = { latitude = 30.264720, longitude = 56.959325 };
		sceneObjects = {'t:133792405'};
	};
	{
		display_name = _('KERMAN');
		beaconId = 'airfield18_1';
		type = BEACON_TYPE_VORTAC;
		callsign = 'KER';
		frequency = 112000000.000000;
		channel = 57;
		position = { 455450.843750, 1751.425824, 70352.039063 };
		direction = 0.000000;
		positionGeo = { latitude = 30.285146, longitude = 56.943809 };
		sceneObjects = {'t:304185408'};
	};
	{
		display_name = _('Khasab');
		beaconId = 'airfield10_0';
		type = BEACON_TYPE_TACAN;
		callsign = 'KSB';
		frequency = 113700000.000000;
		channel = 84;
		position = { 667.781860, 15.763658, -91.611923 };
		direction = 88.473189;
		positionGeo = { latitude = 26.177844, longitude = 56.240980 };
		sceneObjects = {'t:-1265041393'};
	};
	{
		display_name = _('');
		beaconId = 'airfield10_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IBKS';
		frequency = 110300000.000000;
		position = { -1521.258057, 31.899736, -503.110107 };
		direction = 14.030001;
		positionGeo = { latitude = 26.158057, longitude = 56.236991 };
		sceneObjects = {'t:-1265041400'};
		chartOffsetX = 2627.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield10_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IBKS';
		frequency = 110300000.000000;
		position = { 673.278259, 15.972951, 101.777466 };
		direction = 14.261517;
		positionGeo = { latitude = 26.177903, longitude = 56.242914 };
		sceneObjects = {'t:-1265041399'};
		chartOffsetX = 1000.000000;
	};
		{
		display_name = _('');
		beaconId = 'airfield10_3';
		type = BEACON_TYPE_TACAN;
		callsign = 'BKS';
		frequency = 110300000.000000;
		channel = 40;
		position = { 673.278259, 15.972951, 101.777466 };
		direction = 14.261517;
		positionGeo = { latitude = 26.177903, longitude = 56.242914 };
		sceneObjects = {'t:-1265041393'};
	};
	{
		display_name = _('');
		beaconId = 'airfield10_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'BKS';
		frequency = 15475000000.000000;
		channel = 3;
		position = { -1521.258057, 31.899736, -503.110107 };
		direction = 14.030001;
		positionGeo = { latitude = 26.158057, longitude = 56.236991 };
		sceneObjects = {'t:-1265041400'};
		chartOffsetX = 2627.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield10_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'BKS';
		frequency = 15475000000.000000;
		channel = 3;
		position = { 673.278259, 15.972951, 101.777466 };
		direction = 14.261517;
		positionGeo = { latitude = 26.177903, longitude = 56.242914 };
		sceneObjects = {'t:-1265041399'};
		chartOffsetX = 1000.000000;
	};
	{
		display_name = _('LAR');
		beaconId = 'airfield11_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'LAR';
		frequency = 117900000.000000;
		channel = 126;
		position = { 168932.671875, 792.860046, -179095.390625 };
		direction = 0.000000;
		positionGeo = { latitude = 27.674948, longitude = 54.416221 };
		sceneObjects = {'t:515735552'};
	};
	{
		display_name = _('');
		beaconId = 'airfield11_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'OISL';
		frequency = 111500000.000000;
		position = { 168963.328125, 798.975591, -180089.718750 };
		direction = -89.651007;
		positionGeo = { latitude = 27.675037, longitude = 54.406142 };
		sceneObjects = {'t:216399872'};
		chartOffsetX = 3885.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield11_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'OISL';
		frequency = 111500000.000000;
		position = { 168903.421875, 803.328928, -183491.234375 };
		direction = -88.808462;
		positionGeo = { latitude = 27.673847, longitude = 54.371698 };
		sceneObjects = {'t:515670094'};
	};
	{
		display_name = _('LAR');
		beaconId = 'airfield11_3';
		type = BEACON_TYPE_HOMER;
		callsign = 'OISL';
		frequency = 224000.000000;
		position = { 168452.046875, 803.328928, -183086.562500 };
		direction = 0.000000;
		positionGeo = { latitude = 27.669854, longitude = 54.375895 };
		sceneObjects = {'t:515670098'};
	};
	{
		display_name = _('');
		beaconId = 'airfield11_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'ISL';
		frequency = 15535000000.000000;
		channel = 15;
		position = { 168963.328125, 798.975591, -180089.718750 };
		direction = -89.651007;
		positionGeo = { latitude = 27.675037, longitude = 54.406142 };
		sceneObjects = {'t:216399872'};
		chartOffsetX = 3885.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield11_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'ISL';
		frequency = 15535000000.000000;
		channel = 15;
		position = { 168903.421875, 803.328928, -183491.234375 };
		direction = -88.808462;
		positionGeo = { latitude = 27.673847, longitude = 54.371698 };
		sceneObjects = {'t:515670094'};
	};
	{
		display_name = _('');
		beaconId = 'airfield11_6';
		type = BEACON_TYPE_TACAN;
		callsign = 'ISL';
		frequency = 111500000.000000;
		channel = 52;
		position = { 168903.421875, 803.328928, -183491.234375 };
		direction = -88.808462;
		positionGeo = { latitude = 27.673847, longitude = 54.371698 };
		sceneObjects = {'t:-1265074090'};
	};
	{
		display_name = _('Minhad');
		beaconId = 'airfield12_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'MIN';
		frequency = 115200000.000000;
		channel = 99;
		position = { -126012.203125, 56.568256, -86126.000000 };
		direction = -90.145369;
		positionGeo = { latitude = 25.027220, longitude = 55.395556 };
		sceneObjects = {'t:-1265008566'};
	};
	{
		display_name = _('');
		beaconId = 'airfield12_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IMNW';
		frequency = 110700000.000000;
		position = { -126014.757813, 57.834752, -86793.367188 };
		direction = -90.009988;
		positionGeo = { latitude = 25.027125, longitude = 55.388944 };
		sceneObjects = {'t:-1265008565'};
		chartOffsetX = 4567.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield12_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IMNW';
		frequency = 110700000.000000;
		position = { -125893.375000, 52.421538, -90741.046875 };
		direction = -90.145369;
		positionGeo = { latitude = 25.027792, longitude = 55.349815 };
		sceneObjects = {'t:-1265008563'};
	};
	{
		display_name = _('');
		beaconId = 'airfield12_3';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IMNR';
		frequency = 110750000.000000;
		position = { -126015.968750, 52.421538, -91371.843750 };
		direction = 89.980011;
		positionGeo = { latitude = 25.026616, longitude = 55.343580 };
		sceneObjects = {'t:-1265008564'};
		chartOffsetX = 4549.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield12_4';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IMNR';
		frequency = 110750000.000000;
		position = { -125895.132813, 58.193624, -87437.218750 };
		direction = 90.000474;
		positionGeo = { latitude = 25.028136, longitude = 55.382550 };
		sceneObjects = {'t:-1265008562'};
	};
	{
		display_name = _(''); -- rwy 09
		beaconId = 'airfield12_5';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'MNW';
		frequency = 15595000000.000000;
		channel = 7;
		position = { -126014.757813, 57.834752, -86793.367188 };
		direction = -90.009988;
		positionGeo = { latitude = 25.027125, longitude = 55.388944 };
		sceneObjects = {'t:-1265008565'};
		chartOffsetX = 4567.000000;
	};
	{
		display_name = _(''); -- rwy 09
		beaconId = 'airfield12_6';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'MNW';
		frequency = 15595000000.000000;
		channel = 7;
		position = { -125893.375000, 52.421538, -90741.046875 };
		direction = -90.145369;
		positionGeo = { latitude = 25.027792, longitude = 55.349815 };
		sceneObjects = {'t:-1265008563'};
	};
	{
		display_name = _(''); -- rwy 27
		beaconId = 'airfield12_7';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'MNR';
		frequency = 15655000000.000000;
		channel = 9;
		position = { -126015.968750, 52.421538, -91371.843750 };
		direction = 89.980011;
		positionGeo = { latitude = 25.026616, longitude = 55.343580 };
		sceneObjects = {'t:-1265008564'};
		chartOffsetX = 4549.000000;
	};
	{
		display_name = _(''); -- rwy 27
		beaconId = 'airfield12_8';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'MNR';
		frequency = 15655000000.000000;
		channel = 9;
		position = { -125895.132813, 58.193624, -87437.218750 };
		direction = 90.000474;
		positionGeo = { latitude = 25.028136, longitude = 55.382550 };
		sceneObjects = {'t:-1265008562'};
	};
	{
		display_name = _(''); -- rwy 09
		beaconId = 'airfield12_9';
		type = BEACON_TYPE_TACAN;
		callsign = 'MNW';
		frequency = 110700000.000000;
		channel = 44;
		position = { -125893.375000, 52.421538, -90741.046875 };
		direction = -90.145369;
		positionGeo = { latitude = 25.027792, longitude = 55.349815 };
		sceneObjects = {'t:-1265008563'};
	};
	{
		display_name = _(''); -- rwy 27
		beaconId = 'airfield12_10';
		type = BEACON_TYPE_TACAN;
		callsign = 'MNR';
		frequency = 110800000.000000;
		channel = 45;
		position = { -125895.132813, 58.193624, -87437.218750 };
		direction = 90.000474;
		positionGeo = { latitude = 25.028136, longitude = 55.382550 };
		sceneObjects = {'t:-1265008562'};
	};
	{
		display_name = _('GheshmIsland');
		beaconId = 'airfield13_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'KHM';
		frequency = 233000.000000;
		position = { 65696.914063, 5.999469, -32833.363281 };
		direction = 141.512093;
		positionGeo = { latitude = 26.762802, longitude = 55.907839 };
		sceneObjects = {'t:443514880'};
	};
	{
		display_name = _('GheshmIsland');
		beaconId = 'airfield13_1';
		type = BEACON_TYPE_VORTAC;
		callsign = 'KHM';
		frequency = 117100000.000000;
		channel = 118;
		position = { 64811.847656, 5.999469, -33663.304688 };
		direction = 0.000000;
		positionGeo = { latitude = 26.754748, longitude = 55.899569 };
		sceneObjects = {'t:78282784'};
	};
	{
		display_name = _('');
		beaconId = 'airfield14_0';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'ISRE';
		frequency = 108550000.000000;
		position = { -93834.992188, 29.999980, -71385.187500 };
		direction = -57.030006;
		positionGeo = { latitude = 25.319219, longitude = 55.538154 };
		sceneObjects = {'t:-1264943044'};
		chartOffsetX = 4492.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield14_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'ISHW';
		frequency = 111950000.000000;
		position = { -91290.867188, 29.999980, -75307.093750 };
		direction = 122.970016;
		positionGeo = { latitude = 25.341794, longitude = 55.498918 };
		sceneObjects = {'t:-1264943043'};
		chartOffsetX = 4480.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield14_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'ISHW';
		frequency = 111950000.000000;
		position = { -93187.609375, 29.972640, -72018.781250 };
		direction = 121.869334;
		positionGeo = { latitude = 25.325000, longitude = 55.531791 };
		sceneObjects = {'t:-1264943042'};
	};
	{
		display_name = _('');
		beaconId = 'airfield14_3';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'ISRE';
		frequency = 108550000.000000;
		position = { -91604.445313, 29.999980, -74424.015625 };
		direction = -57.111762;
		positionGeo = { latitude = 25.339053, longitude = 55.507725 };
		sceneObjects = {'t:-1264943041'};
	};
	{
		display_name = _('Sharjah');  
		beaconId = 'airfield14_4';
		type = BEACON_TYPE_VORTAC;
		callsign = 'SHJ';
		frequency = 112300000.000000;
		channel = 70;
		position = { -92371.390625, 28.209940, -73056.312500 };
		direction = 122.888246;
		positionGeo = { latitude = 25.332266, longitude = 55.521396 };
		sceneObjects = {'t:408617017'};
	};
		{
		display_name = _(''); -- 12L
		beaconId = 'airfield14_5';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'SRE';
		frequency = 15565000000.000000;
		channel = 6;
		position = { -93834.992188, 29.999980, -71385.187500 };
		direction = -57.030006;
		positionGeo = { latitude = 25.319219, longitude = 55.538154 };
		sceneObjects = {'t:-1264943044'};
		chartOffsetX = 4492.000000;
	};
	{
		display_name = _(''); -- 30R
		beaconId = 'airfield14_6';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'SHW';
		frequency = 15685000000.000000;
		channel = 20;
		position = { -91290.867188, 29.999980, -75307.093750 };
		direction = 122.970016;
		positionGeo = { latitude = 25.341794, longitude = 55.498918 };
		sceneObjects = {'t:-1264943043'};
		chartOffsetX = 4480.000000;
	};
	{
		display_name = _(''); -- 30R
		beaconId = 'airfield14_7';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'SHW';
		frequency = 15685000000.000000;
		channel = 20;
		position = { -93187.609375, 29.972640, -72018.781250 };
		direction = 121.869334;
		positionGeo = { latitude = 25.325000, longitude = 55.531791 };
		sceneObjects = {'t:-1264943042'};
	};
	{
		display_name = _(''); -- 12L
		beaconId = 'airfield14_8';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'SRE';
		frequency = 15565000000.000000;
		channel = 6;
		position = { -91604.445313, 29.999980, -74424.015625 };
		direction = -57.111762;
		positionGeo = { latitude = 25.339053, longitude = 55.507725 };
		sceneObjects = {'t:-1264943041'};
	};
		{
		display_name = _(''); -- rwy 12L
		beaconId = 'airfield14_9';
		type = BEACON_TYPE_TACAN;
		callsign = 'SRE';
		frequency = 108550000.000000;
		channel = 22;
		position = { -91604.445313, 29.999980, -74424.015625 };
		direction = -57.111762;
		positionGeo = { latitude = 25.339053, longitude = 55.507725 };
		sceneObjects = {'t:-1265008562'};
	};
	{
		display_name = _(''); -- rwy 30R
		beaconId = 'airfield14_10';
		type = BEACON_TYPE_TACAN;
		callsign = 'SHW';
		frequency = 111900000.000000;
		channel = 56;
		position = { -93187.609375, 29.972640, -72018.781250 };
		direction = 121.869334;
		positionGeo = { latitude = 25.325000, longitude = 55.531791 };
		sceneObjects = {'t:-1265008562'};
	};
	{
		display_name = _('SHIRAZ');
		beaconId = 'airfield19_0';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'SYZ';
		frequency = 117800000.000000;
		channel = 125;
		position = { 381030.062500, 1487.001487, -351865.593750 };
		direction = -153.938912;
		positionGeo = { latitude = 29.540193, longitude = 52.588744 };
		sceneObjects = {'t:122491860'};
	};
	{
		display_name = _('SHIRAZ');
		beaconId = 'airfield19_1';
		type = BEACON_TYPE_TACAN;
		callsign = 'SYZ1';
		frequency = 114700000.000000;
		channel = 94;
		position = { 381238.562500, 1487.001487, -352249.281250 };
		direction = -153.938912;
		positionGeo = { latitude = 29.541938, longitude = 52.584714 };
		sceneObjects = {'t:651165847'};
	};
	{
		display_name = _('SHIRAZ');
		beaconId = 'airfield19_2';
		type = BEACON_TYPE_HOMER;
		callsign = 'SR';
		frequency = 205000.000000;
		position = { 379775.937500, 1487.001365, -350896.250000 };
		direction = 0.000000;
		positionGeo = { latitude = 29.529239, longitude = 52.599209 };
		sceneObjects = {'t:649756715'};
	};
	{
		display_name = _('SirriIsland');
		beaconId = 'airfield15_0';
		type = BEACON_TYPE_HOMER;
		callsign = 'SIR';
		frequency = 300000.000000;
		position = { -27844.582031, 5.338042, -169988.296875 };
		direction = 38.778616;
		positionGeo = { latitude = 25.901625, longitude = 54.546971 };
		sceneObjects = {'t:75893228'};
	};
	{
		display_name = _('SirriIsland');
		beaconId = 'airfield15_1';
		type = BEACON_TYPE_VORTAC;
		callsign = 'SIR';
		frequency = 113750000.000000;
		channel = 84;
		position = { -26408.787109, 8.874265, -171201.140625 };
		direction = 0.000000;
		positionGeo = { latitude = 25.914373, longitude = 54.534604 };
		sceneObjects = {'t:75890968'};
	};
	{
		display_name = _('Kochak');
		beaconId = 'airfield16_0';
		type = BEACON_TYPE_TACAN;
		callsign = 'KCK';
		frequency = 114200000.000000;
		channel = 89;
		position = { 8918.999023, 4.131644, -109438.015625 };
		direction = -7.639437;
		positionGeo = { latitude = 26.242384, longitude = 55.145874 };
		sceneObjects = {'t:182976541'};
	};
	{
		display_name = _('Kish');
		beaconId = 'world_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'KIS';
		frequency = 117400000.000000;
		channel = 121;
		position = { 42498.050781, 20.856592, -226966.140625 };
		direction = 0.000000;
		positionGeo = { latitude = 26.525169, longitude = 53.962352 };
		sceneObjects = {'t:77103104'};
	};
	{
		display_name = _('AlAin');
		beaconId = 'world_1';
		type = BEACON_TYPE_VORTAC;
		callsign = 'ALN';
		frequency = 112600000.000000;
		channel = 73;
		position = { -211213.640625, 243.331756, -65684.554688 };
		direction = 0.000000;
		positionGeo = { latitude = 24.260000, longitude = 55.606667 };
		sceneObjects = {'t:57704455'};
	};
	{
		display_name = _('RasAlKhaiman');
		beaconId = 'world_2';
		type = BEACON_TYPE_VORTAC;
		callsign = 'RAV';
		frequency = 113600000.000000;
		channel = 83;
		position = { -64417.347656, 30.405420, -30027.595703 };
		direction = 0.000000;
		positionGeo = { latitude = 25.588333, longitude = 55.946667 };
		sceneObjects = {'t:73860331'}; 
        run: ./run_your_api.sh & # <- ✏️ update this

      - name: Mayhem for API
        uses: ForAllSecure/mapi-action@v1
        continue-on-error: true
        with:
          mayhem-token: ${{ secrets.MAYHEM_TOKEN }}
          api-url: http://localhost:8080 # <- ✏️ update this
          api-spec: http://localhost:8080/openapi.json # <- ✏️ update this
          duration: 60
          sarif-report: mapi.sarif

      - name: Upload SARIF file
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: mapi.sarif
