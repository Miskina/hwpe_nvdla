all:
	cmake -B build; make -C build
clean:
	rm -rf build
	rm -rf bin
	rm -rf lib
	rm -rf arch
# PERIPH_TO_CSB currently not working using TB_PERIPH_TO_CSB
# VERILATOR_TESTBENCHES := tb_periph_to_csb tb_periph_to_nvdla tb_hwpe_nvdla
# 
# CPP_TESTS := cpp_tests
# 
# ALL = $(VERILATOR_TESTBENCHES) $(CPP_TESTS)
# 
# BUILD_TARGETS = $(ALL:%=build-%)
# CLEAN_TARGETS = $(ALL:%=clean-%)
# RUN_TARGETS = $(ALL:%=run-%)
# 
# $(BUILD_TARGETS):
# 	cmake $(@:build-%=%) -B $(@:build-%=%)/build; make -C $(@:build-%=%)/build
# 
# # $(RUN_TARGETS): $(BUILD_TARGETS)
# 
# $(CLEAN_TARGETS):
# 	rm -rf $(@:clean-%=%)/build
# 	
# all: $(BUILD_TARGETS)
# clean: $(CLEAN_TARGETS)
# 
# default: all

# tb_hwpe_nvdla: tb_hwpe_nvdla verilator.f
# 	@echo "Building 'tb_hwpe_nvdla'"
# 	cd tb_hwpe_nvdla & cmake -B build & make -C build
# 
# tb_periph_to_csb: tb_periph_to_csb verilator.f
# 	@echo "Building 'tb_periph_to_csb'"
# 	cd tb_periph_to_csb & cmake -B build & make -C build
# 
# tb_periph_to_nvdla: tb_periph_to_nvdla verilator.f
