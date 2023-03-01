suite 'expression/import'

verify 'import "freeze" from core' \
    'prints to stdout' ''
verify 'import "write" from core import "freeze" from core $write($freeze("text"), 1)' \
    'prints to stdout' '"text"'
verify 'import "write" from core import "freeze" from core $write($freeze($freeze), 1)' \
    'prints to stdout' '<import "freeze" from core>'
verify '$return=import "freeze" from core import "write" from core $write($freeze($return), 1)' \
    'prints to stdout' '<import "freeze" from core>'
verify '$name="freeze" import $name from core' \
    'prints to stdout' ''
verify '$function=<"freeze"> import $function() from core' \
    'prints to stdout' ''
verify 'import ["write", "freeze"] from core' \
    'prints to stdout' ''
verify 'import ["write", "freeze"] from core $write($freeze("text"), 1)' \
    'prints to stdout' '"text"'
verify 'import ["write", "freeze"] from core $write($freeze($freeze), 1)' \
    'prints to stdout' '<import "freeze" from core>'
verify '$return=import ["write", "freeze"] from core $write($freeze($return), 1)' \
    'prints to stdout' '[<import "write" from core>, <import "freeze" from core>]'
verify '$names=["write", "freeze"] import $names from core' \
    'prints to stdout' ''
verify '$function=<["write", "freeze"]> import $function() from core' \
    'prints to stdout' ''
verify 'import [] from core' \
    'prints to stdout' ''
verify 'import ["freeze", "freeze", "freeze"] from core' \
    'prints to stdout' ''
verify 'import {"write": "W", "freeze": "F"} from core' \
    'prints to stdout' ''
verify 'import {"write": "W", "freeze": "F"} from core $W($F("text"), 1)' \
    'prints to stdout' '"text"'
verify 'import {"write": "W", "freeze": "F"} from core $W($F($F), 1)' \
    'prints to stdout' '<import "freeze" from core>'
verify '$return=import {"write": "W", "freeze": "F"} from core $W($F($return), 1)' \
    'prints to stdout' '{"F": <import "freeze" from core>, "W": <import "write" from core>}'
verify '$mappings={"write": "W", "freeze": "F"} import $mappings from core' \
    'prints to stdout' ''
verify '$function=<{"write": "W", "freeze": "F"}> import $function() from core' \
    'prints to stdout' ''
verify 'import {} from core' \
    'prints to stdout' ''
verify 'import {"write": "X", "freeze": "X"} from core' \
    'prints to stdout' ''
verify '$print=<import "write" from core $write(argument, 1)> $print("one ") $print("two ") $print("three")' \
    'prints to stdout' 'one two three'

verify 'import' \
    'errors with parse message' 'import'
verify 'import "freeze"' \
    'errors with parse message' '"freeze"'
verify 'import "freeze" from' \
    'errors with parse message' 'from'
verify 'import "freeze" otherwise' \
    'errors with parse message' '"freeze" otherwise'
verify 'import "freeze" from otherwise' \
    'errors with parse message' 'from otherwise'

verify 'import throw "error" from core' \
    'errors with execute message' '"error"'
verify 'import "nope" from core' \
    'errors with execute message' '"absent function"'
verify 'import "ADD" from core' \
    'errors with execute message' '"absent function"'
verify 'import ["add", "sub", "mut", "div"] from core' \
    'errors with execute message' '"absent function"'
verify 'import {"add": "+", "sub": "-", "mut": "*", "div": "/"} from core' \
    'errors with execute message' '"absent function"'

verify 'import null from core' \
    'errors with execute message' '"alien argument"'
verify 'import false from core' \
    'errors with execute message' '"alien argument"'
verify 'import 0 from core' \
    'errors with execute message' '"alien argument"'
verify 'import <> from core' \
    'errors with execute message' '"alien argument"'
verify 'import [null] from core' \
    'errors with execute message' '"alien argument"'
verify 'import [false] from core' \
    'errors with execute message' '"alien argument"'
verify 'import [0] from core' \
    'errors with execute message' '"alien argument"'
verify 'import [[]] from core' \
    'errors with execute message' '"alien argument"'
verify 'import [{}] from core' \
    'errors with execute message' '"alien argument"'
verify 'import [<>] from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": null} from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": false} from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": 0} from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": []} from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": {}} from core' \
    'errors with execute message' '"alien argument"'
verify 'import {"add": <>} from core' \
    'errors with execute message' '"alien argument"'
