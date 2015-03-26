[def print [content]
    [.print content]]

[def if [condition then otherwise]
    [.if condition then otherwise]]

[if [= 53 [- 55 [+ 1 1]]]
    [print "test"]
    [print "rar"]]
