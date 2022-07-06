## About
An architecture for attestation. Features two Copland Attestation Managers (AMs): one in the VM, and one at the CAmkES level.

## Prerequisites
First, you need the [seL4 docker scripts](https://docs.sel4.systems/projects/dockerfiles/).

You will need to add the following lines to `dockerfiles/extra.DockerFile`:
```sh
RUN curl https://cakeml.org/regression/artefacts/1241/cake-x64-32.tar.gz > cake-x64-64.tar.gz \
     && tar -xvzf cake-x64-32.tar.gz && cd cake-x64-32 && make cake \
     && mv cake /usr/bin/cake32

RUN curl https://cakeml.org/regression/artefacts/1241/cake-x64-64.tar.gz > cake-x64-64.tar.gz \
     && tar -xvzf cake-x64-64.tar.gz && cd cake-x64-64 && make cake \
     && mv cake /usr/bin/cake64
```

This downloads an older version of the CakeML compiler. Now, launch the docker script:
```sh
make user HOST_DIR=<attarch-src-base-dir>
```

You will also need the python ordered-set module.
```sh
pip3 install ordered-set
```

## Checking out the source and Building
`repo init -u https://github.com/ku-sldg/attarch-manifest.git -b measurement_integration`
`repo sync`
`cd attarch && ./buildLinux.sh`

## Building

Finally, you can build the project:
```sh
mkdir build && cd build
../init-build.sh -DCAMKES_VM_APP=attarch -DPLATFORM=qemu-arm-virt
ninja
```

This builds a qemu image, which you can run from the build directory with `./simulate`.

## Running

Use username "root" to login to the shell".

The useram will have started automatically, listening on port 5000. You may test it by running `useramClientTest 127.0.0.1`.

## Issues

The useram and test client don't seem to re-build with `ninja`. You must do `ninja clean && ninja`.
