suite 'function/succeeds'

verify '@write(@freeze(@succeeds(null, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(null, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(null, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(null, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(null, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(false, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(true, false)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(true, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(true, null)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(true, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(true, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(true, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(100, 200)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(200, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(100, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(100, -100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(-100, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(32767.999999, -32767.999999)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(-32767.999999, 32767.999999)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(100, null)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(100, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds(100, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds(100, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds("text", "word")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds("word", "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds("text", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds("text", "TEXT")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds("TEXT", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds("text", null)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds("text", 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds("text", true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds("text", [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds([1], [])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([], [1])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds([1, 2, 3], [1, 2])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([1, 2], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds([3, 2, 1], [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([1, 2, 3], [3, 2, 1])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds([1, 2, 3], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@succeeds([1, 2, 3], null)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([1, 2, 3], 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([1, 2, 3], true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@succeeds([1, 2, 3], "text")), 1)' \
    'prints to stdout' 'true'
