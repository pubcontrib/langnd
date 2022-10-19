suite 'function/read'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/read.txt"
    printf 'TEXT FILE' > "$file"
    pass '@write(@read("'"$file"'", null), 1)' 'TEXT FILE'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    pass '@write(@read("'"$file"'", null), 1)' 'LINE 1
LINE 2
LINE 3'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    pass '@write(@read("'"$file"'", "\n"), 1)' 'LINE 1'
    printf 'LINE 1\nLINE 2\nLINE 3' > "$file"
    pass '@write(@read("'"$file"'", "\n"), 1) @write(@read("'"$file"'", "\n"), 1)' 'LINE 1LINE 1'
fi

executefail '@read(0, "")' 'invalid terminator'
executefail '@read(0, "END")' 'invalid terminator'
