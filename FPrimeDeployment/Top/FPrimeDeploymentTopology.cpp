// ======================================================================
// \title  FPrimeDeploymentTopology.cpp
// \brief cpp file containing the topology instantiation code
//
// ======================================================================
// Provides access to autocoded functions
#include <FPrimeDeployment/Top/FPrimeDeploymentTopologyAc.hpp>
// Telemetry packet list for the Svc.TlmPacketizer instance (CfsCore.tlmSend)
#include <FPrimeDeployment/Top/FPrimeDeployment_FPrimeDeploymentPacketsTlmPacketsAc.hpp>

// Public functions for use in main program are namespaced with deployment module FPrimeApp
// This is also the namespace where the topology components are instantiated by FPP.
namespace FPrimeApp {

// The topology has a single rate group driven at the scheduler tick rate; a divisor of 0 marks
// the remaining driver outputs unused
const Svc::RateGroupDriver::DividerSet rateGroupDivisorsSet{{{1, 0}, {0, 0}, {0, 0}}};

// Rate groups may supply a context token to each of the attached children whose purpose is set by the project.
// Each token is set to zero as these contexts are unused in this project.
Svc::ActiveRateGroup::ContextArray rateGroup1Context(0);

/**
 * \brief configure/setup components in project-specific way
 *
 * This is a *helper* function which configures/sets up each component requiring project specific input. This includes
 * allocating resources, passing-in arguments, etc. This function may be inlined into the topology setup function if
 * desired, but is extracted here for clarity.
 */
void configureTopology() {
    // Rate group driver needs a divisor list
    rateGroupDriver.configure(rateGroupDivisorsSet);

    // Rate groups require context arrays.
    rateGroup1.configure(rateGroup1Context);

    // Supply the deployment's generated telemetry packet list to the CfsCore
    // packetizer (replacing the subtopology's default empty packet list)
    CfsCore::tlmSend.setPacketList(FPrimeApp::FPrimeDeployment_FPrimeDeploymentPacketsTlmPackets::packetList,
                                   FPrimeApp::FPrimeDeployment_FPrimeDeploymentPacketsTlmPackets::omittedChannels, 0);
}

void setupTopology(const TopologyState& state) {
    // Autocoded initialization. Function provided by autocoder.
    initComponents(state);
    // Autocoded id setup. Function provided by autocoder.
    setBaseIds();
    // Autocoded connection wiring. Function provided by autocoder.
    connectComponents();
    // Autocoded command registration. Function provided by autocoder.
    regCommands();
    // Autocoded configuration. Function provided by autocoder.
    configComponents(state);
    // Project-specific component configuration. Function provided above. May be inlined, if desired.
    configureTopology();
    // Note: autocoded parameter loading (loadParameters()) is not called: this deployment
    // has no parameter database, and components with parameters (e.g. the TlmPacketizer)
    // fall back to their default parameter values
    // Autocoded task kick-off (active components). Function provided by autocoder.
    startTasks(state);
}

void teardownTopology(const TopologyState& state) {
    // Autocoded (active component) task clean-up. Functions provided by topology autocoder.
    stopTasks(state);
    freeThreads(state);

    tearDownComponents(state);
}
};  // namespace FPrimeApp
