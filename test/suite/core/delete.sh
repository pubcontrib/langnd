suite 'core/delete'

if [ -z "$NO_PERSISTENCE" ]
then
    file="${workspace}/delete.txt"

    printf 'TEXT FILE' > "$file"
    verify 'import "delete" from core $delete("'"$file"'")' \
        'prints to stdout' ''
    printf 'TEXT FILE' > "$file"
    verify 'import ["delete", "read"] from core $delete("'"$file"'") $read("'"$file"'", null)' \
        'errors with execute message' '"absent file"'
fi

verify 'import "delete" from core $delete(0)' \
    'errors with execute message' '"io error"'
verify 'import "delete" from core $delete(1)' \
    'errors with execute message' '"io error"'
verify 'import "delete" from core $delete(2)' \
    'errors with execute message' '"io error"'
