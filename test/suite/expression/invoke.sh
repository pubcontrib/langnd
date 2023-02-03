suite 'expression/invoke'

verify '$function=<> @write(@freeze($function()), 1)' \
    'prints to stdout' 'null'
verify '$function=<null> @write(@freeze($function()), 1)' \
    'prints to stdout' 'null'
verify '$function=<false> @write(@freeze($function()), 1)' \
    'prints to stdout' 'false'
verify '$function=<0> @write(@freeze($function()), 1)' \
    'prints to stdout' '0'
verify '$function=<""> @write(@freeze($function()), 1)' \
    'prints to stdout' '""'
verify '$function=<[]> @write(@freeze($function()), 1)' \
    'prints to stdout' '[]'
verify '$function=<{}> @write(@freeze($function()), 1)' \
    'prints to stdout' '{}'
verify '$function=<<>> @write(@freeze($function()), 1)' \
    'prints to stdout' '<>'
verify '$function=<null false 0 "" [] {} <>> @write(@freeze($function()), 1)' \
    'prints to stdout' '<>'
verify '$say=<@write("hey ", 1)> $say() $say() $say()' \
    'prints to stdout' 'hey hey hey '
verify '$index=<"first"> $index=<"second"> $index=<"third"> @write($index(), 1)' \
    'prints to stdout' 'third'
verify '$original=<@write("source", 1)> $copy=$original $copy()' \
    'prints to stdout' 'source'

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

verify '@huh()' \
    'errors with execute message' '"absent function"'
verify '@add' \
    'errors with execute message' '"unexpected reference type"'
verify '$huh()' \
    'errors with execute message' '"absent variable"'
verify '$value=null $value()' \
    'errors with execute message' '"invocation error"'
verify '$value=false $value()' \
    'errors with execute message' '"invocation error"'
verify '$value=0 $value()' \
    'errors with execute message' '"invocation error"'
verify '$value="" $value()' \
    'errors with execute message' '"invocation error"'
verify '$value=[] $value()' \
    'errors with execute message' '"invocation error"'
verify '$value={} $value()' \
    'errors with execute message' '"invocation error"'
verify '$function=<throw "error"> $function()' \
    'errors with execute message' '"error"'
verify '$function=<break "pick"> $function()' \
    'errors with execute message' '"lost break"'
verify '$function=<continue "pick"> $function()' \
    'errors with execute message' '"lost continue"'
