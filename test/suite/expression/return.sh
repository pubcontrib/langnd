suite 'expression/return'

verify '$function=<return null> @write(@freeze($function()), 1)' \
    'prints to stdout' 'null'
verify '$function=<return false> @write(@freeze($function()), 1)' \
    'prints to stdout' 'false'
verify '$function=<return 0> @write(@freeze($function()), 1)' \
    'prints to stdout' '0'
verify '$function=<return ""> @write(@freeze($function()), 1)' \
    'prints to stdout' '""'
verify '$function=<return []> @write(@freeze($function()), 1)' \
    'prints to stdout' '[]'
verify '$function=<return {}> @write(@freeze($function()), 1)' \
    'prints to stdout' '{}'
verify '$function=<return <>> @write(@freeze($function()), 1)' \
    'prints to stdout' '<>'
verify '$function=<@write("before", 1) return "value" @write("after", 1)> $function()' \
    'prints to stdout' 'before'
verify '$function=<return "first" return "second" return "third"> @write(@freeze($function()), 1)' \
    'prints to stdout' '"first"'

verify 'return' \
    'errors with parse message' 'return'

verify 'return "result"' \
    'errors with execute message' '"lost return"'
