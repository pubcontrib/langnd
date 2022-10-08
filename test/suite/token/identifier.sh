suite 'token/identifier'

lexfail '$$$var' '$$$var'
lexfail '$' '$'
lexfail '$var-me' '-me'
lexfail '$-var' '$-var'
lexfail '$"missing end' '$"missing end'
