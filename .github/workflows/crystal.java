name: Crystal CI

on: import cv2
import numpy as np
from tqdm import tqdm

# Define the available resolutions
resolutions = [
    (320, 240),
    (640, 480),
    (1280, 720),
    (1920, 1080),
    (750, 1334),   # iPhone 6/7/8
    (1080, 1920),  # iPhone 6/7/8 Plus
    (1125, 2436),  # iPhone X/XS/11 Pro
    (1242, 2688),  # iPhone XS Max/11 Pro Max
    (1440, 2560),  # Google Pixel 2 XL
    (1440, 3040),  # Samsung Galaxy S10/S20
    (1440, 3200),  # Sony Xperia 1 II
    (1600, 2560),  # Google Pixel 4 XL
    (2160, 3840)   # Samsung Galaxy S21 Ultra
]

# Print the available resolutions
print("Available resolutions:")
for i, res in enumerate(resolutions):
    if i == 4:
        print("\nFor smartphones:")
    print(f"{i+1}. {res[0]}x{res[1]}")
print()

# Ask the user to select or enter a resolution
res_choice = input("Enter the number of your choice or enter a custom resolution (e.g. 1920x1080): ")
if "x" in res_choice:
    resolution = tuple(map(int, res_choice.split("x")))
else:
    res_choice = int(res_choice)
    resolution = resolutions[res_choice-1]

# Define the video codec and frame rate
codec = cv2.VideoWriter_fourcc(*"mp4v")
fps = 60

# Create the video writer object
filename = f"static_{resolution[0]}x{resolution[1]}.mp4"
out = cv2.VideoWriter(filename, codec, fps, resolution)

# Generate and write each frame of the video
for i in tqdm(range(fps * 10)):
    frame = np.random.randint(0, 255, (resolution[1], resolution[0], 3), dtype=np.uint8)
    out.write(frame)

# Release the video writer object
out.release()

print(f"Static video saved to {filename}")
  push: import numpy as np
from PIL import Image
from tqdm import tqdm
import imageio

# Set up variables
resolution = input("Enter the resolution (e.g. 1920x1080): ")
fps = int(input("Enter the frames per second (e.g. 60): "))
duration = int(input("Enter the duration in seconds (e.g. 10): "))

# Split resolution into width and height
width, height = map(int, resolution.split("x"))

# Generate static frames
frames = []
for i in tqdm(range(int(fps * duration)), desc="Generating frames"):
    frame = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)
    frames.append(Image.fromarray(frame))

# Save frames as GIF
filename = f"static_{resolution}.gif"
imageio.mimsave(filename, frames, fps=fps)
print(f"Static saved as {filename}")
    branches: [ "main" ]
  pull_request: import cv2
import numpy as np
from tqdm import tqdm

# Set the width and height of the video
width = 1920
height = 1080

# Set the frame rate of the video
fps = 60

# Create a VideoWriter object to write the output video
fourcc = cv2.VideoWriter_fourcc(*'mp4v')
out = cv2.VideoWriter('static.mp4', fourcc, fps, (width, height))

# Generate frames of static and write them to the output video
for i in tqdm(range(1800)):
    # Create a random matrix of values between 0 and 255
    static = np.random.randint(0, 256, (height, width, 3), dtype=np.uint8)

    # Apply a Gaussian blur to the matrix to simulate the blurring effect
    static = cv2.GaussianBlur(static, (3, 3), 0)

    # Write the frame to the output video
    out.write(static)

# Release the VideoWriter object
out.release()
    branches: [ "main" ]

jobs: imports:
    - { resource: parameters.yml }
    - { resource: security.yml }
    - { resource: services.yml }

# Put parameters here that don't need to change on each machine where the app is deployed
# http://symfony.com/doc/current/best_practices/configuration.html#application-related-configuration
parameters:
    locale: en

framework:
    #esi:             ~
    #translator:      { fallbacks: ["%locale%"] }
    secret:          "%secret%"
    router:
        resource: "%kernel.root_dir%/config/routing.yml"
        strict_requirements: ~
    form:            ~
    csrf_protection: ~
    validation:      { enable_annotations: true }
    #serializer:      { enable_annotations: true }
    templating:
        engines: ['twig']
    default_locale:  "%locale%"
    trusted_hosts:   ~
    trusted_proxies: ~
    session:
        # http://symfony.com/doc/current/reference/configuration/framework.html#handler-id
        handler_id:  session.handler.native_file
        save_path:   "%kernel.root_dir%/../var/sessions/%kernel.environment%"
    fragments:       ~
    http_method_override: true
    assets: ~

# Twig Configuration
twig:
    debug:            "%kernel.debug%"
    strict_variables: "%kernel.debug%"
    form_themes:
        - 'bootstrap_3_layout.html.twig'

# Doctrine Configuration
doctrine:
    dbal:
        driver:   pdo_mysql
        host:     "%database_host%"
        port:     "%database_port%"
        dbname:   "%database_name%"
        user:     "%database_user%"
        password: "%database_password%"
        charset:  UTF8
        default_table_options:
            charset: UTF8
            collate: utf8_general_ci
        # if using pdo_sqlite as your database driver:
        #   1. add the path in parameters.yml
        #     e.g. database_path: "%kernel.root_dir%/data/data.db3"
        #   2. Uncomment database_path in parameters.yml.dist
        #   3. Uncomment next line:
        #     path:     "%database_path%"

    orm:
        auto_generate_proxy_classes: "%kernel.debug%"
        naming_strategy: doctrine.orm.naming_strategy.underscore
        auto_mapping: true

# Swiftmailer Configuration
swiftmailer:
    transport: "%mailer_transport%"
    host:      "%mailer_host%"
    username:  "%mailer_user%"
    password:  "%mailer_password%"
    spool:     { type: memory }

assetic:
    debug:          '%kernel.debug%'
    use_controller: '%kernel.debug%'
    filters:
        cssrewrite: ~

doctrine_migrations:
    dir_name: "%kernel.root_dir%/DoctrineMigrations"
    namespace: TestHub\Migrations
    table_name: migration_versions
    name: Application Migrations
  build: imports:
    - { resource: config.yml }

framework:
    router:
        resource: "%kernel.root_dir%/config/routing_dev.yml"
        strict_requirements: true
    profiler: { only_exceptions: false }

web_profiler:
    toolbar: true
    intercept_redirects: false

monolog:
    handlers:
        main:
            type: stream
            path: "%kernel.logs_dir%/%kernel.environment%.log"
            level: debug
            channels: [!event]
        console:
            type:   console
            channels: [!event, !doctrine]
        # uncomment to get logging in your browser
        # you may have to allow bigger header sizes in your Web server configuration
        #firephp:
        #    type:   firephp
        #    level:  info
        #chromephp:
        #    type:   chromephp
        #    level:  info

#swiftmailer:
#    delivery_address: me@example.com

    runs-on: imports:
    - { resource: config.yml }

#framework:
#    validation:
#        cache: validator.mapping.cache.doctrine.apc
#    serializer:
#        cache: serializer.mapping.cache.doctrine.apc

#doctrine:
#    orm:
#        metadata_cache_driver: apc
#        result_cache_driver: apc
#        query_cache_driver: apc

monolog:
    handlers:
        main:
            type:         fingers_crossed
            action_level: error
            handler:      nested
        nested:
            type:  stream
            path:  "%kernel.logs_dir%/%kernel.environment%.log"
            level: debug
        console:
            type:  console

    container: imports:
    - { resource: config_dev.yml }

framework:
    test: ~
    session:
        storage_id: session.storage.mock_file
    profiler:
        collect: false

web_profiler:
    toolbar: false
    intercept_redirects: false

swiftmailer:
    disable_delivery: true

doctrine:
    dbal:
        driver_class: TestHubBundle\DBAL\Driver
        host:     localhost
        dbname:   testhub_test
        user:     root
        password: null
      image: # This file is a "template" of what your parameters.yml file should look like
# Set parameters here that may be different on each deployment target of the app, e.g. development, staging, production.
# http://symfony.com/doc/current/best_practices/configuration.html#infrastructure-related-configuration
parameters:
    database_host:     127.0.0.1
    database_port:     ~
    database_name:     symfony
    database_user:     root
    database_password: ~
    # You should uncomment this if you want use pdo_sqlite
    # database_path: "%kernel.root_dir%/data.db3"

    mailer_transport:  smtp
    mailer_host:       127.0.0.1
    mailer_user:       ~
    mailer_password:   ~

    # A secret key that's used to generate certain security-related tokens
    secret:            ThisTokenIsNotSoSecretChangeIt

    steps: homepage:
    path: /
    defaults: { _controller: TestHubBundle:Test:index }

start:
    path: /test/{testID}/start
    defaults: { _controller: TestHubBundle:Test:start }
    methods: [POST]

preface:
    path: /test/{testID}/preface
    defaults: { _controller: TestHubBundle:Test:preface }

question:
    path: /attempt/{attemptID}/question/{questionNumber}
    defaults: { _controller: TestHubBundle:Test:question }
    requirements:
        questionNumber: \d+
        attemptID: \d+

result:
    path: /attempt/{attemptID}/result
    defaults: { _controller: TestHubBundle:Test:result }

confirm:
    path: /attempt/{attemptID}/confirm
    defaults: { _controller: TestHubBundle:Test:confirm }
    - uses: _wdt:
    resource: "@WebProfilerBundle/Resources/config/routing/wdt.xml"
    prefix:   /_wdt

_profiler:
    resource: "@WebProfilerBundle/Resources/config/routing/profiler.xml"
    prefix:   /_profiler

_errors:
    resource: "@TwigBundle/Resources/config/routing/errors.xml"
    prefix:   /_error

_main:
    resource: routing.yml
    - name: # To get started with security, check out the documentation:
# http://symfony.com/doc/current/book/security.html
security:

    # http://symfony.com/doc/current/book/security.html#where-do-users-come-from-user-providers
    providers:
        in_memory:
            memory: ~

    firewalls:
        # disables authentication for assets and the profiler, adapt it according to your needs
        dev:
            pattern: ^/(_(profiler|wdt)|css|images|js)/
            security: false

        main:
            anonymous: ~
            # activate different ways to authenticate

            # http_basic: ~
            # http://symfony.com/doc/current/book/security.html#a-configuring-how-your-users-will-authenticate

            # form_login: ~
            # http://symfony.com/doc/current/cookbook/security/form_login_setup.html
      run: # Learn more about services, parameters and containers at
# http://symfony.com/doc/current/book/service_container.html
parameters:
#    parameter_name: value

services:
#    service_name:
#        class: AppBundle\Directory\ClassName
#        arguments: ["@another_service_name", "plain_value", "%parameter_name%"]
    user_manager:
        class: TestHubBundle\Service\DummyUserManager
        arguments: ["@doctrine.orm.entity_manager"]
    test_service:
        class: TestHubBundle\Service\TestService
        arguments: ["@doctrine.orm.entity_manager"]
    calculator:
        class: TestHubBundle\Service\Calculator
    app.twig_extension:
        class: TestHubBundle\Twig\AppExtension
        public: false
        tags:
            - { name: twig.extension }
    - name: {}
      run: crystal spec
