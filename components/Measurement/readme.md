Be Advised:

This component depends, in an odd way, on the cross\_include directory you see here.

That directory is, from within the docker container used to build the seL4,
     /usr/lib/gcc-cross/aarch64-linux-gnu/10/include

The oddness is that the inclusion is specified in a distant folder:
    ../../am-cakeml/system/crypto/evercrypt/hacl.cmake

It is advantageous to use that file twice rather than duplicate its contents for this component.

However, I dislike that this component's inclusions are not immediately obvious from its own declarations.
