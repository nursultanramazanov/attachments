# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

# This workflow will install Deno then run `deno lint` and `deno test`.
# For more information see: https://github.com/denoland/setup-deno

name: Deno

on: <component name="ArtifactManager">
  <artifact type="jar" name="mvi-jvm">
    <output-path>$PROJECT_DIR$/mvi/build/libs</output-path>
    <root id="archive" name="mvi-jvm.jar">
      <element id="module-output" name="mvi.mvi.jvmMain" />
    </root>
  </artifact>
</component>
  push: [target.x86_64-pc-windows-msvc]
rustflags = ["-C", "target-feature=+crt-static"]

[target.'cfg(all(windows, debug_assertions))']
rustflags = [
  "-C",
  "target-feature=+crt-static",
  "-C",
  # increase the stack size to prevent overflowing the
  # stack in debug when launching sub commands
  "link-arg=/STACK:4194304",
]

[target.x86_64-apple-darwin]
rustflags = [
  "-C",
  "link-args=-weak_framework Metal -weak_framework MetalPerformanceShaders -weak_framework QuartzCore -weak_framework CoreGraphics",
]

[target.aarch64-apple-darwin]
rustflags = [
  "-C",
  "link-args=-fuse-ld=lld -weak_framework Metal -weak_framework MetalPerformanceShaders -weak_framework QuartzCore -weak_framework CoreGraphics",
]

[target.'cfg(all())']
rustflags = [
  "-D",
  "clippy::all",
  "-D",
  "clippy::await_holding_refcell_ref",
  "-D",
  "clippy::missing_safety_doc",
  "-D",
  "clippy::undocumented_unsafe_blocks",
  "--cfg",
  "tokio_unstable",
]
    branches: ["main"]
  pull_request: # To run a build using a local tree:
#
# 0. Check out these repositories as siblings:
#
#     - https://github.com/denoland/deno
#     - https://github.com/denoland/deno_core
#     - https://github.com/denoland/rusty_v8
#
# 1. From `deno`, run: cargo --config .cargo/local-build.toml build

[patch.crates-io]
deno_core = { path = "../deno_core/core" }
deno_ops = { path = "../deno_core/ops" }
serde_v8 = { path = "../deno_core/serde_v8" }
v8 = { path = "../rusty_v8" }
    branches: ["main"]

permissions: FROM mcr.microsoft.com/vscode/devcontainers/rust:1-bullseye

# Install cmake and protobuf-compiler
RUN apt-get update \
  && apt-get install -y cmake \
  && apt-get install -y protobuf-compiler \
  && rm -rf /var/lib/apt/lists/*

# Install Deno
ENV DENO_INSTALL=/usr/local
RUN curl -fsSL https://deno.land/x/install/install.sh | sh

  contents: {
  "name": "Rust",
  "build": {
    "dockerfile": "Dockerfile"
  },
  "runArgs": ["--cap-add=SYS_PTRACE", "--security-opt", "seccomp=unconfined"],
  "customizations": {
    "vscode": {
      "settings": {
        "lldb.executable": "/usr/bin/lldb",
        // VS Code don't watch files under ./target
        "files.watcherExclude": {
          "**/target/**": true
        },
        "extensions": [
          "rust-lang.rust-analyzer",
          "tamasfe.even-better-toml",
          "vadimcn.vscode-lldb",
          "mutantdino.resourcemonitor"
        ]
      }
    }
  },
  "postCreateCommand": "git submodule update --init",
  "remoteUser": "vscode"
}

jobs: CC := gcc
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
  test: include config/defines.mk


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
    runs-on: /**
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

    steps: /**
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
        uses: /**
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
        # uses: # !/bin/bash

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
        uses: # -----------------------------------------------------------------------------
# CONTRIB FRAMEWORK
#
# Based on CMake ExternalProjects, this repository offers functionality
# to configure, build, and install software dependencies that can be used
# by other projects.
#
# It has been developed in course of the open source
# research software BALL (Biochemical ALgorithms Library).
#
#
# Copyright 2016, the BALL team (http://www.ball-project.org)
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# -----------------------------------------------------------------------------


# Generate CMake list that contains all available packages without pkg_ prefix
# Created CMake variable holding available packages: PACKAGES_AVAILABLE
MACRO(PACKAGE_LIST_GENERATE)

        GET_CMAKE_PROPERTY(VAR_LIST VARIABLES)

        FOREACH(TMP_VAR ${VAR_LIST})
                STRING(REGEX MATCH "^pkg_+" MATCH_RESULT ${TMP_VAR})
                IF(NOT ${MATCH_RESULT})
                        STRING(SUBSTRING ${TMP_VAR} 4 -1 PACKAGE)
                        LIST(APPEND PACKAGES_AVAILABLE ${PACKAGE})
                ENDIF()
        ENDFOREACH()

ENDMACRO()


# Set a cross dependency for a contrib package selected for installation
MACRO(SET_CROSS_DEPENDENCIES)

        LIST(FIND PACKAGES_SELECTED ${ARGV0} PACKAGE_FOUND)

        IF(NOT ${PACKAGE_FOUND} EQUAL -1)
                SET(ARGN_LIST "${ARGN}")
                LIST(REMOVE_ITEM PACKAGES_SELECTED ${ARGN_LIST})
                LIST(REVERSE ARGN_LIST)
                LIST(APPEND PACKAGES_SELECTED ${ARGN_LIST})
        ENDIF()

ENDMACRO()


# Configure packages and download sources
MACRO(CONFIGURE_PACKAGES)

        FOREACH(PACKAGE ${PACKAGES_SELECTED})

                # Check if selected package is indeed an available one
                LIST(FIND PACKAGES_AVAILABLE ${PACKAGE} PACKAGE_FOUND)

                IF(${PACKAGE_FOUND} EQUAL -1)
                        MESSAGE(SEND_ERROR "The selected package '${PACKAGE}' is not part of BALL_contrib.")
                ELSE()
                        # Configure package
                        MESSAGE(STATUS "Configuring external project: ${PACKAGE} (${pkg_${PACKAGE}})")
                        INCLUDE("${CONTRIB_LIBRARY_PATH}/${PACKAGE}.cmake")

                        # Check if package source has already been downloaded
                        IF(NOT ${PACKAGE}_downloaded)

                                IF(${DOWNLOAD_TYPE} STREQUAL "archive")
                                        DOWNLOAD_SOURCE_ARCHIVE("${PACKAGE}" "${GIT_BRANCH}")
                                ELSE()
                                        DOWNLOAD_SOURCE_GIT("${PACKAGE}" "${GIT_BRANCH}")
                                ENDIF()

                                # Mark package as donwloaded in CMake internal cache
                                SET("${PACKAGE}_downloaded" TRUE CACHE INTERNAL "Variable indicates that package ${PACKAGE} has already been downloaded")

                        ENDIF()
                ENDIF()

        ENDFOREACH()

ENDMACRO()


# Download package archive (tarball/zipball)
MACRO(DOWNLOAD_SOURCE_ARCHIVE PACKAGE GIT_BRANCH)

        # Download archive
        FILE(DOWNLOAD "${CONTRIB_GITHUB_BASE}/${pkg_${PACKAGE}}/tarball/${GIT_BRANCH}" "${CONTRIB_BINARY_SRC}/${PACKAGE}.tarball"
                LOG "${CONTRIB_BINARY_SRC}/${PACKAGE}_download_out.log"
                TIMEOUT ${DOWNLOAD_TIMEOUT}
                STATUS DOWNLOAD_STATUS
                )

        LIST(GET DOWNLOAD_STATUS 0 DOWNLOAD_EXIT_CODE)

        # Check if download was successful
        IF(${DOWNLOAD_EXIT_CODE} EQUAL 0)

                # Extract archive
                EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xf "${CONTRIB_BINARY_SRC}/${PACKAGE}.tarball" WORKING_DIRECTORY ${CONTRIB_BINARY_SRC}
                        OUTPUT_FILE "${CONTRIB_BINARY_SRC}/${PACKAGE}_extract_out.log"
                        ERROR_FILE "${CONTRIB_BINARY_SRC}/${PACKAGE}_extract_err.log")

                # Move content of extracted archive into package source folder
                FILE(GLOB ARCHIVE_FILE_NAME "${CONTRIB_BINARY_SRC}/*${pkg_${PACKAGE}}*")
                EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E remove_directory ${PACKAGE} WORKING_DIRECTORY ${CONTRIB_BINARY_SRC})
                EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E rename ${ARCHIVE_FILE_NAME} ${PACKAGE} WORKING_DIRECTORY ${CONTRIB_BINARY_SRC})

        ELSE()
                MESSAGE(FATAL_ERROR "Download of package failed (archive): ${PACKAGE}")
        ENDIF()

ENDMACRO()


# Download package source using git clone
MACRO(DOWNLOAD_SOURCE_GIT PACKAGE GIT_BRANCH)

        EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} clone --depth 1 --branch "${GIT_BRANCH}" "${CONTRIB_GITHUB_BASE}/${pkg_${PACKAGE}}" "${CONTRIB_BINARY_SRC}/${PACKAGE}"
                        TIMEOUT ${DOWNLOAD_TIMEOUT}
                        RESULT_VARIABLE DOWNLOAD_EXIT_CODE
                        OUTPUT_FILE "${CONTRIB_BINARY_SRC}/${PACKAGE}_download_out.log"
                        ERROR_FILE "${CONTRIB_BINARY_SRC}/${PACKAGE}_download_err.log"
                        )

        # Check if download was successful
        IF(NOT ${DOWNLOAD_EXIT_CODE} EQUAL 0)
                MESSAGE(FATAL_ERROR "Download of package failed (git clone): ${PACKAGE}")
        ENDIF()

ENDMACRO()


# Macro to write absolute paths as install names for dylibs using install_name_tool
MACRO(FIX_DYLIB_INSTALL_NAMES DYLIB_PREFIX)

        # As this procedure has to wait until the libraries are built and installed,
        # fixing the names is added as an ExternalProject step depending on the install step.
        ExternalProject_Add_Step(${PACKAGE} fixnames

                LOG 1
                DEPENDEES install

                WORKING_DIRECTORY "${CONTRIB_INSTALL_LIB}"
                COMMAND ${CMAKE_COMMAND} -DLIBNAME=${DYLIB_PREFIX} -P "${PROJECT_SOURCE_DIR}/cmake/FixDylibInstallNames.cmake"
        )

ENDMACRO()


# Macro to print licensing information after successful configuration run
MACRO(LICENSE_AGREEMENT_MSG)
        MESSAGE("")
        MESSAGE("   +---------------------------------------------------------------------")
        MESSAGE("   +")
        MESSAGE("   + You successfully configured the Contrib!")
        MESSAGE("   +")
        MESSAGE("   +---------------------------------------------------------------------")
        MESSAGE("   +")
        MESSAGE("   + !!! IMPORTANT LICENSING INFORMATION !!!")
        MESSAGE("   +")
        MESSAGE("   + By running the build process of this project you accept")
        MESSAGE("   + all copyright and license agreements of software packages")
        MESSAGE("   + that are contained in this contrib project.")
        MESSAGE("   +")
        MESSAGE("   + Licensing information can be found in the source")
        MESSAGE("   + directories of the corresponding projects:")
        MESSAGE("   +")

        FOREACH(PACKAGE ${PACKAGES_SELECTED})
                MESSAGE( "   + - ${PACKAGE}:")
                MESSAGE( "   +   ${CONTRIB_BINARY_SRC}/${PACKAGE}")
        ENDFOREACH()

        MESSAGE("   +")
        MESSAGE("   +---------------------------------------------------------------------")
        MESSAGE("")
ENDMACRO()















  # v1.1.2
        with: # -----------------------------------------------------------------------------
# CONTRIB FRAMEWORK
#
# Based on CMake ExternalProjects, this repository offers functionality
# to configure, build, and install software dependencies that can be used
# by other projects.
#
# It has been developed in course of the open source
# research software BALL (Biochemical ALgorithms Library).
#
#
# Copyright 2016, the BALL team (http://www.ball-project.org)
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
# INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# -----------------------------------------------------------------------------


###############################################################################
# Check build type

SET(CONTRIB_BUILD_TYPES "Release" "Debug" "RelWithDebInfo")
LIST(FIND CONTRIB_BUILD_TYPES "${CMAKE_BUILD_TYPE}" VALID_BUILD_TYPE)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
        SET(CMAKE_BUILD_TYPE "Release")
ELSEIF(${VALID_BUILD_TYPE} EQUAL -1)
        MESSAGE(FATAL_ERROR "The specified CMAKE_BUILD_TYPE is invalid. Available build types are: ${CONTRIB_BUILD_TYPES}")
ENDIF()

MESSAGE(STATUS "Contrib build type: ${CMAKE_BUILD_TYPE}")


###############################################################################
# GitHub package repository settings

# Set GitHub base URL
IF(${DOWNLOAD_TYPE} STREQUAL "archive")
        SET(CONTRIB_GITHUB_BASE "https://api.github.com/repos/BALL-contrib" CACHE INTERNAL "GitHub base URL for archive download or cloning")
ELSE()
        SET(CONTRIB_GITHUB_BASE "git://github.com/BALL-contrib" CACHE INTERNAL "GitHub base URL for archive download or cloning")
ENDIF()


###############################################################################
# Set required paths

# Path that contains extracted sources (usually <build_dir>/src)
SET(CONTRIB_BINARY_SRC "${PROJECT_BINARY_DIR}/src" CACHE INTERNAL "Path where sources are located")

# Path that contains the contrib libraries
SET(CONTRIB_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/libraries" CACHE INTERNAL "Package configuration file path")

# Install directory and required subdirectories
SET(CONTRIB_INSTALL_INC "${CONTRIB_INSTALL_PREFIX}/include" CACHE INTERNAL "Installation include prefix")
SET(CONTRIB_INSTALL_BIN "${CONTRIB_INSTALL_PREFIX}/bin" CACHE INTERNAL "Installation bin prefix")
SET(CONTRIB_INSTALL_LIB "${CONTRIB_INSTALL_PREFIX}/lib" CACHE INTERNAL "Installation lib prefix")
FILE(MAKE_DIRECTORY ${CONTRIB_INSTALL_PREFIX})
FILE(MAKE_DIRECTORY ${CONTRIB_INSTALL_BIN})
FILE(MAKE_DIRECTORY ${CONTRIB_INSTALL_INC})
FILE(MAKE_DIRECTORY ${CONTRIB_INSTALL_LIB})


###############################################################################
# Logging options for external project steps

SET(CUSTOM_BUILD_IN_SOURCE 1 CACHE STRING  "Build projects in sources. Default: 1")
SET(CUSTOM_LOG_DOWNLOAD 1 CACHE STRING  "Write download logfile step instead of printing. Default: 1")
SET(CUSTOM_LOG_UPDATE 1 CACHE STRING  "Write update/patch logfile instead of printing. Default: 1")
SET(CUSTOM_LOG_CONFIGURE 1 CACHE STRING  "Write configure logfile instead of printing. Default: 1")
SET(CUSTOM_LOG_BUILD 1 CACHE STRING  "Write build logfile instead of printing. Default: 1")
SET(CUSTOM_LOG_INSTALL 1 CACHE STRING  "Write install logfile instead of printing. Default: 1")


###############################################################################
# Set platform dependent build variables

# Determine whether this is a 32 or 64 bit build
SET(CONTRIB_ADDRESSMODEL 32 CACHE INTERNAL "Platform address model")
SET(MSBUILD "msbuild" "/m:${THREADS}" "/p:Platform=win32" "/p:Configuration=${CMAKE_BUILD_TYPE}" CACHE INTERNAL "MSbuild command including build type, number of threads and address model")

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(CONTRIB_ADDRESSMODEL 64)
        SET(MSBUILD "msbuild" "/m:${THREADS}" "/p:Platform=x64"   "/p:Configuration=${CMAKE_BUILD_TYPE}")
ENDIF()

# Set appropriate build commands for non-windows systems
IF(NOT MSVC)
        SET(MAKE_COMMAND "make" "-j${THREADS}" CACHE INTERNAL "Make command including number of threads")
        SET(MAKE_INSTALL_COMMAND "make" "install" CACHE INTERNAL "Install command")
ENDIF()


###############################################################################
# System inforamtiom

# MSVC version
IF(MSVC12)
        SET(CONTRIB_MSVC_VERSION "12.0")
        SET(CONTRIB_MSVC_VERSION_YEAR "2013")
ELSEIF(MSVC14)
        SET(CONTRIB_MSVC_VERSION "14.0")
        SET(CONTRIB_MSVC_VERSION_YEAR "2015")
ENDIF()


###############################################################################
# Search Git

FIND_PACKAGE(Git)
IF(NOT GIT_FOUND)
        MESSAGE(FATAL_ERROR "Git not found. Please install git or add it to the search path.")
ENDIF()


          deno-version: # Post-install CMake script to change install names of dylibs to absolute paths

# First, select all candidate dylibs in the current working directory.
# The working directory is set by the calling macro (FIX_DYLIB_INSTALL_NAMES) to the install/lib folder.
FILE(GLOB_RECURSE dylibs "${LIBNAME}*.dylib")


FOREACH(dylib ${dylibs})
        GET_FILENAME_COMPONENT(base_name ${dylib} NAME)

        # Only process the library itself and not symlinks to it
        IF(NOT IS_SYMLINK ${dylib})

                EXECUTE_PROCESS(COMMAND otool -L ${dylib} OUTPUT_VARIABLE temp)
                SEPARATE_ARGUMENTS(deps UNIX_COMMAND ${temp})
                LIST(REMOVE_AT deps 0)

                FOREACH(dep ${deps})
                        IF(${dep} MATCHES ".?dylib" AND NOT IS_ABSOLUTE ${dep})
                                GET_FILENAME_COMPONENT(name ${dep} NAME)
                                GET_FILENAME_COMPONENT(abs_name ${dep} ABSOLUTE)

                                IF(${name} STREQUAL ${base_name})
                                        # Set asolute path of dylib as its install name
                                        EXECUTE_PROCESS(COMMAND install_name_tool -id "${abs_name}" ${dylib})
                                ELSE()
                                        # Set absolute paths of dependent shared libraries
                                        EXECUTE_PROCESS(COMMAND install_name_tool -change "${name}" "${abs_name}" ${dylib})
                                ENDIF()
                        ENDIF()
                ENDFOREACH()
        ENDIF()

ENDFOREACH()


      # Uncomment this step to verify the use of 'deno fmt' on each commit.
      # - name: Verify formatting
      #   run: deno fmt --check

      - name: Run linter
        run: deno lint

      - name: Run tests
        run: deno test -A
