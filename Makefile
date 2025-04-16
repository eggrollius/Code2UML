CLANG_LIB=/usr/lib/llvm-10/lib

build/umlclass.o: src/umlclass.h src/umlclass.c
	gcc -g src/umlclass.c -c -o build/umlclass.o

build/drawiogenerator.o: src/drawiogenerator.h src/drawiogenerator.c
	gcc -g src/drawiogenerator.c -c -o build/drawiogenerator.o

build/main: src/main.c build/drawiogenerator.o build/umlclass.o
	gcc -g src/main.c  build/drawiogenerator.o build/umlclass.o -o build/main `llvm-config --cflags --libs` -L"$(CLANG_LIB)" -lclang -lmxml

run: build/main
	./build/main 

clean:
	rm build/*