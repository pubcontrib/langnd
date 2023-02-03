suite 'function/precedes'

verify '@write(@freeze(@precedes(null, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(null, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(null, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(false, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(true, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, 200)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(200, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, -100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(-100, 100)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(32767.999999, -32767.999999)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(-32767.999999, 32767.999999)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(100, "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", "word")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("word", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", "TEXT")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("TEXT", "text")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("\a255", "\a000")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("\a000", "\a255")), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes("text", [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([1], [])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([], [1])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([1, 2, 3], [1, 2])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2], [1, 2, 3])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([3, 2, 1], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], [3, 2, 1])), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([1, 2, 3], [1, 2, 3])), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes([1, 2, 3], {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([1, 2, 3], <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes({"a": 1}, {})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({}, {"a": 1})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, {"a": 1, "b": 2})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes({"c": 3, "b": 2, "a": 1}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, {"c": 3, "b": 2, "a": 1})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, {"a": 1, "b": 2, "c": 3})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 2}, {"a": 1})), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1}, {"a": 2})), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, null)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, 100)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, "text")), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(<$variable="value" $copy=$variable>, <>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(<>, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(<$variable="value" $copy=$variable>, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(<1>, <2>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(<2>, <1>)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@precedes(null, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(true, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes(100, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes("text", <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes([1, 2, 3], <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@precedes({"a": 1, "b": 2, "c": 3}, <$variable="value" $copy=$variable>)), 1)' \
    'prints to stdout' 'true'
