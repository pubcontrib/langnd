suite 'token/whitespace'

verify '' \
    'prints to stdout' ''
verify ' ' \
    'prints to stdout' ''
verify "`printf '\011'`" \
    'prints to stdout' ''
verify "`printf '\012'`" \
    'prints to stdout' ''
verify "`printf '\015'`" \
    'prints to stdout' ''
