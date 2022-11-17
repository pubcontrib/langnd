suite 'function/freeze'

verify '@write(@freeze(0), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(1), 1)' \
    'prints to stdout' '1'
verify '@write(@freeze(123), 1)' \
    'prints to stdout' '123'
verify '@write(@freeze(-1), 1)' \
    'prints to stdout' '-1'
verify '@write(@freeze(-123), 1)' \
    'prints to stdout' '-123'
verify '@write(@freeze(32767), 1)' \
    'prints to stdout' '32767'
verify '@write(@freeze(-32767), 1)' \
    'prints to stdout' '-32767'
verify '@write(@freeze(0.1), 1)' \
    'prints to stdout' '0.099990'
verify '@write(@freeze(0.123), 1)' \
    'prints to stdout' '0.122985'
verify '@write(@freeze(-0.1), 1)' \
    'prints to stdout' '-0.099990'
verify '@write(@freeze(-0.123), 1)' \
    'prints to stdout' '-0.122985'
verify '@write(@freeze(123.456), 1)' \
    'prints to stdout' '123.455993'
verify '@write(@freeze(-123.456), 1)' \
    'prints to stdout' '-123.455993'
verify '@write(@freeze(32767.999999), 1)' \
    'prints to stdout' '32767.999984'
verify '@write(@freeze(-32767.999999), 1)' \
    'prints to stdout' '-32767.999984'
verify '@write(@freeze(0.000000), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(-0.000000), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(0.000015), 1)' \
    'prints to stdout' '0.000015'
verify '@write(@freeze(-0.000015), 1)' \
    'prints to stdout' '-0.000015'
verify '@write(@freeze(0.999999), 1)' \
    'prints to stdout' '0.999984'
verify '@write(@freeze(-0.999999), 1)' \
    'prints to stdout' '-0.999984'
verify '@write(@freeze(0.999984), 1)' \
    'prints to stdout' '0.999984'
verify '@write(@freeze(-0.999984), 1)' \
    'prints to stdout' '-0.999984'
verify '@write(@freeze(0.1), 1)' \
    'prints to stdout' '0.099990'
verify '@write(@freeze(0.12), 1)' \
    'prints to stdout' '0.119995'
verify '@write(@freeze(0.123), 1)' \
    'prints to stdout' '0.122985'
verify '@write(@freeze(0.1234), 1)' \
    'prints to stdout' '0.123397'
verify '@write(@freeze(0.12345), 1)' \
    'prints to stdout' '0.123443'
verify '@write(@freeze(0.123456), 1)' \
    'prints to stdout' '0.123443'
verify '@write(@freeze(0.1234567), 1)' \
    'prints to stdout' '0.123443'
verify '@write(@freeze(0.12345678), 1)' \
    'prints to stdout' '0.123443'
verify '@write(@freeze(0.123456789), 1)' \
    'prints to stdout' '0.123443'
verify '@write(@freeze(-0.1), 1)' \
    'prints to stdout' '-0.099990'
verify '@write(@freeze(-0.12), 1)' \
    'prints to stdout' '-0.119995'
verify '@write(@freeze(-0.123), 1)' \
    'prints to stdout' '-0.122985'
verify '@write(@freeze(-0.1234), 1)' \
    'prints to stdout' '-0.123397'
verify '@write(@freeze(-0.12345), 1)' \
    'prints to stdout' '-0.123443'
verify '@write(@freeze(-0.123456), 1)' \
    'prints to stdout' '-0.123443'
verify '@write(@freeze(-0.1234567), 1)' \
    'prints to stdout' '-0.123443'
verify '@write(@freeze(-0.12345678), 1)' \
    'prints to stdout' '-0.123443'
verify '@write(@freeze(-0.123456789), 1)' \
    'prints to stdout' '-0.123443'
verify '@write(@freeze(""), 1)' \
    'prints to stdout' '""'
verify '@write(@freeze("text"), 1)' \
    'prints to stdout' '"text"'
verify '@write(@freeze("\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"), 1)' \
    'prints to stdout' '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"'
verify '@write(@freeze(null), 1)' \
    'prints to stdout' 'null'
verify '@write(@freeze(false), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(true), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze([]), 1)' \
    'prints to stdout' '[]'
verify '@write(@freeze([null]), 1)' \
    'prints to stdout' '[null]'
verify '@write(@freeze([false]), 1)' \
    'prints to stdout' '[false]'
verify '@write(@freeze([0]), 1)' \
    'prints to stdout' '[0]'
verify '@write(@freeze([""]), 1)' \
    'prints to stdout' '[""]'
verify '@write(@freeze([[]]), 1)' \
    'prints to stdout' '[[]]'
verify '@write(@freeze([{}]), 1)' \
    'prints to stdout' '[{}]'
verify '@write(@freeze([null, false, 0, "", [], {}]), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}]'
verify '@write(@freeze([null, false, 0, "", [null, false, 0, "", [], {}], {}]), 1)' \
    'prints to stdout' '[null, false, 0, "", [null, false, 0, "", [], {}], {}]'
verify '@write(@freeze({}), 1)' \
    'prints to stdout' '{}'
verify '@write(@freeze({"null": null}), 1)' \
    'prints to stdout' '{"null": null}'
verify '@write(@freeze({"boolean": false}), 1)' \
    'prints to stdout' '{"boolean": false}'
verify '@write(@freeze({"number": 0}), 1)' \
    'prints to stdout' '{"number": 0}'
verify '@write(@freeze({"list": []}), 1)' \
    'prints to stdout' '{"list": []}'
verify '@write(@freeze({"map": {}}), 1)' \
    'prints to stdout' '{"map": {}}'
verify '@write(@freeze({"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}}), 1)' \
    'prints to stdout' '{"boolean": false, "list": [], "map": {}, "null": null, "number": 0, "string": ""}'
verify '@write(@freeze({"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}}}), 1)' \
    'prints to stdout' '{"boolean": false, "list": [], "map": {"boolean": false, "list": [], "map": {}, "null": null, "number": 0, "string": ""}, "null": null, "number": 0, "string": ""}'
verify '@write(@freeze({"1": null, "2": null, "3": null}), 1)' \
    'prints to stdout' '{"1": null, "2": null, "3": null}'
verify '@write(@freeze({"3": null, "2": null, "1": null}), 1)' \
    'prints to stdout' '{"1": null, "2": null, "3": null}'
verify '@write(@freeze({"x": 1, "x": 2, "x": 3}), 1)' \
    'prints to stdout' '{"x": 3}'

verify '@freeze()' \
    'errors with execute message' '"absent argument"'
