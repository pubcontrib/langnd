suite 'expression/branch'

verify 'if false \ @write("hit", 1) /' \
    'prints to stdout' ''
verify 'if true \ @write("hit", 1) /' \
    'prints to stdout' 'hit'
verify 'if true \ if true \ @write("hit", 1) / /' \
    'prints to stdout' 'hit'
verify 'if true \ if false \ @write("hit", 1) / /' \
    'prints to stdout' ''
verify 'if false \ if true \ @write("hit", 1) / /' \
    'prints to stdout' ''
verify 'if false \ if false \ @write("hit", 1) / /' \
    'prints to stdout' ''
verify '$a=true if $a \ @write("hit", 1) /' \
    'prints to stdout' 'hit'
verify '$a=false if $a \ @write("hit", 1) /' \
    'prints to stdout' ''
verify 'if @cast("true", "BOOLEAN") \ @write("hit", 1) /' \
    'prints to stdout' 'hit'
verify 'if @cast("false", "BOOLEAN") \ @write("hit", 1) /' \
    'prints to stdout' ''
verify '$a=0 if true \ $a=@add($a,1) $a=@add($a,1) $a=@add($a,1) / @write(@freeze($a),1)' \
    'prints to stdout' '3'
verify '$msg=if true \ "first" "..." "last" / @write(@freeze($msg), 1)' \
    'prints to stdout' '"last"'
verify '$msg=if false \ "first" "..." "last" / @write(@freeze($msg), 1)' \
    'prints to stdout' 'null'
verify '@write(@merge("\\", if true \ " /" /), 1)' \
    'prints to stdout' '\ /'
verify 'if true \ @write("true", 1) / otherwise \ @write("false", 1) /' \
    'prints to stdout' 'true'
verify 'if false \ @write("true", 1) / otherwise \ @write("false", 1) /' \
    'prints to stdout' 'false'
verify '$msg=if true \ "first" / otherwise \ "second" / @write(@freeze($msg), 1)' \
    'prints to stdout' '"first"'
verify '$msg=if false \ "first" / otherwise \ "second" / @write(@freeze($msg), 1)' \
    'prints to stdout' '"second"'
verify '$b=1 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) /' \
    'prints to stdout' 'first'
verify '$b=2 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) /' \
    'prints to stdout' 'second'
verify '$b=3 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) /' \
    'prints to stdout' ''
verify '$b=1 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) / otherwise \ @write("third", 1) /' \
    'prints to stdout' 'first'
verify '$b=2 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) / otherwise \ @write("third", 1) /' \
    'prints to stdout' 'second'
verify '$b=3 if @equals($b, 1) \ @write("first", 1) / else if @equals($b, 2) \ @write("second", 1) / otherwise \ @write("third", 1) /' \
    'prints to stdout' 'third'
verify 'if true \ @write("first ", 1) / if true \ @write("second ", 1) / if true \ @write("third", 1) /' \
    'prints to stdout' 'first second third'

verify 'if' \
    'errors with parse message' 'if'
verify 'if \ /' \
    'errors with parse message' '/'
verify 'if true' \
    'errors with parse message' 'true'
verify 'if true \' \
    'errors with parse message' '\'
verify 'if true /' \
    'errors with parse message' '/'
verify 'if true \ 100' \
    'errors with parse message' '100'
verify 'if true \ 100 / else' \
    'errors with parse message' 'else'
verify 'if true \ 100 / else \' \
    'errors with parse message' 'else \'
verify 'if true \ 100 / else /' \
    'errors with parse message' 'else /'
verify 'if true \ 100 / else \ 200' \
    'errors with parse message' 'else \ 200'
verify 'if true \ 100 / else if' \
    'errors with parse message' 'if'
verify 'if true \ 100 / else if true' \
    'errors with parse message' 'true'
verify 'if true \ 100 / else if true \' \
    'errors with parse message' '\'
verify 'if true \ 100 / else if true /' \
    'errors with parse message' '/'
verify 'if true \ 100 / else if true \ 200' \
    'errors with parse message' '200'
verify 'if true \ 100 / else catch \ /' \
    'errors with parse message' 'else catch \ /'
verify 'if true \ 100 / otherwise' \
    'errors with parse message' 'otherwise'
verify 'if true \ 100 / otherwise \' \
    'errors with parse message' '\'
verify 'if true \ 100 / otherwise /' \
    'errors with parse message' '/'
verify 'if true \ 100 / otherwise \ 200' \
    'errors with parse message' '200'

verify 'if null \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if 100 \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if "text" \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if [1, 2, 3] \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if {"a": 1, "b": 2, "c": 3} \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if false \ / else if null \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if false \ / else if 100 \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if false \ / else if "text" \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if false \ / else if [1, 2, 3] \ /' \
    'errors with execute message' '"branch with non-boolean condition"'
verify 'if false \ / else if {"a": 1, "b": 2, "c": 3} \ /' \
    'errors with execute message' '"branch with non-boolean condition"'

verify 'if true \ "before" @divide(100, 0) "after" /' \
    'errors with execute message' '"arithmetic error"'
verify 'if @divide(100, 0) \ $missing /' \
    'errors with execute message' '"arithmetic error"'
verify 'if false \ / else if true \ "before" @divide(100, 0) "after" /' \
    'errors with execute message' '"arithmetic error"'
verify 'if false \ / else if @divide(100, 0) \ $missing /' \
    'errors with execute message' '"arithmetic error"'
