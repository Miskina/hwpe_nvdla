CMAKE ?= cmake

HWPE_NVDLA_ROOT ?= $(realpath ../..)
RENODE_ROOT ?= $(realpath ../renode)
RENODE_VERILATOR_INTEGRATION_ROOT ?= $(HWPE_NVDLA_ROOT)/renode/renode-verilator-integration

PERIPHERALS = nvdla
PERIPHERALS_CLEAN = $(addprefix clean_, $(PERIPHERALS))

CMAKE_GENERATOR = $(if $(shell command -v ninja 2> /dev/null), -G Ninja)

.PHONY: all $(PERIPHERALS) clean $(PERIPHERALS_CLEAN)

all: $(PERIPHERALS)

$(PERIPHERALS): %:
	@echo Generating build files in $*/build
	@HWPE_NVDLA_ROOT=$(HWPE_NVDLA_ROOT) RENODE_ROOT=$(RENODE_ROOT)          \
	 RENODE_VERILATOR_INTEGRATION_ROOT=$(RENODE_VERILATOR_INTEGRATION_ROOT) \
	 $(CMAKE) -S $* -B $*/build $(CMAKE_GENERATOR)
	@echo Building peripheral $*
	@$(CMAKE) --build $*/build

clean: $(PERIPHERALS_CLEAN)

$(PERIPHERALS_CLEAN): clean_%:
	@echo $*
	rm -rf $*/build