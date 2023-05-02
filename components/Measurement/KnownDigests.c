/* Previously registered sha512 digests
 * Michael Neises
 * 11 July 2022
 */

#include "KnownDigests.h"
#define INTRO_NUM_DIGESTS 100
    /* Allow for a hundred digests, I guess */
    /* We like this strategy because it makes it easy to add new digests */
    /* But we don't like that this value is hard-coded */ 

const char Task_common[] = "32A832B0E77F0116389BFC07A6FCE3D253FB488408892D93A431EEAF3479C8DBD6C26B092003279768FBCE977D10D56A75784DBECC919524F66872A7FB488560";
const char Task_useram[] = "D8330BFBF39937EB621AD225D4F3BF29A600863E98BDE249C7CF7311277DB56DEB4C2BB4784C77018C7D89EF65B2739A86DAAA77C4446F836BFE8BD7A6FF87C8";
const char Task_useramclienttest[] = "B4CD84F32613D338A9D7BC36ABD174ABA807057EF286632952F6102D5644E75B3A16E787E5BEA4641907374B411A489A3C573E2A8433E724364E5932A1FE5913";

const char KernelRodata[] = "4DC685FDC9556E62A0175353E8B042E896150B18DE096D20B3C372C85A8CE30168C0F8B423DF5F0A094124A13501C5897BD10A430882BCFDF051E1FB71715CD2";


const char connection[] = "8E62C8B56560E3C23C28B12F5CD8BA78647FBBFFF522A9E46C2A7041E460DA7D7A103A96D98D95953FEE8E4254614A329F0651BE88BD3C28571138A645E02147";
const char poison[] = "AE8E8DEFB952BC12C3259E2910577864F95DCC0FD82E481A22FC8DF5AD2971514777B97807EFAF8DDE576B916B73E7A8F90ECC0ED52A824BD44436E276E6E069";

void HexToByteString(const char* input_digest, uint8_t* output_digest)
{
    int value;
    for(int i=0; i < 64 && sscanf(input_digest + i * 2, "%2x", &value); i++)
    {
        output_digest[i] = value;
    }
}

int GetKnownDigests(uint8_t* digests)
{
    int numDigests = 0;
    HexToByteString(&Task_common, &digests[64*(numDigests++)]);
    HexToByteString(&Task_useram, &digests[64*(numDigests++)]);
    HexToByteString(&Task_useramclienttest, &digests[64*(numDigests++)]);
    HexToByteString(&KernelRodata, &digests[64*(numDigests++)]);

    HexToByteString(&connection, &digests[64*(numDigests++)]);
    HexToByteString(&poison, &digests[64*(numDigests++)]);
    return numDigests;
}

bool IsThisAKnownDigest(uint8_t* input_digest)
{
    uint8_t* known = calloc(INTRO_NUM_DIGESTS, 64);
    int numKnownDigests = GetKnownDigests(known);
    bool isKnown = false;
    for(int i=0; i < numKnownDigests; i++)
    {
        isKnown = true;
        for(int j=0; j<64; j++)
        {
            if(known[i*64 + j] != input_digest[j])
            {
                isKnown = false;
                break;
            }
        }
        if(isKnown)
        {
            free(known);
            return true;
        }
    }
    free(known);
    return false;
}


