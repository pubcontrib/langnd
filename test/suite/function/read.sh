suite 'function/read'

verify '@read(0, null)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' ''
verify '@write(@read(0, null), 1)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' 'TEXT INPUT'
verify '@write(@read(0, null), 1)' \
    'with stdin' 'LINE 1\nLINE 2\nLINE 3' \
    'prints to stdout' 'LINE 1
LINE 2
LINE 3'
verify '@read(0, "\n")' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' ''
verify '@write(@read(0, "\n"), 1)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' 'TEXT INPUT'
verify '@write(@read(0, "\n"), 1)' \
    'with stdin' 'LINE 1\nLINE 2\nLINE 3' \
    'prints to stdout' 'LINE 1'
verify '@write(@read(0, "\n"), 1) @write(@read(0, "\n"), 1) @write(@read(0, "\n"), 1) @write(@read(0, "\n"), 1)' \
    'with stdin' 'LINE 1\nLINE 2\nLINE 3' \
    'prints to stdout' 'LINE 1LINE 2LINE 3'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/read.txt"

    printf 'TEXT FILE' > "$file"
    verify '@read("'"$file"'", null)' \
        'prints to stdout' ''
    printf '' > "$file"
    verify '@write(@read("'"$file"'", null), 1)' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify '@write(@read("'"$file"'", null), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify '@write(@read("'"$file"'", null), 1)' \
        'prints to stdout' 'LINE 1
LINE 2
LINE 3'
    printf '' > "$file"
    verify '@write(@read("'"$file"'", "\n"), 1)' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify '@write(@read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify '@write(@read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'LINE 1'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify '@write(@read("'"$file"'", "\n"), 1) @write(@read("'"$file"'", "\n"), 1) @write(@read("'"$file"'", "\n"), 1) @write(@read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'LINE 1LINE 1LINE 1LINE 1'
fi

verify '@read(0, "")' \
    'errors with execute message' 'invalid terminator'
verify '@read(0, "END")' \
    'errors with execute message' 'invalid terminator'
