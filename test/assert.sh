suite='unset'

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
        printf '    [suite] %s\n' "$suite" 1>&2
        printf '    [source] %s\n' "$text" 1>&2
        printf '    [expected] %d\n' $expected_code 1>&2
        printf '    [actual] %d\n' $actual_code 1>&2
        exit 1
    fi

    if [ "$actual_output" != "$expected_output" ]
    then
        printf 'failed to match stream of test case\n' 1>&2
        printf '    [suite] %s\n' "$suite" 1>&2
        printf '    [source] %s\n' "$text" 1>&2
        printf '    [expected] %s\n' "$expected_output" 1>&2
        printf '    [actual] %s\n' "$actual_output" 1>&2
        exit 1
    fi
}
