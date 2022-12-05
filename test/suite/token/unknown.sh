suite 'token/unknown'

verify 'unknown' \
    'errors with lex message' 'unknown'
verify 'NULL' \
    'errors with lex message' 'NULL'

verify 'aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjj' \
    'errors with lex message' 'aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeee'
verify '0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999' \
    'errors with parse message' '00000000001111111111222222222233333333334444444444'

verify "`printf '\001'`" \
    'errors with lex message' "`printf '\001'`"
verify "`printf '\002'`" \
    'errors with lex message' "`printf '\002'`"
verify "`printf '\003'`" \
    'errors with lex message' "`printf '\003'`"
verify "`printf '\004'`" \
    'errors with lex message' "`printf '\004'`"
verify "`printf '\005'`" \
    'errors with lex message' "`printf '\005'`"
verify "`printf '\006'`" \
    'errors with lex message' "`printf '\006'`"
verify "`printf '\007'`" \
    'errors with lex message' "`printf '\007'`"
verify "`printf '\010'`" \
    'errors with lex message' "`printf '\010'`"
verify "`printf '\013'`" \
    'errors with lex message' "`printf '\013'`"
verify "`printf '\014'`" \
    'errors with lex message' "`printf '\014'`"
verify "`printf '\016'`" \
    'errors with lex message' "`printf '\016'`"
verify "`printf '\017'`" \
    'errors with lex message' "`printf '\017'`"
verify "`printf '\020'`" \
    'errors with lex message' "`printf '\020'`"
verify "`printf '\021'`" \
    'errors with lex message' "`printf '\021'`"
verify "`printf '\022'`" \
    'errors with lex message' "`printf '\022'`"
verify "`printf '\023'`" \
    'errors with lex message' "`printf '\023'`"
verify "`printf '\024'`" \
    'errors with lex message' "`printf '\024'`"
verify "`printf '\025'`" \
    'errors with lex message' "`printf '\025'`"
verify "`printf '\026'`" \
    'errors with lex message' "`printf '\026'`"
verify "`printf '\027'`" \
    'errors with lex message' "`printf '\027'`"
verify "`printf '\030'`" \
    'errors with lex message' "`printf '\030'`"
verify "`printf '\031'`" \
    'errors with lex message' "`printf '\031'`"
verify "`printf '\032'`" \
    'errors with lex message' "`printf '\032'`"
verify "`printf '\033'`" \
    'errors with lex message' "`printf '\033'`"
verify "`printf '\034'`" \
    'errors with lex message' "`printf '\034'`"
verify "`printf '\035'`" \
    'errors with lex message' "`printf '\035'`"
verify "`printf '\036'`" \
    'errors with lex message' "`printf '\036'`"
verify "`printf '\037'`" \
    'errors with lex message' "`printf '\037'`"
verify "`printf '\177'`" \
    'errors with lex message' "`printf '\177'`"
