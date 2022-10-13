suite 'function/delete'

file="${workspace}/delete.txt"
printf 'TEXT FILE' > "$file"
executefail '@delete("'"$file"'") @read("'"$file"'")' 'absent file'
