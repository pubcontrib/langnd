suite 'statement/string'

verify '"text"' \
    'prints to stdout' ''
verify '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"' \
    'prints to stdout' ''

verify '"' \
    'errors with lex message' '"'
verify '"
"' \
    'errors with lex message' '"'
verify '"'"`printf '\007'`"'"' \
    'errors with lex message' '"'"`printf '\007'`"'"'
