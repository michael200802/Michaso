#Change to your selected compiler (this compiler only works for me, putting gcc instead would work just fine)

compiler=x86_64-w64-mingw32-gcc
#compiler=gcc

all: main.exe

main.exe: main.o text.o num.o show_process_in_edit.o
	$(compiler) $? -mwindows -lpthread -pthread -o $@

main.o: main.c
	$(compiler) -c $? -o $@

text.o: text.h text.c
	$(compiler) -c text.c -o $@

num.o: num.c num.h
	$(compiler) -c num.c -o $@

show_process_in_edit.o: show_process_in_edit.h show_process_in_edit.c
	$(compiler) -c show_process_in_edit.c -o $@

clean: *.o
	rm $?

#test
text_text: text_text.c text.o
	$(compiler) $? -o $@

num_num: num.o num_num.c
	$(compiler) $? -o $@

#test