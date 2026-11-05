# FPrimeApp::EchoResponder

A passive component that receives a raw cFS command (opaque bytes, not an F Prime serialized structure) from
the native cFS `raw_commander` application and echoes the exact payload back as a raw cFS telemetry packet.
It demonstrates inter-application cFS command/telemetry interoperability that is not destined for the ground.

cFS header construction, command checksum verification and telemetry time-tagging happen in the `ComCfs`
subtopology (`FPrimeCfs::CfsRouter` routes the command by (APID, function code) after checking the checksum;
`FPrimeCfs::CfsTlmFramer` and `Svc::Ccsds::SpacePacketFramer` build the telemetry headers). This component
only handles payload bytes.

## Ports

| Name | Kind | Type | Description |
| --- | --- | --- | --- |
| `cfsCommandIn` | sync input | `FPrimeCfs.CfsCommand` | Raw cFS command (function code + payload) from the router |
| `bufferReturnOut` | output | `Fw.BufferSend` | Returns the command payload buffer to the router |
| `cfsTelemetryOut` | output | `Svc.ComDataWithContext` | Echo payload; `FrameContext.apid` set to the configured telemetry APID |
| `cfsTelemetryReturnIn` | sync input | `Svc.ComDataWithContext` | Returns the echo buffer once the bridge has copied it |

## Behavior

`configure(telemetryApid)` sets the APID carried in the frame context of every echo (the raw commander
subscribes to this telemetry message id). On `cfsCommandIn`:

1. A function code other than `EXPECTED_FUNCTION_CODE` (1) is rejected with `UnexpectedFunctionCode`.
2. A payload larger than `ECHO_RESPONDER_MAX_PAYLOAD` (32 bytes) is rejected with `PayloadTooLarge`.
3. If the previous echo buffer has not yet been returned on `cfsTelemetryReturnIn`, the command is dropped
   with `EchoBusy`.
4. Otherwise the payload is copied into the component's single echo buffer, `CommandReceived` is emitted,
   the buffer is sent on `cfsTelemetryOut` with the configured APID in the context, and `EchoSent` is emitted.

In every case the command buffer is returned on `bufferReturnOut`. Each event is paired with a telemetry
channel counting its occurrences (`CommandsReceived`, `EchoesSent`, `UnexpectedFunctionCodes`,
`PayloadsTooLarge`, `EchoesBusy`).

## Requirements

| Requirement | Description | Verification |
| --- | --- | --- |
| FPRIMEAPP-ECHORESPONDER-001 | EchoResponder shall echo the exact payload bytes of each valid raw cFS command on `cfsTelemetryOut` with the configured telemetry APID in the frame context, emitting `CommandReceived` and `EchoSent`. | Unit test, integration test |
| FPRIMEAPP-ECHORESPONDER-002 | EchoResponder shall reject commands whose function code is not the expected one with the `UnexpectedFunctionCode` warning. | Unit test |
| FPRIMEAPP-ECHORESPONDER-003 | EchoResponder shall reject payloads larger than `ECHO_RESPONDER_MAX_PAYLOAD` with the `PayloadTooLarge` warning. | Unit test |
| FPRIMEAPP-ECHORESPONDER-004 | EchoResponder shall drop a command with the `EchoBusy` warning while its echo buffer has not been returned. | Unit test |
| FPRIMEAPP-ECHORESPONDER-005 | EchoResponder shall return every command buffer on `bufferReturnOut` and shall pair every event with an occurrence-counting telemetry channel. | Unit test |
