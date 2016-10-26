#include "redismodule.h"

#include <string.h>
#include <themis/themis.h>

static int cmd_scell_seal_encrypt(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply *reply;
    if (argc != 4) {
        RedisModule_WrongArity(ctx);
        return REDISMODULE_OK;
    }

    size_t encrypted_data_len=0, pass_len=0, message_len=0;
    const char *pass = RedisModule_StringPtrLen(argv[3], &pass_len);
    const char *message = RedisModule_StringPtrLen(argv[2], &message_len);
    if(THEMIS_BUFFER_TOO_SMALL!=themis_secure_cell_encrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, NULL, &encrypted_data_len)){
      RedisModule_ReplyWithError(ctx, "ERR secure seal encription failed (length determination)");
      return REDISMODULE_ERR;      
    }
    uint8_t *encrypted_data = malloc(encrypted_data_len);
    if(!encrypted_data){
      RedisModule_ReplyWithError(ctx, "ERR memory allocation error");
      return REDISMODULE_ERR;      
    }
    themis_status_t res = themis_secure_cell_encrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, encrypted_data, &encrypted_data_len);
    fprintf(stderr, "%i\n", res);
    if(THEMIS_SUCCESS!=res){
      free(encrypted_data);
      RedisModule_ReplyWithError(ctx, "ERR secure seal encription failed");
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
      RedisModule_ReplyWithError(ctx, "ERR secure seal decription failed (length determination)");
      return REDISMODULE_ERR;      
    }
    uint8_t *decrypted_data = malloc(decrypted_data_len);
    if(!decrypted_data){
      RedisModule_ReplyWithError(ctx, "ERR memory allocation error");
      return REDISMODULE_ERR;      
    }
    if(THEMIS_SUCCESS!=themis_secure_cell_decrypt_seal((const uint8_t*)pass, pass_len, NULL, 0, (const uint8_t*)message, message_len, decrypted_data, &decrypted_data_len)){
      free(decrypted_data);
      RedisModule_ReplyWithError(ctx, "ERR secure seal decription failed");
      return REDISMODULE_ERR;      
    }
    RedisModule_ReplyWithStringBuffer(ctx, (const char*)decrypted_data, decrypted_data_len);
    free(decrypted_data);
    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx, "themis", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "themis.scell_seal_set", cmd_scell_seal_encrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    if (RedisModule_CreateCommand(ctx, "themis.scell_seal_get", cmd_scell_seal_decrypt, "no-monitor fast", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;
    return REDISMODULE_OK;
}
