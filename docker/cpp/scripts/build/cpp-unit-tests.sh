#!/bin/bash
###
# #%L
# %%
# Copyright (C) 2016 BMW Car IT GmbH
# %%
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# #L%
###

START=$(date +%s)

source /data/src/docker/base/scripts/global.sh

log "ENVIRONMENT"
env

set -e

cd /data/build/muesli/bin
rm -f *.junit.xml

./muesli-unit-test --gtest_shuffle --gtest_output="xml:muesli-unit-test.junit.xml"

END=$(date +%s)
DIFF=$(( $END - $START ))
log "C++ muesli unit tests time: $DIFF seconds"
