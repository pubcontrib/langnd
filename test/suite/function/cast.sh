suite 'function/cast'

verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast(null, "NULL")), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast(true, "BOOLEAN")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast("false", "BOOLEAN")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast("true", "BOOLEAN")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast(100, "NUMBER")), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "cast"] from core $write($cast($cast(null, "NULL"), "STRING"), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "cast"] from core $write($cast($cast("null", "NULL"), "STRING"), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "cast"] from core $write($cast($cast(true, "BOOLEAN"), "STRING"), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "cast"] from core $write($cast($cast("true", "BOOLEAN"), "STRING"), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "cast"] from core $write($cast($cast(123.456, "NUMBER"), "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify 'import ["write", "cast"] from core $write($cast($cast("123.456", "NUMBER"), "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify 'import ["write", "cast"] from core $write($cast(null, "STRING"), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "cast"] from core $write($cast(false, "STRING"), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "cast"] from core $write($cast(true, "STRING"), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "cast"] from core $write($cast(123.456, "STRING"), 1)' \
    'prints to stdout' '123.455993'
verify 'import ["write", "cast"] from core $write($cast("text", "STRING"), 1)' \
    'prints to stdout' 'text'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast([1, 2, 3], "LIST")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast({"a": 1, "b": 2, "c": 3}, "MAP")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "cast"] from core $write($freeze($cast(<import "multiply" from core $number=argument return $multiply($number, $number)>, "FUNCTION")), 1)' \
    'prints to stdout' '<import "multiply" from core $number=argument return $multiply($number, $number)>'

verify 'import "cast" from core $cast(123.456, "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(true, "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "NULL")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(123.456, "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "BOOLEAN")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(true, "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "NUMBER")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "STRING")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "STRING")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "STRING")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(123.456, "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(true, "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "LIST")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(123.456, "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(true, "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(<>, "MAP")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(123.456, "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(true, "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast("text", "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast([1], "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast({}, "FUNCTION")' \
    'errors with execute message' '"cast error"'
verify 'import "cast" from core $cast(null, "WRONG")' \
    'errors with execute message' '"damaged argument"'
