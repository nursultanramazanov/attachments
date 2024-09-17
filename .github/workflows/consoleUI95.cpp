# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow checks out code, performs a Codacy security scan
# and integrates the results with the
# GitHub Advanced Security code scanning feature.  For more information on
# the Codacy security scan action usage and parameters, see
# https://github.com/codacy/codacy-analysis-cli-action.
# For more information on Codacy Analysis CLI in general, see
# https://github.com/codacy/codacy-analysis-cli.

name: Codacy Security Scan

on: #!/usr/bin/env php
<?php

use Symfony\Bundle\FrameworkBundle\Console\Application;
use Symfony\Component\Console\Input\ArgvInput;
use Symfony\Component\Debug\Debug;

// if you don't want to setup permissions the proper way, just uncomment the following PHP line
// read http://symfony.com/doc/current/book/installation.html#configuration-and-setup for more information
//umask(0000);

set_time_limit(0);

/**
 * @var Composer\Autoload\ClassLoader $loader
 */
$loader = require __DIR__.'/../app/autoload.php';

$input = new ArgvInput();
$env = $input->getParameterOption(['--env', '-e'], getenv('SYMFONY_ENV') ?: 'dev');
$debug = getenv('SYMFONY_DEBUG') !== '0' && !$input->hasParameterOption(['--no-debug', '']) && $env !== 'prod';

if ($debug) {
    Debug::enable();
}

$kernel = new AppKernel($env, $debug);
$application = new Application($kernel);
$application->run($input);
  push: #!/usr/bin/env php
<?php

require_once dirname(__FILE__).'/../var/SymfonyRequirements.php';

$lineSize = 70;
$symfonyRequirements = new SymfonyRequirements();
$iniPath = $symfonyRequirements->getPhpIniConfigPath();

echo_title('Symfony Requirements Checker');

echo '> PHP is using the following php.ini file:'.PHP_EOL;
if ($iniPath) {
    echo_style('green', '  '.$iniPath);
} else {
    echo_style('warning', '  WARNING: No configuration file (php.ini) used by PHP!');
}

echo PHP_EOL.PHP_EOL;

echo '> Checking Symfony requirements:'.PHP_EOL.'  ';

$messages = array();
foreach ($symfonyRequirements->getRequirements() as $req) {
    /** @var $req Requirement */
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('red', 'E');
        $messages['error'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

$checkPassed = empty($messages['error']);

foreach ($symfonyRequirements->getRecommendations() as $req) {
    if ($helpText = get_error_message($req, $lineSize)) {
        echo_style('yellow', 'W');
        $messages['warning'][] = $helpText;
    } else {
        echo_style('green', '.');
    }
}

if ($checkPassed) {
    echo_block('success', 'OK', 'Your system is ready to run Symfony projects');
} else {
    echo_block('error', 'ERROR', 'Your system is not ready to run Symfony projects');

    echo_title('Fix the following mandatory requirements', 'red');

    foreach ($messages['error'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

if (!empty($messages['warning'])) {
    echo_title('Optional recommendations to improve your setup', 'yellow');

    foreach ($messages['warning'] as $helpText) {
        echo ' * '.$helpText.PHP_EOL;
    }
}

echo PHP_EOL;
echo_style('title', 'Note');
echo '  The command console could use a different php.ini file'.PHP_EOL;
echo_style('title', '~~~~');
echo '  than the one used with your web server. To be on the'.PHP_EOL;
echo '      safe side, please check the requirements from your web'.PHP_EOL;
echo '      server using the ';
echo_style('yellow', 'web/config.php');
echo ' script.'.PHP_EOL;
echo PHP_EOL;

exit($checkPassed ? 0 : 1);

function get_error_message(Requirement $requirement, $lineSize)
{
    if ($requirement->isFulfilled()) {
        return;
    }

    $errorMessage = wordwrap($requirement->getTestMessage(), $lineSize - 3, PHP_EOL.'   ').PHP_EOL;
    $errorMessage .= '   > '.wordwrap($requirement->getHelpText(), $lineSize - 5, PHP_EOL.'   > ').PHP_EOL;

    return $errorMessage;
}

function echo_title($title, $style = null)
{
    $style = $style ?: 'title';

    echo PHP_EOL;
    echo_style($style, $title.PHP_EOL);
    echo_style($style, str_repeat('~', strlen($title)).PHP_EOL);
    echo PHP_EOL;
}

function echo_style($style, $message)
{
    // ANSI color codes
    $styles = array(
        'reset' => "\033[0m",
        'red' => "\033[31m",
        'green' => "\033[32m",
        'yellow' => "\033[33m",
        'error' => "\033[37;41m",
        'success' => "\033[37;42m",
        'title' => "\033[34m",
    );
    $supports = has_color_support();

    echo($supports ? $styles[$style] : '').$message.($supports ? $styles['reset'] : '');
}

function echo_block($style, $title, $message)
{
    $message = ' '.trim($message).' ';
    $width = strlen($message);

    echo PHP_EOL.PHP_EOL;

    echo_style($style, str_repeat(' ', $width).PHP_EOL);
    echo_style($style, str_pad(' ['.$title.']', $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_pad($message, $width, ' ', STR_PAD_RIGHT).PHP_EOL);
    echo_style($style, str_repeat(' ', $width).PHP_EOL);
}

function has_color_support()
{
    static $support;

    if (null === $support) {
        if (DIRECTORY_SEPARATOR == '\\') {
            $support = false !== getenv('ANSICON') || 'ON' === getenv('ConEmuANSI');
        } else {
            $support = function_exists('posix_isatty') && @posix_isatty(STDOUT);
        }
    }

    return $support;
}
    branches: [ "main" ]
  pull_request: <?php
namespace Memtext\Form;

abstract class AbstractForm
{
    public $errorMessage;

    public function validate()
    {
        $rules = $this->rules();
        foreach ($rules as $field=>$list) {
            foreach ($list as $rule=>$attributes) {
                $validator = 'validate' . ucfirst($rule);
                if ( !$this->$validator($field, $attributes) ) {
                    return false;
                }
            }
        }
        return true;
    }

    protected function validateNotEmpty($field, $flag = true)
    {
        if (empty($this->$field)) {
            $this->errorMessage = "$field не может быть пустым";
            return false;
        }
        return true;
    }

    protected function validateMaxLength($field, $maxLength)
    {
        if (mb_strlen($this->$field) > $maxLength) {
            $this->errorMessage = "$field должен быть не длиннее $maxLength символов";
            return false;
        }
        return true;
    }

    protected function validateMinLength($field, $minLength)
    {
        if (mb_strlen($this->$field) < $minLength) {
            $this->errorMessage = "$field должен быть не короче $minLength символов";
            return false;
        }
        return true;
    }

    protected function validateIsEmail($field, $flag = true)
    {
        $regExp = '/^[^@\s]+@[^@\s]+\.[^@\s]+$/ui';
        if (!preg_match($regExp, $this->$field)) {
            $this->errorMessage = 'Некорректный адрес электронной почты';
            return false;
        }
        return true;
    }
}
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: <?php
namespace Memtext\Form;

use Memtext\Model\User;
use \Psr\Http\Message\ServerRequestInterface as Request;
use \Memtext\Helper\HashGenerator;

class LoginForm extends AbstractForm
{
    const USER_NOT_FOUND = 'Пользователь не найден';
    const WRONG_PASSWORD = 'Неправильный пароль';

    private $user;

    public $email;
    public $password;
    public $remember;

    public function __construct(Request $request)
    {
        $loginData = $request->getParsedBody()['loginForm'];
        $this->email = isset($loginData['email']) ? $loginData['email'] : null;
        $this->password =
            isset($loginData['password']) ? $loginData['password'] : null;
        $this->remember = isset($loginData['remember']);
    }

    public function validatePassword(User $user = null)
    {
        if ($user == null) {
            $this->errorMessage = self::USER_NOT_FOUND;
            return false;
        } elseif (
            $user->getSaltedHash() !==
            HashGenerator::generateHash($user->getSalt(), $this->password)
        ) {
            $this->errorMessage = self::WRONG_PASSWORD;
            return false;
        }
        $this->user = $user;
        return true;
    }

    protected function rules()
    {
        return [
            'email' =>
                ['notEmpty' => true, 'isEmail' => true, 'maxLength' => 50],
            'password' =>
                ['notEmpty' => true, 'minLength' => 5, 'maxLength' => 50],
        ];
    }

    public function getUser()
    {
        return $this->user;
    }
}
    - cron: <?php
namespace Memtext\Form;

use Psr\Http\Message\ServerRequestInterface as Request;

class LogoutForm extends AbstractForm
{
    public $csrf_token;

    public function __construct(Request $request)
    {
        $formData = $request->getParsedBody()['logoutForm'];
        $this->csrf_token = isset($formData['csrf_token']) ?
            $formData['csrf_token'] : null;
    }
}

permissions: <?php
namespace Memtext\Form;

use Psr\Http\Message\ServerRequestInterface as Request;
use Memtext\Model\User;
use Memtext\Helper\HashGenerator;

class RegisterForm extends AbstractForm
{
    const LOGIN_OCCUPIED = 'Имя занято, попробуйте другое';
    const MAIL_OCCUPIED = 'С этой почты уже была регистрация';
    const REPEAT_PASSWORD = 'Пароли не совпадают';

    public $login;
    public $email;
    public $password;
    public $repeatPassword;
    public $remember;

    private $user;

    public function __construct(Request $request)
    {
        $regData = isset($request->getParsedBody()['registerForm'])
                   ? $request->getParsedBody()['registerForm'] : null;
        $this->login = isset($regData['login']) ? $regData['login'] : null;
        $this->email = isset($regData['email']) ? $regData['email'] : null;
        $this->password = isset($regData['password']) ? $regData['password'] : null;
        $this->repeatPassword = 
            isset($regData['repeatPassword']) ? $regData['repeatPassword'] : null;
        $this->remember = isset($regData['remember']);

        $this->createUser();
    }

    public function validateUniqueEmail($user = null)
    {
        if (!$user) {
            return true;
        }
        $this->errorMessage = self::MAIL_OCCUPIED;
        return false;
    }

    public function validateUniqueLogin($user = null)
    {
        if (!$user) {
            return true;
        }
        $this->errorMessage = self::LOGIN_OCCUPIED;
        return false;
    }

    public function getUser()
    {
        return $this->user;
    }

    protected function validateEquals($field, $toWhat)
    {
        if ($this->$field === $this->$toWhat) {
            return true;
        }
        $this->errorMessage = self::REPEAT_PASSWORD;
        return false;
    }

    private function createUser()
    {
        $user = new User;
        $user->setLogin($this->login);
        $user->setEmail($this->email);
        $user->setSalt(HashGenerator::generateSalt());
        $user->setSaltedHash(
            HashGenerator::generateHash($user->getSalt(), $this->password)
        );
        $this->user = $user;
    }

    protected function rules()
    {
        return [
            'login' =>
                ['notEmpty'=>true, 'maxLength'=>30, 'minLength'=>4],
            'email' =>
                ['notEmpty'=>true, 'maxLength'=>255, 'isEmail'=>true],
            'password' =>
                ['notEmpty'=>true, 'maxLength'=>50, 'minLength'=>5],
            'repeatPassword' =>
                ['equals' => 'password'],
        ];
    }
}
  contents: <?php
namespace Memtext\Form;

use \Psr\Http\Message\ServerRequestInterface as Request;

class TextForm extends AbstractForm
{
    public $content;
    public $title;

    public function __construct(Request $request, \HTMLPurifier $purifier)
    {
        $textFormData = $request->getParsedBody()['textForm'];
        $this->content = isset($textFormData['content'])
                         ? $purifier->purify($textFormData['content']) : null;
        $this->title = isset($textFormData['title'])
                         ? $textFormData['title'] : null;
    }

    protected function rules()
    {
        return [
            'content' => ['notEmpty' => true, 'maxLength' => 65000],
            'title' => ['notEmpty' => true, 'maxLength' => 255],
        ];
    }
}

jobs: <?php
namespace Memtext\Handler;

use Slim\Handlers\NotFound; 
use Slim\Views\Twig; 
use Psr\Http\Message\ServerRequestInterface as Request; 
use Psr\Http\Message\ResponseInterface as Response;

class NotFoundHandler extends NotFound
{
    private $view;

    public function __construct(Twig $view)
    { 
        $this->view = $view; 
    }

    public function __invoke(Request $request, Response $response)
    {
        $this->view->render($response, 'not_found.twig');
        return $response->withStatus(404); 
    }
}
  codacy-security-scan: <?php
namespace Memtext\Helper;

class Csrf
{
    public static function init()
    {
        if (!isset($_COOKIE['csrf_token'])) {
            $token = HashGenerator::generateSalt();
        } else {
            $token = $_COOKIE['csrf_token'];
        }
        setcookie('csrf_token', $token, time() + 24*60*60, '/');
        return $token;
    }
}
    permissions: <?php
namespace Memtext\Helper;

class HashGenerator
{
    const SALT_LENGTH = 40;

    public static function getCharacters()
    {
        return '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
                . '~`!@#$%^&*()-=+_][{}|?><';
    }

    public static function generateSalt()
    {
        $salt = '';
        $characters = self::getCharacters();
        $charactersLength = strlen($characters);
        for ($i = 0; $i < self::SALT_LENGTH; $i++) {
            $salt .= $characters[rand(0, $charactersLength - 1)];
        }
        return $salt;
    }

    public static function generateHash($salt, $password)
    {
        return sha1($salt . $password);
    }
}
      contents: <?php
namespace Memtext\Helper;

class Pager
{
    public $currentPage;
    public $totalItemCount;
    public $perPage;
    public $linksCount;

    private $maxLinksCount;

    public function __construct(
        $currentPage,
        $totalItemCount,
        $perPage = 15,
        $maxLinksCount = 6
    ) {
        $this->currentPage = intval($currentPage);
        $this->totalItemCount = intval($totalItemCount);
        $this->perPage = $perPage;
        $this->maxLinksCount = $maxLinksCount;
        $pageCount = $this->getPageCount();
        $this->linksCount = $this->getLinksCount($pageCount);
    }

    public function getPrevPage()
    {
        return $this->currentPage - 1;
    }

    public function getNextPage()
    {
        return $this->currentPage + 1;
    }

    public function getFirstPage()
    {
        return
            $this->currentPage + $this->linksCount - 1 <= $this->getPageCount()
            ? $this->currentPage
            : $this->getPageCount() - $this->linksCount + 1;
    }

    public function getLastPage()
    {
        return
            $this->currentPage + $this->linksCount - 1 <= $this->getPageCount()
            ? $this->getFirstPage() + $this->linksCount - 1
            : $this->getPageCount();
    }

    public function getOffset()
    {
        return ($this->currentPage - 1) * $this->perPage;
    }

    private function getPageCount()
    {
        return intval(ceil($this->totalItemCount / $this->perPage));
    }

    private function getLinksCount($pageCount)
    {
        return ($this->maxLinksCount > $pageCount)
            ? $pageCount
            : $this->maxLinksCount;
    }
}
      security-events: <?php
namespace Memtext\Helper;

class TextParser
{
    private $words;
    private $text;

    public function parse($text)
    {
        $this->text = $text;

        $this
            ->splitToWords()
            ->deleteLongWords()
            ->filterUnique()
            ->toLowerCase();

        return array_values($this->words);
    }

    private function splitToWords()
    {
        $this->words = preg_split(
            '/(\'s|\'t)?\W+/ui',
            $this->text,
            -1,
            \PREG_SPLIT_NO_EMPTY
        );
        return $this;
    }

    private function deleteLongWords($maxLength = 50)
    {
        $callback = function ($word) use ($maxLength) {
            return mb_strlen($word, 'UTF-8') <= $maxLength;
        };
        $this->words = array_filter($this->words, $callback);
        return $this;
    }

    private function filterUnique()
    {
        $this->words = array_unique($this->words);
        return $this;
    }

    private function toLowerCase()
    {
        array_walk($this->words, function (&$word) {
            $word = mb_convert_case($word, MB_CASE_LOWER);
        });
        return $this;
    }
}
      actions: <?php
namespace Memtext\Mapper;

use Doctrine\DBAL\Connection;

class SphinxMapper
{
    private $connection;

    public function __construct(Connection $connection)
    {
        $this->connection = $connection;
    }

    public function find(array $words, $indexName)
    {
        $sql = "SELECT id, word, type FROM {$indexName} WHERE MATCH(?) LIMIT 10000".
                " OPTION ranker=sph04";
        $conn = $this->connection;
        $sth = $conn->prepare($sql);
        $sth->execute([ join('|', $words) ]);
        $ids = [];
        return $sth->fetchAll();
    }
}
    name: <?php
namespace Memtext\Model;

use Doctrine\Common\Collections\ArrayCollection;

/**
 * Class Text
 * @package Memtext\Model
 *
 * @Entity
 * @Table(name="text")
 */
class Text
{
    /**
     * @var int
     *
     * @Id @Column(type="integer")
     * @GeneratedValue(strategy="AUTO")
     */
    private $id;

    /**
     * @var string
     *
     * @Column(type="text")
     */
    private $content;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $title;

    /**
     * @var User
     *
     * @ManyToOne(targetEntity="User", inversedBy="texts")
     */
    private $author;

    /**
     * @var ArrayCollection
     *
     * @ManyToMany(targetEntity="Word")
     * @JoinTable(
     *   name="text_dictionary",
     *   joinColumns={@JoinColumn(name="text_id", referencedColumnName="id")}),
     *   inverseJoinColumns={@JoinColumn(name="word_id", referencedColumnName="id")}
     * )
     */
    private $words;

    /**
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    public function __construct()
    {
        $this->words = new ArrayCollection();
    }

    /**
     * @return string
     */
    public function getContent()
    {
        return $this->content;
    }

    /**
     * @param string $content
     */
    public function setContent($content)
    {
        $this->content = $content;
    }

    /**
     * @return string
     */
    public function getTitle()
    {
        return $this->title;
    }

    /**
     * @param string $title
     */
    public function setTitle($title)
    {
        $this->title = $title;
    }

    /**
     * @return User
     */
    public function getAuthor()
    {
        return $this->author;
    }

    /**
     * @param User $author
     */
    public function setAuthor(User $author)
    {
        $author->attachText($this);
        $this->author = $author;
    }

    /**
     * @return ArrayCollection
     */
    public function getWords()
    {
        return $this->words;
    }

    /**
     * @return array
     */
    public function getShortWordsArray()
    {
        $entities = $this->words->toArray();
        $words = [];
        foreach ($entities as $entity) {
            if ($entity->getType() != 'short') {
                continue;
            }
            $words[$entity->getKeyword()] = $entity->getDefinition();
        }
        return $words;
    }

    /**
     * @param array $words
     */
    public function setWords(array $words)
    {
        foreach ($words as $word) {
            $this->words[] = $word;
        }
    }
}
    runs-on: <?php
namespace Memtext\Model;

use Doctrine\Common\Collections\ArrayCollection;

/**
 * Class User
 * @package Memtext\Model
 *
 * @Entity
 * @Table(name="user")
 */
class User
{
    /**
     * @var int
     *
     * @Id @Column(type="integer")
     * @GeneratedValue(strategy="AUTO")
     */
    private $id;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $login;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $email;

    /**
     * @var string
     *
     * @Column(name="salted_hash", type="string")
     */
    private $saltedHash;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $salt;

    /**
     * @var ArrayCollection
     *
     * @OneToMany(targetEntity="Text", mappedBy="author")
     */
    private $texts;

    /**
     * @var ArrayCollection
     *
     * @ManyToMany(targetEntity="Word")
     * @JoinTable(
     *   name="user_dictionary",
     *   joinColumns={@JoinColumn(name="user_id", referencedColumnName="id")},
     *   inverseJoinColumns={@JoinColumn(name="word_id", referencedColumnName="id")}
     * )
     */
    private $ignoredWords;

    public function __construct()
    {
        $this->texts = new ArrayCollection();
        $this->ignoredWords = new ArrayCollection();
    }

    /**
     * @return ArrayCollection
     */
    public function getIgnoredWords()
    {
        return $this->ignoredWords;
    }

    /**
     * @param array $words
     */
    public function ignore(array $words)
    {
        foreach ($words as $word) {
            $this->ignoredWords[] = $word;
        }
    }

    /**
     * @param array $texts
     */
    public function attachTexts(array $texts)
    {
        foreach ($texts as $text) {
            $this->texts[] = $text;
        }
    }

    /**
     * @param Text $text
     */
    public function attachText(Text $text)
    {
        $this->texts[] = $text;
    }

    /**
     * @return ArrayCollection
     */
    public function getTexts()
    {
        return $this->texts;
    }

    /**
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * @return string
     */
    public function getLogin()
    {
        return $this->login;
    }

    /**
     * @param $login
     */
    public function setLogin($login)
    {
        $this->login = $login;
    }

    /**
     * @return string
     */
    public function getEmail()
    {
        return $this->email;
    }

    /**
     * @param $email
     */
    public function setEmail($email)
    {
        $this->email = $email;
    }

    /**
     * @return string
     */
    public function getSaltedHash()
    {
        return $this->saltedHash;
    }

    /**
     * @param $saltedHash
     */
    public function setSaltedHash($saltedHash)
    {
        $this->saltedHash = $saltedHash;
    }

    /**
     * @return string
     */
    public function getSalt()
    {
        return $this->salt;
    }

    /**
     * @param $salt
     */
    public function setSalt($salt)
    {
        $this->salt = $salt;
    }
}
    steps: <?php
namespace Memtext\Model;

/**
 * Class Dictionary
 * @package Memtext\Model
 *
 * @Entity
 * @Table(name="dictionary")
 */
class Word
{
    /**
     * @var int
     *
     * @Id @Column(type="integer")
     * @GeneratedValue(strategy="AUTO")
     */
    private $id;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $keyword;

    /**
     * @var string
     *
     * @Column(type="text")
     */
    private $definition;

    /**
     * @var string
     *
     * @Column(type="string")
     */
    private $type;



    /**
     * @return string
     */
    public function getType()
    {
        return $this->type;
    }

    /**
     * @param string $type
     */
    public function setType($type)
    {
        $this->type = $type;
    }

    /**
     * @return int
     */
    public function getId()
    {
        return $this->id;
    }

    /**
     * @return string
     */
    public function getKeyword()
    {
        return $this->keyword;
    }

    /**
     * @param string $keyword
     */
    public function setKeyword($keyword)
    {
        $this->keyword = $keyword;
    }

    /**
     * @return string
     */
    public function getDefinition()
    {
        return $this->definition;
    }

    /**
     * @param string $definition
     */
    public function setDefinition($definition)
    {
        $this->definition = $definition;
    }
}
      # Checkout the repository to the GitHub Actions runner
      - name: <?php
namespace Memtext\Service;

use Doctrine\ORM\EntityRepository;
use Memtext\Form\LoginForm;
use Memtext\Form\RegisterForm;
use Memtext\Model\User;

class LoginManager
{
    private $repo;
    private $loggedUser;
    private $token;

    public function __construct(EntityRepository $repo, $token) {
        $this->repo = $repo;
        $this->loggedUser = $this->getLoggedUser();
        $this->token = $token;
    }

    public function getLoggedUser()
    {
        $id = isset($_COOKIE['id']) ? intval($_COOKIE['id']) : null;
        $hash = isset($_COOKIE['hash']) ? $_COOKIE['hash'] : null;
        if (!$id or !$hash) {
            return null;
        }
        $user = $this->repo->find($id);
        if (!$user) {
            return null;
        } elseif ($user->getSaltedHash() != $hash) {
            return null;
        }
        return $user;
    }

    public function validateLoginForm(LoginForm $form)
    {
        if (!$form->validate()) {
            return false;
        }
        $user = $this->repo->findOneBy( ['email'=>$form->email] );
        return $form->validatePassword($user);
    }

    public function validateRegisterForm(RegisterForm $form)
    {
        if (!$form->validate()) {
            return false;
        }
        $foundedUser = $this->repo->findOneBy( ['email'=>$form->email] );
        if (!$form->validateUniqueEmail($foundedUser)) {
            return false;
        }
        $foundedUser = $this->repo->findOneBy( ['login'=>$form->login] );
        return $form->validateUniqueLogin($foundedUser);
    }

    public function login(User $user, $remember = true, $time = 604800)
    {
        $expires = $remember ? time() + $time : 0;
        $path = '/';
        setcookie('id', $user->getId(), $expires, $path);
        setcookie('hash', $user->getSaltedHash(), $expires, $path);
        $this->loggedUser = $user;
    }

    public function logout()
    {
        setcookie('id', '', time() - 3600);
        setcookie('hash', '', time() - 3600);
        $this->loggedUser = null;
    }

    public function getUserID()
    {
        if ($this->isLogged()) {
            return $this->loggedUser->getId();
        }
        return null;
    }

    public function getUserLogin()
    {
        if ($this->isLogged()) {
            return $this->loggedUser->getLogin();
        }
        return null;
    }

    public function isLogged()
    {
        if ($this->loggedUser !== null) {
            return true;
        }
        return false;
    }

    public function isOwner($textAuthorId)
    {
        if (intval($this->getUserId()) === intval($textAuthorId)) {
            return true;
        }
        return false;
    }

    public function checkToken($formToken)
    {
        return $this->token === $formToken;
    }

    public function getToken()
    {
        return $this->token;
    }
}
        uses: <?php
namespace Memtext\Service;

use Memtext\Mapper\SphinxMapper;

/**
 * Class SphinxService
 * @package Memtext\Service
 */
class SphinxService
{
    /**
     * @var SphinxMapper
     */
    private $mapper;

    /**
     * @var string
     */
    private $indexName;

    /**
     * @param SphinxMapper $mapper
     * @param string $indexName
     */
    public function __construct(
        SphinxMapper $mapper,
        $indexName
    ) {
        $this->mapper = $mapper;
        $this->indexName = $indexName;
    }

    public function find(array $words)
    {
        return $this->mapper->find($words, $this->indexName);
    }
}

      # Execute Codacy Analysis CLI and generate a SARIF output with the security issues identified during the analysis
      - name: <?php
namespace Memtext\Service;

use Doctrine\ORM\EntityManager;
use Doctrine\DBAL\Connection;
use Memtext\Helper\TextParser;
use Memtext\Model\Text;

class TextService
{
    private $parser;
    private $entityManager;
    private $sphinxService;

    public function __construct(
        TextParser $parser,
        EntityManager $entityManager,
        SphinxService $sphinxService
    ) {
        $this->parser = $parser;
        $this->entityManager = $entityManager;
        $this->sphinxService = $sphinxService;
    }

    public function saveWithDictionary(Text $text)
    {
        $words = $this->parseText($text);
        $references = $this->createDictionary($words, $text);

        $text->setWords($references);
        $this->entityManager->persist($text);
        $this->entityManager->flush();
    }

    public function getUserTextCount($userId)
    {
        $em = $this->entityManager;
        $qb = $em->createQueryBuilder();
        $query = $qb->select('COUNT(t)')
            ->from('Memtext:Text', 't')
            ->where('t.author=:id')
            ->setParameter('id', $userId)
            ->getQuery();
        return $query->getSingleScalarResult();
    }

    public function getTextWithWords($textId)
    {
        $dql = "SELECT t, w FROM Memtext:Text t JOIN t.words w WHERE t.id=?1";
        $query = $this->entityManager->createQuery($dql);
        $query->setParameter(1, $textId);
        return $query->getResult()[0];
    }

    private function parseText(Text $text)
    {
        $textContent = strip_tags($text->getContent());
        return $this->parser->parse($textContent);
    }

    private function findHits(array $words)
    {
        return $this->sphinxService->find($words);
    }

    private function filterHits(array $hits, $textContent)
    {
        $filter = function ($word) use ($textContent) {
            if (strpos($textContent, $word['word']) === false) {
                return false;
            } else {
                return true;
            }
        };

        return array_filter($hits, $filter);
    }

    private function getReferences(array $ids)
    {
        $refs = [];
        $em = $this->entityManager;
        foreach ($ids as $id) {
            $refs[] = $em->getReference('Memtext:Word', $id);
        }
        return $refs;
    }

    private function createDictionary(array $words, Text $text)
    {
        $references = [];
        $stopWords = [];
        $ids = [];

        foreach ($words as $word) {
            $hits = $this->findHits([$word]);
            if (!$hits) {
                $stopWords[] = $word;
                continue;
            }
            $hits = $this->filterHits($hits, $text->getContent());
            $ids = array_merge($ids, $this->getHitsIds($hits));
        }
        $references = $this->getReferences(array_unique($ids));
        $fromDb = $this->findInDb($stopWords);

        return array_merge($references, $fromDb);
    }

    private function getHitsIds(array $hits)
    {
        $ids = [];
        foreach ($hits as $hit) {
            $ids[] = $hit['id'];
        }
        return array_unique($ids);
    }

    private function findInDb(array $stopWords)
    {
        $dql = "SELECT w.id FROM dictionary w WHERE w.keyword IN (?)";
        $conn = $this->entityManager->getConnection();
        $stmt = $conn->executeQuery($dql, [$stopWords], [Connection::PARAM_STR_ARRAY]);
        $hits = $stmt->fetchAll();
        $ids = $this->getHitsIds($hits);
        return $this->getReferences(array_unique($ids));
    }
}
        uses: <?php

use Doctrine\ORM\Tools\Setup;
use Doctrine\ORM\EntityManager;

require "../vendor/autoload.php";
require "../config/settings.php";

$devMode = true;
$config = Setup::createAnnotationMetadataConfiguration(
    [__DIR__ . "/Model"],
    $devMode
);
$config->addEntityNamespace('Memtext', 'Memtext\Model');

$db = $settings['db'];
$conn = [
    'host' => $db['host'],
    'dbname' => $db['dbname'],
    'user' => $db['user'],
    'pass' => $db['pass'],
    'driver' => $db['driver'],
];

$em = EntityManager::create($conn, $config);
        with: <?php

use Doctrine\DBAL\Configuration;
use Doctrine\DBAL\DriverManager;
use Memtext\Mapper\SphinxMapper;
use Memtext\Service\TextService;
use Memtext\Service\LoginManager;
use Memtext\Handler\NotFoundHandler;
use Memtext\Helper\TextParser;

$container = $app->getContainer();

$container['csrf_token'] = $token;

// obtain em from bootstrap.php
$container['entityManager'] = $em;

$container['sphinx_conn'] = function ($c) {
    $dbalConfig = new Configuration();
    $sphinx_config = $c['settings']['sphinx'];
    $driver = $c['settings']['db']['driver'];
    $connectionParams = [
        'host' => $sphinx_config['host'],
        'port' => $sphinx_config['port'],
        'driver' => $driver,
    ];
    return DriverManager::getConnection(
        $connectionParams,
        $dbalConfig
    );
};

$container['sphinxMapper'] = function ($c) {
    return new SphinxMapper($c['sphinx_conn']);
};

$container['loginManager'] = function ($c) {
    return new LoginManager(
        $c['entityManager']->getRepository('Memtext:User'),
        $c['csrf_token']
    );
};

$container['purifier'] = function ($c) {
    $config = \HTMLPurifier_Config::createDefault();
    $settings = $c['settings']['purifier'];
    foreach ($settings as $key => $value) {
        $config->set($key, $value);
    }
    return new \HTMLPurifier($settings);
};

$container['textParser'] = new TextParser;

$container['textService'] = function ($c) {
    return new TextService(
        $c['textParser'],
        $c['entityManager'],
        $c['sphinxService']
    );
};

$container['sphinxService'] = function ($c) {
    $sphinxConfig = $c['settings']['sphinx'];
    return new \Memtext\Service\SphinxService(
        $c['sphinxMapper'],
        $sphinxConfig['indexName']
    );
};

$container['view'] = function ($c) {
    $view = new \Slim\Views\Twig('../templates');
    $view->addExtension(new \Twig_Extensions_Extension_Text());
    $view['loginManager'] = $c['loginManager'];
    $view['csrf_token'] = $c['csrf_token'];
    return $view;
};

$container['notFoundHandler'] = function ($c) {
    return new NotFoundHandler(
        $c['view'],
        function (Request $request, Response $response) use ($c) {
            return $c['response']->withStatus(404);
        });
};
          # Check https://github.com/codacy/codacy-analysis-cli#project-token to get your project token from your Codacy repository
          # You can also omit the token and run the tools that support default configurations
          project-token: ar:Arabic
bg:Bulgarian
ca:Catalan
cs:Czech
da:Danish
de:German
el:Greek
es:Spanish
et:Estonian
eu:Basque
fi:Finnish
fr:French
he:Hebrew
hi:Hindi
hr:Croatian
hu:Hungarian
hy:Armenian
is:Icelandic
it:Italian
ja:Japanese
ko:Korean
ku:Kurdish
lt:Lithuanian
lv:Latvian
nb:Norwegian Bokmal
nl:Dutch
pl:Polish
pt_BR:Brazilian Portuguese
pt:Portuguese
ro:Romanian
ru:Russian
sk:Slovak
sl:Slovenian
sr:Serbian
sv:Swedish
ta:Tamil
tr:Turkish
uk:Ukrainian
ur:Urdu
vi:Vietnamese
zh_CN:Chinese (Simplified)
zh_TW:Chinese (Traditional)
          verbose: #!/usr/bin/python3
"""
This script copies the translation files (`.po`) to `./translation-tables/po` and updates them
against the corresponding translation template (`.pot`) via msgmerge and gets the untranslated
strings via msgattrib.

Translation infos are stored in a matrix (dictionary):

                        | length | fr | es | zh_CN | ...
force-quit@cinnamon.org |      3 |  3 |  3 |     0 | ...
weather@mockturtl       |    109 |  4 | 12 |     0 | ...
sticky@scollins         |     46 | 46 |  1 |     0 | ...
.......

where length means the number of translatable strings in the `.pot` file and the other entries are
the number of untranslated strings in the locale `.po` file.

The information in the matrix is used to generate 3 types of translation status tables:
1. README.md
2. UUID.md
3. LOCALE.md
which are stored in `./translation-tables/tables`.
"""

import os
import collections
import shutil
import urllib.parse
import sys

REPO_FOLDER = os.path.realpath(os.path.abspath(os.path.join(
    os.path.normpath(os.path.join(os.getcwd(), *([".."] * 1))))))
REPO_NAME = os.path.basename(REPO_FOLDER)
SPICES_TYPE = REPO_NAME.split('-')[-1]
SPICES_REPO_URL = "https://github.com/linuxmint/cinnamon-spices-" + SPICES_TYPE + "/blob/master/"
SPICES_TYPE = SPICES_TYPE.title()


def terminal_progressbar_update(count, total):
    """ Show progressbar in terminal: https://gist.github.com/vladignatyev/06860ec2040cb497f0f3 """
    bar_len = 60
    filled_len = int(round(bar_len * count / float(total)))

    percents = round(100.0 * count / float(total), 1)
    terminal_bar = '=' * filled_len + ' ' * (bar_len - filled_len)

    if count != total:
        sys.stdout.write('[%s] %s%s\r' % (terminal_bar, percents, '%'))
        sys.stdout.flush()
    else:
        sys.stdout.write('[%s] %s%s\n' % (terminal_bar, percents, '%'))
        sys.stdout.flush()

def get_table_head(class2name):
    """ Opens HTML table tag, adds table head and opens table body tag. """
    table_head = '<table>\n'
    table_head += '  <thead>\n'
    table_head += '    <tr>\n'
    for class_id in class2name:
        table_head += '      <th>\n'
        table_head += '        <a href="#" id="' + class_id + '">' + class2name[class_id] + '</a>\n'
        table_head += '      </th>\n'
    table_head += '    </tr>\n'
    table_head += '  </thead>\n'
    table_head += '  <tbody>\n'
    return table_head

def get_table_content(class2value):
    """ Creates a HTML table row. """
    table_content = '    <tr>\n'
    for class_id in class2value:
        table_content += ('      <td class="' + class_id
                          + '" data-value="' + class2value[class_id][0] + '">\n')
        table_content += '        ' + class2value[class_id][1] + '\n'
        table_content += '      </td>\n'
    table_content += '    </tr>\n'
    return table_content

def get_table_body_close():
    """ Closes HTML body tag. """
    table_body_ending = '  </tbody>\n'
    return table_body_ending

def get_table_close():
    """ Closes HTML table tag. """
    table_ending = '</table>\n\n'
    return table_ending

def str2html_href(link, text):
    """ Creates a HTML code snippet with href link. """
    return '<a href="' + link + '">' + text + '</a>'

def value2html_progress_image(percentage):
    """ Creates a HTML code snippet, which links to a progress bar image to a given percentage. """
    return '<img src="https://progress-bar.dev/' + percentage + '" alt="' + percentage + '%" />'

def progress(untranslated, translated):
    """ Calculates percentage for translation progress. """
    return str(int(round(100 * float(translated - untranslated)/float(translated))))


def populate_id2name():
    """ Reads LINGUAS file and creates an array locale_id->country_name."""
    with open(os.path.join(REPO_FOLDER, ".translation-tables", "LINGUAS"), "r") as linguas_file:
        for linguas_id_line in linguas_file:
            lang_id = linguas_id_line.split(':')[0]
            lang_name = linguas_id_line.split(':')[1].rstrip()
            ID2NAME[lang_id] = lang_name

def check_hidden_dirs():
    """
    Creates hidden dirs `po` and `tables` if they don't exist,
    else removes deleted spices in those dirs.
    """

    if not os.path.isdir(TABLES_DIR):
        os.makedirs(TABLES_DIR)

    #% remove tables for deleted spices
    if os.path.isdir(HIDDEN_PO_DIR):
        for uuid in os.listdir(HIDDEN_PO_DIR):
            if not os.path.isdir(os.path.join(REPO_FOLDER, uuid)):
                shutil.rmtree(os.path.join(HIDDEN_PO_DIR, uuid))
                if os.path.isfile(os.path.join(TABLES_DIR, uuid + '.md')):
                    os.remove(os.path.join(TABLES_DIR, uuid + '.md'))
    else:
        os.makedirs(HIDDEN_PO_DIR)

def populate_translation_matrix():
    """ POPULATE TRANSLATION MATRIX """

    #% Progressbar for Terminal
    prog_total = len([uuid for uuid in os.listdir(REPO_FOLDER)])
    prog_iter = 0

    #% for UUID
    for uuid in os.listdir(REPO_FOLDER):
        #% show progressbar in terminal
        if len(sys.argv) != 2 or sys.argv[1] != "--quiet":
            prog_iter += 1
            terminal_progressbar_update(prog_iter, prog_total)

        #% ignore files and hidden dirs
        if uuid.startswith('.') or not os.path.isdir(os.path.join(REPO_FOLDER, uuid)):
            continue

        #% ignore spices without po dir
        spices_po_dir = os.path.join(REPO_FOLDER, uuid, "files", uuid, "po")
        if not os.path.isdir(spices_po_dir):
            continue

        #% get pot file directory
        pot_file_path = None
        for file in os.listdir(spices_po_dir):
            if file.endswith(".pot"):
                pot_file_path = os.path.join(spices_po_dir, file)

        if pot_file_path == None:
            print("No potfile found for %s, skipping it." % uuid)
            continue

        #% count number of translatable Strings in pot file
        pot_length = int(os.popen('grep "^msgid " ' + pot_file_path + ' | wc -l').read()) - 1
        TRANSLATION_UUID_MATRIX[uuid]["length"] = pot_length
        TRANSLATION_LANG_MATRIX["length"][uuid] = pot_length
        #% init translation matrix
        for known_id in ID2NAME:
            TRANSLATION_UUID_MATRIX[uuid][known_id] = pot_length
            TRANSLATION_LANG_MATRIX[known_id][uuid] = pot_length

        #% # create uuid dir in HIDDEN_PO_DIR
        updated_spices_po_dir = os.path.join(HIDDEN_PO_DIR, uuid)
        try:
            os.makedirs(updated_spices_po_dir)
        except OSError:
            pass
        #% # creating po files in hidden po dir
        for po_file in os.listdir(spices_po_dir):
            if po_file.endswith('.po'):
                current_id = po_file.split('.')[0]
                if current_id in ID2NAME:
                    po_file_path = os.path.join(spices_po_dir, po_file)
                    updated_po_file_path = os.path.join(updated_spices_po_dir, po_file)
                    untranslated_po_file_path = os.path.join(updated_spices_po_dir, '_' + po_file)
                    try:
                        os.remove(untranslated_po_file_path)
                    except OSError:
                        pass
                    #% copy po files to HIDDEN_PO_DIR
                    shutil.copyfile(po_file_path, updated_po_file_path)
                    #% update po from pot
                    os.system('msgmerge --silent --update --backup=none ' + updated_po_file_path
                              + ' ' + pot_file_path)
                    # remove fuzzy and extract untranslated
                    os.system('msgattrib --clear-fuzzy --empty ' + updated_po_file_path
                              + ' | msgattrib --untranslated --output-file='
                              + untranslated_po_file_path)

                    #% if no untranslated exist
                    if not os.path.exists(untranslated_po_file_path):
                        TRANSLATION_UUID_MATRIX[uuid][current_id] = 0
                        TRANSLATION_LANG_MATRIX[current_id][uuid] = 0
                    else:
                        # count untranslated Strings
                        untranslated_length = int(os.popen('grep "^msgid " '
                                                           + untranslated_po_file_path
                                                           + ' | wc -l').read()) - 1
                        TRANSLATION_UUID_MATRIX[uuid][current_id] = untranslated_length
                        TRANSLATION_LANG_MATRIX[current_id][uuid] = untranslated_length

                else:
                    print("Unknown locale: " + uuid + "/po/" + po_file)


def create_uuid_tables():
    """ CREATE UUID.md TRANSLATION TABLES """
    #% TABLE: UUID.md
    for uuid in TRANSLATION_LANG_MATRIX["length"]:
        with open(os.path.join(TABLES_DIR, uuid + '.md'), "w") as uuid_table_file:
            #% TABLE HEAD
            thead_class2name = collections.OrderedDict()
            thead_class2name["language"] = "Language"
            thead_class2name["idpo"] = "ID.po"
            thead_class2name["status"] = "Status"
            thead_class2name["untranslated"] = "Untranslated"
            uuid_table_file.write(get_table_head(thead_class2name))

            uuid_pot_length = TRANSLATION_UUID_MATRIX[uuid]["length"]
            for locale in sorted(ID2NAME):
                if not os.path.isfile(os.path.join(HIDDEN_PO_DIR, uuid, locale + '.po')):
                    continue
                # TABLE CONTENT
                tdata_class2value = collections.OrderedDict()

                tdata_value = ID2NAME[locale]
                tdata_content = str2html_href(locale + '.md', tdata_value)
                tdata_class2value["language"] = [tdata_value, tdata_content]

                tdata_value = locale
                github_po_link = (SPICES_REPO_URL + urllib.parse.quote(uuid) + '/files/'
                                  + urllib.parse.quote(uuid) + '/po/' + locale + '.po')
                tdata_content = str2html_href(github_po_link, tdata_value + '.po')
                tdata_class2value["idpo"] = [tdata_value, tdata_content]

                untranslated_length = TRANSLATION_UUID_MATRIX[uuid][locale]
                tdata_value = progress(untranslated_length, uuid_pot_length)
                tdata_content = value2html_progress_image(tdata_value)
                tdata_class2value["status"] = [tdata_value, tdata_content]

                tdata_value = str(untranslated_length)
                if tdata_value == "0":
                    tdata_content = tdata_value
                else:
                    tdata_content = str2html_href('../po/' + uuid + '/_' + locale + '.po',
                                                  tdata_value)
                tdata_class2value["untranslated"] = [tdata_value, tdata_content]

                uuid_table_file.write(get_table_content(tdata_class2value))

            uuid_table_file.write(get_table_body_close())
            uuid_table_file.write(get_table_close())
        uuid_table_file.close()


def create_readme_locale_tables():
    """ CREATE README.md AND LOCALE.md TRANSLATION TABLES """
    #% README TABLE: README.md
    with open(os.path.join(TABLES_DIR, 'README.md'), "w") as language_table_file:
        #% README TABLE HEAD
        reamde_thead_class2name = collections.OrderedDict()
        reamde_thead_class2name["language"] = "Language"
        reamde_thead_class2name["localeid"] = "ID"
        reamde_thead_class2name["status"] = "Status"
        reamde_thead_class2name["untranslated"] = "Untranslated"
        language_table_file.write(get_table_head(reamde_thead_class2name))

        #% LOCALE TABLE: LOCALE.md
        for locale in sorted(ID2NAME):
            length_sum = 0
            untranslated_sum = 0
            locale_table_file_path = os.path.join(TABLES_DIR, locale + '.md')
            with open(locale_table_file_path, "w") as locale_table_file:
                #% LOCALE TABLE HEAD
                thead_class2name = collections.OrderedDict()
                thead_class2name["uuid"] = "UUID"
                thead_class2name["length"] = "Length"
                thead_class2name["status"] = "Status"
                thead_class2name["untranslated"] = "Untranslated"
                locale_table_file.write(get_table_head(thead_class2name))
                for uuid in sorted(TRANSLATION_LANG_MATRIX[locale]):
                    # LOCALE TABLE CONTENT
                    tdata_class2value = collections.OrderedDict()

                    uuid_pot_length = TRANSLATION_UUID_MATRIX[uuid]["length"]
                    length_sum += uuid_pot_length
                    untranslated_length = TRANSLATION_UUID_MATRIX[uuid][locale]
                    untranslated_sum += untranslated_length


                    tdata_value = uuid
                    tdata_content = str2html_href(uuid + '.md', tdata_value)
                    tdata_class2value["uuid"] = [tdata_value, tdata_content]

                    tdata_value = str(uuid_pot_length)
                    if untranslated_length == uuid_pot_length:
                        tdata_content = tdata_value
                    else:
                        github_po_link = (SPICES_REPO_URL + urllib.parse.quote(uuid) + '/files/'
                                          + urllib.parse.quote(uuid) + '/po/' + locale + '.po')
                        tdata_content = str2html_href(github_po_link, tdata_value)
                    tdata_class2value["length"] = [tdata_value, tdata_content]

                    tdata_value = progress(untranslated_length, uuid_pot_length)
                    tdata_content = value2html_progress_image(tdata_value)
                    tdata_class2value["status"] = [tdata_value, tdata_content]

                    tdata_value = str(untranslated_length)
                    if untranslated_length == 0 or untranslated_length == uuid_pot_length:
                        tdata_content = tdata_value
                    else:
                        tdata_content = str2html_href('../po/' + uuid + '/_' + locale + '.po',
                                                      tdata_value)
                    tdata_class2value["untranslated"] = [tdata_value, tdata_content]

                    locale_table_file.write(get_table_content(tdata_class2value))

                # README TABLE CONTENT
                readme_tdata_class2value = collections.OrderedDict()

                readme_tdata_value = ID2NAME[locale]
                readme_tdata_cont = str2html_href(locale + '.md', readme_tdata_value)
                readme_tdata_class2value["language"] = [readme_tdata_value, readme_tdata_cont]

                readme_tdata_value = locale
                readme_tdata_cont = readme_tdata_value
                readme_tdata_class2value["localeid"] = [readme_tdata_value, readme_tdata_cont]

                # LOCALE TABLE FOOT
                locale_table_file.write('  <tfoot>\n')

                tdata_value = 'Overall statistics:'
                tdata_content = '<b>' + tdata_value + '</b>'
                tdata_class2value["uuid"] = [tdata_value, tdata_content]

                tdata_value = str(length_sum)
                tdata_content = '<b>' + tdata_value + '</b>'
                tdata_class2value["length"] = [tdata_value, tdata_content]

                tdata_value = progress(untranslated_sum, length_sum)
                tdata_content = value2html_progress_image(tdata_value)
                tdata_class2value["status"] = [tdata_value, tdata_content]
                readme_tdata_class2value["status"] = [tdata_value, tdata_content]

                tdata_value = str(untranslated_sum)
                tdata_content = '<b>' + tdata_value + '</b>'
                tdata_class2value["untranslated"] = [tdata_value, tdata_content]
                readme_tdata_class2value["untranslated"] = [tdata_value, tdata_value]

                locale_table_file.write(get_table_content(tdata_class2value))
                locale_table_file.write('  </tfoot>\n')
                locale_table_file.write(get_table_close())

            locale_table_file.close()


            #% write README TABLE content (but only if translations for locale exists)
            if length_sum == untranslated_sum:
                try:
                    os.remove(locale_table_file_path)
                except OSError:
                    pass
            else:
                language_table_file.write(get_table_content(readme_tdata_class2value))

        #% README TABLE close
        language_table_file.write(get_table_body_close())
        language_table_file.write(get_table_close())
    language_table_file.close()



if __name__ == "__main__":
    #% get known lang_id and lang_name from LINGUAS
    ID2NAME = {}
    populate_id2name()

    #% create directories to store updated po files and tables
    HIDDEN_PO_DIR = os.path.join(REPO_FOLDER, ".translation-tables", "po")
    TABLES_DIR = os.path.join(REPO_FOLDER, ".translation-tables", "tables")
    check_hidden_dirs()

    #% store translation info in matrix/dict
    TRANSLATION_UUID_MATRIX = collections.defaultdict(dict) # [UUID][ID]
    TRANSLATION_LANG_MATRIX = collections.defaultdict(dict) # [ID][UUID]
    populate_translation_matrix()

    #% write/update the translation tables
    create_uuid_tables()
    create_readme_locale_tables()
          output: # SOME DESCRIPTIVE TITLE.
# This file is put in the public domain.
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
#
#, fuzzy
msgid ""
msgstr ""
"Project-Id-Version: InternetTimeDesklet@stefan 1.0\n"
"Report-Msgid-Bugs-To: https://github.com/linuxmint/cinnamon-spices-desklets/"
"issues\n"
"POT-Creation-Date: 2024-05-15 08:35+0200\n"
"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
"Language-Team: LANGUAGE <LL@li.org>\n"
"Language: \n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"

#. metadata.json->name
msgid "Internet Time"
msgstr ""

#. metadata.json->description
msgid "Shows the Internet Time in .beat"
msgstr ""
          format: # SOME DESCRIPTIVE TITLE.
# This file is put in the public domain.
#  2024.
#
msgid ""
msgstr ""
"Project-Id-Version: InternetTimeDesklet@stefan 1.0\n"
"Report-Msgid-Bugs-To: https://github.com/linuxmint/cinnamon-spices-desklets/"
"issues\n"
"POT-Creation-Date: 2024-05-15 08:35+0200\n"
"PO-Revision-Date: 2024-05-15 08:42+0200\n"
"Last-Translator: Daniel <d3vf4n (at) tutanota (dot) com>\n"
"Language-Team: \n"
"Language: ca\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"
"X-Generator: Poedit 3.0.1\n"

#. metadata.json->name
msgid "Internet Time"
msgstr "Temps d'Internet"

#. metadata.json->description
msgid "Shows the Internet Time in .beat"
msgstr "Rellotge que pren el temps d'Internet i el mostra en format «.beat»"
          # Adjust severity of non-security issues
          gh-code-scanning-compat: # SOME DESCRIPTIVE TITLE.
# This file is put in the public domain.
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
#
msgid ""
msgstr ""
"Project-Id-Version: InternetTimeDesklet@stefan 1.0\n"
"Report-Msgid-Bugs-To: https://github.com/linuxmint/cinnamon-spices-desklets/"
"issues\n"
"POT-Creation-Date: 2024-04-29 14:50-0400\n"
"PO-Revision-Date: 2024-04-29 19:16-0400\n"
"Last-Translator: \n"
"Language-Team: \n"
"Language: es\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"
"X-Generator: Poedit 3.4.2\n"

#. metadata.json->name
msgid "Internet Time"
msgstr "Hora de Internet"

#. metadata.json->description
msgid "Shows the Internet Time in .beat"
msgstr "Muestra la hora de Internet en .beat"
          # Force 0 exit code to allow SARIF file generation
          # This will handover control about PR rejection to the GitHub side
          max-allowed-issues: # SOME DESCRIPTIVE TITLE.
# This file is put in the public domain.
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
#
msgid ""
msgstr ""
"Project-Id-Version: InternetTimeDesklet@stefan 1.0\n"
"Report-Msgid-Bugs-To: https://github.com/linuxmint/cinnamon-spices-desklets/"
"issues\n"
"POT-Creation-Date: 2024-05-15 08:35+0200\n"
"PO-Revision-Date: 2024-06-26 19:34+0200\n"
"Last-Translator: \n"
"Language-Team: \n"
"Language: hu\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"
"Plural-Forms: nplurals=2; plural=(n != 1);\n"
"X-Generator: Poedit 3.0.1\n"

#. metadata.json->name
msgid "Internet Time"
msgstr "Internet Idő"

#. metadata.json->description
msgid "Shows the Internet Time in .beat"
msgstr "Megjeleníti az internet időt .beat-ben"

      # Upload the SARIF file generated in the previous step
      - name: # SOME DESCRIPTIVE TITLE.
# This file is put in the public domain.
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
#
msgid ""
msgstr ""
"Project-Id-Version: InternetTimeDesklet@stefan 1.0\n"
"Report-Msgid-Bugs-To: https://github.com/linuxmint/cinnamon-spices-desklets/"
"issues\n"
"POT-Creation-Date: 2024-05-15 08:35+0200\n"
"PO-Revision-Date: 2024-07-23 22:04+0200\n"
"Last-Translator: qadzek\n"
"Language-Team: \n"
"Language: nl\n"
"MIME-Version: 1.0\n"
"Content-Type: text/plain; charset=UTF-8\n"
"Content-Transfer-Encoding: 8bit\n"

#. metadata.json->name
msgid "Internet Time"
msgstr "Internet Tijd"

#. metadata.json->description
msgid "Shows the Internet Time in .beat"
msgstr "Toont de Internet tijd in .beat"
        uses: const Desklet = imports.ui.desklet;
const St = imports.gi.St;
const Lang = imports.lang;
const Mainloop = imports.mainloop;

function NetBeatDesklet(metadata, desklet_id) {
    this._init(metadata, desklet_id);
}

NetBeatDesklet.prototype = {
        __proto__: Desklet.Desklet.prototype,

        _init: function(metadata, desklet_id){
                Desklet.Desklet.prototype._init.call(this, metadata, desklet_id);
                this.setupUI();
                this.refresh();
                // set update intervall:
                //  1 beat = 86.4 sec.  Shorter update interval means more precise changing time of the .beat
                this.timeout = Mainloop.timeout_add_seconds(10, Lang.bind(this, this.refresh));
                // only update, if the desklet is running
        this.keepUpdating = true;
        },

        setupUI: function() {

                // I assume that the standard Desklet setting is with no decoration, in case of the header is visible, I set a text for it:
                this.setHeader("Internet Time");

                // Vertical frame, containing title and itime
                this.frame = new St.BoxLayout({vertical:true});
                this.row_title = new St.BoxLayout({vertical:false});
                this.row_itime = new St.BoxLayout({vertical:false});

                // as I want a red dot and black text, I combine 2 seperate labels.
                //        If there is a more simple solution, please tell me.
                //
                // You can change Font and Style here:
                // Red dot:
                this.title_text_1 = new St.Label();
                this.title_text_1.style = "font-family: 'FreeSans'; "
                                                                + "font-weight: bold; "
                                                                + "font-size: 20pt; "
                                                                + "color:red;";
                // Black "beat":
                this.title_text_2 = new St.Label();
                this.title_text_2.style = "font-family: 'FreeSans'; "
                                                                + "font-weight: bold; "
                                                                + "font-size: 20pt; "
                                                                + "color:black;";
                // Display of the .beats:
                this.netbeat = new St.Label();
                this.netbeat.style = "font-family: 'FreeSans'; "
                                                        + "font-weight: bold; "
                                                        + "font-size: 15pt;";

                        this.row_title.add(this.title_text_1);
                        this.row_title.add(this.title_text_2);
                        this.row_itime.add(this.netbeat);

                                this.frame.add(this.row_title, {x_fill: false, x_align: St.Align.MIDDLE});
                                this.frame.add(this.row_itime, {x_fill: false, x_align: St.Align.MIDDLE});

                this.title_text_1.set_text(".");
                this.title_text_2.set_text("beat");

                this.setContent(this.frame);

    },

        on_desklet_removed: function() {

                Mainloop.source_remove(this.timeout);
        },

        refresh: function() {
                let d = new Date();
                let h = d.getUTCHours() + 1;
                let m = d.getUTCMinutes();
                let s = d.getUTCSeconds();
                //let tzoff = 60 + d.getTimezoneOffset();
                //let beats = ('000' + Math.floor((s + (m + tzoff) * 60 + h * 3600) / 86.4) % 1000).slice(-3);

        // calculation fix
        let beatSecs = (s + m * 60 + h * 3600);
        let netBeats = beatSecs / 86.4;
        let beats = ("000" + parseInt(netBeats)).slice(-3);

                this.netbeat.set_text("@" + beats);
                // only update, if the desklet is running
                return this.keepUpdating;
        },

        on_desklet_clicked: function() {

        },

        on_desklet_removed: function() {
                // if the desklet is removed, stop updating, stop Mainloop
                this.keepUpdating = false;
                if (this.timeout) Mainloop.source_remove(this.timeout);
                this.timeout = 0;
        },

};

function main(metadata, desklet_id) {
    return new NetBeatDesklet(metadata, desklet_id);
}
        with: {
    "uuid": "InternetTimeDesklet@stefan", 
    "name": "Internet Time",
    "description": "Shows the Internet Time in .beat", 
    "prevent-decorations": false
}
          sarif_file: {"author": "ramazanovnursultan"}
