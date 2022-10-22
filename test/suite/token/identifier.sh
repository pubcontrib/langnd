suite 'token/identifier'

verify '$$$var' \
    'errors with lex message' '$$$var'
verify '$' \
    'errors with lex message' '$'
verify '$var-me' \
    'errors with lex message' '-me'
verify '$-var' \
    'errors with lex message' '$-var'
verify '$"missing end' \
    'errors with lex message' '$"missing end'
