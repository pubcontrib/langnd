suite 'function/cast'

pass '@write(@cast(@cast(null, "NULL"), "STRING"), 1)' 'null'
pass '@write(@cast(@cast("null", "NULL"), "STRING"), 1)' 'null'
pass '@write(@cast(@cast(true, "BOOLEAN"), "STRING"), 1)' 'true'
pass '@write(@cast(@cast("true", "BOOLEAN"), "STRING"), 1)' 'true'
pass '@write(@cast(@cast(123.456, "NUMBER"), "STRING"), 1)' '123.455993'
pass '@write(@cast(@cast("123.456", "NUMBER"), "STRING"), 1)' '123.455993'
pass '@write(@cast(null, "STRING"), 1)' 'null'
pass '@write(@cast(true, "STRING"), 1)' 'true'
pass '@write(@cast(123.456, "STRING"), 1)' '123.455993'
pass '@write(@cast("text", "STRING"), 1)' 'text'

executefail '@cast(123.456, "NULL")' 'invalid cast'
executefail '@cast(true, "NULL")' 'invalid cast'
executefail '@cast("text", "NULL")' 'invalid cast'
executefail '@cast(null, "BOOLEAN")' 'invalid cast'
executefail '@cast(123.456, "BOOLEAN")' 'invalid cast'
executefail '@cast("text", "BOOLEAN")' 'invalid cast'
executefail '@cast(null, "NUMBER")' 'invalid cast'
executefail '@cast(true, "NUMBER")' 'invalid cast'
executefail '@cast("text", "NUMBER")' 'invalid cast'
executefail '@cast(null, "WRONG")' 'unknown type'