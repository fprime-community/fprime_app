# FPrimeDeployment

An F Prime deployment that runs as a cFS application. The deployment's topology is set up and torn
down by the cFS application entry point in `Main.cpp` (`FPRIME_APP_Main`), which is registered with
cFE Executive Services rather than run as a standalone binary.

The deployment:

1. Configures the `FPrimeCfs::CfsBridge` with a software bus pipe and subscribes to F Prime command
   packets and cFS scheduler (SCH) tick messages.
2. Sets up the F Prime topology (`Top/`), whose rate groups are driven by SCH ticks routed through
   the `FPrimeCfs.SchAppDriver`.
3. Drives the bridge from the cFS run loop via `cfsBridge.process()`.

This application is not built standalone: it is compiled as part of a cFS mission build. See
[fprime_cfs_reference](https://github.com/fprime-community/fprime_cfs_reference) for a complete
system that builds and runs this application.
