# Verilator testbench folder
This folder contains the Verilator testbenches. The testbenches are not meant to be build by themselves,
but all at once by calling `make`. All of the testbench executables are stored in the **bin** folder and
can be run directly or, specifically for the testbenches using NVDLA and NVDLA trace tests, they can be 
run using the `run_verilator_tests.py` script. The script is located in the **trace_test** folder and 
it's options are listed by running:

```
./run_verilator_tests.py --help
```

# Testbenches
The Verilator testbenches all follow a naming policy so the build process is more easily automated.
Each folder is prefixed by *tb_* and contains a `CMakeLists.txt` file. The *CMake* file usually uses
variables specified in the `hwpe_nvdla.cmake` which should contain the abosulte paths
to the required files and make the *CMakeLists* much easier to write. You can look through any
of the *CMakeLists.txt* files in the already provided testbench folders to see an example.