#Change to your selected compiler (this compiler only works for me, putting gcc instead would work just fine)

#compiler=x86_64-w64-mingw32-gcc
compiler=gcc

main.exe: main.o text.o resolve_matrix.o num.o
	$(compiler) $? -mwindows -lpthread -pthread -o $@

main.o: main.c
	$(compiler) -c $? -o $@

text.o: text.h text.c
	$(compiler) -c text.c -o $@

num.o: num.c num.h
	$(compiler) -c num.c -o $@

resolve_matrix.o: resolve_matrix.c resolve_matrix.h
	$(compiler) -c resolve_matrix.c -o $@

cramer.o: cramer.h cramer.c
	$(compiler) -c cramer.c -o $@

gauss.o: gauss.h gauss.h
	$(compiler) -c gauss.c -o $@

jordan.o: jordan.h jordan.c
	$(compiler) -c jordan.c -o $@

clean: *.o
	rm $?

#test
text_text: text_text.c text.o
	$(compiler) $? -o $@

num_num: num.o num_num.c
	$(compiler) $? -o $@

#test