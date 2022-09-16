#!/bin/sh

PROGRAM=$1

testscripts()
{
    pass '' ''
    pass '100' ''
    pass '"text"' ''
    pass '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"' ''
    pass 'null' ''
    pass 'nullnull' ''
    pass 'null null' ''
    pass 'false' ''
    pass 'true' ''
    pass '$a=null'
    pass '$a=100' ''
    pass '$a="text"' ''
    pass '$A="val"' ''
    pass '$0="val"' ''
    pass '$_="val"' ''
    pass '$Text_100="val"' ''
    pass '$"(long) & [special] --> ID"="val"' ''
    pass '$a=100 $a' ''
    pass '$a="text" $a' ''
    pass '$a=100 $a=null $a' ''
    pass '@add(100, 200)' ''
    pass '@merge("this", " & that")' ''
    pass '@write(@cast(0, "STRING"), 1)' '0'
    pass '@write(@cast(1, "STRING"), 1)' '1'
    pass '@write(@cast(123, "STRING"), 1)' '123'
    pass '@write(@cast(-1, "STRING"), 1)' '-1'
    pass '@write(@cast(-123, "STRING"), 1)' '-123'
    pass '@write(@cast(32767, "STRING"), 1)' '32767'
    pass '@write(@cast(-32767, "STRING"), 1)' '-32767'
    pass '@write(@cast(0.1, "STRING"), 1)' '0.099990'
    pass '@write(@cast(0.123, "STRING"), 1)' '0.122985'
    pass '@write(@cast(-0.1, "STRING"), 1)' '-0.099990'
    pass '@write(@cast(-0.123, "STRING"), 1)' '-0.122985'
    pass '@write(@cast(123.456, "STRING"), 1)' '123.455993'
    pass '@write(@cast(-123.456, "STRING"), 1)' '-123.455993'
    pass '@write(@cast(0.999999, "STRING"), 1)' '0.999984'
    pass '@write(@cast(-0.999999, "STRING"), 1)' '-0.999984'
    pass '@write(@cast(32767.999999, "STRING"), 1)' '32767.999984'
    pass '@write(@cast(-32767.999999, "STRING"), 1)' '-32767.999984'
    pass '@write(@cast("text", "STRING"), 1)' 'text'
    pass '@write("text", 1)' 'text'
    pass '@write(@cast(false, "STRING"), 1)' 'false'
    pass '@write(@cast(true, "STRING"), 1)' 'true'
    pass '$a=100 @write(@cast($a, "STRING"), 1)' '100'
    pass '$a="text" @write($a, 1)' 'text'
    pass '$a=true @write(@cast($a, "STRING"), 1)' 'true'
    pass '$a=100 $a=200 $a=300 @write(@cast($a, "STRING"), 1)' '300'
    pass '$a=100 $b=$a @write(@cast($b, "STRING"), 1)' '100'
    pass '$a=100 $a=$a @write(@cast($a, "STRING"), 1)' '100'
    pass '@write(@cast(@and(false, false), "STRING"), 1)' 'false'
    pass '@write(@cast(@and(false, true), "STRING"), 1)' 'false'
    pass '@write(@cast(@and(true, false), "STRING"), 1)' 'false'
    pass '@write(@cast(@and(true, true), "STRING"), 1)' 'true'
    pass '@write(@cast(@or(false, false), "STRING"), 1)' 'false'
    pass '@write(@cast(@or(false, true), "STRING"), 1)' 'true'
    pass '@write(@cast(@or(true, false), "STRING"), 1)' 'true'
    pass '@write(@cast(@or(true, true), "STRING"), 1)' 'true'
    pass '@write(@cast(@not(false), "STRING"), 1)' 'true'
    pass '@write(@cast(@not(true), "STRING"), 1)' 'false'
    pass '@write(@cast(@add(0, 0), "STRING"), 1)' '0'
    pass '@write(@cast(@add(1, 2), "STRING"), 1)' '3'
    pass '@write(@cast(@add(123, 456), "STRING"), 1)' '579'
    pass '@write(@cast(@add(-1, -2), "STRING"), 1)' '-3'
    pass '@write(@cast(@add(-123, -456), "STRING"), 1)' '-579'
    pass '@write(@cast(@add(0.1, 0.2), "STRING"), 1)' '0.299987'
    pass '@write(@cast(@add(0.123, 0.456), "STRING"), 1)' '0.578979'
    pass '@write(@cast(@add(-0.1, -0.2), "STRING"), 1)' '-0.299987'
    pass '@write(@cast(@add(-0.123, -0.456), "STRING"), 1)' '-0.578979'
    pass '@write(@cast(@add(123.456, 123.456), "STRING"), 1)' '246.911987'
    pass '@write(@cast(@add(-123.456, -123.456), "STRING"), 1)' '-246.911987'
    pass '@write(@cast(@subtract(0, 0), "STRING"), 1)' '0'
    pass '@write(@cast(@subtract(1, 2), "STRING"), 1)' '-1'
    pass '@write(@cast(@subtract(123, 456), "STRING"), 1)' '-333'
    pass '@write(@cast(@subtract(-1, -2), "STRING"), 1)' '1'
    pass '@write(@cast(@subtract(-123, -456), "STRING"), 1)' '333'
    pass '@write(@cast(@subtract(0.1, 0.2), "STRING"), 1)' '-0.100006'
    pass '@write(@cast(@subtract(0.123, 0.456), "STRING"), 1)' '-0.333007'
    pass '@write(@cast(@subtract(-0.1, -0.2), "STRING"), 1)' '0.100006'
    pass '@write(@cast(@subtract(-0.123, -0.456), "STRING"), 1)' '0.333007'
    pass '@write(@cast(@subtract(123.456, 123.456), "STRING"), 1)' '0'
    pass '@write(@cast(@subtract(-123.456, -123.456), "STRING"), 1)' '0'
    pass '@write(@cast(@multiply(0, 0), "STRING"), 1)' '0'
    pass '@write(@cast(@multiply(1, 2), "STRING"), 1)' '2'
    pass '@write(@cast(@multiply(-1, -2), "STRING"), 1)' '2'
    pass '@write(@cast(@multiply(0.1, 0.2), "STRING"), 1)' '0.019989'
    pass '@write(@cast(@multiply(0.123, 0.456), "STRING"), 1)' '0.056076'
    pass '@write(@cast(@multiply(-0.1, -0.2), "STRING"), 1)' '0.019989'
    pass '@write(@cast(@multiply(-0.123, -0.456), "STRING"), 1)' '0.056076'
    pass '@write(@cast(@multiply(123.456, 123.456), "STRING"), 1)' '15241.382354'
    pass '@write(@cast(@multiply(-123.456, -123.456), "STRING"), 1)' '15241.382354'
    pass '@write(@cast(@divide(1, 2), "STRING"), 1)' '0.500000'
    pass '@write(@cast(@divide(123, 456), "STRING"), 1)' '0.269729'
    pass '@write(@cast(@divide(-1, -2), "STRING"), 1)' '0.500000'
    pass '@write(@cast(@divide(-123, -456), "STRING"), 1)' '0.269729'
    pass '@write(@cast(@divide(0.1, 0.2), "STRING"), 1)' '0.499954'
    pass '@write(@cast(@divide(0.123, 0.456), "STRING"), 1)' '0.269699'
    pass '@write(@cast(@divide(-0.1, -0.2), "STRING"), 1)' '0.499954'
    pass '@write(@cast(@divide(-0.123, -0.456), "STRING"), 1)' '0.269699'
    pass '@write(@cast(@divide(123.456, 123.456), "STRING"), 1)' '1'
    pass '@write(@cast(@divide(-123.456, -123.456), "STRING"), 1)' '1'
    pass '@write(@type(null), 1)' 'NULL'
    pass '@write(@type(false), 1)' 'BOOLEAN'
    pass '@write(@type(0), 1)' 'NUMBER'
    pass '@write(@type(""), 1)' 'STRING'
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
    pass '@write(@merge("this", " & that"), 1)' 'this & that'
    pass '@write(@merge("this", ""), 1)' 'this'
    pass '@write(@merge("", "that"), 1)' 'that'
    pass '$a=100 $b=200 @write(@cast(@add($a, $b), "STRING"), 1)' '300'
    pass '$a="this" $b=" & that" @write(@merge($a, $b), 1)' 'this & that'
    pass '$"long id"="found" @write($"long id", 1)' 'found'
    pass '$""="found" @write($"", 1)' 'found'
    pass '$"id"="found" @write($"id", 1)' 'found'
    pass '$id="found" @write($"id", 1)' 'found'
    pass '$"id"="found" @write($id, 1)' 'found'
    pass 'if false { @write("hit", 1) }' ''
    pass 'if true { @write("hit", 1) }' 'hit'
    pass 'if true { if true { @write("hit", 1) } }' 'hit'
    pass 'if true { if false { @write("hit", 1) } }' ''
    pass 'if false { if true { @write("hit", 1) } }' ''
    pass 'if false { if false { @write("hit", 1) } }' ''
    pass '$a=true if $a { @write("hit", 1) }' 'hit'
    pass '$a=false if $a { @write("hit", 1) }' ''
    pass 'if @cast("true", "BOOLEAN") { @write("hit", 1) }' 'hit'
    pass 'if @cast("false", "BOOLEAN") { @write("hit", 1) }' ''
    pass '$a=0 if true { $a=@add($a,1) $a=@add($a,1) $a=@add($a,1) } @write(@cast($a,"STRING"),1)' '3'
    pass '$msg=if true { "first" "..." "last" } @write(@cast($msg, "STRING"), 1)' 'last'
    pass '$msg=if false { "first" "..." "last" } @write(@cast($msg, "STRING"), 1)' 'null'
    pass '@write(@merge("<", if true { " >" }), 1)' '< >'
    lexfail 'NULL' 'NULL'
    lexfail 'unknown' 'unknown'
    lexfail '"missing end' '"missing end'
    lexfail 'missing start"' 'missing start"'
    lexfail '$$$var' '$$$var'
    lexfail '$' '$'
    lexfail '$var-me' '-me'
    lexfail '$-var' '$-var'
    lexfail '$"missing end' '$"missing end'
    lexfail '@not(tru)' 'tru)'
    parsefail '@add('
    parsefail '@add(,'
    parsefail '@add(,)'
    parsefail '@merge("1" "2")'
    parsefail '$x="1" $y="2" @merge($x $y)'
    parsefail '@merge(@cast(1, "STRING") @cast(2, "STRING"))'
    parsefail 'if'
    parsefail 'if { }'
    parsefail 'if true'
    parsefail 'if true {'
    parsefail 'if true }'
    parsefail 'if true 100'
    parsefail 'if true { 100'
    parsefail '$a='
    parsefail '$a=true if $a=false { }'
    parsefail '$a=1 @add($a=2, $a=3)'
    executefail '@add()' 'absent argument'
    executefail '@write()' 'absent argument'
    executefail '@cast()' 'absent argument'
    executefail '@not(@not())' 'absent argument'
    executefail '$huh' 'absent variable'
    executefail '@huh()' 'absent function'
    executefail '@add(32767, 32767)' 'arithmetic error'
    executefail '@add(1, 32767)' 'arithmetic error'
    executefail '@add(32767, 1)' 'arithmetic error'
    executefail '@subtract(-32767, 32767)' 'arithmetic error'
    executefail '@subtract(-1, 32767)' 'arithmetic error'
    executefail '@subtract(-32767, 1)' 'arithmetic error'
    executefail '@multiply(32767, 32767)' 'arithmetic error'
    executefail '@multiply(32767, -32767)' 'arithmetic error'
    executefail '@multiply(-32767, 32767)' 'arithmetic error'
    executefail '@multiply(-32767, -32767)' 'arithmetic error'
    executefail '@divide(32767, 0.5)' 'arithmetic error'
    executefail '@divide(32767, -0.5)' 'arithmetic error'
    executefail '@divide(-32767, 0.5)' 'arithmetic error'
    executefail '@divide(-32767, -0.5)' 'arithmetic error'
    executefail '@divide(100, 0)' 'arithmetic error'
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
    executefail '@add' 'unexpected reference type'
    executefail '$msg=if true { "before" @divide(100, 0) "after" }' 'arithmetic error'
}

lexfail()
{
    fail "$1" "langnd: failed to lex code
    [hint] $2"
}

parsefail()
{
    fail "$1" 'langnd: failed to parse code'
}

executefail()
{
    fail "$1" "langnd: failed to execute code
    [hint] $2"
}

pass()
{
    testscript "$1" "$2" 0 1
}

fail()
{
    testscript "$1" "$2" 1 2
}

testscript()
{
    text=$1
    expected_output=$2
    expected_code=$3
    capture_stream=$4

    if [ $capture_stream = 1 ]
    then
        actual_output=`$PROGRAM -t "$text" 2>/dev/null`
        actual_code=$?
    elif [ $capture_stream = 2 ]
    then
        actual_output=`$PROGRAM -t "$text" 2>&1`
        actual_code=$?
    else
        printf 'failed to capture stream\n' 1>&2
        exit 1
    fi

    if [ $actual_code != $expected_code ]
    then
        printf 'failed to match exit code of test case\n' 1>&2
        printf '    [source] %s\n' "$text" 1>&2
        printf '    [expected] %d\n' $expected_code 1>&2
        printf '    [actual] %d\n' $actual_code 1>&2
        exit 1
    fi

    if [ "$actual_output" != "$expected_output" ]
    then
        printf 'failed to match stream of test case\n' 1>&2
        printf '    [source] %s\n' "$text" 1>&2
        printf '    [expected] %s\n' "$expected_output" 1>&2
        printf '    [actual] %s\n' "$actual_output" 1>&2
        exit 1
    fi
}

testscripts
