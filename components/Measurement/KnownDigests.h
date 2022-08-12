/* Hash digests of known kernel modules
 * Michael Neises
 * 11 July 2022
 */

int numKnownDigests = 2;

const char connectionModule[] = "161A1809914905D7786E3528E48EED1A7C0BAF4BB241FE43BBDAE47730F5020713963E9F09DB77145897A7C9A5C3C5688D895480B60313548E59791ECCFA80D6";

const char poisonModule[] = "B0B55CEDA336202E6377A7FA84BCAA71846D9AC74E7D94B205224E52A2690B108647D893058CB9549690E7AAC5A9C62216AFA04689F50725366951990203B22C";

const char glibc_elf_dl_load[] = "80D006FED2956D2DBAFA2487F662BF53458384D22DC08FA0713E91AFE8E56B0E68658C2FE662744F5088B388E17D9213A21E5109769A994EF0F71427997FEA63";

const char useram[] = "3F6767BB02AD576DCB1BB2170AE7EF8F75FBEC92EA10DCD578BED0DF968C4EF63C8F83C21CE57C7441BF3B83F93A9B7BB736ED23A5D6AB1F4232CD0B884F822C";
                             
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

void GetKnownTaskRodataDigests(uint8_t** digests)
{
    HexToByteString(&glibc_elf_dl_load, digests[0]);
    HexToByteString(&useram, digests[1]);
}

