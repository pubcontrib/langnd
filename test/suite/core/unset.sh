suite 'core/unset'

verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset("abc", 1)), 1)' \
    'prints to stdout' '"bc"'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset("abc", 2)), 1)' \
    'prints to stdout' '"ac"'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset("abc", 3)), 1)' \
    'prints to stdout' '"ab"'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset(["a", "b", "c"], 1)), 1)' \
    'prints to stdout' '["b", "c"]'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset(["a", "b", "c"], 2)), 1)' \
    'prints to stdout' '["a", "c"]'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset(["a", "b", "c"], 3)), 1)' \
    'prints to stdout' '["a", "b"]'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset({"a": 1, "b": 2, "c": 3}, "a")), 1)' \
    'prints to stdout' '{"b": 2, "c": 3}'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset({"a": 1, "b": 2, "c": 3}, "b")), 1)' \
    'prints to stdout' '{"a": 1, "c": 3}'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset({"a": 1, "b": 2, "c": 3}, "c")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2}'
verify 'import ["write", "freeze", "unset"] from core $write($freeze($unset({"a": 1, "b": 2, "c": 3}, "d")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'

verify 'import ["set", "unset", "write", "freeze"] from core $list1=["x"] $list1=$set($list1, 1, $list1) $list2=$unset($list1, 1) $write($freeze($list1), 1) $write($freeze($list2), 1)' \
    'prints to stdout' '[["x"]][]'
verify 'import ["set", "unset", "write", "freeze"] from core $list=["x"] $list=$set($list, 1, $list) $list=$unset($list, 1) $write($freeze($list), 1)' \
    'prints to stdout' '[]'

verify 'import "unset" from core $unset("abc", 0)' \
    'errors with execute message' '"absent key"'
verify 'import "unset" from core $unset("abc", 4)' \
    'errors with execute message' '"absent key"'
verify 'import "unset" from core $unset(["a", "b", "c"], 0)' \
    'errors with execute message' '"absent key"'
verify 'import "unset" from core $unset(["a", "b", "c"], 4)' \
    'errors with execute message' '"absent key"'
