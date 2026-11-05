#ifndef FPrimeApp_EchoCommanderTester_HPP
#define FPrimeApp_EchoCommanderTester_HPP

#include "Components/EchoCommander/EchoCommander.hpp"
#include "EchoCommanderGTestBase.hpp"

namespace FPrimeApp {

class EchoCommanderTester final : public EchoCommanderGTestBase {
  public:
    static const U32 MAX_HISTORY_SIZE = 10;
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    static const ComCfg::Apid::T CMD_APID = ComCfg::Apid::RAW_ECHO_CMD;

    EchoCommanderTester();
    ~EchoCommanderTester();

    void testPeriodicCommand();
    void testEchoMatched();
    void testEchoMismatch();
    void testEchoTruncated();
    void testEchoUnexpected();
    void testEchoTimeout();
    void testCommandBusy();

  private:
    //! Tick the rate group until a command is due; returns the sent command buffer (asserts one was sent)
    Fw::Buffer tickUntilCommand();
    //! Copy the sent payload into the telemetry storage and deliver it as a raw telemetry echo
    void deliverEcho(const Fw::Buffer& command, bool corrupt, FwSizeType truncateBy = 0);
    //! Tick until a command is sent, return its buffer to the component and clear histories
    Fw::Buffer sendAndReturnCommand();

    void connectPorts();    //!< Connects all ports for the component under test (auto-generated)
    void initComponents();  //!< Initializes the component under test (auto-generated)

    EchoCommander component;
    U8 m_telemetryStorage[ECHO_COMMANDER_MAX_PAYLOAD] = {};
};

}  // namespace FPrimeApp

#endif
