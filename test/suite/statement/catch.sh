suite 'statement/catch'

verify 'catch { }' \
    'prints to stdout' ''
verify 'catch { @divide(100, 0) }' \
    'prints to stdout' ''
verify 'catch { "not an error" }' \
    'prints to stdout' ''
verify '$value=100 catch { $value }' \
    'prints to stdout' ''
verify '$error=catch { } @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$error=catch { @divide(100, 0) } @write(@freeze($error), 1)' \
    'prints to stdout' '"arithmetic error"'
verify '$error=catch { "not an error" } @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$value=100 $error=catch { $value } @write(@freeze($error), 1)' \
    'prints to stdout' 'null'
verify '$error=catch { @missing() @divide(100, 0) $missing } @write(@freeze($error), 1)' \
    'prints to stdout' '"absent function"'
verify '$error=catch { @divide(100, 0) } $error=$error @write($error, 1)' \
    'prints to stdout' 'arithmetic error'
verify '@write(@merge(catch { @divide(100, 0) }, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify '$error=catch { @divide(100, 0) } @write(@merge($error, "!"), 1)' \
    'prints to stdout' 'arithmetic error!'
verify '$error=catch { throw "custom error" } @write($error, 1)' \
    'prints to stdout' 'custom error'
verify '$error=catch { $me="custom error" throw $me } @write($error, 1)' \
    'prints to stdout' 'custom error'
verify '$error=catch { throw @merge("custom ", "error") } @write($error, 1)' \
    'prints to stdout' 'custom error'
