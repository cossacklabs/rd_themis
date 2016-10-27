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
    res=`redis-cli themis.scell_seal_set test_key test_data test_password`
    assertEquals "OK" "$res"
}

testThemisSealGet() {
    res=`redis-cli themis.scell_seal_get test_key test_password`
    assertEquals "test_data" "$res"
}

testThemisMessageSet() {
    res=`echo -ne '\x55\x45\x43\x32\x00\x00\x00\x2d\x6b\xbb\x79\x79\x03\xfa\xb7\x33\x3a\x4d\x6e\xb7\xc2\x59\xde\x78\x96\xfa\x69\xe6\x63\x86\x91\xc2\x65\xa0\x92\xf6\x5a\x22\x3c\xa9\x8e\xc9\xa7\x35\x42' | redis-cli -x themis.smessage_set test_key test_data`
    assertEquals "OK" "$res"
}

testThemisMessageGet() {
    res=`echo -ne '\x52\x45\x43\x32\x00\x00\x00\x2d\xc7\xa8\xca\x7a\x00\xc3\xb5\xd1\xad\x51\x37\x30\x8f\x45\xe6\x5e\x54\xdf\x2b\x7a\x45\xbc\x85\x08\xe8\xcc\x3b\xc9\x48\x1b\x63\x1a\xe8\x12\x8b\x39\x74' | redis-cli -x themis.smessage_get test_key `
    assertEquals "test_data" "$res"
}


testUnloadModule() {
    curdir=`pwd`
    res=`redis-cli module unload themis`
    assertEquals "OK" "$res"
}

. shunit2