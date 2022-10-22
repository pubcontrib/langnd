suite='unset'

introduce()
{
    count=0

    if [ -z "$NO_PERSISTENCE" ]
    then
        umask 077
        workspace=`mktemp -d`
    fi
}

conclude()
{
    writeoutcome 'PASS' "`printf '%d tests ran successfully\n' $count`" 32

    if [ -z "$NO_PERSISTENCE" ]
    then
        rm -fr "$workspace"
    fi
}

suite()
{
    suite=$1
}

verify()
{
    source=$1
    shift
    expected_output=''
    expected_code=0
    capture_stream=1

    while [ "${1+x}" = 'x' ]
    do
        action="$1"
        shift

        case "$action" in
            'prints to stdout')
                expected_output="$1"
                shift
                capture_stream=1
                ;;
            'prints to stderr')
                expected_output="$1"
                shift
                capture_stream=2
                ;;
            'exits with code')
                expected_code=$1
                shift
                ;;
            'errors with lex message')
                expected_output="langnd: failed to lex code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;
            'errors with parse message')
                expected_output="langnd: failed to parse code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;
            'errors with execute message')
                expected_output="langnd: failed to execute code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;
            *)
                printf 'failed to verify test\n' 1>&2
                exit 1
        esac
    done

    count=`expr $count + 1`

    if [ $capture_stream = 1 ]
    then
        actual_output=`$PROGRAM -t -- "$source" 2>/dev/null`
        actual_code=$?
    elif [ $capture_stream = 2 ]
    then
        actual_output=`$PROGRAM -t -- "$source" 2>&1`
        actual_code=$?
    else
        printf 'failed to capture stream\n' 1>&2
        exit 1
    fi

    if [ $actual_code != $expected_code ]
    then
        writeoutcome 'FAIL' "`printf 'test %d failed\n' $count`" 31
        writedetail 'Reason' 'failed to match exit code of test case'
        writedetail 'Suite' "$suite"
        writedetail 'Source' "$source"
        writedetail 'Expected' "$expected_code"
        writedetail 'Actual' "$actual_code"
        exit 1
    fi

    if [ "$actual_output" != "$expected_output" ]
    then
        writeoutcome 'FAIL' "`printf 'test %d failed\n' $count`" 31
        writedetail 'Reason' 'failed to match stream of test case'
        writedetail 'Suite' "$suite"
        writedetail 'Source' "$source"
        writedetail 'Expected' "$expected_output"
        writedetail 'Actual' "$actual_output"
        exit 1
    fi
}

writeoutcome()
{
    status=$1
    text=$2
    color=$3

    if [ -z "$NO_COLOR" ]
    then
        printf '\033[7;%dm %s \033[0m\n%s\n' $color "$status" "$text" 1>&2
    else
        printf '[%s]\n%s\n' "$status" "$text" 1>&2
    fi
}

writedetail()
{
    header=$1
    text=$2

    if [ -z "$NO_COLOR" ]
    then
        printf '\033[1m%s:\033[0m\n%s\n' "$header" "$text" 1>&2
    else
        printf '%s:\n%s\n' "$header" "$text" 1>&2
    fi
}
