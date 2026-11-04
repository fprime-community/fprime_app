# F Prime CFS Reference App

This is a reference cFS app constructed using the F Prime framework.

See [fprime_cfs](https://github.com/fprime-community/fprime_cfs) for more information on how to build cFS applications using the F Prime framework.

## Components

| Component | Purpose | SDD |
| --- | --- | --- |
| `FPrimeApp::EchoResponder` | Echoes raw cFS command payloads from the native `raw_commander` app back as raw cFS telemetry | [SDD](./Components/EchoResponder/docs/sdd.md) |
| `FPrimeApp::EchoCommander` | Sends raw cFS commands to the native `raw_commanded` app every 10 rate-group ticks and checks the telemetry echo | [SDD](./Components/EchoCommander/docs/sdd.md) |
