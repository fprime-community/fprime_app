#ifndef FPrimeApp_EchoResponder_HPP
#define FPrimeApp_EchoResponder_HPP

#include "Components/EchoResponder/EchoResponderComponentAc.hpp"
#include "Components/EchoResponder/FppConstantsAc.hpp"

namespace FPrimeApp {

class EchoResponder final : public EchoResponderComponentBase {
  public:
    //! Function code the raw cFS commander uses for the echo command
    static constexpr U8 EXPECTED_FUNCTION_CODE = 1;

    explicit EchoResponder(const char* const compName);
    ~EchoResponder();

    //! Set the APID of the raw cFS telemetry echo (placed in the frame context of each echo)
    void configure(ComCfg::Apid::T telemetryApid);

  private:
    void cfsCommandIn_handler(FwIndexType portNum, U8 functionCode, Fw::Buffer& data) override;
    void cfsTelemetryReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) override;

    ComCfg::Apid::T m_telemetryApid;
    U8 m_payload[ECHO_RESPONDER_MAX_PAYLOAD];
    bool m_inFlight;
    U32 m_commandsReceived;
    U32 m_echoesSent;
    U32 m_unexpectedFunctionCodes;
    U32 m_payloadsTooLarge;
    U32 m_echoesBusy;
};

}  // namespace FPrimeApp

#endif
