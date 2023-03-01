suite 'expression/argument'

verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print(null)' \
    'prints to stdout' 'null'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print(false)' \
    'prints to stdout' 'false'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print(0)' \
    'prints to stdout' '0'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print("")' \
    'prints to stdout' '""'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print([])' \
    'prints to stdout' '[]'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print({})' \
    'prints to stdout' '{}'
verify '$print=<import ["write", "freeze"] from core $write($freeze(argument), 1)> $print(<>)' \
    'prints to stdout' '<>'
verify 'import ["write", "freeze"] from core $square=<import "multiply" from core $number=argument return $multiply($number, $number)> $write($freeze($square(4)), 1)' \
    'prints to stdout' '16'
verify '$print=<import "write" from core $write(argument, 1)> $print("one ") $print("two ") $print("three")' \
    'prints to stdout' 'one two three'
verify 'import ["write", "freeze"] from core $decide=<import "equals" from core $pick=argument $true=argument $false=argument if $equals($pick, true) \ return $true / otherwise \ return $false /> $write($freeze($decide(false, "LEFT", "RIGHT")), 1)' \
    'prints to stdout' '"RIGHT"'
verify '$nothing=<> $nothing(throw "error")' \
    'prints to stdout' ''

verify '$function=<argument> $function()' \
    'errors with execute message' '"absent argument"'
