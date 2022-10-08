suite 'function/not'

pass '@write(@freeze(@not(false)), 1)' 'true'
pass '@write(@freeze(@not(true)), 1)' 'false'

executefail '@not(@not())' 'absent argument'
