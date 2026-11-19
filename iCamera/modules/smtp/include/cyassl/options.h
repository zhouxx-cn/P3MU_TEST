/* cyassl options.h
 * generated from wolfssl/options.h
 */
/* wolfssl options.h
* generated from configure options
*
* Copyright (C) 2006-2015 wolfSSL Inc.
*
* This file is part of wolfSSL. (formerly known as CyaSSL)
*
*/

#ifndef CYASSL_OPTIONS_H
#define CYASSL_OPTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

#ifndef WOLFSSL_OPTIONS_IGNORE_SYS
#undef  _POSIX_THREADS
#define _POSIX_THREADS
#endif

#undef  HAVE_THREAD_LS
#define HAVE_THREAD_LS

#undef  TFM_TIMING_RESISTANT
#define TFM_TIMING_RESISTANT

#undef  ECC_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT

#undef  WC_RSA_BLINDING
#define WC_RSA_BLINDING

#undef  WOLFSSL_SNIFFER
#define WOLFSSL_SNIFFER

#undef  OPENSSL_EXTRA
#define OPENSSL_EXTRA

#undef  HAVE_AESGCM
#define HAVE_AESGCM

#undef  NO_DSA
#define NO_DSA

#undef  HAVE_ECC
#define HAVE_ECC

#undef  TFM_ECC256
#define TFM_ECC256

#undef  NO_RC4
#define NO_RC4

#undef  NO_HC128
#define NO_HC128

#undef  NO_RABBIT
#define NO_RABBIT

#undef  HAVE_POLY1305
#define HAVE_POLY1305

#undef  HAVE_ONE_TIME_AUTH
#define HAVE_ONE_TIME_AUTH

#undef  HAVE_CHACHA
#define HAVE_CHACHA

#undef  HAVE_HASHDRBG
#define HAVE_HASHDRBG

#undef  HAVE_TLS_EXTENSIONS
#define HAVE_TLS_EXTENSIONS

#undef  HAVE_SUPPORTED_CURVES
#define HAVE_SUPPORTED_CURVES

#undef  NO_PSK
#define NO_PSK

#undef  NO_MD4
#define NO_MD4

#undef  NO_PWDBASED
#define NO_PWDBASED


#ifdef __cplusplus
}
#endif


#endif /* CYASSL_OPTIONS_H */

