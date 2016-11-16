/*
* Copyright (c) 2016 Cossack Labs Limited
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "redismodule.h"

#include <string.h>
#include <themis/themis.h>

static int cmd_scell_seal_encrypt(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply *reply;
    if (argc != 4) {
        RedisModule_WrongArity(ctx);
        return REDISMODULE_ERR;
    }

    size_t encrypted_data_len=0, pass_len=0, message_len=0;
    const char *pass = RedisModule_StringPtrLen(argv[3], &pass_len);
    const char *message = RedisModule_StringPtrLen(argv[2], &message_len);
    if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, NULL, &encrypted_data_len)){
      RedisModule_ReplyWithError(ctx, "ERR secure seal encryption failed (length determination)");
      return REDISMODULE_ERR;      
    }
    uint8_t *encrypted_data = malloc(encrypted_data_len);
    if(!encrypted_data){
      RedisModule_ReplyWithError(ctx, "ERR memory allocation error");
      return REDISMODULE_ERR;      
    }
    themis_status_t res = themis_secure_cell_encrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, encrypted_data, &encrypted_data_len);
    if(THEMIS_SUCCESS!=res){
      free(encrypted_data);
      RedisModule_ReplyWithError(ctx, "ERR secure seal encryption failed");
      return REDISMODULE_ERR;      
    }
    reply = RedisModule_Call(ctx, "SET", "sb!", argv[1], encrypted_data, encrypted_data_len);
    RedisModule_ReplyWithCallReply(ctx, reply);
    free(encrypted_data);
    return REDISMODULE_OK;
}

static int cmd_scell_seal_decrypt(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply *reply;
    if (argc != 3) {
        RedisModule_WrongArity(ctx);
        return REDISMODULE_OK;
    }

    reply = RedisModule_Call(ctx, "GET", "s", argv[1]);
    if(REDISMODULE_REPLY_NULL == RedisModule_CallReplyType(reply)){
      RedisModule_ReplyWithLongLong(ctx, 0);
      return REDISMODULE_OK;
    }

    if (REDISMODULE_REPLY_STRING != RedisModule_CallReplyType(reply)) {
      RedisModule_ReplyWithError(ctx, "WRONGTYPE Operation against a key holding the wrong kind of value");
      return REDISMODULE_ERR;      
    }

    size_t decrypted_data_len=0, pass_len=0, message_len=0;
    const char *pass = RedisModule_StringPtrLen(argv[2], &pass_len);
    const char *message = RedisModule_CallReplyStringPtr(reply, &message_len);
    if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_decrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, NULL, &decrypted_data_len)){
      RedisModule_ReplyWithError(ctx, "ERR secure seal decryption failed (length determination)");
      return REDISMODULE_ERR;      
    }
    uint8_t *decrypted_data = malloc(decrypted_data_len);
    if(!decrypted_data){
      RedisModule_ReplyWithError(ctx, "ERR memory allocation error");
      return REDISMODULE_ERR;      
    }
    if(THEMIS_SUCCESS!=themis_secure_cell_decrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, decrypted_data, &decrypted_data_len)){
      free(decrypted_data);
      RedisModule_ReplyWithError(ctx, "ERR secure seal decryption failed");
      return REDISMODULE_ERR;      
    }
    RedisModule_ReplyWithStringBuffer(ctx, (const char*)decrypted_data, decrypted_data_len);
    free(decrypted_data);
    return REDISMODULE_OK;
}

//encrypt data with acra ctruct 
static int smessage_encrypt(const uint8_t* data, const uint32_t data_length, const uint8_t* private_key, const uint32_t private_key_length, const uint8_t* public_key, const uint32_t public_key_length, const uint8_t* peer_public_key, const uint32_t peer_public_key_length, uint8_t** enc_data, uint32_t* enc_data_length){
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_message_wrap(private_key, private_key_length, peer_public_key, peer_public_key_length, data, data_length, NULL, (size_t*)enc_data_length)){
    return -1;
  }
  *enc_data = malloc(public_key_length+sizeof(uint32_t)+*enc_data_length);
  if(!(*enc_data)){
    return -2;
  }
  memcpy(*enc_data, &public_key_length, sizeof(public_key_length));
  memcpy(*enc_data+sizeof(public_key_length), public_key, public_key_length);
  if(THEMIS_SUCCESS!=themis_secure_message_wrap(private_key, private_key_length, peer_public_key, peer_public_key_length, data, data_length, *enc_data+sizeof(public_key_length)+public_key_length, (size_t*)enc_data_length)){
    free(*enc_data);
    return -1;
  }
  *enc_data_length+=sizeof(public_key_length)+public_key_length;
  return 0;
}

//decrypt  acra structed data 
static int smessage_decrypt(const uint8_t* data, const uint32_t data_length, const uint8_t* private_key, const uint32_t private_key_length, uint8_t** dec_data, uint32_t* dec_data_length){
  const uint32_t public_key_length = *((const uint32_t*)data);
  if(data_length<=public_key_length+sizeof(uint32_t)){
    return -1;
  }
  const uint8_t* public_key = data+sizeof(uint32_t);
  const uint8_t* data_ = public_key+public_key_length;
  size_t data_length_ = data_length-public_key_length-sizeof(uint32_t); 
  
  if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_message_unwrap(private_key, private_key_length, public_key, public_key_length, data_, data_length_, NULL, (size_t*)dec_data_length)){
    return -1;
  }
  *dec_data = malloc(*dec_data_length);
  if(!(*dec_data)){
    return -2;
  }
  if(THEMIS_SUCCESS!=themis_secure_message_unwrap(private_key, private_key_length, public_key, public_key_length, data_, data_length_, *dec_data, (size_t*)dec_data_length)){
    free(*dec_data);
    return -1;
  }
  return 0;
}

static int smessage_enc(const uint8_t* public_key, const uint32_t public_key_length, const uint8_t* data, const uint8_t data_length, uint8_t** enc_data, uint32_t* enc_data_length){
  uint8_t new_private_key[10240];
  uint8_t new_public_key[10240];

  size_t new_private_key_length=10240, new_public_key_length=10240;
  if(THEMIS_SUCCESS!=themis_gen_ec_key_pair(new_private_key, &new_private_key_length, new_public_key, &new_public_key_length)){
    return -1;
  }
  return smessage_encrypt(data, data_length, new_private_key, new_private_key_length, new_public_key, new_public_key_length, public_key, public_key_length, enc_data, enc_data_length);
}

static int smessage_dec(const uint8_t* private_key, const uint32_t private_key_length, const uint8_t* data, const uint8_t data_length, uint8_t** dec_data, uint32_t* dec_data_length){
  return smessage_decrypt(data, data_length, private_key, private_key_length, dec_data, dec_data_length);
}

static int cmd_smessage_encrypt(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply *reply;
    if (argc != 4) {
        RedisModule_WrongArity(ctx);
        return REDISMODULE_OK;
    }

    size_t encrypted_data_len=0, public_key_len=0, message_len=0;
    const char *public_key = RedisModule_StringPtrLen(argv[3], &public_key_len);
    const char *message = RedisModule_StringPtrLen(argv[2], &message_len);
    uint8_t *encrypted_data=NULL;
    if(0 != smessage_enc((const uint8_t*)public_key, public_key_len, (const uint8_t*)message, message_len, &encrypted_data, (uint32_t*)&encrypted_data_len)){
      RedisModule_ReplyWithError(ctx, "ERR secure message encryption failed");
      return REDISMODULE_ERR;      
    }
    reply = RedisModule_Call(ctx, "SET", "sb!", argv[1], encrypted_data, encrypted_data_len);
    RedisModule_ReplyWithCallReply(ctx, reply);
    free(encrypted_data);
    return REDISMODULE_OK;
}

static int cmd_smessage_decrypt(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply *reply;
    if (argc != 3) {
        RedisModule_WrongArity(ctx);
        return REDISMODULE_OK;
    }

    reply = RedisModule_Call(ctx, "GET", "s", argv[1]);
    if(REDISMODULE_REPLY_NULL == RedisModule_CallReplyType(reply)){
      RedisModule_ReplyWithLongLong(ctx, 0);
      return REDISMODULE_OK;
    }

    if (REDISMODULE_REPLY_STRING != RedisModule_CallReplyType(reply)) {
      RedisModule_ReplyWithError(ctx, "WRONGTYPE Operation against a key holding the wrong kind of value");
      return REDISMODULE_ERR;      
    }

    size_t decrypted_data_len=0, private_key_len=0, message_len=0;
    const char *private_key = RedisModule_StringPtrLen(argv[2], &private_key_len);
    const char *message = RedisModule_CallReplyStringPtr(reply, &message_len);
    uint8_t *decrypted_data = NULL;    
    if(0 != smessage_dec((const uint8_t*)private_key, private_key_len, (const uint8_t*)message, message_len, &decrypted_data, (uint32_t*)&decrypted_data_len)){
      RedisModule_ReplyWithError(ctx, "ERR secure message encryption failed");
      return REDISMODULE_ERR;      
    }
    RedisModule_ReplyWithStringBuffer(ctx, (const char*)decrypted_data, decrypted_data_len);
    free(decrypted_data);
    return REDISMODULE_OK;
}


int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx, "rd_themis", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rd_themis.scell_seal_set", cmd_scell_seal_encrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rd_themis.scell_seal_get", cmd_scell_seal_decrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rd_themis.smessage_set", cmd_smessage_encrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
      return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "rd_themis.smessage_get", cmd_smessage_decrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
      return REDISMODULE_ERR;
    return REDISMODULE_OK;
}
