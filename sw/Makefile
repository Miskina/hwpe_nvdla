
USE_NINJA := $(shell command -v ninja 2> /dev/null)

CMAKE ?= cmake

# ALL := test firmware
# CLEAN_ALL := $(addprefix clean_,$(ALL))
#
#
# test:
# 	$(MAKE) -C test
#
# firmware:
# 	$(MAKE) -C firmware
#
#
# all: $(ALL)
# default: all
#
# clean_all:  $(CLEAN_ALL)
# clean: clean_all
#
# .PHONY: test firmware

all: generate
	$(CMAKE) --build build

generate:
	$(CMAKE) -B build -S . $(if $(USE_NINJA),-G Ninja)

clean: build
	rm -rf build
