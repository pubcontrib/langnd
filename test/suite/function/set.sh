suite 'function/set'

verify '@write(@freeze(@set("abc", 1, "x")), 1)' \
    'prints to stdout' '"xbc"'
verify '@write(@freeze(@set("abc", 2, "x")), 1)' \
    'prints to stdout' '"axc"'
verify '@write(@freeze(@set("abc", 3, "x")), 1)' \
    'prints to stdout' '"abx"'
verify '@write(@freeze(@set("abc", 1, "")), 1)' \
    'prints to stdout' '"bc"'
verify '@write(@freeze(@set("abc", 2, "")), 1)' \
    'prints to stdout' '"ac"'
verify '@write(@freeze(@set("abc", 3, "")), 1)' \
    'prints to stdout' '"ab"'
verify '@write(@freeze(@set("abc", 1, "123")), 1)' \
    'prints to stdout' '"123bc"'
verify '@write(@freeze(@set("abc", 2, "123")), 1)' \
    'prints to stdout' '"a123c"'
verify '@write(@freeze(@set("abc", 3, "123")), 1)' \
    'prints to stdout' '"ab123"'
verify '@write(@freeze(@set(["a", "b", "c"], 1, "x")), 1)' \
    'prints to stdout' '["x", "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 2, "x")), 1)' \
    'prints to stdout' '["a", "x", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 3, "x")), 1)' \
    'prints to stdout' '["a", "b", "x"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, null)), 1)' \
    'prints to stdout' '[null, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, false)), 1)' \
    'prints to stdout' '[false, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, 0)), 1)' \
    'prints to stdout' '[0, "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, "")), 1)' \
    'prints to stdout' '["", "b", "c"]'
verify '@write(@freeze(@set(["a", "b", "c"], 1, [])), 1)' \
    'prints to stdout' '[[], "b", "c"]'

verify '$list1=["x"] $list2=@set($list1, 1, $list1) @write(@freeze($list1), 1) @write(@freeze($list2), 1)' \
    'prints to stdout' '["x"][["x"]]'
verify '$list=["x"] $list=@set($list, 1, $list) @write(@freeze($list), 1)' \
    'prints to stdout' '[["x"]]'

verify '@write(@freeze(@set("abc", 0, "x")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set("abc", 4, "x")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set("abc", 0, "")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set("abc", 4, "")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set("abc", 0, "xxx")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set("abc", 4, "xxx")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set(["a", "b", "c"], 0, "x")), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@set(["a", "b", "c"], 4, "x")), 1)' \
    'errors with execute message' '"absent key"'
