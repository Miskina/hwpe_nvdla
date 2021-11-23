all: hw

hw:
	$(MAKE) -C hw nvdla
clean: clean_hw

clean_hw:
	$(MAKE) -C hw clean

help:
	@echo ""
	@echo "Call 'make <target>' with one of the targets:"
	@echo " - hw             - Build hardware source files."
	@echo " - all            - Run all of the above."
	@echo " - clean_<target> - Cleans the specified <target>."
	@echo " - clean          - Clean all targets."

.PHONY: all hw clean clean_hw help