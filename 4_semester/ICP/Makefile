NAME=xskuta04-xhradi15

all: build

run:
	./src/build/chess

build:
	cd src && cmake CMakeLists.txt && make && mv chess build

doxygen:
	doxygen src/Doxyfile

pack: clean
	zip $(NAME).zip -r src examples doc Makefile

clean: clean_cmake clean_build clean_doc
	-rm $(NAME).zip

clean_build:
	-rm src/build/*

clean_cmake:
	-rm -rf src/chess_autogen
	-rm -rf src/CMakeFiles
	-rm src/cmake_install.cmake
	-rm src/CMakeCache.txt
	-rm src/Makefile

clean_doc:
	-rm -r doc/*

tmp:
	g++ src/API/main.cpp src/API/movement/ChessGame.cpp src/API/movement/Figure.cpp src/API/movement/Square.cpp src/API/parsing/Parser.cpp -o main
