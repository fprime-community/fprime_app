module FPrimeApp {

  # ----------------------------------------------------------------------
  # Symbolic constants for port numbers
  # ----------------------------------------------------------------------

  enum Ports_RateGroups {
    rateGroup1
  }

  enum Ports_ComPacketQueue {
    EVENTS,
    TELEMETRY
  }


  deployment topology FPrimeDeployment {

  # ----------------------------------------------------------------------
  # Subtopology imports
  # ----------------------------------------------------------------------
    import CfsCore.Subtopology
    import ComCfs.Subtopology

  # ----------------------------------------------------------------------
  # Instances used in the topology
  # ----------------------------------------------------------------------
    instance rateGroupDriver
    instance rateGroup1

  # ----------------------------------------------------------------------
  # Pattern graph specifiers
  # ----------------------------------------------------------------------

    command connections instance CfsCore.cmdDisp
    event connections instance CfsCore.events
    telemetry connections instance CfsCore.tlmSend
    text event connections instance CfsCore.evsMirror
    health connections instance CfsCore.$health
    time connections instance CfsCore.cfsTime

  # ----------------------------------------------------------------------
  # Telemetry packets (only used when TlmPacketizer is used)
  # ----------------------------------------------------------------------

    include "FPrimeDeploymentPackets.fppi"

  # ----------------------------------------------------------------------
  # Direct graph specifiers
  # ----------------------------------------------------------------------

    connections Scheduler {
      # cFS scheduler (SCH) tick messages, routed by the ComCfs router to the
      # CfsCore SchAppDriver, drive the rate group
      ComCfs.Subtopology.cfsCommandOut[0]    -> CfsCore.Subtopology.cfsCommandIn
      CfsCore.Subtopology.schBufferReturnOut -> ComCfs.Subtopology.bufferReturnIn
      CfsCore.Subtopology.cycleOut           -> rateGroupDriver.CycleIn
    }

    connections RateGroups {
      # Rate group 1
      rateGroupDriver.CycleOut[Ports_RateGroups.rateGroup1] -> rateGroup1.CycleIn
      rateGroup1.RateGroupMemberOut[0] -> CfsCore.Subtopology.cmdDispRun
      rateGroup1.RateGroupMemberOut[1] -> CfsCore.Subtopology.tlmSendRun
      rateGroup1.RateGroupMemberOut[2] -> CfsCore.Subtopology.healthRun
      rateGroup1.RateGroupMemberOut[3] -> CfsCore.Subtopology.eventsRun
      rateGroup1.RateGroupMemberOut[4] -> ComCfs.Subtopology.bufferManagerSchedIn
    }

    connections Commanding {
      # Routed F Prime command packets to the command dispatcher
      ComCfs.Subtopology.commandOut[0]  -> CfsCore.Subtopology.seqCmdBuff
      CfsCore.Subtopology.seqCmdStatus  -> ComCfs.Subtopology.cmdResponseIn
    }

    connections Downlink {
      # Event and packetized telemetry downlink through the telemetry app bridge
      CfsCore.Subtopology.eventsPktSend      -> ComCfs.Subtopology.comIn[Ports_ComPacketQueue.EVENTS]
      CfsCore.Subtopology.tlmSendPktSend[0]  -> ComCfs.Subtopology.comIn[Ports_ComPacketQueue.TELEMETRY]
    }

    connections Time {
      # cFS time conversion for the telemetry framer secondary headers
      ComCfs.Subtopology.cfsTimeConvertOut -> CfsCore.Subtopology.cfsTimeConvert
    }

  }

}
