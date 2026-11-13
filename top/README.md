# `top/` directory - what's it for?
The top/ directory is a collection utilities, scripts, etc. that are needed to build the code locally and the CI builds.  This includes: 
- Supporting shell scripts - for `env.bat` and `env.sh` - that are used to configure the environment for a specific compiler and target platform.
- Script and configuration for [doxygen](https://www.doxygen.nl/)
- CI scripts for building and executing unit tests
- `check_clang_format.py` - checks (or fixes) that C/C++ files are formatted per the repo-root `.clang-format` file (used by the `clang-format` CI workflow)
