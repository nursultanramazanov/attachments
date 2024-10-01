name: Rust

on: @startuml

bootstrap: Receiving all the records\n from the core node
local_replay: Replaying transactions\n that have been buffered locally\n during bootstrap
normal: Remote transactions are applied\ndirectly to the replica

[*] --> bootstrap
bootstrap --> local_replay : received bootstrap_complete
local_replay --> normal : reached the end of the local rlog

@enduml
  push: @startuml
scale 2000 width

participant "RLOG server" as server #ffc
participant "RLOG agent" as agent #ffc
participant "RLOG bootstrapper" as boot_serv #ffc

participant "RLOG replica" as repl #ccf
participant "RLOG replica\nimporter worker" as repl_imp #ccf
participant "bootstrap client" as boot_client #ccf

activate server
activate repl
activate repl_imp

group Agent initialization
== Probe the connection to minimize risk of double-subscription and negotiate protocol version ==
  repl -> server : probe
  repl <- server : {true, 0}
  note over repl: The protocol version\nshould be the same,\nabort connection if different

== Initiate connection ==
  repl -> server : {connect, LocalCheckpointTS}
  note over server : Spawn a new agent process\nunder the shard supervisor
  server -\\ agent : spawn(now() - SafeInterval)

  activate agent

  repl <- server : {need_bootstrap, AgentPID}
end

== Bootstrapping ==

group Bootstraper initialization
  hnote over repl : bootstrap

  repl -\\ boot_client : spawn()
  activate boot_client

  boot_client -> server : {bootstrap, self()}
  server -\\ boot_serv : spawn(RemotePid)
  activate boot_serv

  boot_serv -> boot_serv : mnesia:dirty_all_keys\nfor each table in shard

  server -> boot_client : {ok, Pid}
end

group Bootstrap
  note over boot_serv : Iterate through the\ncached keys
  loop
    boot_serv -> boot_client : {batch, [{Tab, Record}]}
    boot_client -> boot_client : import batch to the\ntable replica
    boot_serv <- boot_client : ok
  end

  note over agent : At the same time...

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch to the local rlog
  end

  boot_serv -> boot_client : bootstrap_complete
  deactivate boot_serv
  boot_client -> repl : bootstrap_complete
  deactivate boot_client
end

group local_replay
  hnote over repl : local_replay

  note over repl : Iterate through the\ncached transactions

  loop
    server -> agent : {trans, SeqNo, TLOG}
    agent -> repl : '#entry{}'
    repl -> repl : cache batch in the local rlog

    repl -> repl : Import ops from the local rlog\nto the local replica
  end

  note over repl : Reached the end of\nthe local rlog
end


== Normal operation ==

hnote over repl : normal

loop
  server -> agent : {trans, SeqNo, TLOG}
  agent -> repl : '#entry{SeqNo}'
  server -> agent : {trans, SeqNo + 1, TLOG}
  agent -> repl : '#entry{SeqNo + 1}'
  server -> agent : {trans, SeqNo + 2, TLOG}
  agent -> repl : '#entry{SeqNo + 2}'
  repl -> repl_imp : {import_batch, [Entries 1 2 3]}
  repl_imp -> repl_imp : Import batch to the\nlocal replica
  repl <- repl_imp : ok
end

@enduml
    branches: [ "main" ]
  pull_request: -type(cluster() :: atom()).

-type(member_status() :: joining | up | healing | leaving | down).

-type(member_address() :: {inet:ip_address(), inet:port_number()}).

-record(member, {
          node   :: node(),
          addr   :: undefined | member_address(),
          guid   :: undefined | mria_guid:guid(),
          hash   :: undefined | pos_integer(),
          status :: member_status(),
          mnesia :: undefined | running | stopped | false,
          ltime  :: undefined | erlang:timestamp(),
          role   :: mria_rlog:role()
         }).

-type(member() :: #member{}).

-define(JOIN_LOCK_ID, {mria_sync_join, node()}).
    branches: [ "main" ]

env: #!/bin/bash
set -euxo pipefail

echo "Building "${tag}""

[ -d "${tmp_dir}" ] ||
    git clone "${root_dir}" "${tmp_dir}"

cd "${tmp_dir}"
git checkout "${tag}"

# Avoid rebuilding rocksdb:
mkdir -p "_checkouts"
pushd "_checkouts"
[ -L rocksdb ] ||
    ln -s "${root_dir}/_build/default/lib/rocksdb" rocksdb
popd

[ ! -z $(find . -name mria.app) ] ||
    rebar3 as test compile
 CARGO_TERM_COLOR: module.exports.function = function buildMeowAudioInfo(meowAudio) {
  var audioInfo = {};

  /* Since meowAudio are already in audioItem format this mapping is not necessary, but
  this demonstrates how to build the right structure for audioItem and audioInfo */

  audioInfo.audioItem = meowAudio.map(function (audioItem) {
    let audioItemStructure = { //required properties of audioItem
      id: audioItem.id,
      stream: audioItem.stream,
      title: audioItem.title,
      artist:audioItem.artist,
      albumArtUrl: audioItem.albumArtUrl
    }
    //optional properties of audioItem
    if (audioItem.subtitle) {
      audioItemStructure.subtitle = audioItem.subtitle
    }
    if (audioItem.albumName) {
      audioItemStructure.albumName = audioItem.albumName
    }
    if (audioItem.duration) {
      audioItemStructure.duration = audioItem.duration
    }
    return audioItemStructure
  });

  audioInfo.category = 'RADIO';
  audioInfo.displayName = 'Meow Capsule';
  audioInfo.repeatMode = 'ALL';
  audioInfo.doNotWaitForTTS = false;

  return audioInfo;
}

jobs: var meowAudio = require('./meowAudio.js')

module.exports.function = function findMeow(searchTerm) {
    const keysToSearchOn = ['title', 'artist', 'subtitle', 'albumName']
    let meowAudioFound = []

    if (searchTerm) {
        searchTerm = searchTerm.toLowerCase()
        meowAudioFound = meowAudio.audioItems.filter(function (audioItem) {
            return keysToSearchOn.some(function (key) {
                return audioItem[key] && audioItem[key].toLowerCase().includes(searchTerm)
            })
        })
    } else {
        meowAudioFound = meowAudio.audioItems
    }
    return meowAudioFound
}
  build: module.exports.function = function playMeow () {
  return 'Meow';
}

    runs-on: const audioItems = [{
    id: 1,
    stream: [
      {
        url: "https://bigsoundbank.com/UPLOAD/mp3/1901.mp3",
        format: "mp3"
      }
    ],
    title: "Cat-ch Phrase",
    subtitle: "Meow.",
    artist: "Cool Cat",
    albumName: "Catatonic",
    albumArtUrl: "https://upload.wikimedia.org/wikipedia/commons/1/1d/1-month-old_kittens_32.jpg"
  },
  {
    id: 2,
    stream: [
      {
        url: "https://bigsoundbank.com/UPLOAD/mp3/1890.mp3",
        format: "mp3"
      }
    ],
    title: "Fur Real?",
    subtitle: "Meow meow.",
    artist: "Tom Cat",
    albumName: "You gotta be kitten me!",
    albumArtUrl: "https://upload.wikimedia.org/wikipedia/commons/b/bc/Juvenile_Ragdoll.jpg"
  },
  {
    id: 3,
    stream: [
      {
        url: "https://bigsoundbank.com/UPLOAD/mp3/1891.mp3",
        format: "mp3"
      }
    ],
    title: "Mew-sic",
    subtitle: "Meow meow meow.",
    artist: "Radi-claw",
    albumArtUrl: "https://upload.wikimedia.org/wikipedia/commons/f/f3/Youngkitten.JPG"
  }]

  exports.audioItems = audioItems

    steps: action (BuildMeowAudioInfo) {
  type (Search)
  description (Makes a meow audio info, aka a playlist, to play.)
  collect {
    input (meowAudio) {
      type (MeowAudio)
      min (Required) max (Many)
      default-init {
        intent {
          goal: FindMeow
        }
      }
    }
  }
  output (audioPlayer.AudioInfo)
}
    - uses: action (FindMeow) {
  type (Search)
  description (Finds meow audio item using a searchTerm)
  collect {
    input (searchTerm) {
      type (SearchTerm)
      min (Optional) max (One)
    }
  }
  output (MeowAudio)
}
    - name: structure (MeowAudio) {
  role-of (audioPlayer.AudioItem)
}
      run: // To play audio, there is a two step process
// First create the playlist (done via meowToPlay below)
// Second, pass the playlist to the audioPlayer (done via meow below)

action (PlayMeow) {
  type (Search)

  collect {
    input (meowToPlay) {
      description (Create the playlist to play)
      type (audioPlayer.AudioInfo)
      min (Required) max (One)
      default-init {
        intent {
          goal: BuildMeowAudioInfo
        } 
      }
      hidden     
    }

    computed-input (meow) {
      description (By passing in the AudioInfo object to the PlayAudio action, we ask the client to play our sound.)
      type (audioPlayer.Result)
      compute {
        intent {
          goal: audioPlayer.PlayAudio
          value: $expr(meowToPlay)
        }
      }
      hidden
    }
  }
  output (Result)
}
    - name: text (Result) {
  description (For confirmation on whether audio was played successfully or not)
}
      run: text (SearchTerm) {
  description (text users use to search for content)
}
 
