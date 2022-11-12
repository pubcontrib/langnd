suite 'function/get'

verify '@write(@freeze(@get("abc", 1)), 1)' \
    'prints to stdout' '"a"'
verify '@write(@freeze(@get("abc", 2)), 1)' \
    'prints to stdout' '"b"'
verify '@write(@freeze(@get("abc", 3)), 1)' \
    'prints to stdout' '"c"'
verify '@write(@freeze(@get(["a", "b", "c"], 1)), 1)' \
    'prints to stdout' '"a"'
verify '@write(@freeze(@get(["a", "b", "c"], 2)), 1)' \
    'prints to stdout' '"b"'
verify '@write(@freeze(@get(["a", "b", "c"], 3)), 1)' \
    'prints to stdout' '"c"'

verify '$item=@get(["x"], 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$list=["x"] $item=@get($list, 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'
verify '$list=[["x"]] $list=@get($list, 1) $item=@get($list, 1) @write(@freeze($item), 1)' \
    'prints to stdout' '"x"'

verify '@write(@freeze(@get("abc", 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get("abc", 4)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get(["a", "b", "c"], 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get(["a", "b", "c"], 4)), 1)' \
    'errors with execute message' '"absent key"'
