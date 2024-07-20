# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: /**
* CSS3 No-image Chupa-Chupsy candy 
* by @girlie_mac 
* http://girliemac.com/blog/2012/03/24/making-chupa-chups-using-css3-pseudo-elements/
*
* Please, do not compare this with the real one. 
* I feel pretty terrible making the awesome design by Salvador Dalí into some crap.
* But my point of this demo is that you can create the floral shape with CSS, using border-radius:50% and pseudo-element(s)
*/
html {
    background: #444;
    background: linear-gradient(135deg, #555, #222);
    min-height: 100%;
    margin: 0;
    padding: 2em;
}
p {
    margin: 0;    
    padding: 0;
}
body > div {
    margin: 30px
}
.chupa-chups {
    width: 300px;
    height: 300px;
    border-radius: 50%;
}
.chupa-chups.orange {
    background: orange;
    background: linear-gradient(325deg, gold 20%, orangered);
}
.chupa-chups.green {
    background: green;
    background: linear-gradient(325deg, YellowGreen 20%, darkgreen);
}
.chupa-chups.pink {
    background: HotPink;
    background: linear-gradient(325deg, HotPink 20%, FireBrick);
}
.chupa-chups.blue {
    background: MediumBlue;
    background: linear-gradient(325deg, RoyalBlue 20%, MediumBlue);
}
.chupa-chups p {
    z-index: 1;
    position: absolute;
    top: 26px; left: 10px;
    color: #ce1141;
    font-family: Pacifico, cursive;
    font-size: 68px;
    line-height: 70px;
    text-align: center;
    text-shadow: 1px 2px 0 #fff;
}
.floral-shape,  
.floral-shape::before, 
.floral-shape::after {
    width: 200px;       
    height: 200px;
    border-radius: 25%;
    border: 6px solid #ce1141;
    background: yellow;
    background: linear-gradient(325deg, yellow 30%, orange);
}
.floral-shape {
    position: relative;
    top: 42px;
    left: 42px;
}
.floral-shape::before {
    content: "";
    position: absolute;
    top: -3px; left: -3px; /* adjust for the border-width */
    background: linear-gradient(370deg, yellow 30%, orange);
    transform: rotate(45deg);
}
.floral-shape::after {
    content: "";
    position: absolute;
    border: 0 none;
    box-shadow: none;
}
  push: 
<link href='http://fonts.googleapis.com/css?family=Pacifico' rel='stylesheet' type='text/css'>

<!-- Chupa-Chupsy Candy -->
<div>
<div class="chupa-chups orange">
    <div class="floral-shape">
        <p>Chupa<br/>Chups</p>
    </div>
</div>
</div>


<div>
<div class="chupa-chups green">
    <div class="floral-shape">
        <p>Chupa<br/>Chups</p>
    </div>
</div>
</div>


<div>
<div class="chupa-chups pink">
    <div class="floral-shape">
        <p>Chupa<br/>Chups</p>
    </div>
</div>
</div>


<div>
<div class="chupa-chups blue">
    <div class="floral-shape">
        <p>Chupa<br/>Chups</p>
    </div>
</div>
</div>
    branches: [ "main" ]
  pull_request: {"view":"split-vertical","seethrough":"","prefixfree":"1","page":"css"}
    branches: [ "main" ]
  schedule:
    - cron: '30 7 * * 3'

env:
  # Path to the CMake build directory.
  build: '${{ github.workspace }}/build'

permissions:
  contents: read

jobs:
  analyze:
    permissions:
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ env.build }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with:
          cmakeBuildDirectory: ${{ env.build }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
