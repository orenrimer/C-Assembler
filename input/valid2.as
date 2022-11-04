; All the valid commands/instruction:
; Some data at start..
X: .string "First String!"

symbol0: .data -1
symbol00: .data -1, 1, -2, 78, 90, 45, -95
symbol89: .string "H e l l o			. We like chars, so let's put some : 	"
.extern symbol1
XYZ123XYZ: .data 0	 ,  	0 	, 0  ,  0,	 	0, 	0 	, 	0, +15
.entry XYZ123XYZ

; mov 013,13
mov #0, symbol0
mov #-1, r0
mov r0, r1
mov r0, symbol0
mov symbol0, symbol1
mov symbol0, r0

; cmp 013,013
cmp #0, symbol0
cmp #-1, r0
cmp #9, #-29
cmp r0, r1
cmp r0, symbol0
cmp r0, #-98
cmp symbol0, symbol1
cmp symbol0, r0
cmp symbol0, #127


; add 013,13
add #39, symbol00
add #-1, r0
add r2, r3
add r7, symbol89
add X1234YZASFJKFDSA524bsdasfjd, symbol11
add symbol0, r6

; sub 013,13
sub #31, symbol00
sub #-1, r0
sub r2, r3
sub r7, symbol89
sub fasdiu3245dghfgshdsf78dhkj123, symbol11
sub symbol0, r6

; lea 1,13
lea symbol0, fasdiu3245dghfgshdsf78dhkj123
lea symbol11, r4

; clr 13
clr r5
clr XXXXXXXXXXXXXXXXXXXXXXXX

; not 13
not r6
not X

; inc 13
inc r7
inc X1234YZASFJKFDSA524bsdasfjd

; dec 13
dec r0
dec fasdiu3245dghfgshdsf78dhkj123

	inc symbol0
        
; This is comment!!!
	mov fasdiu3245dghfgshdsf78dhkj123.2 ,	r3	

; jmp 12
C0: jmp symbol0
jmp C0

; Put some data here:

ALPHABETAGAMA123: .string "ALPHABETAGAMA123"
.entry ALPHABETAGAMA123

; bne 12
CCC1: bne X
bne CCC1

; jsr 12
C5: jsr X
jsr C5

; get 13
get r4
get symbol00

; prn 013
prn r5
prn #-32
prn mychars

rts
rts

hlt

symbol11: .data 9
XXXXXXXXXXXXXXXXXXXXXXXX: .string " "
.entry XXXXXXXXXXXXXXXXXXXXXXXX
X1234YZASFJKFDSA524bsdasfjd: .data 5
.entry X1234YZASFJKFDSA524bsdasfjd
symbol01: .data -000000, +000000, +000001, -000004
mychars: .string "mychars!@#$%^&#*() 	\/+-=_"

.extern fasdiu3245dghfgshdsf78dhkj123