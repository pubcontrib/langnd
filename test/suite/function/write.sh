suite 'function/write'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/write.txt"
    printf '' > "$file"
    pass '@write("TEXT FILE", "'"$file"'") @write(@read("'"$file"'", null), 1)' 'TEXT FILE'
fi

executefail '@write()' 'absent argument'
