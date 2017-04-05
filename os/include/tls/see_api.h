/****************************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
/// @file see/see_api.h
/// @brief SEE api is supporting security api for using secure storage.

#ifndef __SEE_API_H
#define __SEE_API_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>

#include "../../arch/arm/src/s5j/sss/mb_cmd_dh.h"
#include "../../arch/arm/src/s5j/sss/mb_cmd_hash.h"
#include "../../arch/arm/src/s5j/sss/mb_cmd_rsa.h"

#include "../../arch/arm/src/s5j/sss/isp_define.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_hash.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_rng.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_secure_storage_factorykey.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_secure_storage.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_secure_storage_key.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_dh_securekey.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_rsa_securekey.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_hmac_securekey.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_ecdsa_securekey.h"
#include "../../arch/arm/src/s5j/sss/mb_cmd_secure_storage_data.h"
#include "../../arch/arm/src/s5j/sss/isp_driver_error.h"
#include "../../arch/arm/src/s5j/sss/isp_oid.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

//#define SEE_API_DEBUG
#if defined(SEE_API_DEBUG)
#define SEE_DEBUG printf
#else
#define SEE_DEBUG(...) do {} while (0)
#endif

/* Return value */
#define SEE_OK                   0
#define SEE_ERROR                1
#define SEE_ALLOC_ERROR          2
#define SEE_INVALID_INPUT_PARAMS 3

/* Key index */
#define SEE_AES_KEY_INDEX        1
#define SEE_RSA_PUBLIC_INDEX     1
#define SEE_RSA_PRIVATE_INDEX    2
#define SEE_ECC_PUBLIC_INDEX     3
#define SEE_ECC_PRIVATE_INDEX    4
#define SEE_DH_KEY_INDEX         5
#define SEE_HMAC_KEY_INDEX       6

#define KEY_RSA_LEN_1024    128
#define KEY_RSA_LEN_2048    256

/* Key type */
#define RSA_KEY           0x030000
#define ECC_KEY           0x040000

#define RSA_KEY_1024      ((RSA_KEY)  | (0xB1))
#define RSA_KEY_2048      ((RSA_KEY)  | (0xB2))

#define ECC_KEY_BP192     ((ECC_KEY) | (0x51))
#define ECC_KEY_BP224     ((ECC_KEY) | (0x52))
#define ECC_KEY_BP256     ((ECC_KEY) | (0x53))
#define ECC_KEY_BP384     ((ECC_KEY) | (0x54))
#define ECC_KEY_BP512     ((ECC_KEY) | (0x55))

#define ECC_KEY_NIST192   ((ECC_KEY) | (0x21))
#define ECC_KEY_NIST224   ((ECC_KEY) | (0x22))
#define ECC_KEY_NIST256   ((ECC_KEY) | (0x23))
#define ECC_KEY_NIST384   ((ECC_KEY) | (0x24))
#define ECC_KEY_NIST512   ((ECC_KEY) | (0x25))

#define SEE_MAX_RANDOM_SIZE		(256)

/* cert type */
#define CERT_DER      0x01
#define CERT_PEM      0x02

/* index */
#define MAX_DATA_INDEX	0x20
#define MAX_KEY_INDEX	0x08
#define MAX_CERT_INDEX	0x08

//#define ISP_CHECKBUSY() while(isp_get_status()){}
#define ISP_CHECKBUSY()

#define _SEE_MUTEX_LOCK                     \
{                                          \
	if (see_mutex_lock(&m_handler) != 0)    \
		return -1;                         \
}
#define _SEE_MUTEX_UNLOCK                   \
{                                         \
	if (see_mutex_unlock(&m_handler) != 0)  \
		return -1;                         \
}

/****************************************************************************
 * Public types
****************************************************************************/

typedef struct see_mutex_s {
	pthread_mutex_t mutex;
	int valid;
} see_mutex_t;

typedef struct cert_options {
	/* mandatory */
	unsigned int cert_index;
	unsigned int issuer_key_index;	/* index number of the issuer key       */
	unsigned int subject_key_index;	/* index number of the subject key      */
	unsigned int subject_pub_keylen;
	unsigned char *subject_pub_key;
	const char *subject_name;	/* subject name for certificate         */
	const char *issuer_name;	/* issuer name for certificate          */
	const char *not_before;		/* validity period not before           */
	const char *not_after;		/* validity period not after            */

	/* optional */
	const unsigned char *subject_pwd;	/* password for the subject key file    */
	const char *serial;			/* serial number string                 */
	unsigned char key_usage;	/* key usage flags                      */
	unsigned char ns_cert_type;	/* NS cert type                         */
	int is_ca;					/* is a CA certificate                  */
	int max_pathlen;			/* maximum CA path length               */
	int selfsign;
} cert_opt;

extern see_mutex_t m_handler;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/**
 * @brief see_init initializes security API context.
 *
 * @return On success, SEE_OK will be returned.
 *         On failure, SEE_ERROR will be returned.
 *
 */
int see_init(void);

/**
 * @brief see_free free security API context.
 *
 * @return On success, SEE_OK will be returned.
 *         On failure, SEE_ERROR will be returned.
 *
 */
int see_free(void);

/****************************************************************************
 * Name: Authentication
 *
 * Description:
 *
 ****************************************************************************/
int see_generate_random(unsigned int *data, unsigned int len);
int see_get_certificate(unsigned char *cert, unsigned int *cert_len, unsigned int cert_index, unsigned int cert_type);
int see_set_certificate(unsigned char *cert, unsigned int cert_len, unsigned int cert_index, unsigned int cert_type);
int see_get_ecdsa_signature(struct sECC_SIGN *ecc_sign, unsigned char *hash, unsigned int hash_len, unsigned int key_index);
int see_verify_ecdsa_signature(struct sECC_SIGN *ecc_sign, unsigned char *hash, unsigned int hash_len, unsigned int key_index);
int see_get_hash(struct sHASH_MSG *h_param, unsigned char *hash, unsigned int mode);
int see_compute_ecdh_param(struct sECC_KEY *ecc_pub, unsigned int key_index, unsigned char *output, unsigned int *olen);

/****************************************************************************
 * Name: Internal functions
 *
 * Description:
 *
 ****************************************************************************/
int see_mutex_init(see_mutex_t *m);
int see_mutex_free(see_mutex_t *m);
int see_mutex_lock(see_mutex_t *m);
int see_mutex_unlock(see_mutex_t *m);

unsigned int see_get_keyindex(unsigned int key_type);
unsigned int see_free_keyindex(unsigned int key_type, unsigned int key_index);
int see_check_keyindex(unsigned int key_index);
#endif							/* __SEE_API_H */
