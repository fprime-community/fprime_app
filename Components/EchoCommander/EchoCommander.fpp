module FPrimeApp {

    @ Maximum raw payload (bytes) sent by the commander; matches the raw cFS commanded app
    constant ECHO_COMMANDER_MAX_PAYLOAD = 32

    @ Rate-group ticks between echo commands
    constant ECHO_COMMANDER_TICKS_PER_COMMAND = 10

    @ Every ECHO_COMMANDER_TICKS_PER_COMMAND rate-group ticks, sends a raw cFS command
    @ with a random opaque payload to the raw cFS commanded app and checks that the
    @ raw cFS telemetry echo carries the same bytes. Header construction, checksum and
    @ time-tagging happen in the ComCfs subtopology.
    passive component EchoCommander {

        # ----------------------------------------------------------------------
        # Ports
        # ----------------------------------------------------------------------

        @ Rate-group tick
        sync input port run: Svc.Sched

        @ Raw command payload with frame context (APID, function code set here); framed as a cFS command downstream
        output port cfsCommandOut: Svc.ComDataWithContext

        @ Returns the command buffer after the bridge has copied it
        sync input port cfsCommandReturnIn: Svc.ComDataWithContext

        @ Raw cFS telemetry echo routed by APID
        sync input port cfsTelemetryIn: FPrimeCfs.CfsTelemetry

        @ Returns telemetry payload buffers to the router after processing
        output port bufferReturnOut: Fw.BufferSend

        # ----------------------------------------------------------------------
        # Events (each paired with an occurrence-counting channel)
        # ----------------------------------------------------------------------

        @ A raw cFS echo command was sent
        event CommandSent(
                sequence: U32 @< Command sequence number
                $size: U32    @< Payload size in bytes
                first: U8     @< First payload byte
            ) \
            severity activity low \
            format "Echo command {} sent: size {} first byte 0x{x}"

        @ The raw cFS telemetry echo matched the command payload
        event EchoMatched(
                sequence: U32 @< Command sequence number
                $size: U32    @< Payload size in bytes
            ) \
            severity activity high \
            format "Echo {} matched: {} bytes"

        @ The raw cFS telemetry echo did not match the command payload
        event EchoMismatch(
                sequence: U32 @< Command sequence number
                sentSize: U32 @< Sent payload size in bytes
                receivedSize: U32 @< Received payload size in bytes
            ) \
            severity warning high \
            format "Echo {} MISMATCH: sent {} bytes, received {} bytes"

        @ Telemetry echo received while no command was outstanding
        event EchoUnexpected(
                $size: U32 @< Received payload size in bytes
            ) \
            severity warning low \
            format "Unexpected echo of {} bytes with no command outstanding"

        @ No echo was received before the next command was due
        event EchoTimeout(
                sequence: U32 @< Sequence number of the unanswered command
            ) \
            severity warning high \
            format "Echo {} not received before the next command"

        @ Command due while the previous command buffer had not been returned
        event CommandBusy() \
            severity warning high \
            format "Command buffer still in use; command skipped"

        # ----------------------------------------------------------------------
        # Telemetry (occurrence counters paired with the events)
        # ----------------------------------------------------------------------

        telemetry CommandsSent: U32 update on change
        telemetry EchoesMatched: U32 update on change
        telemetry EchoMismatches: U32 update on change
        telemetry EchoesUnexpected: U32 update on change
        telemetry EchoTimeouts: U32 update on change
        telemetry CommandsBusy: U32 update on change

        ###############################################################################
        # Standard AC Ports
        ###############################################################################
        time get port timeCaller
        event port logOut
        text event port logTextOut
        telemetry port tlmOut
    }
}
