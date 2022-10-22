suite 'function/precedes'

verify '@write(@freeze(@precedes(null, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(null, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(false, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, 200)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(200, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, -100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(-100, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(32767.999999, -32767.999999)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(-32767.999999, 32767.999999)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", "word")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("word", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", "TEXT")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("TEXT", "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", true)), 1)' \
    'prints to stdout' 'false'
