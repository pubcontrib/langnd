suite 'function/write'

verify '@write("", 1)' \
    'prints to stdout' ''
verify '@write("TEXT STREAM", 1)' \
    'prints to stdout' 'TEXT STREAM'
verify '@write("LINE 1\nLINE 2\nLINE 3", 1)' \
    'prints to stdout' 'LINE 1
LINE 2
LINE 3'
verify '@write("", 2)' \
    'prints to stderr' ''
verify '@write("TEXT STREAM", 2)' \
    'prints to stderr' 'TEXT STREAM'
verify '@write("LINE 1\nLINE 2\nLINE 3", 2)' \
    'prints to stderr' 'LINE 1
LINE 2
LINE 3'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/write.txt"

    printf '' > "$file"
    verify '@write("TEXT FILE", "'"$file"'")' \
        'prints to stdout' ''
    printf '' > "$file"
    verify '@write("", "'"$file"'") @write(@read("'"$file"'", null), 1)' \
        'prints to stdout' ''
    printf '' > "$file"
    verify '@write("TEXT FILE", "'"$file"'") @write(@read("'"$file"'", null), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf '' > "$file"
    verify '@write("FIRST", "'"$file"'") @write("LAST", "'"$file"'") @write(@read("'"$file"'", null), 1)' \
        'prints to stdout' 'LAST'
fi

verify '@write()' \
    'errors with execute message' 'absent argument'
