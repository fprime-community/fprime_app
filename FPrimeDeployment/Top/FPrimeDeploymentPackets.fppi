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
    # Raw cFS inter-app echo loop event counters
    FPrimeApp.echoResponder.CommandsReceived
    FPrimeApp.echoResponder.EchoesSent
    FPrimeApp.echoResponder.UnexpectedFunctionCodes
    FPrimeApp.echoResponder.PayloadsTooLarge
    FPrimeApp.echoResponder.EchoesBusy
    FPrimeApp.echoCommander.CommandsSent
    FPrimeApp.echoCommander.EchoesMatched
    FPrimeApp.echoCommander.EchoMismatches
    FPrimeApp.echoCommander.EchoesUnexpected
    FPrimeApp.echoCommander.EchoTimeouts
    FPrimeApp.echoCommander.CommandsBusy
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
  CfsCore.version.CustomVersion11
  CfsCore.version.CustomVersion12
  CfsCore.version.CustomVersion13
  CfsCore.version.CustomVersion14
  CfsCore.version.CustomVersion15
  CfsCore.version.CustomVersion16
  CfsCore.version.CustomVersion17
  CfsCore.version.CustomVersion18
  CfsCore.version.CustomVersion19
  CfsCore.version.CustomVersion20
  CfsCore.version.CustomVersion21
  CfsCore.version.CustomVersion22
  CfsCore.version.CustomVersion23
  CfsCore.version.CustomVersion24
  CfsCore.version.CustomVersion25
  CfsCore.version.CustomVersion26
  CfsCore.version.CustomVersion27
  CfsCore.version.CustomVersion28
  CfsCore.version.CustomVersion29
  CfsCore.version.CustomVersion30
  CfsCore.version.CustomVersion31
  CfsCore.version.CustomVersion32
}
