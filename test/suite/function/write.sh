suite 'function/write'

file="${workspace}/write.txt"
printf '' > "$file"
pass '@write("TEXT FILE", "'"$file"'") @write(@read("'"$file"'"), 1)' 'TEXT FILE'

executefail '@write()' 'absent argument'
