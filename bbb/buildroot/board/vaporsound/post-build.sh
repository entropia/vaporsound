#!/bin/bash

(cd $BR2_EXTERNAL; git rev-parse HEAD) > ${TARGET_DIR}/etc/firmware-version
