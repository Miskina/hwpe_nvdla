.DEFAULT_GOAL := default

ALL := check_dependencies init_submodules update_submodules nvdla bender verilator pulp_rt
CLEAN_ALL = $(addprefix clean_,nvdla bender verilator pulp_rt)

DOCKER ?= 1

check_dependencies:
	@echo "Checking if you have the required dependencies installed"
	./check_dependencies.sh

.PHONY: check_dependencies

init_submodules: .gitmodules
	git submodule init

update_submodules: init_submodules
	git submodule update

ifeq (1, $(DOCKER))
nvdla: update_submodules
	@echo "Generating tree.make for NVDLA"
	@echo "Building Verilog source for NVDLA"
	docker-compose up nvdla-build
else
nvdla: update_submodules
	@echo "Generating tree.make for NVDLA"
	make -C ./nvdla_hw USE_NV_ENV=1
	@echo "Building Verilog sources for NVDLA"
	cd nvdla_hw; ./tools/bin/tmake -build vmod
endif

bender: check_dependencies
	@echo "Fetching bender dependencies"
	bender update

ips:
	@echo "Updating IPs using IPApprox"
	./update_ips.py

verilator: check_dependencies bender nvdla
	@echo "Setting up Verilator requirements"
	./verilator_setup.sh
	@echo "Building Verilator testbenches"
	make -C test/verilator

pulp_rt:
	@echo "Generating required dependencies for PULP Runtime examples"
	make -C test/pulprt/nvdla

clean_nvdla:
	@echo "Cleaning NVDLA"
	rm -rf nvdla_hw/outdir
	make -C nvdla_hw clean

clean_ips:
	@echo "Cleaning up IPs"
	rm -rf dependencies

clean_bender:
	@echo "Cleaning Bender dependencies"
	rm -rf .bender
	rm -rf dependencies

clean_verilator: 
	@echo "Cleaning Verilator dependencies and testbenches"
	rm -f test/verilator/verilator.f
	make -C test/verilator clean

clean_pulp_rt:
	@echo "Cleaning PULP Runtime example dependencies"
	make -C test/pulprt/nvdla clean

clean_all: ${CLEAN_ALL}

clean: clean_all 

all: $(ALL)

default: all

help:
	@echo ""
	@echo "Call 'make <target>' with one of the targets:"
	@echo " - check_dependencies - Checks if you have all of the required dependencies installed. Called automatically."
	@echo " - init_submodules    - Initializes git submodules (NVDLA submodule)."
	@echo " - update_submodules  - Updates the git submodules."
	@echo " - nvdla              - Generates required files for NVDLA and generates NVDLA Verilog code."
	@echo " - bender             - Uses the Bender tool to update dependencies."
	@echo " - ips                - Uses the IPApprox tool to update dependencies - Not used in the 'all' rule, overwrites the bender ones and can break Verilator testbenches."
	@echo " - pulp_rt            - Generates required dependencies (header files) for PULP Runtime examples."
	@echo " - verilator          - Generates files required in Verilator testbenches and builds the testbenches."
	@echo " - all                - Run all of the above except for the 'ips' target."
	@echo " - clean_<target>     - Cleans the specified <target>."
	@echo " - clean              - Clean all targets."
