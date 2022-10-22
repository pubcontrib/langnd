suite 'token/string'

verify '"missing end' \
    'errors with lex message' '"missing end'
verify 'missing start"' \
    'errors with lex message' 'missing start"'
