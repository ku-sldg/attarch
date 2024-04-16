#!/bin/bash
THIS_DIR=`pwd`
cd seL4-CAmkES-L4v-dockerfiles &&
make user HOST_DIR=$THIS_DIR
