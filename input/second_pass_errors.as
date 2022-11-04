;contains second pass errors


K:		.data 22

;undefined operand symbol 'W'
MAIN:	mov S1.1, W
     
		add r2,STR
S2:		.struct -1	 , 	 "12345"


;undefined operand symbol 'W'
XYZ: 	jmp W

		sub r1	, r4	
		bne L3
END:	hlt
STR:	.string "abcdef"

LENGTH:	.data 6, -512 ,511
S1:		.struct 8, "ab"


.entry K	
.extern ABC
.extern L3

; entry symbol is already defined as an external symbol
.entry ABC	
