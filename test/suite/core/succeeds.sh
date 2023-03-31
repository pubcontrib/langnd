suite 'core/succeeds'

verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, null)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, true)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, 100)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, "text")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(null, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(false, true)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, false)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, true)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, 100)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, "text")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(true, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, 200)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(200, 100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, 100)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, -100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(-100, 100)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(32767.999999, -32767.999999)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(-32767.999999, 32767.999999)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, "text")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(100, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", "word")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("word", "text")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", "text")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", "TEXT")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("TEXT", "text")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("\a255", "\a000")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("\a000", "\a255")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", 100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds("text", <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1], [])), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([], [1])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], [1, 2])), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([3, 2, 1], [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], [3, 2, 1])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], 100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], "text")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds([1, 2, 3], <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1}, {})), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({}, {"a": 1})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, {"a": 1, "b": 2})), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"c": 3, "b": 2, "a": 1}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, {"c": 3, "b": 2, "a": 1})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 2}, {"a": 1})), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1}, {"a": 2})), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, 100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, "text")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds({"a": 1, "b": 2, "c": 3}, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, <>)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<>, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, <import "multiply" from core $number=argument return $multiply($number, $number)>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<1>, <2>)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<2>, <1>)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, null)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, 100)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, "text")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "succeeds"] from core $write($freeze($succeeds(<import "multiply" from core $number=argument return $multiply($number, $number)>, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'