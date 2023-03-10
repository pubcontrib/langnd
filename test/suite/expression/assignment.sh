suite 'expression/assignment'

verify '$a=null' \
    'prints to stdout' ''
verify '$a=true' \
    'prints to stdout' ''
verify '$a=100' \
    'prints to stdout' ''
verify '$a="text"' \
    'prints to stdout' ''
verify '$a=[1, 2, 3]' \
    'prints to stdout' ''
verify '$a={"a": 1, "b": 2, "c": 3}' \
    'prints to stdout' ''
verify '$a=<import "multiply" from core $number=argument return $multiply($number, $number)>' \
    'prints to stdout' ''
verify '$A="val"' \
    'prints to stdout' ''
verify '$0="val"' \
    'prints to stdout' ''
verify '$_="val"' \
    'prints to stdout' ''
verify '$""="val"' \
    'prints to stdout' ''
verify '$Text_100="val"' \
    'prints to stdout' ''
verify '$"(long) & [special] --> ID"="val"' \
    'prints to stdout' ''
verify '$a=100 $a' \
    'prints to stdout' ''
verify '$a="text" $a' \
    'prints to stdout' ''
verify '$a=100 $a=null $a' \
    'prints to stdout' ''
verify 'import ["write", "freeze"] from core $a=null $write($freeze($a), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $a=true $write($freeze($a), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze"] from core $a=100 $write($freeze($a), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze"] from core $a="text" $write($freeze($a), 1)' \
    'prints to stdout' '"text"'
verify 'import ["write", "freeze"] from core $a=[1, 2, 3] $write($freeze($a), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze"] from core $a={"a": 1, "b": 2, "c": 3} $write($freeze($a), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'
verify 'import ["write", "freeze"] from core $a=<import "multiply" from core $number=argument return $multiply($number, $number)> $write($freeze($a), 1)' \
    'prints to stdout' '<import "multiply" from core $number=argument return $multiply($number, $number)>'
verify 'import ["write", "freeze"] from core $a=100 $a=200 $a=300 $write($freeze($a), 1)' \
    'prints to stdout' '300'
verify 'import ["write", "freeze"] from core $a=100 $b=$a $write($freeze($b), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze"] from core $a=100 $a=$a $write($freeze($a), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze", "add"] from core $a=100 $b=200 $write($freeze($add($a, $b)), 1)' \
    'prints to stdout' '300'
verify 'import ["write", "merge"] from core $a="this" $b=" & that" $write($merge($a, $b), 1)' \
    'prints to stdout' 'this & that'
verify 'import "write" from core $"long id"="found" $write($"long id", 1)' \
    'prints to stdout' 'found'
verify 'import "write" from core $""="found" $write($"", 1)' \
    'prints to stdout' 'found'
verify 'import "write" from core $"id"="found" $write($"id", 1)' \
    'prints to stdout' 'found'
verify 'import "write" from core $id="found" $write($"id", 1)' \
    'prints to stdout' 'found'
verify 'import "write" from core $"id"="found" $write($id, 1)' \
    'prints to stdout' 'found'
verify 'import "write" from core $copy=$new="found" $write($copy, 1)' \
    'prints to stdout' 'found'

verify '$="val"' \
    'errors with lex message' '$="val"'

verify '$a=' \
    'errors with parse message' '='
verify '$"\?"=' \
    'errors with parse message' '$"\?"='

verify '$huh' \
    'errors with execute message' '"absent variable"'
