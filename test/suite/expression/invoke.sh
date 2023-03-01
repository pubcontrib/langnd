suite 'expression/invoke'

verify 'import ["write", "freeze"] from core $function=<> $write($freeze($function()), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $function=<null> $write($freeze($function()), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $function=<false> $write($freeze($function()), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze"] from core $function=<0> $write($freeze($function()), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze"] from core $function=<""> $write($freeze($function()), 1)' \
    'prints to stdout' '""'
verify 'import ["write", "freeze"] from core $function=<[]> $write($freeze($function()), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze"] from core $function=<{}> $write($freeze($function()), 1)' \
    'prints to stdout' '{}'
verify 'import ["write", "freeze"] from core $function=<<>> $write($freeze($function()), 1)' \
    'prints to stdout' '<>'
verify 'import ["write", "freeze"] from core $function=<null false 0 "" [] {} <>> $write($freeze($function()), 1)' \
    'prints to stdout' '<>'
verify '$say=<import "write" from core $write("hey ", 1)> $say() $say() $say()' \
    'prints to stdout' 'hey hey hey '
verify 'import "write" from core $index=<"first"> $index=<"second"> $index=<"third"> $write($index(), 1)' \
    'prints to stdout' 'third'
verify '$original=<import "write" from core $write("source", 1)> $copy=$original $copy()' \
    'prints to stdout' 'source'
verify '$function=<$local="value"> $function() $local' \
    'errors with execute message' '"absent variable"'
verify '$outer="value" $function=<$outer> $function()' \
    'errors with execute message' '"absent variable"'
verify 'import ["write", "freeze"] from core $outer="created" $function=<$outer="updated"> $function() $write($freeze($outer), 1)' \
    'prints to stdout' '"created"'
verify '$top=<$bottom=<$local="value"> $bottom() $local> $top()' \
    'errors with execute message' '"absent variable"'
verify '$top=<$outer="value" $bottom=<$outer> $bottom()> $top()' \
    'errors with execute message' '"absent variable"'
verify '$top=<import ["write", "freeze"] from core $outer="created" $bottom=<$outer="updated"> $bottom() $write($freeze($outer), 1)> $top()' \
    'prints to stdout' '"created"'

verify '$add(' \
    'errors with parse message' '('
verify '$add(,' \
    'errors with parse message' ','
verify '$add(,)' \
    'errors with parse message' ',)'
verify '$merge("1" "2")' \
    'errors with parse message' '"1" "2")'
verify '$x="1" $y="2" $merge($x $y)' \
    'errors with parse message' '$x $y)'
verify '$merge($freeze(1) $freeze(2))' \
    'errors with parse message' ') $freeze(2))'

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
