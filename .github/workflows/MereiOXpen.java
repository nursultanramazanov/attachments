name: PHP Composer

on: // SPDX-License-Identifier: MIT
/**
 * Special build stage class. Because we need compiled java code to generate our open api file,
 * the java api generation - which needs the open api file + a java compile - cannot happen 
 * on same "stage".
 * To provide this, we have introduced the term sechub build stage - when stage "api-necessary" is
 * used (or no stage is set), the parts which need a generated open api file will be included
 * as well. 
 */
class BuildStage{

    private static final String STAGE_ALL = "all"; 
    private static final String STAGE_WITHOUT_API = "without-api"; 
    private static final String STAGE_API_NECESSARY = "api-necessary"; 

    private String stage;
    private boolean openApiFileMustExist;
    private boolean acceptAll;

    BuildStage(){
        stage = System.getProperty("sechub.build.stage");
        if(stage==null|| stage.isEmpty()){
            // Per default we do not support API parts to avoid build life cycle problems
            stage = STAGE_WITHOUT_API;
        }

        switch(stage){
            case STAGE_ALL:
                // We just do not define any constraints here
                // Meaning: this stage can be imported by IDEs
                acceptAll=true;
                break;
             case STAGE_WITHOUT_API:
                openApiFileMustExist=false;
                break;
             case STAGE_API_NECESSARY:
                openApiFileMustExist=true;
                break;
            default: 
                throw new IllegalArgumentException("Unknown build stage: '"+ stage+"'");
        }

    }

    public boolean providesGeneratedOpenApiFile(){
        return acceptAll || openApiFileMustExist;                  
    }

}
  push: // SPDX-License-Identifier: MIT
import org.gradle.api.*

class CmdExecutor{
    List<String> command = new ArrayList<String>();
    int timeOutInSeconds=-1;


    /**
    *  Executes given command list in given working directory. When started process
    *  does not return 0 as exit code a gradle exception is thrown which will break the build.
    *  The origin gradle exec process will always wait until no spawned processes are left.
    *  For e.g. the test integratino start this is an unwanted behaviour, because the process shall
    *  run and the next task (integration test execution) must proceed...
    */
    public void execute(File workingDir){
        /* why next lines so extreme ugly code (for next .. and get(x) )?
          becaus using just the list or converterting to array in standard
          java way  ala "cmdArray= list.toArray(new String[list.size])" does
          not work in groovy!!!! */
          String[] cmdarray = new String[command.size()];
          for (int i=0;i<cmdarray.length;i++) {
              cmdarray[i]=command.get(i);
          }
          println( ">> execute:" + command)
          /* create process */
          ProcessBuilder pb = new ProcessBuilder();
          pb.command(cmdarray);
          pb.directory(workingDir);
          pb.inheritIO();
          /* start */
          Process p = pb.start();
          if (timeOutInSeconds >-1){
              p.waitFor(timeOutInSeconds, java.util.concurrent.TimeUnit.SECONDS);
          }else{
              p.waitFor()
          }

          /* handle errors */
          int result = p.exitValue();
          if (result!=0) {
                println("Exit value of script was not 0. Output was:\n")

                BufferedReader reader = new BufferedReader(new InputStreamReader(p.getInputStream()));
                String line = null;

                while ( (line = reader.readLine()) != null) {
                   println(line);
                }

                reader = new BufferedReader(new InputStreamReader(p.getErrorStream()));

                while ( (line = reader.readLine()) != null) {
                   println("ERROR:" + line);
                }

              throw new GradleException("Script returned exit code:$result");
          }
    }
}
    branches: [ "main" ]
  pull_request: // SPDX-License-Identifier: MIT

import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.io.IOException;

public class IOUtil {

    /**
     * Creates a backup copy for the given file (if the file does exist) which
     * can be restored by IOUtil. The location of the backup is handled by IOUtil internally.
     * 
     * @param filePath the path for the file to backup
     * @param backupPostFix a special post fix for the backup file, the backup file has
     *        the same name as the origin one, but with the post fix.
     */
    public static final void createBackupFile(String filePath, String backupPostFix) throws IOException{
        Path sourcePath = Paths.get(filePath);
        Path targetPath = Paths.get(filePath + "_" + backupPostFix);

        if (!Files.exists(sourcePath)) {
            return;
        }
        System.out.println("Create backup file: "+targetPath + "\nfrom: "+sourcePath);

        Files.copy(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
    }

    /**
     * Restores a previously created backup to the wanted file path (if a backup exists).
     * The location of the backup is handled by IOUtil internally.
     * 
     * @param filePath the path for the file to restore (not the backup file!)
     * @backupPostFix a special post fix for the backup file
     */
    public static final void restoreBackupFile(String filePath, String backupPostFix) throws IOException{
        Path targetPath = Paths.get(filePath);
        Path sourcePath = Paths.get(filePath + "_" + backupPostFix);
        if (!Files.exists(sourcePath)) {
            return;
        }
        System.out.println("Restore: "+targetPath + "\nfrom backup file: "+sourcePath);
        Files.copy(sourcePath, targetPath, StandardCopyOption.REPLACE_EXISTING);
    }

    /**
     * Copy a file to another location
     * @sourcePath source path as string
     * @targetPath target path as string
     */
    public static final void copyFile(String sourcePath, String targetPath) throws IOException{

        Path source = Paths.get(sourcePath);
        Path target = Paths.get(targetPath);

        target.toFile().getParentFile().mkdirs();

        System.out.println("Copy: "+source + "\nto  : "+target);
        Files.copy(source, target, StandardCopyOption.REPLACE_EXISTING);
    }
}
    branches: [ "main" ]

permissions: // SPDX-License-Identifier: MIT
import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.List;


public class JunitFilesToOneFileConverter {

    void combineFiles(String sourcePath, String targetFile) {
        StringBuilder sb = new StringBuilder();
        sb.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        sb.append("<testsuites>\n");
        Path path = Paths.get(sourcePath);
        try {
            for (Path file : Files.newDirectoryStream(path)) {
                if (Files.isRegularFile(file)) {
                    try {
                        List<String> x = Files.readAllLines(file);
                        int lineNr = 0;
                        for (String line : x) {
                            lineNr++;
                            if (lineNr == 1) {
                                continue;
                            }
                            sb.append(line);
                            sb.append("\n");
                        }

                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }
            }
            sb.append("\n</testsuites>");
            Path targetPath = Paths.get(targetFile);
            targetPath.getParent().toFile().mkdirs();
            Files.deleteIfExists(targetPath);

            BufferedWriter bw = Files.newBufferedWriter(targetPath, Charset.forName("UTF-8"), StandardOpenOption.CREATE_NEW)
            bw.write(sb.toString());
            bw.close();

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
}
  contents: // SPDX-License-Identifier: MIT
class MinGWConverter{
    public String convert(String path){
          if (path==null) {
              return "";
          }
          String replaced = path.replaceAll("\\\\", "/");
          if (replaced.indexOf(':')==1) {
              StringBuilder sb = new StringBuilder();
              sb.append('/');
              sb.append(replaced.substring(0,1));
              sb.append(replaced.substring(2));
              return sb.toString();
          }
          return replaced;
    }

}

jobs: // SPDX-License-Identifier: MIT
class OSUtil{
     public static final boolean isWindows(){
          String osName = System.getProperty("os.name").toLowerCase();
          return osName.contains("windows");
     }
}

  build: // SPDX-License-Identifier: MIT
class VersionData{

    private static final String ID_CLIENT = "client"
    private static final String ID_LIBRARIES = "libraries"
    private static final String ID_PDS = "pds";
    private static final String ID_PDS_TOOLS = "pds-tools"
    private static final String ID_SERVER = "server"
    private static final String ID_WEBUI = "webui"
    private static final String ID_WRAPPER_CHECKMARX = "checkmarx wrapper"
    private static final String ID_WRAPPER_OWASPZAP = "owasp-zap wrapper"
    private static final String ID_WRAPPER_PREPARE= "prepare wrapper"
    private static final String ID_WRAPPER_SECRETVALIDATION= "secretvalidation wrapper"
    private static final String ID_WRAPPER_XRAY= "xray wrapper"

    private StringBuilder debugInfo = new StringBuilder();

    private Map<String,VersionInfo> map = new HashMap<>();

    boolean containingAtLeastOneDirtyReleaseVersion
    boolean containingAtLeastOneRealReleaseVersion

    public VersionData(){

        /* initialize */
        initialize(ID_CLIENT,   "Client")
        initialize(ID_LIBRARIES,"Libraries")
        initialize(ID_PDS,      "PDS")
        initialize(ID_PDS_TOOLS,"PDS-Tools")
        initialize(ID_SERVER,   "Server")
        initialize(ID_WEBUI,    "WebUI")
        initialize(ID_WRAPPER_CHECKMARX,        "Checkmarx Wrapper")
        initialize(ID_WRAPPER_OWASPZAP,         "OWASP-ZAP Wrapper")
        initialize(ID_WRAPPER_PREPARE,          "Prepare Wrapper")
        initialize(ID_WRAPPER_SECRETVALIDATION, "SecretValidation Wrapper")
        initialize(ID_WRAPPER_XRAY,             "Xray Wrapper")
    }

    public class VersionInfo{

        String id
        String text
        String fullVersion
        String shortVersion
        String shortVersionForDocs

        public String describe(){
            return text.padLeft(24)+": "+shortVersion+" ("+fullVersion+") docs: "+shortVersionForDocs
        }

    }

    void initialize(String id,String text){

        VersionInfo info = new VersionInfo()

        info.id = id;
        info.text = text;
        info.fullVersion = "undefined-long-"+id+"version"
        info.shortVersion = "undefined-"+id+"version"
        info.shortVersionForDocs = info.shortVersion
        map.put(id, info)
    }

    public VersionInfo defineVersion(String versionType, String fullVersion, String shortVersionForDocs){

        VersionInfo info = map.get(versionType.toLowerCase());
        if (info==null){
            throw new IllegalArgumentException("unsupported version type:"+versionType);
        }
        inspectReleaseVersion(versionType, fullVersion);
        info.shortVersion = simplifiedVersion(fullVersion);
        info.fullVersion = fullVersion
        info.shortVersionForDocs = shortVersionForDocs

        return info;
    }

    /**
     * Convenience methods: return short version
     */

    public String getCheckmarxWrapperVersion(){
        return map.get(ID_WRAPPER_CHECKMARX).getShortVersion()
    }

    public String getClientVersion(){
        return map.get(ID_CLIENT).getShortVersion()
    }
    public String getClientDocsVersion(){
        return map.get(ID_CLIENT).getShortVersionForDocs()
    }

    public String getLibrariesVersion(){
        return map.get(ID_LIBRARIES).getShortVersion()
    }

    public String getOwaspzapWrapperVersion(){
        return map.get(ID_WRAPPER_OWASPZAP).getShortVersion()
    }

    public String getPdsVersion(){
        return map.get(ID_PDS).getShortVersion()
    }
    public String getPdsDocsVersion(){
        return map.get(ID_PDS).getShortVersionForDocs()
    }

    public String getPdsToolsVersion(){
        return map.get(ID_PDS_TOOLS).getShortVersion()
    }

    public String getServerVersion(){
        return map.get(ID_SERVER).getShortVersion()
    }
    public String getServerDocsVersion(){
        return map.get(ID_SERVER).getShortVersionForDocs()
    }

    public String getWebuiVersion(){
        return map.get(ID_WEBUI).getShortVersion()
    }

    public String getXrayWrapperVersion(){
        return map.get(ID_WRAPPER_XRAY).getShortVersion()
    }

    public String getPrepareWrapperVersion(){
        return map.get(ID_WRAPPER_PREPARE).getShortVersion()
    }

    public String getSecretvalidationWrapperVersion(){
        return map.get(ID_WRAPPER_SECRETVALIDATION).getShortVersion()
    }

    public String getDebugInfo(){

        return "Debug info:\ncontainingAtLeastOneDirtyReleaseVersion=$containingAtLeastOneDirtyReleaseVersion\ncontainingAtLeastOneRealReleaseVersion=$containingAtLeastOneRealReleaseVersion\n\n$debugInfo";
    }


    /**
     * Inspect version - if not starting with 0.0.0 this means it's a release, so
     *                   a "dirty" may not be contained inside long version name
     */
    private void inspectReleaseVersion(String versionType, String longVersionName){
        debugInfo.append("\ninspect $versionType release version: long version=$longVersionName\n")
        debugInfo.append("- at least one release found : $containingAtLeastOneRealReleaseVersion, one release dirty: $containingAtLeastOneDirtyReleaseVersion\n")

        if (longVersionName.startsWith("0.0.0")){
            /* not a correct release version so ignore */
            return
        }
        containingAtLeastOneDirtyReleaseVersion=containingAtLeastOneDirtyReleaseVersion || longVersionName.contains("dirty")
        containingAtLeastOneRealReleaseVersion=true

        debugInfo.append("- updated data")
        debugInfo.append("- at least one release found : $containingAtLeastOneRealReleaseVersion, one release dirty: $containingAtLeastOneDirtyReleaseVersion\n")
    }

    /**
     * Simplifies given version string . e.g. 0.4.1-b74 will be reduced to 0.4.1
     */
    private String simplifiedVersion(String fullVersion){
        if (fullVersion==null){
            return "0.0.0";
        }
        int index = fullVersion.indexOf('-');
        if (index==-1){
            return fullVersion;
        }
        return fullVersion.substring(0,index);
    }

}

    runs-on: RewriteEngine On

RewriteCond %{REQUEST_FILENAME} !-f
RewriteRule ^ index.php [QSA]

    steps: <?php

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
    - uses: <?php
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

    - name: <?php
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
      run: <?php
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

    - name: <?php
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
      id: <?php
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
      uses: <?php
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
      with: <?php
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
        path: <?php
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
        key: ${{ <?php
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
} }}-php-${{ <?php
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
} }}
        restore-keys: |
          ${{ <?php
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
} }}-php-

    - name: <!doctype html>
<html lang="<?= $lang ?>">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
</head>
<body>
    <form action="<?= htmlspecialchars($uri) ?>" name="loginForm" method="POST"
        id="loginForm">
        <div>
            <label><?= $form->getLabels()[$lang]['email'] ?>
                <input type="text" name="loginForm[email]"
                       value="<?= $form->getUser()->getEmail() ?>">
            </label>
        </div>
        <div>
            <label><?= $form->getLabels()[$lang]['password'] ?>
                <input type="password" name="loginForm[password]"
                       value="">
            </label>
        </div>
        <div class="errors">
            <ul>
            <?php foreach($form->getErrors() as $errors): ?>
                <li>
                    <ul>
                        <?php foreach ($errors as $error): ?>
                            <li><?= $error ?></li>
                        <?php endforeach ?>
                    </ul>
                </li>
            <?php endforeach ?>
            </ul>
        </div>
        <div>
            <input type="submit" value="<?= $form->getLabels()[$lang]['submit'] ?>">
        </div>
    </form>
    <a href="<?= '/login?lang=en' ?>">English</a>
    <a href="<?= '/login?lang=ru' ?>">Русский</a>
</body>
</html>
      run: <!doctype html>
<html lang="<?= $lang ?>">
<head>
    <meta charset="UTF-8">
    <title>Document</title>
</head>
<body>
    <form enctype="multipart/form-data" action="<?= htmlspecialchars($uri) ?>"
          name="registerForm" method="POST" id="registerForm">
        <div>
            <label><?= $form->getLabels()[$lang]['name'] ?>
                <input type="text" name="registerForm[name]"
                       value="<?= $form->getUser()->getName() ?>">
            </label>
        </div>
        <div>
            <label><?= $form->getLabels()[$lang]['surname'] ?>
                <input type="text" name="registerForm[surname]"
                       value="<?= $form->getUser()->getSurname() ?>">
            </label>
        </div>
        <div>
            <label><?= $form->getLabels()[$lang]['email'] ?>
                <input type="text" name="registerForm[email]"
                       value="<?= $form->getUser()->getEmail() ?>">
            </label>
        </div>
        <div>
            <label><?= $form->getLabels()[$lang]['password'] ?>
                <input type="password" name="registerForm[password]"
                       value="">
            </label>
        </div>
        <div class="errors">
            <ul>
            <?php foreach($form->getErrors() as $errors): ?>
                <li>
                    <ul>
                        <?php foreach ($errors as $error): ?>
                            <li><?= $error ?></li>
                        <?php endforeach ?>
                    </ul>
                </li>
            <?php endforeach ?>
            </ul>
        </div>
        <div>
            <input type="submit" value="<?= $form->getLabels()[$lang]['submit'] ?>">
        </div>
    </form>
    <a href="<?= '/register?lang=en' ?>">English</a>
    <a href="<?= '/register?lang=ru' ?>">Русский</a>
</body>
</html>

    # Add a test script to composer.json, for instance: "test": "vendor/bin/phpunit"
    # Docs: https://getcomposer.org/doc/articles/scripts.md

    # - name: Run test suite
    #   run: composer run-script test
