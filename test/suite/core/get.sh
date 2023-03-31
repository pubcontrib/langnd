suite 'core/get'

verify 'import ["write", "freeze", "get"] from core $write($freeze($get("abc", 1)), 1)' \
    'prints to stdout' '"a"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get("abc", 2)), 1)' \
    'prints to stdout' '"b"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get("abc", 3)), 1)' \
    'prints to stdout' '"c"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get(["a", "b", "c"], 1)), 1)' \
    'prints to stdout' '"a"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get(["a", "b", "c"], 2)), 1)' \
    'prints to stdout' '"b"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get(["a", "b", "c"], 3)), 1)' \
    'prints to stdout' '"c"'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get({"a": 1, "b": 2, "c": 3}, "a")), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get({"a": 1, "b": 2, "c": 3}, "b")), 1)' \
    'prints to stdout' '2'
verify 'import ["write", "freeze", "get"] from core $write($freeze($get({"a": 1, "b": 2, "c": 3}, "c")), 1)' \
    'prints to stdout' '3'

verify 'import ["get", "write", "freeze"] from core $item=$get(["x"], 1) $write($freeze($item), 1)' \
    'prints to stdout' '"x"'
verify 'import ["get", "write", "freeze"] from core $list=["x"] $item=$get($list, 1) $write($freeze($item), 1)' \
    'prints to stdout' '"x"'
verify 'import ["get", "write", "freeze"] from core $list=[["x"]] $list=$get($list, 1) $item=$get($list, 1) $write($freeze($item), 1)' \
    'prints to stdout' '"x"'
verify 'import ["get", "write", "freeze"] from core $item=$get({"k": "x"}, "k") $write($freeze($item), 1)' \
    'prints to stdout' '"x"'
verify 'import ["get", "write", "freeze"] from core $map={"k": "x"} $value=$get($map, "k") $write($freeze($value), 1)' \
    'prints to stdout' '"x"'
verify 'import ["get", "write", "freeze"] from core $map={"m": {"k": "x"}} $map=$get($map, "m") $value=$get($map, "k") $write($freeze($value), 1)' \
    'prints to stdout' '"x"'

verify 'import "get" from core $get("abc", 0)' \
    'errors with execute message' '"absent key"'
verify 'import "get" from core $get("abc", 4)' \
    'errors with execute message' '"absent key"'
verify 'import "get" from core $get(["a", "b", "c"], 0)' \
    'errors with execute message' '"absent key"'
verify 'import "get" from core $get(["a", "b", "c"], 4)' \
    'errors with execute message' '"absent key"'
verify 'import "get" from core $get({"a": 1, "b": 2, "c": 3}, "d")' \
    'errors with execute message' '"absent key"'
