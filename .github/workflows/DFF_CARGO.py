name: Rust

on: [alias]
build-man = "run --package xtask-build-man --"
stale-label = "run --package xtask-stale-label --"
bump-check = "run --package xtask-bump-check --"
lint-docs = "run --package xtask-lint-docs --"

[env]
# HACK: Until this is stabilized, `snapbox`s polyfill could get confused
# inside of the rust-lang/rust repo because it looks for the furthest-away `Cargo.toml`
CARGO_RUSTC_CURRENT_DIR = { value = "", relative = true }
  push: - config:
    id: c_c++
    name: C / C++
    icons_html: >
      <i class="devicon-cplusplus-plain"></i>
      <i class="devicon-c-plain"></i>
  projects:
    - url: https://github.com/spotify/annoy
    - url: https://github.com/spotify/sparkey
    - url: https://github.com/spotify/spotify-json
    - url: https://github.com/spotify/pedalboard
    - url: https://github.com/spotify/voyager
- config:
    id: java
    name: Java
    icons_html: >
      <i class="devicon-java-plain-wordmark"></i>
  projects:
    - url: https://github.com/spotify/android-auth
    - url: https://github.com/spotify/android-sdk
    - url: https://github.com/spotify/completable-futures
    - url: https://github.com/spotify/dbeam
    - url: https://github.com/spotify/folsom
    - url: https://github.com/spotify/futures-extra
    - url: https://github.com/spotify/missinglink
    - url: https://github.com/spotify/mobius
    - url: https://github.com/spotify/zoltar
    - url: https://github.com/spotify/github-java-client
    - url: https://github.com/spotify/dataenum
    - url: https://github.com/spotify/futuristic-feline
    - url: https://github.com/spotify/logging-java
    - url: https://github.com/spotify/java-hamcrest
    - url: https://github.com/spotify/fmt-maven-plugin
    - url: https://github.com/spotify/dns-java
    - url: https://github.com/spotify/semantic-metrics
    - url: https://github.com/spotify/Vandelay
- config:
    id: javascript
    name: JavaScript
    icons_html: >
      <i class="devicon-javascript-plain"></i>
  projects:
    - url: https://github.com/backstage/backstage
    - url: https://github.com/spotify/reactochart
    - url: https://github.com/spotify/web-scripts
    - url: https://github.com/spotify/should-up
    - url: https://github.com/spotify/spotify-web-api-ts-sdk
    - url: https://github.com/spotify/confidence-openfeature-provider-js
    - url: https://github.com/spotify/lighthouse-audit-service
    - url: https://github.com/spotify/basic-pitch-ts
    - url: https://github.com/spotify/soa-reference-integration
    - url: https://github.com/spotify/web-api-examples
- config:
    id: kotlin
    name: Kotlin
    icons_html: >
      <i class="devicon-kotlin-plain"></i>
  projects:
    - url: https://github.com/spotify/ruler
    - url: https://github.com/spotify/confidence-openfeature-provider-kotlin
- config:
    id: objective-c
    name: Objective-C
    icons_html: >
      <i class="devicon-objectivec-plain"></i>
  projects:
    - url: https://github.com/spotify/ios-style
    - url: https://github.com/spotify/ios-sdk
    - url: https://github.com/spotify/ios-auth
    - url: https://github.com/spotify/SPTDataLoader
    - url: https://github.com/spotify/SPTPersistentCache
- config:
    id: python
    name: Python
    icons_html: >
      <i class="devicon-python-plain alt-colored"></i>
  projects:
    - url: https://github.com/spotify/annoy
    - url: https://github.com/spotify/basic-pitch
    - url: https://github.com/spotify/confidence
    - url: https://github.com/spotify/realbook
    - url: https://github.com/spotify/ulogger
    - url: https://github.com/spotify/cstar
    - url: https://github.com/spotify/dh-virtualenv
    - url: https://github.com/spotify/luigi
    - url: https://github.com/spotify/pythonflow
    - url: https://github.com/spotify/spotify-tensorflow
    - url: https://github.com/spotify/chartify
    - url: https://github.com/spotify/klio
    - url: https://github.com/spotify/pedalboard
    - url: https://github.com/spotify/postgresql-metrics
    - url: https://github.com/spotify/sparkey-python
- config:
    id: ruby
    name: Ruby
    icons_html: >
      <i class="devicon-ruby-plain"></i>
  projects:
    - url: https://github.com/spotify/homebrew-public
    - url: https://github.com/spotify/talos
- config:
    id: scala
    name: Scala
    icons_html: >
      <i class="devicon-scala-plain"></i>
  projects:
    - url: https://github.com/spotify/big-data-rosetta-code
    - url: https://github.com/spotify/featran
    - url: https://github.com/spotify/noether
    - url: https://github.com/spotify/ratatool
    - url: https://github.com/spotify/scio
    - url: https://github.com/spotify/magnolify
    - url: https://github.com/spotify/tfreader
    - url: https://github.com/spotify/scio-idea-plugin
    - url: https://github.com/spotify/gcs-tools
    - url: https://github.com/spotify/elitzur
    - url: https://github.com/spotify/scio.g8
- config:
    id: go
    name: Go
    icons_html: >
      <i class="devicon-go-original-wordmark"></i>
  projects:
    - url: https://github.com/spotify/flyte-flink-plugin
    - url: https://github.com/spotify/flink-on-k8s-operator
    - url: https://github.com/spotify/confidence-openfeature-provider-go
- config:
    id: swift
    name: Swift
    icons_html: >
      <i class="devicon-swift-plain"></i>
  projects:
    - url: https://github.com/spotify/Mobius.swift
    - url: https://github.com/spotify/XCRemoteCache
    - url: https://github.com/spotify/XCMetrics
    - url: https://github.com/spotify/confidence-openfeature-provider-swift
    - url: https://github.com/spotify/diffuser

- config:
    id: other
    name: Other
  projects:
    - url: https://github.com/spotify/git-test
    - url: https://github.com/spotify/cstar
    - url: https://github.com/spotify/dockerfile-mode
    - url: https://github.com/spotify/foss-root
    branches: [ "main" ]
pull_request: - config:
    id: c_c++
    name: C / C++
    icons_html: |
      <i class="devicon-cplusplus-plain"></i> <i class="devicon-c-plain"></i>
  projects:
    - name: annoy
      description: Approximate Nearest Neighbors in C++/Python optimized for memory
        usage and loading/saving to disk
      stargazerCount: 13101
      forkCount: 1156
      url: https://github.com/spotify/annoy
      openIssues:
        totalCount: 55
      watchers:
        totalCount: 319
    - name: sparkey
      description: Simple constant key/value storage library, for read-heavy systems
        with infrequent large bulk inserts.
      stargazerCount: 1160
      forkCount: 80
      url: https://github.com/spotify/sparkey
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 144
    - name: spotify-json
      description: Fast and nice to use C++ JSON library.
      stargazerCount: 194
      forkCount: 40
      url: https://github.com/spotify/spotify-json
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 20
    - name: pedalboard
      description: 🎛 🔊 A Python library for audio.
      stargazerCount: 5127
      forkCount: 259
      url: https://github.com/spotify/pedalboard
      openIssues:
        totalCount: 54
      watchers:
        totalCount: 55
    - name: voyager
      description: 🛰️ An approximate nearest-neighbor search library for Python and
        Java with a focus on ease of use, simplicity, and deployability.
      stargazerCount: 1271
      forkCount: 52
      url: https://github.com/spotify/voyager
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 13
- config:
    id: java
    name: Java
    icons_html: |
      <i class="devicon-java-plain-wordmark"></i>
  projects:
    - name: android-auth
      description: Spotify authentication and authorization for Android. Part of the
        Spotify Android SDK.
      stargazerCount: 143
      forkCount: 66
      url: https://github.com/spotify/android-auth
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 32
    - name: android-sdk
      description: Spotify SDK for Android
      stargazerCount: 459
      forkCount: 119
      url: https://github.com/spotify/android-sdk
      openIssues:
        totalCount: 153
      watchers:
        totalCount: 59
    - name: completable-futures
      description: Utilities for working with futures in Java 8
      stargazerCount: 389
      forkCount: 51
      url: https://github.com/spotify/completable-futures
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 101
    - name: dbeam
      description: DBeam exports SQL tables into Avro files using JDBC and Apache Beam
      stargazerCount: 189
      forkCount: 56
      url: https://github.com/spotify/dbeam
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 19
    - name: folsom
      description: An asynchronous memcache client for Java
      stargazerCount: 147
      forkCount: 67
      url: https://github.com/spotify/folsom
      openIssues:
        totalCount: 11
      watchers:
        totalCount: 94
    - name: futures-extra
      description: Java library for working with Guava futures
      stargazerCount: 137
      forkCount: 32
      url: https://github.com/spotify/futures-extra
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 25
    - name: missinglink
      description: Build time tool for detecting link problems in java projects
      stargazerCount: 144
      forkCount: 27
      url: https://github.com/spotify/missinglink
      openIssues:
        totalCount: 16
      watchers:
        totalCount: 96
    - name: mobius
      description: A functional reactive framework for managing state evolution and
        side-effects.
      stargazerCount: 1221
      forkCount: 97
      url: https://github.com/spotify/mobius
      openIssues:
        totalCount: 3
      watchers:
        totalCount: 49
    - name: zoltar
      description: Common library for serving TensorFlow, XGBoost and scikit-learn
        models in production.
      stargazerCount: 139
      forkCount: 33
      url: https://github.com/spotify/zoltar
      openIssues:
        totalCount: 17
      watchers:
        totalCount: 24
    - name: github-java-client
      description: A Java client to Github API
      stargazerCount: 129
      forkCount: 82
      url: https://github.com/spotify/github-java-client
      openIssues:
        totalCount: 16
      watchers:
        totalCount: 21
    - name: dataenum
      description: Algebraic data types in Java.
      stargazerCount: 164
      forkCount: 16
      url: https://github.com/spotify/dataenum
      openIssues:
        totalCount: 3
      watchers:
        totalCount: 16
    - name: futuristic-feline
      description: futuristic-feline is a library for detecting blocking Java futures
        at runtime
      stargazerCount: 11
      forkCount: 9
      url: https://github.com/spotify/futuristic-feline
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: logging-java
      description: Utility classes to set up logging in a Spotify compatible way
      stargazerCount: 22
      forkCount: 23
      url: https://github.com/spotify/logging-java
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 98
    - name: java-hamcrest
      description: Libraries that extend the Hamcrest matching library with useful matchers
      stargazerCount: 82
      forkCount: 16
      url: https://github.com/spotify/java-hamcrest
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 20
    - name: fmt-maven-plugin
      description: Opinionated Maven Plugin that formats your Java code.
      stargazerCount: 326
      forkCount: 62
      url: https://github.com/spotify/fmt-maven-plugin
      openIssues:
        totalCount: 11
      watchers:
        totalCount: 12
    - name: dns-java
      description: DNS wrapper library that provides SRV lookup functionality
      stargazerCount: 205
      forkCount: 47
      url: https://github.com/spotify/dns-java
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 106
    - name: semantic-metrics
      description: Capturing meaningful metrics in your Java application
      stargazerCount: 66
      forkCount: 36
      url: https://github.com/spotify/semantic-metrics
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 20
    - name: Vandelay
      description: A small library for importing/exporting BigTable instance schemas
        and row data.
      stargazerCount: 10
      forkCount: 1
      url: https://github.com/spotify/Vandelay
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 9
- config:
    id: javascript
    name: JavaScript
    icons_html: |
      <i class="devicon-javascript-plain"></i>
  projects:
    - name: backstage
      description: Backstage is an open framework for building developer portals
      stargazerCount: 27552
      forkCount: 5840
      url: https://github.com/backstage/backstage
      openIssues:
        totalCount: 343
      watchers:
        totalCount: 223
    - name: reactochart
      description: ":chart_with_upwards_trend: React chart component library
        :chart_with_downwards_trend:"
      stargazerCount: 552
      forkCount: 54
      url: https://github.com/spotify/reactochart
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 20
    - name: web-scripts
      description: "DEPRECATED: A collection of base configs and CLI wrappers used to
        speed up development @ Spotify."
      stargazerCount: 383
      forkCount: 62
      url: https://github.com/spotify/web-scripts
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 6
    - name: should-up
      description: Remove most of the "should" noise from your tests
      stargazerCount: 155
      forkCount: 23
      url: https://github.com/spotify/should-up
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 12
    - name: spotify-web-api-ts-sdk
      description: A Typescript SDK for the Spotify Web API with types for returned data.
      stargazerCount: 355
      forkCount: 69
      url: https://github.com/spotify/spotify-web-api-ts-sdk
      openIssues:
        totalCount: 29
      watchers:
        totalCount: 10
    - name: confidence-sdk-js
      description: null
      stargazerCount: 9
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-js
      openIssues:
        totalCount: 6
      watchers:
        totalCount: 10
    - name: lighthouse-audit-service
      description: null
      stargazerCount: 95
      forkCount: 30
      url: https://github.com/spotify/lighthouse-audit-service
      openIssues:
        totalCount: 17
      watchers:
        totalCount: 19
    - name: basic-pitch-ts
      description: A lightweight yet powerful audio-to-MIDI converter with pitch bend
        detection.
      stargazerCount: 228
      forkCount: 18
      url: https://github.com/spotify/basic-pitch-ts
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 20
    - name: soa-reference-integration
      description: SOA Reference Integration
      stargazerCount: 11
      forkCount: 2
      url: https://github.com/spotify/soa-reference-integration
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 9
    - name: web-api-examples
      description: Basic examples to authenticate and fetch data using the Spotify Web API
      stargazerCount: 1980
      forkCount: 1654
      url: https://github.com/spotify/web-api-examples
      openIssues:
        totalCount: 50
      watchers:
        totalCount: 68
- config:
    id: kotlin
    name: Kotlin
    icons_html: |
      <i class="devicon-kotlin-plain"></i>
  projects:
    - name: ruler
      description: Gradle plugin which helps you analyze the size of your Android apps.
      stargazerCount: 1129
      forkCount: 58
      url: https://github.com/spotify/ruler
      openIssues:
        totalCount: 13
      watchers:
        totalCount: 25
    - name: confidence-sdk-android
      description: null
      stargazerCount: 8
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-android
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 10
- config:
    id: objective-c
    name: Objective-C
    icons_html: |
      <i class="devicon-objectivec-plain"></i>
  projects:
    - name: ios-style
      description: Guidelines for iOS development in use at Spotify
      stargazerCount: 242
      forkCount: 36
      url: https://github.com/spotify/ios-style
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 130
    - name: ios-sdk
      description: Spotify SDK for iOS
      stargazerCount: 653
      forkCount: 186
      url: https://github.com/spotify/ios-sdk
      openIssues:
        totalCount: 205
      watchers:
        totalCount: 58
    - name: ios-auth
      description: Spotify authentication and authorization for iOS.
      stargazerCount: 10
      forkCount: 3
      url: https://github.com/spotify/ios-auth
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 4
    - name: SPTDataLoader
      description: The HTTP library used by the Spotify iOS client
      stargazerCount: 629
      forkCount: 74
      url: https://github.com/spotify/SPTDataLoader
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 114
    - name: SPTPersistentCache
      description: Everyone tries to implement a cache at some point in their iOS
        app’s lifecycle, and this is ours.
      stargazerCount: 1245
      forkCount: 78
      url: https://github.com/spotify/SPTPersistentCache
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 56
- config:
    id: python
    name: Python
    icons_html: |
      <i class="devicon-python-plain alt-colored"></i>
  projects:
    - name: annoy
      description: Approximate Nearest Neighbors in C++/Python optimized for memory
        usage and loading/saving to disk
      stargazerCount: 13101
      forkCount: 1156
      url: https://github.com/spotify/annoy
      openIssues:
        totalCount: 55
      watchers:
        totalCount: 319
    - name: basic-pitch
      description: A lightweight yet powerful audio-to-MIDI converter with pitch bend
        detection
      stargazerCount: 3338
      forkCount: 257
      url: https://github.com/spotify/basic-pitch
      openIssues:
        totalCount: 24
      watchers:
        totalCount: 49
    - name: confidence
      description: null
      stargazerCount: 255
      forkCount: 32
      url: https://github.com/spotify/confidence
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 15
    - name: realbook
      description: Easier audio-based machine learning with TensorFlow.
      stargazerCount: 112
      forkCount: 5
      url: https://github.com/spotify/realbook
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 8
    - name: ulogger
      description: A simple micrologging framework
      stargazerCount: 15
      forkCount: 8
      url: https://github.com/spotify/ulogger
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: cstar
      description: Apache Cassandra cluster orchestration tool for the command line
      stargazerCount: 254
      forkCount: 37
      url: https://github.com/spotify/cstar
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 22
    - name: dh-virtualenv
      description: Python virtualenvs in Debian packages
      stargazerCount: 1612
      forkCount: 186
      url: https://github.com/spotify/dh-virtualenv
      openIssues:
        totalCount: 35
      watchers:
        totalCount: 41
    - name: luigi
      description: "Luigi is a Python module that helps you build complex pipelines of
        batch jobs. It handles dependency resolution, workflow management,
        visualization etc. It also comes with Hadoop support built in. "
      stargazerCount: 17705
      forkCount: 2391
      url: https://github.com/spotify/luigi
      openIssues:
        totalCount: 105
      watchers:
        totalCount: 474
    - name: pythonflow
      description: ":snake: Dataflow programming for python."
      stargazerCount: 284
      forkCount: 48
      url: https://github.com/spotify/pythonflow
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 74
    - name: spotify-tensorflow
      description: Provides Spotify-specific TensorFlow helpers
      stargazerCount: 124
      forkCount: 25
      url: https://github.com/spotify/spotify-tensorflow
      openIssues:
        totalCount: 12
      watchers:
        totalCount: 27
    - name: chartify
      description: Python library that makes it easy for data scientists to create charts.
      stargazerCount: 3519
      forkCount: 324
      url: https://github.com/spotify/chartify
      openIssues:
        totalCount: 43
      watchers:
        totalCount: 86
    - name: klio
      description: Smarter data pipelines for audio.
      stargazerCount: 835
      forkCount: 48
      url: https://github.com/spotify/klio
      openIssues:
        totalCount: 5
      watchers:
        totalCount: 20
    - name: pedalboard
      description: 🎛 🔊 A Python library for audio.
      stargazerCount: 5127
      forkCount: 259
      url: https://github.com/spotify/pedalboard
      openIssues:
        totalCount: 54
      watchers:
        totalCount: 55
    - name: postgresql-metrics
      description: Tool that extracts and provides metrics on your PostgreSQL database
      stargazerCount: 588
      forkCount: 51
      url: https://github.com/spotify/postgresql-metrics
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 113
    - name: sparkey-python
      description: Python bindings for the sparkey key value store
      stargazerCount: 38
      forkCount: 7
      url: https://github.com/spotify/sparkey-python
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 10
- config:
    id: ruby
    name: Ruby
    icons_html: |
      <i class="devicon-ruby-plain"></i>
  projects:
    - name: homebrew-public
      description: Homebrew formula for open-source software developed by Spotify
      stargazerCount: 55
      forkCount: 27
      url: https://github.com/spotify/homebrew-public
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 36
    - name: talos
      description: Hiera secrets distribution over HTTP
      stargazerCount: 26
      forkCount: 6
      url: https://github.com/spotify/talos
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 14
- config:
    id: scala
    name: Scala
    icons_html: |
      <i class="devicon-scala-plain"></i>
  projects:
    - name: big-data-rosetta-code
      description: Code snippets for solving common big data problems in various
        platforms. Inspired by Rosetta Code
      stargazerCount: 290
      forkCount: 33
      url: https://github.com/spotify/big-data-rosetta-code
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 29
    - name: featran
      description: A Scala feature transformation library for data science and machine
        learning
      stargazerCount: 466
      forkCount: 68
      url: https://github.com/spotify/featran
      openIssues:
        totalCount: 10
      watchers:
        totalCount: 29
    - name: noether
      description: Scala Aggregators used for ML Model metrics monitoring
      stargazerCount: 91
      forkCount: 16
      url: https://github.com/spotify/noether
      openIssues:
        totalCount: 4
      watchers:
        totalCount: 22
    - name: ratatool
      description: A tool for data sampling, data generation, and data diffing
      stargazerCount: 341
      forkCount: 55
      url: https://github.com/spotify/ratatool
      openIssues:
        totalCount: 18
      watchers:
        totalCount: 27
    - name: scio
      description: A Scala API for Apache Beam and Google Cloud Dataflow.
      stargazerCount: 2550
      forkCount: 513
      url: https://github.com/spotify/scio
      openIssues:
        totalCount: 119
      watchers:
        totalCount: 117
    - name: magnolify
      description: A collection of Magnolia add-on modules
      stargazerCount: 165
      forkCount: 26
      url: https://github.com/spotify/magnolify
      openIssues:
        totalCount: 19
      watchers:
        totalCount: 21
    - name: tfreader
      description: TensorFlow TFRecord reader CLI tool
      stargazerCount: 61
      forkCount: 16
      url: https://github.com/spotify/tfreader
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 18
    - name: scio-idea-plugin
      description: Scio IDEA plugin
      stargazerCount: 30
      forkCount: 18
      url: https://github.com/spotify/scio-idea-plugin
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 24
    - name: gcs-tools
      description: GCS support for avro-tools, parquet-tools and protobuf
      stargazerCount: 73
      forkCount: 15
      url: https://github.com/spotify/gcs-tools
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 17
    - name: elitzur
      description: null
      stargazerCount: 22
      forkCount: 20
      url: https://github.com/spotify/elitzur
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 15
    - name: scio.g8
      description: A Giter8 template for scio
      stargazerCount: 29
      forkCount: 20
      url: https://github.com/spotify/scio.g8
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 20
- config:
    id: go
    name: Go
    icons_html: |
      <i class="devicon-go-original-wordmark"></i>
  projects:
    - name: flyte-flink-plugin
      description: Flyte Flink k8s plugin.
      stargazerCount: 18
      forkCount: 12
      url: https://github.com/spotify/flyte-flink-plugin
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 19
    - name: flink-on-k8s-operator
      description: Kubernetes operator for managing the lifecycle of Apache Flink and
        Beam applications.
      stargazerCount: 188
      forkCount: 68
      url: https://github.com/spotify/flink-on-k8s-operator
      openIssues:
        totalCount: 32
      watchers:
        totalCount: 11
    - name: confidence-sdk-go
      description: null
      stargazerCount: 2
      forkCount: 1
      url: https://github.com/spotify/confidence-sdk-go
      openIssues:
        totalCount: 2
      watchers:
        totalCount: 12
- config:
    id: swift
    name: Swift
    icons_html: |
      <i class="devicon-swift-plain"></i>
  projects:
    - name: Mobius.swift
      description: A functional reactive framework for managing state evolution and
        side-effects [Swift implementation]
      stargazerCount: 556
      forkCount: 42
      url: https://github.com/spotify/Mobius.swift
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 27
    - name: XCRemoteCache
      description: null
      stargazerCount: 833
      forkCount: 52
      url: https://github.com/spotify/XCRemoteCache
      openIssues:
        totalCount: 24
      watchers:
        totalCount: 25
    - name: XCMetrics
      description: XCMetrics is the easiest way to collect Xcode build metrics and
        improve developer productivity.
      stargazerCount: 1101
      forkCount: 78
      url: https://github.com/spotify/XCMetrics
      openIssues:
        totalCount: 14
      watchers:
        totalCount: 25
    - name: confidence-sdk-swift
      description: null
      stargazerCount: 18
      forkCount: 3
      url: https://github.com/spotify/confidence-sdk-swift
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 11
    - name: diffuser
      description: Declaratively define relationships between data and side effects
      stargazerCount: 45
      forkCount: 13
      url: https://github.com/spotify/diffuser
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 14
- config:
    id: other
    name: Other
  projects:
    - name: git-test
      description: test your commits
      stargazerCount: 203
      forkCount: 41
      url: https://github.com/spotify/git-test
      openIssues:
        totalCount: 1
      watchers:
        totalCount: 111
    - name: cstar
      description: Apache Cassandra cluster orchestration tool for the command line
      stargazerCount: 254
      forkCount: 37
      url: https://github.com/spotify/cstar
      openIssues:
        totalCount: 8
      watchers:
        totalCount: 22
    - name: dockerfile-mode
      description: An emacs mode for handling Dockerfiles
      stargazerCount: 535
      forkCount: 79
      url: https://github.com/spotify/dockerfile-mode
      openIssues:
        totalCount: 9
      watchers:
        totalCount: 20
    - name: foss-root
      description: Maven POM that serves as a parent for Spotify FOSS projects
      stargazerCount: 1
      forkCount: 7
      url: https://github.com/spotify/foss-root
      openIssues:
        totalCount: 0
      watchers:
        totalCount: 10
    branches: [ "main" ]

env: {
  // See https://go.microsoft.com/fwlink/?LinkId=827846 to learn about workspace recommendations.
  // Extension identifier format: ${publisher}.${name}. Example: vscode.csharp
  // List of extensions which should be recommended for users of this workspace.
  "recommendations": [
    "dbaeumer.vscode-eslint",
    "rust-lang.rust-analyzer",
    "bradlc.vscode-tailwindcss",
    "tauri-apps.tauri-vscode",
    "esbenp.prettier-vscode"
  ],
  // List of extensions recommended by VS Code that should not be recommended for users of this workspace.
  "unwantedRecommendations": []
}
  CARGO_TERM_COLOR: CC := gcc
ifeq ($(USE_GPU),1)
CUCC := nvcc
endif

# Select backend files based on selected backend
# Supported values: naive, onednn
BACKEND ?= onednn

# Set to 1 to use accelerated matrix products when using naive backend
USE_AVX ?= 0

# The root directory of the oneDNN library, only needed when using
# onednn backend
ONEDNN_ROOT_DIR ?= lib/onednn
ONEDNN_INCLUDE_DIR := $(ONEDNN_ROOT_DIR)/include
ONEDNN_SHARED_DIR := $(ONEDNN_ROOT_DIR)/lib/

# Can be set in case the directory where libcudart.so is located is not
# in the default directories
CUDA_LIB_DIR ?=

# Select log level
# Supported values: 1(error), 2(warn), 3(info), 4(trace)
LOG_LEVEL ?= 3

# Set to 1 to enable gdb support
DEBUG ?= 0


ifeq ($(DEBUG),1)
ifeq ($(USE_AVX),1)
$(error Can not have DEBUG=1 and USE_AVX=1 at the same time)
endif
endif


CFLAGS :=
CUFLAGS :=
ifdef LOG_LEVEL
CFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
CUFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
endif
ifeq ($(USE_AVX),1)
CFLAGS += -march=haswell -DUSE_AVX
endif
ifeq ($(USE_GPU),1)
CFLAGS += -DUSE_GPU
CUFLAGS += -DUSE_GPU
endif
ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
CUFLAGS += -g -DDEBUG
else
CFLAGS += -O3 -Ofast
CUFLAGS += -O3
endif


# math library
LDFLAGS := -lm


SOURCEDIR := src

# INCLUDE and SOURCE file located in the src directory
INCLUDE := -I$(SOURCEDIR)/lib -I$(SOURCEDIR)/common
SRC := $(shell find $(SOURCEDIR)/common -name '*.c')
SRC += $(SOURCEDIR)/lib/log.c $(SOURCEDIR)/lib/config_info.c $(SOURCEDIR)/lib/random.c
# Also add the target source file
SRC += $(TARGET).c


# Select backend files based on selected backend
ifeq ($(BACKEND),naive)
INCLUDE += -I$(SOURCEDIR)/naive -I$(SOURCEDIR)/include
SRC += $(shell find $(SOURCEDIR)/naive -name '*.c')
ifeq ($(USE_GPU),1)
SRC += $(shell find $(SOURCEDIR)/naive -name '*.cu')
ifneq ($(CUDA_LIB_DIR),)
LDFLAGS += -L$(CUDA_LIB_DIR)
endif
LDFLAGS += -lcudart
endif
CFLAGS += -DBACKEND_NAIVE
CUFLAGS += -DBACKEND_NAIVE
else ifeq ($(BACKEND),onednn)
INCLUDE += -I$(SOURCEDIR)/onednn -I$(ONEDNN_INCLUDE_DIR)
SRC += $(shell find $(SOURCEDIR)/onednn -name '*.c')
LDFLAGS += -L$(ONEDNN_SHARED_DIR) -ldnnl 
CFLAGS += -DBACKEND_ONEDNN
else
$(error Only naive and onednn implementation available.)
endif


# Object files are placed in same directory as src files, just with different file extension
OBJ := $(SRC:.c=.o)
ifeq ($(USE_GPU),1)
OBJ := $(OBJ:.cu=.o)
endif

jobs: include config/defines.mk


# Link all object files into a source file
$(TARGET): $(OBJ)
        $(CC) $^ -o $@ $(LDFLAGS)


# Rule to compile a single translation unit
%.o: %.c
        $(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

# Rule to compile a single cuda translation unit
ifeq ($(USE_GPU),1)
%.o: %.cu
        $(CUCC) $(INCLUDE) $(CUFLAGS) -c $< -o $@
endif


clean:
        @$(RM) -rv $(TARGET) $(OBJ)


rebuild:
        make clean && make


run: $(TARGET)
# since oneDNN is built as a shared library, need to add its location
# to LD_LIBRARY_PATH so that the target executable can find it
ifeq ($(BACKEND),onednn)
run: export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(ONEDNN_SHARED_DIR)
endif
run:
        @$(TARGET)


.PHONY: clean rebuild run
  build: /**
 * @file lenet5_mnist.c
 * @brief Training a CNN on Fashion MNIST
 * 
 * Training a CNN on Fashion MNIST.
 */


#include <inttypes.h>
#include <math.h>
#include <stdio.h>


#include "core/layer.h"
#include "core/loss.h"
#include "core/optimizer.h"

#include "optimizer/adam.h"
#include "optimizer/rmsprop.h"
#include "optimizer/sgd.h"

#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"

#include "dataset/dataset.h"
#include "dataset/dataset_utils.h"
#include "dataset/mnist.h"

#include "augment/augment_pipeline.h"
#include "augment/image_flip.h"
#include "augment/random_crop.h"
#include "augment/color_augment.h"

#include "util/training_utils.h"

#include "config_info.h"
#include "log.h"
#include "tensor.h"


/* set to location of mnist or fashion_mnist root folder */
static const char* mnist_path = "datasets/fashion_mnist";

static const size_t batch_size = 32;
static const size_t num_epochs = 10000;
static const size_t test_every = 5;

/* learning rate schedule parameters */
static const float initial_lr = 0.2f;
static const float final_lr = 0.01f;
static const size_t decay_begin = 0;
static const size_t decay_end = 20;
static const int32_t reduce_lr_after = 7;
static const float reduce_lr_by_factor = 10.0f;

static const float dropout_rate = 0.25f;

/* augmentation config */
static const bool augment = true;
static const size_t padding = 2;

/* optimizer config */
static const optimizer_impl_t* optimizer = &sgd_optimizer;
static const sgd_config_t optimizer_config = {
    .learning_rate = 0.0f, /* overwritten by lr_schedule */
    .weight_reg_kind = SGD_WEIGHT_REG_L2,
    .weight_reg_strength = 1e-4f,
};

/* conv -> batchnorm -> relu -> conv -> batchnorm -> relu -> pool */
void model_desc_add_conv_block(model_desc_t* desc, size_t out_channels, size_t filter_size)
{
    static const float bn_momentum = 0.9f;
    static const float bn_eps = 1e-8f;

    const size_t padding = filter_size / 2;

    model_desc_add_convolutional_layer(desc, out_channels, filter_size, 1, padding, winit_he, winit_zeros);
    model_desc_add_batch_norm_layer(desc, bn_momentum, bn_eps);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);

    model_desc_add_convolutional_layer(desc, out_channels, filter_size, 1, padding, winit_he, winit_zeros);
    model_desc_add_batch_norm_layer(desc, bn_momentum, bn_eps);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);

    model_desc_add_pooling_layer(desc, 2, 1, 0, POOLING_MAX);
}

/* linear -> relu -> ?dropout */
void model_desc_add_linear_dropout(model_desc_t* desc, size_t out_channels, float dropout_rate)
{
    model_desc_add_linear_layer(desc, out_channels, winit_he, winit_zeros);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_RELU);
    if (dropout_rate > 0.0f) {
        model_desc_add_dropout_layer(desc, dropout_rate);
    }
}


/* crop1: 93.51% */
layer_t create_small_cnn(const tensor_shape_t* input_shape, float dropout_rate, size_t batch_size)
{
    layer_t model = NULL;
    model_desc_t* desc = NULL;

    model_desc_create(&desc);

    model_desc_add_conv_block(desc, 64, 3);
    model_desc_add_conv_block(desc, 64, 3);
    model_desc_add_linear_dropout(desc, 512, dropout_rate);
    model_desc_add_linear_dropout(desc, 128, dropout_rate);
    model_desc_add_linear_layer(desc, 10, winit_he, winit_zeros);

    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, batch_size);

    model_desc_destroy(desc);
    return model;
}


/* crop: 2, dropout: 0.5 - 93.60%, dropout: 0.25 - 94.10% */
layer_t create_cnn(const tensor_shape_t* input_shape, float dropout_rate, size_t batch_size)
{
    layer_t model = NULL;
    model_desc_t* desc = NULL;

    model_desc_create(&desc);

    model_desc_add_conv_block(desc, 128, 3);
    model_desc_add_conv_block(desc, 128, 3);
    model_desc_add_linear_dropout(desc, 1024, dropout_rate);
    model_desc_add_linear_dropout(desc, 256, dropout_rate);
    model_desc_add_linear_layer(desc, 10, winit_he, winit_zeros);


    /* Print a model overview to stdout. */
    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, batch_size);

    model_desc_destroy(desc);
    return model;
}


augment_pipeline_t setup_augment_pipeline()
{
    const image_flip_config_t flip_config = {
        .horizontal_flip_prob = 0.5f,
        .vertical_flip_prob = 0.0f,
    };

    const random_crop_config_t crop_config = {
        .padding = padding,
    };

    const color_augment_config_t color_config = {
        .brightness_augment_prob = 1.0f,
        .brightness_std = 0.1f,
        .contrast_augment_prob = 1.0f,
        .contrast_std = 0.1f,
        .contrast_midpoint = 0.0f /* since images are normalized */
    };


    const augment_pipeline_config_entry_t pipeline_entries[] = {
        { .impl = &aug_image_flip, .config = &flip_config },
        { .impl = &aug_random_crop, .config = &crop_config },
        //{ .impl = &aug_color, .config = &color_config }
    };

    const augment_pipeline_config_t pipeline_config = {
        .entries = pipeline_entries,
        .num_entries = sizeof(pipeline_entries) / sizeof(*pipeline_entries),
    };

    augment_pipeline_t augment_pipeline = NULL;
    augment_pipeline_create(&augment_pipeline, &pipeline_config);
    return augment_pipeline;
}


static dataset_t train_set = NULL, test_set = NULL;
static Loss loss;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    if (state->epoch % test_every == 0) {    
        module_test(state->model, test_set, batch_size, &loss, &test_accuracy, &test_loss);
        printf("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.2f%% | Test loss %f "
            "| Test accuracy %5.2f%% | lr %.2e\n",
            state->epoch,
            state->train_loss,
            state->train_accuracy * 100.0f,
            test_loss,
            test_accuracy * 100.0f,
            optimizer_get_learning_rate(state->optimizer)
        );
    }
}


float linear_lr_schedule(const training_state_t* state)
{
    static float min_train_loss = INFINITY;
    static int32_t min_train_loss_epoch = 0;

    if (state->train_loss < min_train_loss) {
        min_train_loss = state->train_loss;
        min_train_loss_epoch = state->epoch;
    }

    if (state->epoch < decay_begin) {
        return initial_lr;
    } else if (state->epoch <= decay_end) {
        return final_lr + (initial_lr - final_lr) * (decay_end - state->epoch)
            / (decay_end - decay_begin);
    } else {
        if (reduce_lr_after != 0 && state->epoch - min_train_loss_epoch >= reduce_lr_after) {
            min_train_loss = state->train_loss;
            min_train_loss_epoch = state->epoch;
            return optimizer_get_learning_rate(state->optimizer) / reduce_lr_by_factor;
        } else {
            return optimizer_get_learning_rate(state->optimizer);
        }
    }
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };

    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    /* initialize augmentation pipeline */
    augment_pipeline_t augment_pipeline = augment ? setup_augment_pipeline() : NULL;
    if (augment && augment_pipeline == NULL) {
        LOG_ERROR("There was an error setting up the augmentation pipeline\n");
        return 1;
    }
    LOG_INFO("Successfully set up the augmentation pipeline\n");


    /* create the model */
    const tensor_shape_t* data_shape = dataset_get_shape(train_set);
    layer_t model = create_cnn(data_shape, dropout_rate, batch_size);
    size_t num_params = module_get_num_params(model);
    LOG_INFO("Created the model. #params = %zu. Training starts now\n", num_params);


    /* initialize loss */
    LossInit(&loss, layer_get_output_shape(model), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);

    /* train */
    module_train(model, train_set, augment_pipeline, num_epochs, batch_size, optimizer,
        &optimizer_config, linear_lr_schedule, &loss, train_callback);


    /* free resources */
    layer_destroy(model);
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    if (augment_pipeline != NULL) {
        augment_pipeline_destroy(augment_pipeline);
    }
    LossDeinit(&loss);

    return 0;
}

    runs-on: /**
 * @file lenet5_mnist.c
 * @brief Train LeNet-5 on the MNIST dataset
 * 
 * This example illustrates how this library can be used to implement the LeNet-5 architecture
 * and train it on the MNIST dataset.
 */

#include <inttypes.h>
#include <math.h>

#include "neuralnet.h"
#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"
#include "dataset/mnist.h"
#include "optimizer/sgd.h"

/* path to MNIST or Fashion MNIST dataset */
const char* mnist_path = "datasets/mnist";

#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif

/* When training on mnist with this configuration, the model should reach an accuracy of 90%+
    after one epoch and an accuracy of ~98% after 10 epochs */
static const size_t num_epochs = 50;
static const size_t batch_size = 32;
static const sgd_config_t optimizer_config = {
    .learning_rate = 1e-2f,
    .weight_reg_kind = WEIGHT_REG_NONE,
};


layer_t create_lenet5(const tensor_shape_t* input_shape, size_t batch_size)
{
    model_desc_t* desc = NULL;
    layer_t model = NULL;

    /* Some default configurations */
    const activation_layer_create_info_t act_config = {
        .activation_function = ACTIVATION_FUNCTION_TANH,
    };
    const pooling_layer_create_info_t pool_config = {
        .kernel_width = 2,
        .pooling_operation = POOLING_AVERAGE,
    };
    const linear_layer_create_info_t linear_default_config = {
        .weight_init = winit_xavier,
        .bias_init = winit_zeros,
    };

    model_desc_create(&desc);

    convolutional_layer_create_info_t conv1_config = conv_default_config;
    conv1_config.output_channels = 6;
    conv1_config.filter_height = 5;
    conv1_config.filter_width = 5;
    conv1_config.padding_y = 2;
    conv1_config.padding_x = 2;
    model_desc_add_layer(desc, &convolutional_layer_impl, &conv1_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);
    model_desc_add_layer(desc, &pooling_layer_impl, &pool_config);

    convolutional_layer_create_info_t conv2_config = conv_default_config;
    conv2_config.output_channels = 16;
    conv2_config.filter_height = 5;
    conv2_config.filter_width = 5;
    model_desc_add_layer(desc, &convolutional_layer_impl, &conv2_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);
    model_desc_add_layer(desc, &pooling_layer_impl, &pool_config);

    linear_layer_create_info_t linear1_config = linear_default_config;
    linear1_config.output_size = 120;
    model_desc_add_layer(desc, &linear_layer_impl, &linear1_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);

    linear_layer_create_info_t linear2_config = linear_default_config;
    linear2_config.output_size = 84;
    model_desc_add_layer(desc, &linear_layer_impl, &linear2_config);
    model_desc_add_layer(desc, &activation_layer_impl, &act_config);

    linear_layer_create_info_t linear3_config = linear_default_config;
    linear3_config.output_size = 10;
    model_desc_add_layer(desc, &linear_layer_impl, &linear3_config);

    model_desc_dump(desc);

    const sequential_model_create_info_t create_info = {
        .desc = desc,
        .max_batch_size = batch_size,
    };
    layer_create(&model, &sequential_model_impl, &create_info, input_shape, device, batch_size);
    model_desc_destroy(desc);

    return model;
}


dataset_t train_set = NULL, test_set = NULL;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    module_test(state->model, test_set, batch_size, state->loss, &test_accuracy, &test_loss);
    LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
        "| Test accuracy %5.3f%%\n",
        state->epoch,
        state->train_loss,
        state->train_accuracy * 100.0f,
        test_loss,
        test_accuracy * 100.0f
    );
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    layer_t lenet5 = create_lenet5(dataset_get_shape(train_set), batch_size);

    const size_t num_params = module_get_num_params(lenet5);
    LOG_INFO("Created the model. #parameters %d. Start training...\n", num_params);

    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(lenet5), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);


    module_train(lenet5, train_set, NULL, num_epochs, batch_size, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    /* Free resources */
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(lenet5);
    LossDeinit(&loss);

    return 0;
}

    steps: /**
 * @file logistic_regression.c
 * @brief Multi-class logistic regression with linear classifier
 * 
 */


#if defined(BACKEND_ONEDNN)
#error "onednn backend is not supported by this example" 
#endif


#include <inttypes.h>
#include <math.h>
#include <stddef.h>

#include "neuralnet.h"
#include "dataset/mnist.h"
#include "layer/linear_layer.h"
#include "optimizer/sgd.h"


static const char* mnist_path = "datasets/mnist";

#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif

/* config */
static const size_t num_epochs = 1000;
static const float learning_rate = 0.1f;
static const float l2_reg_strength = 0.0f; /* ridge regression */

dataset_t train_set, test_set;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    const tensor_shape_t* test_set_shape = dataset_get_shape(test_set);
    const size_t test_samples = tensor_shape_get_dim(test_set_shape, TENSOR_BATCH_DIM);

    if (state->epoch != 0) {
        module_test(state->model, test_set, test_samples, state->loss, &test_accuracy, &test_loss);
        LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
            "| Test accuracy %5.3f%%\n",
            state->epoch,
            state->train_loss,
            state->train_accuracy * 100.0f,
            test_loss,
            test_accuracy * 100.0f
        );
    }
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");

    const tensor_shape_t* train_set_shape = dataset_get_shape(train_set);
    const size_t num_samples = tensor_shape_get_dim(train_set_shape, TENSOR_BATCH_DIM);


    /* create classifier as simple linear layer */
    layer_t classifier;
    const linear_layer_create_info_t classifier_config = {
        .output_size = 10,
        .weight_init = winit_xavier,
        .bias_init = winit_zeros
    };
    layer_create(&classifier, &linear_layer_impl, &classifier_config, train_set_shape, device, num_samples);
    if (classifier == NULL) {
        LOG_ERROR("There was an error creating the model\n");
        return 1;
    }
    LOG_INFO("Created the classifier. Start training...\n");


    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(classifier), num_samples, LOSS_FUNCTION_CROSS_ENTROPY);


    /* training loop */
    const sgd_config_t optimizer_config = {
        .learning_rate = learning_rate,
        .weight_reg_kind = WEIGHT_REG_L2,
        .weight_reg_strength = l2_reg_strength
    };
    module_train(classifier, train_set, NULL, num_epochs, num_samples, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(classifier);
    LossDeinit(&loss);

    return 0;
}
    - uses: /**
 * @file two_layer_mlp_mnist.c
 * @brief Train a two-layered MLP on the MNIST dataset
 * 
 * This example illustrates how this library can be used to implement a two layer deep MLP and
 * trains it on the MNIST dataset.
 */

#include <inttypes.h>
#include <math.h>
#include <stddef.h>

#include "neuralnet.h"

#include "sequential/model_desc.h"
#include "sequential/sequential_model.h"

#include "dataset/mnist.h"
#include "optimizer/sgd.h"


#if defined(USE_GPU)
static const device_t device = device_gpu;
#else
static const device_t device = device_cpu;
#endif


static const char* mnist_path = "datasets/mnist";

/* config */
static const size_t num_epochs = 100;
static const size_t hidden_size = 300;
static const size_t batch_size = 32;
static const float learning_rate = 0.5f;


/* FC(hidden_size) -> Sigmoid -> FC(10) */
layer_t create_mlp(const tensor_shape_t* input_shape, size_t hidden_size, size_t batch_size)
{
    model_desc_t* desc;
    layer_t model;

    model_desc_create(&desc);

    model_desc_add_linear_layer(desc, hidden_size, winit_xavier, winit_zeros);
    model_desc_add_activation_layer(desc, ACTIVATION_FUNCTION_SIGMOID);
    model_desc_add_linear_layer(desc, 10, winit_xavier, winit_zeros);

    /* Print a model overview to stdout. */
    model_desc_dump(desc);

    const sequential_model_create_info_t config = {
        .desc = desc,
        .max_batch_size = batch_size
    };
    layer_create(&model, &sequential_model_impl, &config, input_shape, device, batch_size);

    /* Model desc not needed anymore */
    model_desc_destroy(desc);

    return model;
}


dataset_t train_set, test_set;
void train_callback(const training_state_t* state)
{
    float test_accuracy = NAN;
    float test_loss = NAN;

    module_test(state->model, test_set, batch_size, state->loss, &test_accuracy, &test_loss);
    LOG_INFO("Epoch %" PRIi32 " | Train loss %f | Train accuracy %5.3f%% | Test loss %f "
        "| Test accuracy %5.3f%%\n",
        state->epoch,
        state->train_loss,
        state->train_accuracy * 100.0f,
        test_loss,
        test_accuracy * 100.0f
    );
}


int main()
{
    /* load the dataset */
    const mnist_create_info_t dataset_config = {
        .path = mnist_path,
        .padding = 0,
    };
    if (dataset_create_train_and_test(&mnist_dataset, &dataset_config, true, &train_set,
                                      &test_set) != 0) {
        LOG_ERROR("There was an error loading the mnist dataset\n");
        return 1;
    }
    LOG_INFO("Successfully loaded mnist\n");


    /* create the model */
    layer_t mlp = create_mlp(dataset_get_shape(train_set), hidden_size, batch_size);
    if (mlp == NULL) {
        LOG_ERROR("There was an error creating the model\n");
        return 1;
    }
    LOG_INFO("Created the model. Start training...\n");


    /* create the loss */
    Loss loss;
    LossInit(&loss, layer_get_output_shape(mlp), batch_size, LOSS_FUNCTION_CROSS_ENTROPY);


    /* Training loop */
    const sgd_config_t optimizer_config = {
        .learning_rate = learning_rate,
        .weight_reg_kind = WEIGHT_REG_NONE
    };
    module_train(mlp, train_set, NULL, num_epochs, batch_size, &sgd_optimizer, &optimizer_config,
                 NULL, &loss, train_callback);


    /* Free resources */
    dataset_destroy(train_set);
    dataset_destroy(test_set);
    layer_destroy(mlp);
    LossDeinit(&loss);

    return 0;
}
    - name: #include "tensor.h"
#include "tensor_impl.h"

#include "context.h"

#include "layer/activation_layer.h"
#include "core/layer.h"

#include "random.h"

#include "log.h"
#include <stdio.h>

void test_tensor()
{
    tensor_shape_t shape = make_tensor_shape(4, 1, 2, 3, 4);
    LOG_INFO("Created shape\n");

    size_t size = tensor_size_from_shape(&shape);
    LOG_INFO("Tensor size is %zu\n", size);

    tensor_t tensor; 
    tensor_allocate(&tensor, &shape);
    LOG_INFO("Allocated tensor\n");

    LOG_INFO("Tensor data:\n");
    float* data = tensor_get_data(&tensor);
    for (size_t i = 0; i < size; i++) {
        printf("%f ", data[i]);
    }
    printf("\n");

    LOG_INFO("Filling tensor\n");
    tensor_fill(&tensor, 42.0f);

    LOG_INFO("Tensor data:\n");
    for (size_t i = 0; i < size; i++) {
        printf("%f ", data[i]);
    }
    printf("\n");

    LOG_INFO("Destroying tensor\n");
    tensor_destory(&tensor);
}


void test_activation_layer()
{
    layer_t layer;

    tensor_shape_t input_shape = make_tensor_shape(4, 1, 1, 2, 2);
    size_t size = tensor_size_from_shape(&input_shape);

    activation_layer_create_info_t act_config = {
        .activation_function = ACTIVATION_FUNCTION_RELU
    };

    LOG_INFO("Creating activation layer\n");
    layer_create(&layer, &activation_layer_impl, &act_config, &input_shape, 0);

    LOG_INFO("Allocating input tensor\n");
    tensor_t input;
    tensor_allocate(&input, &input_shape);
    float* input_data = tensor_get_data(&input);
    for (size_t i = 0; i < size; i++) {
        input_data[i] = RandomNormal(0.0f, 1.0f);
        printf("%f ", input_data[i]);
    }
    printf("\n");

    LOG_INFO("Applying activation\n");
    tensor_t* output;
    layer_forward(layer, LAYER_FORWARD_TRAINING, &input, &output);

    float* output_data = tensor_get_data(output);
    for (size_t i = 0; i < size; i++) {
        printf("%f ", output_data[i]);
    }
    printf("\n");

    LOG_INFO("Destroying input\n");
    tensor_destory(&input);
    LOG_INFO("Destroying layer\n");
    layer_destroy(layer);
}


int main() {

    if (backend_context_init() != 0) {
        LOG_ERROR("Backend context init failed\n");
    }
    LOG_INFO("Initialized backend context\n");

    test_activation_layer();
}
      run: # !/bin/bash

export CC=gcc
export CXX=g++


ROOT_PWD=$(pwd)

# Go to temporary folder
mkdir -p tmp && cd tmp

# Clone oneDNN source code
git clone https://github.com/oneapi-src/oneDNN.git
cd oneDNN

# Library is tested at this commit, but feel free to use different version
git checkout 9ef80d1732d054b7f12f0475d7181b37ffeba662

# Create build directory
mkdir -p build && cd build

# Configure CMake and generate makefiles
cmake .. -DCMAKE_INSTALL_PREFIX=${ROOT_PWD}/lib/onednn

# Build the library with half of the available cores to not overload the system.
make -j $((($(nproc) + 1) / 2 ))


# Install the library and headers
cmake --build . --target install

# Remove temporary folder
cd ${ROOT_PWD}
rm -rf tmp
    - name: Run tests
      run: cargo test --verbose

