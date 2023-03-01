suite 'expression/continue'

verify 'import ["precedes", "add", "equals", "modulo", "write", "freeze"] from core $i=0 while $precedes($i, 10) \ $i=$add($i, 1) if $equals($modulo($i, 2), 0) \ continue "here" / $write($freeze($i), 1) /' \
    'prints to stdout' '13579'
verify 'import ["precedes", "add", "equals", "modulo", "write", "freeze"] from core $i=0 $result=while $precedes($i, 10) \ $i=$add($i, 1) if $equals($modulo($i, 2), 0) \ continue "here" / "there" / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'import ["precedes", "add", "equals", "modulo", "write", "freeze"] from core $i=0 $result=while $precedes($i, 10) \ $i=$add($i, 1) if $equals($modulo($i, 2), 1) \ continue "here" / "there" / $write($freeze($result), 1)' \
    'prints to stdout' '"there"'
verify 'import ["precedes", "add", "equals", "modulo", "write", "freeze"] from core $i=0 $result=while $precedes($i, 10) \ $i=$add($i, 1) if $equals($modulo($i, 2), 0) \ continue continue "here" / "there" / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'break continue "text"' \
    'errors with execute message' '"lost continue"'
verify 'throw continue "text"' \
    'errors with execute message' '"lost continue"'

verify 'continue' \
    'errors with parse message' 'continue'

verify 'continue "here"' \
    'errors with execute message' '"lost continue"'
