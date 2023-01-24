suite 'statement/catch'

verify 'catch \ /' \
    'prints to stdout' ''
verify 'catch \ @divide(100, 0) /' \
    'prints to stdout' ''
verify 'catch \ "not an error" /' \
    'prints to stdout' ''
verify '$value=100 catch \ $value /' \
    'prints to stdout' ''
verify '$error=catch \ / @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$error=catch \ @divide(100, 0) / @write(@freeze($error), 1)' \
    'prints to stdout' '"arithmetic error"'
verify '$error=catch \ "not an error" / @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$value=100 $error=catch \ $value / @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$error=catch \ @missing() @divide(100, 0) $missing / @write(@freeze($error), 1)' \
    'prints to stdout' '"absent function"'
verify '$error=catch \ @divide(100, 0) / $error=$error @write($error, 1)' \
    'prints to stdout' 'arithmetic error'
verify '@write(@merge(catch \ @divide(100, 0) /, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify '$error=catch \ @divide(100, 0) / @write(@merge($error, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify '$error=catch \ throw "custom error" / @write($error, 1)' \
    'prints to stdout' 'custom error'
verify '$error=catch \ $me="custom error" throw $me / @write($error, 1)' \
    'prints to stdout' 'custom error'
verify '$error=catch \ throw @merge("custom ", "error") / @write($error, 1)' \
    'prints to stdout' 'custom error'
verify '@write(@freeze(catch \ throw null /), 1)' \
    'prints to stdout' 'null'
verify '@write(@freeze(catch \ throw true /), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(catch \ throw 100 /), 1)' \
    'prints to stdout' '100'
verify '@write(@freeze(catch \ throw "text" /), 1)' \
    'prints to stdout' '"text"'
verify '@write(@freeze(catch \ throw [1, 2, 3] /), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify '@write(@freeze(catch \ throw {"a": 1, "b": 2, "c": 3} /), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'

verify 'catch' \
    'errors with parse message' 'catch'
verify 'catch \' \
    'errors with parse message' '\'
verify 'catch "error"' \
    'errors with parse message' 'catch "error"'
