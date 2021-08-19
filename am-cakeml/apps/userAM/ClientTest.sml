(* Depends on: util, copland, am/Measurements, am/ServerAm *)

(* Just @userAM (@platAM CPY) *)
val term = Att (S O) (Att (S (S O)) (Asp Cpy))

fun sendMap addr = Map.fromList nat_compare [(S O, addr),(S (S O), "/dev/uio0")]

fun sendReq addr =
    let val am    = serverAm BString.empty (sendMap addr)
        val nonce = Random.random (Random.seed (Meas.urand 32)) 16
        val ev    = evalTerm am (N (Id O) nonce Mt) term
     in print ("Evaluating term:\n" ^ termToString term ^ "\n\nNonce:\n" ^
               BString.show nonce ^ "\n\nEvidence recieved:\n" ^
               evToString ev ^ "\n")
    end
    handle Socket.Err s     => log Error ("Socket failure on connection: " ^ s)
         | Socket.InvalidFD => log Error "Invalid file descriptor"
         | DispatchErr s    => log Error ("Dispatch error: " ^ s)
         | _                => log Error "Fatal: unknown error"

fun main () =
    let val name  = CommandLine.name ()
        val usage = "Usage: " ^ name ^ " address\n"
                  ^ "e.g.   " ^ name ^ " 127.0.0.1\n"
     in case CommandLine.arguments () of
              [addr] => sendReq addr
            | _ => TextIO.print_err usage
     end
val _ = Log.setLogDebug Log.noLog
val _ = main ()
