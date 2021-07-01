.DEFAULT_GOAL := default

ALL := check_dependencies init_submodules update_submodules nvdla bender verilator pulp_rt
CLEAN_ALL = $(addprefix clean_,nvdla bender verilator pulp_rt)

check_dependencies:
	@echo "Checking if you have the required dependencies installed"
	./check_dependencies.sh

.PHONY: check_dependencies

init_submodules: .gitmodules
	git submodule init

update_submodules: init_submodules
	git submodule update

nvdla: update_submodules
	@echo "Generating tree.make for NVDLA"
	make -C ./nvdla_hw USE_NV_ENV=1
	@echo "Building Verilog sources for NVDLA"
	cd nvdla_hw; ./tools/bin/tmake -build vmod

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

clean: $(CLEAN_ALL)

all: $(ALL)

default: all
