#ifndef FPrimeApp_EchoCommander_HPP
#define FPrimeApp_EchoCommander_HPP

#include "Components/EchoCommander/EchoCommanderComponentAc.hpp"
#include "config/FppConstantsAc.hpp"
#include "Components/EchoCommander/FppConstantsAc.hpp"

namespace FPrimeApp {

class EchoCommander final : public EchoCommanderComponentBase {
  public:
    //! Function code the raw cFS commanded app expects for the echo command
    static constexpr U8 ECHO_FUNCTION_CODE = static_cast<U8>(ComCfg::EchoFunctionCode);

    explicit EchoCommander(const char* const compName);
    ~EchoCommander();

    //! Set the APID of the raw cFS echo command (placed in the frame context of each command)
    void configure(ComCfg::Apid::T commandApid);

  private:
    void run_handler(FwIndexType portNum, U32 context) override;
    void cfsCommandReturnIn_handler(FwIndexType portNum,
                                    Fw::Buffer& data,
                                    const ComCfg::FrameContext& context) override;
    void cfsTelemetryIn_handler(FwIndexType portNum,
                                const ComCfg::Apid& apid,
                                const FPrimeCfs::CfsTime& sysTime,
                                Fw::Buffer& data) override;

    //! Next pseudo-random value (xorshift32)
    U32 nextRandom();
    void sendCommand();

    ComCfg::Apid::T m_commandApid;
    U8 m_payload[ECHO_COMMANDER_MAX_PAYLOAD];
    FwSizeType m_payloadSize;
    U32 m_ticks;
    U32 m_sequence;
    U32 m_seed;
    bool m_inFlight;     //!< Command buffer handed to the bridge and not yet returned
    bool m_outstanding;  //!< Command sent and echo not yet received
    U32 m_commandsSent;
    U32 m_echoesMatched;
    U32 m_echoMismatches;
    U32 m_echoesUnexpected;
    U32 m_echoTimeouts;
    U32 m_commandsBusy;
};

}  // namespace FPrimeApp

#endif
