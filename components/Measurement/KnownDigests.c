/* Previously registered sha512 digests
 * Michael Neises
 * 11 July 2022
 */

#include "KnownDigests.h"

const char Task_useram[] = "D8330BFBF39937EB621AD225D4F3BF29A600863E98BDE249C7CF7311277DB56DEB4C2BB4784C77018C7D89EF65B2739A86DAAA77C4446F836BFE8BD7A6FF87C8";
const char Task_useramclienttest[] = "B4CD84F32613D338A9D7BC36ABD174ABA807057EF286632952F6102D5644E75B3A16E787E5BEA4641907374B411A489A3C573E2A8433E724364E5932A1FE5913";

const char connection[] = "8E62C8B56560E3C23C28B12F5CD8BA78647FBBFFF522A9E46C2A7041E460DA7D7A103A96D98D95953FEE8E4254614A329F0651BE88BD3C28571138A645E02147";
const char poison[] = "AE8E8DEFB952BC12C3259E2910577864F95DCC0FD82E481A22FC8DF5AD2971514777B97807EFAF8DDE576B916B73E7A8F90ECC0ED52A824BD44436E276E6E069";
const char Task_common[] = "32A832B0E77F0116389BFC07A6FCE3D253FB488408892D93A431EEAF3479C8DBD6C26B092003279768FBCE977D10D56A75784DBECC919524F66872A7FB488560";
const char introspect[] = "6FED3D67BB74B77729C78BA79910D99732974B9145A9C93EBE457040FFC19485FE7CF4D9326DCFF098D9B68580813E2F1023DAEA2B5B3A33DF3DA620D94BE65D";
const char KernelRodata[] = "D11343746B1EFB12D11DD6688CB91E033B6475506B72E4A1F6E18A12CDFEB6497E554E188DE70BE042F1C82106E68FAB461D9288B7DF3A21A35C34BF2D34FF41";

void HexToByteString(const char* input_digest, uint8_t* output_digest)
{
    int value;
    for(int i=0; i < DIGEST_NUM_BYTES && sscanf(input_digest + i * 2, "%2x", &value); i++)
    {
        output_digest[i] = value;
    }
}

int GetKnownDigests(uint8_t* digests)
{
    int numDigests = 0;
    HexToByteString(&Task_useram, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    HexToByteString(&Task_useramclienttest, &digests[DIGEST_NUM_BYTES*(numDigests++)]);

    HexToByteString(&connection, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    HexToByteString(&poison, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    HexToByteString(&Task_common, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    HexToByteString(&introspect, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    HexToByteString(&KernelRodata, &digests[DIGEST_NUM_BYTES*(numDigests++)]);
    return numDigests;
}

bool IsThisAKnownDigest(uint8_t* input_digest)
{
    uint8_t* known = calloc(INTRO_NUM_DIGESTS, DIGEST_NUM_BYTES);
    int numKnownDigests = GetKnownDigests(known);
    bool isKnown = false;
    for(int i=0; i < numKnownDigests; i++)
    {
        isKnown = true;
        for(int j=0; j<DIGEST_NUM_BYTES; j++)
        {
            if(known[i*DIGEST_NUM_BYTES + j] != input_digest[j])
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

void RenderDigestDeclaration(char* name, uint8_t* digest)
{
    printf("\tconst char %s[] = \"", name);
    PrintDigest(digest);        
    printf("\";\n");
    printf("\tHexToByteString(&%s, &digests[DIGEST_NUM_BYTES*(numDigests++)]);\n", name);
}



