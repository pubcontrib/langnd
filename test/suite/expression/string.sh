suite 'expression/string'

verify '"text"' \
    'prints to stdout' ''
verify '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"' \
    'prints to stdout' ''
verify '"\a000\a001\a002\a003\a004\a005\a006\a007\a008\a011\a012\a014\a015\a016\a017\a018\a019\a020\a021\a022\a023\a024\a025\a026\a027\a028\a029\a030\a031\a127"' \
    'prints to stdout' ''
verify '"\a128\a129\a130\a131\a132\a133\a134\a135\a136\a137\a138\a139\a140\a141\a142\a143\a144\a145\a146\a147\a148\a149\a150\a151\a152\a153\a154\a155\a156\a157\a158\a159\a160\a161\a162\a163\a164\a165\a166\a167\a168\a169\a170\a171\a172\a173\a174\a175\a176\a177\a178\a179\a180\a181\a182\a183\a184\a185\a186\a187\a188\a189\a190\a191\a192\a193\a194\a195\a196\a197\a198\a199\a200\a201\a202\a203\a204\a205\a206\a207\a208\a209\a210\a211\a212\a213\a214\a215\a216\a217\a218\a219\a220\a221\a222\a223\a224\a225\a226\a227\a228\a229\a230\a231\a232\a233\a234\a235\a236\a237\a238\a239\a240\a241\a242\a243\a244\a245\a246\a247\a248\a249\a250\a251\a252\a253\a254\a255"' \
    'prints to stdout' ''

verify '"\a"' \
    'errors with parse message' '"\a"'
verify '"\a0"' \
    'errors with parse message' '"\a0"'
verify '"\a00"' \
    'errors with parse message' '"\a00"'
verify '"\a-10"' \
    'errors with parse message' '"\a-10"'
verify '"\a.10"' \
    'errors with parse message' '"\a.10"'
verify '"\a256"' \
    'errors with parse message' '"\a256"'
verify '"\a999"' \
    'errors with parse message' '"\a999"'
verify '"\atxt"' \
    'errors with parse message' '"\atxt"'
verify '"\?"' \
    'errors with parse message' '"\?"'

verify '"' \
    'errors with lex message' '"'
verify '"
"' \
    'errors with lex message' '"'
verify '"\"' \
    'errors with lex message' '"\"'
verify '"'"`printf '\007'`"'"' \
    'errors with lex message' '"'"`printf '\007'`"'"'
