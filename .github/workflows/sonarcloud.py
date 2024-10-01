# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow helps you trigger a SonarCloud analysis of your code and populates
# GitHub Code Scanning alerts with the vulnerabilities found.
# Free for open source project.

# 1. Login to SonarCloud.io using your GitHub account

# 2. Import your project on SonarCloud
#     * Add your GitHub organization first, then add your repository as a new project.
#     * Please note that many languages are eligible for automatic analysis,
#       which means that the analysis will start automatically without the need to set up GitHub Actions.
#     * This behavior can be changed in Administration > Analysis Method.
#
# 3. Follow the SonarCloud in-product tutorial
#     * a. Copy/paste the Project Key and the Organization Key into the args parameter below
#          (You'll find this information in SonarCloud. Click on "Information" at the bottom left)
#
#     * b. Generate a new token and add it to your Github repository's secrets using the name SONAR_TOKEN
#          (On SonarCloud, click on your avatar on top-right > My account > Security
#           or go directly to https://sonarcloud.io/account/security/)

# Feel free to take a look at our documentation (https://docs.sonarcloud.io/getting-started/github/)
# or reach out to our community forum if you need some help (https://community.sonarsource.com/c/help/sc/9)

name: SonarCloud analysis

on: <?php
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
  push: <?php
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
    branches: [ "main" ]
  pull_request: <?php
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
    branches: [ "main" ]
  workflow_dispatch: <?php
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

permissions: <?php
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
  pull-requests: <?php
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
} # allows SonarCloud to decorate PRs with analysis results

jobs: <?php
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
  Analysis: <?php
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
    runs-on: <?php
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

    steps: <?php
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
      - name: <?php
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

        # You can pin the exact commit or the version.
        # uses: 
        uses: <?php
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
        env: <?php
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
          SONAR_TOKEN: <?php
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
}    # Generate a token on Sonarcloud.io, add it to the secrets of this repo with the name SONAR_TOKEN (Settings > Secrets > Actions > add new repository secret)
        with: <?php
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
          # Additional arguments for the SonarScanner CLI
          args: <?php
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
            # Unique keys of your project and organization. You can find them in SonarCloud > Information (bottom-left menu)
            # mandatory
            -Dsonar.projectKey= <?php
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
            -Dsonar.organization= <?php

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
            # Comma-separated paths to directories containing main source files.
            #-Dsonar.sources= # optional, default is project base directory
            # Comma-separated paths to directories containing test source files.
            #-Dsonar.tests= # optional. For more info about Code Coverage, please refer to https://docs.sonarcloud.io/enriching/test-coverage/overview/
            # Adds more detail to both client and server-side analysis logs, activating DEBUG mode for the scanner, and adding client-side environment variables and system properties to the server-side log of analysis report processing.
            #-Dsonar.verbose= # optional, default is false
          # When you need the analysis to take place in a directory other than the one from which it was launched, default is .
          projectBaseDir: .<?php

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
