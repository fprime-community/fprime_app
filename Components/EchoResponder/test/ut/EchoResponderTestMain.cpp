#include "EchoResponderTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, Echo) {
    COMMENT("A valid raw cFS command is echoed byte-for-byte as raw telemetry with the configured APID");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-001");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-005");
    FPrimeApp::EchoResponderTester tester;
    tester.testEcho();
}

TEST(OffNominal, UnexpectedFunctionCode) {
    COMMENT("A command with an unexpected function code is dropped with a warning and counted");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-002");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-005");
    FPrimeApp::EchoResponderTester tester;
    tester.testUnexpectedFunctionCode();
}

TEST(OffNominal, PayloadTooLarge) {
    COMMENT("A command payload larger than the echo buffer is dropped with a warning and counted");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-003");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-005");
    FPrimeApp::EchoResponderTester tester;
    tester.testPayloadTooLarge();
}

TEST(OffNominal, EchoBusy) {
    COMMENT("A command arriving before the previous echo buffer is returned is dropped with a warning");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-004");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-005");
    FPrimeApp::EchoResponderTester tester;
    tester.testEchoBusy();
}

TEST(Nominal, EchoEmpty) {
    COMMENT("A zero-length raw command is echoed as a zero-length telemetry payload");
    REQUIREMENT("FPRIMEAPP-ECHORESPONDER-001");
    FPrimeApp::EchoResponderTester tester;
    tester.testEchoEmpty();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
