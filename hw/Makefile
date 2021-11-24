.DEFAULT=all

DOCKER ?= 1
DOCKER_USER ?= $(USER)
DOCKER_GROUP ?= nvdla

ifeq (1, $(DOCKER))

DOCKER_COND = true

DOCKER_RUN = docker-compose run $(DOCKER_RUN_OPTIONS) nvdla

DOCKER_USER_ID = $(shell id -u $(DOCKER_USER))
DOCKER_GROUP_ID = $(shell getent group $(DOCKER_GROUP) | awk -F: '{printf "%d", $$3}')
.env:
	@echo "Generating .env file"
	@printf "USER_ID=$(DOCKER_USER_ID)\nGROUP_ID=$(DOCKER_GROUP_ID)\n" > .env

endif

ALL := nvdla bender verilator pulprt
CLEAN_ALL = $(addprefix clean_, $(ALL))

all: $(ALL)

init_submodules: ../.gitmodules
	git submodule init

update_submodules: init_submodules
	git submodule update

nvdla/tree.make: $(if $(DOCKER_COND), .env)
	@echo "Generating tree.make for NVDLA"
	@$(DOCKER_RUN) make -C nvdla USE_NV_ENV=1

nvdla/outdir: nvdla/tree.make $(if $(DOCKER_COND), .env)
	@echo "Building Verilog source and Verilator for NVDLA"
	@$(DOCKER_RUN) bash -c "pwd && cd nvdla && ./tools/bin/tmake -build verilator"

nvdla: nvdla/outdir

check_dependencies:
	@echo "Checking if you have the required dependencies installed"
	@./check_dependencies.sh $(if $(DOCKER_COND), -d)

test/verilator/%.f: %_setup.sh
	@echo "Setting up $(@F)"
	@./$< $(if $(DOCKER_COND), /home/nvdla/hwpe_nvdla/hw)

verilator: check_dependencies bender nvdla test/verilator/verilator.f test/verilator/verilator_nv_small.f
	@echo "Building Verilator testbenches"
	@$(DOCKER_RUN) make -C test/verilator

.bender:
	@echo "Fetching bender dependencies"
	@bender update

bender: .bender

ips:
	@echo "Updating IPs using IPApprox"
	@./update_ips.py

pulprt:
	@echo "Generating required dependencies for PULP Runtime examples"
	@$(MAKE) -C test/pulprt

clean_nvdla:
	@echo "Cleaning NVDLA"
	@rm .env
	@rm -rf nvdla/outdir
	@$(MAKE) -C nvdla clean

clean_ips:
	@echo "Cleaning IPs"
	@rm -rf dependencies

clean_bender:
	@echo "Cleaning Bender dependencies"
	@rm -rf .bender

clean_verilator: 
	@echo "Cleaning Verilator dependencies and testbenches"
	@rm -f test/verilator/verilator.f
	@rm -f test/verilator/verilator_nv_small.f
	@$(MAKE) -C test/verilator clean

clean_pulprt:
	@echo "Cleaning PULP Runtime example dependencies"
	@$(MAKE) -C test/pulprt clean

clean_all: ${CLEAN_ALL}

clean: clean_all 

help:
	@echo ""
	@echo "Call 'make <target>' with one of the targets:"
	@echo " - check_dependencies - Checks if you have all of the required dependencies installed. Called automatically."
	@echo " - init_submodules    - Initializes git submodules (NVDLA submodule)."
	@echo " - update_submodules  - Updates the git submodules."
	@echo " - nvdla              - Generates required files for NVDLA and generates NVDLA Verilog code."
	@echo " - bender             - Uses the Bender tool to update dependencies."
	@echo " - ips                - Uses the IPApprox tool to update dependencies - Not used in the 'all' rule, overwrites the bender ones and can break Verilator testbenches."
	@echo " - pulprt             - Generates required dependencies (header files) for PULP Runtime examples."
	@echo " - verilator          - Generates files required in Verilator testbenches and builds the testbenches."
	@echo " - all                - Run all of the above except for the 'ips' target."
	@echo " - clean_<target>     - Cleans the specified <target>."
	@echo " - clean              - Clean all targets."

.PHONY: all check_dependencies $(ALL) clean clean_all $(CLEAN_ALL) help
