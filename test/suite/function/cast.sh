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
    'errors with execute message' '"invalid cast"'
verify '@cast(true, "NULL")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "NULL")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "NULL")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "NULL")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "NULL")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(123.456, "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "BOOLEAN")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(true, "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "NUMBER")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "STRING")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "STRING")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "STRING")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(123.456, "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(true, "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "LIST")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(123.456, "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(true, "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "MAP")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(123.456, "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(true, "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast("text", "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast([1], "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast({}, "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(<>, "FUNCTION")' \
    'errors with execute message' '"invalid cast"'
verify '@cast(null, "WRONG")' \
    'errors with execute message' '"unknown type"'
