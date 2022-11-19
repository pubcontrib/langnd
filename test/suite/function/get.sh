suite 'function/get'

verify '@write(@freeze(@get("abc", 1)), 1)' \
    'prints to stdout' '"a"'
verify '@write(@freeze(@get("abc", 2)), 1)' \
    'prints to stdout' '"b"'
verify '@write(@freeze(@get("abc", 3)), 1)' \
    'prints to stdout' '"c"'
verify '@write(@freeze(@get(["a", "b", "c"], 1)), 1)' \
    'prints to stdout' '"a"'
verify '@write(@freeze(@get(["a", "b", "c"], 2)), 1)' \
    'prints to stdout' '"b"'
verify '@write(@freeze(@get(["a", "b", "c"], 3)), 1)' \
    'prints to stdout' '"c"'
verify '@write(@freeze(@get({"a": 1, "b": 2, "c": 3}, "a")), 1)' \
    'prints to stdout' '1'
verify '@write(@freeze(@get({"a": 1, "b": 2, "c": 3}, "b")), 1)' \
    'prints to stdout' '2'
verify '@write(@freeze(@get({"a": 1, "b": 2, "c": 3}, "c")), 1)' \
    'prints to stdout' '3'

verify '$item=@get(["x"], 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$list=["x"] $item=@get($list, 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$list=[["x"]] $list=@get($list, 1) $item=@get($list, 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$item=@get({"k": "x"}, "k") @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$map={"k": "x"} $value=@get($map, "k") @write(@freeze($value), 1)' \
    'prints to stdout' '"x"'
verify '$map={"m": {"k": "x"}} $map=@get($map, "m") $value=@get($map, "k") @write(@freeze($value), 1)' \
    'prints to stdout' '"x"'

verify '@write(@freeze(@get("abc", 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get("abc", 4)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get(["a", "b", "c"], 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get(["a", "b", "c"], 4)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get({"a": 1, "b": 2, "c": 3}, "d")), 1)' \
    'errors with execute message' '"absent key"'
