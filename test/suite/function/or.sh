suite 'function/or'

pass '@write(@freeze(@or(false, false)), 1)' 'false'
pass '@write(@freeze(@or(false, true)), 1)' 'true'
pass '@write(@freeze(@or(true, false)), 1)' 'true'
pass '@write(@freeze(@or(true, true)), 1)' 'true'
