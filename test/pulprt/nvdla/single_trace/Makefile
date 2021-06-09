TRACE_VAR = $(shell echo $(trace) | tr a-z A-Z)

PULP_APP = single_trace
PULP_APP_SRCS = single_trace.c
PULP_CFLAGS = -O3 -g -D$(TRACE_VAR) -I../include


include $(PULPRT_HOME)/rules/pulp.mk
