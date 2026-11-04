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

## Initialization error handling

Initialization failures are fatal to the application. If pipe creation or any software bus
subscription fails during `FPRIME_APP_Init`, an EVS error event is emitted (event IDs 2 and 3
respectively), the topology is not started, and the application exits via
`CFE_ES_ExitApp(CFE_ES_RunStatus_APP_ERROR)`. On success an informational event (event ID 1)
reports the application version.

This application is not built standalone: it is compiled as part of a cFS mission build. See
[fprime_cfs_reference](https://github.com/fprime-community/fprime_cfs_reference) for a complete
system that builds and runs this application.

## Raw cFS echo demonstration

Besides the F Prime command/telemetry traffic, the deployment subscribes to two native cFS messages for the raw
inter-application echo demonstration: command APID `0x0A0` (message id `0x18A0`, function code 1, from the
reference's `raw_commander` app) routed to `FPrimeApp::EchoResponder`, and telemetry APID `0x0A3` (message id
`0x08A3`, from the `raw_commanded` app) routed to `FPrimeApp::EchoCommander`. `EchoResponder` answers on telemetry
APID `0x0A1` and `EchoCommander` commands APID `0x0A2`, function code 1, every 10 rate-group ticks. See
`Components/EchoResponder/docs/sdd.md` and `Components/EchoCommander/docs/sdd.md`.
