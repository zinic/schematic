[def first [list]
    [.first list]]

[def rfirst [list]
    [.rfirst list]]

[def len [list]
    [.len list]]

[def if [condition then otherwise]
    [.if [condition] [then] [otherwise]]]

[def empty [list]
    [> 0 [len list]]]

[def with [includes]
    [if [empty includes]
        [do [ctx]]
        [do
            [.load [first includes]]
            [with [rfirst includes] ctx]]]]
