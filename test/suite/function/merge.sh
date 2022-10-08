suite 'function/merge'

pass '@write(@merge("this", " & that"), 1)' 'this & that'
pass '@write(@merge("this", ""), 1)' 'this'
pass '@write(@merge("", "that"), 1)' 'that'

pass '@merge("this", " & that")' ''
