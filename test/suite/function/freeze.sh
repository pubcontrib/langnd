suite 'function/freeze'

verify 'import ["write", "freeze"] from core $write($freeze(null), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $write($freeze(false), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze"] from core $write($freeze(true), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze"] from core $write($freeze(0), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze"] from core $write($freeze(1), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze"] from core $write($freeze(123), 1)' \
    'prints to stdout' '123'
verify 'import ["write", "freeze"] from core $write($freeze(-1), 1)' \
    'prints to stdout' '-1'
verify 'import ["write", "freeze"] from core $write($freeze(-123), 1)' \
    'prints to stdout' '-123'
verify 'import ["write", "freeze"] from core $write($freeze(32767), 1)' \
    'prints to stdout' '32767'
verify 'import ["write", "freeze"] from core $write($freeze(-32767), 1)' \
    'prints to stdout' '-32767'
verify 'import ["write", "freeze"] from core $write($freeze(0.1), 1)' \
    'prints to stdout' '0.099990'
verify 'import ["write", "freeze"] from core $write($freeze(0.123), 1)' \
    'prints to stdout' '0.122985'
verify 'import ["write", "freeze"] from core $write($freeze(-0.1), 1)' \
    'prints to stdout' '-0.099990'
verify 'import ["write", "freeze"] from core $write($freeze(-0.123), 1)' \
    'prints to stdout' '-0.122985'
verify 'import ["write", "freeze"] from core $write($freeze(123.456), 1)' \
    'prints to stdout' '123.455993'
verify 'import ["write", "freeze"] from core $write($freeze(-123.456), 1)' \
    'prints to stdout' '-123.455993'
verify 'import ["write", "freeze"] from core $write($freeze(32767.999999), 1)' \
    'prints to stdout' '32767.999984'
verify 'import ["write", "freeze"] from core $write($freeze(-32767.999999), 1)' \
    'prints to stdout' '-32767.999984'
verify 'import ["write", "freeze"] from core $write($freeze(0.000000), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze"] from core $write($freeze(-0.000000), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze"] from core $write($freeze(0.000015), 1)' \
    'prints to stdout' '0.000015'
verify 'import ["write", "freeze"] from core $write($freeze(-0.000015), 1)' \
    'prints to stdout' '-0.000015'
verify 'import ["write", "freeze"] from core $write($freeze(0.999999), 1)' \
    'prints to stdout' '0.999984'
verify 'import ["write", "freeze"] from core $write($freeze(-0.999999), 1)' \
    'prints to stdout' '-0.999984'
verify 'import ["write", "freeze"] from core $write($freeze(0.999984), 1)' \
    'prints to stdout' '0.999984'
verify 'import ["write", "freeze"] from core $write($freeze(-0.999984), 1)' \
    'prints to stdout' '-0.999984'
verify 'import ["write", "freeze"] from core $write($freeze(0.1), 1)' \
    'prints to stdout' '0.099990'
verify 'import ["write", "freeze"] from core $write($freeze(0.12), 1)' \
    'prints to stdout' '0.119995'
verify 'import ["write", "freeze"] from core $write($freeze(0.123), 1)' \
    'prints to stdout' '0.122985'
verify 'import ["write", "freeze"] from core $write($freeze(0.1234), 1)' \
    'prints to stdout' '0.123397'
verify 'import ["write", "freeze"] from core $write($freeze(0.12345), 1)' \
    'prints to stdout' '0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(0.123456), 1)' \
    'prints to stdout' '0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(0.1234567), 1)' \
    'prints to stdout' '0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(0.12345678), 1)' \
    'prints to stdout' '0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(0.123456789), 1)' \
    'prints to stdout' '0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(-0.1), 1)' \
    'prints to stdout' '-0.099990'
verify 'import ["write", "freeze"] from core $write($freeze(-0.12), 1)' \
    'prints to stdout' '-0.119995'
verify 'import ["write", "freeze"] from core $write($freeze(-0.123), 1)' \
    'prints to stdout' '-0.122985'
verify 'import ["write", "freeze"] from core $write($freeze(-0.1234), 1)' \
    'prints to stdout' '-0.123397'
verify 'import ["write", "freeze"] from core $write($freeze(-0.12345), 1)' \
    'prints to stdout' '-0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(-0.123456), 1)' \
    'prints to stdout' '-0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(-0.1234567), 1)' \
    'prints to stdout' '-0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(-0.12345678), 1)' \
    'prints to stdout' '-0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(-0.123456789), 1)' \
    'prints to stdout' '-0.123443'
verify 'import ["write", "freeze"] from core $write($freeze(""), 1)' \
    'prints to stdout' '""'
verify 'import ["write", "freeze"] from core $write($freeze("text"), 1)' \
    'prints to stdout' '"text"'
verify 'import ["write", "freeze"] from core $write($freeze("\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"), 1)' \
    'prints to stdout' '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"'
verify 'import ["write", "freeze"] from core $write($freeze("\a000\a001\a002\a003\a004\a005\a006\a007\a008\a011\a012\a014\a015\a016\a017\a018\a019\a020\a021\a022\a023\a024\a025\a026\a027\a028\a029\a030\a031\a127"), 1)' \
    'prints to stdout' '"\a000\a001\a002\a003\a004\a005\a006\a007\a008\a011\a012\a014\a015\a016\a017\a018\a019\a020\a021\a022\a023\a024\a025\a026\a027\a028\a029\a030\a031\a127"'
verify 'import ["write", "freeze"] from core $write($freeze("\a128\a129\a130\a131\a132\a133\a134\a135\a136\a137\a138\a139\a140\a141\a142\a143\a144\a145\a146\a147\a148\a149\a150\a151\a152\a153\a154\a155\a156\a157\a158\a159\a160\a161\a162\a163\a164\a165\a166\a167\a168\a169\a170\a171\a172\a173\a174\a175\a176\a177\a178\a179\a180\a181\a182\a183\a184\a185\a186\a187\a188\a189\a190\a191\a192\a193\a194\a195\a196\a197\a198\a199\a200\a201\a202\a203\a204\a205\a206\a207\a208\a209\a210\a211\a212\a213\a214\a215\a216\a217\a218\a219\a220\a221\a222\a223\a224\a225\a226\a227\a228\a229\a230\a231\a232\a233\a234\a235\a236\a237\a238\a239\a240\a241\a242\a243\a244\a245\a246\a247\a248\a249\a250\a251\a252\a253\a254\a255"), 1)' \
    'prints to stdout' '"\a128\a129\a130\a131\a132\a133\a134\a135\a136\a137\a138\a139\a140\a141\a142\a143\a144\a145\a146\a147\a148\a149\a150\a151\a152\a153\a154\a155\a156\a157\a158\a159\a160\a161\a162\a163\a164\a165\a166\a167\a168\a169\a170\a171\a172\a173\a174\a175\a176\a177\a178\a179\a180\a181\a182\a183\a184\a185\a186\a187\a188\a189\a190\a191\a192\a193\a194\a195\a196\a197\a198\a199\a200\a201\a202\a203\a204\a205\a206\a207\a208\a209\a210\a211\a212\a213\a214\a215\a216\a217\a218\a219\a220\a221\a222\a223\a224\a225\a226\a227\a228\a229\a230\a231\a232\a233\a234\a235\a236\a237\a238\a239\a240\a241\a242\a243\a244\a245\a246\a247\a248\a249\a250\a251\a252\a253\a254\a255"'
verify 'import ["write", "freeze"] from core $write($freeze("\a065\a066\a067"), 1)' \
    'prints to stdout' '"ABC"'
verify 'import ["write", "freeze"] from core $write($freeze([]), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze"] from core $write($freeze([null]), 1)' \
    'prints to stdout' '[null]'
verify 'import ["write", "freeze"] from core $write($freeze([false]), 1)' \
    'prints to stdout' '[false]'
verify 'import ["write", "freeze"] from core $write($freeze([0]), 1)' \
    'prints to stdout' '[0]'
verify 'import ["write", "freeze"] from core $write($freeze([""]), 1)' \
    'prints to stdout' '[""]'
verify 'import ["write", "freeze"] from core $write($freeze([[]]), 1)' \
    'prints to stdout' '[[]]'
verify 'import ["write", "freeze"] from core $write($freeze([{}]), 1)' \
    'prints to stdout' '[{}]'
verify 'import ["write", "freeze"] from core $write($freeze([<>]), 1)' \
    'prints to stdout' '[<>]'
verify 'import ["write", "freeze"] from core $write($freeze([null, false, 0, "", [], {}, <>]), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}, <>]'
verify 'import ["write", "freeze"] from core $write($freeze([null, false, 0, "", [null, false, 0, "", [], {}, <>], {}, <>]), 1)' \
    'prints to stdout' '[null, false, 0, "", [null, false, 0, "", [], {}, <>], {}, <>]'
verify 'import ["write", "freeze"] from core $write($freeze({}), 1)' \
    'prints to stdout' '{}'
verify 'import ["write", "freeze"] from core $write($freeze({"null": null}), 1)' \
    'prints to stdout' '{"null": null}'
verify 'import ["write", "freeze"] from core $write($freeze({"boolean": false}), 1)' \
    'prints to stdout' '{"boolean": false}'
verify 'import ["write", "freeze"] from core $write($freeze({"number": 0}), 1)' \
    'prints to stdout' '{"number": 0}'
verify 'import ["write", "freeze"] from core $write($freeze({"list": []}), 1)' \
    'prints to stdout' '{"list": []}'
verify 'import ["write", "freeze"] from core $write($freeze({"map": {}}), 1)' \
    'prints to stdout' '{"map": {}}'
verify 'import ["write", "freeze"] from core $write($freeze({"function": <>}), 1)' \
    'prints to stdout' '{"function": <>}'
verify 'import ["write", "freeze"] from core $write($freeze({"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}, "function": <>}), 1)' \
    'prints to stdout' '{"boolean": false, "function": <>, "list": [], "map": {}, "null": null, "number": 0, "string": ""}'
verify 'import ["write", "freeze"] from core $write($freeze({"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}, "function": <>}, "function": <>}), 1)' \
    'prints to stdout' '{"boolean": false, "function": <>, "list": [], "map": {"boolean": false, "function": <>, "list": [], "map": {}, "null": null, "number": 0, "string": ""}, "null": null, "number": 0, "string": ""}'
verify 'import ["write", "freeze"] from core $write($freeze({"1": null, "2": null, "3": null}), 1)' \
    'prints to stdout' '{"1": null, "2": null, "3": null}'
verify 'import ["write", "freeze"] from core $write($freeze({"3": null, "2": null, "1": null}), 1)' \
    'prints to stdout' '{"1": null, "2": null, "3": null}'
verify 'import ["write", "freeze"] from core $write($freeze({"x": 1, "x": 2, "x": 3}), 1)' \
    'prints to stdout' '{"x": 3}'
verify 'import ["write", "freeze"] from core $write($freeze(<>), 1)' \
    'prints to stdout' '<>'
verify 'import ["write", "freeze"] from core $write($freeze(<null>), 1)' \
    'prints to stdout' '<null>'
verify 'import ["write", "freeze"] from core $write($freeze(<0>), 1)' \
    'prints to stdout' '<0>'
verify 'import ["write", "freeze"] from core $write($freeze(<"">), 1)' \
    'prints to stdout' '<"">'
verify 'import ["write", "freeze"] from core $write($freeze(<[]>), 1)' \
    'prints to stdout' '<[]>'
verify 'import ["write", "freeze"] from core $write($freeze(<{}>), 1)' \
    'prints to stdout' '<{}>'
verify 'import ["write", "freeze"] from core $write($freeze(<<>>), 1)' \
    'prints to stdout' '<<>>'
verify 'import ["write", "freeze"] from core $write($freeze(<   >), 1)' \
    'prints to stdout' '<   >'
verify 'import ["write", "freeze"] from core $write($freeze(<null 0 "" [] {} <>>), 1)' \
    'prints to stdout' '<null 0 "" [] {} <>>'
