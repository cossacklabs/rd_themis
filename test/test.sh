#!/usr/bin/shunit2

testRedisAlive() {
    res=`redis-cli ping`
    assertEquals "PONG" "$res"
}

testLoadModule() {
    curdir=`pwd`
    res=`redis-cli module load ${curdir}/themis_module.so`
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

testUnloadModule() {
    curdir=`pwd`
    res=`redis-cli module unload themis`
    assertEquals "OK" "$res"
}
