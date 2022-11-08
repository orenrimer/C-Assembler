# C-Assembler
A 10-bit assembler based on the two-pass assemblermodel. Designed for a custom assembly language, supporting16 custom instructions, encoding each instruction into custom base32:


|   |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
|---|---|---|---|---|---|---|---|---|---|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
| ! | @ | # | $ | % | ^ | & | * | < | > | a  | b  | c  | d  | e  | f  | g  | h  | i  | j  | k  | l  | m  | n  | o  | p  | q  | r  | s  | t  | u  | v  |




### How To RUn

Use makefile to compile the project like this:
```
>   make
```
then run the file


## Instruction Structure
Every software *instruction* is encoded into a few *words* in memory (max is 5 *words*).
The first word is of the following structure:

|  9&emsp;8&emsp;7&emsp;6  |  5&emsp;4  |  3&emsp;2  |  1&emsp;0  |
| ------------------------ | ---------- | ---------- | ---------- |
|          Opcode          | Source operand addressing type | Destination operand addressing type | E, R, A
