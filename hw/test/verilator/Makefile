CMAKE ?= cmake

CMAKE_GENERATOR = $(if $(shell command -v ninja 2> /dev/null), -G Ninja)

all:
	@echo "Generating the required CMake files for the Verilator tests, using Make."
	@cmake -S . -B build $(CMAKE_GENERATOR)
	@cmake --build build

clean:
	rm -rf build
	rm -rf bin
	rm -rf lib
	rm -rf arch