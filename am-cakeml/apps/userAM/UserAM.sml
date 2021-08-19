(* Depends on: util, copland, system/sockets, am/Measurements, am/CommTypes,
   am/ServerAm, CrossVMDispatch *)

val priv = BString.unshow "2E5773B2A19A2CB05FEE44650D8DC877B3D806F74C199043657C805288CD119B"

val am = userAm priv (Map.insert emptyNsMap (S (S O)) "/dev/uio0") emptyNsMap

fun evalJson s =
    let val (REQ pl1 pl2 map t ev) = jsonToRequest (JsonExtra.fromString s)
        (* Note, that the am currently ignores the nsMap from the request.
           In theory, instead of using its own, it should use the request's map.
           But then, it needs a way to distinguish ip from dataport dispatches

           It should also use "pl2" as "me"
         *)
        val ev' = evalTerm am ev t
        val response = RES (me am) pl1 ev'
     in JsonExtra.toString (responseToJson response)
    end
    handle Json.ERR s1 s2 => (log Error ("JSON error" ^ s1 ^ ": " ^ s2); "Invalid JSON/Copland term")
         | USMexpn s      => (log Error ("USM error: " ^ s); "USM failure")
         | DispatchErr s  => (log Error ("Dispatch error: " ^ s); "Dispatch error")

(* fun respondToMsg client = Socket.output client (evalJson (Socket.inputAll client)) *)
fun respondToMsg client = 
    let val req  = Socket.inputAll client
        val resp = (
            log Debug ("Recieved request: " ^ req);
            evalJson req
        )
     in log Debug ("Sending back response: " ^ resp);
        Socket.output client resp
    end

fun handleIncoming listener =
    let val client = Socket.accept listener
     in log Debug "Incoming connection accepted";
        respondToMsg client;
        Socket.close client
    end
    handle Socket.Err s     => log Error ("Socket failure: " ^ s)
         | Socket.InvalidFD => log Error "Invalid file descriptor"

fun startServer port qLen = 
    (* loop handleIncoming (Socket.listen port qLen) *)
    let val listener = Socket.listen port qLen
     in log Debug "Listener opened";
        loop handleIncoming listener
    end
    handle Socket.Err s => log Error ("Socket failure on listener instantiation: " ^ s)
         | Crypto.Err s => log Error ("Crypto error: " ^ s)
         | _            => log Error "Fatal: unknown error"

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
val _ = Log.setLogDebug Log.noLog
val _ = main ()
