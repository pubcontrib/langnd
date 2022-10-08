suite 'function/get'

pass '@write(@freeze(@get("abc", 1)), 1)' '"a"'
pass '@write(@freeze(@get("abc", 2)), 1)' '"b"'
pass '@write(@freeze(@get("abc", 3)), 1)' '"c"'

executefail '@write(@freeze(@get("abc", 0)), 1)' 'absent key'
executefail '@write(@freeze(@get("abc", 4)), 1)' 'absent key'
