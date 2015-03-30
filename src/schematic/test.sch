[def factoral [num]
    [if [<= num 1]
        [num]
        [* num [factoral [- num 1]]]]]

[def write-conf [factoral-num]
    [with [
            [fout [open "~/test.txt" "w"]]
        ]

        [write factoral-num fout]]]

[def read-conf []
    [with [
            [fin [open "~/test.txt" "r"]]
        ]

        [factoral [int [read fin]]]]]

[if [not [write-conf 9]]
    [print "Failed to write config!"]
    [print "Reading config...\n" [read-conf]]]
