NVDLA_HW := ../../nvdla

.PHONY: all generate_traces

all: include/opendla.h generate_traces

include/opendla.h:
	cp $(NVDLA_HW)/outdir/nv_small/spec/manual/opendla.h include

generate_traces:
	./trace_test_to_c.py -d $(NVDLA_HW)/verif/tests/trace_tests/nv_small -a include

.PHONY: clean clean_opendla clean_traces

clean: clean_opendla clean_traces

clean_traces:
	rm -f include/nvdla_trace_tests.h

clean_opendla:
	rm -f include/opendla.h