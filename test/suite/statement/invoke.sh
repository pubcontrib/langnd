suite 'statement/invoke'

verify '@add(' \
    'errors with parse message' '('
verify '@add(,' \
    'errors with parse message' ','
verify '@add(,)' \
    'errors with parse message' ',)'
verify '@merge("1" "2")' \
    'errors with parse message' '"1" "2")'
verify '$x="1" $y="2" @merge($x $y)' \
    'errors with parse message' '$x $y)'
verify '@merge(@freeze(1) @freeze(2))' \
    'errors with parse message' ') @freeze(2))'
verify '$a=1 @add($a=2, $a=3)' \
    'errors with parse message' '2, $a=3)'

verify '@huh()' \
    'errors with execute message' '"absent function"'
verify '@add' \
    'errors with execute message' '"unexpected reference type"'
