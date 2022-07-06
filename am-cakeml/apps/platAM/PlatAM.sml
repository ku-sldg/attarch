(* Depends on Util, camkes/Dataport *)

val bootEv  = BString.unshow "9626808860FDC8D992C19588BBB7BDDD668CA02A428D5FBB5F36E8045A46D733"
val privEnc = BString.unshow "050648DFE4278789847D3DFE1FF9599E68DC15FC7828A384BF5DFC2FCCF87010"

fun release privEnc pad = BString.xor BString.BigEndian privEnc pad

(* val priv = BString.unshow "9320C85784DA4F5016BCA876A44EE4430E50B5D63AA5FC3FE06B142B96BEA723"
val pub  = BString.unshow "26930400B922522DDE9B5A0575EB9FA85C5CE1AE280BC7F7D67CD641B195E5D9C82F6D7BA67F000040E482F9E45500000000000000000000A04181F9E4550000" *)
val priv = release privEnc bootEv

val dummyDisp : pl -> copEval = (const o const o const) Mt

val usmHi : usm = const (BString.fromString ("Howdy!\n"))
val usmMap = Map.fromList id_compare [(Id O, usmHi)]

val platAm = Am
    O 
    dummyDisp
    usmMap
    priv
    Crypto.signMsg
    Crypto.hash

(* TODO: same function used by several servers. Abstract somewhere *)
fun evalJson am s = 
    let val (REQ pl1 pl2 map t ev) = jsonToRequest (JsonExtra.fromString s)
        val ev' = evalTerm am ev t
        val resp = RES (me am) pl1 ev'
     in JsonExtra.toString (responseToJson resp)
    end
    handle Json.ERR s1 s2 => (log Error ("JSON error" ^ s1 ^ ": " ^ s2); "Invalid JSON/Copland term")
         | USMexpn s      => (log Error ("USM error: " ^ s); "USM failure")

(* string -> () *)
fun respond dp = (
    log Debug "platam: Waiting for am_dp";
    waitDataport dp;
    let val request = BString.toCString (readDataport dp 4096)
        val respStr = (
            log Debug ("platam: Got request: " ^ request);
            evalJson platAm request
        )
     in log Debug ("platam: Sending response: " ^ respStr);
        writeDataport dp (BString.nullTerminated respStr);
        emitDataport dp
    end)

(* Placeholder for real measurement *)
fun appraiseUserAM () = 
    True
(* fun appraiseUserAM () = False *)

local 
    val pad = BString.unshow "12F988F544849B4F8D526CCCAB2BD284669CFF409A325423EA883457F934EC52"
in 
    fun releaseUserAMKey dp = 
        if appraiseUserAM () then (
            log Debug "platam: UserAM passed initial appraisal. Waiting for UserAM green light.";
            waitDataport dp;
            log Debug "platam: releasing key to UserAM.";
            writeDataport dp pad;
            log Debug "platam: waiting for UserAM to finish.";
            waitDataport dp;
            log Debug "platam: key release finished.";
            True
        ) else (
            log Debug "platam: UserAM failed initial appraisal, key not released";
            False
        )
end

fun main () = (
    log Debug "platam: PlatAM started";
    setConnsFromList [("am_dp", 0)];
    writeDataport "am_dp" (BString.nulls 4096);
    log Debug "platam: am_dp initialized";
    (*setConnsFromList [("ms_dp", 1)];
    writeDataport "ms_dp" (BString.nulls 4096);
    log Debug "platam: ms_dp initialized";*)
    if releaseUserAMKey "am_dp" then
        loop respond "am_dp"
    else ()
) handle DataportErr => log Error "Dataport error"
       | _           => log Error "Fatal: unknown error"
(* val _ = Log.setLogDebug Log.noLog *)
val _ = main ()
