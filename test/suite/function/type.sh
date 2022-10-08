suite 'function/type'

pass '@write(@type(null), 1)' 'NULL'
pass '@write(@type(false), 1)' 'BOOLEAN'
pass '@write(@type(0), 1)' 'NUMBER'
pass '@write(@type(""), 1)' 'STRING'
