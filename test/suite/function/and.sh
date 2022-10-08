suite 'function/and'

pass '@write(@freeze(@and(false, false)), 1)' 'false'
pass '@write(@freeze(@and(false, true)), 1)' 'false'
pass '@write(@freeze(@and(true, false)), 1)' 'false'
pass '@write(@freeze(@and(true, true)), 1)' 'true'
