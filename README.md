## About
An architecture for attestation. Features the Copland Attestation Manager and novel introspective measurements over Linux.

## Prerequisites

* make
* curl
* git 
* [repo tool](https://android.googlesource.com/tools/repo)
* [docker engine](https://docs.docker.com/engine/install/)

## How to build

1. `repo init -u https://github.com/ku-sldg/attarch-manifest.git`
2. `repo sync`
2. `./dockerfiles.sh`
3. `./startDocker.sh`
4. `./linux.sh 6.1.y`
5. `python3 ScrapeSystemMap.py attarch >> components/Measurement/configurations/linux_definitions.h`
6. `mkdir build && cd build`
7. `../init-build.sh -DCAMKES_VM_APP=attarch -DPLATFORM=qemu-arm-virt`
8. `ninja`
9. `./simulate`

## Notes

Use username "root" to login to the shell.
The AM in the Linux VM will request measurement right after booting, and the AM in the seL4 thread will respond.
