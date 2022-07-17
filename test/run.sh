#!/bin/sh

PROGRAM=$1

testscripts()
{
    testscript '' '' 0
    testscript '100' '' 0
    testscript '$a=100' '' 0
    testscript '@add(100, 200)' '' 0
    testscript '$a=100 $b=200 $c=@add($a, $b)' '' 0
    testscript '$a=100 $a' '' 0
    testscript '@add' '' 0
    testscript '$$$a' '' 1
    testscript '@add,,,' '' 1
    testscript '@add()' '' 1
    testscript '$huh' '' 1
    testscript '@huh()' '' 1
}

testscript()
{
    text=$1
    expected_output=$2
    expected_code=$3

    actual_output=`$PROGRAM -t "$text" 2>/dev/null`
    actual_code=$?

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
        printf 'failed to match stdout of test case\n' 1>&2
        printf '    [source] %s\n' "$text" 1>&2
        printf '    [expected] %d\n' "$expected_output" 1>&2
        printf '    [actual] %d\n' "$actual_output" 1>&2
        exit 1
    fi
}

testscripts
