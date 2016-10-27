#!/usr/bin/python3.5

import redis
import os

r = redis.StrictRedis(host='localhost', port=6379, db=0)

#load module
r.execute_command("module load {}/../rd_themis.so".format(os.getcwd()))

#scell set
r.execute_command("rd_themis.scell_seal_set {} {} {}".format("key", "data", "password"))

#scell get
data = r.execute_command("rd_themis.scell_seal_get {} {}".format("key", "password"))

print(data)

#smessage set
r.execute_command("rd_themis.smessage_set {} {} {}".format("key", "data", "\x55\x45\x43\x32\x00\x00\x00\x2d\x6b\xbb\x79\x79\x03\xfa\xb7\x33\x3a\x4d\x6e\xb7\xc2\x59\xde\x78\x96\xfa\x69\xe6\x63\x86\x91\xc2\x65\xa0\x92\xf6\x5a\x22\x3c\xa9\x8e\xc9\xa7\x35\x42"))

#smessage get
data = r.execute_command("rd_themis.smessage_get {} {}".format("key", "\x52\x45\x43\x32\x00\x00\x00\x2d\xc7\xa8\xca\x7a\x00\xc3\xb5\xd1\xad\x51\x37\x30\x8f\x45\xe6\x5e\x54\xdf\x2b\x7a\x45\xbc\x85\x08\xe8\xcc\x3b\xc9\x48\x1b\x63\x1a\xe8\x12\x8b\x39\x74"))

print(data)

#unload module
r.execute_command("module unload rd_themis".format(os.getcwd()))
