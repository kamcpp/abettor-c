#ifndef _NAEEM_CRYPTO_H_
#define _NAEEM_CRYPTO_H_

#include <stdlib.h>
#include <stdio.h>

#include <naeem/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DES_KEY_LENGTH 8

typedef NAEEM_byte NAEEM_DES_key[DES_KEY_LENGTH];


NAEEM_result
NAEEM_crypto_SHA1 (NAEEM_data      /* Data */,
                   NAEEM_uint32    /* Data length */,
                   NAEEM_data      /* Hash */);


NAEEM_result
NAEEM_crypto_SHA256 (NAEEM_data      /* Data */,
                     NAEEM_uint32    /* Data length */,
                     NAEEM_data      /* Hash */);


NAEEM_result
NAEEM_crypto_DES_cbc_encrypt (NAEEM_data          /* Data */,
                              NAEEM_uint32        /* Data length */,
                              NAEEM_data          /* Cipher */,
                              NAEEM_uint32_ptr    /* Pointer to cipher length */,
                              NAEEM_DES_key       /* DES key */);


NAEEM_result    
NAEEM_crypto_DES_cbc_decrypt (NAEEM_data          /* Cipher */,
                              NAEEM_uint32        /* Cipher length */,
                              NAEEM_data          /* Data */,
                              NAEEM_uint32_ptr    /* Pointer to data length */,
                              NAEEM_DES_key       /* DES key */);


NAEEM_result
NAEEM_crypto_3DES_cbc_encrypt (NAEEM_data           /* Data */,
                               NAEEM_uint32         /* Data length */,
                               NAEEM_data           /* Cipher */,
                               NAEEM_uint32_ptr     /* Pointer to cipher length */,
                               NAEEM_DES_key        /* First DES key */,
                               NAEEM_DES_key        /* Second DES key */,
                               NAEEM_DES_key        /* Third DES key */);


NAEEM_result
NAEEM_crypto_3DES_cbc_decrypt (NAEEM_data           /* Cipher */,
                               NAEEM_uint32         /* Cipher length */,
                               NAEEM_data           /* Data */,
                               NAEEM_uint32_ptr     /* Pointer to data length */,
                               NAEEM_DES_key        /* First DES key */,
                               NAEEM_DES_key        /* Second DES key */,
                               NAEEM_DES_key        /* Third DES key */);


NAEEM_result
NAEEM_crypto_MAC_v3 (NAEEM_data         /* Message */,
                     NAEEM_uint32       /* Message length */,
                     NAEEM_data         /* MAC */,
                     NAEEM_data         /* Initial check block */,
                     NAEEM_bool         /* Is padded ? */,
                     NAEEM_DES_key      /* Key 1*/,
                     NAEEM_DES_key      /* Key 2*/);


#ifdef __cplusplus
}
#endif

#endif