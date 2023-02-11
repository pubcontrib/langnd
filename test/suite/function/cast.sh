suite 'function/cast'

verify '@write(@freeze(@cast(null, "NULL")), 1)' \
    'prints to stdout' 'null'
verify '@write(@freeze(@cast(true, "BOOLEAN")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@cast("false", "BOOLEAN")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@cast("true", "BOOLEAN")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@cast(100, "NUMBER")), 1)' \
    'prints to stdout' '100'
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
verify '@write(@cast(false, "STRING"), 1)' \
    'prints to stdout' 'false'
verify '@write(@cast(true, "STRING"), 1)' \
    'prints to stdout' 'true'
verify '@write(@cast(123.456, "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify '@write(@cast("text", "STRING"), 1)' \
    'prints to stdout' 'text'
verify '@write(@freeze(@cast([1, 2, 3], "LIST")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify '@write(@freeze(@cast({"a": 1, "b": 2, "c": 3}, "MAP")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'
verify '@write(@freeze(@cast(<$number=argument return @multiply($number, $number)>, "FUNCTION")), 1)' \
    'prints to stdout' '<$number=argument return @multiply($number, $number)>'

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
verify '@cast(null, "WRONG")' \
    'errors with execute message' '"unknown type"'
