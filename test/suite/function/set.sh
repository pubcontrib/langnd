suite 'function/set'

verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 1, "x")), 1)' \
    'prints to stdout' '"xbc"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 2, "x")), 1)' \
    'prints to stdout' '"axc"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 3, "x")), 1)' \
    'prints to stdout' '"abx"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 1, "")), 1)' \
    'prints to stdout' '"bc"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 2, "")), 1)' \
    'prints to stdout' '"ac"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 3, "")), 1)' \
    'prints to stdout' '"ab"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 1, "123")), 1)' \
    'prints to stdout' '"123bc"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 2, "123")), 1)' \
    'prints to stdout' '"a123c"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set("abc", 3, "123")), 1)' \
    'prints to stdout' '"ab123"'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, "x")), 1)' \
    'prints to stdout' '["x", "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 2, "x")), 1)' \
    'prints to stdout' '["a", "x", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 3, "x")), 1)' \
    'prints to stdout' '["a", "b", "x"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, null)), 1)' \
    'prints to stdout' '[null, "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, false)), 1)' \
    'prints to stdout' '[false, "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, 0)), 1)' \
    'prints to stdout' '[0, "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, "")), 1)' \
    'prints to stdout' '["", "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, [])), 1)' \
    'prints to stdout' '[[], "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, {})), 1)' \
    'prints to stdout' '[{}, "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set(["a", "b", "c"], 1, <>)), 1)' \
    'prints to stdout' '[<>, "b", "c"]'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", "x")), 1)' \
    'prints to stdout' '{"a": "x", "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "b", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": "x", "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "c", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": "x"}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", null)), 1)' \
    'prints to stdout' '{"a": null, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "d", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3, "d": "x"}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", false)), 1)' \
    'prints to stdout' '{"a": false, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", 0)), 1)' \
    'prints to stdout' '{"a": 0, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", "")), 1)' \
    'prints to stdout' '{"a": "", "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", [])), 1)' \
    'prints to stdout' '{"a": [], "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", {})), 1)' \
    'prints to stdout' '{"a": {}, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "set"] from core $write($freeze($set({"a": 1, "b": 2, "c": 3}, "a", <>)), 1)' \
    'prints to stdout' '{"a": <>, "b": 2, "c": 3}'

verify 'import ["set", "write", "freeze"] from core $list1=["x"] $list2=$set($list1, 1, $list1) $write($freeze($list1), 1) $write($freeze($list2), 1)' \
    'prints to stdout' '["x"][["x"]]'
verify 'import ["set", "write", "freeze"] from core $list=["x"] $list=$set($list, 1, $list) $write($freeze($list), 1)' \
    'prints to stdout' '[["x"]]'
verify 'import ["set", "write", "freeze"] from core $map1={"k": "x"} $map2=$set($map1, "k", $map1) $write($freeze($map1), 1) $write($freeze($map2), 1)' \
    'prints to stdout' '{"k": "x"}{"k": {"k": "x"}}'
verify 'import ["set", "write", "freeze"] from core $map1={"k": "x"} $map2=$set($map1, "m", $map1) $write($freeze($map1), 1) $write($freeze($map2), 1)' \
    'prints to stdout' '{"k": "x"}{"k": "x", "m": {"k": "x"}}'
verify 'import ["set", "write", "freeze"] from core $map={"k": "x"} $map=$set($map, "k", $map) $write($freeze($map), 1)' \
    'prints to stdout' '{"k": {"k": "x"}}'

verify 'import "set" from core $set("abc", 0, "x")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set("abc", 4, "x")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set("abc", 0, "")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set("abc", 4, "")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set("abc", 0, "xxx")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set("abc", 4, "xxx")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set(["a", "b", "c"], 0, "x")' \
    'errors with execute message' '"absent key"'
verify 'import "set" from core $set(["a", "b", "c"], 4, "x")' \
    'errors with execute message' '"absent key"'
