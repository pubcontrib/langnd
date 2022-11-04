suite 'statement/throw'

verify 'throw null' \
    'errors with execute message' 'null'
verify 'throw true' \
    'errors with execute message' 'true'
verify 'throw 100' \
    'errors with execute message' '100'
verify 'throw "text"' \
    'errors with execute message' '"text"'
verify '$var="PANIC!" throw $var' \
    'errors with execute message' '"PANIC!"'
verify 'throw @merge("PANIC", "!")' \
    'errors with execute message' '"PANIC!"'
verify '$error=catch { @divide(100, 0) } throw $error' \
    'errors with execute message' '"arithmetic error"'
verify 'throw @merge($missing, "!")' \
    'errors with execute message' '"absent variable"'
