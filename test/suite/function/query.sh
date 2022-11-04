suite 'function/query'

LND_TEST_VALUE=
export LND_TEST_VALUE
verify '@write(@query("LND_TEST_VALUE"), 1)' \
    'prints to stdout' ''
LND_TEST_VALUE='100'
export LND_TEST_VALUE
verify '@write(@query("LND_TEST_VALUE"), 1)' \
    'prints to stdout' '100'
LND_TEST_VALUE='TEXT'
export LND_TEST_VALUE
verify '@write(@query("LND_TEST_VALUE"), 1)' \
    'prints to stdout' 'TEXT'
unset LND_TEST_VALUE
verify '@write(@query("LND_TEST_VALUE"), 1)' \
    'errors with execute message' '"absent environment variable"'
