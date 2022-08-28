#!/bin/sh

PROGRAM=$1

testscripts()
{
    pass '' ''
    pass '100' ''
    pass '"text"' ''
    pass '"\t\n\r !\"#$%&'"'"'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"' ''
    pass '$a=100' ''
    pass '$a="text"' ''
    pass '$A="val"' ''
    pass '$0="val"' ''
    pass '$_="val"' ''
    pass '$Text_100="val"' ''
    pass '$"(long) & [special] --> ID"="val"' ''
    pass '$a=100 $a' ''
    pass '$a="text" $a' ''
    pass '@add(100, 200)' ''
    pass '@merge("this", " & that")' ''
    pass '@write(@stringify(0), 1)' '0'
    pass '@write(@stringify(1), 1)' '1'
    pass '@write(@stringify(123), 1)' '123'
    pass '@write(@stringify(-1), 1)' '-1'
    pass '@write(@stringify(-123), 1)' '-123'
    pass '@write(@stringify(32767), 1)' '32767'
    pass '@write(@stringify(-32767), 1)' '-32767'
    pass '@write(@stringify(0.1), 1)' '0.099990'
    pass '@write(@stringify(0.123), 1)' '0.122985'
    pass '@write(@stringify(-0.1), 1)' '-0.099990'
    pass '@write(@stringify(-0.123), 1)' '-0.122985'
    pass '@write(@stringify(123.456), 1)' '123.455993'
    pass '@write(@stringify(-123.456), 1)' '-123.455993'
    pass '@write(@stringify(0.999999), 1)' '0.999984'
    pass '@write(@stringify(-0.999999), 1)' '-0.999984'
    pass '@write(@stringify(32767.999999), 1)' '32767.999984'
    pass '@write(@stringify(-32767.999999), 1)' '-32767.999984'
    pass '@write(@stringify("text"), 1)' 'text'
    pass '@write("text", 1)' 'text'
    pass '$a=100 @write(@stringify($a), 1)' '100'
    pass '$a="text" @write($a, 1)' 'text'
    pass '@write(@stringify(@add(0, 0)), 1)' '0'
    pass '@write(@stringify(@add(1, 2)), 1)' '3'
    pass '@write(@stringify(@add(123, 456)), 1)' '579'
    pass '@write(@stringify(@add(-1, -2)), 1)' '-3'
    pass '@write(@stringify(@add(-123, -456)), 1)' '-579'
    pass '@write(@stringify(@add(0.1, 0.2)), 1)' '0.299987'
    pass '@write(@stringify(@add(0.123, 0.456)), 1)' '0.578979'
    pass '@write(@stringify(@add(-0.1, -0.2)), 1)' '-0.299987'
    pass '@write(@stringify(@add(-0.123, -0.456)), 1)' '-0.578979'
    pass '@write(@stringify(@add(123.456, 123.456)), 1)' '246.911987'
    pass '@write(@stringify(@add(-123.456, -123.456)), 1)' '-246.911987'
    pass '@write(@stringify(@subtract(0, 0)), 1)' '0'
    pass '@write(@stringify(@subtract(1, 2)), 1)' '-1'
    pass '@write(@stringify(@subtract(123, 456)), 1)' '-333'
    pass '@write(@stringify(@subtract(-1, -2)), 1)' '1'
    pass '@write(@stringify(@subtract(-123, -456)), 1)' '333'
    pass '@write(@stringify(@subtract(0.1, 0.2)), 1)' '-0.100006'
    pass '@write(@stringify(@subtract(0.123, 0.456)), 1)' '-0.333007'
    pass '@write(@stringify(@subtract(-0.1, -0.2)), 1)' '0.100006'
    pass '@write(@stringify(@subtract(-0.123, -0.456)), 1)' '0.333007'
    pass '@write(@stringify(@subtract(123.456, 123.456)), 1)' '0'
    pass '@write(@stringify(@subtract(-123.456, -123.456)), 1)' '0'
    pass '@write(@stringify(@multiply(0, 0)), 1)' '0'
    pass '@write(@stringify(@multiply(1, 2)), 1)' '2'
    pass '@write(@stringify(@multiply(123, 456)), 1)' '0'
    pass '@write(@stringify(@multiply(-1, -2)), 1)' '2'
    pass '@write(@stringify(@multiply(-123, -456)), 1)' '0'
    pass '@write(@stringify(@multiply(0.1, 0.2)), 1)' '0.019989'
    pass '@write(@stringify(@multiply(0.123, 0.456)), 1)' '0.056076'
    pass '@write(@stringify(@multiply(-0.1, -0.2)), 1)' '0.019989'
    pass '@write(@stringify(@multiply(-0.123, -0.456)), 1)' '0.056076'
    pass '@write(@stringify(@multiply(123.456, 123.456)), 1)' '15241.382354'
    pass '@write(@stringify(@multiply(-123.456, -123.456)), 1)' '15241.382354'
    pass '@write(@stringify(@divide(0, 0)), 1)' '0'
    pass '@write(@stringify(@divide(1, 2)), 1)' '0.500000'
    pass '@write(@stringify(@divide(123, 456)), 1)' '0.269729'
    pass '@write(@stringify(@divide(-1, -2)), 1)' '0.500000'
    pass '@write(@stringify(@divide(-123, -456)), 1)' '0.269729'
    pass '@write(@stringify(@divide(0.1, 0.2)), 1)' '0.499954'
    pass '@write(@stringify(@divide(0.123, 0.456)), 1)' '0.269699'
    pass '@write(@stringify(@divide(-0.1, -0.2)), 1)' '0.499954'
    pass '@write(@stringify(@divide(-0.123, -0.456)), 1)' '0.269699'
    pass '@write(@stringify(@divide(123.456, 123.456)), 1)' '1'
    pass '@write(@stringify(@divide(-123.456, -123.456)), 1)' '1'
    pass '@write(@merge("this", " & that"), 1)' 'this & that'
    pass '@write(@merge("this", ""), 1)' 'this'
    pass '@write(@merge("", "that"), 1)' 'that'
    pass '$a=100 $b=200 @write(@stringify(@add($a, $b)), 1)' '300'
    pass '$a="this" $b=" & that" @write(@merge($a, $b), 1)' 'this & that'
    pass '$"long id"="found" @write($"long id", 1)' 'found'
    pass '$""="found" @write($"", 1)' 'found'
    pass '$"id"="found" @write($"id", 1)' 'found'
    pass '$id="found" @write($"id", 1)' 'found'
    pass '$"id"="found" @write($id, 1)' 'found'
    pass '@add' ''
    lexfail '"missing end' '"missing end'
    lexfail 'missing start"' 'missing start"'
    lexfail '$$$var' '$$$var'
    lexfail '$' '$'
    lexfail '$var-me' '-me'
    lexfail '$-var' '$-var'
    lexfail '$"missing end' '$"missing end'
    parsefail '@add,,,'
    executefail '@add()' 'absent argument'
    executefail '@write()' 'absent argument'
    executefail '@stringify()' 'absent argument'
    executefail '$huh' 'absent variable'
    executefail '@huh()' 'absent function'
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
