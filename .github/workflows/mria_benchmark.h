# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: #!/bin/bash
# Run full cluster benchmark
set -eu pipefail

# Perform OS check (we need `netem' and `iptables' features to inject
# faults/delays into the system, those are Linux-only):
[ $(uname) = Linux ] || {
    echo "Sorry, this script relies on some Linux IP stack features, and only works on Linux";
    exit 1;
}

export SCRIPT_DIR=$(dirname $0)

# Start nemesis process:
echo "Root permissions are needed to start nemesis process"
sudo -b ${SCRIPT_DIR}/nemesis.sh

# Run benchmark:
rebar3 do ct --name ct@127.0.0.1 --suite mria_mnesia_SUITE --case cluster_benchmark --readable=true

# Collect stats:
${SCRIPT_DIR}/latency_graph.gp
  push: #!/usr/bin/env -S gnuplot -persist
# GNUplot script to render plots of mria_mnesia performance

set datafile separator ','

set ylabel "Transaction time (μs)"
set xlabel "Cluster size"

set key outside right \
    title 'Network latency (μs)'

plot for [col=2:*] '/tmp/mnesia_stats.csv' using 1:col with linespoints pt 4 lc col dashtype 2 t 'mnesia '.columnhead(col), \
     for [col=2:*] '/tmp/mria_mnesia_stats.csv' using 1:col with linespoints pt 2 lc col t 'rlog '.columnhead(col)
    branches: [ "main" ]
  pull_request: #!/bin/bash
# This script runs as root, receives commands from the common test
# suite over FIFO, and forwards them to slowdown.sh
set -uo pipefail

FIFO=/tmp/nemesis

if [ -p $FIFO ]; then
    echo "Nemesis is already running"
    exit 0
fi

trap "rm -f $FIFO" EXIT

mkfifo $FIFO
chmod 666 $FIFO

while true; do
    if read line < $FIFO; then
        echo "Received command ${line}"
        $(dirname $0)/slowdown.sh -d $line -j 1 epmd
    fi
done
    branches: [ "main" ]
  schedule: #!/bin/bash
set -euo pipefail

[ $(uname) = Linux ] || {
    echo "Sorry, this script only works on Linux";
    exit 1;
}

[ -z ${1+1} ] && {
    echo "Emulate network latency on the localhost.

USAGE:

    $(basename $0) [ -d DELAY ] [ -j JITTER ] [ -r RATE ] [PORT1 PORT2 ...]

It is possible to specify PORT as 'empd' to apply delay to the
distribution ports of all running BEAM VMs (excluding the CT
master).

Both DELAY and JITTER should be more than 0

Port can be:

   1. A number: it will apply netem on the messages that are sent to
      and from the port

   2. A number with 'd' prefix (e.g. d1883): it will apply netem on
      the messages sent to the port

   3. A number with 's' prefix (e.g. s1883): it will apply netem on
      the messages sent from the port

   4. 'epmd': It will apply delay to all ports registered in epmd and
      apply netem to all erlang distribution protocol connections.

EXAMPLE:

    $(basename $0) -d 500ms -j 1 -r 10kbps 8001 s8002 empd
"
    exit 1;
}

DELAY="10ms"
JITTER=1
INTERFACE=lo
RATE=1000Mbps

while getopts "r:i:d:j:" flag; do
    case "$flag" in
        i) INTERFACE="$OPTARG";;
        d) DELAY="$OPTARG";;
        j) JITTER="$OPTARG";;
        r) RATE="$OPTARG";;
    esac
done
shift $((OPTIND-1))

CHAIN="OUTPUT"
# Clean up:
iptables -t mangle -F "$CHAIN" || true
tc qdisc del dev "$INTERFACE" root || true

echo "Delay=${DELAY} jitter=${JITTER} rate=${RATE} interface=${INTERFACE}"

# Shape packets marked as 12
MARK=12
ID=$MARK
tc qdisc add dev "$INTERFACE" root handle 1: htb
tc class add dev "$INTERFACE" parent 1: classid 1:$ID htb rate "$RATE"
# tc qdisc add dev "$INTERFACE" root netem rate "$RATE" delay "$DELAY" "$JITTER"
tc qdisc add dev "$INTERFACE" parent 1:$ID handle $MARK netem delay $DELAY $JITTER distribution normal
tc filter add dev "$INTERFACE" parent 1: prio 1 protocol ip handle $MARK fw flowid 1:$ID

mark() {
    echo "Applying netem on $1 $2"
    iptables -A "${CHAIN}" -p tcp -t mangle -j MARK --set-mark $MARK $1 $2
}

# Create firewall rules to mark the packets:
mark_port() {
    local PORT=$1
    if [[ $PORT =~ ^([0-9]+)$ ]]; then
        mark --sport $PORT
        mark --dport $PORT
    elif [[ $PORT =~ ^s([0-9]+)$ ]]; then
        PORT=${BASH_REMATCH[1]}
        mark --sport $PORT
    elif [[ $PORT =~ ^d([0-9]+)$ ]]; then
        PORT=${BASH_REMATCH[1]}
        mark --dport $PORT
    fi
}

while [ ! -z ${1+1} ]; do
    PORT=$1
    shift
    if [ $PORT = epmd ]; then
        for i in $(epmd -names | grep -v 'ct@' | awk '/at port/{print $5}'); do
            mark_port $i
        done
    else
        mark_port $PORT
    fi
done
    - cron: '35 10 * * 4'

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
