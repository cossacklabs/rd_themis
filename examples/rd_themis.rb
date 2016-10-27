#!/usr/local/bin/ruby

require "redis"

redis = Redis.new(:host => "127.0.0.1", :port => 6379, :db => 0)

# load module 
redis.call([:module, "load", Dir.pwd+"/../rd_themis.so"])

#scell set
redis.call([:"rd_themis.scell_seal_set", "key", "data", "password"])

#scell get
data = redis.call([:"rd_themis.scell_seal_get", "key", "password"])

p data

#smessage set
redis.call([:"rd_themis.smessage_set", "key", "data", "\x55\x45\x43\x32\x00\x00\x00\x2d\x6b\xbb\x79\x79\x03\xfa\xb7\x33\x3a\x4d\x6e\xb7\xc2\x59\xde\x78\x96\xfa\x69\xe6\x63\x86\x91\xc2\x65\xa0\x92\xf6\x5a\x22\x3c\xa9\x8e\xc9\xa7\x35\x42"])

#smessage get
data = redis.call([:"rd_themis.smessage_get", "key", "\x52\x45\x43\x32\x00\x00\x00\x2d\xc7\xa8\xca\x7a\x00\xc3\xb5\xd1\xad\x51\x37\x30\x8f\x45\xe6\x5e\x54\xdf\x2b\x7a\x45\xbc\x85\x08\xe8\xcc\x3b\xc9\x48\x1b\x63\x1a\xe8\x12\x8b\x39\x74"])

p data

# unload module
redis.call([:module, "unload",  "rd_themis"])