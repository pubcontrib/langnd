suite 'function/equals'

verify '@write(@freeze(@equals(null, null)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(null, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(null, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(null, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(null, "null")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(null, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(false, false)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(true, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(false, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, "true")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(true, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(0, 0)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(-100, -100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals(100, 0)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, -100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, "100")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals(100, [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals("text", "")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", "word")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", "TEXT")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("", "")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals("text", null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals("text", [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1], [])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([], [1])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], [1, 2])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([3, 2, 1], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], [3, 2, 1])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@equals([1, 2, 3], null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@equals([1, 2, 3], "text")), 1)' \
    'prints to stdout' 'false'
