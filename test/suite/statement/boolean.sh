suite 'statement/boolean'

verify 'false' \
    'prints to stdout' ''
verify 'true' \
    'prints to stdout' ''

verify 'FALSE' \
    'errors with lex message' 'FALSE'
verify 'TRUE' \
    'errors with lex message' 'TRUE'
