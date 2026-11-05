#include "Components/EchoResponder/EchoResponder.hpp"

#include <cstring>

namespace FPrimeApp {

constexpr U8 EchoResponder::EXPECTED_FUNCTION_CODE;

EchoResponder ::EchoResponder(const char* const compName)
    : EchoResponderComponentBase(compName),
      m_telemetryApid(ComCfg::Apid::FW_PACKET_UNKNOWN),
      m_payload(),
      m_inFlight(false),
      m_commandsReceived(0),
      m_echoesSent(0),
      m_unexpectedFunctionCodes(0),
      m_payloadsTooLarge(0),
      m_echoesBusy(0) {}

EchoResponder ::~EchoResponder() {}

void EchoResponder ::configure(ComCfg::Apid::T telemetryApid) {
    this->m_telemetryApid = telemetryApid;
}

void EchoResponder ::cfsCommandIn_handler(FwIndexType portNum,
                                          const ComCfg::Apid& apid,
                                          U8 functionCode,
                                          Fw::Buffer& data) {
    const U32 size = static_cast<U32>(data.getSize());
    if (functionCode != EXPECTED_FUNCTION_CODE) {
        this->m_unexpectedFunctionCodes++;
        this->log_WARNING_LO_UnexpectedFunctionCode(functionCode, EXPECTED_FUNCTION_CODE);
        this->tlmWrite_UnexpectedFunctionCodes(this->m_unexpectedFunctionCodes);
    } else if (data.getSize() > sizeof(this->m_payload)) {
        this->m_payloadsTooLarge++;
        this->log_WARNING_HI_PayloadTooLarge(size, static_cast<U32>(sizeof(this->m_payload)));
        this->tlmWrite_PayloadsTooLarge(this->m_payloadsTooLarge);
    } else if (this->m_inFlight) {
        this->m_echoesBusy++;
        this->log_WARNING_HI_EchoBusy();
        this->tlmWrite_EchoesBusy(this->m_echoesBusy);
    } else {
        this->m_commandsReceived++;
        this->log_ACTIVITY_HI_CommandReceived(functionCode, size, (size > 0) ? data.getData()[0] : 0);
        this->tlmWrite_CommandsReceived(this->m_commandsReceived);
        // Echo the exact bytes; the APID in the context selects the cFS telemetry message
        (void)std::memcpy(this->m_payload, data.getData(), data.getSize());
        Fw::Buffer echo(this->m_payload, static_cast<Fw::Buffer::SizeType>(data.getSize()));
        ComCfg::FrameContext context;
        context.set_apid(this->m_telemetryApid);
        this->m_inFlight = true;
        this->cfsTelemetryOut_out(0, echo, context);
        this->m_echoesSent++;
        this->log_ACTIVITY_LO_EchoSent(size);
        this->tlmWrite_EchoesSent(this->m_echoesSent);
    }
    // The command payload belongs to the router; hand it back once processed
    this->bufferReturnOut_out(0, data);
}

void EchoResponder ::cfsTelemetryReturnIn_handler(FwIndexType portNum,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    FW_ASSERT(data.getData() == this->m_payload);
    this->m_inFlight = false;
}

}  // namespace FPrimeApp
