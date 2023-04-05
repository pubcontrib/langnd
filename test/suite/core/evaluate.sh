suite 'core/evaluate'

verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("null")), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("false")), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("0")), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("\"\"")), 1)' \
    'prints to stdout' '""'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("[]")), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("{}")), 1)' \
    'prints to stdout' '{}'
verify 'import ["write", "freeze", "evaluate"] from core $write($freeze($evaluate("<>")), 1)' \
    'prints to stdout' '<>'

verify 'import "evaluate" from core $evaluate("import \"write\" from core $write(\"HERE\", 1)")' \
    'prints to stdout' 'HERE'

verify 'import "evaluate" from core $evaluate("...")' \
    'errors with execute message' '{"hint": "...", "message": "failed to lex code"}'
verify 'import "evaluate" from core $evaluate("[")' \
    'errors with execute message' '{"hint": "[", "message": "failed to parse code"}'
verify 'import "evaluate" from core $evaluate("throw \"error\"")' \
    'errors with execute message' '{"hint": "\"error\"", "message": "failed to execute code"}'

verify 'import "evaluate" from core $evaluate("return \"lost\"")' \
    'errors with execute message' '{"hint": "\"lost return\"", "message": "failed to execute code"}'
verify 'import "evaluate" from core $evaluate("break \"lost\"")' \
    'errors with execute message' '{"hint": "\"lost break\"", "message": "failed to execute code"}'
verify 'import "evaluate" from core $evaluate("continue \"lost\"")' \
    'errors with execute message' '{"hint": "\"lost continue\"", "message": "failed to execute code"}'
