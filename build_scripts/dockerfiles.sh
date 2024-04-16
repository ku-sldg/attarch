#!/bin/bash
# Append extras.Dockerfile with commands to fetch CakeML for the Copland AM, and 
# flex & bison for the Linux kernel.
cat <<EOF >> seL4-CAmkES-L4v-dockerfiles/dockerfiles/extras.Dockerfile
RUN apt-get install -y --no-install-recommends \\
    flex \\
    bison

RUN curl -L https://github.com/CakeML/cakeml/releases/download/v2076/cake-x64-32.tar.gz > cake-x64-32.tar.gz \\
    && tar -xvzf cake-x64-32.tar.gz && cd cake-x64-32 && make cake \\
    && mv cake /usr/bin/cake32

RUN curl -L https://github.com/CakeML/cakeml/releases/download/v2076/cake-x64-64.tar.gz > cake-x64-64.tar.gz \\
    && tar -xvzf cake-x64-64.tar.gz && cd cake-x64-64 && make cake \\
    && mv cake /usr/bin/cake64
EOF
