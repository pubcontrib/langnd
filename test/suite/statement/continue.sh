suite 'statement/continue'

verify '$i=0 while @precedes($i, 10) < $i=@add($i, 1) if @equals(@modulo($i, 2), 0) < continue "here" > @write(@freeze($i), 1) >' \
    'prints to stdout' '13579'
verify '$i=0 $result=while @precedes($i, 10) < $i=@add($i, 1) if @equals(@modulo($i, 2), 0) < continue "here" > "there" > @write(@freeze($result), 1)' \
    'prints to stdout' '"here"'
verify '$i=0 $result=while @precedes($i, 10) < $i=@add($i, 1) if @equals(@modulo($i, 2), 1) < continue "here" > "there" > @write(@freeze($result), 1)' \
    'prints to stdout' '"there"'

verify 'continue' \
    'errors with parse message' 'continue'

verify 'continue "here"' \
    'errors with execute message' '"lost continue"'
