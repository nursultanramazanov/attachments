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
  build:

    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4
    - name: Build
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
