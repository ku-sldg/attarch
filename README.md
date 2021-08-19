## About
An architecture for attestation. Features two Copland Attestation Managers (AMs): one in the VM, and one at the CAmkES level.

## Checking out the source
See https://github.com/gjurgensen/attarch-manifest

## Building
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
make user <attarch-src-base-dir>
```

Finally, you can build the project:
```sh
mkdir build && cd build
../init-script.sh -DCAMKES_VM_APP=attarch -DPLATFORM=qemu-arm-virt
ninja
```

This builds a qemu image, which you can run from the build directory with `./simulate`.