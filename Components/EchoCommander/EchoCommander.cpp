#include "Components/EchoCommander/EchoCommander.hpp"

#include <cstring>

namespace FPrimeApp {

EchoCommander ::EchoCommander(const char* const compName)
    : EchoCommanderComponentBase(compName),
      m_commandApid(ComCfg::Apid::FW_PACKET_UNKNOWN),
      m_payload(),
      m_payloadSize(0),
      m_ticks(0),
      m_sequence(0),
      m_seed(0),
      m_inFlight(false),
      m_outstanding(false),
      m_commandsSent(0),
      m_echoesMatched(0),
      m_echoMismatches(0),
      m_echoesUnexpected(0),
      m_echoTimeouts(0),
      m_commandsBusy(0) {}

EchoCommander ::~EchoCommander() {}

void EchoCommander ::configure(ComCfg::Apid::T commandApid) {
    this->m_commandApid = commandApid;
}

U32 EchoCommander ::nextRandom() {
    if (this->m_seed == 0) {
        // Seed once from the current time; the payload only needs to vary between commands
        const Fw::Time now = this->getTime();
        this->m_seed = (now.getSeconds() ^ (now.getUSeconds() << 7)) | 1U;
    }
    U32 x = this->m_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    this->m_seed = (x == 0) ? 0x9E3779B9U : x;
    return this->m_seed;
}

void EchoCommander ::run_handler(FwIndexType portNum, U32 context) {
    this->m_ticks++;
    if (this->m_ticks >= ECHO_COMMANDER_TICKS_PER_COMMAND) {
        this->m_ticks = 0;
        this->sendCommand();
    }
}

void EchoCommander ::sendCommand() {
    if (this->m_inFlight) {
        this->m_commandsBusy++;
        this->log_WARNING_HI_CommandBusy();
        this->tlmWrite_CommandsBusy(this->m_commandsBusy);
        return;
    }
    if (this->m_outstanding) {
        this->m_echoTimeouts++;
        this->log_WARNING_HI_EchoTimeout(this->m_sequence);
        this->tlmWrite_EchoTimeouts(this->m_echoTimeouts);
    }
    this->m_sequence++;
    this->m_payloadSize = 1 + (this->nextRandom() % sizeof(this->m_payload));
    for (FwSizeType i = 0; i < this->m_payloadSize; i++) {
        this->m_payload[i] = static_cast<U8>(this->nextRandom() & 0xFF);
    }
    // The APID and function code in the context select the cFS command message; headers,
    // checksum and sequence count are added downstream
    Fw::Buffer command(this->m_payload, static_cast<Fw::Buffer::SizeType>(this->m_payloadSize));
    ComCfg::FrameContext context;
    context.set_apid(this->m_commandApid);
    context.set_functionCode(ECHO_FUNCTION_CODE);
    this->m_inFlight = true;
    this->m_outstanding = true;
    this->cfsCommandOut_out(0, command, context);
    this->m_commandsSent++;
    this->log_ACTIVITY_LO_CommandSent(this->m_sequence, static_cast<U32>(this->m_payloadSize), this->m_payload[0]);
    this->tlmWrite_CommandsSent(this->m_commandsSent);
}

void EchoCommander ::cfsCommandReturnIn_handler(FwIndexType portNum,
                                                Fw::Buffer& data,
                                                const ComCfg::FrameContext& context) {
    FW_ASSERT(data.getData() == this->m_payload);
    this->m_inFlight = false;
}

void EchoCommander ::cfsTelemetryIn_handler(FwIndexType portNum, const FPrimeCfs::CfsTime& sysTime, Fw::Buffer& data) {
    const U32 size = static_cast<U32>(data.getSize());
    if (!this->m_outstanding) {
        this->m_echoesUnexpected++;
        this->log_WARNING_LO_EchoUnexpected(size);
        this->tlmWrite_EchoesUnexpected(this->m_echoesUnexpected);
    } else {
        this->m_outstanding = false;
        if ((data.getSize() == this->m_payloadSize) &&
            (std::memcmp(data.getData(), this->m_payload, this->m_payloadSize) == 0)) {
            this->m_echoesMatched++;
            this->log_ACTIVITY_HI_EchoMatched(this->m_sequence, size);
            this->tlmWrite_EchoesMatched(this->m_echoesMatched);
        } else {
            this->m_echoMismatches++;
            this->log_WARNING_HI_EchoMismatch(this->m_sequence, static_cast<U32>(this->m_payloadSize), size);
            this->tlmWrite_EchoMismatches(this->m_echoMismatches);
        }
    }
    // The telemetry payload belongs to the router; hand it back once processed
    this->bufferReturnOut_out(0, data);
}

}  // namespace FPrimeApp
