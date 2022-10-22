suite 'function/delete'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/delete.txt"

    printf 'TEXT FILE' > "$file"
    verify '@delete("'"$file"'")' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify '@delete("'"$file"'") @read("'"$file"'", null)' \
        'errors with execute message' 'absent file'
fi

verify '@delete(0)' \
    'errors with execute message' 'unable to delete file'
verify '@delete(1)' \
    'errors with execute message' 'unable to delete file'
verify '@delete(2)' \
    'errors with execute message' 'unable to delete file'
