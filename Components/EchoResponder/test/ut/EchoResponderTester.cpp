#include "EchoResponderTester.hpp"

#include <cstring>

#include "STest/Random/Random.hpp"

namespace FPrimeApp {

const ComCfg::Apid::T EchoResponderTester::TLM_APID;

EchoResponderTester ::EchoResponderTester()
    : EchoResponderGTestBase("EchoResponderTester", EchoResponderTester::MAX_HISTORY_SIZE),
      component("EchoResponder") {
    this->initComponents();
    this->connectPorts();
    this->component.configure(TLM_APID);
}

EchoResponderTester ::~EchoResponderTester() {}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

Fw::Buffer EchoResponderTester ::sendCommand(U8 functionCode, FwSizeType size) {
    FW_ASSERT(size <= sizeof(this->m_commandStorage), static_cast<FwAssertArgType>(size));
    for (FwSizeType i = 0; i < size; i++) {
        this->m_commandStorage[i] = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));
    }
    Fw::Buffer command(this->m_commandStorage, static_cast<Fw::Buffer::SizeType>(size));
    this->invoke_to_cfsCommandIn(0, functionCode, command);
    return command;
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void EchoResponderTester ::testEcho() {
    const FwSizeType size = static_cast<FwSizeType>(STest::Random::lowerUpper(1, ECHO_RESPONDER_MAX_PAYLOAD));
    Fw::Buffer command = this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, size);

    // Exact bytes echoed with the configured APID in the context
    ASSERT_from_cfsTelemetryOut_SIZE(1);
    Fw::Buffer echo = this->fromPortHistory_cfsTelemetryOut->at(0).data;
    const ComCfg::FrameContext& context = this->fromPortHistory_cfsTelemetryOut->at(0).context;
    ASSERT_EQ(echo.getSize(), size);
    ASSERT_NE(echo.getData(), command.getData());  // copied, not aliased
    ASSERT_EQ(std::memcmp(echo.getData(), this->m_commandStorage, size), 0);
    ASSERT_EQ(context.get_apid(), TLM_APID);

    // Command buffer returned; events and counters
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), command.getData());
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_CommandReceived_SIZE(1);
    ASSERT_EVENTS_CommandReceived(0, EchoResponder::EXPECTED_FUNCTION_CODE, static_cast<U32>(size),
                                  this->m_commandStorage[0]);
    ASSERT_EVENTS_EchoSent_SIZE(1);
    ASSERT_EVENTS_EchoSent(0, static_cast<U32>(size));
    ASSERT_TLM_CommandsReceived_SIZE(1);
    ASSERT_TLM_CommandsReceived(0, 1);
    ASSERT_TLM_EchoesSent_SIZE(1);
    ASSERT_TLM_EchoesSent(0, 1);

    // Returning the echo buffer frees the component for the next command
    this->invoke_to_cfsTelemetryReturnIn(0, echo, context);
    this->clearHistory();
    this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, 1);
    ASSERT_from_cfsTelemetryOut_SIZE(1);
    ASSERT_EVENTS_EchoBusy_SIZE(0);
    ASSERT_TLM_CommandsReceived(0, 2);
}

void EchoResponderTester ::testUnexpectedFunctionCode() {
    const U8 badCode = static_cast<U8>(EchoResponder::EXPECTED_FUNCTION_CODE + 1);
    Fw::Buffer command = this->sendCommand(badCode, 4);
    ASSERT_from_cfsTelemetryOut_SIZE(0);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), command.getData());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UnexpectedFunctionCode_SIZE(1);
    ASSERT_EVENTS_UnexpectedFunctionCode(0, badCode, EchoResponder::EXPECTED_FUNCTION_CODE);
    ASSERT_TLM_UnexpectedFunctionCodes_SIZE(1);
    ASSERT_TLM_UnexpectedFunctionCodes(0, 1);
    ASSERT_TLM_CommandsReceived_SIZE(0);
}

void EchoResponderTester ::testPayloadTooLarge() {
    Fw::Buffer command = this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, ECHO_RESPONDER_MAX_PAYLOAD + 1);
    ASSERT_from_cfsTelemetryOut_SIZE(0);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), command.getData());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_PayloadTooLarge_SIZE(1);
    ASSERT_EVENTS_PayloadTooLarge(0, ECHO_RESPONDER_MAX_PAYLOAD + 1, ECHO_RESPONDER_MAX_PAYLOAD);
    ASSERT_TLM_PayloadsTooLarge_SIZE(1);
    ASSERT_TLM_PayloadsTooLarge(0, 1);
}

void EchoResponderTester ::testEchoBusy() {
    this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, 8);
    ASSERT_from_cfsTelemetryOut_SIZE(1);
    Fw::Buffer echo = this->fromPortHistory_cfsTelemetryOut->at(0).data;
    const ComCfg::FrameContext context = this->fromPortHistory_cfsTelemetryOut->at(0).context;
    this->clearHistory();

    // Echo buffer not yet returned: the second command is dropped but its buffer still returned
    Fw::Buffer command = this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, 8);
    ASSERT_from_cfsTelemetryOut_SIZE(0);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), command.getData());
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EchoBusy_SIZE(1);
    ASSERT_TLM_EchoesBusy_SIZE(1);
    ASSERT_TLM_EchoesBusy(0, 1);

    // After the return, commands are echoed again
    this->invoke_to_cfsTelemetryReturnIn(0, echo, context);
    this->clearHistory();
    this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, 8);
    ASSERT_from_cfsTelemetryOut_SIZE(1);
    ASSERT_EVENTS_EchoBusy_SIZE(0);
}

void EchoResponderTester ::testEchoEmpty() {
    this->sendCommand(EchoResponder::EXPECTED_FUNCTION_CODE, 0);
    ASSERT_from_cfsTelemetryOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_cfsTelemetryOut->at(0).data.getSize(), 0u);
    ASSERT_EVENTS_CommandReceived_SIZE(1);
    ASSERT_EVENTS_CommandReceived(0, EchoResponder::EXPECTED_FUNCTION_CODE, 0, 0);
    ASSERT_EVENTS_EchoSent(0, 0);
    ASSERT_TLM_CommandsReceived(0, 1);
    ASSERT_from_bufferReturnOut_SIZE(1);
}

}  // namespace FPrimeApp

