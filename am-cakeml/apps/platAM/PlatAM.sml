(* Depends on Util, camkes/Dataport *)

local type bs = BString.bstring in
    val dummyDisp : pl  -> copEval  = (const o const o const) Mt
    val dummySign : key -> bs -> bs = (const o const) BString.empty
    val dummyHsh  : bs  -> bs       = const BString.empty
end

val usmHi : usm = const (BString.fromString ("Howdy!\n"))
val usmMap = Map.fromList id_compare [(Id O, usmHi)]

val dummyAm = Am
    O 
    dummyDisp
    usmMap
    BString.empty
    dummySign
    dummyHsh

(* TODO: same funtion used by several servers. Abstract somewhere *)
fun evalJson am s = 
    let val (REQ pl1 pl2 map t ev) = jsonToRequest (JsonExtra.fromString s)
        val ev' = evalTerm am ev t
        val resp = RES (me am) pl1 ev'
     in JsonExtra.toString (responseToJson resp)
    end
    handle Json.ERR s1 s2 => (log Error ("JSON error" ^ s1 ^ ": " ^ s2); "Invalid JSON/Copland term")
         | USMexpn s      => (log Error ("USM error: " ^ s); "USM failure")

fun respond () = (
    log Debug "Waiting for am_dp";
    waitDataport "am_dp";
    let val request = BString.toCString (readDataport "am_dp" 4096)
        val respStr = (
            log Debug ("Got request: " ^ request);
            evalJson dummyAm request
        )
     in log Debug ("Sending response: " ^ respStr);
        writeDataport "am_dp" (BString.nullTerminated respStr);
        emitDataport "am_dp"
    end)

fun main () = (
    log Debug "PlatAM started";
    setConnsFromList [("am_dp", 0)];
    writeDataport "am_dp" (BString.nulls 4096);
    log Debug "am_dp initialized";
    loop respond ()
) handle DataportErr => log Error "Dataport error"
       | _           => log Error "Fatal: unknown error"
val _ = Log.setLogDebug Log.noLog
val _ = main ()