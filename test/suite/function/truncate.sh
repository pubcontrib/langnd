suite 'function/truncate'

pass '@write(@freeze(@truncate(0)), 1)' '0'
pass '@write(@freeze(@truncate(1)), 1)' '1'
pass '@write(@freeze(@truncate(123)), 1)' '123'
pass '@write(@freeze(@truncate(-1)), 1)' '-1'
pass '@write(@freeze(@truncate(-123)), 1)' '-123'
pass '@write(@freeze(@truncate(32767)), 1)' '32767'
pass '@write(@freeze(@truncate(-32767)), 1)' '-32767'
pass '@write(@freeze(@truncate(0.1)), 1)' '0'
pass '@write(@freeze(@truncate(0.123)), 1)' '0'
pass '@write(@freeze(@truncate(-0.1)), 1)' '0'
pass '@write(@freeze(@truncate(-0.123)), 1)' '0'
pass '@write(@freeze(@truncate(123.456)), 1)' '123'
pass '@write(@freeze(@truncate(-123.456)), 1)' '-123'
pass '@write(@freeze(@truncate(0.999999)), 1)' '0'
pass '@write(@freeze(@truncate(-0.999999)), 1)' '0'
pass '@write(@freeze(@truncate(32767.999999)), 1)' '32767'
pass '@write(@freeze(@truncate(-32767.999999)), 1)' '-32767'