
/* These digest declarations are generated at runtime */
/* These are left here as an example of where to place the generated syntax */

int register_digests(uint8_t (*digests)[INTRO_NUM_DIGESTS * DIGEST_NUM_BYTES]) {
    int numDigests = 0;

	const char KernelRodata[] = "C8499FAA34EF56C1003F0C884468B6A75EB7429E2D64BAD693F2918D887848C4D64E98C190CD93E048D39108D66100B7842F7928D6D5F57DB83AAAF83B714218";
	HexToByteString(&KernelRodata, &(((uint8_t*)digests)[DIGEST_NUM_BYTES*(numDigests++)]));

	const char KernelSystemCallTable[] = "B6B7C22DF65A7FD373929EE4198F80305BD4D4B3144CBCBBAAA8666F33C4B7A76FC571AEFB6FB1772AD42C457CA916E9337A1A65F8C1EA56FCC2F9275489AC56";
	HexToByteString(&KernelSystemCallTable, &(((uint8_t*)digests)[DIGEST_NUM_BYTES*(numDigests++)]));

	const char poison[] = "1ADFA120EBA54BD4F62CF2E610E6ECCCAAD1DDE37A61D8154891FFF6B21A5714F49E59088BF903892BF5AB7B644CB33F2CAD61AD2F3B5CE470F7271A1C37977D";
	HexToByteString(&poison, &(((uint8_t*)digests)[DIGEST_NUM_BYTES*(numDigests++)]));

	const char connection[] = "5429E7F837D841A3B7478D78C8FD86A04721883A71BC0D0F0FABB01D4DAD1C0DB704F67C102E25AD03E362A617BA1AF7596CEFABACDCC77F5B30671DCF1C56A8";
	HexToByteString(&connection, &(((uint8_t*)digests)[DIGEST_NUM_BYTES*(numDigests++)]));

    return numDigests;
}

/* What follows is output of a successful appraisal on all measurements */

/* Appraising ASP with ID:  cm_aspid */
/* DEBUG: here's your measurement id: 0 */
/* Type: Rodata */
/* Name: KernelRodata */
/* Digest: C8499FAA34EF56C1003F0C884468B6A75EB7429E2D64BAD693F2918D887848C4D64E98C190CD93E048D39108D66100B7842F7928D6D5F57DB83AAAF83B714218 */
/* Digest Recognized. */

/* Type: Rodata */
/* Name: KernelSystemCallTable */
/* Digest: B6B7C22DF65A7FD373929EE4198F80305BD4D4B3144CBCBBAAA8666F33C4B7A76FC571AEFB6FB1772AD42C457CA916E9337A1A65F8C1EA56FCC2F9275489AC56 */
/* Digest Recognized. */

/* Type: Module */
/* Name: poison */
/* Digest: 1ADFA120EBA54BD4F62CF2E610E6ECCCAAD1DDE37A61D8154891FFF6B21A5714F49E59088BF903892BF5AB7B644CB33F2CAD61AD2F3B5CE470F7271A1C37977D */
/* Digest Recognized. */

/* Type: Module */
/* Name: connection */
/* Digest: 5429E7F837D841A3B7478D78C8FD86A04721883A71BC0D0F0FABB01D4DAD1C0DB704F67C102E25AD03E362A617BA1AF7596CEFABACDCC77F5B30671DCF1C56A8 */
/* Digest Recognized. */

/* Appraisal Result: PASS: All Evidence Digests Recognized */
/* PASSED */

