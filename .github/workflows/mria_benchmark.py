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
  pull_request: from .main import json_normalize
from .key_joiners import last_node_name

__all__ = [
    "json_normalize",
    "last_node_name",
]

__version__ = "1.1.0"

VERSION = __version__
    branches: [ "main" ]

env: def last_node_name(node_names):
    return node_names[-1]


def n_last_node_name(n, delimiter="."):
    def temp(node_names):
        return delimiter.join(node_names[-n:])

    return temp
  CARGO_TERM_COLOR: from itertools import chain, product
from collections import defaultdict
from typing import Literal, Iterable, Callable, Generator, Any, Dict, Tuple, Union


def json_normalize(
    tree: Union[dict, Iterable],
    combine_lists: Literal["chain", "product"] = None,
    drop_nodes: Iterable[str] = (),
    freeze_nodes: Iterable[str] = (),
    key_joiner: Union[str, Callable] = ".",
) -> Generator[dict, None, None]:
    """
    Will flatten a dict-list structure to a list of flat dicts.
    >>> list(json_normalize({'a': {'A': 123}, 'b':[{'c':1},{'c':2}]}))
    [{'a.A': 123, 'b.c': 1}, {'a.A': 123, 'b.c': 2}]

    Dropping certain branches
    >>> list(json_normalize({'a': {'A': 123}, 'b':[{'c':1},{'c':2}]}, drop_nodes=('a',)))
    [{'b.c': 1}, {'b.c': 2}]

    Custom paths
    >>> list(json_normalize({'a': {'b':1}}, key_joiner=lambda p: p[-1].upper()))
    [{'B': 1}]
    """
    _validate_input(
        tree,
        combine_lists,
        drop_nodes,
        freeze_nodes,
        key_joiner,
    )

    if isinstance(key_joiner, str):
        key_joiner_str = key_joiner

        def key_joiner(path):
            return key_joiner_str.join(path)

    elif isinstance(key_joiner, Callable):
        pass
    else:
        raise TypeError(f"key_joiner has to be either a Callable or a str, got {type(key_joiner)}")

    flattened = _json_normalize(
        tree,
        combine_lists=combine_lists,
        drop_nodes=set(drop_nodes),
        freeze_nodes=set(freeze_nodes),
        key_joiner=key_joiner,
        tree_name=(),
    )
    return map(
        lambda p: _apply_key_joiner(key_joiner, p),
        flattened,
    )


def _validate_input(
    tree,
    combine_lists,
    drop_nodes,
    freeze_nodes,
    key_joiner,
):
    allowed_values = ("chain", "product", None)
    if combine_lists not in allowed_values:
        raise ValueError(f"combine_lists allowed values: {allowed_values}, got {combine_lists}")


def _json_normalize(tree, **kwargs) -> Generator[Dict[Tuple, Any], None, None]:
    if isinstance(tree, dict):
        current_node_name = kwargs.pop("tree_name", ())
        freeze_nodes = kwargs.get("freeze_nodes", ())
        drop_nodes = kwargs.get("drop_nodes", ())

        tree = _flatten_dict(tree, current_node_name, drop_nodes, freeze_nodes)
        leaves = _leaves(tree, freeze_nodes)
        branches = _branches(tree, kwargs, freeze_nodes)

        for branch_data in _combine_branches(branches, kwargs["combine_lists"]):
            yield _merge_dicts(leaves, *branch_data)

    elif not _is_leaf(tree):
        for branch in tree:
            for flatted_branch in _json_normalize(branch, **kwargs):
                yield flatted_branch

    else:
        yield {kwargs["tree_name"]: tree}


def _flatten_dict(a_dict, name, drop_nodes, freeze_nodes) -> Dict[Tuple[str], Any]:
    out = {}
    for k, v in a_dict.items():
        node_name = (*name, k)
        if k in drop_nodes:
            pass
        elif isinstance(v, dict) and k not in freeze_nodes:
            out.update(_flatten_dict(v, node_name, drop_nodes, freeze_nodes))
        else:
            out[node_name] = v
    return out


def _branches(tree, kwargs, freeze_nodes):
    return [
        _json_normalize(v, **kwargs, tree_name=k)
        for k, v in tree.items()
        if not _is_leaf(v) and k[-1] not in freeze_nodes
    ]


def _leaves(tree, freeze_nodes):
    return {k: v for k, v in tree.items() if (_is_leaf(v) or k[-1] in freeze_nodes)}


def _is_leaf(node):
    return isinstance(node, str) or not isinstance(node, Iterable)


def _combine_branches(branches, method) -> Iterable[Tuple[Dict]]:
    if not branches:
        return ({},)
    elif len(branches) == 1:
        return map(lambda x: (x,), branches[0])
    elif method == "product":
        return product(*branches)
    elif method == "chain":
        return map(lambda x: (x,), chain.from_iterable(branches))
    else:
        raise ValueError("Multiple branches dont know how to handle these, either ")


def _apply_key_joiner(key_joiner, raw):
    out = {key_joiner(k): v for k, v in raw.items()}

    if len(out) != len(raw):
        msg = _build_helper_message(key_joiner, raw)
        raise ValueError(f"Multiple raw keys were writtern to the same key. \n{msg}")
    return out


def _build_helper_message(key_joiner, raw):
    helper = defaultdict(list)
    for k in raw:
        helper[key_joiner(k)].append(k)

    helper = {k: v for k, v in helper.items() if len(v) > 1}
    msg = "\n\n".join("\n".join(f"\t{v_i} -> {k}" for v_i in v) for k, v in helper.items())
    return msg


def _merge_dicts(*dicts):
    output = {}
    for d in dicts:
        output.update(d)
    return output

jobs:            
  build: from json_normalize import json_normalize
import pytest


def test_combine_lists_chain():
    tree = {
        "a": [1, 2],
        "b": [
            {"x": "hej"},
            {"x": "san"},
        ],
    }
    expected = [
        {"a": 1},
        {"a": 2},
        {"b.x": "hej"},
        {"b.x": "san"},
    ]
    actual = json_normalize(tree, combine_lists="chain")
    assert list(actual) == expected


def test_combine_lists_product():
    tree = {
        "a": [1, 2],
        "b": [3, 4],
    }
    expected = [
        {"a": 1, "b": 3},
        {"a": 1, "b": 4},
        {"a": 2, "b": 3},
        {"a": 2, "b": 4},
    ]
    actual = json_normalize(tree, combine_lists="product")
    assert list(actual) == expected


def test_multiple_dependant_lists():
    tree = {
        "a": 1,
        "b": [{"x": "1"}, {"x": "2"}],
        "c": {
            "d": [
                {"y": "1"},
                {"y": "2"},
            ],
        },
    }
    expected = [
        {"a": 1, "b.x": "1", "c.d.y": "1"},
        {"a": 1, "b.x": "1", "c.d.y": "2"},
        {"a": 1, "b.x": "2", "c.d.y": "1"},
        {"a": 1, "b.x": "2", "c.d.y": "2"},
    ]
    actual = json_normalize(tree, combine_lists="product")
    assert list(actual) == expected


def test_combine_lists_default_none_should_raise_error():
    tree = {
        "a": [1, 2],
        "b": [1, 2],
    }
    with pytest.raises(ValueError):
        list(json_normalize(tree))


def test_improper_input():
    with pytest.raises(ValueError):
        list(json_normalize({}, combine_lists="This should not work"))

    runs-on: from json_normalize import json_normalize


def test_ignore_nodes_single_node():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
            {"c": "y", "d": 3},
        ],
    }
    expected = [
        {"b.c": "x", "b.d": 2},
        {"b.c": "y", "b.d": 3},
    ]
    actual = json_normalize(tree, drop_nodes=("a",))
    assert list(actual) == expected


def test_ignore_nodes_entire_branch():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
            {"c": "y", "d": 3},
        ],
    }
    expected = [
        {"a": 1},
    ]
    actual = json_normalize(tree, drop_nodes=("b",))
    assert list(actual) == expected

    steps: from json_normalize import json_normalize


def test_freeze_nodes_single_node():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
            {"c": "y", "d": 3},
        ],
    }
    expected = [
        {
            "a": 1,
            "b.c": "x",
            "b.d": 2,
        },
        {
            "a": 1,
            "b.c": "y",
            "b.d": 3,
        },
    ]
    actual = json_normalize(tree, freeze_nodes=("a",))
    assert list(actual) == expected


def test_freeze_nodes_entire_branch():
    tree = {
        "a": 1,
        "b": [1, 2, 3],
        "c": ["x", "y"],
    }
    expected = [
        {
            "a": 1,
            "b": [1, 2, 3],
            "c": "x",
        },
        {
            "a": 1,
            "b": [1, 2, 3],
            "c": "y",
        },
    ]
    actual = json_normalize(tree, freeze_nodes=("b",))
    assert list(actual) == expected
    - uses: from json_normalize import json_normalize


def test_listless_tree():
    tree = {
        "a": 1,
        "b": {
            "c": "x",
            "d": 2,
        },
    }
    expected = [
        {
            "a": 1,
            "b.c": "x",
            "b.d": 2,
        }
    ]
    actual = json_normalize(tree)
    assert list(actual) == expected


def test_tree_with_list_independant_items():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
            {"c": "y", "d": 3},
        ],
    }
    expected = [
        {
            "a": 1,
            "b.c": "x",
            "b.d": 2,
        },
        {
            "a": 1,
            "b.c": "y",
            "b.d": 3,
        },
    ]
    actual = json_normalize(tree)
    assert list(actual) == expected


def test_list_of_values():
    tree = {
        "a": 1,
        "b": [1, 2, 3],
    }
    expected = [
        {"a": 1, "b": 1},
        {"a": 1, "b": 2},
        {"a": 1, "b": 3},
    ]
    actual = json_normalize(tree)
    assert list(actual) == expected


def test_list_input():
    tree = [
        {"a": 1, "b": {"d": 2}},
        {
            "a": 2,
            "b": {"d": 4},
        },
    ]
    expected = [
        {"a": 1, "b.d": 2},
        {"a": 2, "b.d": 4},
    ]
    actual = json_normalize(tree)
    assert list(actual) == expected


def test_nested_list_input():
    tree = [
        {"a": 1, "b": [1, 2]},
        {
            "a": 2,
            "b": [3, 4],
        },
    ]
    expected = [
        {"a": 1, "b": 1},
        {"a": 1, "b": 2},
        {"a": 2, "b": 3},
        {"a": 2, "b": 4},
    ]
    actual = json_normalize(tree)
    assert list(actual) == expected
    - name: from json_normalize import json_normalize, last_node_name
import pytest


def test_custom_key_joiner_string():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
        ],
    }
    expected = [
        {"a": 1, "b-c": "x", "b-d": 2},
    ]
    actual = json_normalize(tree, key_joiner="-")
    assert list(actual) == expected


def test_custom_key_joiner_function():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
        ],
    }
    expected = [
        {"a": 1, "b-c": "x", "b-d": 2},
    ]
    actual = json_normalize(tree, key_joiner=lambda p: "-".join(p))
    assert list(actual) == expected


def test_custom_key_joiner_function_last_node_name():
    tree = {
        "a": 1,
        "b": [
            {"c": "x", "d": 2},
        ],
    }
    expected = [
        {"a": 1, "c": "x", "d": 2},
    ]
    actual = json_normalize(tree, key_joiner=last_node_name)
    assert list(actual) == expected


def test_overwriting_names_should_raise_error():
    tree = {
        "a": 1,
        "b": {
            "a": "1",
        },
    }
    with pytest.raises(ValueError):
        list(json_normalize(tree, key_joiner=last_node_name))


def test_improper_input_type():
    with pytest.raises(TypeError):
        list(json_normalize({}, key_joiner=dict()))
      run: from json_normalize import json_normalize


def test_generator_input():
    def a_dict_generator():
        yield {"a": 1}
        yield {"a": 2}

    expected = [
        {"a": 1},
        {"a": 2},
    ]
    actual = json_normalize(a_dict_generator())
    assert list(actual) == expected


def test_nested_generators():
    def a_dict_generator(n):
        yield {"a": n}
        yield {"a": 2 * n}

    def another_dict_generator():
        yield {"b": 3, "c": a_dict_generator(1)}
        yield {"b": 4, "c": a_dict_generator(2)}

    expected = [
        {"b": 3, "c.a": 1},
        {"b": 3, "c.a": 2},
        {"b": 4, "c.a": 2},
        {"b": 4, "c.a": 4},
    ]
    actual = json_normalize(another_dict_generator())
    assert list(actual) == expected
    - name: -type(cluster() :: atom()).

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
      run: #!/bin/bash
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
