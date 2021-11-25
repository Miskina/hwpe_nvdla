ALL = peripherals
CLEAN_ALL = $(addprefix clean_, $(ALL))

all: $(ALL)

$(ALL): %:
	$(MAKE) -C $*

clean: $(CLEAN_ALL)

$(CLEAN_ALL): clean_%:
	$(MAKE) -C $* clean

.PHONY: all $(ALL) clean $(CLEAN_ALL)