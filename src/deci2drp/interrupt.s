    .set push
    .set noat

    .global func_00001820
func_00001820:
    la  $2, 0xbf801070
    lw  $2, 0x0($2)
    li  $3, 0x800000
    and $2, $3
    ## label prevents putting the and in the delay slot
1:
    jr  $31

    .global func_0000183C
func_0000183C:
    la  $1, 0xbf803820
    lhu $2, 0x0($1)
    lhu $3, 0x4($1)
    xor $2, $3
    lhu $3, 0x8($1)
    and $2, $3
1:
    jr  $31

    .set pop
