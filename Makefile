all: submodules hw

submodules:
	git submodule update --init --recursive

hw:
	$(MAKE) -C hw nvdla
clean: clean_hw clean_submodules

clean_hw:
	$(MAKE) -C hw clean

clean_submodules:
	rm -rf hw/nvdla

help:
	@echo ""
	@echo "Call 'make <target>' with one of the targets:"
	@echo " - hw             - Build hardware source files."
	@echo " - submodules     - Initialize and update all submodules recursively."
	@echo " - all            - Run all of the above."
	@echo " - clean_<target> - Cleans the specified <target>."
	@echo " - clean          - Clean all targets."

.PHONY: all hw clean clean_hw help