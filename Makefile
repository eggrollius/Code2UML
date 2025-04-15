CLANG_LIB=/usr/lib/llvm-10/lib

build/drawiogenerator.o: src/drawiogenerator.h
	gcc -g src/drawiogenerator.c -c -o build/drawiogenerator.o -lmxml

main: src/main.c build/drawiogenerator.o
	gcc -g src/main.c  build/drawiogenerator.o -o build/main `llvm-config --cflags --libs` -L$(CLANG_LIB) -lclang -lmxml

run: main
	./build/main 

clean:
	rm build/*