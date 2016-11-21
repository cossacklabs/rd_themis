# RD_Themis v.0.1

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

3. To load the module, start Redis with the `--loadmodule /path/to/module.so` option, add it as a directive to the configuration file or send a `MODULE LOAD` command.

Features
---

**Symmetric container, cget/cset**: Encrypts data with [Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem) symmetric container with context awareness, authenticity and other features.

**Asymmetric container, msget/msset**: One-way asymmetric container, which was born in [Acra](https://cossacklabs.com/acra/)'s development and will emerge in next versions of Themis, it allows any piece of code to: 
1. Encrypt payload with random symmetric key.
2. Store this key in asymmetric envelope, sent from random keypair to desired keypair. 
3. Stack these together with format control. 
It enables you to bind secrecy and authenticity to one private key, while preserving speed benefits of symmetric cryptography over main payload. 

Commands
---

### `rd_themis.cset key password data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with [Themis Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem) in Seal Mode) instead of the plaintext data.

### `rd_themis.cget key password`
Decrypt and return stored data.

### `rd_themis.msset key public_key data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with [Themis Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem) with random key, wrapped in [Themis Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem) with random sender key and fixed decryption key) instead of the clear data.

### `rd_themis.msget key private_key`
Decrypt and return stored data.

Command alternatives, using `RedisModule_BlockClient` API
---

### `rd_themis.csetbl key password data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with [Themis Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem) in Seal Mode) instead of the plaintext data.

### `rd_themis.cgetbl key password`
Decrypt and return stored data.

### `rd_themis.mssetbl key public_key data`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with [Themis Secure Cell](https://github.com/cossacklabs/themis/wiki/Secure-Cell-cryptosystem) with random key, wrapped in [Themis Secure Message](https://github.com/cossacklabs/themis/wiki/Secure-Message-cryptosystem) with random sender key and fixed decryption key) instead of the clear data.

### `rd_themis.msgetbl key private_key`
Decrypt and return stored data.

Examples and use-cases
--- 

In `/examples/`, we've put two simple examples on using Rd_Themis in Python and Ruby. They require corresponding Themis wrappers ([Python package](https://pypi.python.org/pypi/pythemis), [Ruby Gem](https://rubygems.org/gems/rubythemis)) to run.

Rd_Themis is especially beneficial within apps, which use Themis for many things. For example, you may encrypt something in your Python application, store it in Redis via regular `SET` command, then fetch the same blob of data from JS code in environment, which doesn't have Themis port, and use `rd_themis.cget` to decrypt the blob.

