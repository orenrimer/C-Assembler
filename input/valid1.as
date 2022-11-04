K:		.data 22
MAIN:	mov S1.1, W
		add r2,STR
S2:		.struct -1	 , 	 "12345"


XYZ: 	jmp W
		macro m1
		inc K
        
		
; This is comment!!!
		mov S1.2 ,	r3	
		endmacro
		prn 	#-50
		sub r1	, r4
		
		bne L3
END:	hlt
STR:	.string "abcdef"
				m1   
LENGTH:	.data 6,-112, +108

macro m2
	add r2,STR
	sub r1	, r4
	;;;;;	
	bne L3
endmacro

S1:		.struct 8, "ab"
m2

.entry K	
.entry XYZ
.extern L3
.extern W
