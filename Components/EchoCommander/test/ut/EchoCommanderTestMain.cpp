#include "EchoCommanderTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, PeriodicCommand) {
    COMMENT("A raw cFS command with a random payload is sent every ECHO_COMMANDER_TICKS_PER_COMMAND ticks");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-001");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-006");
    FPrimeApp::EchoCommanderTester tester;
    tester.testPeriodicCommand();
}

TEST(Nominal, EchoMatched) {
    COMMENT("A telemetry echo equal to the sent payload is reported as matched");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-002");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-006");
    FPrimeApp::EchoCommanderTester tester;
    tester.testEchoMatched();
}

TEST(OffNominal, EchoMismatch) {
    COMMENT("A telemetry echo differing from the sent payload is reported as a mismatch");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-002");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-003");
    FPrimeApp::EchoCommanderTester tester;
    tester.testEchoMismatch();
}

TEST(OffNominal, EchoUnexpected) {
    COMMENT("Telemetry with no command outstanding is reported as unexpected and returned");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-003");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-006");
    FPrimeApp::EchoCommanderTester tester;
    tester.testEchoUnexpected();
}

TEST(OffNominal, EchoTimeout) {
    COMMENT("A command due while the previous one is unanswered reports a timeout");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-004");
    FPrimeApp::EchoCommanderTester tester;
    tester.testEchoTimeout();
}

TEST(OffNominal, CommandBusy) {
    COMMENT("A command due while the previous buffer is not returned is skipped with a warning");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-005");
    FPrimeApp::EchoCommanderTester tester;
    tester.testCommandBusy();
}

TEST(OffNominal, EchoTruncated) {
    COMMENT("A telemetry echo shorter than the sent payload is reported as a mismatch with both sizes");
    REQUIREMENT("FPRIMEAPP-ECHOCOMMANDER-002");
    FPrimeApp::EchoCommanderTester tester;
    tester.testEchoTruncated();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
