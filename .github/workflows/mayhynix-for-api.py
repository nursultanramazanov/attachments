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
      - uses: <?php
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

      # Run your API in the background. Ideally, the API would run in debug
      # mode & send stacktraces back on "500 Internal Server Error" responses
      # (don't do this in production though!)
      - name: ISO-10303-21; 
HEADER; 
FILE_DESCRIPTION (( 'STEP AP203' ), 
    '1' ); 
FILE_NAME ('delanclip-gopro-adapter.STEP', 
    '2022-01-13T15:09:10', 
    ( '' ), 
    ( '' ), 
    'SwSTEP 2.0', 
    'SolidWorks 2018', 
    '' ); 
FILE_SCHEMA (( 'CONFIG_CONTROL_DESIGN' )); 
ENDSEC; 

DATA; 
#1 = ORIENTED_EDGE ( 'NONE', *, *, #646, .T. ) ; 
#2 = APPLICATION_PROTOCOL_DEFINITION ( 'international standard', 'config_control_design', 1994, #697 ) ; 
#3 = DATE_TIME_ROLE ( 'creation_date' ) ; 
#4 = ORIENTED_EDGE ( 'NONE', *, *, #273, .F. ) ; 
#5 = ADVANCED_FACE ( 'NONE', ( #180 ), #838, .T. ) ; 
#6 = VERTEX_POINT ( 'NONE', #906 ) ; 
#7 = VECTOR ( 'NONE', #294, 1000.000000000000000 ) ; 
#8 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 2.000000000000000000, 1.500000000000004885 ) ) ; 
#9 = ADVANCED_FACE ( 'NONE', ( #635 ), #489, .F. ) ; 
#10 = ORIENTED_EDGE ( 'NONE', *, *, #440, .F. ) ; 
#11 = EDGE_CURVE ( 'NONE', #198, #25, #447, .T. ) ; 
#12 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#13 = EDGE_CURVE ( 'NONE', #610, #788, #820, .T. ) ; 
#14 = VERTEX_POINT ( 'NONE', #577 ) ; 
#15 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#16 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, 1.000000000000000000 ) ) ; 
#17 = ORIENTED_EDGE ( 'NONE', *, *, #632, .F. ) ; 
#18 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#19 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#20 = CC_DESIGN_APPROVAL ( #284, ( #326 ) ) ; 
#21 = LINE ( 'NONE', #301, #7 ) ; 
#22 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#23 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#24 = LINE ( 'NONE', #63, #860 ) ; 
#25 = VERTEX_POINT ( 'NONE', #99 ) ; 
#26 = ADVANCED_FACE ( 'NONE', ( #754 ), #295, .F. ) ; 
#27 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#28 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, 9.000000000000000000, 7.480000000000004867 ) ) ; 
#29 = VERTEX_POINT ( 'NONE', #96 ) ; 
#30 = DIRECTION ( 'NONE',  ( -1.284980352575412699E-16, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#31 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#32 = FACE_OUTER_BOUND ( 'NONE', #521, .T. ) ; 
#33 = AXIS2_PLACEMENT_3D ( 'NONE', #359, #573, #785 ) ; 
#34 = FACE_BOUND ( 'NONE', #523, .T. ) ; 
#35 = VECTOR ( 'NONE', #813, 1000.000000000000000 ) ; 
#36 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#37 = ORIENTED_EDGE ( 'NONE', *, *, #584, .T. ) ; 
#38 = ORIENTED_EDGE ( 'NONE', *, *, #654, .T. ) ; 
#39 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#40 = AXIS2_PLACEMENT_3D ( 'NONE', #348, #539, #768 ) ; 
#41 = EDGE_CURVE ( 'NONE', #29, #474, #56, .T. ) ; 
#42 = PLANE ( 'NONE',  #115 ) ; 
#43 = EDGE_LOOP ( 'NONE', ( #861, #565, #641, #410 ) ) ; 
#44 = ORIENTED_EDGE ( 'NONE', *, *, #199, .T. ) ; 
#45 = FACE_OUTER_BOUND ( 'NONE', #117, .T. ) ; 
#46 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 2.000000000000000000, -9.499999999999994671 ) ) ; 
#47 = CALENDAR_DATE ( 2022, 13, 1 ) ; 
#48 = EDGE_CURVE ( 'NONE', #531, #758, #255, .T. ) ; 
#49 = VECTOR ( 'NONE', #262, 1000.000000000000000 ) ; 
#50 = ORIENTED_EDGE ( 'NONE', *, *, #737, .T. ) ; 
#51 = PRODUCT_DEFINITION ( 'UNKNOWN', '', #326, #325 ) ; 
#52 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#53 = VECTOR ( 'NONE', #882, 1000.000000000000000 ) ; 
#54 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#55 = FACE_OUTER_BOUND ( 'NONE', #134, .T. ) ; 
#56 = LINE ( 'NONE', #287, #582 ) ; 
#57 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#58 = FACE_OUTER_BOUND ( 'NONE', #351, .T. ) ; 
#59 = PLANE ( 'NONE',  #843 ) ; 
#60 = EDGE_CURVE ( 'NONE', #736, #810, #439, .T. ) ; 
#61 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#62 = VECTOR ( 'NONE', #601, 1000.000000000000000 ) ; 
#63 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, 0.000000000000000000, 4.280000000000007354 ) ) ; 
#64 = CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT ( #165, #459, ( #326 ) ) ; 
#65 = LINE ( 'NONE', #140, #145 ) ; 
#66 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#67 = EDGE_LOOP ( 'NONE', ( #547, #652, #899, #313 ) ) ; 
#68 = MECHANICAL_CONTEXT ( 'NONE', #830, 'mechanical' ) ; 
#69 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#70 = ORIENTED_EDGE ( 'NONE', *, *, #844, .T. ) ; 
#71 = EDGE_LOOP ( 'NONE', ( #37, #665, #81, #880 ) ) ; 
#72 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#73 = EDGE_CURVE ( 'NONE', #718, #118, #627, .T. ) ; 
#74 = VECTOR ( 'NONE', #236, 1000.000000000000000 ) ; 
#75 = ADVANCED_FACE ( 'NONE', ( #34, #394 ), #216, .T. ) ; 
#76 = VECTOR ( 'NONE', #155, 1000.000000000000000 ) ; 
#77 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#78 = AXIS2_PLACEMENT_3D ( 'NONE', #226, #222, #219 ) ; 
#79 = VECTOR ( 'NONE', #298, 1000.000000000000000 ) ; 
#80 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#81 = ORIENTED_EDGE ( 'NONE', *, *, #486, .F. ) ; 
#82 = LINE ( 'NONE', #650, #238 ) ; 
#83 = ADVANCED_FACE ( 'NONE', ( #794 ), #500, .F. ) ; 
#84 = ORIENTED_EDGE ( 'NONE', *, *, #716, .T. ) ; 
#85 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 2.000000000000000000, 0.000000000000000000 ) ) ; 
#86 = FACE_OUTER_BOUND ( 'NONE', #618, .T. ) ; 
#87 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 9.000000000000000000, 4.280000000000006466 ) ) ; 
#88 = VECTOR ( 'NONE', #277, 1000.000000000000000 ) ; 
#89 = EDGE_LOOP ( 'NONE', ( #160, #224, #621, #263, #528, #10 ) ) ; 
#90 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#91 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#92 = EDGE_CURVE ( 'NONE', #380, #815, #510, .T. ) ; 
#93 = EDGE_CURVE ( 'NONE', #667, #536, #329, .T. ) ; 
#94 = ORIENTED_EDGE ( 'NONE', *, *, #632, .T. ) ; 
#95 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#96 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 0.000000000000000000, 1.500000000000001332 ) ) ; 
#97 = ORIENTED_EDGE ( 'NONE', *, *, #696, .T. ) ; 
#98 = EDGE_LOOP ( 'NONE', ( #747, #711, #94, #156, #402, #432, #377, #647, #875, #877, #488, #316 ) ) ; 
#99 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 2.000000000000000000, 1.500000000000004885 ) ) ; 
#100 = APPROVAL ( #557, 'UNSPECIFIED' ) ; 
#101 = CIRCLE ( 'NONE', #790, 1.599999999999997202 ) ; 
#102 = LINE ( 'NONE', #116, #150 ) ; 
#103 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, -1.836970198720894780E-17, 5.880000000000006111 ) ) ; 
#104 = LINE ( 'NONE', #611, #629 ) ; 
#105 = VERTEX_POINT ( 'NONE', #87 ) ; 
#106 = ORIENTED_EDGE ( 'NONE', *, *, #352, .T. ) ; 
#107 = ORIENTED_EDGE ( 'NONE', *, *, #239, .F. ) ; 
#108 = EDGE_CURVE ( 'NONE', #105, #848, #82, .T. ) ; 
#109 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#110 = EDGE_CURVE ( 'NONE', #713, #736, #586, .T. ) ; 
#111 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, -13.49999999999999467 ) ) ; 
#112 = FACE_OUTER_BOUND ( 'NONE', #478, .T. ) ; 
#113 = ORIENTED_EDGE ( 'NONE', *, *, #763, .F. ) ; 
#114 = ORIENTED_EDGE ( 'NONE', *, *, #490, .F. ) ; 
#115 = AXIS2_PLACEMENT_3D ( 'NONE', #109, #512, #516 ) ; 
#116 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 7.480000000000004867 ) ) ; 
#117 = EDGE_LOOP ( 'NONE', ( #829, #743, #201, #17 ) ) ; 
#118 = VERTEX_POINT ( 'NONE', #708 ) ; 
#119 = LINE ( 'NONE', #464, #419 ) ; 
#120 = EDGE_CURVE ( 'NONE', #869, #159, #811, .T. ) ; 
#121 = ORIENTED_EDGE ( 'NONE', *, *, #816, .F. ) ; 
#122 = PLANE ( 'NONE',  #444 ) ; 
#123 = EDGE_CURVE ( 'NONE', #598, #588, #168, .T. ) ; 
#124 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#125 = ORIENTED_EDGE ( 'NONE', *, *, #11, .F. ) ; 
#126 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 0.000000000000000000, 4.280000000000006466 ) ) ; 
#127 = ORIENTED_EDGE ( 'NONE', *, *, #253, .T. ) ; 
#128 = PLANE ( 'NONE',  #541 ) ; 
#129 = CC_DESIGN_APPROVAL ( #100, ( #51 ) ) ; 
#130 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#131 = CALENDAR_DATE ( 2022, 13, 1 ) ; 
#132 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 2.000000000000000000, -11.09999999999999432 ) ) ; 
#133 = ORIENTED_EDGE ( 'NONE', *, *, #654, .F. ) ; 
#134 = EDGE_LOOP ( 'NONE', ( #314, #834, #340, #741, #434, #656, #624, #518 ) ) ; 
#135 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, 13.50000000000000178 ) ) ; 
#136 = EDGE_CURVE ( 'NONE', #248, #810, #148, .T. ) ; 
#137 = VERTEX_POINT ( 'NONE', #719 ) ; 
#138 = ORIENTED_EDGE ( 'NONE', *, *, #93, .T. ) ; 
#139 = VECTOR ( 'NONE', #54, 1000.000000000000000 ) ; 
#140 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, -13.49999999999999467 ) ) ; 
#141 = LINE ( 'NONE', #637, #353 ) ; 
#142 = PLANE ( 'NONE',  #40 ) ; 
#143 = DATE_AND_TIME ( #47, #221 ) ; 
#144 = CC_DESIGN_DATE_AND_TIME_ASSIGNMENT ( #493, #591, ( #793 ) ) ; 
#145 = VECTOR ( 'NONE', #908, 1000.000000000000000 ) ; 
#146 = EDGE_LOOP ( 'NONE', ( #386, #171, #496, #113 ) ) ; 
#147 = APPROVAL_ROLE ( '' ) ; 
#148 = LINE ( 'NONE', #28, #836 ) ; 
#149 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 2.000000000000000000, 1.500000000000004885 ) ) ; 
#150 = VECTOR ( 'NONE', #886, 1000.000000000000000 ) ; 
#151 = ADVANCED_FACE ( 'NONE', ( #508 ), #174, .F. ) ; 
#152 = ORIENTED_EDGE ( 'NONE', *, *, #240, .T. ) ; 
#153 = LINE ( 'NONE', #472, #782 ) ; 
#154 = APPROVAL_PERSON_ORGANIZATION ( #671, #284, #878 ) ; 
#155 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#156 = ORIENTED_EDGE ( 'NONE', *, *, #534, .T. ) ; 
#157 = EDGE_CURVE ( 'NONE', #903, #272, #350, .T. ) ; 
#158 = VECTOR ( 'NONE', #387, 1000.000000000000000 ) ; 
#159 = VERTEX_POINT ( 'NONE', #674 ) ; 
#160 = ORIENTED_EDGE ( 'NONE', *, *, #108, .F. ) ; 
#161 = VERTEX_POINT ( 'NONE', #529 ) ; 
#162 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000004707, 0.000000000000000000, 1.500000000000004885 ) ) ; 
#163 = VECTOR ( 'NONE', #726, 1000.000000000000000 ) ; 
#164 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#165 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#166 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, -1.836970198720894780E-17, 7.480000000000004867 ) ) ; 
#167 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#168 = LINE ( 'NONE', #91, #79 ) ; 
#169 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, 1.000000000000000000 ) ) ; 
#170 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#171 = ORIENTED_EDGE ( 'NONE', *, *, #60, .T. ) ; 
#172 = VECTOR ( 'NONE', #320, 1000.000000000000000 ) ; 
#173 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, -40.49999999999999289 ) ) ; 
#174 = CYLINDRICAL_SURFACE ( 'NONE', #704, 1.599999999999998979 ) ; 
#175 = ORIENTED_EDGE ( 'NONE', *, *, #157, .T. ) ; 
#176 = LINE ( 'NONE', #196, #514 ) ; 
#177 = ORIENTED_EDGE ( 'NONE', *, *, #304, .T. ) ; 
#178 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#179 = ORIENTED_EDGE ( 'NONE', *, *, #844, .F. ) ; 
#180 = FACE_OUTER_BOUND ( 'NONE', #784, .T. ) ; 
#181 = EDGE_CURVE ( 'NONE', #531, #137, #102, .T. ) ; 
#182 = LINE ( 'NONE', #868, #279 ) ; 
#183 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#184 = EDGE_CURVE ( 'NONE', #536, #667, #791, .T. ) ; 
#185 = APPROVAL_STATUS ( 'not_yet_approved' ) ; 
#186 = LINE ( 'NONE', #625, #483 ) ; 
#187 = EDGE_LOOP ( 'NONE', ( #114, #38, #596, #850, #290, #125 ) ) ; 
#188 = CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT ( #183, #256, ( #326 ) ) ; 
#189 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#190 = ORIENTED_EDGE ( 'NONE', *, *, #778, .T. ) ; 
#191 = LINE ( 'NONE', #31, #392 ) ; 
#192 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#193 = EDGE_CURVE ( 'NONE', #684, #713, #210, .T. ) ; 
#194 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#195 = ORIENTED_EDGE ( 'NONE', *, *, #73, .F. ) ; 
#196 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, 1.500000000000004885 ) ) ; 
#197 = EDGE_CURVE ( 'NONE', #885, #161, #327, .T. ) ; 
#198 = VERTEX_POINT ( 'NONE', #664 ) ; 
#199 = EDGE_CURVE ( 'NONE', #118, #563, #213, .T. ) ; 
#200 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#201 = ORIENTED_EDGE ( 'NONE', *, *, #197, .F. ) ; 
#202 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, 7.480000000000004867 ) ) ; 
#203 = LINE ( 'NONE', #540, #561 ) ; 
#204 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#205 = VECTOR ( 'NONE', #208, 1000.000000000000000 ) ; 
#206 = ORIENTED_EDGE ( 'NONE', *, *, #197, .T. ) ; 
#207 = PLANE ( 'NONE',  #384 ) ; 
#208 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#209 = ORIENTED_EDGE ( 'NONE', *, *, #181, .F. ) ; 
#210 = CIRCLE ( 'NONE', #852, 1.599999999999998979 ) ; 
#211 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 0.000000000000000000, -13.49999999999999467 ) ) ; 
#212 = VECTOR ( 'NONE', #322, 1000.000000000000000 ) ; 
#213 = LINE ( 'NONE', #597, #139 ) ; 
#214 = LINE ( 'NONE', #343, #441 ) ; 
#215 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, 1.000000000000000000 ) ) ; 
#216 = PLANE ( 'NONE',  #288 ) ; 
#217 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#218 = ORIENTED_EDGE ( 'NONE', *, *, #733, .F. ) ; 
#219 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#220 = PLANE ( 'NONE',  #859 ) ; 
#221 = LOCAL_TIME ( 16, 9, 10.00000000000000000, #269 ) ; 
#222 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#223 = LINE ( 'NONE', #124, #766 ) ; 
#224 = ORIENTED_EDGE ( 'NONE', *, *, #764, .F. ) ; 
#225 = EDGE_CURVE ( 'NONE', #272, #25, #634, .T. ) ; 
#226 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 2.000000000000000000, -9.499999999999994671 ) ) ; 
#227 = DIRECTION ( 'NONE',  ( -1.927470528863119049E-16, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#228 = ORIENTED_EDGE ( 'NONE', *, *, #199, .F. ) ; 
#229 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, -1.000000000000000888, 7.480000000000004867 ) ) ; 
#230 = CC_DESIGN_APPROVAL ( #590, ( #793 ) ) ; 
#231 = LINE ( 'NONE', #271, #581 ) ; 
#232 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, 4.280000000000006466 ) ) ; 
#233 = ADVANCED_FACE ( 'NONE', ( #276 ), #247, .F. ) ; 
#234 = VECTOR ( 'NONE', #846, 1000.000000000000000 ) ; 
#235 = LINE ( 'NONE', #149, #205 ) ; 
#236 = DIRECTION ( 'NONE',  ( 1.284980352575412699E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#237 = EDGE_CURVE ( 'NONE', #423, #758, #424, .T. ) ; 
#238 = VECTOR ( 'NONE', #722, 1000.000000000000000 ) ; 
#239 = EDGE_CURVE ( 'NONE', #663, #660, #604, .T. ) ; 
#240 = EDGE_CURVE ( 'NONE', #335, #309, #572, .T. ) ; 
#241 = ORIENTED_EDGE ( 'NONE', *, *, #123, .T. ) ; 
#242 = CYLINDRICAL_SURFACE ( 'NONE', #513, 1.599999999999998090 ) ; 
#243 = VECTOR ( 'NONE', #130, 1000.000000000000000 ) ; 
#244 = ORIENTED_EDGE ( 'NONE', *, *, #831, .F. ) ; 
#245 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, 1.500000000000001332 ) ) ; 
#246 = PERSON_AND_ORGANIZATION_ROLE ( 'classification_officer' ) ; 
#247 = PLANE ( 'NONE',  #475 ) ; 
#248 = VERTEX_POINT ( 'NONE', #456 ) ; 
#249 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#250 = ORIENTED_EDGE ( 'NONE', *, *, #646, .F. ) ; 
#251 = APPROVAL_STATUS ( 'not_yet_approved' ) ; 
#252 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#253 = EDGE_CURVE ( 'NONE', #29, #367, #765, .T. ) ; 
#254 = VECTOR ( 'NONE', #249, 1000.000000000000000 ) ; 
#255 = CIRCLE ( 'NONE', #519, 1.599999999999998979 ) ; 
#256 = PERSON_AND_ORGANIZATION_ROLE ( 'design_supplier' ) ; 
#257 = VECTOR ( 'NONE', #685, 1000.000000000000000 ) ; 
#258 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, -1.000000000000000888, 4.280000000000006466 ) ) ; 
#259 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, -40.49999999999999289 ) ) ; 
#260 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#261 = ORIENTED_EDGE ( 'NONE', *, *, #237, .F. ) ; 
#262 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#263 = ORIENTED_EDGE ( 'NONE', *, *, #267, .F. ) ; 
#264 = VECTOR ( 'NONE', #477, 1000.000000000000000 ) ; 
#265 = VECTOR ( 'NONE', #731, 1000.000000000000000 ) ; 
#266 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, -1.836970198720894780E-17, 4.280000000000006466 ) ) ; 
#267 = EDGE_CURVE ( 'NONE', #869, #474, #119, .T. ) ; 
#268 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, 9.000000000000000000, 5.880000000000006111 ) ) ; 
#269 = COORDINATED_UNIVERSAL_TIME_OFFSET ( 1, 0, .AHEAD. ) ; 
#270 = ORIENTED_EDGE ( 'NONE', *, *, #803, .T. ) ; 
#271 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, 13.50000000000000178 ) ) ; 
#272 = VERTEX_POINT ( 'NONE', #189 ) ; 
#273 = EDGE_CURVE ( 'NONE', #137, #866, #482, .T. ) ; 
#274 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 0.000000000000000000, -9.499999999999994671 ) ) ; 
#275 = ORIENTED_EDGE ( 'NONE', *, *, #13, .F. ) ; 
#276 = FACE_OUTER_BOUND ( 'NONE', #71, .T. ) ; 
#277 = DIRECTION ( 'NONE',  ( 1.284980352575412699E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#278 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, -1.000000000000000888, 7.480000000000004867 ) ) ; 
#279 = VECTOR ( 'NONE', #306, 1000.000000000000000 ) ; 
#280 = CC_DESIGN_SECURITY_CLASSIFICATION ( #793, ( #326 ) ) ; 
#281 = VECTOR ( 'NONE', #200, 1000.000000000000000 ) ; 
#282 = ORIENTED_EDGE ( 'NONE', *, *, #600, .T. ) ; 
#283 = ADVANCED_FACE ( 'NONE', ( #45 ), #122, .F. ) ; 
#284 = APPROVAL ( #251, 'UNSPECIFIED' ) ; 
#285 = ORIENTED_EDGE ( 'NONE', *, *, #455, .F. ) ; 
#286 = ADVANCED_FACE ( 'NONE', ( #599 ), #812, .F. ) ; 
#287 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 0.000000000000000000, 1.500000000000004885 ) ) ; 
#288 = AXIS2_PLACEMENT_3D ( 'NONE', #356, #167, #16 ) ; 
#289 = ADVANCED_FACE ( 'NONE', ( #32 ), #802, .T. ) ; 
#290 = ORIENTED_EDGE ( 'NONE', *, *, #225, .T. ) ; 
#291 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#292 = LINE ( 'NONE', #15, #678 ) ; 
#293 = VERTEX_POINT ( 'NONE', #232 ) ; 
#294 = DIRECTION ( 'NONE',  ( 1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#295 = PLANE ( 'NONE',  #33 ) ; 
#296 = ORIENTED_EDGE ( 'NONE', *, *, #890, .F. ) ; 
#297 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#298 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#299 = ADVANCED_FACE ( 'NONE', ( #86 ), #752, .F. ) ; 
#300 = VECTOR ( 'NONE', #872, 1000.000000000000000 ) ; 
#301 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, 9.000000000000000000, 4.280000000000006466 ) ) ; 
#302 = ORGANIZATION ( 'UNSPECIFIED', 'UNSPECIFIED', '' ) ; 
#303 = VERTEX_POINT ( 'NONE', #893 ) ; 
#304 = EDGE_CURVE ( 'NONE', #815, #380, #101, .T. ) ; 
#305 = ADVANCED_FACE ( 'NONE', ( #732 ), #605, .F. ) ; 
#306 = DIRECTION ( 'NONE',  ( -1.927470528863119049E-16, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#307 = VERTEX_POINT ( 'NONE', #755 ) ; 
#308 = AXIS2_PLACEMENT_3D ( 'NONE', #896, #532, #675 ) ; 
#309 = VERTEX_POINT ( 'NONE', #454 ) ; 
#310 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#311 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#312 = AXIS2_PLACEMENT_3D ( 'NONE', #828, #705, #480 ) ; 
#313 = ORIENTED_EDGE ( 'NONE', *, *, #751, .F. ) ; 
#314 = ORIENTED_EDGE ( 'NONE', *, *, #904, .F. ) ; 
#315 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#316 = ORIENTED_EDGE ( 'NONE', *, *, #501, .T. ) ; 
#317 = EDGE_CURVE ( 'NONE', #815, #667, #363, .T. ) ; 
#318 = DIRECTION ( 'NONE',  ( 1.927470528863119049E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#319 = VECTOR ( 'NONE', #227, 1000.000000000000000 ) ; 
#320 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#321 = CALENDAR_DATE ( 2022, 13, 1 ) ; 
#322 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#323 = CYLINDRICAL_SURFACE ( 'NONE', #709, 1.599999999999998979 ) ; 
#324 = ORIENTED_EDGE ( 'NONE', *, *, #501, .F. ) ; 
#325 = DESIGN_CONTEXT ( 'detailed design', #697, 'design' ) ; 
#326 = PRODUCT_DEFINITION_FORMATION_WITH_SPECIFIED_SOURCE ( 'ANY', '', #636, .NOT_KNOWN. ) ; 
#327 = LINE ( 'NONE', #879, #158 ) ; 
#328 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 1.000000000000000000, -0.000000000000000000 ) ) ; 
#329 = CIRCLE ( 'NONE', #78, 1.599999999999998979 ) ; 
#330 = CC_DESIGN_DATE_AND_TIME_ASSIGNMENT ( #615, #3, ( #51 ) ) ; 
#331 = ORIENTED_EDGE ( 'NONE', *, *, #120, .F. ) ; 
#332 = LINE ( 'NONE', #400, #681 ) ; 
#333 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#334 = APPROVAL_DATE_TIME ( #821, #100 ) ; 
#335 = VERTEX_POINT ( 'NONE', #545 ) ; 
#336 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#337 = CLOSED_SHELL ( 'NONE', ( #151, #75, #283, #728, #9, #83, #645, #233, #286, #864, #566, #841, #651, #305, #564, #487, #5, #289, #435, #703, #492, #628, #26, #299, #560, #721 ) ) ; 
#338 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#339 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, 0.000000000000000000, 7.480000000000003979 ) ) ; 
#340 = ORIENTED_EDGE ( 'NONE', *, *, #48, .F. ) ; 
#341 = ORIENTED_EDGE ( 'NONE', *, *, #612, .F. ) ; 
#342 = VECTOR ( 'NONE', #853, 1000.000000000000000 ) ; 
#343 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 2.000000000000000000, 0.000000000000000000 ) ) ; 
#344 = COORDINATED_UNIVERSAL_TIME_OFFSET ( 1, 0, .AHEAD. ) ; 
#345 = LINE ( 'NONE', #757, #35 ) ; 
#346 = ORIENTED_EDGE ( 'NONE', *, *, #661, .T. ) ; 
#347 = DIRECTION ( 'NONE',  ( -1.927470528863119049E-16, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#348 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#349 = ORIENTED_EDGE ( 'NONE', *, *, #614, .F. ) ; 
#350 = LINE ( 'NONE', #509, #750 ) ; 
#351 = EDGE_LOOP ( 'NONE', ( #543, #881, #796, #1, #717, #688, #680, #107 ) ) ; 
#352 = EDGE_CURVE ( 'NONE', #14, #730, #460, .T. ) ; 
#353 = VECTOR ( 'NONE', #217, 1000.000000000000000 ) ; 
#354 = AXIS2_PLACEMENT_3D ( 'NONE', #268, #683, #537 ) ; 
#355 = CALENDAR_DATE ( 2022, 13, 1 ) ; 
#356 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 9.000000000000000000, 0.000000000000000000 ) ) ; 
#357 = ORIENTED_EDGE ( 'NONE', *, *, #253, .F. ) ; 
#358 = AXIS2_PLACEMENT_3D ( 'NONE', #760, #393, #603 ) ; 
#359 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, -1.000000000000000888, 7.480000000000004867 ) ) ; 
#360 = ORIENTED_EDGE ( 'NONE', *, *, #696, .F. ) ; 
#361 = ORIENTED_EDGE ( 'NONE', *, *, #682, .F. ) ; 
#362 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#363 = LINE ( 'NONE', #666, #254 ) ; 
#364 = DIRECTION ( 'NONE',  ( 1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#365 = EDGE_CURVE ( 'NONE', #105, #643, #203, .T. ) ; 
#366 = VECTOR ( 'NONE', #57, 1000.000000000000000 ) ; 
#367 = VERTEX_POINT ( 'NONE', #897 ) ; 
#368 = PLANE ( 'NONE',  #742 ) ; 
#369 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 1.000000000000000000, -0.000000000000000000 ) ) ; 
#370 = PRODUCT_RELATED_PRODUCT_CATEGORY ( 'detail', '', ( #636 ) ) ; 
#371 = EDGE_LOOP ( 'NONE', ( #457, #756, #856, #349, #133, #228, #552, #511 ) ) ; 
#372 = LINE ( 'NONE', #648, #319 ) ; 
#373 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 9.000000000000000000, 4.280000000000008242 ) ) ; 
#374 = CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT ( #194, #835, ( #51 ) ) ; 
#375 = ORIENTED_EDGE ( 'NONE', *, *, #13, .T. ) ; 
#376 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#377 = ORIENTED_EDGE ( 'NONE', *, *, #816, .T. ) ; 
#378 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#379 = AXIS2_PLACEMENT_3D ( 'NONE', #505, #712, #642 ) ; 
#380 = VERTEX_POINT ( 'NONE', #824 ) ; 
#381 = VECTOR ( 'NONE', #19, 1000.000000000000000 ) ; 
#382 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 2.000000000000000000, -13.49999999999999467 ) ) ; 
#383 = LINE ( 'NONE', #499, #342 ) ; 
#384 = AXIS2_PLACEMENT_3D ( 'NONE', #819, #204, #549 ) ; 
#385 = LINE ( 'NONE', #485, #88 ) ; 
#386 = ORIENTED_EDGE ( 'NONE', *, *, #110, .T. ) ; 
#387 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#388 = APPROVAL_ROLE ( '' ) ; 
#389 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#390 = ORIENTED_EDGE ( 'NONE', *, *, #365, .T. ) ; 
#391 = EDGE_CURVE ( 'NONE', #507, #248, #525, .T. ) ; 
#392 = VECTOR ( 'NONE', #579, 1000.000000000000000 ) ; 
#393 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#394 = FACE_OUTER_BOUND ( 'NONE', #98, .T. ) ; 
#395 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#396 = FACE_OUTER_BOUND ( 'NONE', #146, .T. ) ; 
#397 = AXIS2_PLACEMENT_3D ( 'NONE', #421, #827, #411 ) ; 
#398 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#399 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#400 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 0.000000000000000000, 7.480000000000003979 ) ) ; 
#401 =( GEOMETRIC_REPRESENTATION_CONTEXT ( 3 ) GLOBAL_UNCERTAINTY_ASSIGNED_CONTEXT ( ( #773 ) ) GLOBAL_UNIT_ASSIGNED_CONTEXT ( ( #559, #775, #702 ) ) REPRESENTATION_CONTEXT ( 'NONE', 'WORKASPACE' ) ); 
#402 = ORIENTED_EDGE ( 'NONE', *, *, #486, .T. ) ; 
#403 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, 7.480000000000004867 ) ) ; 
#404 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#405 = VECTOR ( 'NONE', #729, 1000.000000000000000 ) ; 
#406 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#407 = COORDINATED_UNIVERSAL_TIME_OFFSET ( 1, 0, .AHEAD. ) ; 
#408 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, 0.000000000000000000, 7.480000000000004867 ) ) ; 
#409 = LINE ( 'NONE', #259, #595 ) ; 
#410 = ORIENTED_EDGE ( 'NONE', *, *, #716, .F. ) ; 
#411 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#412 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#413 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 0.000000000000000000, 1.500000000000004885 ) ) ; 
#414 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, 13.50000000000000533 ) ) ; 
#415 = APPLICATION_PROTOCOL_DEFINITION ( 'international standard', 'config_control_design', 1994, #830 ) ; 
#416 = DIRECTION ( 'NONE',  ( -1.000000000000000000, -0.000000000000000000, -4.565061778886333058E-16 ) ) ; 
#417 = ORIENTED_EDGE ( 'NONE', *, *, #612, .T. ) ; 
#418 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, 1.000000000000000000 ) ) ; 
#419 = VECTOR ( 'NONE', #170, 1000.000000000000000 ) ; 
#420 = LINE ( 'NONE', #622, #746 ) ; 
#421 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#422 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#423 = VERTEX_POINT ( 'NONE', #126 ) ; 
#424 = LINE ( 'NONE', #580, #53 ) ; 
#425 = AXIS2_PLACEMENT_3D ( 'NONE', #46, #522, #315 ) ; 
#426 = LINE ( 'NONE', #585, #720 ) ; 
#427 = COORDINATED_UNIVERSAL_TIME_OFFSET ( 1, 0, .AHEAD. ) ; 
#428 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#429 = LINE ( 'NONE', #339, #551 ) ; 
#430 = ORIENTED_EDGE ( 'NONE', *, *, #534, .F. ) ; 
#431 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, -1.000000000000000888, 4.280000000000006466 ) ) ; 
#432 = ORIENTED_EDGE ( 'NONE', *, *, #748, .F. ) ; 
#433 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 0.000000000000000000, -9.499999999999994671 ) ) ; 
#434 = ORIENTED_EDGE ( 'NONE', *, *, #584, .F. ) ; 
#435 = ADVANCED_FACE ( 'NONE', ( #807 ), #128, .F. ) ; 
#436 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#437 = LINE ( 'NONE', #111, #74 ) ; 
#438 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, -1.284980352575412699E-16 ) ) ; 
#439 = LINE ( 'NONE', #858, #234 ) ; 
#440 = EDGE_CURVE ( 'NONE', #848, #159, #753, .T. ) ; 
#441 = VECTOR ( 'NONE', #556, 1000.000000000000000 ) ; 
#442 = EDGE_LOOP ( 'NONE', ( #44, #854, #546, #494 ) ) ; 
#443 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#444 = AXIS2_PLACEMENT_3D ( 'NONE', #863, #438, #30 ) ; 
#445 = EDGE_LOOP ( 'NONE', ( #331, #467, #285, #341 ) ) ; 
#446 = ORIENTED_EDGE ( 'NONE', *, *, #748, .T. ) ; 
#447 = LINE ( 'NONE', #8, #264 ) ; 
#448 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#449 = COORDINATED_UNIVERSAL_TIME_OFFSET ( 1, 0, .AHEAD. ) ; 
#450 = SHAPE_DEFINITION_REPRESENTATION ( #727, #738 ) ; 
#451 = ORIENTED_EDGE ( 'NONE', *, *, #41, .F. ) ; 
#452 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#453 = ORIENTED_EDGE ( 'NONE', *, *, #317, .F. ) ; 
#454 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 2.000000000000000000, 1.500000000000004885 ) ) ; 
#455 = EDGE_CURVE ( 'NONE', #826, #335, #626, .T. ) ; 
#456 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, 9.000000000000000000, 7.480000000000003091 ) ) ; 
#457 = ORIENTED_EDGE ( 'NONE', *, *, #110, .F. ) ; 
#458 = ORIENTED_EDGE ( 'NONE', *, *, #440, .T. ) ; 
#459 = PERSON_AND_ORGANIZATION_ROLE ( 'creator' ) ; 
#460 = LINE ( 'NONE', #749, #212 ) ; 
#461 = SECURITY_CLASSIFICATION_LEVEL ( 'unclassified' ) ; 
#462 = VECTOR ( 'NONE', #686, 1000.000000000000000 ) ; 
#463 = ORIENTED_EDGE ( 'NONE', *, *, #662, .T. ) ; 
#464 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#465 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#466 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#467 = ORIENTED_EDGE ( 'NONE', *, *, #600, .F. ) ; 
#468 = CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT ( #845, #246, ( #793 ) ) ; 
#469 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 9.000000000000000000, -11.09999999999999254 ) ) ; 
#470 = EDGE_CURVE ( 'NONE', #848, #598, #191, .T. ) ; 
#471 = EDGE_LOOP ( 'NONE', ( #673, #804, #361, #360, #524, #706, #458, #417 ) ) ; 
#472 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#473 = DATE_AND_TIME ( #321, #734 ) ; 
#474 = VERTEX_POINT ( 'NONE', #571 ) ; 
#475 = AXIS2_PLACEMENT_3D ( 'NONE', #745, #805, #318 ) ; 
#476 = ORIENTED_EDGE ( 'NONE', *, *, #240, .F. ) ; 
#477 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#478 = EDGE_LOOP ( 'NONE', ( #70, #530, #241, #694, #106, #762 ) ) ; 
#479 = AXIS2_PLACEMENT_3D ( 'NONE', #640, #18, #862 ) ; 
#480 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#481 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 0.000000000000000000, 4.280000000000006466 ) ) ; 
#482 = LINE ( 'NONE', #229, #592 ) ; 
#483 = VECTOR ( 'NONE', #771, 1000.000000000000000 ) ; 
#484 = ORIENTED_EDGE ( 'NONE', *, *, #701, .T. ) ; 
#485 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#486 = EDGE_CURVE ( 'NONE', #307, #866, #372, .T. ) ; 
#487 = ADVANCED_FACE ( 'NONE', ( #832 ), #368, .F. ) ; 
#488 = ORIENTED_EDGE ( 'NONE', *, *, #887, .T. ) ; 
#489 = PLANE ( 'NONE',  #873 ) ; 
#490 = EDGE_CURVE ( 'NONE', #563, #198, #383, .T. ) ; 
#491 = ORIENTED_EDGE ( 'NONE', *, *, #317, .T. ) ; 
#492 = ADVANCED_FACE ( 'NONE', ( #548 ), #907, .F. ) ; 
#493 = DATE_AND_TIME ( #710, #638 ) ; 
#494 = ORIENTED_EDGE ( 'NONE', *, *, #73, .T. ) ; 
#495 = PLANE ( 'NONE',  #606 ) ; 
#496 = ORIENTED_EDGE ( 'NONE', *, *, #136, .F. ) ; 
#497 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#498 = ORIENTED_EDGE ( 'NONE', *, *, #267, .T. ) ; 
#499 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, 1.500000000000001332 ) ) ; 
#500 = PLANE ( 'NONE',  #379 ) ; 
#501 = EDGE_CURVE ( 'NONE', #303, #507, #21, .T. ) ; 
#502 = FACE_OUTER_BOUND ( 'NONE', #817, .T. ) ; 
#503 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 4.500000000000000000, 0.000000000000000000 ) ) ; 
#504 = LOCAL_TIME ( 16, 9, 10.00000000000000000, #344 ) ; 
#505 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#506 = LINE ( 'NONE', #607, #257 ) ; 
#507 = VERTEX_POINT ( 'NONE', #653 ) ; 
#508 = FACE_OUTER_BOUND ( 'NONE', #567, .T. ) ; 
#509 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#510 = CIRCLE ( 'NONE', #801, 1.599999999999997202 ) ; 
#511 = ORIENTED_EDGE ( 'NONE', *, *, #871, .F. ) ; 
#512 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#513 = AXIS2_PLACEMENT_3D ( 'NONE', #589, #657, #583 ) ; 
#514 = VECTOR ( 'NONE', #822, 1000.000000000000000 ) ; 
#515 = APPROVAL_PERSON_ORGANIZATION ( #617, #100, #147 ) ; 
#516 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#517 = ORIENTED_EDGE ( 'NONE', *, *, #661, .F. ) ; 
#518 = ORIENTED_EDGE ( 'NONE', *, *, #41, .T. ) ; 
#519 = AXIS2_PLACEMENT_3D ( 'NONE', #842, #164, #428 ) ; 
#520 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#521 = EDGE_LOOP ( 'NONE', ( #576, #476, #282, #498, #451, #127 ) ) ; 
#522 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#523 = EDGE_LOOP ( 'NONE', ( #177, #725 ) ) ; 
#524 = ORIENTED_EDGE ( 'NONE', *, *, #123, .F. ) ; 
#525 = CIRCLE ( 'NONE', #354, 1.599999999999997202 ) ; 
#526 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 9.000000000000000000, -9.499999999999994671 ) ) ; 
#527 = LINE ( 'NONE', #211, #776 ) ; 
#528 = ORIENTED_EDGE ( 'NONE', *, *, #120, .T. ) ; 
#529 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#530 = ORIENTED_EDGE ( 'NONE', *, *, #887, .F. ) ; 
#531 = VERTEX_POINT ( 'NONE', #166 ) ; 
#532 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#533 = LINE ( 'NONE', #735, #265 ) ; 
#534 = EDGE_CURVE ( 'NONE', #885, #307, #141, .T. ) ; 
#535 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 2.000000000000000000, 0.000000000000000000 ) ) ; 
#536 = VERTEX_POINT ( 'NONE', #865 ) ; 
#537 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#538 = ORIENTED_EDGE ( 'NONE', *, *, #455, .T. ) ; 
#539 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, -1.284980352575412699E-16 ) ) ; 
#540 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 9.000000000000000000, 4.280000000000006466 ) ) ; 
#541 = AXIS2_PLACEMENT_3D ( 'NONE', #619, #609, #676 ) ; 
#542 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 9.000000000000000000, 7.480000000000004867 ) ) ; 
#543 = ORIENTED_EDGE ( 'NONE', *, *, #662, .F. ) ; 
#544 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#545 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#546 = ORIENTED_EDGE ( 'NONE', *, *, #569, .F. ) ; 
#547 = ORIENTED_EDGE ( 'NONE', *, *, #225, .F. ) ; 
#548 = FACE_OUTER_BOUND ( 'NONE', #43, .T. ) ; 
#549 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#550 = APPROVAL_DATE_TIME ( #473, #284 ) ; 
#551 = VECTOR ( 'NONE', #333, 1000.000000000000000 ) ; 
#552 = ORIENTED_EDGE ( 'NONE', *, *, #555, .F. ) ; 
#553 = AXIS2_PLACEMENT_3D ( 'NONE', #466, #814, #399 ) ; 
#554 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#555 = EDGE_CURVE ( 'NONE', #161, #118, #857, .T. ) ; 
#556 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#557 = APPROVAL_STATUS ( 'not_yet_approved' ) ; 
#558 = VERTEX_POINT ( 'NONE', #781 ) ; 
#559 =( LENGTH_UNIT ( ) NAMED_UNIT ( * ) SI_UNIT ( .MILLI., .METRE. ) ); 
#560 = ADVANCED_FACE ( 'NONE', ( #798 ), #220, .T. ) ; 
#561 = VECTOR ( 'NONE', #687, 1000.000000000000000 ) ; 
#562 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#563 = VERTEX_POINT ( 'NONE', #162 ) ; 
#564 = ADVANCED_FACE ( 'NONE', ( #883 ), #42, .F. ) ; 
#565 = ORIENTED_EDGE ( 'NONE', *, *, #763, .T. ) ; 
#566 = ADVANCED_FACE ( 'NONE', ( #502 ), #849, .F. ) ; 
#567 = EDGE_LOOP ( 'NONE', ( #891, #570, #691, #453 ) ) ; 
#568 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#569 = EDGE_CURVE ( 'NONE', #718, #198, #533, .T. ) ; 
#570 = ORIENTED_EDGE ( 'NONE', *, *, #890, .T. ) ; 
#571 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 0.000000000000000000, 1.500000000000004885 ) ) ; 
#572 = LINE ( 'NONE', #669, #62 ) ; 
#573 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#574 = APPROVAL_PERSON_ORGANIZATION ( #874, #590, #388 ) ; 
#575 = FACE_OUTER_BOUND ( 'NONE', #677, .T. ) ; 
#576 = ORIENTED_EDGE ( 'NONE', *, *, #900, .T. ) ; 
#577 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#578 = AXIS2_PLACEMENT_3D ( 'NONE', #36, #311, #797 ) ; 
#579 = DIRECTION ( 'NONE',  ( -1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#580 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 4.280000000000006466 ) ) ; 
#581 = VECTOR ( 'NONE', #416, 1000.000000000000000 ) ; 
#582 = VECTOR ( 'NONE', #364, 1000.000000000000000 ) ; 
#583 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#584 = EDGE_CURVE ( 'NONE', #6, #137, #182, .T. ) ; 
#585 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#586 = LINE ( 'NONE', #715, #867 ) ; 
#587 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#588 = VERTEX_POINT ( 'NONE', #443 ) ; 
#589 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, -0.000000000000000000, 5.880000000000006111 ) ) ; 
#590 = APPROVAL ( #185, 'UNSPECIFIED' ) ; 
#591 = DATE_TIME_ROLE ( 'classification_date' ) ; 
#592 = VECTOR ( 'NONE', #369, 1000.000000000000000 ) ; 
#593 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#594 = LOCAL_TIME ( 16, 9, 10.00000000000000000, #407 ) ; 
#595 = VECTOR ( 'NONE', #465, 1000.000000000000000 ) ; 
#596 = ORIENTED_EDGE ( 'NONE', *, *, #352, .F. ) ; 
#597 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, -0.000000000000000000, -40.49999999999999289 ) ) ; 
#598 = VERTEX_POINT ( 'NONE', #376 ) ; 
#599 = FACE_OUTER_BOUND ( 'NONE', #471, .T. ) ; 
#600 = EDGE_CURVE ( 'NONE', #335, #869, #426, .T. ) ; 
#601 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#602 = FACE_BOUND ( 'NONE', #780, .T. ) ; 
#603 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#604 = LINE ( 'NONE', #382, #774 ) ; 
#605 = PLANE ( 'NONE',  #668 ) ; 
#606 = AXIS2_PLACEMENT_3D ( 'NONE', #278, #770, #616 ) ; 
#607 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, -1.000000000000000888, 4.280000000000006466 ) ) ; 
#608 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#609 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#610 = VERTEX_POINT ( 'NONE', #655 ) ; 
#611 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, -1.000000000000000888, 4.280000000000006466 ) ) ; 
#612 = EDGE_CURVE ( 'NONE', #159, #826, #695, .T. ) ; 
#613 = LINE ( 'NONE', #310, #300 ) ; 
#614 = EDGE_CURVE ( 'NONE', #730, #293, #437, .T. ) ; 
#615 = DATE_AND_TIME ( #355, #504 ) ; 
#616 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#617 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#618 = EDGE_LOOP ( 'NONE', ( #839, #390, #630, #261 ) ) ; 
#619 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 4.500000000000000000, 0.000000000000000000 ) ) ; 
#620 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#621 = ORIENTED_EDGE ( 'NONE', *, *, #904, .T. ) ; 
#622 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 9.000000000000000000, 7.480000000000004867 ) ) ; 
#623 = PERSON_AND_ORGANIZATION_ROLE ( 'design_owner' ) ; 
#624 = ORIENTED_EDGE ( 'NONE', *, *, #795, .T. ) ; 
#625 = CARTESIAN_POINT ( 'NONE',  ( 1.496986094846181597E-15, 0.000000000000000000, -7.899999999999995914 ) ) ; 
#626 = LINE ( 'NONE', #297, #405 ) ; 
#627 = LINE ( 'NONE', #855, #76 ) ; 
#628 = ADVANCED_FACE ( 'NONE', ( #789 ), #242, .F. ) ; 
#629 = VECTOR ( 'NONE', #328, 1000.000000000000000 ) ; 
#630 = ORIENTED_EDGE ( 'NONE', *, *, #779, .F. ) ; 
#631 = EDGE_LOOP ( 'NONE', ( #50, #97, #175, #270 ) ) ; 
#632 = EDGE_CURVE ( 'NONE', #810, #885, #613, .T. ) ; 
#633 = MANIFOLD_SOLID_BREP ( 'Mirror2', #337 ) ; 
#634 = LINE ( 'NONE', #808, #172 ) ; 
#635 = FACE_OUTER_BOUND ( 'NONE', #371, .T. ) ; 
#636 = PRODUCT ( 'delanclip-gopro-adapter', 'delanclip-gopro-adapter', '', ( #68 ) ) ; 
#637 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#638 = LOCAL_TIME ( 16, 9, 10.00000000000000000, #427 ) ; 
#639 = ORIENTED_EDGE ( 'NONE', *, *, #184, .T. ) ; 
#640 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000001243, 0.000000000000000000, 5.880000000000006111 ) ) ; 
#641 = ORIENTED_EDGE ( 'NONE', *, *, #391, .F. ) ; 
#642 = DIRECTION ( 'NONE',  ( 1.927470528863119049E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#643 = VERTEX_POINT ( 'NONE', #373 ) ; 
#644 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 9.000000000000000000, -9.499999999999994671 ) ) ; 
#645 = ADVANCED_FACE ( 'NONE', ( #55 ), #792, .F. ) ; 
#646 = EDGE_CURVE ( 'NONE', #610, #718, #231, .T. ) ; 
#647 = ORIENTED_EDGE ( 'NONE', *, *, #365, .F. ) ; 
#648 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#649 = VECTOR ( 'NONE', #714, 1000.000000000000000 ) ; 
#650 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#651 = ADVANCED_FACE ( 'NONE', ( #772 ), #207, .F. ) ; 
#652 = ORIENTED_EDGE ( 'NONE', *, *, #157, .F. ) ; 
#653 = CARTESIAN_POINT ( 'NONE',  ( -6.400000000000006573, 9.000000000000000000, 4.280000000000006466 ) ) ; 
#654 = EDGE_CURVE ( 'NONE', #563, #730, #176, .T. ) ; 
#655 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, 13.50000000000000533 ) ) ; 
#656 = ORIENTED_EDGE ( 'NONE', *, *, #778, .F. ) ; 
#657 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#658 = APPROVAL_DATE_TIME ( #143, #590 ) ; 
#659 = LINE ( 'NONE', #690, #49 ) ; 
#660 = VERTEX_POINT ( 'NONE', #698 ) ; 
#661 = EDGE_CURVE ( 'NONE', #531, #558, #332, .T. ) ; 
#662 = EDGE_CURVE ( 'NONE', #309, #663, #707, .T. ) ; 
#663 = VERTEX_POINT ( 'NONE', #759 ) ; 
#664 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, 1.500000000000001332 ) ) ; 
#665 = ORIENTED_EDGE ( 'NONE', *, *, #273, .T. ) ; 
#666 = CARTESIAN_POINT ( 'NONE',  ( 1.301042606982605321E-15, 0.000000000000000000, -11.09999999999999432 ) ) ; 
#667 = VERTEX_POINT ( 'NONE', #132 ) ; 
#668 = AXIS2_PLACEMENT_3D ( 'NONE', #260, #587, #740 ) ; 
#669 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#670 = VECTOR ( 'NONE', #39, 1000.000000000000000 ) ; 
#671 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#672 = CIRCLE ( 'NONE', #358, 1.599999999999997202 ) ; 
#673 = ORIENTED_EDGE ( 'NONE', *, *, #831, .T. ) ; 
#674 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#675 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#676 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#677 = EDGE_LOOP ( 'NONE', ( #250, #375, #190, #218, #430, #206, #700, #195 ) ) ; 
#678 = VECTOR ( 'NONE', #608, 1000.000000000000000 ) ; 
#679 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#680 = ORIENTED_EDGE ( 'NONE', *, *, #751, .T. ) ; 
#681 = VECTOR ( 'NONE', #338, 1000.000000000000000 ) ; 
#682 = EDGE_CURVE ( 'NONE', #903, #660, #153, .T. ) ; 
#683 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#684 = VERTEX_POINT ( 'NONE', #266 ) ; 
#685 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 1.000000000000000000, -0.000000000000000000 ) ) ; 
#686 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#687 = DIRECTION ( 'NONE',  ( -1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#688 = ORIENTED_EDGE ( 'NONE', *, *, #11, .T. ) ; 
#689 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 0.000000000000000000, 4.280000000000007354 ) ) ; 
#690 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000003908, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#691 = ORIENTED_EDGE ( 'NONE', *, *, #93, .F. ) ; 
#692 = EDGE_LOOP ( 'NONE', ( #296, #767, #491, #806 ) ) ; 
#693 = PLANE ( 'NONE',  #769 ) ; 
#694 = ORIENTED_EDGE ( 'NONE', *, *, #737, .F. ) ; 
#695 = LINE ( 'NONE', #178, #670 ) ; 
#696 = EDGE_CURVE ( 'NONE', #588, #903, #292, .T. ) ; 
#697 = APPLICATION_CONTEXT ( 'configuration controlled 3d designs of mechanical parts and assemblies' ) ; 
#698 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 2.000000000000000000, -13.49999999999999467 ) ) ; 
#699 = LINE ( 'NONE', #620, #809 ) ; 
#700 = ORIENTED_EDGE ( 'NONE', *, *, #555, .T. ) ; 
#701 = EDGE_CURVE ( 'NONE', #293, #684, #799, .T. ) ; 
#702 =( NAMED_UNIT ( * ) SI_UNIT ( $, .STERADIAN. ) SOLID_ANGLE_UNIT ( ) ); 
#703 = ADVANCED_FACE ( 'NONE', ( #833 ), #323, .F. ) ; 
#704 = AXIS2_PLACEMENT_3D ( 'NONE', #433, #422, #452 ) ; 
#705 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#706 = ORIENTED_EDGE ( 'NONE', *, *, #470, .F. ) ; 
#707 = LINE ( 'NONE', #85, #870 ) ; 
#708 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000004707, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#709 = AXIS2_PLACEMENT_3D ( 'NONE', #274, #61, #404 ) ; 
#710 = CALENDAR_DATE ( 2022, 13, 1 ) ; 
#711 = ORIENTED_EDGE ( 'NONE', *, *, #136, .T. ) ; 
#712 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 1.927470528863119049E-16 ) ) ; 
#713 = VERTEX_POINT ( 'NONE', #408 ) ; 
#714 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#715 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 7.480000000000004867 ) ) ; 
#716 = EDGE_CURVE ( 'NONE', #684, #507, #24, .T. ) ; 
#717 = ORIENTED_EDGE ( 'NONE', *, *, #569, .T. ) ; 
#718 = VERTEX_POINT ( 'NONE', #135 ) ; 
#719 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000002132, 0.000000000000000000, 7.480000000000004867 ) ) ; 
#720 = VECTOR ( 'NONE', #876, 1000.000000000000000 ) ; 
#721 = ADVANCED_FACE ( 'NONE', ( #396 ), #495, .T. ) ; 
#722 = DIRECTION ( 'NONE',  ( -1.927470528863119049E-16, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#723 = AXIS2_PLACEMENT_3D ( 'NONE', #173, #378, #448 ) ; 
#724 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#725 = ORIENTED_EDGE ( 'NONE', *, *, #92, .T. ) ; 
#726 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 1.000000000000000000, 0.000000000000000000 ) ) ; 
#727 = PRODUCT_DEFINITION_SHAPE ( 'NONE', 'NONE',  #51 ) ; 
#728 = ADVANCED_FACE ( 'NONE', ( #112 ), #142, .F. ) ; 
#729 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#730 = VERTEX_POINT ( 'NONE', #413 ) ; 
#731 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -0.000000000000000000, -1.000000000000000000 ) ) ; 
#732 = FACE_OUTER_BOUND ( 'NONE', #67, .T. ) ; 
#733 = EDGE_CURVE ( 'NONE', #307, #6, #659, .T. ) ; 
#734 = LOCAL_TIME ( 16, 9, 10.00000000000000000, #449 ) ; 
#735 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, -40.49999999999999289 ) ) ; 
#736 = VERTEX_POINT ( 'NONE', #202 ) ; 
#737 = EDGE_CURVE ( 'NONE', #14, #588, #884, .T. ) ; 
#738 = ADVANCED_BREP_SHAPE_REPRESENTATION ( 'delanclip-gopro-adapter', ( #633, #312 ), #401 ) ; 
#739 = ORIENTED_EDGE ( 'NONE', *, *, #779, .T. ) ; 
#740 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#741 = ORIENTED_EDGE ( 'NONE', *, *, #181, .T. ) ; 
#742 = AXIS2_PLACEMENT_3D ( 'NONE', #503, #27, #169 ) ; 
#743 = ORIENTED_EDGE ( 'NONE', *, *, #871, .T. ) ; 
#744 = VECTOR ( 'NONE', #90, 1000.000000000000000 ) ; 
#745 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#746 = VECTOR ( 'NONE', #905, 1000.000000000000000 ) ; 
#747 = ORIENTED_EDGE ( 'NONE', *, *, #391, .T. ) ; 
#748 = EDGE_CURVE ( 'NONE', #558, #866, #420, .T. ) ; 
#749 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#750 = VECTOR ( 'NONE', #389, 1000.000000000000000 ) ; 
#751 = EDGE_CURVE ( 'NONE', #25, #660, #214, .T. ) ; 
#752 = PLANE ( 'NONE',  #837 ) ; 
#753 = LINE ( 'NONE', #362, #744 ) ; 
#754 = FACE_OUTER_BOUND ( 'NONE', #901, .T. ) ; 
#755 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000003908, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#756 = ORIENTED_EDGE ( 'NONE', *, *, #193, .F. ) ; 
#757 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 4.500000000000000000, -13.49999999999999467 ) ) ; 
#758 = VERTEX_POINT ( 'NONE', #481 ) ; 
#759 = CARTESIAN_POINT ( 'NONE',  ( 2.450000000000000622, 2.000000000000000000, -13.49999999999999467 ) ) ; 
#760 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 9.000000000000000000, 5.880000000000006111 ) ) ; 
#761 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 4.280000000000006466 ) ) ; 
#762 = ORIENTED_EDGE ( 'NONE', *, *, #614, .T. ) ; 
#763 = EDGE_CURVE ( 'NONE', #713, #248, #429, .T. ) ; 
#764 = EDGE_CURVE ( 'NONE', #423, #105, #104, .T. ) ; 
#765 = LINE ( 'NONE', #245, #163 ) ; 
#766 = VECTOR ( 'NONE', #825, 1000.000000000000000 ) ; 
#767 = ORIENTED_EDGE ( 'NONE', *, *, #304, .F. ) ; 
#768 = DIRECTION ( 'NONE',  ( -1.284980352575412699E-16, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#769 = AXIS2_PLACEMENT_3D ( 'NONE', #818, #336, #895 ) ; 
#770 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#771 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#772 = FACE_OUTER_BOUND ( 'NONE', #442, .T. ) ; 
#773 = UNCERTAINTY_MEASURE_WITH_UNIT (LENGTH_MEASURE( 1.000000000000000082E-05 ), #559, 'distance_accuracy_value', 'NONE'); 
#774 = VECTOR ( 'NONE', #252, 1000.000000000000000 ) ; 
#775 =( NAMED_UNIT ( * ) PLANE_ANGLE_UNIT ( ) SI_UNIT ( $, .RADIAN. ) ); 
#776 = VECTOR ( 'NONE', #347, 1000.000000000000000 ) ; 
#777 = PERSON ( 'UNSPECIFIED', 'UNSPECIFIED', 'UNSPECIFIED', ('UNSPECIFIED'), ('UNSPECIFIED'), ('UNSPECIFIED') ) ; 
#778 = EDGE_CURVE ( 'NONE', #788, #6, #847, .T. ) ; 
#779 = EDGE_CURVE ( 'NONE', #758, #643, #783, .T. ) ; 
#780 = EDGE_LOOP ( 'NONE', ( #138, #639 ) ) ; 
#781 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, 9.000000000000000000, 7.480000000000004867 ) ) ; 
#782 = VECTOR ( 'NONE', #66, 1000.000000000000000 ) ; 
#783 = LINE ( 'NONE', #689, #243 ) ; 
#784 = EDGE_LOOP ( 'NONE', ( #463, #244, #538, #152 ) ) ; 
#785 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#786 = EDGE_LOOP ( 'NONE', ( #484, #84, #324, #179 ) ) ; 
#787 = EDGE_CURVE ( 'NONE', #367, #610, #409, .T. ) ; 
#788 = VERTEX_POINT ( 'NONE', #406 ) ; 
#789 = FACE_OUTER_BOUND ( 'NONE', #823, .T. ) ; 
#790 = AXIS2_PLACEMENT_3D ( 'NONE', #526, #520, #593 ) ; 
#791 = CIRCLE ( 'NONE', #425, 1.599999999999998979 ) ; 
#792 = PLANE ( 'NONE',  #578 ) ; 
#793 = SECURITY_CLASSIFICATION ( '', '', #461 ) ; 
#794 = FACE_OUTER_BOUND ( 'NONE', #89, .T. ) ; 
#795 = EDGE_CURVE ( 'NONE', #788, #29, #699, .T. ) ; 
#796 = ORIENTED_EDGE ( 'NONE', *, *, #787, .T. ) ; 
#797 = DIRECTION ( 'NONE',  ( 0.000000000000000000, -0.000000000000000000, 1.000000000000000000 ) ) ; 
#798 = FACE_OUTER_BOUND ( 'NONE', #786, .T. ) ; 
#799 = LINE ( 'NONE', #761, #462 ) ; 
#800 = ORIENTED_EDGE ( 'NONE', *, *, #795, .F. ) ; 
#801 = AXIS2_PLACEMENT_3D ( 'NONE', #644, #568, #497 ) ; 
#802 = PLANE ( 'NONE',  #553 ) ; 
#803 = EDGE_CURVE ( 'NONE', #272, #14, #223, .T. ) ; 
#804 = ORIENTED_EDGE ( 'NONE', *, *, #239, .T. ) ; 
#805 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 1.927470528863119049E-16 ) ) ; 
#806 = ORIENTED_EDGE ( 'NONE', *, *, #184, .F. ) ; 
#807 = FACE_OUTER_BOUND ( 'NONE', #445, .T. ) ; 
#808 = CARTESIAN_POINT ( 'NONE',  ( -2.450000000000000622, 4.500000000000000000, 1.500000000000004885 ) ) ; 
#809 = VECTOR ( 'NONE', #892, 1000.000000000000000 ) ; 
#810 = VERTEX_POINT ( 'NONE', #403 ) ; 
#811 = LINE ( 'NONE', #12, #888 ) ; 
#812 = PLANE ( 'NONE',  #308 ) ; 
#813 = DIRECTION ( 'NONE',  ( -0.000000000000000000, -1.000000000000000000, -0.000000000000000000 ) ) ; 
#814 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#815 = VERTEX_POINT ( 'NONE', #469 ) ; 
#816 = EDGE_CURVE ( 'NONE', #558, #643, #672, .T. ) ; 
#817 = EDGE_LOOP ( 'NONE', ( #357, #800, #275, #840 ) ) ; 
#818 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#819 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, -40.49999999999999289 ) ) ; 
#820 = LINE ( 'NONE', #414, #381 ) ; 
#821 = DATE_AND_TIME ( #131, #594 ) ; 
#822 = DIRECTION ( 'NONE',  ( -1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#823 = EDGE_LOOP ( 'NONE', ( #894, #739, #121, #517 ) ) ; 
#824 = CARTESIAN_POINT ( 'NONE',  ( 1.496986094846181597E-15, 9.000000000000000000, -7.899999999999997691 ) ) ; 
#825 = DIRECTION ( 'NONE',  ( -1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#826 = VERTEX_POINT ( 'NONE', #679 ) ; 
#827 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#828 = CARTESIAN_POINT ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#829 = ORIENTED_EDGE ( 'NONE', *, *, #60, .F. ) ; 
#830 = APPLICATION_CONTEXT ( 'configuration controlled 3d designs of mechanical parts and assemblies' ) ; 
#831 = EDGE_CURVE ( 'NONE', #826, #663, #345, .T. ) ; 
#832 = FACE_OUTER_BOUND ( 'NONE', #631, .T. ) ; 
#833 = FACE_OUTER_BOUND ( 'NONE', #692, .T. ) ; 
#834 = ORIENTED_EDGE ( 'NONE', *, *, #237, .T. ) ; 
#835 = PERSON_AND_ORGANIZATION_ROLE ( 'creator' ) ; 
#836 = VECTOR ( 'NONE', #851, 1000.000000000000000 ) ; 
#837 = AXIS2_PLACEMENT_3D ( 'NONE', #258, #192, #395 ) ; 
#838 = PLANE ( 'NONE',  #397 ) ; 
#839 = ORIENTED_EDGE ( 'NONE', *, *, #764, .T. ) ; 
#840 = ORIENTED_EDGE ( 'NONE', *, *, #787, .F. ) ; 
#841 = ADVANCED_FACE ( 'NONE', ( #575 ), #693, .F. ) ; 
#842 = CARTESIAN_POINT ( 'NONE',  ( 6.400000000000006573, -1.836970198720894780E-17, 5.880000000000006111 ) ) ; 
#843 = AXIS2_PLACEMENT_3D ( 'NONE', #535, #412, #418 ) ; 
#844 = EDGE_CURVE ( 'NONE', #293, #303, #506, .T. ) ; 
#845 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#846 = DIRECTION ( 'NONE',  ( -0.000000000000000000, 1.000000000000000000, -0.000000000000000000 ) ) ; 
#847 = LINE ( 'NONE', #398, #366 ) ; 
#848 = VERTEX_POINT ( 'NONE', #902 ) ; 
#849 = PLANE ( 'NONE',  #723 ) ; 
#850 = ORIENTED_EDGE ( 'NONE', *, *, #803, .F. ) ; 
#851 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#852 = AXIS2_PLACEMENT_3D ( 'NONE', #103, #52, #889 ) ; 
#853 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 1.000000000000000000, -0.000000000000000000 ) ) ; 
#854 = ORIENTED_EDGE ( 'NONE', *, *, #490, .T. ) ; 
#855 = CARTESIAN_POINT ( 'NONE',  ( -3.800000000000005151, 2.000000000000000000, 13.50000000000000178 ) ) ; 
#856 = ORIENTED_EDGE ( 'NONE', *, *, #701, .F. ) ; 
#857 = LINE ( 'NONE', #436, #649 ) ; 
#858 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, -1.000000000000000888, 7.480000000000004867 ) ) ; 
#859 = AXIS2_PLACEMENT_3D ( 'NONE', #431, #22, #95 ) ; 
#860 = VECTOR ( 'NONE', #544, 1000.000000000000000 ) ; 
#861 = ORIENTED_EDGE ( 'NONE', *, *, #193, .T. ) ; 
#862 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#863 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#864 = ADVANCED_FACE ( 'NONE', ( #602, #58 ), #59, .F. ) ; 
#865 = CARTESIAN_POINT ( 'NONE',  ( 1.496986094846181597E-15, 2.000000000000000000, -7.899999999999995914 ) ) ; 
#866 = VERTEX_POINT ( 'NONE', #542 ) ; 
#867 = VECTOR ( 'NONE', #23, 1000.000000000000000 ) ; 
#868 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 0.000000000000000000, -13.49999999999999467 ) ) ; 
#869 = VERTEX_POINT ( 'NONE', #77 ) ; 
#870 = VECTOR ( 'NONE', #909, 1000.000000000000000 ) ; 
#871 = EDGE_CURVE ( 'NONE', #736, #161, #65, .T. ) ; 
#872 = DIRECTION ( 'NONE',  ( 1.284980352575412699E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#873 = AXIS2_PLACEMENT_3D ( 'NONE', #80, #562, #215 ) ; 
#874 = PERSON_AND_ORGANIZATION ( #777, #302 ) ; 
#875 = ORIENTED_EDGE ( 'NONE', *, *, #108, .T. ) ; 
#876 = DIRECTION ( 'NONE',  ( 1.000000000000000000, -0.000000000000000000, -0.000000000000000000 ) ) ; 
#877 = ORIENTED_EDGE ( 'NONE', *, *, #470, .T. ) ; 
#878 = APPROVAL_ROLE ( '' ) ; 
#879 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000000355, 9.000000000000000000, 13.50000000000000533 ) ) ; 
#880 = ORIENTED_EDGE ( 'NONE', *, *, #733, .T. ) ; 
#881 = ORIENTED_EDGE ( 'NONE', *, *, #900, .F. ) ; 
#882 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#883 = FACE_OUTER_BOUND ( 'NONE', #187, .T. ) ; 
#884 = LINE ( 'NONE', #69, #281 ) ; 
#885 = VERTEX_POINT ( 'NONE', #72 ) ; 
#886 = DIRECTION ( 'NONE',  ( 1.000000000000000000, -0.000000000000000000, 0.000000000000000000 ) ) ; 
#887 = EDGE_CURVE ( 'NONE', #598, #303, #385, .T. ) ; 
#888 = VECTOR ( 'NONE', #291, 1000.000000000000000 ) ; 
#889 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#890 = EDGE_CURVE ( 'NONE', #380, #536, #186, .T. ) ; 
#891 = ORIENTED_EDGE ( 'NONE', *, *, #92, .F. ) ; 
#892 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
#893 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, 4.280000000000006466 ) ) ; 
#894 = ORIENTED_EDGE ( 'NONE', *, *, #48, .T. ) ; 
#895 = DIRECTION ( 'NONE',  ( -1.000000000000000000, 0.000000000000000000, -0.000000000000000000 ) ) ; 
#896 = CARTESIAN_POINT ( 'NONE',  ( -8.400000000000002132, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#897 = CARTESIAN_POINT ( 'NONE',  ( 3.799999999999998046, 2.000000000000000000, 1.500000000000001332 ) ) ; 
#898 = CC_DESIGN_PERSON_AND_ORGANIZATION_ASSIGNMENT ( #724, #623, ( #636 ) ) ; 
#899 = ORIENTED_EDGE ( 'NONE', *, *, #682, .T. ) ; 
#900 = EDGE_CURVE ( 'NONE', #367, #309, #235, .T. ) ; 
#901 = EDGE_LOOP ( 'NONE', ( #346, #446, #4, #209 ) ) ; 
#902 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000000355, 9.000000000000000000, -13.49999999999999467 ) ) ; 
#903 = VERTEX_POINT ( 'NONE', #554 ) ; 
#904 = EDGE_CURVE ( 'NONE', #423, #474, #527, .T. ) ; 
#905 = DIRECTION ( 'NONE',  ( 1.000000000000000000, 0.000000000000000000, 0.000000000000000000 ) ) ; 
#906 = CARTESIAN_POINT ( 'NONE',  ( 8.400000000000003908, 0.000000000000000000, 13.50000000000000533 ) ) ; 
#907 = CYLINDRICAL_SURFACE ( 'NONE', #479, 1.599999999999998090 ) ; 
#908 = DIRECTION ( 'NONE',  ( 1.284980352575412699E-16, 0.000000000000000000, 1.000000000000000000 ) ) ; 
#909 = DIRECTION ( 'NONE',  ( 0.000000000000000000, 0.000000000000000000, -1.000000000000000000 ) ) ; 
ENDSEC; 
END-ISO-10303-21;  
        run: solid delanclip-gopro-adapter 
   facet normal -1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 1.836970e-20 1.778000e-02 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
         vertex 0.000000e+00 1.836970e-20 1.778000e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 4.600000e-03 2.000000e-03 1.500000e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 4.600000e-03 2.000000e-03 1.500000e-02 
         vertex 0.000000e+00 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
         vertex 3.575692e-03 1.836970e-20 1.910216e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.575692e-03 1.836970e-20 1.910216e-02 
         vertex 3.503508e-03 1.836970e-20 1.883277e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.503508e-03 1.836970e-20 1.883277e-02 
         vertex 3.385640e-03 1.836970e-20 1.858000e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.385640e-03 1.836970e-20 1.858000e-02 
         vertex 3.225671e-03 1.836970e-20 1.835154e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.225671e-03 1.836970e-20 1.835154e-02 
         vertex 3.028460e-03 1.836970e-20 1.815433e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.028460e-03 1.836970e-20 1.815433e-02 
         vertex 2.800000e-03 1.836970e-20 1.799436e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.800000e-03 1.836970e-20 1.799436e-02 
         vertex 2.547232e-03 1.836970e-20 1.787649e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.547232e-03 1.836970e-20 1.787649e-02 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 4.600000e-03 2.000000e-03 1.500000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 4.600000e-03 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.400000e-02 1.836970e-20 1.799436e-02 
         vertex 1.377154e-02 1.836970e-20 1.815433e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.425277e-02 1.836970e-20 1.787649e-02 
         vertex 1.400000e-02 1.836970e-20 1.799436e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.377154e-02 1.836970e-20 1.815433e-02 
         vertex 1.357433e-02 1.836970e-20 1.835154e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.357433e-02 1.836970e-20 1.835154e-02 
         vertex 1.341436e-02 1.836970e-20 1.858000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.341436e-02 1.836970e-20 1.858000e-02 
         vertex 1.329649e-02 1.836970e-20 1.883277e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 1.500000e-02 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
         vertex 1.452216e-02 1.836970e-20 1.780431e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.452216e-02 1.836970e-20 1.780431e-02 
         vertex 1.425277e-02 1.836970e-20 1.787649e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.329649e-02 1.836970e-20 1.883277e-02 
         vertex 1.322431e-02 1.836970e-20 1.910216e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.322431e-02 1.836970e-20 1.910216e-02 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.220000e-02 2.000000e-03 1.500000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.220000e-02 2.000000e-03 1.500000e-02 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.680000e-02 1.836970e-20 1.500000e-02 
         vertex 1.220000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 1.836970e-20 1.778000e-02 
         vertex 1.680000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 1.778000e-02 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
         vertex 1.680000e-02 1.836970e-20 1.500000e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 -0.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 0.000000e+00 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal -8.715556e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 2.000000e-03 9.000000e-03 1.778000e-02 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal -8.715556e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 2.000000e-03 9.000000e-03 1.778000e-02 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 2.000000e-03 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal 8.715556e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 1.480000e-02 9.000000e-03 1.778000e-02 
         vertex 1.452216e-02 1.836970e-20 1.780431e-02 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.480000e-02 9.000000e-03 1.778000e-02 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 1.778000e-02 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.480000e-02 1.836970e-20 1.778000e-02 
      endloop 
   endfacet 
   facet normal -2.588198e-01 0.000000e+00 9.659256e-01 
      outer loop 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
      endloop 
   endfacet 
   facet normal -2.588198e-01 0.000000e+00 9.659256e-01 
      outer loop 
         vertex 2.547232e-03 1.836970e-20 1.787649e-02 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 2.277837e-03 1.836970e-20 1.780431e-02 
      endloop 
   endfacet 
   facet normal 2.588198e-01 0.000000e+00 9.659256e-01 
      outer loop 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.425277e-02 1.836970e-20 1.787649e-02 
         vertex 1.452216e-02 1.836970e-20 1.780431e-02 
      endloop 
   endfacet 
   facet normal 8.715556e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 1.480000e-02 9.000000e-03 1.778000e-02 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.452216e-02 1.836970e-20 1.780431e-02 
      endloop 
   endfacet 
   facet normal -4.226172e-01 0.000000e+00 9.063083e-01 
      outer loop 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 2.547232e-03 1.836970e-20 1.787649e-02 
      endloop 
   endfacet 
   facet normal -4.226172e-01 0.000000e+00 9.063083e-01 
      outer loop 
         vertex 2.800000e-03 1.836970e-20 1.799436e-02 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 2.547232e-03 1.836970e-20 1.787649e-02 
      endloop 
   endfacet 
   facet normal 4.226172e-01 0.000000e+00 9.063083e-01 
      outer loop 
         vertex 1.425277e-02 9.000000e-03 1.787649e-02 
         vertex 1.400000e-02 1.836970e-20 1.799436e-02 
         vertex 1.425277e-02 1.836970e-20 1.787649e-02 
      endloop 
   endfacet 
   facet normal 2.588198e-01 0.000000e+00 9.659256e-01 
      outer loop 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.425277e-02 9.000000e-03 1.787649e-02 
         vertex 1.425277e-02 1.836970e-20 1.787649e-02 
      endloop 
   endfacet 
   facet normal -5.735769e-01 0.000000e+00 8.191517e-01 
      outer loop 
         vertex 3.028460e-03 9.000000e-03 1.815433e-02 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 2.800000e-03 1.836970e-20 1.799436e-02 
      endloop 
   endfacet 
   facet normal -5.735769e-01 0.000000e+00 8.191517e-01 
      outer loop 
         vertex 3.028460e-03 1.836970e-20 1.815433e-02 
         vertex 3.028460e-03 9.000000e-03 1.815433e-02 
         vertex 2.800000e-03 1.836970e-20 1.799436e-02 
      endloop 
   endfacet 
   facet normal 5.735769e-01 0.000000e+00 8.191517e-01 
      outer loop 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 1.377154e-02 1.836970e-20 1.815433e-02 
         vertex 1.400000e-02 1.836970e-20 1.799436e-02 
      endloop 
   endfacet 
   facet normal 4.226172e-01 0.000000e+00 9.063083e-01 
      outer loop 
         vertex 1.425277e-02 9.000000e-03 1.787649e-02 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 1.400000e-02 1.836970e-20 1.799436e-02 
      endloop 
   endfacet 
   facet normal -7.071068e-01 0.000000e+00 7.071068e-01 
      outer loop 
         vertex 3.225671e-03 9.000000e-03 1.835154e-02 
         vertex 3.028460e-03 9.000000e-03 1.815433e-02 
         vertex 3.028460e-03 1.836970e-20 1.815433e-02 
      endloop 
   endfacet 
   facet normal -7.071068e-01 0.000000e+00 7.071068e-01 
      outer loop 
         vertex 3.225671e-03 1.836970e-20 1.835154e-02 
         vertex 3.225671e-03 9.000000e-03 1.835154e-02 
         vertex 3.028460e-03 1.836970e-20 1.815433e-02 
      endloop 
   endfacet 
   facet normal 7.071068e-01 0.000000e+00 7.071068e-01 
      outer loop 
         vertex 1.377154e-02 9.000000e-03 1.815433e-02 
         vertex 1.357433e-02 1.836970e-20 1.835154e-02 
         vertex 1.377154e-02 1.836970e-20 1.815433e-02 
      endloop 
   endfacet 
   facet normal 5.735769e-01 0.000000e+00 8.191517e-01 
      outer loop 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 1.377154e-02 9.000000e-03 1.815433e-02 
         vertex 1.377154e-02 1.836970e-20 1.815433e-02 
      endloop 
   endfacet 
   facet normal -8.191523e-01 0.000000e+00 5.735761e-01 
      outer loop 
         vertex 3.385640e-03 9.000000e-03 1.858000e-02 
         vertex 3.225671e-03 9.000000e-03 1.835154e-02 
         vertex 3.225671e-03 1.836970e-20 1.835154e-02 
      endloop 
   endfacet 
   facet normal -8.191523e-01 0.000000e+00 5.735761e-01 
      outer loop 
         vertex 3.385640e-03 1.836970e-20 1.858000e-02 
         vertex 3.385640e-03 9.000000e-03 1.858000e-02 
         vertex 3.225671e-03 1.836970e-20 1.835154e-02 
      endloop 
   endfacet 
   facet normal 8.191523e-01 0.000000e+00 5.735761e-01 
      outer loop 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 1.341436e-02 1.836970e-20 1.858000e-02 
         vertex 1.357433e-02 1.836970e-20 1.835154e-02 
      endloop 
   endfacet 
   facet normal 7.071068e-01 0.000000e+00 7.071068e-01 
      outer loop 
         vertex 1.377154e-02 9.000000e-03 1.815433e-02 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 1.357433e-02 1.836970e-20 1.835154e-02 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 4.226185e-01 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.883277e-02 
         vertex 3.385640e-03 9.000000e-03 1.858000e-02 
         vertex 3.385640e-03 1.836970e-20 1.858000e-02 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 4.226185e-01 
      outer loop 
         vertex 3.503508e-03 1.836970e-20 1.883277e-02 
         vertex 3.503508e-03 9.000000e-03 1.883277e-02 
         vertex 3.385640e-03 1.836970e-20 1.858000e-02 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 4.226185e-01 
      outer loop 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
         vertex 1.329649e-02 1.836970e-20 1.883277e-02 
         vertex 1.341436e-02 1.836970e-20 1.858000e-02 
      endloop 
   endfacet 
   facet normal 8.191523e-01 0.000000e+00 5.735761e-01 
      outer loop 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
         vertex 1.341436e-02 1.836970e-20 1.858000e-02 
      endloop 
   endfacet 
   facet normal -9.659259e-01 0.000000e+00 2.588187e-01 
      outer loop 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 3.503508e-03 9.000000e-03 1.883277e-02 
         vertex 3.503508e-03 1.836970e-20 1.883277e-02 
      endloop 
   endfacet 
   facet normal -9.659259e-01 0.000000e+00 2.588187e-01 
      outer loop 
         vertex 3.575692e-03 1.836970e-20 1.910216e-02 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 3.503508e-03 1.836970e-20 1.883277e-02 
      endloop 
   endfacet 
   facet normal 9.659259e-01 0.000000e+00 2.588187e-01 
      outer loop 
         vertex 1.329649e-02 9.000000e-03 1.883277e-02 
         vertex 1.322431e-02 1.836970e-20 1.910216e-02 
         vertex 1.329649e-02 1.836970e-20 1.883277e-02 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 4.226185e-01 
      outer loop 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
         vertex 1.329649e-02 9.000000e-03 1.883277e-02 
         vertex 1.329649e-02 1.836970e-20 1.883277e-02 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 8.715541e-02 
      outer loop 
         vertex 3.599999e-03 9.000000e-03 1.938000e-02 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 3.575692e-03 1.836970e-20 1.910216e-02 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 8.715541e-02 
      outer loop 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
         vertex 3.599999e-03 9.000000e-03 1.938000e-02 
         vertex 3.575692e-03 1.836970e-20 1.910216e-02 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 8.715541e-02 
      outer loop 
         vertex 1.322431e-02 9.000000e-03 1.910216e-02 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
         vertex 1.322431e-02 1.836970e-20 1.910216e-02 
      endloop 
   endfacet 
   facet normal 9.659259e-01 0.000000e+00 2.588187e-01 
      outer loop 
         vertex 1.329649e-02 9.000000e-03 1.883277e-02 
         vertex 1.322431e-02 9.000000e-03 1.910216e-02 
         vertex 1.322431e-02 1.836970e-20 1.910216e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.575692e-03 1.836970e-20 1.965784e-02 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 3.575692e-03 9.000000e-03 1.965784e-02 
         vertex 3.599999e-03 9.000000e-03 1.938000e-02 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 3.575692e-03 1.836970e-20 1.965784e-02 
         vertex 3.575692e-03 9.000000e-03 1.965784e-02 
         vertex 3.599999e-03 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.322431e-02 1.836970e-20 1.965784e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 1.320000e-02 9.000000e-03 1.938000e-02 
         vertex 1.322431e-02 1.836970e-20 1.965784e-02 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 8.715541e-02 
      outer loop 
         vertex 1.322431e-02 9.000000e-03 1.910216e-02 
         vertex 1.320000e-02 9.000000e-03 1.938000e-02 
         vertex 1.320000e-02 1.836970e-20 1.938000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.503508e-03 1.836970e-20 1.992723e-02 
         vertex 3.575692e-03 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal -9.659260e-01 0.000000e+00 -2.588183e-01 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 3.575692e-03 9.000000e-03 1.965784e-02 
         vertex 3.575692e-03 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal -9.659260e-01 0.000000e+00 -2.588183e-01 
      outer loop 
         vertex 3.503508e-03 1.836970e-20 1.992723e-02 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 3.575692e-03 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.329649e-02 1.836970e-20 1.992723e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.322431e-02 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal 9.659260e-01 0.000000e+00 -2.588183e-01 
      outer loop 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
         vertex 1.329649e-02 1.836970e-20 1.992723e-02 
         vertex 1.322431e-02 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 1.320000e-02 9.000000e-03 1.938000e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
         vertex 1.322431e-02 1.836970e-20 1.965784e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.385640e-03 1.836970e-20 2.018000e-02 
         vertex 3.503508e-03 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 3.385640e-03 9.000000e-03 2.018000e-02 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 3.503508e-03 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 3.385640e-03 1.836970e-20 2.018000e-02 
         vertex 3.385640e-03 9.000000e-03 2.018000e-02 
         vertex 3.503508e-03 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.341436e-02 1.836970e-20 2.018000e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.329649e-02 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
         vertex 1.341436e-02 1.836970e-20 2.018000e-02 
         vertex 1.329649e-02 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal 9.659260e-01 0.000000e+00 -2.588183e-01 
      outer loop 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
         vertex 1.329649e-02 1.836970e-20 1.992723e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.225671e-03 1.836970e-20 2.040846e-02 
         vertex 3.385640e-03 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 3.225671e-03 9.000000e-03 2.040846e-02 
         vertex 3.385640e-03 9.000000e-03 2.018000e-02 
         vertex 3.385640e-03 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 3.225671e-03 1.836970e-20 2.040846e-02 
         vertex 3.225671e-03 9.000000e-03 2.040846e-02 
         vertex 3.385640e-03 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.357433e-02 1.836970e-20 2.040846e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.341436e-02 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 1.341436e-02 9.000000e-03 2.018000e-02 
         vertex 1.357433e-02 1.836970e-20 2.040846e-02 
         vertex 1.341436e-02 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
         vertex 1.341436e-02 9.000000e-03 2.018000e-02 
         vertex 1.341436e-02 1.836970e-20 2.018000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 3.028460e-03 1.836970e-20 2.060567e-02 
         vertex 3.225671e-03 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal -7.071076e-01 0.000000e+00 -7.071059e-01 
      outer loop 
         vertex 3.028460e-03 9.000000e-03 2.060567e-02 
         vertex 3.225671e-03 9.000000e-03 2.040846e-02 
         vertex 3.225671e-03 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal -7.071076e-01 0.000000e+00 -7.071059e-01 
      outer loop 
         vertex 3.028460e-03 1.836970e-20 2.060567e-02 
         vertex 3.028460e-03 9.000000e-03 2.060567e-02 
         vertex 3.225671e-03 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.377154e-02 1.836970e-20 2.060567e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.357433e-02 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal 7.071076e-01 0.000000e+00 -7.071059e-01 
      outer loop 
         vertex 1.357433e-02 9.000000e-03 2.040846e-02 
         vertex 1.377154e-02 1.836970e-20 2.060567e-02 
         vertex 1.357433e-02 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 1.341436e-02 9.000000e-03 2.018000e-02 
         vertex 1.357433e-02 9.000000e-03 2.040846e-02 
         vertex 1.357433e-02 1.836970e-20 2.040846e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 2.800000e-03 1.836970e-20 2.076564e-02 
         vertex 3.028460e-03 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal -5.735757e-01 0.000000e+00 -8.191525e-01 
      outer loop 
         vertex 2.800000e-03 9.000000e-03 2.076564e-02 
         vertex 3.028460e-03 9.000000e-03 2.060567e-02 
         vertex 3.028460e-03 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal -5.735757e-01 0.000000e+00 -8.191525e-01 
      outer loop 
         vertex 2.800000e-03 1.836970e-20 2.076564e-02 
         vertex 2.800000e-03 9.000000e-03 2.076564e-02 
         vertex 3.028460e-03 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.400000e-02 1.836970e-20 2.076564e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.377154e-02 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal 5.735757e-01 0.000000e+00 -8.191525e-01 
      outer loop 
         vertex 1.377154e-02 9.000000e-03 2.060567e-02 
         vertex 1.400000e-02 1.836970e-20 2.076564e-02 
         vertex 1.377154e-02 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal 7.071076e-01 0.000000e+00 -7.071059e-01 
      outer loop 
         vertex 1.357433e-02 9.000000e-03 2.040846e-02 
         vertex 1.377154e-02 9.000000e-03 2.060567e-02 
         vertex 1.377154e-02 1.836970e-20 2.060567e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 2.547232e-03 1.836970e-20 2.088351e-02 
         vertex 2.800000e-03 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal -4.226185e-01 0.000000e+00 -9.063077e-01 
      outer loop 
         vertex 2.547232e-03 9.000000e-03 2.088351e-02 
         vertex 2.800000e-03 9.000000e-03 2.076564e-02 
         vertex 2.800000e-03 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal -4.226185e-01 0.000000e+00 -9.063077e-01 
      outer loop 
         vertex 2.547232e-03 1.836970e-20 2.088351e-02 
         vertex 2.547232e-03 9.000000e-03 2.088351e-02 
         vertex 2.800000e-03 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.425277e-02 1.836970e-20 2.088351e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.400000e-02 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal 4.226185e-01 0.000000e+00 -9.063077e-01 
      outer loop 
         vertex 1.400000e-02 9.000000e-03 2.076564e-02 
         vertex 1.425277e-02 1.836970e-20 2.088351e-02 
         vertex 1.400000e-02 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal 5.735757e-01 0.000000e+00 -8.191525e-01 
      outer loop 
         vertex 1.377154e-02 9.000000e-03 2.060567e-02 
         vertex 1.400000e-02 9.000000e-03 2.076564e-02 
         vertex 1.400000e-02 1.836970e-20 2.076564e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 2.277837e-03 1.836970e-20 2.095569e-02 
         vertex 2.547232e-03 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal -2.588198e-01 0.000000e+00 -9.659256e-01 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 2.547232e-03 9.000000e-03 2.088351e-02 
         vertex 2.547232e-03 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal -2.588198e-01 0.000000e+00 -9.659256e-01 
      outer loop 
         vertex 2.277837e-03 1.836970e-20 2.095569e-02 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 2.547232e-03 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.452216e-02 1.836970e-20 2.095569e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.425277e-02 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal 2.588198e-01 0.000000e+00 -9.659256e-01 
      outer loop 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
         vertex 1.452216e-02 1.836970e-20 2.095569e-02 
         vertex 1.425277e-02 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal 4.226185e-01 0.000000e+00 -9.063077e-01 
      outer loop 
         vertex 1.400000e-02 9.000000e-03 2.076564e-02 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
         vertex 1.425277e-02 1.836970e-20 2.088351e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 2.000000e-03 1.836970e-20 2.098000e-02 
         vertex 2.277837e-03 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal -8.715556e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 2.277837e-03 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal -8.715556e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 2.000000e-03 1.836970e-20 2.098000e-02 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 2.277837e-03 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.452216e-02 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal 8.715556e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 1.452216e-02 9.000000e-03 2.095569e-02 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
         vertex 1.452216e-02 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal 2.588198e-01 0.000000e+00 -9.659256e-01 
      outer loop 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
         vertex 1.452216e-02 9.000000e-03 2.095569e-02 
         vertex 1.452216e-02 1.836970e-20 2.095569e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 1.836970e-20 2.700000e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 0.000000e+00 9.000000e-03 2.098000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 0.000000e+00 1.836970e-20 2.700000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.000000e-03 1.836970e-20 2.098000e-02 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.098000e-02 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 2.000000e-03 1.836970e-20 2.098000e-02 
         vertex 0.000000e+00 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 2.098000e-02 
         vertex 1.680000e-02 1.836970e-20 2.700000e-02 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 2.700000e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 8.715556e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 1.452216e-02 9.000000e-03 2.095569e-02 
         vertex 1.480000e-02 9.000000e-03 2.098000e-02 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.480000e-02 9.000000e-03 2.098000e-02 
         vertex 1.680000e-02 1.836970e-20 2.098000e-02 
         vertex 1.480000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.098000e-02 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.680000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.680000e-02 1.836970e-20 2.700000e-02 
         vertex 1.680000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.480000e-02 9.000000e-03 2.098000e-02 
         vertex 1.680000e-02 9.000000e-03 2.098000e-02 
         vertex 1.680000e-02 1.836970e-20 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 0.000000e+00 1.836970e-20 2.700000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 4.600000e-03 1.836970e-20 2.700000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.680000e-02 1.836970e-20 2.700000e-02 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.220000e-02 1.836970e-20 2.700000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.680000e-02 1.836970e-20 2.700000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.014359e-03 2.000000e-03 3.200000e-03 
         vertex 6.896492e-03 2.000000e-03 3.452769e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
         vertex 8.122163e-03 2.000000e-03 2.424308e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.122163e-03 2.000000e-03 2.424308e-03 
         vertex 7.852767e-03 2.000000e-03 2.496492e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.852767e-03 2.000000e-03 2.496492e-03 
         vertex 7.600000e-03 2.000000e-03 2.614359e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.600000e-03 2.000000e-03 2.614359e-03 
         vertex 7.371539e-03 2.000000e-03 2.774329e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.371539e-03 2.000000e-03 2.774329e-03 
         vertex 7.174329e-03 2.000000e-03 2.971540e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.174329e-03 2.000000e-03 2.971540e-03 
         vertex 7.014359e-03 2.000000e-03 3.200000e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.896492e-03 2.000000e-03 3.452769e-03 
         vertex 6.824307e-03 2.000000e-03 3.722163e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.824307e-03 2.000000e-03 3.722163e-03 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 5.950000e-03 4.500000e-03 1.500000e-02 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 5.950000e-03 4.500000e-03 1.500000e-02 
         vertex 5.950000e-03 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 8.947232e-03 2.000000e-03 2.496492e-03 
         vertex 8.677837e-03 2.000000e-03 2.424308e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 8.677837e-03 2.000000e-03 2.424308e-03 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.903508e-03 2.000000e-03 3.452769e-03 
         vertex 9.785640e-03 2.000000e-03 3.200000e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.785640e-03 2.000000e-03 3.200000e-03 
         vertex 9.625671e-03 2.000000e-03 2.971540e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.625671e-03 2.000000e-03 2.971540e-03 
         vertex 9.428460e-03 2.000000e-03 2.774329e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.428460e-03 2.000000e-03 2.774329e-03 
         vertex 9.200000e-03 2.000000e-03 2.614359e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.200000e-03 2.000000e-03 2.614359e-03 
         vertex 8.947232e-03 2.000000e-03 2.496492e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
         vertex 9.975692e-03 2.000000e-03 3.722163e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 9.975692e-03 2.000000e-03 3.722163e-03 
         vertex 9.903508e-03 2.000000e-03 3.452769e-03 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
         vertex 1.085000e-02 4.500000e-03 1.500000e-02 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 4.500000e-03 1.500000e-02 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 1.085000e-02 2.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -8.715547e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 8.677837e-03 9.000000e-03 2.424308e-03 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
      endloop 
   endfacet 
   facet normal -8.715547e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 8.677837e-03 2.000000e-03 2.424308e-03 
         vertex 8.677837e-03 9.000000e-03 2.424308e-03 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
      endloop 
   endfacet 
   facet normal 8.715547e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 8.122163e-03 2.000000e-03 2.424308e-03 
         vertex 8.400000e-03 2.000000e-03 2.400001e-03 
      endloop 
   endfacet 
   facet normal 2.588184e-01 0.000000e+00 9.659260e-01 
      outer loop 
         vertex 8.122163e-03 9.000000e-03 2.424308e-03 
         vertex 7.852767e-03 2.000000e-03 2.496492e-03 
         vertex 8.122163e-03 2.000000e-03 2.424308e-03 
      endloop 
   endfacet 
   facet normal 8.715547e-02 0.000000e+00 9.961947e-01 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 8.122163e-03 9.000000e-03 2.424308e-03 
         vertex 8.122163e-03 2.000000e-03 2.424308e-03 
      endloop 
   endfacet 
   facet normal -2.588184e-01 0.000000e+00 9.659260e-01 
      outer loop 
         vertex 8.947232e-03 9.000000e-03 2.496492e-03 
         vertex 8.677837e-03 9.000000e-03 2.424308e-03 
         vertex 8.677837e-03 2.000000e-03 2.424308e-03 
      endloop 
   endfacet 
   facet normal -2.588184e-01 0.000000e+00 9.659260e-01 
      outer loop 
         vertex 8.947232e-03 2.000000e-03 2.496492e-03 
         vertex 8.947232e-03 9.000000e-03 2.496492e-03 
         vertex 8.677837e-03 2.000000e-03 2.424308e-03 
      endloop 
   endfacet 
   facet normal 4.226174e-01 0.000000e+00 9.063082e-01 
      outer loop 
         vertex 7.852767e-03 9.000000e-03 2.496492e-03 
         vertex 7.600000e-03 2.000000e-03 2.614359e-03 
         vertex 7.852767e-03 2.000000e-03 2.496492e-03 
      endloop 
   endfacet 
   facet normal 2.588184e-01 0.000000e+00 9.659260e-01 
      outer loop 
         vertex 8.122163e-03 9.000000e-03 2.424308e-03 
         vertex 7.852767e-03 9.000000e-03 2.496492e-03 
         vertex 7.852767e-03 2.000000e-03 2.496492e-03 
      endloop 
   endfacet 
   facet normal -4.226174e-01 0.000000e+00 9.063082e-01 
      outer loop 
         vertex 9.200000e-03 9.000000e-03 2.614359e-03 
         vertex 8.947232e-03 9.000000e-03 2.496492e-03 
         vertex 8.947232e-03 2.000000e-03 2.496492e-03 
      endloop 
   endfacet 
   facet normal -4.226174e-01 0.000000e+00 9.063082e-01 
      outer loop 
         vertex 9.200000e-03 2.000000e-03 2.614359e-03 
         vertex 9.200000e-03 9.000000e-03 2.614359e-03 
         vertex 8.947232e-03 2.000000e-03 2.496492e-03 
      endloop 
   endfacet 
   facet normal 5.735764e-01 0.000000e+00 8.191521e-01 
      outer loop 
         vertex 7.600000e-03 9.000000e-03 2.614359e-03 
         vertex 7.371539e-03 2.000000e-03 2.774329e-03 
         vertex 7.600000e-03 2.000000e-03 2.614359e-03 
      endloop 
   endfacet 
   facet normal 4.226174e-01 0.000000e+00 9.063082e-01 
      outer loop 
         vertex 7.852767e-03 9.000000e-03 2.496492e-03 
         vertex 7.600000e-03 9.000000e-03 2.614359e-03 
         vertex 7.600000e-03 2.000000e-03 2.614359e-03 
      endloop 
   endfacet 
   facet normal -5.735764e-01 0.000000e+00 8.191521e-01 
      outer loop 
         vertex 9.428460e-03 9.000000e-03 2.774329e-03 
         vertex 9.200000e-03 9.000000e-03 2.614359e-03 
         vertex 9.200000e-03 2.000000e-03 2.614359e-03 
      endloop 
   endfacet 
   facet normal -5.735764e-01 0.000000e+00 8.191521e-01 
      outer loop 
         vertex 9.428460e-03 2.000000e-03 2.774329e-03 
         vertex 9.428460e-03 9.000000e-03 2.774329e-03 
         vertex 9.200000e-03 2.000000e-03 2.614359e-03 
      endloop 
   endfacet 
   facet normal 7.071074e-01 0.000000e+00 7.071062e-01 
      outer loop 
         vertex 7.371539e-03 9.000000e-03 2.774329e-03 
         vertex 7.174329e-03 2.000000e-03 2.971540e-03 
         vertex 7.371539e-03 2.000000e-03 2.774329e-03 
      endloop 
   endfacet 
   facet normal 5.735764e-01 0.000000e+00 8.191521e-01 
      outer loop 
         vertex 7.600000e-03 9.000000e-03 2.614359e-03 
         vertex 7.371539e-03 9.000000e-03 2.774329e-03 
         vertex 7.371539e-03 2.000000e-03 2.774329e-03 
      endloop 
   endfacet 
   facet normal -7.071074e-01 0.000000e+00 7.071062e-01 
      outer loop 
         vertex 9.625671e-03 9.000000e-03 2.971540e-03 
         vertex 9.428460e-03 9.000000e-03 2.774329e-03 
         vertex 9.428460e-03 2.000000e-03 2.774329e-03 
      endloop 
   endfacet 
   facet normal -7.071074e-01 0.000000e+00 7.071062e-01 
      outer loop 
         vertex 9.625671e-03 2.000000e-03 2.971540e-03 
         vertex 9.625671e-03 9.000000e-03 2.971540e-03 
         vertex 9.428460e-03 2.000000e-03 2.774329e-03 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 5.735769e-01 
      outer loop 
         vertex 7.174329e-03 9.000000e-03 2.971540e-03 
         vertex 7.014359e-03 2.000000e-03 3.200000e-03 
         vertex 7.174329e-03 2.000000e-03 2.971540e-03 
      endloop 
   endfacet 
   facet normal 7.071074e-01 0.000000e+00 7.071062e-01 
      outer loop 
         vertex 7.371539e-03 9.000000e-03 2.774329e-03 
         vertex 7.174329e-03 9.000000e-03 2.971540e-03 
         vertex 7.174329e-03 2.000000e-03 2.971540e-03 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 5.735769e-01 
      outer loop 
         vertex 9.785640e-03 9.000000e-03 3.200000e-03 
         vertex 9.625671e-03 9.000000e-03 2.971540e-03 
         vertex 9.625671e-03 2.000000e-03 2.971540e-03 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 5.735769e-01 
      outer loop 
         vertex 9.785640e-03 2.000000e-03 3.200000e-03 
         vertex 9.785640e-03 9.000000e-03 3.200000e-03 
         vertex 9.625671e-03 2.000000e-03 2.971540e-03 
      endloop 
   endfacet 
   facet normal 9.063083e-01 0.000000e+00 4.226172e-01 
      outer loop 
         vertex 7.014359e-03 9.000000e-03 3.200000e-03 
         vertex 6.896492e-03 2.000000e-03 3.452769e-03 
         vertex 7.014359e-03 2.000000e-03 3.200000e-03 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 5.735769e-01 
      outer loop 
         vertex 7.174329e-03 9.000000e-03 2.971540e-03 
         vertex 7.014359e-03 9.000000e-03 3.200000e-03 
         vertex 7.014359e-03 2.000000e-03 3.200000e-03 
      endloop 
   endfacet 
   facet normal -9.063083e-01 0.000000e+00 4.226172e-01 
      outer loop 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
         vertex 9.785640e-03 9.000000e-03 3.200000e-03 
         vertex 9.785640e-03 2.000000e-03 3.200000e-03 
      endloop 
   endfacet 
   facet normal -9.063083e-01 0.000000e+00 4.226172e-01 
      outer loop 
         vertex 9.903508e-03 2.000000e-03 3.452769e-03 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
         vertex 9.785640e-03 2.000000e-03 3.200000e-03 
      endloop 
   endfacet 
   facet normal 9.659256e-01 0.000000e+00 2.588199e-01 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 3.452769e-03 
         vertex 6.824307e-03 2.000000e-03 3.722163e-03 
         vertex 6.896492e-03 2.000000e-03 3.452769e-03 
      endloop 
   endfacet 
   facet normal 9.063083e-01 0.000000e+00 4.226172e-01 
      outer loop 
         vertex 7.014359e-03 9.000000e-03 3.200000e-03 
         vertex 6.896492e-03 9.000000e-03 3.452769e-03 
         vertex 6.896492e-03 2.000000e-03 3.452769e-03 
      endloop 
   endfacet 
   facet normal -9.659256e-01 0.000000e+00 2.588199e-01 
      outer loop 
         vertex 9.975692e-03 9.000000e-03 3.722163e-03 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
         vertex 9.903508e-03 2.000000e-03 3.452769e-03 
      endloop 
   endfacet 
   facet normal -9.659256e-01 0.000000e+00 2.588199e-01 
      outer loop 
         vertex 9.975692e-03 2.000000e-03 3.722163e-03 
         vertex 9.975692e-03 9.000000e-03 3.722163e-03 
         vertex 9.903508e-03 2.000000e-03 3.452769e-03 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 8.715527e-02 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 3.722163e-03 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
         vertex 6.824307e-03 2.000000e-03 3.722163e-03 
      endloop 
   endfacet 
   facet normal 9.659256e-01 0.000000e+00 2.588199e-01 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 3.452769e-03 
         vertex 6.824307e-03 9.000000e-03 3.722163e-03 
         vertex 6.824307e-03 2.000000e-03 3.722163e-03 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 8.715527e-02 
      outer loop 
         vertex 1.000000e-02 9.000000e-03 4.000001e-03 
         vertex 9.975692e-03 9.000000e-03 3.722163e-03 
         vertex 9.975692e-03 2.000000e-03 3.722163e-03 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 8.715527e-02 
      outer loop 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
         vertex 1.000000e-02 9.000000e-03 4.000001e-03 
         vertex 9.975692e-03 2.000000e-03 3.722163e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.824307e-03 2.000000e-03 4.277837e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 6.800000e-03 9.000000e-03 4.000001e-03 
         vertex 6.824307e-03 2.000000e-03 4.277837e-03 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 8.715527e-02 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 3.722163e-03 
         vertex 6.800000e-03 9.000000e-03 4.000001e-03 
         vertex 6.800000e-03 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.975692e-03 2.000000e-03 4.277837e-03 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 9.975692e-03 9.000000e-03 4.277837e-03 
         vertex 1.000000e-02 9.000000e-03 4.000001e-03 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal -9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 9.975692e-03 2.000000e-03 4.277837e-03 
         vertex 9.975692e-03 9.000000e-03 4.277837e-03 
         vertex 1.000000e-02 2.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.896492e-03 2.000000e-03 4.547233e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 6.824307e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal 9.659258e-01 0.000000e+00 -2.588190e-01 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 4.277837e-03 
         vertex 6.896492e-03 2.000000e-03 4.547233e-03 
         vertex 6.824307e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal 9.961947e-01 0.000000e+00 -8.715556e-02 
      outer loop 
         vertex 6.800000e-03 9.000000e-03 4.000001e-03 
         vertex 6.824307e-03 9.000000e-03 4.277837e-03 
         vertex 6.824307e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.903508e-03 2.000000e-03 4.547233e-03 
         vertex 9.975692e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal -9.659258e-01 0.000000e+00 -2.588190e-01 
      outer loop 
         vertex 9.903508e-03 9.000000e-03 4.547233e-03 
         vertex 9.975692e-03 9.000000e-03 4.277837e-03 
         vertex 9.975692e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal -9.659258e-01 0.000000e+00 -2.588190e-01 
      outer loop 
         vertex 9.903508e-03 2.000000e-03 4.547233e-03 
         vertex 9.903508e-03 9.000000e-03 4.547233e-03 
         vertex 9.975692e-03 2.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.014359e-03 2.000000e-03 4.800000e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 6.896492e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
         vertex 7.014359e-03 2.000000e-03 4.800000e-03 
         vertex 6.896492e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal 9.659258e-01 0.000000e+00 -2.588190e-01 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 4.277837e-03 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
         vertex 6.896492e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.785640e-03 2.000000e-03 4.800000e-03 
         vertex 9.903508e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 9.785640e-03 9.000000e-03 4.800000e-03 
         vertex 9.903508e-03 9.000000e-03 4.547233e-03 
         vertex 9.903508e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal -9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 9.785640e-03 2.000000e-03 4.800000e-03 
         vertex 9.785640e-03 9.000000e-03 4.800000e-03 
         vertex 9.903508e-03 2.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.174329e-03 2.000000e-03 5.028460e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 7.014359e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 7.014359e-03 9.000000e-03 4.800000e-03 
         vertex 7.174329e-03 2.000000e-03 5.028460e-03 
         vertex 7.014359e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal 9.063077e-01 0.000000e+00 -4.226185e-01 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
         vertex 7.014359e-03 9.000000e-03 4.800000e-03 
         vertex 7.014359e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.625671e-03 2.000000e-03 5.028460e-03 
         vertex 9.785640e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 9.625671e-03 9.000000e-03 5.028460e-03 
         vertex 9.785640e-03 9.000000e-03 4.800000e-03 
         vertex 9.785640e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal -8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 9.625671e-03 2.000000e-03 5.028460e-03 
         vertex 9.625671e-03 9.000000e-03 5.028460e-03 
         vertex 9.785640e-03 2.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.371539e-03 2.000000e-03 5.225671e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 7.174329e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal 7.071074e-01 0.000000e+00 -7.071062e-01 
      outer loop 
         vertex 7.174329e-03 9.000000e-03 5.028460e-03 
         vertex 7.371539e-03 2.000000e-03 5.225671e-03 
         vertex 7.174329e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal 8.191517e-01 0.000000e+00 -5.735769e-01 
      outer loop 
         vertex 7.014359e-03 9.000000e-03 4.800000e-03 
         vertex 7.174329e-03 9.000000e-03 5.028460e-03 
         vertex 7.174329e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.428460e-03 2.000000e-03 5.225671e-03 
         vertex 9.625671e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal -7.071074e-01 0.000000e+00 -7.071062e-01 
      outer loop 
         vertex 9.428460e-03 9.000000e-03 5.225671e-03 
         vertex 9.625671e-03 9.000000e-03 5.028460e-03 
         vertex 9.625671e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal -7.071074e-01 0.000000e+00 -7.071062e-01 
      outer loop 
         vertex 9.428460e-03 2.000000e-03 5.225671e-03 
         vertex 9.428460e-03 9.000000e-03 5.225671e-03 
         vertex 9.625671e-03 2.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.600000e-03 2.000000e-03 5.385641e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 7.371539e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal 5.735764e-01 0.000000e+00 -8.191521e-01 
      outer loop 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
         vertex 7.600000e-03 2.000000e-03 5.385641e-03 
         vertex 7.371539e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal 7.071074e-01 0.000000e+00 -7.071062e-01 
      outer loop 
         vertex 7.174329e-03 9.000000e-03 5.028460e-03 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
         vertex 7.371539e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 9.200000e-03 2.000000e-03 5.385641e-03 
         vertex 9.428460e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -5.735764e-01 0.000000e+00 -8.191521e-01 
      outer loop 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
         vertex 9.428460e-03 9.000000e-03 5.225671e-03 
         vertex 9.428460e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -5.735764e-01 0.000000e+00 -8.191521e-01 
      outer loop 
         vertex 9.200000e-03 2.000000e-03 5.385641e-03 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
         vertex 9.428460e-03 2.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.852767e-03 2.000000e-03 5.503508e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 7.600000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 4.226174e-01 0.000000e+00 -9.063082e-01 
      outer loop 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
         vertex 7.852767e-03 2.000000e-03 5.503508e-03 
         vertex 7.600000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 5.735764e-01 0.000000e+00 -8.191521e-01 
      outer loop 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
         vertex 7.600000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 8.947232e-03 2.000000e-03 5.503508e-03 
         vertex 9.200000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal -4.226174e-01 0.000000e+00 -9.063082e-01 
      outer loop 
         vertex 8.947232e-03 9.000000e-03 5.503508e-03 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
         vertex 9.200000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal -4.226174e-01 0.000000e+00 -9.063082e-01 
      outer loop 
         vertex 8.947232e-03 2.000000e-03 5.503508e-03 
         vertex 8.947232e-03 9.000000e-03 5.503508e-03 
         vertex 9.200000e-03 2.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 7.852767e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal 2.588215e-01 0.000000e+00 -9.659252e-01 
      outer loop 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
         vertex 7.852767e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal 4.226174e-01 0.000000e+00 -9.063082e-01 
      outer loop 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
         vertex 7.852767e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 8.677837e-03 2.000000e-03 5.575693e-03 
         vertex 8.947232e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal -2.588215e-01 0.000000e+00 -9.659252e-01 
      outer loop 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
         vertex 8.947232e-03 9.000000e-03 5.503508e-03 
         vertex 8.947232e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal -2.588215e-01 0.000000e+00 -9.659252e-01 
      outer loop 
         vertex 8.677837e-03 2.000000e-03 5.575693e-03 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
         vertex 8.947232e-03 2.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.400000e-03 2.000000e-03 5.600001e-03 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 8.715547e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
         vertex 8.400000e-03 2.000000e-03 5.600001e-03 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 2.588215e-01 0.000000e+00 -9.659252e-01 
      outer loop 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
         vertex 8.122163e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 8.400000e-03 2.000000e-03 5.600001e-03 
         vertex 8.677837e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -8.715547e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
         vertex 8.677837e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -8.715547e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 8.400000e-03 2.000000e-03 5.600001e-03 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
         vertex 8.677837e-03 2.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 8.715547e-02 0.000000e+00 -9.961947e-01 
      outer loop 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
         vertex 8.400000e-03 2.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 4.600000e-03 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
         vertex 4.600000e-03 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 5.950000e-03 4.500000e-03 1.500000e-02 
         vertex 5.950000e-03 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.220000e-02 2.000000e-03 1.500000e-02 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.085000e-02 4.500000e-03 1.500000e-02 
         vertex 1.220000e-02 2.000000e-03 1.500000e-02 
         vertex 1.085000e-02 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.085000e-02 4.500000e-03 1.500000e-02 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.220000e-02 2.000000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.220000e-02 2.000000e-03 2.700000e-02 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 1.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 4.600000e-03 2.000000e-03 2.700000e-02 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 0.000000e+00 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
         vertex 0.000000e+00 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
         vertex 0.000000e+00 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
         vertex 5.950000e-03 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 5.950000e-03 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 4.500000e-03 1.500000e-02 
         vertex 0.000000e+00 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 5.950000e-03 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -0.000000e+00 -1.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 1.680000e-02 4.500000e-03 0.000000e+00 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.085000e-02 4.500000e-03 1.500000e-02 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 -1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 4.500000e-03 0.000000e+00 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.085000e-02 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
         vertex 1.680000e-02 4.500000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 1.000000e+00 0.000000e+00 -0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 4.500000e-03 1.500000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.014359e-03 9.000000e-03 3.200000e-03 
         vertex 7.174329e-03 9.000000e-03 2.971540e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.174329e-03 9.000000e-03 2.971540e-03 
         vertex 7.371539e-03 9.000000e-03 2.774329e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.371539e-03 9.000000e-03 2.774329e-03 
         vertex 7.600000e-03 9.000000e-03 2.614359e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.600000e-03 9.000000e-03 2.614359e-03 
         vertex 7.852767e-03 9.000000e-03 2.496492e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.800000e-03 9.000000e-03 4.000001e-03 
         vertex 6.824307e-03 9.000000e-03 3.722163e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 3.722163e-03 
         vertex 6.896492e-03 9.000000e-03 3.452769e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 3.452769e-03 
         vertex 7.014359e-03 9.000000e-03 3.200000e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
         vertex 6.824307e-03 9.000000e-03 4.277837e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 6.824307e-03 9.000000e-03 4.277837e-03 
         vertex 6.800000e-03 9.000000e-03 4.000001e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 7.852767e-03 9.000000e-03 2.496492e-03 
         vertex 8.122163e-03 9.000000e-03 2.424308e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.122163e-03 9.000000e-03 2.424308e-03 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
         vertex 0.000000e+00 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.947232e-03 9.000000e-03 2.496492e-03 
         vertex 9.200000e-03 9.000000e-03 2.614359e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.785640e-03 9.000000e-03 3.200000e-03 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.200000e-03 9.000000e-03 2.614359e-03 
         vertex 9.428460e-03 9.000000e-03 2.774329e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.428460e-03 9.000000e-03 2.774329e-03 
         vertex 9.625671e-03 9.000000e-03 2.971540e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.625671e-03 9.000000e-03 2.971540e-03 
         vertex 9.785640e-03 9.000000e-03 3.200000e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.480000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.480000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 9.000000e-03 1.778000e-02 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 2.400001e-03 
         vertex 8.677837e-03 9.000000e-03 2.424308e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.677837e-03 9.000000e-03 2.424308e-03 
         vertex 8.947232e-03 9.000000e-03 2.496492e-03 
         vertex 1.680000e-02 9.000000e-03 0.000000e+00 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.975692e-03 9.000000e-03 3.722163e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.903508e-03 9.000000e-03 3.452769e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.000000e-02 9.000000e-03 4.000001e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.975692e-03 9.000000e-03 3.722163e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.975692e-03 9.000000e-03 4.277837e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 1.000000e-02 9.000000e-03 4.000001e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.903508e-03 9.000000e-03 4.547233e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.975692e-03 9.000000e-03 4.277837e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 7.014359e-03 9.000000e-03 4.800000e-03 
         vertex 6.896492e-03 9.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.785640e-03 9.000000e-03 4.800000e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.903508e-03 9.000000e-03 4.547233e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 7.174329e-03 9.000000e-03 5.028460e-03 
         vertex 7.014359e-03 9.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.625671e-03 9.000000e-03 5.028460e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.785640e-03 9.000000e-03 4.800000e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
         vertex 7.174329e-03 9.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.428460e-03 9.000000e-03 5.225671e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.625671e-03 9.000000e-03 5.028460e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 1.778000e-02 
         vertex 2.000000e-03 9.000000e-03 1.778000e-02 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.000000e-03 9.000000e-03 1.778000e-02 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
         vertex 7.371539e-03 9.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.428460e-03 9.000000e-03 5.225671e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 1.780431e-02 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
         vertex 7.600000e-03 9.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.947232e-03 9.000000e-03 5.503508e-03 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 1.425277e-02 9.000000e-03 1.787649e-02 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.425277e-02 9.000000e-03 1.787649e-02 
         vertex 1.452216e-02 9.000000e-03 1.780431e-02 
         vertex 9.200000e-03 9.000000e-03 5.385641e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.547232e-03 9.000000e-03 1.787649e-02 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
         vertex 7.852767e-03 9.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 8.947232e-03 9.000000e-03 5.503508e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.800000e-03 9.000000e-03 1.799436e-02 
         vertex 3.028460e-03 9.000000e-03 1.815433e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.028460e-03 9.000000e-03 1.815433e-02 
         vertex 3.225671e-03 9.000000e-03 1.835154e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.225671e-03 9.000000e-03 1.835154e-02 
         vertex 3.385640e-03 9.000000e-03 1.858000e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.385640e-03 9.000000e-03 1.858000e-02 
         vertex 3.503508e-03 9.000000e-03 1.883277e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.883277e-02 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 8.122163e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 1.377154e-02 9.000000e-03 1.815433e-02 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.377154e-02 9.000000e-03 1.815433e-02 
         vertex 1.400000e-02 9.000000e-03 1.799436e-02 
         vertex 8.677837e-03 9.000000e-03 5.575693e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.575692e-03 9.000000e-03 1.910216e-02 
         vertex 3.599999e-03 9.000000e-03 1.938000e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.599999e-03 9.000000e-03 1.938000e-02 
         vertex 3.575692e-03 9.000000e-03 1.965784e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.575692e-03 9.000000e-03 1.965784e-02 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
         vertex 1.357433e-02 9.000000e-03 1.835154e-02 
         vertex 8.400000e-03 9.000000e-03 5.600001e-03 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 1.329649e-02 9.000000e-03 1.883277e-02 
         vertex 1.341436e-02 9.000000e-03 1.858000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 1.322431e-02 9.000000e-03 1.910216e-02 
         vertex 1.329649e-02 9.000000e-03 1.883277e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 1.320000e-02 9.000000e-03 1.938000e-02 
         vertex 1.322431e-02 9.000000e-03 1.910216e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
         vertex 1.320000e-02 9.000000e-03 1.938000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.385640e-03 9.000000e-03 2.018000e-02 
         vertex 3.225671e-03 9.000000e-03 2.040846e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.225671e-03 9.000000e-03 2.040846e-02 
         vertex 3.028460e-03 9.000000e-03 2.060567e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.503508e-03 9.000000e-03 1.992723e-02 
         vertex 3.385640e-03 9.000000e-03 2.018000e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 3.028460e-03 9.000000e-03 2.060567e-02 
         vertex 2.800000e-03 9.000000e-03 2.076564e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.800000e-03 9.000000e-03 2.076564e-02 
         vertex 2.547232e-03 9.000000e-03 2.088351e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.547232e-03 9.000000e-03 2.088351e-02 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 1.322431e-02 9.000000e-03 1.965784e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.341436e-02 9.000000e-03 2.018000e-02 
         vertex 1.329649e-02 9.000000e-03 1.992723e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.357433e-02 9.000000e-03 2.040846e-02 
         vertex 1.341436e-02 9.000000e-03 2.018000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.377154e-02 9.000000e-03 2.060567e-02 
         vertex 1.357433e-02 9.000000e-03 2.040846e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.400000e-02 9.000000e-03 2.076564e-02 
         vertex 1.377154e-02 9.000000e-03 2.060567e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
         vertex 1.400000e-02 9.000000e-03 2.076564e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.452216e-02 9.000000e-03 2.095569e-02 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.425277e-02 9.000000e-03 2.088351e-02 
      endloop 
   endfacet 
   facet normal -0.000000e+00 1.000000e+00 -0.000000e+00 
      outer loop 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 2.277837e-03 9.000000e-03 2.095569e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.480000e-02 9.000000e-03 2.098000e-02 
         vertex 1.452216e-02 9.000000e-03 2.095569e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 0.000000e+00 9.000000e-03 2.700000e-02 
         vertex 2.000000e-03 9.000000e-03 2.098000e-02 
         vertex 0.000000e+00 9.000000e-03 2.098000e-02 
      endloop 
   endfacet 
   facet normal 0.000000e+00 1.000000e+00 0.000000e+00 
      outer loop 
         vertex 1.680000e-02 9.000000e-03 2.700000e-02 
         vertex 1.680000e-02 9.000000e-03 2.098000e-02 
         vertex 1.480000e-02 9.000000e-03 2.098000e-02 
      endloop 
   endfacet 
endsolid ./run_your_api.sh & # <- ✏️ update this

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
