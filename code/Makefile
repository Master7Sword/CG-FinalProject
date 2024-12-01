all: build
	cd build && make -j4

build:
	cmake -S . -B build

run: all
	./build/Skybox_Project

clean:
	rm -rf build/