suite 'function/delete'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/delete.txt"
    printf 'TEXT FILE' > "$file"
    executefail '@delete("'"$file"'") @read("'"$file"'", null)' 'absent file'
fi
