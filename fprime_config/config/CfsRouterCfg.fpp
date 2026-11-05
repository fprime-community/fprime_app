# ======================================================================
# CfsRouterCfg.fpp
# Project override of the CfsRouter routing table: routes F Prime commands,
# file packets, the cFS scheduler (SCH) tick message that drives the rate
# groups via the FPrimeCfs.SchAppDriver, the raw cFS echo command from the
# raw_commander app (APID + function code) to EchoResponder, and the raw cFS
# echo telemetry from the raw_commanded app (APID) to EchoCommander.
# ======================================================================
module FPrimeCfs {

    @ Number of F Prime command output ports on the CfsRouter
    constant CFS_ROUTER_FPRIME_COMMAND_PORTS = 10

    @ Number of cFS command output ports on the CfsRouter
    constant CFS_ROUTER_CFS_COMMAND_PORTS = 10

    @ Number of cFS telemetry output ports on the CfsRouter
    constant CFS_ROUTER_CFS_TELEMETRY_PORTS = 10

    @ Maximum number of buffers simultaneously outstanding on the pass-through
    @ routes (cFS command, cFS telemetry, unknown) awaiting return
    constant CFS_ROUTER_MAX_PENDING_BUFFERS = 10

    @ Number of entries in the routing table
    constant CFS_ROUTER_ROUTE_TABLE_SIZE = 5

    @ The routing table: (APID, function code for commands) -> (route type, output port index)
    constant CFS_ROUTER_ROUTE_TABLE = [
        { apid = ComCfg.Apid.FW_PACKET_COMMAND, functionCode = ComCfg.FprimeCommandFunctionCode, routeType = CfsRouteType.FPRIME_COMMAND, portIndex = 0 },
        { apid = ComCfg.Apid.FW_PACKET_FILE,    functionCode = 0, routeType = CfsRouteType.FILE,           portIndex = 0 },
        { apid = ComCfg.Apid.CFS_SCH_TICK,      functionCode = 0, routeType = CfsRouteType.CFS_COMMAND,    portIndex = 0 },
        { apid = ComCfg.Apid.FPRIME_ECHO_CMD,   functionCode = 1, routeType = CfsRouteType.CFS_COMMAND,    portIndex = 1 },
        { apid = ComCfg.Apid.RAW_ECHO_TLM,      functionCode = 0, routeType = CfsRouteType.CFS_TELEMETRY,  portIndex = 0 },
    ]
}
