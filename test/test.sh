#
# Copyright (c) 2016 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#!/bin/bash

testRedisAlive() {
    res=`redis-cli ping`
    assertEquals "PONG" "$res"
}

testLoadModule() {
    curdir=`pwd`
    res=`redis-cli module load ${curdir}/rd_themis.so`
    assertEquals "OK" "$res"
}

testThemisSealSet() {
    res=`redis-cli rd_themis.cset test_key test_password test_data`
    assertEquals "OK" "$res"
}

testThemisSealGet() {
    res=`redis-cli rd_themis.cget test_key test_password`
    assertEquals "test_data" "$res"
}

testThemisMessageSet() {
    key=`echo -ne '\x55\x45\x43\x32\x00\x00\x00\x2d\x6b\xbb\x79\x79\x03\xfa\xb7\x33\x3a\x4d\x6e\xb7\xc2\x59\xde\x78\x96\xfa\x69\xe6\x63\x86\x91\xc2\x65\xa0\x92\xf6\x5a\x22\x3c\xa9\x8e\xc9\xa7\x35\x42'`
    res=`redis-cli -x "rd_themis.msset test_key $key test_data"`
    assertEquals "OK" "$res"
}

testThemisMessageGet() {
    res=`echo -ne '\x52\x45\x43\x32\x00\x00\x00\x2d\xc7\xa8\xca\x7a\x00\xc3\xb5\xd1\xad\x51\x37\x30\x8f\x45\xe6\x5e\x54\xdf\x2b\x7a\x45\xbc\x85\x08\xe8\xcc\x3b\xc9\x48\x1b\x63\x1a\xe8\x12\x8b\x39\x74' | redis-cli -x rd_themis.msget test_key `
    assertEquals "test_data" "$res"
}


testUnloadModule() {
    curdir=`pwd`
    res=`redis-cli module unload rd_themis`
    assertEquals "OK" "$res"
}

. shunit2
