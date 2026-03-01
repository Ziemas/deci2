	.set push            
    .set noat            
	.global func_00000F50
func_00000F50:      
    la  $1, 0xbf801070   
    lw  $1, 0($1)        
    
    li  $3, 2            
    and $2, $1, $3       
    bne $0, $2, ret      
    
    li  $3, 8            
    and $2, $1, $3       
    beq $0, $2, ret      

    la  $2, 0xbf8010f4   
    lw  $2, 0($2)        
    li  $3, 0x4000000    
    and $2, $3           

ret:                     
    jr $31             
	.set pop             
