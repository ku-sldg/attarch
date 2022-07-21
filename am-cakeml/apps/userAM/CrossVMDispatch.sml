(* Depends on util, copland, am *)

(* pl -> nsMap -> pl -> copEval *)
fun crossVMDispatch me nsMap pl ev t = 
    let val name = case Map.lookup nsMap pl of 
              Some n => n
            | None => raise DispatchErr ("Place " ^ plToString pl ^ "not in nameserver map")
        val req     = REQ pl me nsMap t ev
        val reqStr  = JsonExtra.toString (requestToJson req)
        val resp = (
        (*
            log Debug ("useram: Writing to " ^ name ^ ": " ^ reqStr);
            *)
            writeDataport name (BString.nullTerminated reqStr);
            emitDataport name;
            (* TODO: make length flexible and/or parameterized *)
            (*   Perhaps the conns table could have length parameter, and a `readAll` function *)
            readDataport name 4096
        )
     in case (jsonToResponse o JsonExtra.fromString o BString.toCString) resp of 
          RES _ _ ev => ev
    end
    handle Json.ERR _ _  => raise DispatchErr "Could not parse Json response"
         | DataportErr s => raise DispatchErr ("Dataport error: " ^ s)

(* pl -> nsMap -> nsMap -> pl -> copEval *)
fun userAMDispatch me crossVMMap netMap pl = 
    let val mapUnion = Map.union crossVMMap netMap
     in if MapExtra.exists crossVMMap pl then 
            crossVMDispatch me mapUnion pl
        else 
            socketDispatch me mapUnion pl
    end

(* key -> nsMap -> nsMap -> am *)
fun userAm privKey crossVMMap netMap = Am
    O
    (userAMDispatch O crossVMMap netMap)
    defaultUsmMap
    privKey
    Crypto.signMsg
    Crypto.hash
