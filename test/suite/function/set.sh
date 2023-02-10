suite 'function/set'

verify '@write(@freeze(@set("abc", 1, "x")), 1)' \
    'prints to stdout' '"xbc"'
verify '@write(@freeze(@set("abc", 2, "x")), 1)' \
    'prints to stdout' '"axc"'
verify '@write(@freeze(@set("abc", 3, "x")), 1)' \
    'prints to stdout' '"abx"'
verify '@write(@freeze(@set("abc", 1, "")), 1)' \
    'prints to stdout' '"bc"'
verify '@write(@freeze(@set("abc", 2, "")), 1)' \
    'prints to stdout' '"ac"'
verify '@write(@freeze(@set("abc", 3, "")), 1)' \
    'prints to stdout' '"ab"'
verify '@write(@freeze(@set("abc", 1, "123")), 1)' \
    'prints to stdout' '"123bc"'
verify '@write(@freeze(@set("abc", 2, "123")), 1)' \
    'prints to stdout' '"a123c"'
verify '@write(@freeze(@set("abc", 3, "123")), 1)' \
    'prints to stdout' '"ab123"'
verify '@write(@freeze(@set(["a", "b", "c"], 1, "x")), 1)' \
    'prints to stdout' '["x", "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 2, "x")), 1)' \
    'prints to stdout' '["a", "x", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 3, "x")), 1)' \
    'prints to stdout' '["a", "b", "x"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, null)), 1)' \
    'prints to stdout' '[null, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, false)), 1)' \
    'prints to stdout' '[false, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, 0)), 1)' \
    'prints to stdout' '[0, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, "")), 1)' \
    'prints to stdout' '["", "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, [])), 1)' \
    'prints to stdout' '[[], "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, {})), 1)' \
    'prints to stdout' '[{}, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, <>)), 1)' \
    'prints to stdout' '[<>, "b", "c"]'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", "x")), 1)' \
    'prints to stdout' '{"a": "x", "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "b", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": "x", "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "c", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": "x"}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", null)), 1)' \
    'prints to stdout' '{"a": null, "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "d", "x")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3, "d": "x"}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", false)), 1)' \
    'prints to stdout' '{"a": false, "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", 0)), 1)' \
    'prints to stdout' '{"a": 0, "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", "")), 1)' \
    'prints to stdout' '{"a": "", "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", [])), 1)' \
    'prints to stdout' '{"a": [], "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", {})), 1)' \
    'prints to stdout' '{"a": {}, "b": 2, "c": 3}'
verify '@write(@freeze(@set({"a": 1, "b": 2, "c": 3}, "a", <>)), 1)' \
    'prints to stdout' '{"a": <>, "b": 2, "c": 3}'

verify '$list1=["x"] $list2=@set($list1, 1, $list1) @write(@freeze($list1), 1) @write(@freeze($list2), 1)' \
    'prints to stdout' '["x"][["x"]]'
verify '$list=["x"] $list=@set($list, 1, $list) @write(@freeze($list), 1)' \
    'prints to stdout' '[["x"]]'
verify '$map1={"k": "x"} $map2=@set($map1, "k", $map1) @write(@freeze($map1), 1) @write(@freeze($map2), 1)' \
    'prints to stdout' '{"k": "x"}{"k": {"k": "x"}}'
verify '$map1={"k": "x"} $map2=@set($map1, "m", $map1) @write(@freeze($map1), 1) @write(@freeze($map2), 1)' \
    'prints to stdout' '{"k": "x"}{"k": "x", "m": {"k": "x"}}'
verify '$map={"k": "x"} $map=@set($map, "k", $map) @write(@freeze($map), 1)' \
    'prints to stdout' '{"k": {"k": "x"}}'

verify '@set("abc", 0, "x")' \
    'errors with execute message' '"absent key"'
verify '@set("abc", 4, "x")' \
    'errors with execute message' '"absent key"'
verify '@set("abc", 0, "")' \
    'errors with execute message' '"absent key"'
verify '@set("abc", 4, "")' \
    'errors with execute message' '"absent key"'
verify '@set("abc", 0, "xxx")' \
    'errors with execute message' '"absent key"'
verify '@set("abc", 4, "xxx")' \
    'errors with execute message' '"absent key"'
verify '@set(["a", "b", "c"], 0, "x")' \
    'errors with execute message' '"absent key"'
verify '@set(["a", "b", "c"], 4, "x")' \
    'errors with execute message' '"absent key"'
