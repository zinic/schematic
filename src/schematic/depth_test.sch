[def factoral [num]
    [if [<= num 1]
        [num]
        [* num [factoral [- num 1]]]]]

[print [factoral 10]]
