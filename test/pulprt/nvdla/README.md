# PULP Runtime examples
This folder C programs which use PULP Runtime to be run with PULPissimo in QuestaSim.
The tests can be run as described in PULP's [runtime repository](https://github.com/pulp-platform/pulp-runtime).
The only different one is the `single_trace` program which can run trace tests.

## Single Trace
The `single_trace` program, besides the default PULP Runtime `make` options also has an additional required parameter.
The parameter is the name of the trace test to be run. An example of running the basic `cdp_1x1x1_lrn3_int8_0` test is the following:

```
make all run trace=cdp_1x1x1_lrn3_int8_0
```

It can also be run with QuestaSim GUI like so:

```
make all run trace_cdp_1x1x1_lrn3_int8_0 gui=1
```

## Run all script
The [questa](../../questa) folder contains the [Python script](../../questa/run_questa_tests.py) which
can run a list of trace tests specified, output the logs and similar utilities. The particular
options can be seen by running the script with the `--help` parameter.
