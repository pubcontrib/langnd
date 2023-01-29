suite 'expression/throw'

verify 'throw null' \
    'errors with execute message' 'null'
verify 'throw true' \
    'errors with execute message' 'true'
verify 'throw 100' \
    'errors with execute message' '100'
verify 'throw "text"' \
    'errors with execute message' '"text"'
verify 'throw [1, 2, 3]' \
    'errors with execute message' '[1, 2, 3]'
verify 'throw {"a": 1, "b": 2, "c": 3}' \
    'errors with execute message' '{"a": 1, "b": 2, "c": 3}'
verify '$var="PANIC!" throw $var' \
    'errors with execute message' '"PANIC!"'
verify 'throw @merge("PANIC", "!")' \
    'errors with execute message' '"PANIC!"'
verify '$error=catch \ @divide(100, 0) / throw $error' \
    'errors with execute message' '"arithmetic error"'
verify 'throw @merge($missing, "!")' \
    'errors with execute message' '"absent variable"'

verify 'throw "0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999"' \
    'errors with execute message' '"0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999"'

verify 'throw' \
    'errors with parse message' 'throw'
