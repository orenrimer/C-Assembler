# C-Assembler
A 10-bit assembler based on the two-pass assemblermodel. Designed for a custom assembly language, supporting16 custom instructions, encoding each instruction into custom base32:


|   |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
|---|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
| ! | @ | # | $ | % | ^ | & | * | < | > | a  | b  | c  | d  | e  | f  | g  | h  | i  | j  | k  | l  | m  | n  | o  | p  | q  | r  | s  | t  | u  | v  |




### How To Run

Use makefile to compile the project like this:
```
>   make
```
you will have to provide a path to an .as file written in the custom assemly, then run the file with:
```
>   ./run filename.as 
```


## Instruction Structure
Every software *instruction* is encoded into a up to 5 *words* in memory. The first word always has the following structure:

|  9&emsp;8&emsp;7&emsp;6  |  5&emsp;4  |  3&emsp;2  |  1&emsp;0  |
| ------------------------ | ---------- | ---------- | ---------- |
|          Opcode          | Source operand addressing type | Destination operand addressing type | E, R, A



## Instructions
This assembler supports 16 cutom instructions, each instruction op will be stored in bits 6-9 of the first mechine word:

| Opcode (decimal) | Command Name |
| ---------------- | ------------ |
|	0	|	mov	|
|	1	|	cmp	|
|	2	|	add	|
|	3	|	sub	|
|	4	|	not	|
|	5	|	clr	|
|	6	|	lea	|
|	7	|	inc	|
|	8	|	dec	|
|	9	|	jmp	|
|	10	|	bne	|
|	11	|	get	|
|	12	|	prn	|
|	13	|	jsr	|
|	14	|	rts	|
|	15	|	hlt	|



## Directives
A **_directive_** line of the following structure:

1. An **optional** preceding *label*. e.g. `PLACE1: `.
2. A _directive_: `.data`, `.string`, `.struct`, `.entry` or `.extern`.
3. Operands according to the type of the *directive*.

   ### `.data`
   This directive allocates memory in the data image for storing received integers later in memory (also increases the _data counter_ and updates the _symbol table_).
   The parameters of `.data` are valid integers (separated by a comma).  
   e.g. `LABEL1: .data      +7, -56, 4, 9`.
   
   ### `.string`
   This direcive receives a string as an operand and stores it in the data image. It stores all characters by their order in the string, encoded ny their *ASCII* values.
   e.g. `STRING1: .string "abcdef"` is a valid directive.
   
   ### `.struct`
   This directive allocates memory for a *structure* consists of 2 fields: the first is an integer and the second is a string.
   e.g. `STRUCT4: .struct 4, "abc"` is a vaild directive.
   
   ### `.entry`
   This directive outputs a received name of a *label* to the *symbol table*, so that later it will be recognized by other assembly files (and they would be able to use it).
   e.g. 
   ```
   ; file1.as
   .entry HELLO
   HELLO: add #1, r1 
   ```
   ### `.extern`
   This directive receives a name of a *label* as a parameter and declares the *label* as being external (defined in another file) and that the current file shall use it.  
   This way, the directive `.extern HELLO` in `file2.as` will match the `.entry` directive in the previous example.
