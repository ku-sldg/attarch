These timing notes are over Linux 5.4

The other files in this directory are raw timing data collections.

Each file is related to zero or more measurements indicated by file name.

The following bash script was used to automate the test procedure described below.

```
#!/bin/sh

export CML\_STACK\_SIZE=10
export CML\_HEAP\_SIZE=10

touch /root/timing\_output.txt
/useram/useram -m /useram/manifest.json -k /useram/private.key -t /useram/term.json

for i in $(seq 1 37)
do
    (time /useram/useram -m /useram/manifest.json -k /useram/private.key -t /useram/term.json) 2>&1 | grep "real" >> timing\_output.txt
done
```

1. Choose which measurement(s) to use and build the seL4 image.
2. Boot the seL4 image and log in to the Linux VM as root.
3. Execute the above script.
4. Collect results from /root/timing\_output.txt

The script includes an initial invocation of the attestation tools because the first invocation after Linux boot takes slightly longer than every subsequent invocation. The script leaves this invocation out as anomalous.
