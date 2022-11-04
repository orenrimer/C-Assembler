;contains pre-assembler errors

K:		.data 22
MAIN:	mov S1.1, W

XYZ: 	jmp W

		macro %m&&!!
; This is invalid macro, don't copy it!!!
		endmacro

		prn 	#-50
		sub r1	, r4
		
		bne L3
END:	hlt
STR:	.string "abcdef"

macro m1
		inc K
; This is a valid macro
		mov S1.2 ,	r3	
		endmacro
   
LENGTH:	.data 6,-512,511
m1
S1:		.struct 8, "ab"
.entry K	
.entry XYZ
.extern L3
.extern W

; invalid macro call has to be on a separate line 
m1 blablabla  

macro m1    
    ; invalid macro name, macro is already defined
	inc K
endmacro

