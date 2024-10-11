# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
# Frogbot Scan Pull Request does the following:
# Automatically scans new pull requests for security vulnerabilities.
# Uses JFrog Xray to scan the project.
# Read more about Frogbot here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot

# Some projects require creating a frogbot-config.yml file. Read more about it here - https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot/setup-frogbot/frogbot-configuration

name: "Frogbot Scan Pull Request"
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
  pull_request_target: import numpy as np
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
    types: [import cv2
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
out.release()]
permissions: 


/*
**        Description:        To decode the data, encoded using 3 character encoding technique.
*/





#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput);

int main(void)
{
        uint32_t forDecodingDebug;
        uint8_t byte3, byte2, byte1;
        while(1)
        {
                printf("Input 3 bytes of data : \n");
                scanf("%u%u%u", &byte3, &byte2, &byte1);
                ThreeCharacterEncoding_Decode(byte3, byte2, byte1, &forDecodingDebug);
                printf("Decoded Value = %d\n", forDecodingDebug);
        }
}


/*
**        @brief                                                                                        Function to decode the 3 character encoding, to extract the encoded distance.
**        @byte3(PARAM_IN)                                                Higher encoded byte.
**        @byte2(PARAM_IN)                                                High encoded byte.
**        @byte1(PARAM_IN)                                                Low encoded byte.
**        @decodedOutput(PARAM_OUT)                Pointer to the variable which will contain the decoded output.
*/
void ThreeCharacterEncoding_Decode(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint32_t *decodedOutput)
{
        uint8_t temp_byte1;
        uint8_t temp_byte2;
        uint8_t temp_byte3;

        /*        Decryption starts with subtracting 0x30 from each of the encoded bytes        */
        temp_byte3 = byte3 - 0x30;
        temp_byte2 = byte2 - 0x30;
        temp_byte1 = byte1 - 0x30;

        /*        Reset all bits of the output variable        */
        *decodedOutput &= ~(*decodedOutput);

        /*        Populate the output variable with decoded data        */
        *decodedOutput |= (temp_byte3 << 12);
        *decodedOutput |= (temp_byte2 << 6);
        *decodedOutput |= (temp_byte1 << 0);

        return;
}
  pull-requests: write
  contents: read
jobs:
  scan-pull-request:
    runs-on: ubuntu-latest
    # A pull request needs to be approved, before Frogbot scans it. Any GitHub user who is associated with the
    # "frogbot" GitHub environment can approve the pull request to be scanned.
    # Read more here (Install Frogbot Using GitHub Actions): https://docs.jfrog-applications.jfrog.io/jfrog-applications/frogbot/setup-frogbot/setup-frogbot-using-github-actions
    environment: frogbot
    steps:
      - uses: actions/checkout@v4
        with:
          ref: ${{ github.event.pull_request.head.sha }}

      - uses: jfrog/frogbot@5d9c42c30f1169d8be4ba5510b40e75ffcbbc2a9  # v2.21.2
        env:
          # [Mandatory if the two conditions below are met]
          # 1. The project uses npm, yarn 2, NuGet or .NET to download its dependencies
          # 2. The `installCommand` variable isn't set in your frogbot-config.yml file.
          #
          # The command that installs the project dependencies (e.g "npm i", "nuget restore" or "dotnet restore")
          # JF_INSTALL_DEPS_CMD: ""

          # [Mandatory]
          # JFrog platform URL
          JF_URL: ${{ secrets.JF_URL }}

          # [Mandatory if JF_USER and JF_PASSWORD are not provided]
          # JFrog access token with 'read' permissions on Xray service
          JF_ACCESS_TOKEN: ${{ secrets.JF_ACCESS_TOKEN }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog username with 'read' permissions for Xray. Must be provided with JF_PASSWORD
          # JF_USER: ${{ secrets.JF_USER }}

          # [Mandatory if JF_ACCESS_TOKEN is not provided]
          # JFrog password. Must be provided with JF_USER
          # JF_PASSWORD: ${{ secrets.JF_PASSWORD }}

          # [Mandatory]
          # The GitHub token automatically generated for the job
          JF_GIT_TOKEN: ${{ secrets.GITHUB_TOKEN }}

          # [Optional]
          # If the machine that runs Frogbot has no access to the internat, set the name of a remote repository
          # in Artifactory, which proxies https://releases.jfrog.io/artifactory
          # The 'frogbot' executable and other tools it needs will be downloaded through this repository.
          # JF_RELEASES_REPO: ""

          # [Optional]
          # Frogbot will download the project dependencies, if they're not cached locally. To download the
          # dependencies from a virtual repository in Artifactory, set the name of of the repository. There's no
          # need to set this value, if it is set in the frogbot-config.yml file.
          # JF_DEPS_REPO: ""
