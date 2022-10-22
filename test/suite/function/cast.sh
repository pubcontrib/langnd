suite 'function/cast'

verify '@write(@cast(@cast(null, "NULL"), "STRING"), 1)' \
    'prints to stdout' 'null'
verify '@write(@cast(@cast("null", "NULL"), "STRING"), 1)' \
    'prints to stdout' 'null'
verify '@write(@cast(@cast(true, "BOOLEAN"), "STRING"), 1)' \
    'prints to stdout' 'true'
verify '@write(@cast(@cast("true", "BOOLEAN"), "STRING"), 1)' \
    'prints to stdout' 'true'
verify '@write(@cast(@cast(123.456, "NUMBER"), "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify '@write(@cast(@cast("123.456", "NUMBER"), "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify '@write(@cast(null, "STRING"), 1)' \
    'prints to stdout' 'null'
verify '@write(@cast(true, "STRING"), 1)' \
    'prints to stdout' 'true'
verify '@write(@cast(123.456, "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify '@write(@cast("text", "STRING"), 1)' \
    'prints to stdout' 'text'

verify '@cast(123.456, "NULL")' \
    'errors with execute message' 'invalid cast'
verify '@cast(true, "NULL")' \
    'errors with execute message' 'invalid cast'
verify '@cast("text", "NULL")' \
    'errors with execute message' 'invalid cast'
verify '@cast(null, "BOOLEAN")' \
    'errors with execute message' 'invalid cast'
verify '@cast(123.456, "BOOLEAN")' \
    'errors with execute message' 'invalid cast'
verify '@cast("text", "BOOLEAN")' \
    'errors with execute message' 'invalid cast'
verify '@cast(null, "NUMBER")' \
    'errors with execute message' 'invalid cast'
verify '@cast(true, "NUMBER")' \
    'errors with execute message' 'invalid cast'
verify '@cast("text", "NUMBER")' \
    'errors with execute message' 'invalid cast'
verify '@cast(null, "WRONG")' \
    'errors with execute message' 'unknown type'
