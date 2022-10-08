suite 'function/add'

pass '@write(@freeze(@add(0, 0)), 1)' '0'
pass '@write(@freeze(@add(1, 2)), 1)' '3'
pass '@write(@freeze(@add(123, 456)), 1)' '579'
pass '@write(@freeze(@add(-1, -2)), 1)' '-3'
pass '@write(@freeze(@add(-123, -456)), 1)' '-579'
pass '@write(@freeze(@add(0.1, 0.2)), 1)' '0.299987'
pass '@write(@freeze(@add(0.123, 0.456)), 1)' '0.578979'
pass '@write(@freeze(@add(-0.1, -0.2)), 1)' '-0.299987'
pass '@write(@freeze(@add(-0.123, -0.456)), 1)' '-0.578979'
pass '@write(@freeze(@add(123.456, 123.456)), 1)' '246.911987'
pass '@write(@freeze(@add(-123.456, -123.456)), 1)' '-246.911987'

executefail '@add()' 'absent argument'

executefail '@add(32767, 32767)' 'arithmetic error'
executefail '@add(1, 32767)' 'arithmetic error'
executefail '@add(32767, 1)' 'arithmetic error'

pass '@add(100, 200)' ''
