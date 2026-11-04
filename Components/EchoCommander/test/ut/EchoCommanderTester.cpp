#include "EchoCommanderTester.hpp"

#include <cstring>

namespace FPrimeApp {

const ComCfg::Apid::T EchoCommanderTester::CMD_APID;

EchoCommanderTester ::EchoCommanderTester()
    : EchoCommanderGTestBase("EchoCommanderTester", EchoCommanderTester::MAX_HISTORY_SIZE),
      component("EchoCommander") {
    this->initComponents();
    this->connectPorts();
    this->component.configure(CMD_APID);
}

EchoCommanderTester ::~EchoCommanderTester() {}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

Fw::Buffer EchoCommanderTester ::tickUntilCommand() {
    const U32 before = this->fromPortHistory_cfsCommandOut->size();
    for (U32 i = 0; i < ECHO_COMMANDER_TICKS_PER_COMMAND; i++) {
        this->invoke_to_run(0, 0);
    }
    EXPECT_EQ(this->fromPortHistory_cfsCommandOut->size(), before + 1);
    return this->fromPortHistory_cfsCommandOut->at(before).data;
}

void EchoCommanderTester ::deliverEcho(const Fw::Buffer& command, bool corrupt) {
    const FwSizeType size = command.getSize();
    FW_ASSERT(size <= sizeof(this->m_telemetryStorage), static_cast<FwAssertArgType>(size));
    (void)std::memcpy(this->m_telemetryStorage, command.getData(), size);
    if (corrupt) {
        this->m_telemetryStorage[size - 1] = static_cast<U8>(~this->m_telemetryStorage[size - 1]);
    }
    Fw::Buffer telemetry(this->m_telemetryStorage, static_cast<Fw::Buffer::SizeType>(size));
    this->invoke_to_cfsTelemetryIn(0, FPrimeCfs::CfsTime(0, 0), telemetry);
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void EchoCommanderTester ::testPeriodicCommand() {
    // No command before the tenth tick
    for (U32 i = 0; i < ECHO_COMMANDER_TICKS_PER_COMMAND - 1; i++) {
        this->invoke_to_run(0, 0);
    }
    ASSERT_from_cfsCommandOut_SIZE(0);
    ASSERT_EVENTS_SIZE(0);

    this->invoke_to_run(0, 0);
    ASSERT_from_cfsCommandOut_SIZE(1);
    const Fw::Buffer& command = this->fromPortHistory_cfsCommandOut->at(0).data;
    const ComCfg::FrameContext& context = this->fromPortHistory_cfsCommandOut->at(0).context;
    ASSERT_GE(command.getSize(), 1u);
    ASSERT_LE(command.getSize(), static_cast<Fw::Buffer::SizeType>(ECHO_COMMANDER_MAX_PAYLOAD));
    ASSERT_EQ(context.get_apid(), CMD_APID);
    ASSERT_EQ(context.get_functionCode(), EchoCommander::ECHO_FUNCTION_CODE);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CommandSent_SIZE(1);
    ASSERT_EVENTS_CommandSent(0, 1, static_cast<U32>(command.getSize()), command.getData()[0]);
    ASSERT_TLM_CommandsSent_SIZE(1);
    ASSERT_TLM_CommandsSent(0, 1);

    // Payloads differ between commands (random), and the sequence advances
    this->invoke_to_cfsCommandReturnIn(0, const_cast<Fw::Buffer&>(command), context);
    this->deliverEcho(command, false);
    this->clearHistory();
    Fw::Buffer second = this->tickUntilCommand();
    ASSERT_EVENTS_CommandSent(0, 2, static_cast<U32>(second.getSize()), second.getData()[0]);
    ASSERT_TLM_CommandsSent(0, 2);
}

void EchoCommanderTester ::testEchoMatched() {
    Fw::Buffer command = this->tickUntilCommand();
    ComCfg::FrameContext context = this->fromPortHistory_cfsCommandOut->at(0).context;
    this->invoke_to_cfsCommandReturnIn(0, command, context);
    this->clearHistory();

    this->deliverEcho(command, false);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), this->m_telemetryStorage);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EchoMatched_SIZE(1);
    ASSERT_EVENTS_EchoMatched(0, 1, static_cast<U32>(command.getSize()));
    ASSERT_TLM_EchoesMatched_SIZE(1);
    ASSERT_TLM_EchoesMatched(0, 1);
    ASSERT_TLM_EchoMismatches_SIZE(0);
}

void EchoCommanderTester ::testEchoMismatch() {
    Fw::Buffer command = this->tickUntilCommand();
    ComCfg::FrameContext context = this->fromPortHistory_cfsCommandOut->at(0).context;
    this->invoke_to_cfsCommandReturnIn(0, command, context);
    this->clearHistory();

    this->deliverEcho(command, true);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EchoMismatch_SIZE(1);
    ASSERT_EVENTS_EchoMismatch(0, 1, static_cast<U32>(command.getSize()), static_cast<U32>(command.getSize()));
    ASSERT_TLM_EchoMismatches_SIZE(1);
    ASSERT_TLM_EchoMismatches(0, 1);
    ASSERT_TLM_EchoesMatched_SIZE(0);

    // A second echo for the same command is unexpected (the command is no longer outstanding)
    this->clearHistory();
    this->deliverEcho(command, false);
    ASSERT_EVENTS_EchoUnexpected_SIZE(1);
}

void EchoCommanderTester ::testEchoUnexpected() {
    U8 bytes[3] = {0x11, 0x22, 0x33};
    Fw::Buffer telemetry(bytes, sizeof(bytes));
    this->invoke_to_cfsTelemetryIn(0, FPrimeCfs::CfsTime(0, 0), telemetry);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).fwBuffer.getData(), bytes);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_EchoUnexpected_SIZE(1);
    ASSERT_EVENTS_EchoUnexpected(0, sizeof(bytes));
    ASSERT_TLM_EchoesUnexpected_SIZE(1);
    ASSERT_TLM_EchoesUnexpected(0, 1);
}

void EchoCommanderTester ::testEchoTimeout() {
    Fw::Buffer command = this->tickUntilCommand();
    ComCfg::FrameContext context = this->fromPortHistory_cfsCommandOut->at(0).context;
    this->invoke_to_cfsCommandReturnIn(0, command, context);
    this->clearHistory();

    // No echo arrives: the next command reports the timeout and is still sent
    this->tickUntilCommand();
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_EchoTimeout_SIZE(1);
    ASSERT_EVENTS_EchoTimeout(0, 1);
    ASSERT_EVENTS_CommandSent_SIZE(1);
    ASSERT_TLM_EchoTimeouts_SIZE(1);
    ASSERT_TLM_EchoTimeouts(0, 1);
}

void EchoCommanderTester ::testCommandBusy() {
    this->tickUntilCommand();
    this->clearHistory();

    // Command buffer not returned by the bridge: the next command is skipped
    for (U32 i = 0; i < ECHO_COMMANDER_TICKS_PER_COMMAND; i++) {
        this->invoke_to_run(0, 0);
    }
    ASSERT_from_cfsCommandOut_SIZE(0);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CommandBusy_SIZE(1);
    ASSERT_TLM_CommandsBusy_SIZE(1);
    ASSERT_TLM_CommandsBusy(0, 1);
}

}  // namespace FPrimeApp
