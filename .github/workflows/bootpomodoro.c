# This starter workflow is for a CMake project running on a single platform. There is a different starter workflow if you need cross-platform coverage.
# See: https://github.com/actions/starter-workflows/blob/main/ci/cmake-multi-platform.yml
name: CMake on a single platform

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        echo "DOCKER_VERSION= indent=force-tab=8
style=linux
indent-preprocessor
indent-cases
break-blocks=all
pad-oper
pad-header
unpad-paren
keep-one-line-blocks
keep-one-line-statements
align-pointer=name
align-reference=name
-n #--suffix=none
ignore-exclude-errors-x
lineend=linux
exclude=EASTL
add-brackets
max-code-length=120
        DEFCONFIG_FILE="build/configs/'use strict';

import React from 'react';
import ReactDOM from 'react-dom';
import TestUtils from 'react-addons-test-utils';
import Footer from './../../../app/components/Footer/Footer';

import { assert } from 'chai';

describe('<Footer /> tests of node elements', () => {
  var component;

  var wrapper = React.createClass({
    render() {
      return <Footer />;
    }
  });

  beforeEach(function() {
    component = TestUtils.renderIntoDocument(
      React.createElement(wrapper)
    );
  });

  it('the link of github author should be https://github.com/afonsopacifer', () => {
    let linkElement = TestUtils.scryRenderedDOMComponentsWithTag(component, 'a');

    assert.equal(linkElement.length, 1);
    assert.equal(linkElement[0].getAttribute('href'), 'https://github.com/afonsopacifer', 'the link is incorrect');
  });

});/defconfig"
        if [ -f "$DEFCONFIG_FILE" ] && grep -q '^CONFIG_DOCKER_VERSION' "$DEFCONFIG_FILE"; then
          DOCKER_IMG_VERSION=$(grep '^CONFIG_DOCKER_VERSION' 'use strict';

import React from 'react';
import ReactDOM from 'react-dom';
import Pomodoro from './../../../app/components/Pomodoro/Pomodoro';
import { mount } from 'enzyme';

import { assert } from 'chai';

describe('<Pomodoro /> tests of default settings', () => {
  var component; 

  beforeEach(() => {
    component = mount(<Pomodoro />);
  });

  it('verify the default state are correctly', () => {
    assert.isFalse(component.node.state.play);
    assert.equal(component.node.state.time, 1500);
    assert.equal(component.node.state.timeType, 1500);
    assert.equal(component.node.state.title, '25:00 | Pomodoro timer');
  });

  it('the options of notification should be unchecked', () => {
    assert.isFalse(component.node.refs.audio.checked);
    assert.isFalse(component.node.refs.vibrate.checked);
    assert.isFalse(component.node.refs.notification.checked);
  });

  it('should be two buttons to control the play and pause', () => {
    let buttons = component.find('div.pomodoro div.controlsPlay button');

    assert.equal(buttons.length, 2);

    let playButton = buttons.find('.play').node,
        stopButton = buttons.find('.stop').node;
    
    assert.isDefined(playButton);
    assert.isDefined(stopButton);
  });

  it('should be three buttons to change pomodoro type', () => {
    let buttons = component.find('div.pomodoro div.main div.types button');

    assert.equal(buttons.length, 3);

    let codeButton    = buttons.find('.code').node,
        socialButton  = buttons.find('.social').node,
        coffeeButton  = buttons.find('.coffee').node;
  
    assert.isDefined(codeButton);
    assert.isDefined(socialButton);
    assert.isDefined(coffeeButton);

    assert.equal(codeButton.innerHTML, 'Code');
    assert.equal(socialButton.innerHTML, 'Social');
    assert.equal(coffeeButton.innerHTML, 'Coffee');
  });
});

describe('<Pomodoro /> tests behavior of buttons', () => {
  var component; 

  beforeEach(() => {
    component = mount(<Pomodoro />);
  });

  it('when click on play the state should be changed', () => {
    let playButton = component.find('div.pomodoro div.controlsPlay button.play');

    assert.isFalse(component.node.state.play);
    playButton.simulate('click');
    assert.isTrue(component.node.state.play);
  });

  it('when click on social type the states should be changed', () => {
    let socialButton = component.find('div.pomodoro div.types button.social');

    socialButton.simulate('click');

    assert.isTrue(component.node.state.play);
    assert.equal(component.node.state.time, 300);
    assert.equal(component.node.state.timeType, 300);
    assert.equal(component.node.state.title, '05:00 | Pomodoro timer');
  });

  it('when click on coffee type the states should be changed', () => {
    let coffeeButton = component.find('div.pomodoro div.types button.coffee');

    coffeeButton.simulate('click');

    assert.isTrue(component.node.state.play);
    assert.equal(component.node.state.time, 900);
    assert.equal(component.node.state.timeType, 900);
    assert.equal(component.node.state.title, '15:00 | Pomodoro timer');
  });
});

describe('<Pomodoro /> check if items on localStorage should be exists', () => {
  var component; 

  beforeEach(() => {
    component = mount(<Pomodoro />);
  });

  afterEach(() => {
    localStorage.clear();
  });

  it('after checked the notification input', () => {
    let notificationInput = component.find('div.pomodoro div.controlsCheck #notification');

    let item = 'react-pomodoro-notification';

    assert.isUndefined(localStorage.getItem(item));
    notificationInput.simulate('change', { target: { checked: true } });
    assert.equal(localStorage.getItem(item), 'true');
  });

  it('after checked the audio input', () => {
    let audioInput = component.find('div.pomodoro div.controlsCheck #audio');

    let item = 'react-pomodoro-audio';
    
    assert.isUndefined(localStorage.getItem(item));
    audioInput.simulate('change', { target: { checked: true } });
    assert.equal(localStorage.getItem(item), 'true');
  });

  it('after checked the vibrate input', () => {
    let audioInput = component.find('div.pomodoro div.controlsCheck #vibrate');

    let item = 'react-pomodoro-vibrate';

    assert.isUndefined(localStorage.getItem(item));
    audioInput.simulate('change', { target: { checked: true } });
    assert.equal(localStorage.getItem(item), 'true');
  });
}); | sed 's/.*=//; s/"//g')
          echo "GREP AND MODIFIED VERSION=${DOCKER_IMG_VERSION}"
        fi

        docker create -v /${WORKING_DIR} --name ${VOLUME} --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash
        docker cp ./. ${VOLUME}:/${WORKING_DIR}
        docker run -d --rm -it --name ${BUILDER} --volumes-from=${VOLUME} -w /${WORKING_DIR}/os --privileged tizenrt/tizenrt:${DOCKER_IMG_VERSION} /bin/bash

  arm-version: &arm-version
    run:
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CONFIG_PATH}"
        docker exec -it ${BUILDER} bash -c "make"


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
            
  artik055s_audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik055s/audio
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053_tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: artik053/tc
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu_build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: qemu/build_test
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit_hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: esp_wrover_kit/hello_with_tash
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk_loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: imxrt1020-evk/loadable_elf_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8721csm/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_ddr_nand:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_ddr_nand
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_flat_dev_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/flat_dev_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_ddr_st7785:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_ddr_st7785
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e_loadable_ext_psram:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: rtl8730e/loadable_ext_psram
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/hello
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_apps
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_xip_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/xip_all
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  bk7239n_loadable_all:
    machine:
      image: default
    working_directory: ~/TizenRT
    environment:
      CONFIG_PATH: bk7239n/loadable_all
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
      - artik055s_audio:
          requires:
            - checkout_code
      - artik053_tc:
          requires:
            - checkout_code
      - qemu_build_test:
          requires:
            - checkout_code
      - esp_wrover_kit_hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk_loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm_hello:
          requires:
            - checkout_code
      - rtl8721csm_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_apps:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_ddr_nand:
          requires:
            - checkout_code
      - rtl8730e_flat_dev_psram:
          requires:
            - checkout_code
      - rtl8730e_loadable_apps:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_ddr_st7785:
          requires:
            - checkout_code
      - rtl8730e_loadable_ext_psram:
          requires:
            - checkout_code
      - bk7239n_hello:
          requires:
            - checkout_code
      - bk7239n_loadable_apps:
          requires:
            - checkout_code
      - bk7239n_xip_all:
          requires:
            - checkout_code
      - bk7239n_loadable_all:
          requires:
            - checkout_code
  push:
    branches: [ "main" ]
  pull_request:
    branches: [ "main" ]

env:
  # Customize the CMake build type here (Release, Debug, RelWithDebInfo, etc.)
  BUILD_TYPE: Release

jobs:
  build:
    # The CMake configure and build commands are platform agnostic and should work equally well on Windows or Mac.
    # You can convert this to a matrix build if you need cross-platform coverage.
    # See: https://docs.github.com/en/free-pro-team@latest/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Configure CMake
      # Configure CMake in a 'build' subdirectory. `CMAKE_BUILD_TYPE` is only required if you are using a single-configuration generator such as make.
      # See https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=cmake_build_type
      run: cmake -B ${{github.workspace}}/build -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}}

    - name: Build
      # Build your program with the given configuration
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}

    - name: Test
      working-directory: ${{github.workspace}}/build
      # Execute tests defined by the CMake configuration.
      # See https://cmake.org/cmake/help/latest/manual/ctest.1.html for more detail
      run: ctest -C ${{env.BUILD_TYPE}}

