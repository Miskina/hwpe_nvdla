TRACE_VAR = $(shell echo $(trace) | tr a-z A-Z)

ifeq ($(TRACE_VAR),)
$(error "You must specify the trace test which will be run, via the 'trace' variable")
endif

NVDLA_INCLUDE := $(HWPE_NVDLA_HOME)/test/pulprt/nvdla/include

PULP_APP = single_trace
PULP_APP_SRCS = single_trace.c
PULP_CFLAGS = -O3 -g -D$(TRACE_VAR) -I$(NVDLA_INCLUDE)


include $(PULPRT_HOME)/rules/pulp.mk
