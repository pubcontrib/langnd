suite 'function/unset'

pass '@write(@freeze(@unset("abc", 1)), 1)' '"bc"'
pass '@write(@freeze(@unset("abc", 2)), 1)' '"ac"'
pass '@write(@freeze(@unset("abc", 3)), 1)' '"ab"'

executefail '@write(@freeze(@unset("abc", 0)), 1)' 'absent key'
executefail '@write(@freeze(@unset("abc", 4)), 1)' 'absent key'
