suite 'statement/invoke'

parsefail '@add(' '('
parsefail '@add(,' ','
parsefail '@add(,)' ',)'
parsefail '@merge("1" "2")' '"1" "2")'
parsefail '$x="1" $y="2" @merge($x $y)' '$x $y)'
parsefail '@merge(@freeze(1) @freeze(2))' ') @freeze(2))'
parsefail '$a=1 @add($a=2, $a=3)' '2, $a=3)'

executefail '@huh()' 'absent function'
executefail '@add' 'unexpected reference type'
