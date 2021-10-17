#include <stdlib.h>
#include <mbedtls/gcm.h>
#include "cpm_crypto.h"


mbedtls_gcm_context todo_ctx;
char* todo_ta_buffer;
int todo_ta_size;

int cpm_crypto_init(const unsigned char *key, const unsigned char *iv) 
{
    int ret = 0;
    
    mbedtls_gcm_init(&todo_ctx );

    ret = mbedtls_gcm_setkey(
        &todo_ctx,              /* The GCM context. */
        MBEDTLS_CIPHER_ID_AES , /* The 128-bit block cipher to use */
        key,                    /* The encryption key  */
        256/8);                 /* The key size in bits. ( 128, 192 or 256 bits) */
    
    if (ret != 0) {
        goto error_key;
    }
        
    ret = mbedtls_gcm_starts(
        &todo_ctx,              /* The GCM context. */
        MBEDTLS_GCM_DECRYPT,    /* The operation to perform: MBEDTLS_GCM_ENCRYPT or MBEDTLS_GCM_DECRYPT. */
        iv,                     /* The initialization vector. */
        16,                     /* The length of the IV */
        NULL,                   /* The buffer holding the (optional) additional data */
        0);                     /* The length of the (optional) additional data */
    
    if (ret != 0) {
        goto error_gcm_starts;
    }
    return 0;
    
    todo_ta_buffer = malloc(TODO_MAX_TA_SIZE);
    if (todo_ta_buffer == NULL) {
        goto gcm_mem_error;
    }
    todo_ta_size = 0;

gcm_mem_error:
error_gcm_starts:
error_key:
    mbedtls_gcm_free(&todo_ctx);
error_gcm_init:
    return -1;
}


int cpm_crypto_update( unsigned char* data, unsigned int data_size)
{
    if (todo_ta_buffer == NULL) {
        return -1;
    }
    
    if ((todo_ta_size + data_size )> TODO_MAX_TA_SIZE) {
        return -1;
    }
    
    mbedtls_gcm_update(
        &todo_ctx,                          /* The GCM context. */
        data_size,                          /* The length of the input data */
        data,                               /* Input data */
        &(todo_ta_buffer[todo_ta_size]));   /* Output data */
    
    todo_ta_size += data_size;
}

