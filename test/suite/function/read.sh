suite 'function/read'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/read.txt"
    printf 'TEXT FILE' > "$file"
    pass '@write(@read("'"$file"'"), 1)' 'TEXT FILE'
fi
