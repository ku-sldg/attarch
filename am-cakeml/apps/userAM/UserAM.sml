(* Depends on: util, copland, system/sockets, am/Measurements, am/CommTypes,
   am/ServerAm, CrossVMDispatch *)

val privEnc = BString.unshow "3CAEFB47E51EB7FFD2BC28A9A6A61AF3D544F9B7D62BC4608FF4B40571F9FDC9"
(* val pad     = BString.unshow "12F988F544849B4F8D526CCCAB2BD284669CFF409A325423EA883457F934EC52" *)

fun release privEnc pad = BString.xor BString.BigEndian privEnc pad

(* val priv = BString.unshow "2E5773B2A19A2CB05FEE44650D8DC877B3D806F74C199043657C805288CD119B" *)
val priv = Ref privEnc

(* val am = userAm priv (Map.insert emptyNsMap (S (S O)) "/dev/uio0") emptyNsMap *)

fun evalJson s =
    let val (REQ pl1 pl2 map t ev) = jsonToRequest (JsonExtra.fromString s)
        (* Note, that the am currently ignores the nsMap from the request.
           In theory, instead of using its own, it should use the request's map.
           But then, it needs a way to distinguish ip from dataport dispatches

           It should also use "pl2" as "me"
         *)
        val am = userAm (!priv) (Map.insert emptyNsMap (S (S O)) "/dev/uio0") emptyNsMap
        val ev' = evalTerm am ev t
        val response = RES (me am) pl1 ev'
     in JsonExtra.toString (responseToJson response)
    end
    handle Json.ERR s1 s2 => (log Error ("useram: JSON error" ^ s1 ^ ": " ^ s2); "Invalid JSON/Copland term")
         | USMexpn s      => (log Error ("useram: USM error: " ^ s); "USM failure")
         | DispatchErr s  => (log Error ("useram: Dispatch error: " ^ s); "Dispatch error")

(* fun respondToMsg client = Socket.output client (evalJson (Socket.inputAll client)) *)
fun respondToMsg client = 
    let val req  = Socket.inputAll client
        val resp = (
            log Debug ("useram: Received request: " ^ req);
            evalJson req
        )
     in log Debug ("useram: Sending back response: " ^ resp);
        Socket.output client resp
    end

fun handleIncoming listener =
    let val _ = log Debug ("useram: Waiting.")
        val client = Socket.accept listener
     in log Debug "useram: Incoming connection accepted";
        respondToMsg client;
        Socket.close client
    end
    handle Socket.Err s     => log Error ("useram: Socket failure: " ^ s)
         | Socket.InvalidFD => log Error "useram: Invalid file descriptor"

fun getKey () =
    let val pad = (
            log Debug "useram: Signal ready for key. Waiting...";
            emitDataport "/dev/uio0";
            (* We do not need to wait here, because the platAM preempts execution of the userAM *)
            log Debug "useram: Collecting key.";
            readDataport "/dev/uio0" 32
        )
     in log Debug "useram: Received key.";
        priv := release (!priv) pad;
        emitDataport "/dev/uio0"
    end
    
fun startServer port qLen = 
    (* loop handleIncoming (Socket.listen port qLen) *)
    let val listener = Socket.listen port qLen
     in log Debug "useram: Listener opened";
        getKey ();
        loop handleIncoming listener
    end
    handle Socket.Err s => log Error ("useram: Socket failure on listener instantiation: " ^ s)
         | Crypto.Err s => log Error ("useram: Crypto error: " ^ s)
         | _            => log Error "useram: Fatal: unknown error"

fun main () =
    let val name  = CommandLine.name ()
        val usage = "Usage: " ^ name ^ " portNumber queueLength\n"
                  ^ "e.g.   " ^ name ^ " 5000 5\n"
     in case CommandLine.arguments () of
             [portStr, qLenStr] => (
                case (Int.fromString portStr, Int.fromString qLenStr) of
                     (Some port, Some qLen) => startServer port qLen
                    | _ => TextIO.print_err usage)
           | _ => TextIO.print_err usage
    end
(* val _ = Log.setLogDebug Log.noLog *)
val _ = main ()
