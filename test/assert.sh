suite='unset'

introduce()
{
    count=0
}

conclude()
{
    printf '\033[7;32m PASS \033[0m\n' 1>&2
    printf '%d tests ran successfully\n' $count 1>&2
}

suite()
{
    suite=$1
}

lexfail()
{
    fail "$1" "langnd: failed to lex code
    [hint] $2"
}

parsefail()
{
    fail "$1" "langnd: failed to parse code
    [hint] $2"
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

    count=`expr $count + 1`

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
        printf '\033[7;31m FAIL \033[0m\n' 1>&2
        printf 'test %d failed\n' $count 1>&2
        printf '\033[1mreason: \033[0m\nfailed to match exit code of test case\n' 1>&2
        printf '\033[1msuite: \033[0m\n%s\n' "$suite" 1>&2
        printf '\033[1msource: \033[0m\n%s\n' "$text" 1>&2
        printf '\033[1mexpected: \033[0m\n%d\n' $expected_code 1>&2
        printf '\033[1mactual: \033[0m\n%d\n' $actual_code 1>&2
        exit 1
    fi

    if [ "$actual_output" != "$expected_output" ]
    then
        printf '\033[7;31m FAIL \033[0m\n' 1>&2
        printf 'test %d failed\n' $count 1>&2
        printf '\033[1mreason: \033[0m\nfailed to match stream of test case\n' 1>&2
        printf '\033[1msuite: \033[0m\n%s\n' "$suite" 1>&2
        printf '\033[1msource: \033[0m\n%s\n' "$text" 1>&2
        printf '\033[1mexpected: \033[0m\n%s\n' "$expected_output" 1>&2
        printf '\033[1mactual: \033[0m\n%s\n' "$actual_output" 1>&2
        exit 1
    fi
}
