/* Hash digests of known kernel modules
 * Michael Neises
 * 11 July 2022
 */

int numKnownDigests = 2;

const char connectionModule[] = "161A1809914905D7786E3528E48EED1A7C0BAF4BB241FE43BBDAE47730F5020713963E9F09DB77145897A7C9A5C3C5688D895480B60313548E59791ECCFA80D6";

const char poisonModule[] = "B0B55CEDA336202E6377A7FA84BCAA71846D9AC74E7D94B205224E52A2690B108647D893058CB9549690E7AAC5A9C62216AFA04689F50725366951990203B22C";
                             
void HexToByteString(const char* input_digest, uint8_t* output_digest)
{
    int value;
    for(int i=0; i < 64 && sscanf(input_digest + i * 2, "%2x", &value); i++)
    {
        output_digest[i] = value;
    }
}

void GetKnownModuleDigests(uint8_t** digests)
{
    HexToByteString(&connectionModule, digests[0]);
    HexToByteString(&poisonModule, digests[1]);
}

