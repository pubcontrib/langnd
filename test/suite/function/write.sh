suite 'function/write'

pass '@write("", 1)' ''
pass '@write("TEXT STREAM", 1)' 'TEXT STREAM'
pass '@write("LINE 1\nLINE 2\nLINE 3", 1)' 'LINE 1
LINE 2
LINE 3'
testscript '@write("", 2)' '' 0 2
testscript '@write("TEXT STREAM", 2)' 'TEXT STREAM' 0 2
testscript '@write("LINE 1\nLINE 2\nLINE 3", 2)' 'LINE 1
LINE 2
LINE 3' 0 2

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/write.txt"

    printf '' > "$file"
    pass '@write("TEXT FILE", "'"$file"'")' ''
    printf '' > "$file"
    pass '@write("", "'"$file"'") @write(@read("'"$file"'", null), 1)' ''
    printf '' > "$file"
    pass '@write("TEXT FILE", "'"$file"'") @write(@read("'"$file"'", null), 1)' 'TEXT FILE'
    printf '' > "$file"
    pass '@write("FIRST", "'"$file"'") @write("LAST", "'"$file"'") @write(@read("'"$file"'", null), 1)' 'LAST'
fi

executefail '@write()' 'absent argument'
