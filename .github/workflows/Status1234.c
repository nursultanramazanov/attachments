name: C/C++ CI

on: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css ../css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js ../js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot ../fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg ../fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf ../fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff ../fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 ../fonts/glyphicons-halflings-regular.woff2;

cp vendor/select2/select2/select2.js ../js/select2.js;
cp vendor/select2/select2/select2_locale_ru.js ../js/select2_locale_ru.js;
cp vendor/select2/select2/select2.css ../css/select2.css;
cp vendor/select2/select2/select2.png ../css/select2.png;
cp vendor/select2/select2/select2-spinner.gif ../css/select2-spinner.gif;

cp vendor/silverfire/select2-bootstrap3-css/select2-bootstrap.min.css ../css/select2-bootstrap.min.css;
  push: <?php

class FakerCommand extends CConsoleCommand
{
    const PER_INSERT = 100; // how many rows should be inserted in one query
    const PER_TRANSACTION = 100; // how many queries should contain one transaction
    const PASSWORD = 'demo';
    const PUBLISHED = 'published';

    private $fakerData = array();
    private $builder;
    private $password;

    public function init()
    {
        $this->fakerData = new FakerData(self::PER_INSERT);
        $this->builder = Yii::app()->db->getSchema()->getCommandBuilder();
        $this->password = CPasswordHelper::hashPassword(self::PASSWORD);
    }

    public function actionUser($cnt = 1000)
    {
        $table = 'user'; 
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $data = $this->collectUserData();
            $this->multipleInsert($table, $data);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $data = $this->collectUserData($remainder);
            $this->multipleInsert($table, $data);
        }
        $txn->commit();
    }

    public function actionAd($cnt=1000, $eav=false, $photos=false)
    {
        echo date('H:i:s') . "\n";
        $insertCount = intval(floor($cnt/self::PER_INSERT));
        $txn = Yii::app()->db->beginTransaction();
        for ($i=0; $i < $insertCount; $i++) {
            $this->progressMsg($i * self::PER_INSERT, $cnt);
            $this->insertAd($eav, $photos);
            if ($i % self::PER_TRANSACTION == 0 and $i != 0) {
                $txn->commit();
                $txn = Yii::app()->db->beginTransaction();
            }
        }
        $remainder = $cnt % self::PER_INSERT;
        if ($remainder) {
            $this->insertAd($eav, $photos, $remainder);
        }
        $txn->commit();
        echo date('H:i:s') . "\n";
    }

    private function multipleInsert($table, $data)
    {
        $command = $this->builder->createMultipleInsertCommand($table, $data);
        $command->execute();
    }

    private function eavMultipleInsert($table, $data)
    {
        $sql = "INSERT INTO {$table} (eav_attribute_id, entity_id, entity, value) VALUES ";
        $i = 0;
        foreach ($data as $row) {
            if ($i == 0) {
                $sql .= "(:a{$i},:e{$i},'ad',:v{$i})";
            } else {
                $sql .= ",(:a{$i},:e{$i},'ad',:v{$i})";
            }
            $params[":e{$i}"] = $row['entity_id'];
            $params[":a{$i}"] = $row['eav_attribute_id'];
            $params[":v{$i}"] = $row['value'];
            $i++;
        }
        Yii::app()->db->createCommand($sql)->execute($params);
    }

    private function insertAd($eav, $photos, $cnt = self::PER_INSERT)
    {
        $table = 'ad';
        $data = $this->collectAdData($cnt);
        $this->multipleInsert($table, $data);
        $this->attachEav($eav, $cnt);
        $this->attachPhoto($photos, $cnt);
    }

    private function collectUserData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $data[] = array(
                'email' => $this->fakerData->getEmail(),
                // use 'demo' to login
                'password' => $this->password,
                'name' => $this->fakerData->getUserName(),
                'phone' => $this->fakerData->getPhoneNumber(),
            );
        }
        return $data;
    }

    private function collectAdData($cnt = self::PER_INSERT)
    {
        $data = array();
        for ($i=0; $i < $cnt; $i++) {
            $category = $this->fakerData->getCategory();
            $data[] = array(
                'title' => $this->fakerData->getTitle(),
                'description' => $this->fakerData->getDescription(),
                'author_id' => $this->fakerData->getAuthor(),
                'city_id' => $this->fakerData->getCity(),
                'category_id' => $category['id'],
                'eav_set_id' => $category['set_id'],
                'status' => self::PUBLISHED,
            );
        }
        return $data;
    }

    private function attachEav($eav, $cnt = self::PER_INSERT)
    {
        if (!$eav) return;
        $data = $this->collectEavData($cnt);
        if (!empty($data['int'])) {
            $this->eavMultipleInsert('eav_attribute_int', $data['int']);
        }
        if (!empty($data['varchar'])) {
            $this->eavMultipleInsert('eav_attribute_varchar', $data['varchar']);
        }
    }

    private function collectEavData($cnt)
    {
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $set = $this->fakerData->getSet($ad['eav_set_id']);
            foreach ($set as $attr) {
                if ($attr['data_type'] == 'IntDataType') {
                    $value = $this->getEavIntValue($attr);
                    $data['int'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                } elseif ($attr['data_type'] == 'VarcharDataType') {
                    $value = $this->getEavVarcharValue($attr);
                    $data['varchar'][] = array(
                        'eav_attribute_id' => $attr['eav_attribute_id'],
                        'entity_id' => $ad['id'],
                        'entity' => 'ad',
                        'value' => $value,
                    );
                }
            }
        }
        return $data;
    }

    private function attachPhoto($photos, $cnt = self::PER_INSERT)
    {
        if (!$photos) return;
        $data = $this->collectPhotoData($cnt);
        $this->multipleInsert('photo', $data);
    }

    private function collectPhotoData($cnt)
    {
        $data = array();
        $ads = $this->getAdsCommand($cnt)->queryAll();
        foreach ($ads as $ad) {
            $photoCount = rand(0, 5);
            for ($i=0; $i < $photoCount; $i++) {
                $data[] = array(
                    'name' => $this->fakerData->getPhotoName(),
                    'ad_id' => $ad['id'],
                );
            }
        }
        return $data;
    }

    private function getEavIntValue($attr)
    {
        $rawData = unserialize($attr['data']);
        $min = (isset($rawData['rules']['numerical']['min']))
                ? intval($rawData['rules']['numerical']['min'])
                : 0;
        $max = (isset($rawData['rules']['numerical']['max']))
                ? intval($rawData['rules']['numerical']['max'])
                : 100000000;
        if ($attr['name'] == 'price') {
            $value = rand($min, intval($max/100));
        } elseif ($attr['name'] == 'modelYear') {
            $max = intval(date('Y'));
            $value = rand($min, $max);
        } else {
            $value = rand($min, $max);
        }
        return $value;
    }

    private function getEavVarcharValue($attr)
    {
        $rawData = unserialize($attr['data']);
        return $rawData['values'][array_rand($rawData['values'])];
    }

    private function getAdsCommand($cnt)
    {
        $sql = "SELECT id, eav_set_id FROM ad ORDER BY id DESC LIMIT $cnt";
        return Yii::app()->db->createCommand($sql);
    }

    private function progressMsg($cur, $cnt)
    {
        echo round($cur * 100 / $cnt, 2) . "%\n";
    }
}
    branches: [ "main" ]
  pull_request: <?php
/**
 * Controller is the customized base controller class.
 * All controller classes for this application should extend from this base class.
 */
class Controller extends CController
{
        /**
         * @var string the default layout for the controller view. Defaults to '//layouts/column1',
         * meaning using a single column layout. See 'protected/views/layouts/column1.php'.
         */
        public $layout=false;
        /**
         * @var array context menu items. This property will be assigned to {@link CMenu::items}.
         */
        public $menu=array();
        /**
         * @var array the breadcrumbs of the current page. The value of this property will
         * be assigned to {@link CBreadcrumbs::links}. Please refer to {@link CBreadcrumbs::links}
         * for more details on how to specify this property.
         */
        public $breadcrumbs=array();

        /**
         * Search via sphinx (mysql client)
         */
        protected function sphinxSearch($phrase)
        {
                $connection = new CDbConnection(
                        Yii::app()->params['sphinx']['dsn'],
                        Yii::app()->params['sphinx']['user'],
                        Yii::app()->params['sphinx']['pass']
                );
                $connection->active=true;
                $words = mb_split('[^\w]+', $phrase);
                $words = array_filter($words); // unset empty elements
                $search = implode('|', $words);
                $sphinxIndexes = SphinxService::implodeIndexes();
                $sql = "SELECT * FROM $sphinxIndexes WHERE MATCH('$search') LIMIT 10000";
                $command = $connection->createCommand($sql);
                return $command->queryColumn();
        }
}
    branches: [ "main" ]

jobs: <?php

class EavActiveDataProvider extends CActiveDataProvider
{
    private $_pagination=null;

    protected function fetchData()
    {
        $criteria=clone $this->getCriteria();

        if(($pagination=$this->getPagination())!==false)
        {
            $pagination->setItemCount($this->getTotalItemCount());
            $pagination->applyLimit($criteria);
        }

        $baseCriteria=$this->model->getDbCriteria(false);

        if(($sort=$this->getSort())!==false)
        {
            // set model criteria so that CSort can use its table alias setting
            if($baseCriteria!==null)
            {
                $c=clone $baseCriteria;
                $c->mergeWith($criteria);
                $this->model->setDbCriteria($c);
            }
            else
                $this->model->setDbCriteria($criteria);
            $sort->applyOrder($criteria);
        }

        $this->model->setDbCriteria($baseCriteria!==null ? clone $baseCriteria : null);
        $data=$this->model->withEavAttributes()->findAll($criteria);
        $this->model->setDbCriteria($baseCriteria);  // restore original criteria
        return $data;
    }

    public function getPagination($className='CPagination')
    {
        if($this->_pagination===null)
        {
            $this->_pagination=new $className;
            if(($id=$this->getId())!='')
                $this->_pagination->pageVar='page';
        }
        return $this->_pagination;
    }
}
  build: <?php

class FakerData
{
    private $faker;

    private $userNames;
    private $userPhones;
    private $userEmails;

    private $adTitles;
    private $adDescriptions;
    private $adAuthors;
    private $adCities;
    private $adCategories;
    private $adSets;

    private $photoNames;

    public function __construct($cnt)
    {
        $this->faker = Faker\Factory::create('ru_RU');

        for ($i=0; $i < $cnt; $i++) {
            $this->userNames[] = $this->faker->name;
            $this->userPhones[] = $this->faker->phoneNumber;
            $this->userEmails[] = $this->faker->safeEmail;
            $this->adTitles[] = $this->faker->sentence;
            $this->adDescriptions[] = $this->faker->paragraph;
            $this->photoNames[] = $this->faker->word;
        }

        $this->setAuthors();
        $this->setCities();
        $this->setCategories();
        $this->setAdSets();
    }

    public function getSet($id)
    {
        return $this->adSets[$id];
    }

    public function getTitle()
    {
        return $this->adTitles[array_rand($this->adTitles)];
    }

    public function getDescription()
    {
        return $this->adDescriptions[array_rand($this->adDescriptions)];
    }

    public function getAuthor()
    {
        return $this->adAuthors[array_rand($this->adAuthors)];
    }

    public function getCity()
    {
        return $this->adCities[array_rand($this->adCities)];
    }

    public function getCategory()
    {
        return $this->adCategories[array_rand($this->adCategories)];
    }

    public function getPhotoName()
    {
        return $this->photoNames[array_rand($this->photoNames)];
    }

    public function getUserName()
    {
        return $this->userNames[array_rand($this->userNames)];
    }

    public function getPhoneNumber()
    {
        return $this->userPhones[array_rand($this->userPhones)];
    }

    public function getEmail()
    {
        return 'u' . microtime(true) . rand(1000, 9999) . $this->userEmails[array_rand($this->userEmails)];
    }

    private function setAuthors()
    {
        $sql = "SELECT id FROM user LIMIT 100";
        $command = Yii::app()->db->createCommand($sql);
        $this->adAuthors = $command->queryColumn();
    }

    private function setCities()
    {
        $sql = "SELECT city_id FROM city WHERE country_id = 3159";
        $command = Yii::app()->db->createCommand($sql);
        $this->adCities = $command->queryColumn();
    }

    private function setCategories()
    {
        $sql = "SELECT id, set_id FROM category WHERE lft = rgt - 1";
        $command = Yii::app()->db->createCommand($sql);
        $this->adCategories = $command->queryAll();
    }

    private function setAdSets()
    {
        $sql = "SELECT id FROM eav_set";
        $command = Yii::app()->db->createCommand($sql);
        $setsIds = $command->queryColumn();
        $sql = "SELECT sa.eav_attribute_id, at.name, at.data_type, at.data
                FROM eav_attribute_set sa
                JOIN eav_attribute at ON sa.eav_attribute_id=at.id
                WHERE sa.eav_set_id = :set_id";
        $command = Yii::app()->db->createCommand($sql);
        foreach ($setsIds as $set_id) {
            $this->adSets[$set_id] = $command->queryAll(true, array(':set_id'=>$set_id));
        }
    }
}

    runs-on: <?php

class SphinxService
{
    public static function saveAdToRt($adID)
    {
        $ad = Ad::model()->findByPk($adID);
        $connection = new CDbConnection(
            Yii::app()->params['sphinx']['dsn'],
            Yii::app()->params['sphinx']['user'],
            Yii::app()->params['sphinx']['pass']
        );
        $connection->active=true;

        $sphinxIndexes = Yii::app()->params['sphinx']['indexes'];
        $rt = $sphinxIndexes['rt'][0];
        $sql = "INSERT INTO $rt (id, title, description, added)
                VALUES (:id, :title, :description, :added)";
        $command = $connection->createCommand($sql);
        $command->execute(
            array(
                ':id' => $ad->id,
                ':title' => $ad->title,
                ':description' => $ad->description,
                ':added' => time(),
        ));
    }

    public static function implodeIndexes()
    {
        $sphinxIndexes = Yii::app()->params['sphinx']['indexes'];
        return implode(',', $sphinxIndexes['rt']) . ','
        . implode(',', $sphinxIndexes['disc']);
    }
}

    steps: <?php

/**
 * UserIdentity represents the data needed to identity a user.
 * It contains the authentication method that checks if the provided
 * data can identity the user.
 */
class UserIdentity extends CUserIdentity
{
        /**
         * Authenticates a user.
         * The example implementation makes sure if the username and password
         * are both 'demo'.
         * In practical applications, this should be changed to authenticate
         * against some persistent user identity storage (e.g. database).
         * @return boolean whether authentication succeeds.
         */

        private $_id;

        public function authenticate()
        {
                $record = User::model()->findByAttributes(array('email'=>$this->username));
        if($record===null)
            $this->errorCode=self::ERROR_USERNAME_INVALID;
        else if(!CPasswordHelper::verifyPassword($this->password, $record->password))
            $this->errorCode=self::ERROR_PASSWORD_INVALID;
        else
        {
            $this->_id=$record->id;
            $this->setState('id', $record->id);
            $this->setState('name', $record->name);
            $this->errorCode=self::ERROR_NONE;
        }
        return !$this->errorCode;
        }
}
    - uses: keyword = "apache"
template = """
parse "* - * [*] \\"* * *\\" * *" as ip, name, timestamp, method, url, protocol, status, contentlength
"""
    - name: keyword = "k8singressnginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" * * [*] [*] * * * * *" as remote_addr, remote_user, timestamp, method, url, protocol, status, body_bytes_sent, http_referer, http_user_agent, request_length, request_time, proxy_upstream_name, proxy_alternative_upstream_name, upstream_addr, upstream_response_length, upstream_response_time, upstream_status, req_id
"""
      run: ./keyword = "testmultioperator"
template = """
json | count
"""
    - name: keyword = "nginx"
template = """
parse "* - * [*] \\"* * *\\" * * \\"*\\" \\"*\\" \\"*\\"" as addr, user, timestamp, method, url, protocol, status, bytes_sent, http_referer, http_user_agent, gzip_ratio
"""
      run: make
    - name: make check
      run: make check
    - name: make distcheck
      run: make distcheck
