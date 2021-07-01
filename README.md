# HWPE NVDLA
Repository of NVDLA adapted to PULP's HWPE interfaces.
The system specifications tested on:
- Ubuntu 20.04
- GCC 9.4.0
- G++/CPP 11.1.1
- clang 10.0.0
- Verilator 4.201
- Perl 5
- Python 3.8.10
- Java 11.0.11
- GNU Make 4.2.1
- CMake 3.16.3 (All scripts adapted for 3.8)

## Working with the repository
Use the Makefile in the root repository to build all of the required dependencies. Running the `make help` command will show all
of the available options. However, running just `make` will generate all of the required files, dependencies and build all of the 
Verilator dependencies, prepare the PULP Runtime examples (QuestaSim tests), etc. 

## Handling dependencies
Besides the `nvdla_hw` dependency which is resolved using a git submodule, there are other
component dependencies. Since this project integrates with PULP we used their dependency management tools. The preferred, newer dependency tool is **Bender**, but since it has some
issues with **FPGA**, we also support **IPApprox**. The **IPApprox** version has not yet been
tested appropriately and is not recommended. The make rules currently use Bender and **IPApprox** is only run if the `make ips` target is called.

Bender can be installed by following the instructions at it's [github repository](https://github.com/pulp-platform/bender#installation).

## Components of the repository
The repository consists a couple of subfolders each serving a different purpose. The
folders and subfolders will be described further in the following chapters.

The main folder consists of mostly helper scripts which are used within the Makefile or are used by the dependency management tools. It also has *docker* container bindings, but the
container was meant for usage with the NVDLA repository and is not currently pulling all
of the required git modules and building them, only the dependencies.

### nvdla_hw
The `nvdla_hw` folder is a *git submodule* folder of the NVDLA HW (Hardware) repository modified for the needs of this project. It contains a seperate Makefile which is used
to generate paths to certain required tools (such as GCC, Verilator, etc.). The default
paths are automatically deduced if the tools are installed, however the file can be manually
changed if different tools are required. The folder contains it's own README, from the
NVDLA repository.

### dependencies
The `dependencies` folder consists of modules pulled from other git repositories, which are
linked using PULP Platform's dependency management tools. More about the tools can be found in the [Handling dependencies](#handling-dependencies) section.

### intf
Interfaces used in the HWPE NVDLA implementation.

### src
The SystemVerilog implementation of the HWPE NVDLA module which is later linked with a
PULP repository (Specifically PULPissimo).

### test
Conatins different testing methods and scripts which help or are required for the tests.

#### test/src
SystemVerilog testbenches which can be used in Verilator simulations or other kinds of
testing.

#### test/formal
Formal tests, currently WIP.

#### test/pulprt and test/questa
Bindings and tests using PULP Runtime, and a script which runs a list of tests with the `single_trace` program in QuestaSim. 

#### test/verilator
Contains everything required for the Verilator testbenches. C++ wrappers and utility classes.
Also contains the C++ (Verilator) testbenches which should be prefixed with follow the naming
`tb_<name_of_component>`. All of the Verilator testbenches are built using the directories `CMakeLists.txt` file. The binaries are saved in the `bin` folder. There is also a very simple
`Makefile` which simplifies the use of CMake and is acutally called in the root `Makefile`.

#### test/verilator/trace_test
Contains the script which can run any of the testbenches which process trace tests,
saves their logs, caches the run, etc. The testbenches currently using trace tests are
`tb_periph_to_nvdla` and `tb_hwpe_nvdla`.
