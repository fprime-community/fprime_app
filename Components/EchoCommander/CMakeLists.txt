register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/EchoCommander.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/EchoCommander.cpp"
    DEPENDS
        FPrimeCfs_Interfaces
        FPrimeCfs_Ports
)

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/EchoCommander.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/EchoCommanderTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/EchoCommanderTester.cpp"
    DEPENDS
        STest
    UT_AUTO_HELPERS
)
