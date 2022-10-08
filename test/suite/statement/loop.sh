suite 'statement/loop'

pass 'while false { @write("HERE", 1) }' ''
pass '$i=1 while @precedes($i, 4) { @write(@freeze($i), 1) $i=@add($i, 1) }' '123'
pass '$i=1 while @precedes($i, 4) { $j=1 while @precedes($j, 4) { @write(@freeze(@multiply($i, $j)), 1) $j=@add($j, 1) } $i=@add($i, 1) }' '123246369'

parsefail 'while' 'while'
parsefail 'while { }' '{ }'
parsefail 'while false {' '{'
parsefail 'while false }' 'false }'
parsefail 'while false 100' 'false 100'
parsefail 'while false { 100' '100'

executefail 'while true { "before" @divide(100, 0) "after" }' 'arithmetic error'
executefail 'while @divide(100, 0) { $missing }' 'arithmetic error'
