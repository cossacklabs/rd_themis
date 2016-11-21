# Rd_Themis

Redis module for Themis
===

This module provides a simple and secure way to store data in Redis via encrypting/decrypting with various [Themis](https://www.github.com/cossacklabs/themis) primitives. 

It is Apache 2.0 licensed, so you may use it quite freely.

Quick start guide
---

1. Prepare dependencies: 
   - You might want to install [Redis](http://redis.io/) server with support for [modules](http://redismodules.com/) to actually use Rd_Themis module.
   - Rd_Themis needs [Themis](https://www.github.com/cossacklabs/themis) crypto library, included as submodule in `/src/`, builds itself during make.
   - Themis needs [libcrypto.so](https://wiki.openssl.org/index.php/Libcrypto_API) to run, libssl-dev package to build Themis.
2. Build the rd_themis module:

    ```
    git clone --recursive https://github.com/cossacklabs/rd_themis
    cd rd_themis
    make
    ```

3. To load the module, Start Redis with the `--loadmodule /path/to/module.so` option, add it as a directive to the configuration file or send a `MODULE LOAD` command.


Commands
---

### `rd_themis.cset key password data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with themis secure cell in seal mode) instead of the clear data.

### `rd_themis.cget key password`
Decrypt and return stored data.

### `rd_themis.msset key public_key data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted by themis secure message with randomly generated key pair and public_key) instead of the clear data.

### `rd_themis.msget key private_key`
Decrypt and return stored data.

Commands alternatives uses `RedisModule_BlockClient` API
---

### `rd_themis.csetbl key password data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with themis secure cell in seal mode) instead of the clear data.

### `rd_themis.cgetbl key password`
Decrypt and return stored data.

### `rd_themis.mssetbl key public_key data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted by themis secure message with randomly generated key pair and public_key) instead of the clear data.

### `rd_themis.msgetbl key private_key`
Decrypt and return stored data.

Examples and use-cases
--- 

In `/examples/`, we provide two simple examples on using Themis within Python and Ruby. They require corresponding Themis wrappers ([Python package](https://pypi.python.org/pypi/pythemis), [Ruby Gem](https://rubygems.org/gems/rubythemis)) 

Rd_Themis is especially beneficial within apps, which use Themis for many things. For example, you may encrypt something in your Python application, store it in Redis via regular `SET` command, then fetch the same blob of data from JS code in environment, which doesn't have Themis port, and use `rd_themis.cget` to decrypt the blob.

