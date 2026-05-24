	.set push
    .set noat
	.global func_00000F50
func_00000F50:
    la  $at, 0xbf801070         # I_STAT
    lw  $at, 0($at)

    ## Check IRQ bits
    li  $v1, (1 << 1)           # SBUS IRQ
    and $v0, $at, $v1
    bne $zero, $v0, ret

    li  $v1, (1 << 3)           # DMA IRQ
    and $v0, $at, $v1
    beq $zero, $2, ret

    la  $v0, 0xbf8010f4         # DMA_DICR
    lw  $v0, 0($v0)
    li  $v1, (1 << 26)
    and $v0, $v1

ret:
    jr $ra
	.set pop
