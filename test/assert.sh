suite='unset'

introduce()
{
    count=0
    source_format="${SOURCE_FORMAT:-text}"

    case "$source_format" in
        'text')
            ;;

        'file')
            if [ -n "$NO_PERSISTENCE" ]
            then
                printf 'persistence is required\n' 1>&2
                exit 1
            fi
            ;;

        *)
            printf 'invalid source format\n' 1>&2
            exit 1
            ;;
    esac

    if [ -z "$NO_PERSISTENCE" ]
    then
        umask 077
        workspace=`mktemp -d`
    fi
}

conclude()
{
    writeoutcome 'PASS' "`printf '%d tests ran successfully\n' $count`" 32
    cleanup
}

cleanup()
{
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
    argv=''

    # Increment the test count
    count=`expr $count + 1`

    # Keep building up test setup while there are more arguments
    while [ "${1+x}" = 'x' ]
    do
        expression="$1"
        shift

        case "$expression" in
            'prints to stdout')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing output'
                    break
                fi

                expected_output="$1"
                shift
                capture_stream=1
                ;;

            'prints to stderr')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing output'
                    break
                fi

                expected_output="$1"
                shift
                capture_stream=2
                ;;

            'with stdin')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing input'
                    break
                fi

                input="$1"
                shift
                ;;

            'with argv')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing input'
                    break
                fi

                argv="$argv $1"
                shift
                ;;

            'exits with code')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing exit code'
                    break
                fi

                expected_code=$1
                shift
                ;;

            'errors with lex message')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing error hint'
                    break
                fi

                expected_output="langnd: failed to lex code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;

            'errors with parse message')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing error hint'
                    break
                fi

                expected_output="langnd: failed to parse code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;

            'errors with execute message')
                if [ "${1+x}" != 'x' ]
                then
                    flaw='missing error hint'
                    break
                fi

                expected_output="langnd: failed to execute code
    [hint] $1"
                shift
                capture_stream=2
                expected_code=1
                ;;

            *)
                flaw='unknown test expression'
                break
        esac
    done

    # Ensure no test setup failed part way through
    if [ "${flaw+x}" = 'x' ]
    then
        writeoutcome 'FLAW' "`printf 'test %d is flawed\n' $count`" 31
        writedetail 'Reason' "$flaw"
        writedetail 'Suite' "$suite"
        writedetail 'Source' "$source"
        exit 1
    fi

    # Run the test while waiting for output and an exit code
    actual_output=

    if [ "$source_format" = 'text' ]
    then
        if [ $capture_stream = 1 ]
        then
            if [ -z "$input" ]
            then
                actual_output=`$PROGRAM -t -- "$source" $argv 2>/dev/null`
            else
                actual_output=`printf '%s' "$input" | $PROGRAM -t -- "$source" $argv 2>/dev/null`
            fi
        elif [ $capture_stream = 2 ]
        then
            if [ -z "$input" ]
            then
                actual_output=`$PROGRAM -t -- "$source" $argv 2>&1 > /dev/null`
            else
                actual_output=`printf '%s' "$input" | $PROGRAM -t -- "$source" $argv 2>&1 > /dev/null`
            fi
        fi
    elif [ "$source_format" = 'file' ]
    then
        source_file="${workspace}/source.txt"
        printf '%s' "$source" > "$source_file"

        if [ $capture_stream = 1 ]
        then
            if [ -z "$input" ]
            then
                actual_output=`$PROGRAM -f "$source_file" $argv 2>/dev/null`
            else
                actual_output=`printf '%s' "$input" | $PROGRAM -f "$source_file" $argv 2>/dev/null`
            fi
        elif [ $capture_stream = 2 ]
        then
            if [ -z "$input" ]
            then
                actual_output=`$PROGRAM -f "$source_file" $argv 2>&1 > /dev/null`
            else
                actual_output=`printf '%s' "$input" | $PROGRAM -f "$source_file" $argv 2>&1 > /dev/null`
            fi
        fi
    fi

    actual_code=$?

    # Ensure the run had the expected exit code
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

    # Ensure the run had the expected output
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

    # Any test making it this far passes
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
