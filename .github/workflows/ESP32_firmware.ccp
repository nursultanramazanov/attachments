name: Erlang CI

on: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511181141 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $table = $schema->createTable('user');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('name', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('salt', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('hash', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('email', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('login', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('accessToken', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('user');
    }
}
  push: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511202416 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $userTable = $schema->getTable('user');

        $testTable = $schema->createTable('test');
        $testTable->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $testTable->addColumn('user_id', 'integer');
        $testTable->addForeignKeyConstraint($userTable, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $testTable->addColumn('title', 'string');
        $testTable->addColumn('description', 'text', [
            'notnull' => false,
        ]);
        $testTable->addColumn('time_limit', 'integer', [
            'default' => 0,
        ]);
        $testTable->addColumn('added', 'datetime');

        $testTable->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test');
    }
}
    branches: [ "main" ]
  pull_request: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511212105 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $test = $schema->getTable('test');

        $tag = $schema->createTable('tag');
        $tag->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $tag->addColumn('name', 'string');
        $tag->addUniqueIndex(['name']);
        $tag->setPrimaryKey(['id']);

        $test_tag = $schema->createTable('test_tag');
        $test_tag->addColumn('test_id', 'integer');
        $test_tag->addColumn('tag_id', 'integer');
        $test_tag->addForeignKeyConstraint($tag, ['tag_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->addForeignKeyConstraint($test, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->setPrimaryKey(['test_id', 'tag_id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test_tag');
        $schema->dropTable('tag');
    }
}
    branches: [ "main" ]

permissions: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512025516 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $testTable = $schema->getTable('test');

        $table = $schema->createTable('question');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('description', 'text');
        $table->addColumn('points', 'integer');
        $table->addColumn('sequence_number', 'integer');
        $table->addColumn('test_id', 'integer');
        $table->addForeignKeyConstraint($testTable, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $table->addColumn('answer_text', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('answer_decimal', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('precision', 'decimal', [
            'notnull' => false,
        ]);
        $table->addColumn('type', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('question');
    }
}
  contents: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512133552 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $question = $schema->getTable('question');

        $variant = $schema->createTable('variant');
        $variant->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $variant->addColumn('value', 'string');
        $variant->addColumn('is_right', 'string');
        $variant->addColumn('question_id', 'integer');
        $variant->addForeignKeyConstraint($question, ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $variant->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('variant');
    }
}

jobs: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512135536 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $user = $schema->getTable('user');

        $attempt = $schema->createTable('attempt');
        $attempt->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $attempt->addColumn('started', 'datetime');
        $attempt->addColumn('user_id', 'integer');
        $attempt->addForeignKeyConstraint($user, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $attempt->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('attempt');
    }
}

  build: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512145224 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $answer = $schema->createTable('answer');
        $answer->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $answer->addColumn('question_id', 'integer');
        $answer->addColumn('attempt_id', 'integer');
        $answer->addColumn('variant_id', 'integer', [
            'notnull' => false
        ]);
        $answer->addForeignKeyConstraint('question', ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('attempt', ['attempt_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('variant', ['variant_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addColumn('decimal_answer', 'decimal', [
            'notnull' => false,
        ]);
        $answer->addColumn('text_answer', 'string', [
            'notnull' => false,
        ]);
        $answer->addColumn('type', 'string');

        $answer->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('answer');
    }
}

    runs-on: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160513005635 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('test_id', 'integer');
        $attempt->addForeignKeyConstraint('test', ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('test_id');
    }
}

    container: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160518161400 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('status', 'string', [
            'default' => 'active',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('status');
    }
}
      image: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}

    steps: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}
    - uses: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %}
    - name: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
      run: -----BEGIN CERTIFICATE-----
MIIDRTCCAi2gAwIBAgIGMTM2OTY5MA0GCSqGSIb3DQEBCwUAMF4xEDAOBgNVBAMT
B1Rlc3QgQ0ExCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpDYWxpZm9ybmlhMRYwFAYD
VQQHEw1TYW4gRnJhbmNpc2NvMRAwDgYDVQQKEwdUZXN0IENBMB4XDTIyMDMwNTE5
MDU0NloXDTIzMDMwNTE5MDU0NlowEzERMA8GA1UEAxMIZGMubG9jYWwwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC65kPnRp6v4pSoc1celQy1veAXKKma
uUU+TfHWWBw/NOReepxsWxDgl+g9N8ISMrbIZB2tnmqaCXAV/E4GcggHlihm5Mci
JYznsREtQ0q1dA2VGHhtX28b9V9TeBhiF6ZXIqfKkclCl7xCgUCJQ0JV6jCS84GI
pSi4DD9XPdfjdUa9fU+5Cs/XfkCC1bqiWlpPBticD4uydFdB6N8vJlo41uKLfPUi
bduqvQWAkDrBLVT2MSJxZEOaM5T6b3zilwZBeWwfwo/X3KmGze7rHL9ItQM7vUPH
k5jnLZYfTTztHCUOfN058Mm8tJNzMXBYsJJS53+cRsrcYiWhk1MRwotpAgMBAAGj
VDBSMAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQWMBQGCCsG
AQUFBwMBBggrBgEFBQcDAjATBgNVHREEDDAKgghkYy5sb2NhbDANBgkqhkiG9w0B
AQsFAAOCAQEAfPv5vzoJDFF9OPJ34tzK6zdcJVb5sRWUdAs8PIxBqrv0wSyW8iA6
9nJI3Fw9/hmF/ipLZlmDFYvKvYdMgU1i2P0mhmz377Yj/XgO/6z4izY9L1NliS8z
9AioA7wKf5vEri8f4w3w1nwKxWPcENvSv9Bm73uGk6cOYzXnxJMx4QxnxAx276tA
SV8AvrCAoKK9CUxp+VWAVp2HYBRt/sFT7NA020vCbS7I/cbYDBbs+FpnDrI9cBAr
UtTlThfHbH2XdSjV5TTeg+NqqfZfz0kxHloOuX53C5W7T/brmWGjvQkcq3b2+CYY
VxPTjjpduhkjhvJs6p19d87F0SMkP3ONGA==
-----END CERTIFICATE-----

-----BEGIN CERTIFICATE-----
MIIDXjCCAkagAwIBAgIFOTA4MTUwDQYJKoZIhvcNAQELBQAwXjEQMA4GA1UEAxMH
VGVzdCBDQTELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNhbGlmb3JuaWExFjAUBgNV
BAcTDVNhbiBGcmFuY2lzY28xEDAOBgNVBAoTB1Rlc3QgQ0EwHhcNMjIwMzA0MTUz
OTUxWhcNMjMwMzA0MTUzOTUxWjBeMRAwDgYDVQQDEwdUZXN0IENBMQswCQYDVQQG
EwJVUzETMBEGA1UECBMKQ2FsaWZvcm5pYTEWMBQGA1UEBxMNU2FuIEZyYW5jaXNj
bzEQMA4GA1UEChMHVGVzdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC
ggEBAMjvly5N/iieO1YM97pB+K5eJQ5Jxu+LRePgEDAEwysxHgmP5g0xdaw1tZNN
dw8Z5/emvNUFecCKjU0IgPTjlfdLgHOkg16+fEOOj5FlN7I4Lc9teAhdBtHPZr+n
ok0XNdEulI5ijFeZdIyO4gBvwKfwwCY71XLxNiNLsQvuDUxabDyaFppNJliYdAuw
yh1jEpd+sFPWtsEK56chcchKu4yKJZWaeSvD7bJAJCdaSdnknSUYc9S5RoR+sAu4
DpSl9tAO8bKRgyo9ApnwqWS3RM1yYY9z6GJGauXH1BHXpMEoc5h2/nTaI9ripqHy
GaQwMmnGtcTdvbL5wTDl6mkHQw0CAwEAAaMjMCEwDwYDVR0TAQH/BAUwAwEB/zAO
BgNVHQ8BAf8EBAMCAgQwDQYJKoZIhvcNAQELBQADggEBAFfziw0Pjdmo3pgu900J
Pw6H/yCqEvQIEa7HrFowVbutr9feIRPXKEI91mSlkFXhX8axM0mu/KHkUrhr1ApY
wW1hH1+dOZfiPSvhi3GGkwsD2M43xMOYfNo0COpBvogLu44xJapT7i908Krocg4M
w2zAse4o+0m6fGD99tyc5PnbIJ09M/zeHyxwPtnbW9hIvum/CdoLcylZD2jYd9d3
9quxbjrj0oHynGRjV20esQuRqNHrqC/QlNnEVomeL62OrPIbrhRIFbG77zrteNxB
gZTmrCIAlaVm7caGpk+e7pKPb5C7OyWWPLYeR5h9e+hNta0+ht80/jYvUS5nw5Ge
0ZE=
-----END CERTIFICATE-----
    - name: -----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAuuZD50aer+KUqHNXHpUMtb3gFyipmrlFPk3x1lgcPzTkXnqc
bFsQ4JfoPTfCEjK2yGQdrZ5qmglwFfxOBnIIB5YoZuTHIiWM57ERLUNKtXQNlRh4
bV9vG/VfU3gYYhemVyKnypHJQpe8QoFAiUNCVeowkvOBiKUouAw/Vz3X43VGvX1P
uQrP135AgtW6olpaTwbYnA+LsnRXQejfLyZaONbii3z1Im3bqr0FgJA6wS1U9jEi
cWRDmjOU+m984pcGQXlsH8KP19yphs3u6xy/SLUDO71Dx5OY5y2WH0087RwlDnzd
OfDJvLSTczFwWLCSUud/nEbK3GIloZNTEcKLaQIDAQABAoIBAHnK4gcNYw101rDq
8KOyN2UbhbMxwUxdXT0bp/Oo2UdrHUJgHh0SPu5Mr39tHV+kgq4VZ62jJQ6aBdud
3e6fUpI3DL74aLfcxnSZU3sudbnHFa7KEngbj5kljegjw6fwh2oS4ErGM+PcY0bn
WXnZRZodEWhBnHDCR8Wf3Yrpdwz2Wz/mtg+c8CMjTV4WG7OZvYMRwIm8Sapqbws+
YzBzv1YSNgxg1iOuvCXkqvL26ENiBuk+ibbIz3QOYEqCRQ0dqXNCzd0s7anK0aIG
3RqsmCXF4vEbb3ie1lf6YE7fn0ld8/1LDjVBhmpLoNpbFNTubuoDD3vVYS42W8wj
3VQO74ECgYEA49E0Oan9m2cS89JC6/6nsRVvbjWBTiZn0/wW9uLMNK/IvQ1C41rc
MpGk/RzMQHxz0LaWFDcVSng3GdA6LDdDe56JWIlrCvFCDDLL38Ss8TebsliQJP7+
uGXvjxT70aX6ErCZURD/L3xZdpu3Q3p/LkUpAYsAriYMuS1AtdBHwDECgYEA0gU6
aAFg5jNdPWVXe4eeJ1vfGST9m1L+hO3Zu8FcpcRp8u7u1+zWJZWmt4Knux9WFjoR
+jd87ihuYiOkH31KpH8aJmXJ/OctMQLwDSms7JsQc7n8pxB5ye5pq6e6yuAknHf+
cOnwXRqxeN3nUB7bs+hT38/McGhWkxKouFaxKLkCgYEAtDmefTfT34rNXYIrWxMc
imUQwDkn0DFl1jgJ2spVDiE6RwUks0QACylZQrvzgdhyrtyjId7pwv4TaSJnwu79
PeCPezFLM17lB01wJ9gpkLKTQ6X4cqwfyrUprSHGjXYmvX5j69PsbR6Mj95kPse7
sVkZNZVHtn13d08rcCoTeSECgYAt8CTRYRPP5oZLiqIzC/wFAexvtI6gbevsKZaJ
ogbg33CJScgGhPBmcjBwTWzg2j7bR8Js57w037UdVExu8tnj10ViBrvCzNIuuUTG
rEn9ywEzPwhLhqgNClAk0uBhxdtcOkb2bcU8qD0PggI5nTZTSMsn1m0SXVAqhVdQ
Uu4XSQKBgBn7UlZnQN0FOh0yGdnq6Pz8GFI9WxYaNjAu8JF+lsfSOMbPYJwTjxhb
clrVPaB8FWy9XLat8ewSfvPIgEIbpyyYGEDL9UM4BzYnNmosQ4PN0xhvMtw4Ilp7
NgMFbaN5R8IeVJNwZnrCXzRJXx8sycl+Fb2wzyiq3cbw/mMtfSCq
-----END RSA PRIVATE KEY-----
      run: import hashlib
import random
from typing import List
import requests
import json
import math


URL = "https://dc.local/token/"
PSK = "1234"

NUM_TOKENS = 200
TOKEN_LENGTH = 32
BATCH_SIZE = 50

def getRandomToken():
    token = ""
    for _ in range(TOKEN_LENGTH):
        byte = random.randint(0, 255)
        token += f'{byte:02x}'
    return token


def removeDuplicates(_list):
    _set = set(_list)
    return list(_set)


def sort(_list):
    return sorted(_list)

def makeRequestBody(tokens):
    body =  '{\n'
    body += '\t"tokens": [\n'
    for token in tokens:
        body += '\t\t{\n'
        body += f'\t\t\t"toPut": "{token}"\n'
        body += '\t\t},\n'
    if body[-2] == ',':
        body = body[:-2] + '\n'
    body += '\t]\n'
    body += '}\n'
    return body


def sendBatched(tokens: List[str]):
    while len(tokens) > 0:
        print(len(tokens))
        data = makeRequestBody(tokens[:BATCH_SIZE])
        tokens = tokens[BATCH_SIZE:]      
        resp = requests.request(
            "PUT",
            URL,
            headers={
                "psk": PSK
            },
            data=data,
            verify=False
        )
        print(f"Resp code: {resp.status_code}")
        print(f"Resp body: {resp.content}")        


tokensList = [getRandomToken() for _ in range(NUM_TOKENS)]
tokensUnique = removeDuplicates(tokensList)
tokensSorted = sort(tokensUnique)

tokensStr = ""
for token in tokensSorted:
    tokensStr += token

hash = hashlib.sha256(tokensStr.encode());
print(hash.hexdigest())

print(f"Unique: {len(tokensUnique)}")

sendBatched(tokensList)


resp = requests.request(
    "GET",
    URL,
    headers={
        "psk": PSK
    },
    verify=False
)

dcHash = str(resp.json()['hash'])
print(dcHash)
print(hash.hexdigest() == dcHash) , ct
