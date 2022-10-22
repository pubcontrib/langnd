suite 'token/keyword'

verify '@not(tru)' \
    'errors with lex message' 'tru)'
