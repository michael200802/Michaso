compiler=x86_64-w64-mingw32-gcc

main.exe: main.o text.o resolve_matrix.o num.o
	$(compiler) $? -mwindows -lpthread -pthread -o $@

main.o: main.c
	$(compiler) -c $? -o $@

text.o: text.c text.h
	$(compiler) -c text.c -o $@

resolve_matrix.o: resolve_matrix.c resolve_matrix.h
	$(compiler) -c resolve_matrix.c -o $@

num.o: num.c num.h
	$(compiler) -c num.c -o $@

clean: *.o
	rm $?
