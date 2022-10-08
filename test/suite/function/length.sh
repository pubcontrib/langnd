suite 'function/length'

pass '@write(@freeze(@length("")), 1)' '0'
pass '@write(@freeze(@length("0")), 1)' '1'
pass '@write(@freeze(@length("0123456789")), 1)' '10'
