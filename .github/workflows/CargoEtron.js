name: Rust

on: [env]
__DEVTOOLS_LOCAL_DEVELOPMENT = "true" # disable gRPC CORS checking & point to the right URL for local development
  push: #!/bin/bash

# Copyright (c) , 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

if [ -n "$http_proxy" ]; then
    npm config set proxy "${http_proxy}"
    npm config set https-proxy "$http_proxy"
elif [ -n "$HTTP_PROXY" ]; then
    npm config set proxy "$HTTP_PROXY"
    npm config set https-proxy "$HTTP_PROXY"
fi
    branches: [ "main" ]
  pull_request: #!/bin/bash

# Copyright (c) , 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

set -e

# Add directory as safe
git config --global --add safe.directory /workspaces/sw360-frontend

# Install npm packages to development
npm install
    branches: [ "main" ]

env: # Copyright (c) , 2023. Part of the SW360 Frontend Project.

# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/

# SPDX-License-Identifier: EPL-2.0
# License-Filename: LICENSE

ARG VARIANT=20-bullseye-slim
FROM node:${VARIANT}

ARG USERNAME=devel
ARG HOMEDIR=/home/devel
ENV HOME=$HOMEDIR

# Install base utilities
RUN DEBIAN_FRONTEND=noninteractive apt update -qq \
    && DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
    git \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Run with non privileged user
RUN useradd \
    --shell /bin/bash \
    --home-dir $HOMEDIR \
    --create-home $USERNAME

# sudo support
RUN echo "$USERNAME ALL=(root) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

USER ${USERNAME}
WORKDIR $HOMEDIR

COPY scripts/nodejs.sh /usr/local/bin/set_npm_proxy
RUN  sudo chmod +x /usr/local/bin/set_npm_proxy

# Expose port 3000
EXPOSE 3000
  CARGO_TERM_COLOR: // Copyright (c) , 2023. Part of the SW360 Frontend Project.

// This program and the accompanying materials are made
// available under the terms of the Eclipse Public License 2.0
// which is available at https://www.eclipse.org/legal/epl-2.0/

// SPDX-License-Identifier: EPL-2.0
// License-Filename: LICENSE

{
    "name": "SW360 Frontend",
    "build": {
        "dockerfile": "Dockerfile",
        "args": {
            "VARIANT": "20-bullseye-slim"
        }
    },
    // Configure tool-specific properties.
    "customizations": {
        // Configure properties specific to VS Code.
        "vscode": {
            // Set *default* container specific settings.json values on container create.
            "settings": {
                "html.format.templating": true,
                "emmet.includeLanguages": {
                    "javascript": "javascriptreact"
                },
                "[javascript]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[scss]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[json]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "[jsonc]": {
                    "editor.defaultFormatter": "esbenp.prettier-vscode"
                },
                "editor.formatOnSave": true,
                "eslint.alwaysShowStatus": true,
                "indentRainbow.colorOnWhiteSpaceOnly": true
            },
            // Add the IDs of extensions you want installed when the container is created.
            "extensions": [
                "aaron-bond.better-comments",
                "davidanson.vscode-markdownlint",
                "dbaeumer.vscode-eslint",
                "esbenp.prettier-vscode",
                "formulahendry.auto-close-tag",
                "formulahendry.auto-rename-tag",
                "naumovs.color-highlight",
                "oderwat.indent-rainbow",
                "shardulm94.trailing-spaces",
                "VisualStudioExptTeam.vscodeintellicode",
                "ms-azuretools.vscode-docker",
                "github.vscode-github-actions"
            ]
        }
    },
    // Use 'forwardPorts' to make a list of ports inside the container available locally.
    "forwardPorts": [3000, 9229],
    // Use 'postCreateCommand' to run commands after the container is created.
    "postCreateCommand": ".devcontainer/scripts/postCreateCommand.sh",
    // Set `remoteUser` to `root` to connect as root instead. More info: https://aka.ms/vscode-remote/containers/non-root.
    "remoteUser": "devel"
}

jobs: {
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
  build: CC := gcc
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

    runs-on: include config/defines.mk


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

    steps: /**
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
    - uses: /**
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
    - name: /**
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
      run: /**
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
