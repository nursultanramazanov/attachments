on: """
Warning: gibberish code
This tool isn't affiliated to SchoolMouv in any way
"""
import requests
from bs4 import BeautifulSoup
import json
import os
import webbrowser


class resource:
    """Parent class
    Argument : url (str)
    Functions : validate -> bool : validation tests
                download -> None : download 'url' into 'into' (folder) as 'save_as' (filename, default:None)
                relevant_filename -> str : suggests a nice formatted filename
    """

    def __init__(self, url: str):
        self.url = url.split("#")[0].split("?")[0]

    def validate(self, url: str):
        # no to be used for security improvement
        pass

    def download(self, url: str, into: str, save_as=None, overwrite=False):
        if type(url) != str:
            print(f'URL needs to be a string, not a {type(url).__name__}')
            return False
        if not os.path.exists(into):
            print(
                f'No such folder, consider changing the value passed in the \'into\' parameter')
            return False
        assert self.validate(url)
        _ = requests.get(url)
        assert _.status_code == 200
        save_as = self.relevant_filename(url) if save_as == None else save_as
        abs_path = os.path.abspath(os.path.join(into, save_as))
        if os.path.exists(abs_path) and not overwrite:
            print("File already exists, set overwrite to True to overwrite it anyway")
            return False
        with open(abs_path, 'wb') as file_to_write:
            file_to_write.write(_.content)

    def relevant_filename(self, url: str):
        _ = ''
        __ = url.split('/')[-2].capitalize().replace('-', ' ')
        already_done = 0
        for caract in __:
            if already_done == 0 and caract.isnumeric():
                _ += ' '+caract
                already_done += 1
            else:
                _ += caract
        _ = _.replace('  ', ' ')
        return _+'.pdf' if url.endswith('.pdf') else _+'.mp4'

    def see(self, url: str):
        success = webbrowser.open_new_tab(url)
        if not success:
            return success


class video(resource):
    """
    Argument : url (str), for eg: https://www.schoolmouv.fr/cours/little-red-riding-hood/cours-video
    Functions : extract_json
                get_source
                get_direct_links
                get_soup
                ----------------
                validate
                download
    """

    def __init__(self, url: str):
        super().__init__(url)
        self.url = url
        self.useful_headers = {
            'Accept': '*/*',
            'Accept-Encoding': 'gzip, deflate, br',
            'Accept-Language': 'en-US,en;q=0.9,zh-CN;q=0.6,zh;q=0.5',
            'DNT': '1',
            'Referer': self.url,
            'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4130.0 Safari/537.36'
        }
        self.basic_headers = {
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4130.0 Safari/537.36 Edg/84.0.502.0'
        }

    def get_soup(self, url, headers={}):
        headers = self.basic_headers if headers == {} else headers
        for _ in range(5):
            po = requests.get(url, headers=headers)
            if po.status_code == 200:
                break
        return BeautifulSoup(po.content, 'html.parser')

    def extract_json(self, mess_up):
        mess_up = str(mess_up)
        mess_up = mess_up.split('};')[0]+'}'
        try:
            mess_up = '{'+mess_up.split('= {')[1]
        except IndexError:
            mess_up = '{'+mess_up.split('={')[1]
        mess_up = json.loads(mess_up)
        return mess_up

    def get_source(self, mess_up):
        assert 'sheet' in mess_up.keys()
        _r = []
        for _ in mess_up['sheet']['resources']:
            _source = mess_up['sheet']['resources'][_]['source']
            _r.append(_source)
        return list(set(_r))

    def get_direct_links(self, json_):
        # print(json_)
        assert 'request' in json_.keys()
        return list(set([i['url'] for i in json_['request']['files']['progressive']]))
        #keys : ['profile', 'width', 'mime', 'fps', 'url', 'cdn', 'quality', 'id', 'origin', 'height']

    def run(self):
        # 1st part
        # GET https://www.schoolmouv.fr/cours/little-red-riding-hood/cours-video
        soup = self.get_soup(self.url)
        script_tag_containing_json = [
            i for i in soup.find_all('script') if str(i).count("{") > 9]
        #assert len(script_tag_containing_json)==1
        # parsing json from <script> tag
        self.script_tag_containing_json = self.extract_json(
            script_tag_containing_json[0])
        # extract unique 9-digit id, different for each video
        sources = self.get_source(self.script_tag_containing_json)

        # 2nd part
        for _source in sources:
            assert len(_source) == 9 and _source.isalnum()
            # GET https://player.vimeo.com/video/9-DIGIT_ID?app_id=schoolmouv_app_id
            soup__ = self.get_soup(
                f'https://player.vimeo.com/video/{_source}', headers=self.useful_headers)  # ?app_id=122963
            script_tag_containing_mp4 = [
                i for i in soup__.find_all('script') if '.mp4' in str(i)]
            #assert len(script_tag_containing_mp4_content) == 1
            self.script_tag_containing_mp4 = self.extract_json(
                script_tag_containing_mp4)
            self.result = self.get_direct_links(self.script_tag_containing_mp4)

    def validate(self, url: str) -> bool:
        return url.endswith('.mp4') and url.startswith('https://vod-progressive.akamaized.net')


class pdf(resource):
    """
    Argument : url (str), for eg: https://www.schoolmouv.fr/cours/little-red-riding-hood/fiche-de-cours
    Functions : run
                download
    """

    def __init__(self, url):
        super().__init__(url)
        self.valids = [
            'scientifique',
            'mouvement-litteraire',
            'schema-bilan',
            'fiche-methode-bac',
            'fiche-de-revision',
            'demonstration',
            'repere',
            'personnages-historique',
            'lecon',
            'fiche-materiel',
            'evenement-historique',
            'savoir-faire',
            'fiche-methode',
            'bien-rediger',
            'fiche-pratique',
            'auteur',
            'philosophe',
            'formule-ses',
            'figure-de-style',
            'fiche-annale',
            'definition',
            'algorithme',
            'fiche-calculatrice',
            'courant-philosophique',
            'fiche-methode-brevet',
            'fiche-de-cours',
            'genre-litteraire',
            'registre-litteraire',
            'carte',
            'fiche-de-lecture',
            'fiche-oeuvre',
            'notion'
        ]
        self.url = url

    def run(self):
        if len([i for i in self.valids if i in self.url]) > 0:
            if self.url.startswith("https://www.schoolmouv.fr/"):
                TO_BE_REPLACED = "www.schoolmouv.fr/eleves" if "/eleves/" in self.url else "www.schoolmouv.fr"
                self.result = f"{self.url.replace(TO_BE_REPLACED,'pdf-schoolmouv.s3.eu-west-1.amazonaws.com')}.pdf"

    def validate(self, url: str) -> bool:
        return url.endswith('.pdf') and url.startswith('https://pdf-schoolmouv.s3.eu-west-1.amazonaws.com/')   
push: <?php

/*
 * This file is part of the Symfony package.
 *
 * (c)
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

/*
 * Users of PHP 5.2 should be able to run the requirements checks.
 * This is why the file and all classes must be compatible with PHP 5.2+
 * (e.g. not using namespaces and closures).
 *
 * ************** CAUTION **************
 *
 * DO NOT EDIT THIS FILE as it will be overridden by Composer as part of
 * the installation/update process. The original file resides in the
 * SensioDistributionBundle.
 *
 * ************** CAUTION **************
 */

/**
 * Represents a single PHP requirement, e.g. an installed extension.
 * It can be a mandatory requirement or an optional recommendation.
 * There is a special subclass, named PhpIniRequirement, to check a php.ini configuration.
 *
 *
 */
class Requirement
{
    private $fulfilled;
    private $testMessage;
    private $helpText;
    private $helpHtml;
    private $optional;

    /**
     * Constructor that initializes the requirement.
     *
     * @param bool        $fulfilled   Whether the requirement is fulfilled
     * @param string      $testMessage The message for testing the requirement
     * @param string      $helpHtml    The help text formatted in HTML for resolving the problem
     * @param string|null $helpText    The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     * @param bool        $optional    Whether this is only an optional recommendation not a mandatory requirement
     */
    public function __construct($fulfilled, $testMessage, $helpHtml, $helpText = null, $optional = false)
    {
        $this->fulfilled = (bool) $fulfilled;
        $this->testMessage = (string) $testMessage;
        $this->helpHtml = (string) $helpHtml;
        $this->helpText = null === $helpText ? strip_tags($this->helpHtml) : (string) $helpText;
        $this->optional = (bool) $optional;
    }

    /**
     * Returns whether the requirement is fulfilled.
     *
     * @return bool true if fulfilled, otherwise false
     */
    public function isFulfilled()
    {
        return $this->fulfilled;
    }

    /**
     * Returns the message for testing the requirement.
     *
     * @return string The test message
     */
    public function getTestMessage()
    {
        return $this->testMessage;
    }

    /**
     * Returns the help text for resolving the problem.
     *
     * @return string The help text
     */
    public function getHelpText()
    {
        return $this->helpText;
    }

    /**
     * Returns the help text formatted in HTML.
     *
     * @return string The HTML help
     */
    public function getHelpHtml()
    {
        return $this->helpHtml;
    }

    /**
     * Returns whether this is only an optional recommendation and not a mandatory requirement.
     *
     * @return bool true if optional, false if mandatory
     */
    public function isOptional()
    {
        return $this->optional;
    }
}

/**
 * Represents a PHP requirement in form of a php.ini configuration.
 *
 * 
 */
class PhpIniRequirement extends Requirement
{
    /**
     * Constructor that initializes the requirement.
     *
     * @param string        $cfgName           The configuration name used for ini_get()
     * @param bool|callback $evaluation        Either a boolean indicating whether the configuration should evaluate to true or false,
     *                                         or a callback function receiving the configuration value as parameter to determine the fulfillment of the requirement
     * @param bool          $approveCfgAbsence If true the Requirement will be fulfilled even if the configuration option does not exist, i.e. ini_get() returns false.
     *                                         This is helpful for abandoned configs in later PHP versions or configs of an optional extension, like Suhosin.
     *                                         Example: You require a config to be true but PHP later removes this config and defaults it to true internally.
     * @param string|null   $testMessage       The message for testing the requirement (when null and $evaluation is a boolean a default message is derived)
     * @param string|null   $helpHtml          The help text formatted in HTML for resolving the problem (when null and $evaluation is a boolean a default help is derived)
     * @param string|null   $helpText          The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     * @param bool          $optional          Whether this is only an optional recommendation not a mandatory requirement
     */
    public function __construct($cfgName, $evaluation, $approveCfgAbsence = false, $testMessage = null, $helpHtml = null, $helpText = null, $optional = false)
    {
        $cfgValue = ini_get($cfgName);

        if (is_callable($evaluation)) {
            if (null === $testMessage || null === $helpHtml) {
                throw new InvalidArgumentException('You must provide the parameters testMessage and helpHtml for a callback evaluation.');
            }

            $fulfilled = call_user_func($evaluation, $cfgValue);
        } else {
            if (null === $testMessage) {
                $testMessage = sprintf('%s %s be %s in php.ini',
                    $cfgName,
                    $optional ? 'should' : 'must',
                    $evaluation ? 'enabled' : 'disabled'
                );
            }

            if (null === $helpHtml) {
                $helpHtml = sprintf('Set <strong>%s</strong> to <strong>%s</strong> in php.ini<a href="#phpini">*</a>.',
                    $cfgName,
                    $evaluation ? 'on' : 'off'
                );
            }

            $fulfilled = $evaluation == $cfgValue;
        }

        parent::__construct($fulfilled || ($approveCfgAbsence && false === $cfgValue), $testMessage, $helpHtml, $helpText, $optional);
    }
}

/**
 * A RequirementCollection represents a set of Requirement instances.
 *
 * @author Tobias Schultze <http://tobion.de>
 */
class RequirementCollection implements IteratorAggregate
{
    private $requirements = array();

    /**
     * Gets the current RequirementCollection as an Iterator.
     *
     * @return Traversable A Traversable interface
     */
    public function getIterator()
    {
        return new ArrayIterator($this->requirements);
    }

    /**
     * Adds a Requirement.
     *
     * @param Requirement $requirement A Requirement instance
     */
    public function add(Requirement $requirement)
    {
        $this->requirements[] = $requirement;
    }

    /**
     * Adds a mandatory requirement.
     *
     * @param bool        $fulfilled   Whether the requirement is fulfilled
     * @param string      $testMessage The message for testing the requirement
     * @param string      $helpHtml    The help text formatted in HTML for resolving the problem
     * @param string|null $helpText    The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     */
    public function addRequirement($fulfilled, $testMessage, $helpHtml, $helpText = null)
    {
        $this->add(new Requirement($fulfilled, $testMessage, $helpHtml, $helpText, false));
    }

    /**
     * Adds an optional recommendation.
     *
     * @param bool        $fulfilled   Whether the recommendation is fulfilled
     * @param string      $testMessage The message for testing the recommendation
     * @param string      $helpHtml    The help text formatted in HTML for resolving the problem
     * @param string|null $helpText    The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     */
    public function addRecommendation($fulfilled, $testMessage, $helpHtml, $helpText = null)
    {
        $this->add(new Requirement($fulfilled, $testMessage, $helpHtml, $helpText, true));
    }

    /**
     * Adds a mandatory requirement in form of a php.ini configuration.
     *
     * @param string        $cfgName           The configuration name used for ini_get()
     * @param bool|callback $evaluation        Either a boolean indicating whether the configuration should evaluate to true or false,
     *                                         or a callback function receiving the configuration value as parameter to determine the fulfillment of the requirement
     * @param bool          $approveCfgAbsence If true the Requirement will be fulfilled even if the configuration option does not exist, i.e. ini_get() returns false.
     *                                         This is helpful for abandoned configs in later PHP versions or configs of an optional extension, like Suhosin.
     *                                         Example: You require a config to be true but PHP later removes this config and defaults it to true internally.
     * @param string        $testMessage       The message for testing the requirement (when null and $evaluation is a boolean a default message is derived)
     * @param string        $helpHtml          The help text formatted in HTML for resolving the problem (when null and $evaluation is a boolean a default help is derived)
     * @param string|null   $helpText          The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     */
    public function addPhpIniRequirement($cfgName, $evaluation, $approveCfgAbsence = false, $testMessage = null, $helpHtml = null, $helpText = null)
    {
        $this->add(new PhpIniRequirement($cfgName, $evaluation, $approveCfgAbsence, $testMessage, $helpHtml, $helpText, false));
    }

    /**
     * Adds an optional recommendation in form of a php.ini configuration.
     *
     * @param string        $cfgName           The configuration name used for ini_get()
     * @param bool|callback $evaluation        Either a boolean indicating whether the configuration should evaluate to true or false,
     *                                         or a callback function receiving the configuration value as parameter to determine the fulfillment of the requirement
     * @param bool          $approveCfgAbsence If true the Requirement will be fulfilled even if the configuration option does not exist, i.e. ini_get() returns false.
     *                                         This is helpful for abandoned configs in later PHP versions or configs of an optional extension, like Suhosin.
     *                                         Example: You require a config to be true but PHP later removes this config and defaults it to true internally.
     * @param string        $testMessage       The message for testing the requirement (when null and $evaluation is a boolean a default message is derived)
     * @param string        $helpHtml          The help text formatted in HTML for resolving the problem (when null and $evaluation is a boolean a default help is derived)
     * @param string|null   $helpText          The help text (when null, it will be inferred from $helpHtml, i.e. stripped from HTML tags)
     */
    public function addPhpIniRecommendation($cfgName, $evaluation, $approveCfgAbsence = false, $testMessage = null, $helpHtml = null, $helpText = null)
    {
        $this->add(new PhpIniRequirement($cfgName, $evaluation, $approveCfgAbsence, $testMessage, $helpHtml, $helpText, true));
    }

    /**
     * Adds a requirement collection to the current set of requirements.
     *
     * @param RequirementCollection $collection A RequirementCollection instance
     */
    public function addCollection(RequirementCollection $collection)
    {
        $this->requirements = array_merge($this->requirements, $collection->all());
    }

    /**
     * Returns both requirements and recommendations.
     *
     * @return array Array of Requirement instances
     */
    public function all()
    {
        return $this->requirements;
    }

    /**
     * Returns all mandatory requirements.
     *
     * @return array Array of Requirement instances
     */
    public function getRequirements()
    {
        $array = array();
        foreach ($this->requirements as $req) {
            if (!$req->isOptional()) {
                $array[] = $req;
            }
        }

        return $array;
    }

    /**
     * Returns the mandatory requirements that were not met.
     *
     * @return array Array of Requirement instances
     */
    public function getFailedRequirements()
    {
        $array = array();
        foreach ($this->requirements as $req) {
            if (!$req->isFulfilled() && !$req->isOptional()) {
                $array[] = $req;
            }
        }

        return $array;
    }

    /**
     * Returns all optional recommendations.
     *
     * @return array Array of Requirement instances
     */
    public function getRecommendations()
    {
        $array = array();
        foreach ($this->requirements as $req) {
            if ($req->isOptional()) {
                $array[] = $req;
            }
        }

        return $array;
    }

    /**
     * Returns the recommendations that were not met.
     *
     * @return array Array of Requirement instances
     */
    public function getFailedRecommendations()
    {
        $array = array();
        foreach ($this->requirements as $req) {
            if (!$req->isFulfilled() && $req->isOptional()) {
                $array[] = $req;
            }
        }

        return $array;
    }

    /**
     * Returns whether a php.ini configuration is not correct.
     *
     * @return bool php.ini configuration problem?
     */
    public function hasPhpIniConfigIssue()
    {
        foreach ($this->requirements as $req) {
            if (!$req->isFulfilled() && $req instanceof PhpIniRequirement) {
                return true;
            }
        }

        return false;
    }

    /**
     * Returns the PHP configuration file (php.ini) path.
     *
     * @return string|false php.ini file path
     */
    public function getPhpIniConfigPath()
    {
        return get_cfg_var('cfg_file_path');
    }
}

/**
 * This class specifies all requirements and optional recommendations that
 * are necessary to run the Symfony Standard Edition.
 *
 * @author Tobias Schultze <http://tobion.de>
 * @author Fabien Potencier <fabien@symfony.com>
 */
class SymfonyRequirements extends RequirementCollection
{
    const REQUIRED_PHP_VERSION = '5.3.3';

    /**
     * Constructor that initializes the requirements.
     */
    public function __construct()
    {
        /* mandatory requirements follow */

        $installedPhpVersion = phpversion();

        $this->addRequirement(
            version_compare($installedPhpVersion, self::REQUIRED_PHP_VERSION, '>='),
            sprintf('PHP version must be at least %s (%s installed)', self::REQUIRED_PHP_VERSION, $installedPhpVersion),
            sprintf('You are running PHP version "<strong>%s</strong>", but Symfony needs at least PHP "<strong>%s</strong>" to run.
                Before using Symfony, upgrade your PHP installation, preferably to the latest version.',
                $installedPhpVersion, self::REQUIRED_PHP_VERSION),
            sprintf('Install PHP %s or newer (installed version is %s)', self::REQUIRED_PHP_VERSION, $installedPhpVersion)
        );

        $this->addRequirement(
            version_compare($installedPhpVersion, '5.3.16', '!='),
            'PHP version must not be 5.3.16 as Symfony won\'t work properly with it',
            'Install PHP 5.3.17 or newer (or downgrade to an earlier PHP version)'
        );

        $this->addRequirement(
            is_dir(__DIR__.'/../vendor/composer'),
            'Vendor libraries must be installed',
            'Vendor libraries are missing. Install composer following instructions from <a href="http://getcomposer.org/">http://getcomposer.org/</a>. '.
                'Then run "<strong>php composer.phar install</strong>" to install them.'
        );

        $cacheDir = is_dir(__DIR__.'/../var/cache') ? __DIR__.'/../var/cache' : __DIR__.'/cache';

        $this->addRequirement(
            is_writable($cacheDir),
            'app/cache/ or var/cache/ directory must be writable',
            'Change the permissions of either "<strong>app/cache/</strong>" or  "<strong>var/cache/</strong>" directory so that the web server can write into it.'
        );

        $logsDir = is_dir(__DIR__.'/../var/logs') ? __DIR__.'/../var/logs' : __DIR__.'/logs';

        $this->addRequirement(
            is_writable($logsDir),
            'app/logs/ or var/logs/ directory must be writable',
            'Change the permissions of either "<strong>app/logs/</strong>" or  "<strong>var/logs/</strong>" directory so that the web server can write into it.'
        );

        if (version_compare($installedPhpVersion, '7.0.0', '<')) {
            $this->addPhpIniRequirement(
                'date.timezone', true, false,
                'date.timezone setting must be set',
                'Set the "<strong>date.timezone</strong>" setting in php.ini<a href="#phpini">*</a> (like Europe/Paris).'
            );
        }

        if (version_compare($installedPhpVersion, self::REQUIRED_PHP_VERSION, '>=')) {
            $timezones = array();
            foreach (DateTimeZone::listAbbreviations() as $abbreviations) {
                foreach ($abbreviations as $abbreviation) {
                    $timezones[$abbreviation['timezone_id']] = true;
                }
            }

            $this->addRequirement(
                isset($timezones[@date_default_timezone_get()]),
                sprintf('Configured default timezone "%s" must be supported by your installation of PHP', @date_default_timezone_get()),
                'Your default timezone is not supported by PHP. Check for typos in your <strong>php.ini</strong> file and have a look at the list of deprecated timezones at <a href="http://php.net/manual/en/timezones.others.php">http://php.net/manual/en/timezones.others.php</a>.'
            );
        }

        $this->addRequirement(
            function_exists('iconv'),
            'iconv() must be available',
            'Install and enable the <strong>iconv</strong> extension.'
        );

        $this->addRequirement(
            function_exists('json_encode'),
            'json_encode() must be available',
            'Install and enable the <strong>JSON</strong> extension.'
        );

        $this->addRequirement(
            function_exists('session_start'),
            'session_start() must be available',
            'Install and enable the <strong>session</strong> extension.'
        );

        $this->addRequirement(
            function_exists('ctype_alpha'),
            'ctype_alpha() must be available',
            'Install and enable the <strong>ctype</strong> extension.'
        );

        $this->addRequirement(
            function_exists('token_get_all'),
            'token_get_all() must be available',
            'Install and enable the <strong>Tokenizer</strong> extension.'
        );

        $this->addRequirement(
            function_exists('simplexml_import_dom'),
            'simplexml_import_dom() must be available',
            'Install and enable the <strong>SimpleXML</strong> extension.'
        );

        if (function_exists('apc_store') && ini_get('apc.enabled')) {
            if (version_compare($installedPhpVersion, '5.4.0', '>=')) {
                $this->addRequirement(
                    version_compare(phpversion('apc'), '3.1.13', '>='),
                    'APC version must be at least 3.1.13 when using PHP 5.4',
                    'Upgrade your <strong>APC</strong> extension (3.1.13+).'
                );
            } else {
                $this->addRequirement(
                    version_compare(phpversion('apc'), '3.0.17', '>='),
                    'APC version must be at least 3.0.17',
                    'Upgrade your <strong>APC</strong> extension (3.0.17+).'
                );
            }
        }

        $this->addPhpIniRequirement('detect_unicode', false);

        if (extension_loaded('suhosin')) {
            $this->addPhpIniRequirement(
                'suhosin.executor.include.whitelist',
                create_function('$cfgValue', 'return false !== stripos($cfgValue, "phar");'),
                false,
                'suhosin.executor.include.whitelist must be configured correctly in php.ini',
                'Add "<strong>phar</strong>" to <strong>suhosin.executor.include.whitelist</strong> in php.ini<a href="#phpini">*</a>.'
            );
        }

        if (extension_loaded('xdebug')) {
            $this->addPhpIniRequirement(
                'xdebug.show_exception_trace', false, true
            );

            $this->addPhpIniRequirement(
                'xdebug.scream', false, true
            );

            $this->addPhpIniRecommendation(
                'xdebug.max_nesting_level',
                create_function('$cfgValue', 'return $cfgValue > 100;'),
                true,
                'xdebug.max_nesting_level should be above 100 in php.ini',
                'Set "<strong>xdebug.max_nesting_level</strong>" to e.g. "<strong>250</strong>" in php.ini<a href="#phpini">*</a> to stop Xdebug\'s infinite recursion protection erroneously throwing a fatal error in your project.'
            );
        }

        $pcreVersion = defined('PCRE_VERSION') ? (float) PCRE_VERSION : null;

        $this->addRequirement(
            null !== $pcreVersion,
            'PCRE extension must be available',
            'Install the <strong>PCRE</strong> extension (version 8.0+).'
        );

        if (extension_loaded('mbstring')) {
            $this->addPhpIniRequirement(
                'mbstring.func_overload',
                create_function('$cfgValue', 'return (int) $cfgValue === 0;'),
                true,
                'string functions should not be overloaded',
                'Set "<strong>mbstring.func_overload</strong>" to <strong>0</strong> in php.ini<a href="#phpini">*</a> to disable function overloading by the mbstring extension.'
            );
        }

        /* optional recommendations follow */

        if (file_exists(__DIR__.'/../vendor/composer')) {
            require_once __DIR__.'/../vendor/autoload.php';

            try {
                $r = new ReflectionClass('Sensio\Bundle\DistributionBundle\SensioDistributionBundle');

                $contents = file_get_contents(dirname($r->getFileName()).'/Resources/skeleton/app/SymfonyRequirements.php');
            } catch (ReflectionException $e) {
                $contents = '';
            }
            $this->addRecommendation(
                file_get_contents(__FILE__) === $contents,
                'Requirements file should be up-to-date',
                'Your requirements file is outdated. Run composer install and re-check your configuration.'
            );
        }

        $this->addRecommendation(
            version_compare($installedPhpVersion, '5.3.4', '>='),
            'You should use at least PHP 5.3.4 due to PHP bug #52083 in earlier versions',
            'Your project might malfunction randomly due to PHP bug #52083 ("Notice: Trying to get property of non-object"). Install PHP 5.3.4 or newer.'
        );

        $this->addRecommendation(
            version_compare($installedPhpVersion, '5.3.8', '>='),
            'When using annotations you should have at least PHP 5.3.8 due to PHP bug #55156',
            'Install PHP 5.3.8 or newer if your project uses annotations.'
        );

        $this->addRecommendation(
            version_compare($installedPhpVersion, '5.4.0', '!='),
            'You should not use PHP 5.4.0 due to the PHP bug #61453',
            'Your project might not work properly due to the PHP bug #61453 ("Cannot dump definitions which have method calls"). Install PHP 5.4.1 or newer.'
        );

        $this->addRecommendation(
            version_compare($installedPhpVersion, '5.4.11', '>='),
            'When using the logout handler from the Symfony Security Component, you should have at least PHP 5.4.11 due to PHP bug #63379 (as a workaround, you can also set invalidate_session to false in the security logout handler configuration)',
            'Install PHP 5.4.11 or newer if your project uses the logout handler from the Symfony Security Component.'
        );

        $this->addRecommendation(
            (version_compare($installedPhpVersion, '5.3.18', '>=') && version_compare($installedPhpVersion, '5.4.0', '<'))
            ||
            version_compare($installedPhpVersion, '5.4.8', '>='),
            'You should use PHP 5.3.18+ or PHP 5.4.8+ to always get nice error messages for fatal errors in the development environment due to PHP bug #61767/#60909',
            'Install PHP 5.3.18+ or PHP 5.4.8+ if you want nice error messages for all fatal errors in the development environment.'
        );

        if (null !== $pcreVersion) {
            $this->addRecommendation(
                $pcreVersion >= 8.0,
                sprintf('PCRE extension should be at least version 8.0 (%s installed)', $pcreVersion),
                '<strong>PCRE 8.0+</strong> is preconfigured in PHP since 5.3.2 but you are using an outdated version of it. Symfony probably works anyway but it is recommended to upgrade your PCRE extension.'
            );
        }

        $this->addRecommendation(
            class_exists('DomDocument'),
            'PHP-DOM and PHP-XML modules should be installed',
            'Install and enable the <strong>PHP-DOM</strong> and the <strong>PHP-XML</strong> modules.'
        );

        $this->addRecommendation(
            function_exists('mb_strlen'),
            'mb_strlen() should be available',
            'Install and enable the <strong>mbstring</strong> extension.'
        );

        $this->addRecommendation(
            function_exists('iconv'),
            'iconv() should be available',
            'Install and enable the <strong>iconv</strong> extension.'
        );

        $this->addRecommendation(
            function_exists('utf8_decode'),
            'utf8_decode() should be available',
            'Install and enable the <strong>XML</strong> extension.'
        );

        $this->addRecommendation(
            function_exists('filter_var'),
            'filter_var() should be available',
            'Install and enable the <strong>filter</strong> extension.'
        );

        if (!defined('PHP_WINDOWS_VERSION_BUILD')) {
            $this->addRecommendation(
                function_exists('posix_isatty'),
                'posix_isatty() should be available',
                'Install and enable the <strong>php_posix</strong> extension (used to colorize the CLI output).'
            );
        }

        $this->addRecommendation(
            extension_loaded('intl'),
            'intl extension should be available',
            'Install and enable the <strong>intl</strong> extension (used for validators).'
        );

        if (extension_loaded('intl')) {
            // in some WAMP server installations, new Collator() returns null
            $this->addRecommendation(
                null !== new Collator('fr_FR'),
                'intl extension should be correctly configured',
                'The intl extension does not behave properly. This problem is typical on PHP 5.3.X x64 WIN builds.'
            );

            // check for compatible ICU versions (only done when you have the intl extension)
            if (defined('INTL_ICU_VERSION')) {
                $version = INTL_ICU_VERSION;
            } else {
                $reflector = new ReflectionExtension('intl');

                ob_start();
                $reflector->info();
                $output = strip_tags(ob_get_clean());

                preg_match('/^ICU version +(?:=> )?(.*)$/m', $output, $matches);
                $version = $matches[1];
            }

            $this->addRecommendation(
                version_compare($version, '4.0', '>='),
                'intl ICU version should be at least 4+',
                'Upgrade your <strong>intl</strong> extension with a newer ICU version (4+).'
            );

            if (class_exists('Symfony\Component\Intl\Intl')) {
                $this->addRecommendation(
                    \Symfony\Component\Intl\Intl::getIcuDataVersion() === \Symfony\Component\Intl\Intl::getIcuVersion(),
                    sprintf('intl ICU version installed on your system (%s) should match the ICU data bundled with Symfony (%s)', \Symfony\Component\Intl\Intl::getIcuVersion(), \Symfony\Component\Intl\Intl::getIcuDataVersion()),
                    'In most cases you should be fine, but please verify there is no inconsistencies between data provided by Symfony and the intl extension. See https://github.com/symfony/symfony/issues/15007 for an example of inconsistencies you might run into.'
                );
            }

            $this->addPhpIniRecommendation(
                'intl.error_level',
                create_function('$cfgValue', 'return (int) $cfgValue === 0;'),
                true,
                'intl.error_level should be 0 in php.ini',
                'Set "<strong>intl.error_level</strong>" to "<strong>0</strong>" in php.ini<a href="#phpini">*</a> to inhibit the messages when an error occurs in ICU functions.'
            );
        }

        $accelerator =
            (extension_loaded('eaccelerator') && ini_get('eaccelerator.enable'))
            ||
            (extension_loaded('apc') && ini_get('apc.enabled'))
            ||
            (extension_loaded('Zend Optimizer+') && ini_get('zend_optimizerplus.enable'))
            ||
            (extension_loaded('Zend OPcache') && ini_get('opcache.enable'))
            ||
            (extension_loaded('xcache') && ini_get('xcache.cacher'))
            ||
            (extension_loaded('wincache') && ini_get('wincache.ocenabled'))
        ;

        $this->addRecommendation(
            $accelerator,
            'a PHP accelerator should be installed',
            'Install and/or enable a <strong>PHP accelerator</strong> (highly recommended).'
        );

        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
            $this->addRecommendation(
                $this->getRealpathCacheSize() > 1000,
                'realpath_cache_size should be above 1024 in php.ini',
                'Set "<strong>realpath_cache_size</strong>" to e.g. "<strong>1024</strong>" in php.ini<a href="#phpini">*</a> to improve performance on windows.'
            );
        }

        $this->addPhpIniRecommendation('short_open_tag', false);

        $this->addPhpIniRecommendation('magic_quotes_gpc', false, true);

        $this->addPhpIniRecommendation('register_globals', false, true);

        $this->addPhpIniRecommendation('session.auto_start', false);

        $this->addRecommendation(
            class_exists('PDO'),
            'PDO should be installed',
            'Install <strong>PDO</strong> (mandatory for Doctrine).'
        );

        if (class_exists('PDO')) {
            $drivers = PDO::getAvailableDrivers();
            $this->addRecommendation(
                count($drivers) > 0,
                sprintf('PDO should have some drivers installed (currently available: %s)', count($drivers) ? implode(', ', $drivers) : 'none'),
                'Install <strong>PDO drivers</strong> (mandatory for Doctrine).'
            );
        }
    }

    /**
     * Loads realpath_cache_size from php.ini and converts it to int.
     *
     * (e.g. 16k is converted to 16384 int)
     *
     * @return int
     */
    protected function getRealpathCacheSize()
    {
        $size = ini_get('realpath_cache_size');
        $size = trim($size);
        $unit = strtolower(substr($size, -1, 1));
        switch ($unit) {
            case 'g':
                return $size * 1024 * 1024 * 1024;
            case 'm':
                return $size * 1024 * 1024;
            case 'k':
                return $size * 1024;
            default:
                return (int) $size;
        }
    }
}  
