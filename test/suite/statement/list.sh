suite 'statement/list'

verify '[]' \
    'prints to stdout' ''
verify '[null, false, 0, "", [], {}]' \
    'prints to stdout' ''

verify '[' \
    'errors with parse message' '['
verify '[,' \
    'errors with parse message' ','
verify '[,]' \
    'errors with parse message' ',]'
verify '[1 2]' \
    'errors with parse message' '1 2]'
verify '$item=1 [$item]' \
    'errors with parse message' '$item]'
verify '[@cast("1", "NUMBER")]' \
    'errors with parse message' ')]'
verify '[if true \ 1 / else \ 2 /]' \
    'errors with parse message' '/]'
verify '[catch \ throw 1 /]' \
    'errors with parse message' '/]'
