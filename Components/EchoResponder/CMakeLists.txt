register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/EchoResponder.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/EchoResponder.cpp"
    DEPENDS
        FPrimeCfs_Ports
)

### Unit Tests ###
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/EchoResponder.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/EchoResponderTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/EchoResponderTester.cpp"
    DEPENDS
        STest
    UT_AUTO_HELPERS
)
