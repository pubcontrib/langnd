suite 'expression/null'

verify 'null' \
    'prints to stdout' ''
verify 'nullnull' \
    'prints to stdout' ''
verify 'null null' \
    'prints to stdout' ''

verify 'NULL' \
    'errors with lex message' 'NULL'
