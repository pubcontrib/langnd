suite 'function/write'

verify 'import "write" from core $write("", 1)' \
    'prints to stdout' ''
verify 'import "write" from core $write("TEXT STREAM", 1)' \
    'prints to stdout' 'TEXT STREAM'
verify 'import "write" from core $write("LINE 1\nLINE 2\nLINE 3", 1)' \
    'prints to stdout' 'LINE 1
LINE 2
LINE 3'
verify 'import "write" from core $write("", 2)' \
    'prints to stderr' ''
verify 'import "write" from core $write("TEXT STREAM", 2)' \
    'prints to stderr' 'TEXT STREAM'
verify 'import "write" from core $write("LINE 1\nLINE 2\nLINE 3", 2)' \
    'prints to stderr' 'LINE 1
LINE 2
LINE 3'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/write.txt"

    printf '' > "$file"
    verify 'import "write" from core $write("TEXT FILE", "'"$file"'")' \
        'prints to stdout' ''
    printf '' > "$file"
    verify 'import ["write", "read"] from core $write("", "'"$file"'") $write($read("'"$file"'", null), 1)' \
        'prints to stdout' ''
    printf '' > "$file"
    verify 'import ["write", "read"] from core $write("TEXT FILE", "'"$file"'") $write($read("'"$file"'", null), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf '' > "$file"
    verify 'import ["write", "read"] from core $write("FIRST", "'"$file"'") $write("LAST", "'"$file"'") $write($read("'"$file"'", null), 1)' \
        'prints to stdout' 'LAST'
fi
