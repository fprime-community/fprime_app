# FPrimeApp::EchoCommander

A passive component that, every `ECHO_COMMANDER_TICKS_PER_COMMAND` (10) rate-group ticks, sends a raw cFS
command with a random opaque payload (1 to `ECHO_COMMANDER_MAX_PAYLOAD` = 32 bytes) to the native cFS
`raw_commanded` application, and checks that the raw cFS telemetry echo carries the same bytes. It
demonstrates inter-application cFS command/telemetry interoperability that is not destined for the ground.

cFS header construction, checksum and time-tagging happen in the `ComCfs` subtopology (`FPrimeCfs::CfsCmdFramer`
and `Svc::Ccsds::SpacePacketFramer` build the command; cFE stamps the sequence count and checksum on
transmission; `FPrimeCfs::CfsRouter` routes the telemetry echo by APID). This component only handles payload
bytes and sets the destination APID and function code in the frame context.

## Ports

| Name | Kind | Type | Description |
| --- | --- | --- | --- |
| `run` | sync input | `Svc.Sched` | Rate-group tick |
| `cfsCommandOut` | output | `Svc.ComDataWithContext` | Command payload; `FrameContext.apid`/`functionCode` set to the configured command APID and `ECHO_FUNCTION_CODE` (1) |
| `cfsCommandReturnIn` | sync input | `Svc.ComDataWithContext` | Returns the command buffer once the bridge has copied it |
| `cfsTelemetryIn` | sync input | `FPrimeCfs.CfsTelemetry` | Raw cFS telemetry echo from the router |
| `bufferReturnOut` | output | `Fw.BufferSend` | Returns the telemetry payload buffer to the router |

## Behavior

`configure(commandApid)` sets the APID carried in the frame context of every command. On every tenth `run`
tick:

1. If the previous command buffer has not been returned on `cfsCommandReturnIn`, the command is skipped with
   `CommandBusy`.
2. If the previous command is still unanswered, `EchoTimeout` is emitted.
3. A new random payload (xorshift32, seeded from the component time on first use) is generated, sent on
   `cfsCommandOut` with APID and function code in the context, and `CommandSent` is emitted.

On `cfsTelemetryIn`: with no command outstanding the echo is reported with `EchoUnexpected`; otherwise the
payload is compared byte-for-byte with the sent payload and `EchoMatched` or `EchoMismatch` is emitted. The
telemetry buffer is always returned on `bufferReturnOut`. Each event is paired with a telemetry channel
counting its occurrences (`CommandsSent`, `EchoesMatched`, `EchoMismatches`, `EchoesUnexpected`,
`EchoTimeouts`, `CommandsBusy`).

## Requirements

| Requirement | Description | Verification |
| --- | --- | --- |
| FPRIMEAPP-ECHOCOMMANDER-001 | EchoCommander shall send one raw cFS command with a random payload of 1 to `ECHO_COMMANDER_MAX_PAYLOAD` bytes every `ECHO_COMMANDER_TICKS_PER_COMMAND` rate-group ticks, with the configured APID and `ECHO_FUNCTION_CODE` in the frame context, emitting `CommandSent`. | Unit test, integration test |
| FPRIMEAPP-ECHOCOMMANDER-002 | EchoCommander shall emit `EchoMatched` when the telemetry echo payload equals the sent payload byte-for-byte, and `EchoMismatch` otherwise. | Unit test, integration test |
| FPRIMEAPP-ECHOCOMMANDER-003 | EchoCommander shall emit `EchoUnexpected` for telemetry received with no command outstanding. | Unit test |
| FPRIMEAPP-ECHOCOMMANDER-004 | EchoCommander shall emit `EchoTimeout` when a command is due and the previous command is still unanswered. | Unit test |
| FPRIMEAPP-ECHOCOMMANDER-005 | EchoCommander shall skip a command with the `CommandBusy` warning while its command buffer has not been returned. | Unit test |
| FPRIMEAPP-ECHOCOMMANDER-006 | EchoCommander shall return every telemetry buffer on `bufferReturnOut` and shall pair every event with an occurrence-counting telemetry channel. | Unit test |
