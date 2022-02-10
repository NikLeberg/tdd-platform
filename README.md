# tdd-platform

This repo is still a work in progress. In the end it should provide an abstraction layer for multiple embedded hardware platforms. With the same C code one can then run on many different platforms and boards.

Following platforms and boards are targeted (because I have these laying around):
- esp-idf / esp32
- esp8266
- stm32 / CARME-M4
- Linux / native

Another main goal is to make testing embedded C code easy. By also implementing the same abstraction layer on linux one can run tests in the native development environment. Currently only this native testing is supported. Testing on target boards is planned for the future.


## Structure

Folders:
- **build** - build files go here
- **doc** - additional documentation files (Doxygen).
- **lib** - additional external libraries, preferably as git submodule
- **platform** - the definition of the platform abstraction interface and the implementations of the different platforms
- **src** - main executable source files and its private libraries
- **test** - tests for the main executable and its private libraries


## CMake

The buildsystem is generated with CMake. It creates multiple targets to propagate dependencies or compiler settings:
- **main** - Interface library to which every other target should link to. It sets compile features to `c11` and enables many warnings and errors.
- **platform** - Library that implements the platform abstraction.
- **PROJECT_NAME** - Executable that is named after the project.


## VSCode

This project is highly integrated into VSCode. Provided are:
- **devcontainer** - Has all the toolchains for the platforms installed and is ready to go.
- **tasks** - Fast access to the cmake tasks and problem matchers.
- **c_cpp_properties** - Provides configurations for the different platforms, the currently active configuration defines for what platform will be built.


## License

[MIT](LICENSE) © [NikLeberg](https://github.com/NikLeberg).
