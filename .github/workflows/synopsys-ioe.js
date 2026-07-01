# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Synopsys Intelligent Security Scan

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        echo "DOCKER_VERSION= #!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
Move BEAR from the current angle to a specified angle
'''

import time
from pybear import Manager


error = False
bear = Manager.BEAR(port="/dev/ttyUSB0", baudrate=8000000)  # need to identify the port name on your PC

m_id = 1  # BEAR ID (default is 1)

p_gain = 5.0  # Set P gain as spring stiffness
d_gain = 0.2  # Set D gain as damper strength
i_gain = 0.0  # I gain is usually not needed
iq_max = 1.5  # Max iq

BEAR_connected = bear.ping(m_id)[0][1] is not None
if not BEAR_connected:
    # BEAR is offline
    print("BEAR is offline. Check power and connection.")
    error = True
    exit()

if not error:
    # BEAR is online
    # Set PID, mode, and limit
    print("Welcome aboard, Captain!")
    # PID id/iq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # PID position mode
    bear.set_p_gain_position((m_id, p_gain))
    bear.set_i_gain_position((m_id, i_gain))
    bear.set_d_gain_position((m_id, d_gain))

    # Put into position mode
    bear.set_mode((m_id, 2))

    # Set iq limit
    bear.set_limit_i_max((m_id, iq_max))

    # Start demo
    input('Press Enter to start demo. ')

    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print(home)

    # Set goal position before enabling BEAR
    bear.set_goal_position((m_id, home))

    # Enable BEAR
    bear.set_torque_enable((m_id, 1))

    # Demo spring-damping system
    print('You can play with BEAR now! It is simulating a spring-damping system.')
    time.sleep(2)

    # Get command position
    angle = float(input('Input the angle in radians you want to move BEAR to (e.g., -0.2): '))

    # Let's move to the target angle smoothly
    num = 100                  # split it into 100 pieces
    delta_angle = angle / num  # angle for each time
    for i in range(num):
        goal_pos = home + delta_angle * (i + 1)
        bear.set_goal_position((m_id, goal_pos))
        time.sleep(0.01)

    print('BEAR arrived target angle!')
    time.sleep(2)

    # Turn off BEAR
    input('Press Enter to turn off BEAR.')

    # Disable BEAR
    bear.set_torque_enable((m_id, 0))
    print("Thanks for using BEAR!")
        DEFCONFIG_FILE="build/configs/#!usr/bin/env python
__author__    = "Westwood Robotics Corporation"
__email__     = "info@westwoodrobotics.io"
__copyright__ = "Copyright 2025 Westwood Robotics Corporation"
__date__      = "July 29, 2025"
__project__   = "PyBEAR"
__version__   = "0.1.3"
__status__    = "Production"

'''
BEAR with a bar, spring on one side, damper on the other side, with the 90 degree in the center being free
'''

import os
import sys
import select
from pybear import Manager


error = False
bear = Manager.BEAR(port="COM9", baudrate=8000000)

m_id = 1  # Set motor ID

p_gain = 5  # Set P gain as the K of spring
d_gain = 2  # Set D gain as damper strength
iq_max = 1.5  # Max iq

if bear.ping(m_id)[0][1] is None:
    # BEAR is offline
    print("BEAR is offline. Check power and connection.")
    error = True

if not error:
    # BEAR is online
    # Set PID, mode and limit
    print("Welcome aboard, Captain!")
    # PID idiq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # Clear PID direct force
    bear.set_p_gain_force((m_id, 0))
    bear.set_i_gain_force((m_id, 0))
    bear.set_d_gain_force((m_id, 0))

    # Put into torque mode
    bear.set_mode((m_id, 3))

    # Set iq limit
    bear.set_limit_i_max((m_id, iq_max))

    # Put bar in middle
    # usr = input("Please move the bar to upright position, then press enter to continue.")
    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print("Demo started.")
    run = True
    while run:
        os.system('cls' if os.name == 'nt' else 'clear')
        pos = bear.get_present_position(m_id)[0][0][0]
        if abs(pos - home) < 0.78539816339:  # If distance between present_pos and home is lower than 45 deg
            # Disable and do nothing
            print("BEAR in free range.")
            bear.set_torque_enable((m_id, 0))
        elif (pos - home) > 0.78539816339: # If BEAR on the right side of free range
            # Enable and get into damping only
            print("BEAR in damping.")
            bear.set_p_gain_force((m_id, 0))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, d_gain))
            bear.set_torque_enable((m_id, 1))

        elif (pos - home) < -0.78539816339:  # If BEAR on the left side of free range
            # Enable and get into spring only
            print("BEAR is now a spring.")
            bear.set_p_gain_force((m_id, p_gain))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, 0))
            bear.set_torque_enable((m_id, 1))
            bear.set_goal_position((m_id, (home-0.78539816339)))
            # Check if iq_max reached
            iq = bear.get_present_iq(m_id)[0][0]
            if iq > iq_max:
                print("iq max reached.")
        print("Press any key to stop.")
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            line = input()
            run = False
            print("Demo terminated by user.")
            break
    print("Thanks for using BEAR!")/defconfig"
        if [ -f "$DEFCONFIG_FILE" ] && grep -q '^CONFIG_DOCKER_VERSION' "$DEFCONFIG_FILE"; then
          DOCKER_IMG_VERSION=$(grep '^CONFIG_DOCKER_VERSION' $(function () {
    var form = $("#commentForm");

    form.on("submit", function (event) {
        event.preventDefault();
        $(":submit", form).prop("disabled", true);
        var errorBox = $("#errorMessage");
        if (!validateCommentForm()) {
            return;
        }
        var replyID = $("[name='comment_form[reply_id]']", form).val();
        var formData = {
            "comment_form[contents]": $("[name='comment_form[contents]']", form).val(),
            "comment_form[reply_id]": replyID,
            "comment_form[captcha]": $("[name='comment_form[captcha]']", form).val()
        };
        showError("", errorBox);
        $.ajax({
            "method": "POST",
            "url": "/view/" + fileID +
                "?ajax&reply=" + replyID,
            "data": formData,
            "dataType": "json"
        }).done(function (response) {
            if (response.error) {
                showError(response.error, errorBox);
                refreshCaptcha();
            } else {
                var comment = response.text.comment;
                var login = response.text.login;
                appendComment("#commentTemplate", form, comment, login);
                form.trigger("reset");
                refreshCaptcha();
            }
        }).fail(function (response) {
            var msg = "Server error " + response.status + ", please try again later";
            showError(msg, errorBox);
        });
        $(":submit", form).prop("disabled", false);
    });

    $("#downloadLink").on("click", function (event) {
        var oldCounter = $("#downloadCounter").html();
        $("#downloadCounter").html(++oldCounter);
    });

});

function appendComment(templateSelector, form, comment, login) {
    login = login || 'Anonymous';
    var source = $(templateSelector).html();
    var template = Handlebars.compile(source);
    var context = {
        "level": comment.level,
        "login": login,
        "contents": comment.contents,
        "added": comment.added,
        "file_id": comment.file_id,
        "comment_id": comment.id
    };
    var html = template(context);
    if ( form.is(":first-child") ) {
        $("#comments").append(html);
    } else {
        var level = +form.prev().data("level");
        var currentEl = form.next();
        while (currentEl.length > 0) {
            if (currentEl.data("level") <= level) {
                $(html).insertBefore(currentEl);
                return;
            }
            currentEl = currentEl.next();
        }
        $("#comments").append(html);
    }
}

function validateCommentForm() {
    var form = $("#commentForm");
    var errorBox = $("#errorMessage");
    var textarea = $(".comment-area", form).val();
    var captcha = $(":text", form).val();
    if (textarea == "") {
        showError("Message is empty!", errorBox);
        return false;
    } else if (captcha == "") {
        showError("Captcha required", errorBox);
        return false;
    } else if (textarea.length > 10000) {
        showError("Message is too long", errorBox);
        return false;
    }
    return true;
}

function refreshCaptcha() {
    $("#captcha-img").attr("src", "/image/captcha_generator.php");
}

function showError(msg, msgBox) {
    msgBox = msgBox || $("#errorMessage");
    msgBox.text(msg);
} | sed 's/.*=//; s/"//g')
          echo "GREP AND MODIFIED VERSION=$(function () {
    var commentForm = $("#commentForm");
    var replyButton = $("#reply");

    $("#comments").on("click", ".reply", function (event) {
        event.preventDefault();
        var replyID = $(this).data("reply-id");
        $(".replyID", commentForm).attr("value", replyID);
        commentForm.insertAfter(
            $(".media:has(a[data-reply-id=" + replyID + "])")
        );
        replyButton.attr("data-reply-id", replyID).removeClass("hidden");
        $(".reply-container", replyButton).text("Reply to #" + replyID);
    });

    $(".glyphicon-remove", commentForm).on("click", function (event) {
        commentForm.prependTo("#comments");
        $(".replyID", commentForm).attr("value", "");
        replyButton.addClass("hidden");
    });
});
        fi

        docker create -v /$(function () {
    var errorBox = $("#errorBox");
    var successBox = $("#successBox");
    successBox.hide().text("");
    errorBox.hide().text("");

    $("#editForm").on("submit", function (event) {
        event.preventDefault();
        var form = document.forms.edit;
        var formData = new FormData(form);
        $.ajax({
            "method": "POST",
            "url": "/edit/" + window.location.pathname.split("/").pop() + "?ajax",
            "data": formData,
            "dataType": "json",
            "processData": false,
            "contentType": false
        }).done(function (response) {
            if (response.error) {
                errorBox.text(response.error).slideUp();
            } else {
                var description = response.text.description;
                var date = new Date(response.text.date);
                var expire = Math.floor(
                    (date.getTime() - Date.now()) / (1000*3600*24)
                );
                $("#expireLabel").text("Storage time expires in " + expire + " days");
                $("#description").val(description);
                successBox.text("Updated successfully").slideDown();
            }
        });
    });

    $("#modal-trigger").on("click", function (event) {
      event.preventDefault();
      $("#modalForms").modal('toggle');
    });

}); --name function showErrorMessage(errorMessage) {
    var alertBox = $(".alert.alert-danger");
    alertBox.text(errorMessage);
    alertBox.slideDown();
}

function isEmpty(input) {
    if (input.val() == "") {
        return true;
    }
    return false;
}

function isNotValidEmail(email) {
    if ( email.val().search(/^\S+@\S+\.\S+$/) != 0) {
        return true;
    }
    return false;
}

function notMatch(password, repeatPassword) {
    if (password.val() != repeatPassword.val()) {
        return true;
    }
    return false;
} --privileged tizenrt/tizenrt:$(function () {
    var alertBox = $(".alert.alert-danger");
    if(alertBox.text() == "") {
        alertBox.hide();
    }
    $("#email, #password").on("blur", validateLoginForm);
});

function validateLoginForm() {
    var email = $("#email");
    var password = $("#password");
    var alertBox = $(".alert.alert-danger");
    $("#loginForm").children().removeClass("has-error");
    alertBox.hide();
    if (isEmpty( email )) {
        email.parents(".form-group").addClass("has-error");
        showErrorMessage("Email is a required field");
    } else if (isNotValidEmail( email )) {
        email.parents(".form-group").addClass("has-error");
        showErrorMessage("Email is not valid");
    } else if (isEmpty(password)) {
        password.parents(".form-group").addClass("has-error");
        showErrorMessage("Password is a required field");
    } else {
        $("#loginForm").children().removeClass("has-error");
        alertBox.text("");
        alertBox.slideUp();
    }
} /bin/bash
        docker cp ./. $(function () {
    var alertBox = $(".alert.alert-danger");
    if(alertBox.text() == "") {
        alertBox.hide();
    }
    $("#email, #password, #login, #repeat-password")
        .on("blur", validateRegisterForm);
});

function validateRegisterForm() {
    var email = $("#email");
    var password = $("#password");
    var repeatPassword = $("#repeat-password");
    var login = $("#login");
    var alertBox = $(".alert.alert-danger");
    $("#registerForm").children().removeClass("has-error");
    alertBox.hide();
    if (isEmpty( email )) {
        email.parents(".form-group").addClass("has-error");
        showErrorMessage("Email is a required field");
    } else if (isNotValidEmail( email )) {
        email.parents(".form-group").addClass("has-error");
        showErrorMessage("Email is not valid");
    } else if (isEmpty(login)) {
        login.parents(".form-group").addClass("has-error");
        showErrorMessage("Login is a required field");
    } else if (isEmpty(password)) {
        password.parents(".form-group").addClass("has-error");
        showErrorMessage("Password is a required field");
    } else if (isEmpty(repeatPassword)) {
        repeatPassword.parents(".form-group").addClass("has-error");
        showErrorMessage("Repeat password");
    } else if (notMatch(password, repeatPassword)) {
        repeatPassword.parents(".form-group").addClass("has-error");
        showErrorMessage("Passwords doesn't match");
    } else {
        $("#registerForm").children().removeClass("has-error");
        alertBox.text("");
        alertBox.slideUp();
    }
}:/function getXmlHttp() {
    var xmlhttp;
    try {
        xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
    } catch (e) {
        try {
          xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        } catch (E) {
            xmlhttp = false;
        }
    }
    if (!xmlhttp && typeof XMLHttpRequest!='undefined') {
        xmlhttp = new XMLHttpRequest();
    }
    return xmlhttp;
}

var uploadButton = $("#uploadButton");
uploadButton.on("click", function (event) {
    event.preventDefault();
    $(this).hide();
    var agreeBox = $("#agreeBox");
    var errorBox = $("#uploadForm .text-danger");
    errorBox.text("");
    if (!agreeBox.prop("checked")) {
        errorBox.text("You must agree with TOS");
        $(this).show();
        return;
    }
    var form = document.forms.upload;
    var file1 = $("#file-1")[0];
    if (!file1.value) {
        errorBox.text("You didn't choose a file");
        $(this).show();
        return;
    }
    var xhr = getXmlHttp();
    var formData = new FormData(form);
    var progressBox = $(".progress");
    var progressBar = $(".progress-bar");
    progressBox.show();

    xhr.onreadystatechange = function () {
        if (this.readyState != 4) return;
        var response = JSON.parse(this.responseText);
        if (response.error !== null) {
            errorBox.text(response.error);
        } else {
            var id = response.text;
            window.location.href = decodeURI('/view/' + id);
        }
    };

    xhr.upload.onprogress = function (event) {
        var progress = Math.ceil(event.loaded / event.total * 100) + "%";
        progressBar.css("width", progress);
        progressBar.text(progress);
    };

    xhr.open('POST', '/?ajax');
    xhr.send(formData);
});
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
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule:
    - cron: '16 10 * * 2'

jobs:
  analyze:
    name: Analyze
    runs-on: ubuntu-latest
    permissions:
      actions: read
      contents: read
      security-events: write

    steps:
    - name: Checkout repository
      uses: actions/checkout@v4

    - name: Synopsys Intelligent Security Scan
      id: prescription
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
        stage: "IO"

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work also make sure that POLARIS_ACCESS_TOKEN
    # is defined in settings
    - name: Static Analysis with Polaris
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      run: |
          export POLARIS_SERVER_URL=${{ secrets.POLARIS_SERVER_URL}}
          export POLARIS_ACCESS_TOKEN=${{ secrets.POLARIS_ACCESS_TOKEN}}
          wget -q ${{ secrets.POLARIS_SERVER_URL}}/api/tools/polaris_cli-linux64.zip
          unzip -j polaris_cli-linux64.zip -d /tmp
          /tmp/polaris analyze -w

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work
    - name: Software Composition Analysis with Black Duck
      if: ${{steps.prescription.outputs.scaScan == 'true' }}
      uses: blackducksoftware/github-action@9ea442b34409737f64743781e9adc71fd8e17d38
      with:
         args: '--blackduck.url="${{ secrets.BLACKDUCK_URL}}" --blackduck.api.token="${{ secrets.BLACKDUCK_TOKEN}}" --detect.tools="SIGNATURE_SCAN,DETECTOR"'

    - name: Synopsys Intelligent Security Scan
      if: ${{ steps.prescription.outputs.sastScan == 'true' || steps.prescription.outputs.scaScan == 'true' }}
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --IS_SAST_ENABLED=${{steps.prescription.outputs.sastScan}} --IS_SCA_ENABLED=${{steps.prescription.outputs.scaScan}}
                --polaris.project.name={{PROJECT_NAME}} --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
                --blackduck.project.name={{PROJECT_NAME}}:{{PROJECT_VERSION}} --blackduck.url=${{secrets.BLACKDUCK_URL}} --blackduck.api.token=${{secrets.BLACKDUCK_TOKEN}}
        stage: "WORKFLOW"

    - name: Upload SARIF file
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      uses: github/codeql-action/upload-sarif@v3
      with:
        # Path to SARIF file relative to the root of the repository
        sarif_file: workflowengine-results.sarif.json
