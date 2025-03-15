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

beaconsTableFormat = 2
beacons = {
	{
		display_name = _('DEIR_EZ-ZOR');
		beaconId = 'world_0';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'DRZ';
		frequency = 295000.000000;
		position = { 25885.554688, 210.741055, 390774.875000 };
		direction = 0.000000;
		positionGeo = { latitude = 35.289111, longitude = 40.187417 };
		sceneObjects = {'t:265781248'};
	};
	{
		display_name = _('GAZIANTEP');
		beaconId = 'world_1';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'GAZ';
		frequency = 432000.000000;
		position = { 210917.500000, 692.497213, 146450.375000 };
		direction = 0.000000;
		positionGeo = { latitude = 36.953265, longitude = 37.468546 };
		sceneObjects = {'t:348651520'};
	};
	{
		display_name = _('BANIAS');
		beaconId = 'world_2';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'BAN';
		frequency = 304000.000000;
		position = { 22735.679688, 8.349090, 5935.380859 };
		direction = 0.000000;
		positionGeo = { latitude = 35.228286, longitude = 35.957919 };
		sceneObjects = {'t:263324964'};
	};
	{
		display_name = _('ALEPPO');
		beaconId = 'world_3';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'ALE';
		frequency = 396000.000000;
		position = { 126175.296875, 383.767260, 123040.015625 };
		direction = 0.000000;
		positionGeo = { latitude = 36.185997, longitude = 37.223297 };
		sceneObjects = {'t:310182023'};
	};
	{
		display_name = _('KAHRAMANMARAS');
		beaconId = 'world_4';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'KHM';
		frequency = 374000.000000;
		position = { 276904.968750, 501.965376, 101895.742188 };
		direction = 0.000000;
		positionGeo = { latitude = 37.540196, longitude = 36.952346 };
		sceneObjects = {'t:374571008'};
	};
	{
		display_name = _('MEZZEH');
		beaconId = 'world_5';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'MEZ';
		frequency = 358000.000000;
		position = { -171265.828125, 727.153620, 25122.662109 };
		direction = 0.000000;
		positionGeo = { latitude = 33.485917, longitude = 36.226722 };
		sceneObjects = {'t:201690543'};
	};
	{
		display_name = _('KLEYATE');
		beaconId = 'world_6';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'RA';
		frequency = 450000.000000;
		position = { -48636.152344, 4.410843, 7884.588867 };
		direction = 0.000000;
		positionGeo = { latitude = 34.586111, longitude = 36.002778 };
		sceneObjects = {'t:243567680'};
	};
	{
		display_name = _('KARIATAIN');
		beaconId = 'world_7';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'KTN';
		frequency = 372500.000000;
		position = { -91109.367188, 752.277974, 120109.953125 };
		direction = 0.000000;
		positionGeo = { latitude = 34.227181, longitude = 37.233667 };
		sceneObjects = {'t:232622590'};
	};
	{
		display_name = _('ALEPPO');
		beaconId = 'world_8';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'MER';
		frequency = 365000.000000;
		position = { 124610.679688, 372.728827, 130592.070313 };
		direction = 0.000000;
		positionGeo = { latitude = 36.173114, longitude = 37.307553 };
		sceneObjects = {'t:310249747'};
	};
	{
		display_name = _('TURAIF');
		beaconId = 'world_9';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'TRF';
		frequency = 116100000.000000;
		position = { -373082.937500, 851.637692, 257743.343750 };
		direction = 0.000000;
		positionGeo = { latitude = 31.696111, longitude = 38.735583 };
		sceneObjects = {'t:44367872'};
	};
	{
		display_name = _('DEIR_EZ-ZOR');
		beaconId = 'world_10';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'DRZ';
		frequency = 117000000.000000;
		position = { 26163.650391, 221.187477, 387712.218750 };
		direction = 0.000000;
		positionGeo = { latitude = 35.291944, longitude = 40.153778 };
		sceneObjects = {'t:91979776'};
	};
	{
		display_name = _('BAYSUR');
		beaconId = 'world_11';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'BAR';
		frequency = 113900000.000000;
		position = { -137939.109375, 834.106852, -36216.691406 };
		direction = 0.000000;
		positionGeo = { latitude = 33.769444, longitude = 35.555556 };
		sceneObjects = {'t:73436275'};
	};
	{
		display_name = _('ALEPPO');
		beaconId = 'world_12';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'ALE';
		frequency = 114500000.000000;
		position = { 125445.273736, 382.200455, 121782.458582 };
		direction = 0.000000;
		positionGeo = { latitude = 36.179210, longitude = 37.209468 };
		sceneObjects = {'t:106172250'};
	};
	{
		display_name = _('MARKA');
		beaconId = 'world_13';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'AMN';
		frequency = 116300000.000000;
		position = { -335215.781250, 672.064637, 5626.460938 };
		direction = 0.000000;
		positionGeo = { latitude = 32.004072, longitude = 36.065986 };
		sceneObjects = {'t:44400927'};
	};
	{
		display_name = _('GAZIANTEP');
		beaconId = 'world_14';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'GAZ';
		frequency = 116700000.000000;
		position = { 210721.703125, 688.248064, 146838.281250 };
		direction = 0.000000;
		positionGeo = { latitude = 36.951556, longitude = 37.472937 };
		sceneObjects = {'t:121798656'};
	};
	{
		display_name = _('ROSH-PINA');
		beaconId = 'world_15';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'ROP';
		frequency = 115300000.000000;
		position = { -225297.281250, 261.793353, -37497.257813 };
		direction = 0.000000;
		positionGeo = { latitude = 32.982528, longitude = 35.572778 };
		sceneObjects = {'t:59769246'};
	};
	{
		display_name = _('TANF');
		beaconId = 'world_16';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'TAN';
		frequency = 114000000.000000;
		position = { -175046.437500, 704.918491, 250662.015625 };
		direction = 0.000000;
		positionGeo = { latitude = 33.482383, longitude = 38.654064 };
		sceneObjects = {'t:67469362'};
	};
	{
		display_name = _('NATANIA');
		beaconId = 'world_17';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'NAT';
		frequency = 112400000.000000;
		position = { -295272.531250, 21.331805, -96707.609375 };
		direction = 0.000000;
		positionGeo = { latitude = 32.333889, longitude = 34.968889 };
		sceneObjects = {'t:45942939'};
	};
	{
		display_name = _('KAHRAMANMARAS');
		beaconId = 'world_18';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'KHM';
		frequency = 113900000.000000;
		position = { 276922.218750, 503.680679, 101996.453125 };
		direction = 0.000000;
		positionGeo = { latitude = 37.540371, longitude = 36.953481 };
		sceneObjects = {'t:130383872'};
	};
	{
		display_name = _('KARIATAIN');
		beaconId = 'world_19';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'KTN';
		frequency = 117700000.000000;
		position = { -92699.039063, 737.505681, 122897.039063 };
		direction = 0.000000;
		positionGeo = { latitude = 34.213283, longitude = 37.264208 };
		sceneObjects = {'t:78544896'};
	};
	{
		display_name = _('');
		beaconId = 'airfield2_0';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IADA';
		frequency = 108700000.000000;
		position = { 220391.796875, 17.000017, -46974.203125 };
		direction = -124.198999;
		positionGeo = { latitude = 36.990667, longitude = 35.294835 };
		sceneObjects = {'t:352027538'};
		chartOffsetX = 3030.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield2_1';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IADA';
		frequency = 108700000.000000;
		position = { 218806.687500, 17.000017, -49094.089844 };
		direction = -125.000001;
		positionGeo = { latitude = 36.975663, longitude = 35.271760 };
		sceneObjects = {'t:714343684'};
	};
	{
		display_name = _('ADANA');
		beaconId = 'airfield2_2';
		type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
		callsign = 'ADN';
		frequency = 395000000.000000;
		position = { 218312.296875, 12.017589, -50018.769531 };
		direction = 0.000000;
		positionGeo = { latitude = 36.970889, longitude = 35.261611 };
		sceneObjects = {'t:351470586'};
	};
	{
		display_name = _('ADANA');
		beaconId = 'airfield2_3';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'ADA';
		frequency = 112700000.000000;
		position = { 215128.828125, 6.722708, -54714.742188 };
		direction = 0.000000;
		positionGeo = { latitude = 36.940583, longitude = 35.210389 };
		sceneObjects = {'t:122327286'};
	};
	{
		display_name = _('KALDE');
		beaconId = 'airfield6_0';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'KAD';
		frequency = 112600000.000000;
		position = { -133511.000000, 12.000012, -42518.968750 };
		direction = 0.000000;
		positionGeo = { latitude = 33.807390, longitude = 35.485991 };
		sceneObjects = {'t:73307064'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_1';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IBB';
		frequency = 110100000.000000;
		position = { -130320.203125, 12.000011, -42839.488281 };
		direction = -11.107002;
		positionGeo = { latitude = 33.836015, longitude = 35.481357 };
		sceneObjects = {'t:510821922'};
		chartOffsetX = 3722.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_2';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IKK';
		frequency = 110700000.000000;
		position = { -131222.921875, 12.000012, -40739.011719 };
		direction = -145.184003;
		positionGeo = { latitude = 33.828534, longitude = 35.504347 };
		sceneObjects = {'t:218628880'};
		chartOffsetX = 4140.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_3';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'BIL';
		frequency = 109500000.000000;
		position = { -133542.953125, 12.000012, -42243.164063 };
		direction = -1.121999;
		positionGeo = { latitude = 33.807187, longitude = 35.488977 };
		sceneObjects = {'t:218501264'};
		chartOffsetX = 3510.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_4';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IBB';
		frequency = 110100000.000000;
		position = { -133400.578125, 12.000012, -42358.453125 };
		direction = -11.106999;
		positionGeo = { latitude = 33.808433, longitude = 35.487681 };
		sceneObjects = {'t:218501253'};
		chartOffsetX = 3722.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_5';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'BIL';
		frequency = 109500000.000000;
		position = { -130440.914063, 12.000012, -42160.257813 };
		direction = -1.121999;
		positionGeo = { latitude = 33.835138, longitude = 35.488728 };
		sceneObjects = {'t:510821923'};
		chartOffsetX = 3510.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_6';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IKK';
		frequency = 110700000.000000;
		position = { -133752.968750, 12.000012, -42644.976563 };
		direction = -145.184003;
		positionGeo = { latitude = 33.805173, longitude = 35.484721 };
		sceneObjects = {'t:509281067'};
		chartOffsetX = 4140.000000;
	};
	{
		display_name = _('BEIRUT');
		beaconId = 'airfield6_7';
		type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
		callsign = 'BOD';
		frequency = 351000000.000000;
		position = { -122843.140625, 3.000003, -42540.902344 };
		direction = 0.000000;
		positionGeo = { latitude = 33.903416, longitude = 35.481814 };
		sceneObjects = {'t:224460815'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_0';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDA';
		frequency = 109900000.000000;
		position = { -177673.234375, 612.000612, 53046.078125 };
		direction = 52.625842;
		positionGeo = { latitude = 33.434538, longitude = 36.528631 };
		sceneObjects = {'t:480608361'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDA';
		frequency = 109900000.000000;
		position = { -180064.953125, 612.000612, 50398.371094 };
		direction = 49.999989;
		positionGeo = { latitude = 33.412415, longitude = 36.500792 };
		sceneObjects = {'t:197594063'};
		chartOffsetX = 4063.000000;
	};
	{
		display_name = _('Damascus');
		beaconId = 'airfield7_2';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'DAM';
		frequency = 116000000.000000;
		position = { -185235.656250, 610.646057, 47226.804688 };
		direction = 0.000000;
		positionGeo = { latitude = 33.365131, longitude = 36.468066 };
		sceneObjects = {'t:65082116'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_3';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'DAML';
		frequency = 111100000.000000;
		position = { -180656.640625, 612.000612, 52815.601563 };
		direction = -130.000016;
		positionGeo = { latitude = 33.407604, longitude = 36.526916 };
		sceneObjects = {'t:195693526'};
		chartOffsetX = 3926.000000;
	};
	{
		display_name = _('DAMASCUS');
		beaconId = 'airfield7_4';
		type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
		callsign = 'DAL';
		frequency = 342000000.000000;
		position = { -171986.156250, 592.527326, 60026.417969 };
		direction = 0.000000;
		positionGeo = { latitude = 33.487262, longitude = 36.602245 };
		sceneObjects = {'t:25791255'};
	};
	{
		display_name = _('ABYAD');
		beaconId = 'airfield7_5';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'ABD';
		frequency = 264000.000000;
		position = { -188453.578125, 626.805558, 43443.796875 };
		direction = 0.000000;
		positionGeo = { latitude = 33.335299, longitude = 36.428295 };
		sceneObjects = {'t:24051773'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_6';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'DAML';
		frequency = 111100000.000000;
		position = { -183014.953125, 612.000673, 50184.816406 };
		direction = -130.000001;
		positionGeo = { latitude = 33.385786, longitude = 36.499259 };
		sceneObjects = {'t:476250836'};
	};
	{
		display_name = _('HATAY');
		beaconId = 'airfield15_0';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'HTY';
		frequency = 112050000.000000;
		position = { 147753.046875, 77.282510, 39662.273438 };
		direction = -45.690008;
		positionGeo = { latitude = 36.362972, longitude = 36.290113 };
		sceneObjects = {'t:110139987'};
	};
	{
		display_name = _('');
		beaconId = 'airfield15_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IHAT';
		frequency = 108900000.000000;
		position = { 149019.375000, 78.702029, 40718.835938 };
		direction = -135.689998;
		positionGeo = { latitude = 36.374642, longitude = 36.301483 };
		sceneObjects = {'t:322309965'};
		chartOffsetX = 3421.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield15_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IHAT';
		frequency = 108900000.000000;
		position = { 146921.406250, 76.628716, 38518.476563 };
		direction = -136.812009;
		positionGeo = { latitude = 36.355193, longitude = 36.277639 };
		sceneObjects = {'t:668369721'};
	};
	{
		display_name = _('HATAY');
		beaconId = 'airfield15_3';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'HTY';
		frequency = 336000.000000;
		position = { 147771.703125, 77.308656, 39693.753906 };
		direction = -45.690008;
		positionGeo = { latitude = 36.363148, longitude = 36.290458 };
		sceneObjects = {'t:322309964'};
	};
	{
		display_name = _('');
		beaconId = 'airfield15_4';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IHTY';
		frequency = 108150000.000000;
		position = { 148442.625000, 76.732834, 40322.585938 };
		direction = 43.187996;
		positionGeo = { latitude = 36.369348, longitude = 36.297250 };
		sceneObjects = {'t:669680597'};
	};
	{
		display_name = _('');
		beaconId = 'airfield15_5';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IHTY';
		frequency = 108150000.000000;
		position = { 146243.843750, 76.015060, 38009.367188 };
		direction = 44.310007;
		positionGeo = { latitude = 36.348962, longitude = 36.272184 };
		sceneObjects = {'t:320443707'};
		chartOffsetX = 3582.000000;
	};
	{
		display_name = _('INCIRLIC');
		beaconId = 'airfield16_0';
		type = BEACON_TYPE_VORTAC;
		callsign = 'DAN';
		frequency = 108400000.000000;
		channel = 21;
		position = { 222639.437500, 73.699811, -33216.257813 };
		direction = 0.000000;
		positionGeo = { latitude = 37.015611, longitude = 35.448194 };
		sceneObjects = {'t:124814096'};
	};
	{
		display_name = _('');
		beaconId = 'airfield16_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IDAN';
		frequency = 109300000.000000;
		position = { 222315.812500, 73.638219, -33655.464844 };
		direction = -125.002995;
		positionGeo = { latitude = 37.012552, longitude = 35.443402 };
		sceneObjects = {'t:356059568'};
		chartOffsetX = 3617.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield16_2';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IDAN';
		frequency = 109300000.000000;
		position = { 220377.484375, 48.478762, -36217.171875 };
		direction = -125.000001;
		positionGeo = { latitude = 36.994247, longitude = 35.415483 };
		sceneObjects = {'t:716015361'};
		chartOffsetX = 3617.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield16_3';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'DANM';
		frequency = 111700000.000000;
		position = { 222001.125000, 63.883437, -34347.218750 };
		direction = 55.000018;
		positionGeo = { latitude = 37.009487, longitude = 35.435774 };
		sceneObjects = {'t:717392620'};
	};
	{
		display_name = _('');
		beaconId = 'airfield16_4';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'DANM';
		frequency = 111700000.000000;
		position = { 220088.968750, 46.796395, -36832.261719 };
		direction = 54.996984;
		positionGeo = { latitude = 36.991443, longitude = 35.408706 };
		sceneObjects = {'t:352452685'};
		chartOffsetX = 3639.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield21_0';
		type = BEACON_TYPE_ILS_GLIDESLOPE;
		callsign = 'IBA';
		frequency = 109100000.000000;
		position = { 43061.328125, 28.398050, 5722.375977 };
		direction = -1.444000;
		positionGeo = { latitude = 35.411243, longitude = 35.948708 };
		sceneObjects = {'t:595624788'};
	};
	{
		display_name = _('');
		beaconId = 'airfield21_1';
		type = BEACON_TYPE_ILS_LOCALIZER;
		callsign = 'IBA';
		frequency = 109100000.000000;
		position = { 40365.289063, 25.536156, 5696.175781 };
		direction = -1.444000;
		positionGeo = { latitude = 35.386960, longitude = 35.949335 };
		sceneObjects = {'t:268665151'};
		chartOffsetX = 2966.000000;
	};
	{
		display_name = _('LATAKIA');
		beaconId = 'airfield21_2';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'LTK';
		frequency = 114800000.000000;
		position = { 41444.796875, 27.958673, 5975.583496 };
		direction = 0.000000;
		positionGeo = { latitude = 35.396758, longitude = 35.952041 };
		sceneObjects = {'t:93095699'};
	};
	{
		display_name = _('LATAKIA');
		beaconId = 'airfield21_3';
		type = BEACON_TYPE_AIRPORT_HOMER_WITH_MARKER;
		callsign = 'LTK';
		frequency = 414000000.000000;
		position = { 50737.488281, 120.053244, 5622.082031 };
		direction = -1.444000;
		positionGeo = { latitude = 35.480331, longitude = 35.944991 };
		sceneObjects = {'t:34832953'};
	};
	{
		display_name = _('PALMYRA');
		beaconId = 'airfield28_0';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'PLR';
		frequency = 363000.000000;
		position = { -55843.890625, 413.752092, 215545.687500 };
		direction = 0.000000;
		positionGeo = { latitude = 34.555739, longitude = 38.266916 };
		sceneObjects = {'t:241895405'};
	};
	{
		display_name = _('PALMYRA');
		beaconId = 'airfield28_1';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'PAL';
		frequency = 337000.000000;
		position = { -56185.023438, 389.323876, 218419.765625 };
		direction = 0.000000;
		positionGeo = { latitude = 34.552847, longitude = 38.298267 };
		sceneObjects = {'t:241928761'};
	};
	{
		display_name = _('RAMATDAVID');
		beaconId = 'airfield30_0';
		type = BEACON_TYPE_AIRPORT_HOMER;
		callsign = 'RMD';
		frequency = 368000.000000;
		position = { -259140.859375, 40.595511, -74983.906250 };
		direction = 0.000000;
		positionGeo = { latitude = 32.666274, longitude = 35.185461 };
		sceneObjects = {'t:153027415'};
	};
	{
		display_name = _('Cheka');
		beaconId = 'airfield40_0';
		type = BEACON_TYPE_VOR_DME;
		callsign = 'CAK';
		frequency = 116200000.000000;
		position = { -79459.062500, 193.868297, -20936.335938 };
		direction = 0.000000;
		positionGeo = { latitude = 34.300489, longitude = 35.699946 };
		sceneObjects = {'t:79463354'};
	};
	
	
	--MOD start
	
	-- Adana RWY 04
	{
		display_name = _('');
		beaconId = 'airfield2_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IADA';
		frequency = 15475000000.000000;
		channel = 3;
		position = { 220391.796875, 17.000017, -46974.203125 };
		direction = -124.198999;
		positionGeo = { latitude = 36.990667, longitude = 35.294835 };
		sceneObjects = {'t:352027538'};
		chartOffsetX = 3030.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield2_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IADA';
		frequency = 15475000000.000000;
		channel = 3;
		position = { 218806.687500, 17.000017, -49094.089844 };
		direction = -125.000001;
		positionGeo = { latitude = 36.975663, longitude = 35.271760 };
		sceneObjects = {'t:714343684'};
	};
	{
		display_name = _('');
		beaconId = 'airfield2_6';
		type = BEACON_TYPE_TACAN;
		callsign = 'IADA';
		frequency = 108700000.000000;
		channel = 24;
		position = { 218806.687500, 17.000017, -49094.089844 };
		direction = 0.000000;
		positionGeo = { latitude = 36.975663, longitude = 35.271760 };
		sceneObjects = {'t:714343684'};
	};
----------------
	
-- Beirut RWY 16
	{
		display_name = _('');
		beaconId = 'airfield6_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IBB';
		frequency = 15655000000.000000;
		channel = 9;
		position = { -133400.578125, 12.000012, -42358.453125 };
		direction = -11.106999;
		positionGeo = { latitude = 33.808433, longitude = 35.487681 };
		sceneObjects = {'t:218501253'};
		chartOffsetX = 3722.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_1';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IBB';
		frequency = 15655000000.000000;
		channel = 9;
		position = { -130320.203125, 12.000011, -42839.488281 };
		direction = -11.107002;
		positionGeo = { latitude = 33.836015, longitude = 35.481357 };
		sceneObjects = {'t:510821922'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_1';
		type = BEACON_TYPE_TACAN;
		callsign = 'IBB';
		frequency = 110100000.000000;
		channel = 38;
		position = { -130320.203125, 12.000011, -42839.488281 };
		direction = -11.107002;
		positionGeo = { latitude = 33.836015, longitude = 35.481357 };
		sceneObjects = {'t:510821922'};
	};
	
-- Beirut RWY 03	
	{
		display_name = _('');
		beaconId = 'airfield6_2';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IKK';
		frequency = 15415000000.000000;
		channel = 11;
		position = { -131222.921875, 12.000012, -40739.011719 };
		direction = -145.184003;
		positionGeo = { latitude = 33.828534, longitude = 35.504347 };
		sceneObjects = {'t:218628880'};
		chartOffsetX = 4140.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_6';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IKK';
		frequency = 15415000000.000000;
		channel = 11;
		position = { -133752.968750, 12.000012, -42644.976563 };
		direction = -145.184003;
		positionGeo = { latitude = 33.805173, longitude = 35.484721 };
		sceneObjects = {'t:509281067'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_6';
		type = BEACON_TYPE_TACAN;
		callsign = 'IKK';
		frequency = 110700000.000000;
		channel = 44;
		position = { -133752.968750, 12.000012, -42644.976563 };
		direction = -145.184003;
		positionGeo = { latitude = 33.805173, longitude = 35.484721 };
		sceneObjects = {'t:509281067'};
	};

-- Beirut RWY 17
	{
		display_name = _('');
		beaconId = 'airfield6_3';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'BIL';
		frequency = 15685000000.000000;
		channel = 10;
		position = { -133542.953125, 12.000012, -42243.164063 };
		direction = -1.121999;
		positionGeo = { latitude = 33.807187, longitude = 35.488977 };
		sceneObjects = {'t:218501264'};
		chartOffsetX = 3510.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield6_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'BIL';
		frequency = 15685000000.000000;
		channel = 10;
		position = { -130440.914063, 12.000012, -42160.257813 };
		direction = -1.121999;
		positionGeo = { latitude = 33.835138, longitude = 35.488728 };
		sceneObjects = {'t:510821923'};
	};
	{
		display_name = _('');
		beaconId = 'airfield6_5';
		type = BEACON_TYPE_TACAN;
		callsign = 'BIL';
		frequency = 109500000.000000;
		channel = 32;
		position = { -130440.914063, 12.000012, -42160.257813 };
		direction = 0.000000;
		positionGeo = { latitude = 33.835138, longitude = 35.488728 };
		sceneObjects = {'t:510821923'};
	};

----------------

-- Damascus RWY 23R
	{
		display_name = _('');
		beaconId = 'airfield7_7';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IDA';
		frequency = 15565000000.000000;
		channel = 6;
		position = { -180064.953125, 612.000612, 50398.371094 };
		direction = 49.999989;
		positionGeo = { latitude = 33.412415, longitude = 36.500792 };
		sceneObjects = {'t:197594063'};
		chartOffsetX = 4063.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield7_8';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IDA';
		frequency = 15565000000.000000;
		channel = 6;
		position = { -177673.234375, 612.000612, 53046.078125 };
		direction = 52.625842;
		positionGeo = { latitude = 33.434538, longitude = 36.528631 };
		sceneObjects = {'t:480608361'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_9';
		type = BEACON_TYPE_TACAN;
		callsign = 'IDA';
		frequency = 109900000.000000;
		channel = 36;
		position = { -177673.234375, 612.000612, 53046.078125 };
		direction = 0.000000;
		positionGeo = { latitude = 33.434538, longitude = 36.528631 };
		sceneObjects = {'t:480608361'};
	};

	-- Damascus RWY 05R

	{
		display_name = _('');
		beaconId = 'airfield7_10';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DAML';
		frequency = 15595000000.000000;
		channel = 7;
		position = { -180656.640625, 612.000612, 52815.601563 };
		direction = -130.000016;
		positionGeo = { latitude = 33.407604, longitude = 36.526916 };
		sceneObjects = {'t:195693526'};
		chartOffsetX = 3926.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield7_11';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DAML';
		frequency = 15595000000.000000;
		channel = 7;
		position = { -183014.953125, 612.000673, 50184.816406 };
		direction = -130.000001;
		positionGeo = { latitude = 33.385786, longitude = 36.499259 };
		sceneObjects = {'t:476250836'};
	};
	{
		display_name = _('');
		beaconId = 'airfield7_12';
		type = BEACON_TYPE_TACAN;
		callsign = 'DAML';
		frequency = 111100000.000000;
		channel = 48;
		position = { -183014.953125, 612.000673, 50184.816406 };
		direction = 0.000000;
		positionGeo = { latitude = 33.385786, longitude = 36.499259 };
		sceneObjects = {'t:476250836'};
	};
----------------

-- Hatay RWY 04
	{
		display_name = _('');
		beaconId = 'airfield15_6';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IHAT';
		frequency = 15535000000.000000;
		channel = 5;
		position = { 149019.375000, 78.702029, 40718.835938 };
		direction = -135.689998;
		positionGeo = { latitude = 36.374642, longitude = 36.301483 };
		sceneObjects = {'t:322309965'};
		chartOffsetX = 3421.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield15_7';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IHAT';
		frequency = 15535000000.000000;
		channel = 5;
		position = { 146921.406250, 76.628716, 38518.476563 };
		direction = -136.812009;
		positionGeo = { latitude = 36.355193, longitude = 36.277639 };
		sceneObjects = {'t:668369721'};
	};
	{
		display_name = _('');
		beaconId = 'airfield15_8';
		type = BEACON_TYPE_TACAN;
		callsign = 'IHAT';
		frequency = 108900000.000000;
		channel = 26;
		position = { 146921.406250, 76.628716, 38518.476563 };
		direction = 0.000000;
		positionGeo = { latitude = 36.355193, longitude = 36.277639 };
		sceneObjects = {'t:668369721'};
	};

-- Hatay RWY 22
	{
		display_name = _('');
		beaconId = 'airfield15_9';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IHTY';
		frequency = 15505000000.000000;
		channel = 4;
		position = { 146243.843750, 76.015060, 38009.367188 };
		direction = 44.310007;
		positionGeo = { latitude = 36.348962, longitude = 36.272184 };
		sceneObjects = {'t:320443707'};
		chartOffsetX = 3582.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield15_10';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IHTY';
		frequency = 15505000000.000000;
		channel = 4;
		position = { 148442.625000, 76.732834, 40322.585938 };
		direction = 43.187996;
		positionGeo = { latitude = 36.369348, longitude = 36.297250 };
		sceneObjects = {'t:669680597'};
	};
	{
		display_name = _('');
		beaconId = 'airfield15_11';
		type = BEACON_TYPE_TACAN;
		callsign = 'IHTY';
		frequency = 108150000.000000;
		channel = 18;
		position = { 148442.625000, 76.732834, 40322.585938 };
		direction = 0.000000;
		positionGeo = { latitude = 36.369348, longitude = 36.297250 };
		sceneObjects = {'t:669680597'};
	};
----------------

--Incirlik RWY 05	
	{
		display_name = _('');
		beaconId = 'airfield16_5';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IDAN';
		frequency = 15445000000.000000;
		channel = 2;
		position = { 222315.812500, 73.638219, -33655.464844 };
		direction = -125.002995;
		positionGeo = { latitude = 37.012552, longitude = 35.443402 };
		sceneObjects = {'t:356059568'};
		chartOffsetX = 3617.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield16_6';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IDAN';
		frequency = 15445000000.000000;
		channel = 2;
		position = { 220377.484375, 48.478762, -36217.171875 };
		direction = -125.000001;
		positionGeo = { latitude = 36.994247, longitude = 35.415483 };
		sceneObjects = {'t:716015361'};
	};
	{
		display_name = _('');
		beaconId = 'airfield16_7';
		type = BEACON_TYPE_TACAN;
		callsign = 'IDAN';
		frequency = 109300000.000000;
		channel = 30;
		position = { 220377.484375, 48.478762, -36217.171875 };
		direction = 0.000000;
		positionGeo = { latitude = 36.994247, longitude = 35.415483 };
		sceneObjects = {'t:716015361'};
	};
-- Incirlik RWY 23
	
	{
		display_name = _('');
		beaconId = 'airfield16_8';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'DANM';
		frequency = 15415000000.000000;
		channel = 1;
		position = { 220088.968750, 46.796395, -36832.261719 };
		direction = 54.996984;
		positionGeo = { latitude = 36.991443, longitude = 35.408706 };
		sceneObjects = {'t:352452685'};
		chartOffsetX = 3639.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield16_9';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'DANM';
		frequency = 15415000000.000000;
		channel = 1;
		position = { 222001.125000, 63.883437, -34347.218750 };
		direction = 55.000018;
		positionGeo = { latitude = 37.009487, longitude = 35.435774 };
		sceneObjects = {'t:717392620'};
	};
	{
		display_name = _('');
		beaconId = 'airfield16_10';
		type = BEACON_TYPE_TACAN;
		callsign = 'DANM';
		frequency = 111700000.000000;
		channel = 54;
		position = { 222001.125000, 63.883437, -34347.218750 };
		direction = 0.000000;
		positionGeo = { latitude = 37.009487, longitude = 35.435774 };
		sceneObjects = {'t:717392620'};
	};

----------------

-- Khmeimim/Bassel Al-Assad RWY 17R
	{
		display_name = _('');
		beaconId = 'airfield21_4';
		type = BEACON_TYPE_ICLS_LOCALIZER;
		callsign = 'IBA';
		frequency = 15625000000.000000;
		channel = 8;
		position = { 40365.289063, 25.536156, 5696.175781 };
		direction = -1.444000;
		positionGeo = { latitude = 35.386960, longitude = 35.949335 };
		sceneObjects = {'t:268665151'};
		chartOffsetX = 2966.000000;
	};
	{
		display_name = _('');
		beaconId = 'airfield21_5';
		type = BEACON_TYPE_ICLS_GLIDESLOPE;
		callsign = 'IBA';
		frequency = 15625000000.000000;
		channel = 8;
		position = { 43061.328125, 28.398050, 5722.375977 };
		direction = -1.444000;
		positionGeo = { latitude = 35.411243, longitude = 35.948708 };
		sceneObjects = {'t:595624788'};
	};
	{
		display_name = _('');
		beaconId = 'airfield21_6';
		type = BEACON_TYPE_TACAN;
		callsign = 'IBA';
		frequency = 109100000.000000;
		channel = 28;
		position = { 43061.328125, 28.398050, 5722.375977 };
		direction = 0.000000;
		positionGeo = { latitude = 35.411243, longitude = 35.948708 };
		sceneObjects = {'t:595624788'};
	};

----------------

	
} --name ${VOLUME} --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash
        docker cp ./. ${VOLUME}:/${WORKING_DIR}
        docker run -d --rm -it --name ${BUILDER} --volumes-from=${VOLUME} -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

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
