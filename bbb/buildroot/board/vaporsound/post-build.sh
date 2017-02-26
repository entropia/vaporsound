#!/bin/bash

(cd $BR2_EXTERNAL_Vaporsound_PATH; git rev-parse HEAD) > ${TARGET_DIR}/etc/firmware-version
