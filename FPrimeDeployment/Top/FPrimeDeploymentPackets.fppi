telemetry packets FPrimeDeploymentPackets {

  @ Single packet layout: cFS assumes one layout per APID, and all F Prime telemetry
  @ shares one APID. Health and error channels only; all other telemetry is omitted.
  packet Health id 1 group 1 {
    CfsCore.cmdDisp.CommandsDispatched
    CfsCore.cmdDisp.CommandsDropped
    CfsCore.$health.PingLateWarnings
    CfsCore.events.EventsDropped

    ComCfs.commsBufferManager.NoBuffs
    ComCfs.commsBufferManager.EmptyBuffs

    FPrimeApp.rateGroup1.RgCycleSlips
  }

} omit {
  CfsCore.cmdDisp.CommandErrors
  CfsCore.tlmSend.GroupConfigs
  CfsCore.tlmSend.SectionEnabled

  ComCfs.commsBufferManager.TotalBuffs
  ComCfs.commsBufferManager.CurrBuffs
  ComCfs.commsBufferManager.HiBuffs

  FPrimeApp.rateGroup1.RgMaxTime

  CfsCore.version.FrameworkVersion
  CfsCore.version.ProjectVersion
  CfsCore.version.LibraryVersion01
  CfsCore.version.LibraryVersion02
  CfsCore.version.LibraryVersion03
  CfsCore.version.LibraryVersion04
  CfsCore.version.LibraryVersion05
  CfsCore.version.LibraryVersion06
  CfsCore.version.LibraryVersion07
  CfsCore.version.LibraryVersion08
  CfsCore.version.LibraryVersion09
  CfsCore.version.LibraryVersion10
  CfsCore.version.CustomVersion01
  CfsCore.version.CustomVersion02
  CfsCore.version.CustomVersion03
  CfsCore.version.CustomVersion04
  CfsCore.version.CustomVersion05
  CfsCore.version.CustomVersion06
  CfsCore.version.CustomVersion07
  CfsCore.version.CustomVersion08
  CfsCore.version.CustomVersion09
  CfsCore.version.CustomVersion10
}
