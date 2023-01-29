suite 'expression/map'

verify '{}' \
    'prints to stdout' ''
verify '{"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}}' \
    'prints to stdout' ''

verify '{' \
    'errors with parse message' '{'
verify '{:' \
    'errors with parse message' ':'
verify '{,' \
    'errors with parse message' ','
verify '{:}' \
    'errors with parse message' ':}'
verify '{,}' \
    'errors with parse message' ',}'
verify '{"key" "value"}' \
    'errors with parse message' '"key" "value"}'
verify '{"key": "value"' \
    'errors with parse message' '"value"'
verify '{"key": "value",' \
    'errors with parse message' ','
verify '{null: null}' \
    'errors with parse message' 'null: null}'
verify '{0: 0}' \
    'errors with parse message' '0: 0}'
verify '{false: false}' \
    'errors with parse message' 'false: false}'
verify '{[]: []}' \
    'errors with parse message' ']: []}'
verify '{{}: {}}' \
    'errors with parse message' '}: {}}'
verify '$key="k" $value="v" {$key: $value}' \
    'errors with parse message' '$key: $value}'
verify '{@cast("key", "STRING"): @cast("value", "STRING")}' \
    'errors with parse message' '): @cast("value", "STRING")}'
verify '{if true \ "key" /: if true \ "value" /}' \
    'errors with parse message' '/: if true \ "value" /}'
verify '{catch \ throw "key" /: catch \ throw "value" /}' \
    'errors with parse message' '/: catch \ throw "value" /}'
