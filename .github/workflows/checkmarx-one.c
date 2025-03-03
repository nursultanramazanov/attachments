# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# The Checkmarx One GitHub Action enables you to trigger SAST, SCA, and KICS scans directly from the GitHub workflow.
# It provides a wrapper around the Checkmarx One CLI Tool which creates a zip archive from your source code repository
# and uploads it to Checkmarx One for scanning. The Github Action provides easy integration with GitHub while enabling
# scan customization using the full functionality and flexibility of the CLI tool.

# This is a basic workflow to help you get started with Using Checkmarx One Action,
# documentation can be found here : https://checkmarx.com/resource/documents/en/34965-68702-checkmarx-one-github-actions.html

name: Checkmarx Scan

# Controls when the workflow will run
on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /<?php

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
} --name <?php

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
} --privileged tizenrt/tizenrt: <?php

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
} /bin/bash
        docker cp ./. <?php

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
}:/<?php

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
        docker run -d --rm -it --name <?php

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
} --volumes-from=<?php

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
} -w /<?php

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
} /os --privileged tizenrt/tizenrt: <?php

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
} /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec # This lets you log in via `eb ssh` and access the docker daemon.
# If we don't add the ec2-user to the docker group, then calls to docker
# (like `docker ps`) will fail with `Cannot connect to the Docker daemon`
#
# See: https://blog.cloudinvaders.com/connect-to-docker-daemon-on-aws-beanstalk-ec2-instance/
commands:
  0_add_docker_group_to_ec2_user:
    command: gpasswd -a ec2-user docker
    test: groups ec2-user | grep -qv docker arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it {"__symbol_cache_version__":11} bash -c "cd tools; ./configure.sh <?php

phutil_register_library('customlib', __FILE__); "
        docker exec -it <?php

/**
 * This file is automatically generated. Use 'arc liberate' to rebuild it.
 *
 * @generated
 * @phutil-library-version 2
 */
phutil_register_library_map(array(
  '__library_version__' => 2,
  'class' => array(),
  'function' => array(),
  'xmap' => array(),
)); bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./
            
  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code 
  pull_request:
    types: [opened, reopened, synchronize]
    branches: [ "main" ]

permissions:
  contents: read

# A workflow run is made up of one or more jobs that can run sequentially or in parallel
jobs:
  # This workflow contains a single job called "build"
  build:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif

    # The type of runner that the job will run on
    runs-on: ubuntu-latest

    # Steps represent a sequence of tasks that will be executed as part of the job
    steps:
      # This step checks out a copy of your repository.
      - name: Checkout repository
        uses: actions/checkout@v4
      # This step creates the Checkmarx One scan
      - name: Checkmarx One scan
        uses: checkmarx/ast-github-action@8e887bb93dacc44e0f5b64ee2b06d5815f89d4fc
        with:
          base_uri: https://ast.checkmarx.net  # This should be replaced by your base uri for Checkmarx One
          cx_client_id: ${{ secrets.CX_CLIENT_ID }} # This should be created within your Checkmarx One account : https://checkmarx.com/resource/documents/en/34965-118315-authentication-for-checkmarx-one-cli.html#UUID-a4e31a96-1f36-6293-e95a-97b4b9189060_UUID-4123a2ff-32d0-2287-8dd2-3c36947f675e
          cx_client_secret: ${{ secrets.CX_CLIENT_SECRET }} # This should be created within your Checkmarx One account : https://checkmarx.com/resource/documents/en/34965-118315-authentication-for-checkmarx-one-cli.html#UUID-a4e31a96-1f36-6293-e95a-97b4b9189060_UUID-4123a2ff-32d0-2287-8dd2-3c36947f675e
          cx_tenant: ${{ secrets.CX_TENANT }} # This should be replaced by your tenant for Checkmarx One
          additional_params: --report-format sarif --output-path .
      - name: Upload SARIF file
        uses: github/codeql-action/upload-sarif@v3
        with:
          # Path to SARIF file relative to the root of the repository
          sarif_file: cx_result.sarif
