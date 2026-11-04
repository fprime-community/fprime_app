#ifndef FPrimeApp_EchoResponderTester_HPP
#define FPrimeApp_EchoResponderTester_HPP

#include "Components/EchoResponder/EchoResponder.hpp"
#include "EchoResponderGTestBase.hpp"

namespace FPrimeApp {

class EchoResponderTester final : public EchoResponderGTestBase {
  public:
    static const U32 MAX_HISTORY_SIZE = 10;
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;
    static const ComCfg::Apid::T TLM_APID = ComCfg::Apid::FPRIME_ECHO_TLM;

    EchoResponderTester();
    ~EchoResponderTester();

    void testEcho();
    void testUnexpectedFunctionCode();
    void testPayloadTooLarge();
    void testEchoBusy();

  private:
    //! Send a command of the given size with random payload bytes; returns the command buffer
    Fw::Buffer sendCommand(U8 functionCode, FwSizeType size);

    void connectPorts();    //!< Connects all ports for the component under test (auto-generated)
    void initComponents();  //!< Initializes the component under test (auto-generated)

    EchoResponder component;
    U8 m_commandStorage[ECHO_RESPONDER_MAX_PAYLOAD + 1] = {};
};

}  // namespace FPrimeApp

#endif
