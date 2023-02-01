suite 'expression/function'

verify '<>' \
    'prints to stdout' ''
verify '<null>' \
    'prints to stdout' ''
verify '<false>' \
    'prints to stdout' ''
verify '<0>' \
    'prints to stdout' ''
verify '<"">' \
    'prints to stdout' ''
verify '<[]>' \
    'prints to stdout' ''
verify '<{}>' \
    'prints to stdout' ''
verify '<<>>' \
    'prints to stdout' ''
verify '<null false 0 "" [] {} <>>' \
    'prints to stdout' ''

verify '<' \
    'errors with parse message' '<'
verify '<,' \
    'errors with parse message' ','
verify '<#comment>' \
    'errors with parse message' '<#comment>'
