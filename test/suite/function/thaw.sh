suite 'function/thaw'

verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("null")), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("true")), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("100")), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("\"text\"")), 1)' \
    'prints to stdout' '"text"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("[1, 2, 3]")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("{\"a\": 1, \"b\": 2, \"c\": 3}")), 1)' \
    'prints to stdout' '{"a": 1, "b": 2, "c": 3}'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("<import \"multiply\" from core $number=argument return $multiply($number, $number)>")), 1)' \
    'prints to stdout' '<import "multiply" from core $number=argument return $multiply($number, $number)>'

verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw(" \t\n100")), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("100 \t\n")), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("# number\n100")), 1)' \
    'prints to stdout' '100'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("100\n# number")), 1)' \
    'prints to stdout' '100'

verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("junk")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("{\"key\" \"value\"}")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("# comment")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("1 2 3")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("$value=100")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("$add(1, 2)")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("if true \\ /")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("while true \\ /")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("catch \\ /")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("throw \"error\"")), 1)' \
    'errors with execute message' '"cast error"'
verify 'import ["write", "freeze", "thaw"] from core $write($freeze($thaw("$value")), 1)' \
    'errors with execute message' '"cast error"'
