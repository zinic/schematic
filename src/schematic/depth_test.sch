[def factoral [num]
    [if [<= num 1]
        [num]
        [* num [factoral [- num 1]]]]]

[def magic_print [content]
    [print content]]

[def a []
    [magic_print "balls"]]

[def b []
    [a]]

[b]

[print "\n" [+ 1 1] "\n"]
