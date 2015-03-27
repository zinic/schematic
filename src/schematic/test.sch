[def factoral [num]
    [if [= 1 num]
        [num]
        [* num [factoral [- num 1]]]]]

[print [factoral 3]]

[if [= 52 [- 55 [+ 1 2]]]
    [print "test"]
    [print "rar"]]
