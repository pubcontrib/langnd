suite 'token/keyword'

verify 'import "not" from core $not(tru)' \
    'errors with lex message' 'tru)'
