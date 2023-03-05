suite 'function/read'

verify 'import "read" from core $read(0, null)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' ''
verify 'import ["write", "read"] from core $write($read(0, null), 1)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' 'TEXT INPUT'
verify 'import ["write", "read"] from core $write($read(0, null), 1)' \
    'with stdin' 'LINE 1
LINE 2
LINE 3' \
    'prints to stdout' 'LINE 1
LINE 2
LINE 3'
verify 'import "read" from core $read(0, "\n")' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' ''
verify 'import ["write", "read"] from core $write($read(0, "\n"), 1)' \
    'with stdin' 'TEXT INPUT' \
    'prints to stdout' 'TEXT INPUT'
verify 'import ["write", "read"] from core $write($read(0, "\n"), 1)' \
    'with stdin' 'LINE 1
LINE 2
LINE 3' \
    'prints to stdout' 'LINE 1'
verify 'import ["write", "read"] from core $write($read(0, "\n"), 1) $write($read(0, "\n"), 1) $write($read(0, "\n"), 1) $write($read(0, "\n"), 1)' \
    'with stdin' 'LINE 1
LINE 2
LINE 3' \
    'prints to stdout' 'LINE 1LINE 2LINE 3'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/read.txt"

    printf 'TEXT FILE' > "$file"
    verify 'import "read" from core $read("'"$file"'", null)' \
        'prints to stdout' ''
    printf '' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", null), 1)' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", null), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", null), 1)' \
        'prints to stdout' 'LINE 1
LINE 2
LINE 3'
    printf '' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", "\n"), 1)' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'TEXT FILE'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'LINE 1'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", "\n"), 1) $write($read("'"$file"'", "\n"), 1) $write($read("'"$file"'", "\n"), 1) $write($read("'"$file"'", "\n"), 1)' \
        'prints to stdout' 'LINE 1LINE 1LINE 1LINE 1'

    printf 'WORD 1' > "$file"
    dd if=/dev/zero of="$file" ibs=1 count=1 obs=1 seek=6 conv=notrunc 2>/dev/null
    printf 'WORD 2' >> "$file"
    dd if=/dev/zero of="$file" ibs=1 count=1 obs=1 seek=13 conv=notrunc 2>/dev/null
    printf 'WORD 3' >> "$file"
    verify 'import ["write", "read"] from core $write($read("'"$file"'", "\a000"), 1)' \
        'prints to stdout' 'WORD 1'
fi

verify 'import "read" from core $read(0, "")' \
    'errors with execute message' '"damaged argument"'
verify 'import "read" from core $read(0, "END")' \
    'errors with execute message' '"damaged argument"'
