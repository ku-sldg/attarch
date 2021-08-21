(* Depends on: util, copland, am/Measurements, am/ServerAm *)

val userAM : pl = S O
val platAM : pl = S (S O)

val userAMPub = BString.unshow "490E2422528F14AC6A48DDB9D72CB30B8345AF2E939003BC7A33A6057F2FFB0101000000000000002DD0B7F53A560000A049D882A37F00000000000000000000"
val platAMPub = BString.unshow "26930400B922522DDE9B5A0575EB9FA85C5CE1AE280BC7F7D67CD641B195E5D9C82F6D7BA67F000040E482F9E45500000000000000000000A04181F9E4550000"

val goldenHash = BString.unshow "C18DEED4345BA0A2B1A131AE5CC4AD2ED932457E3C9C789D38E7774B0B648AAA5A6868DF326549E70E8D82E0164905BB929FBEA4348D7208454CF48037B92FB0"

val measurement = Asp (Aspc (Id O) [])
val term =
    Att userAM (
        Lseq 
        (Att platAM (
            Lseq 
            (Bpar (NONE, ALL) (Lseq measurement (Asp Hsh)) (Asp Cpy))
            (Asp Sig)))
        (Asp Sig)
    )

(* It seems like the parallel evidence is reversed? *)
fun appraisePlatSubEv expNonce ev = case ev of 
      G platSig (PP (N (Id O) nonce Mt) (H hash)) =>
          if not (Option.valOf (verifySig ev platAMPub)) then 
                        Err "Bad PlatAM signature"
                    else if nonce <> expNonce then 
                        Err "Bad nonce value"
                    else if hash <> goldenHash then 
                        Err "Bad hash value"
                    else 
                        Ok ()
    | _ => Err "Unexpected shape of evidence"

fun appraise expNonce ev = case ev of 
      G userSig ev' => 
          if not (Option.valOf (verifySig ev userAMPub)) then 
              Err "Bad UserAM signature"
          else
              appraisePlatSubEv expNonce ev'
    | _ => Err "Unexpected shape of evidence"

fun sendMap addr = Map.fromList nat_compare [(S O, addr),(S (S O), "/dev/uio0")]

fun sendReq addr =
    let val am    = serverAm BString.empty (sendMap addr)
        val nonce = Random.random (Random.seed (Meas.urand 32)) 16
        val ev    = evalTerm am (N (Id O) nonce Mt) term
     in print ("Evaluating term:\n" ^ termToString term ^ "\n\nNonce:\n" ^
               BString.show nonce ^ "\n\nEvidence recieved:\n" ^
               evToString ev ^ "\n\nAppraisal " ^ (
               case appraise nonce ev of
                      Ok ()   => "succeeded\n"
                    | Err msg => "failed: " ^ msg ^ "\n")
              )
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
