#!/bin/bash

(cd $BR2_EXTERNAL; git show-ref -s HEAD) > ${TARGET_DIR}/etc/firmware-version
