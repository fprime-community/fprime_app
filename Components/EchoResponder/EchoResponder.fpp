module FPrimeApp {

    @ Maximum raw payload (bytes) echoed by the responder; matches the raw cFS commander app
    constant ECHO_RESPONDER_MAX_PAYLOAD = 32

    @ Receives a raw cFS command (opaque bytes) from the raw cFS commander app and
    @ echoes the payload back as a raw cFS telemetry packet. Header construction,
    @ checksum verification and time-tagging happen in the ComCfs subtopology.
    passive component EchoResponder {

        # ----------------------------------------------------------------------
        # Ports
        # ----------------------------------------------------------------------

        @ Raw cFS command (function code + payload) routed by (APID, function code)
        sync input port cfsCommandIn: FPrimeCfs.CfsCommand

        @ Returns command payload buffers to the router after processing
        output port bufferReturnOut: Fw.BufferSend

        @ Raw echo payload with frame context (APID set here); framed as cFS telemetry downstream
        output port cfsTelemetryOut: Svc.ComDataWithContext

        @ Returns the echo buffer after the bridge has copied it
        sync input port cfsTelemetryReturnIn: Svc.ComDataWithContext

        # ----------------------------------------------------------------------
        # Events (each paired with an occurrence-counting channel)
        # ----------------------------------------------------------------------

        @ A raw cFS echo command was received
        event CommandReceived(
                functionCode: U8 @< Function code from the cFS command secondary header
                $size: U32       @< Payload size in bytes
                first: U8        @< First payload byte
            ) \
            severity activity high \
            format "Echo command received: fc {} size {} first byte 0x{x}"

        @ The payload was echoed as raw cFS telemetry
        event EchoSent(
                $size: U32 @< Payload size in bytes
            ) \
            severity activity low \
            format "Echo telemetry sent: {} bytes"

        @ Command received with a function code other than the expected one
        event UnexpectedFunctionCode(
                functionCode: U8 @< The received function code
                expected: U8     @< The expected function code
            ) \
            severity warning low \
            format "Echo command with unexpected function code {} (expected {}); dropped"

        @ Command payload exceeds the echo buffer
        event PayloadTooLarge(
                $size: U32 @< Payload size in bytes
                max: U32   @< Maximum payload size
            ) \
            severity warning high \
            format "Echo command payload of {} bytes exceeds {} bytes; dropped"

        @ Command received while the previous echo buffer had not been returned
        event EchoBusy() \
            severity warning high \
            format "Echo buffer still in use; command dropped"

        # ----------------------------------------------------------------------
        # Telemetry (occurrence counters paired with the events)
        # ----------------------------------------------------------------------

        telemetry CommandsReceived: U32 update on change
        telemetry EchoesSent: U32 update on change
        telemetry UnexpectedFunctionCodes: U32 update on change
        telemetry PayloadsTooLarge: U32 update on change
        telemetry EchoesBusy: U32 update on change

        ###############################################################################
        # Standard AC Ports
        ###############################################################################
        time get port timeCaller
        event port logOut
        text event port logTextOut
        telemetry port tlmOut
    }
}
