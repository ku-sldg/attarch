/* Hash digests of known kernel modules
 * Michael Neises
 * 11 July 2022
 */

int numKnownDigests = 1;

const char connectionModule[] = "161A1809914905D7786E3528E48EED1A7C0BAF4BB241FE43BBDAE47730F5020713963E9F09DB77145897A7C9A5C3C5688D895480B60313548E59791ECCFA80D6";

//const char poisonModule[] = "E0EDDBDDA7DEA7E05A80150E4DF72269F0974ACE3AD1C0AE59204DA96BE3194F507F1662BA0BA0F8495C27CA6BAFFC300FCA043B2D3147A917DBAE180B052A95";

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
    //HexToByteString(&poisonModule, digests[1]);
}

