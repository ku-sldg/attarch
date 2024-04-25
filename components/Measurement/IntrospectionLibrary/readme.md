The files measurements.c and measurementAndAppraisal.c perform similar functions, but they have different use-cases.

measurements.c performs the introspective measurements and packages the evidence in a particular way to be used with the Copland Attestation Manager (CakeML AM).

On the other hand, measurementAndAppraisal.c can be used when the Copland AM is not used. That is, if instant results are desired and no special bundling or communication is required, then measurementAndAppraisal.c offers a simpler pipeline.
