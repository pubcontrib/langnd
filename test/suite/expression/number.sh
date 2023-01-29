suite 'expression/number'

verify '0' \
    'prints to stdout' ''
verify '-0' \
    'prints to stdout' ''
verify '+0' \
    'prints to stdout' ''
verify '1' \
    'prints to stdout' ''
verify '-1' \
    'prints to stdout' ''
verify '+1' \
    'prints to stdout' ''
verify '1.0' \
    'prints to stdout' ''
verify '-1.0' \
    'prints to stdout' ''
verify '+1.0' \
    'prints to stdout' ''
verify '100' \
    'prints to stdout' ''
verify '-100' \
    'prints to stdout' ''
verify '+100' \
    'prints to stdout' ''
verify '-1-2-3' \
    'prints to stdout' ''
verify '+1+2+3' \
    'prints to stdout' ''
verify '1 2 3' \
    'prints to stdout' ''
verify '-1 -2 -3' \
    'prints to stdout' ''
verify '+1 +2 +3' \
    'prints to stdout' ''
verify '-1.0-2.0-3.0' \
    'prints to stdout' ''
verify '+1.0+2.0+3.0' \
    'prints to stdout' ''
verify '1.0 2.0 3.0' \
    'prints to stdout' ''
verify '-1.0 -2.0 -3.0' \
    'prints to stdout' ''
verify '+1.0 +2.0 +3.0' \
    'prints to stdout' ''

verify '32768' \
    'errors with parse message' '32768'
verify '-32768' \
    'errors with parse message' '-32768'
verify '+32768' \
    'errors with parse message' '+32768'
verify '000000000000000000000000000' \
    'errors with parse message' '000000000000000000000000000'
verify '-000000000000000000000000000' \
    'errors with parse message' '-000000000000000000000000000'
verify '+000000000000000000000000000' \
    'errors with parse message' '+000000000000000000000000000'
verify '000000000000000000000000000.0' \
    'errors with parse message' '000000000000000000000000000.0'
verify '-000000000000000000000000000.0' \
    'errors with parse message' '-000000000000000000000000000.0'
verify '+000000000000000000000000000.0' \
    'errors with parse message' '+000000000000000000000000000.0'
verify '999999999999999999999999999' \
    'errors with parse message' '999999999999999999999999999'
verify '-999999999999999999999999999' \
    'errors with parse message' '-999999999999999999999999999'
verify '+999999999999999999999999999' \
    'errors with parse message' '+999999999999999999999999999'
verify '999999999999999999999999999.9' \
    'errors with parse message' '999999999999999999999999999.9'
verify '-999999999999999999999999999.9' \
    'errors with parse message' '-999999999999999999999999999.9'
verify '+999999999999999999999999999.9' \
    'errors with parse message' '+999999999999999999999999999.9'

verify '.1' \
    'errors with lex message' '.1'
verify '-.1' \
    'errors with lex message' '-.1'
verify '+.1' \
    'errors with lex message' '+.1'
verify '0..1' \
    'errors with lex message' '0..1'
verify '-0..1' \
    'errors with lex message' '-0..1'
verify '+0..1' \
    'errors with lex message' '+0..1'
verify '--1' \
    'errors with lex message' '--1'
verify '++1' \
    'errors with lex message' '++1'
