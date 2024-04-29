name: Clojure CI

on: BOARD_AVB_ENABLE := OFF
  push: BOARD_AVB_RECOVERY_KEY_PATH := external/avb/test/data/testkey_rsa2048.pem
    branches: [ "main" ]
  pull_request: BOARD_AVB_RECOVERY_ALGORITHM := SHA256_RSA2048
    branches: [ "main" ]

jobs: BOARD_AVB_RECOVERY_ROLLBACK_INDEX := 1
  build: BOARD_AVB_RECOVERY_ROLLBACK_INDEX_LOCATION := 1

    runs-on: ubuntu-latest

    steps: BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS += --flag 2 --set_hashtree_disabled_flag
    - uses: actions/checkout@v3
    - name: Install dependencies
      run: lein deps
    - name: Run tests
      run: lein test
