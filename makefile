CC = gcc
HEADER_DIR = headers
OBJS = main.o pre_assembler.o first_pass.o second_pass.o common.o symbol_list.o output_manager.o utils.o
COMP_FLAG = -ansi -Wall -pedantic 
EXEC = run


$(EXEC) : $(OBJS) $(HEADER_DIR)/globals.h
	$(CC) -g $(COMP_FLAG) $(OBJS) -o $@  

main.o: main.c
	$(CC) -c $(COMP_FLAG) $*.c

pre_assembler.o: pre_assembler.c $(HEADER_DIR)/pre_assembler.h
	$(CC) -c $(COMP_FLAG) $*.c

first_pass.o: first_pass.c $(HEADER_DIR)/first_pass.h
	$(CC) -c $(COMP_FLAG) $*.c

second_pass.o: second_pass.c $(HEADER_DIR)/second_pass.h
	$(CC) -c $(COMP_FLAG) $*.c

common.o: common.c $(HEADER_DIR)/common.h
	$(CC) -c $(COMP_FLAG) $*.c

symbol_list.o: symbol_list.c $(HEADER_DIR)/symbol_list.h
	$(CC) -c $(COMP_FLAG) $*.c

output_manager.o: output_manager.c $(HEADER_DIR)/output_manager.h
	$(CC) -c $(COMP_FLAG) $*.c

utils.o: utils.c $(HEADER_DIR)/utils.h
	$(CC) -c $(COMP_FLAG) $*.c
