suite 'statement/string'

verify '"text"' \
    'prints to stdout' ''
verify '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^`_abcdefghijklmnopqrstuvwxyz{|}~"' \
    'prints to stdout' ''
