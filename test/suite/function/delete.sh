suite 'function/delete'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/delete.txt"

    printf 'TEXT FILE' > "$file"
    pass '@delete("'"$file"'")' ''
    printf 'TEXT FILE' > "$file"
    executefail '@delete("'"$file"'") @read("'"$file"'", null)' 'absent file'
fi

executefail '@delete(0)' 'unable to delete file'
executefail '@delete(1)' 'unable to delete file'
executefail '@delete(2)' 'unable to delete file'
