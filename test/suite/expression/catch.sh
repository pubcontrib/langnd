suite 'expression/catch'

verify 'catch \ /' \
    'prints to stdout' ''
verify 'import "divide" from core catch \ $divide(100, 0) /' \
    'prints to stdout' ''
verify 'catch \ "not an error" /' \
    'prints to stdout' ''
verify '$value=100 catch \ $value /' \
    'prints to stdout' ''
verify 'import ["write", "freeze"] from core $error=catch \ / $write($freeze($error), 1)' \
    'prints to stdout' 'null'
verify 'import ["divide", "write", "freeze"] from core $error=catch \ $divide(100, 0) / $write($freeze($error), 1)' \
    'prints to stdout' '"arithmetic error"'
verify 'import ["write", "freeze"] from core $error=catch \ "not an error" / $write($freeze($error), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $value=100 $error=catch \ $value / $write($freeze($error), 1)' \
    'prints to stdout' 'null'
verify 'import ["divide", "write", "freeze"] from core $error=catch \ import "missing" from core $divide(100, 0) $missing / $write($freeze($error), 1)' \
    'prints to stdout' '"absent function"'
verify 'import ["divide", "write"] from core $error=catch \ $divide(100, 0) / $error=$error $write($error, 1)' \
    'prints to stdout' 'arithmetic error'
verify 'import ["write", "merge", "divide"] from core $write($merge(catch \ $divide(100, 0) /, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify 'import ["divide", "write", "merge"] from core $error=catch \ $divide(100, 0) / $write($merge($error, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify 'import "write" from core $error=catch \ throw "custom error" / $write($error, 1)' \
    'prints to stdout' 'custom error'
verify 'import "write" from core $error=catch \ $me="custom error" throw $me / $write($error, 1)' \
    'prints to stdout' 'custom error'
verify 'import ["merge", "write"] from core $error=catch \ throw $merge("custom ", "error") / $write($error, 1)' \
    'prints to stdout' 'custom error'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw null /), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw true /), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw 100 /), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw "text" /), 1)' \
    'prints to stdout' '"text"'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw [1, 2, 3] /), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw {"a": 1, "b": 2, "c": 3} /), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'
verify 'import ["write", "freeze"] from core $write($freeze(catch \ throw <import "multiply" from core $number=argument return $multiply($number, $number)> /), 1)' \
    'prints to stdout' '<import "multiply" from core $number=argument return $multiply($number, $number)>'
verify 'import ["write", "freeze"] from core $error=catch catch \ throw "custom error" / $write($freeze($error), 1)' \
    'prints to stdout' 'null'

verify 'catch' \
    'errors with parse message' 'catch'
verify 'catch \' \
    'errors with parse message' '\'
