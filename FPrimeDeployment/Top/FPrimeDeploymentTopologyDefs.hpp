// ======================================================================
// \title  FPrimeDeploymentTopologyDefs.hpp
// \brief required header file containing the required definitions for the topology autocoder
//
// ======================================================================
#ifndef FPRIMEDEPLOYMENT_FPRIMEDEPLOYMENTTOPOLOGYDEFS_HPP
#define FPRIMEDEPLOYMENT_FPRIMEDEPLOYMENTTOPOLOGYDEFS_HPP

// Subtopology PingEntries includes
#include "FPrimeCfs/Subtopologies/CfsCore/PingEntries.hpp"
#include "FPrimeCfs/Subtopologies/ComCfs/PingEntries.hpp"

// SubtopologyTopologyDefs includes
#include "FPrimeCfs/Subtopologies/CfsCore/SubtopologyTopologyDefs.hpp"
#include "FPrimeCfs/Subtopologies/ComCfs/SubtopologyTopologyDefs.hpp"

// Include autocoded FPP constants
#include "FPrimeDeployment/Top/FppConstantsAc.hpp"
#include "Fw/Types/MallocAllocator.hpp"

/**
 * \brief required ping constants
 *
 * The topology autocoder requires a WARN and FATAL constant definition for each component that supports the health-ping
 * interface. These are expressed as enum constants placed in a namespace named for the component instance. These
 * are all placed in the PingEntries namespace.
 *
 * Each constant specifies how many missed pings are allowed before a WARNING_HI/FATAL event is triggered. In the
 * following example, the health component will emit a WARNING_HI event if the component instance cmdDisp does not
 * respond for 3 pings and will FATAL if responses are not received after a total of 5 pings.
 *
 * ```c++
 * namespace PingEntries {
 * namespace cmdDisp {
 *     enum { WARN = 3, FATAL = 5 };
 * }
 * }
 * ```
 */
namespace PingEntries {
    namespace FPrimeApp_rateGroup1 {enum { WARN = 3, FATAL = 5 };}
}  // namespace PingEntries

// Definitions are placed within the same namespace as the FPP module that contains the topology.
namespace FPrimeApp {

/**
 * \brief required type definition to carry state
 *
 * The topology autocoder requires an object that carries state with the name `FPrimeApp::TopologyState`. Only the type
 * definition is required by the autocoder and the contents of this object are otherwise opaque to the autocoder. The
 * contents are entirely up to the definition of the project. This deployment uses subtopologies.
 */
struct TopologyState {
    CfsCore::SubtopologyState cfsCore;   //!< Subtopology state for CfsCore
    ComCfs::SubtopologyState comCfs;     //!< Subtopology state for ComCfs
};

namespace PingEntries = ::PingEntries;
}  // namespace FPrimeApp

#endif
