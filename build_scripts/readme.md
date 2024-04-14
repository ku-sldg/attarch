How to do:

1. Run dockerfiles.sh to add dependencies
2. Run startDocker.sh to enter a docker container
3. Run linux.sh to shallow-clone and build the Linux kernel
4. To collect Linux build values necessary for introspection, run
   `python3 ScrapeSystemMap.py attarch/linux-stable >> components/Measurement/configurations/linux_definitions.h`
5. Build like a normal seL4 project (init-build and ninja)
