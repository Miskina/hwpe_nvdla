.DEFAULT_GOAL := default
NVDLA_HW := ../../../nvdla_hw

opendla: $(NVDLA_HW) $(NVDLA_HW)/outdir/nv_small/spec/manual/opendla.h
	cp $(NVDLA_HW)/outdir/nv_small/spec/manual/opendla.h include

generate_traces: $(NVDLA_HW)
	./trace_test_to_c.py -d $(NVDLA_HW)/verif/tests/trace_tests/nv_small -a include

clean_traces:
	rm -f include/nvdla_trace_tests.h

clean_opendla:
	rm -f include/opendla.h

all: opendla generate_traces

clean: clean_opendla clean_traces

default: all
