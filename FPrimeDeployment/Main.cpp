// ======================================================================
// \title  Main.cpp
// \brief main program for the F' application. Intended for CLI-based systems (Linux, macOS)
//
// ======================================================================
// Used to access topology functions
#include <FPrimeDeployment/Top/FPrimeDeploymentTopology.hpp>
#include <FPrimeDeployment/Top/FPrimeDeploymentTopologyAc.hpp>
// OSAL initialization
#include <Os/Os.hpp>

extern "C" {
    #include "cfe.h"
    #include "cfe_config.h"
    #include "fprime_app_version.h"
    #include "fprime_app_internal_cfg.h"
    void FPRIME_APP_Main(void);
}

CFE_Status_t FPRIME_APP_Init(FPrimeApp::TopologyState& inputs);

// Topology state stored at file scope so the delete callback can always reach it.
static FPrimeApp::TopologyState g_topologyState;

static void FPRIME_APP_StopAndTeardown(FPrimeApp::TopologyState& inputs)
{
    FPrimeApp::teardownTopology(inputs);
}

static void FPRIME_APP_Shutdown(FPrimeApp::TopologyState& inputs, uint32 status)
{
    FPRIME_APP_StopAndTeardown(inputs);
    CFE_ES_ExitApp(status);
}

void FPRIME_APP_delete_callback(void)
{
    FPRIME_APP_StopAndTeardown(g_topologyState);
}

void FPRIME_APP_Main(void) {
    Os::init();

    FPrimeApp::TopologyState& inputs = g_topologyState;
    uint32 run_status = CFE_ES_RunStatus_APP_RUN;

    CFE_Status_t status = FPRIME_APP_Init(inputs);
    if (status != CFE_SUCCESS) {
        FPRIME_APP_Shutdown(inputs, status);
    }

    // The rate groups are driven by cFS scheduler (SCH) tick messages received
    // through the bridge and routed to the SchAppDriver; no polling timer is needed
    while (CFE_ES_RunLoop(&run_status) == true)
    {
        FPrimeApp::cfsBridge.process();
    }
    FPRIME_APP_Shutdown(inputs, run_status);
}

CFE_Status_t FPRIME_APP_Init(FPrimeApp::TopologyState& inputs)
{
    CFE_Status_t status;
    char         VersionString[FPRIME_APP_CFG_MAX_VERSION_STR_LEN];

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("F Prime App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
    }
    else
    {
        status = FPrimeApp::cfsBridge.configure(FPRIME_APP_PLATFORM_PIPE_DEPTH, FPRIME_APP_PLATFORM_PIPE_NAME);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(2, CFE_EVS_EventType_ERROR,
                              "F Prime App: Error creating SB Command Pipe, RC = 0x%08lX", (unsigned long)status);
        }
    }
    if (status == CFE_SUCCESS)
    {
        status = FPrimeApp::cfsBridge.subscribe(ComCfg::Apid::FW_PACKET_COMMAND);
        if (status == CFE_SUCCESS)
        {
            status = FPrimeApp::cfsBridge.subscribeCfs(ComCfg::Apid::CFS_SCH_TICK,
                                                       FPrimeCfs::CfsBridge::CfsMessageType::COMMAND);
        }
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(2, CFE_EVS_EventType_ERROR,
                              "F Prime App: Error subscribing to cFS messages, RC = 0x%08lX", (unsigned long)status);
        }
    }
    OS_TaskInstallDeleteHandler(&FPRIME_APP_delete_callback);
    FPrimeApp::setupTopology(inputs);

    if (status == CFE_SUCCESS)
    {
        CFE_Config_GetVersionString(VersionString, FPRIME_APP_CFG_MAX_VERSION_STR_LEN, "F Prime App", FPRIME_APP_VERSION,
                                    FPRIME_APP_BUILD_CODENAME, FPRIME_APP_LAST_OFFICIAL);

        CFE_EVS_SendEvent(1, CFE_EVS_EventType_INFORMATION, "F Prime App Initialized.%s",
                          VersionString);
    }

    return status;
}
