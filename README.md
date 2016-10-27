# rd_themis

Redis module for Themis
===

This module provides a simple and secure way to store data in Redis
and later get them.

Quick start guide
---

1. Build a Redis server with support for modules.
2. Build the themis module: `make`
3. To load the module, Start Redis with the `--loadmodule /path/to/module.so` option, add it as a directive to the configuration file or send a `MODULE LOAD` command.


Commands
---

### `themis.scell_seal_set key data password`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted with themis secure cell in seal mode) instead of the clear data.

### `themis.scell_seal_get key password`
Decrypt and return stored data.

### `themis.smessage_set key data public_key`
Works like the standard Redis `SET` command, but stores the encrypted data (encrypted by themis secure message with randomly generated key pair and public_key) instead of the clear data.

### `themis.smessage_get key private_key`
Decrypt and return stored data.
