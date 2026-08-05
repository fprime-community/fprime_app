telemetry packets FPrimeDeploymentPackets {

  packet CDH id 1 group 1 {
    CfsCore.cmdDisp.CommandsDispatched
    CfsCore.cmdDisp.CommandsDropped

    ComCfs.commsBufferManager.TotalBuffs
    ComCfs.commsBufferManager.CurrBuffs
    ComCfs.commsBufferManager.HiBuffs

    FPrimeApp.rateGroup1.RgMaxTime
  }

  packet CDHErrors id 2 group 1 {
    CfsCore.$health.PingLateWarnings
    CfsCore.events.EventsDropped

    ComCfs.commsBufferManager.NoBuffs
    ComCfs.commsBufferManager.EmptyBuffs

    FPrimeApp.rateGroup1.RgCycleSlips
  }

  packet Version1 id 22 group 2 {
    CfsCore.version.FrameworkVersion
    CfsCore.version.ProjectVersion
  }

  packet Version_Library1 id 23 group 2 {
    CfsCore.version.LibraryVersion01
    CfsCore.version.LibraryVersion02
  }

  packet Version_Library2 id 24 group 2 {
    CfsCore.version.LibraryVersion03
    CfsCore.version.LibraryVersion04
  }

  packet Version_Library3 id 25 group 2 {
    CfsCore.version.LibraryVersion05
    CfsCore.version.LibraryVersion06
  }

  packet Version_Library4 id 26 group 2 {
    CfsCore.version.LibraryVersion07
    CfsCore.version.LibraryVersion08
  }

  packet Version_Library5 id 27 group 2 {
    CfsCore.version.LibraryVersion09
    CfsCore.version.LibraryVersion10
  }

  packet Version_Custom1 id 28 group 2 {
    CfsCore.version.CustomVersion01
    CfsCore.version.CustomVersion02
    CfsCore.version.CustomVersion03
    CfsCore.version.CustomVersion04
    CfsCore.version.CustomVersion05
  }

  packet Version_Custom2 id 29 group 2 {
    CfsCore.version.CustomVersion06
    CfsCore.version.CustomVersion07
    CfsCore.version.CustomVersion08
    CfsCore.version.CustomVersion09
    CfsCore.version.CustomVersion10
  }

} omit {
  CfsCore.cmdDisp.CommandErrors
  CfsCore.tlmSend.GroupConfigs
  CfsCore.tlmSend.SectionEnabled
}
