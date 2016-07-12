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

source /data/src/docker/base/scripts/global.sh

ENABLE_CLANG_FORMATTER='OFF'

function usage
{
    echo "usage: cpp-clean-build.sh [--jobs X --enableclangformatter ON|OFF]"
    echo "default jobs is $JOBS"
}

while [ "$1" != "" ]; do
    case $1 in
        --target )               shift
                                 TARGET=$1
                                 ;;
        --jobs )                 shift
                                 JOBS=$1
                                 ;;
        --enableclangformatter ) ENABLE_CLANG_FORMATTER='ON'
                                 ;;
        * )                      usage
                                 exit 1
    esac
    shift
done

log "CPP CLEAN BUILD TARGET: $TARGET JOBS: $JOBS"

log "Enable core dumps"
ulimit -c unlimited

START=$(date +%s)

log "ENVIRONMENT"
env

log "CLEAN BUILD DIRECTORY"
rm -rf ~/.cmake/packages
rm -rf /data/build/muesli
mkdir /data/build/muesli

cd /data/build/muesli

log "RUN CMAKE"

# fail on first error
set -e
cmake -DCMAKE_BUILD_TYPE=Debug /data/src \
      -DENABLE_CLANG_FORMATTER=$ENABLE_CLANG_FORMATTER \
      -DCMAKE_INSTALL_SYSCONFDIR=/etc \
      -DCMAKE_INSTALL_PREFIX=/usr

log "BUILD C++ MUESLI"
make -j $JOBS

END=$(date +%s)
DIFF=$(( $END - $START ))
log "C++ build time: $DIFF seconds"
