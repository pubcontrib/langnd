suite 'function/read'

file="${workspace}/read.txt"
printf 'TEXT FILE' > "$file"
pass '@write(@read("'"$file"'"), 1)' 'TEXT FILE'
