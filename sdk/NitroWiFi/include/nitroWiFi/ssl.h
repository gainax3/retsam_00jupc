/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CPS
  File:     ssl.h

  Copyright 2001-2005 Ubiquitous Corporation.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Ubiquitous Corporation, and are protected
  by Federal copyright law.  They may not be disclosed to third parties
  or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Ubiquitous Corporation.

  $Revision: 1.16 $ $Date: 2005/10/27 08:31:48 $

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SSL_H_
#define NITROWIFI_SSL_H_

#ifdef __cplusplus

extern "C" {
#endif

//

//	MD5 stuff

//
typedef struct
{
    u32 state[4];   // A, B, C, D (see rfc1321)
    u32 count[2];   // number of total bits (little-endian)
    u8  buffer[64]; // temporary buffer for the block
} CPSMd5Ctx;

extern void CPSi_md5_init(CPSMd5Ctx* );
extern void CPSi_md5_calc(CPSMd5Ctx* , void* , int);
extern void CPSi_md5_result(CPSMd5Ctx* , u8* );

//

//	SHA1 stuff

//
typedef struct
{
    u32 state[5];   // A, B, C, D, E (see fips-180-1)
    u32 count[2];   // number of total bits (big-endian)
    u8  buffer[64]; // temporary buffer for the block
} CPSSha1Ctx;

extern void CPSi_sha1_init(CPSSha1Ctx* );
extern void CPSi_sha1_calc(CPSSha1Ctx* , void* , int);
extern void CPSi_sha1_result(CPSSha1Ctx* , u8* );
extern void CPSi_sha1_result_prng(CPSSha1Ctx* , u8* );

//

//	RC4 stuff

//
typedef struct
{
    u8  x;
    u8  y;
    u8  m[256];
    u16 padding;
} CPSRc4Ctx;

extern void CPSi_rc4_init(CPSRc4Ctx* , u8* , int);
extern void CPSi_rc4_crypt(CPSRc4Ctx* , u8* , int);

//
//	Biginteger stuff
//
extern void CPSi_big_add(u16* , u16* , u16* , int);
extern void CPSi_big_sub(u16* , u16* , u16* , int);
extern void CPSi_big_mult(u16* , u16* , u16* , int);
extern void CPSi_big_div(u16* , u16* , u16* , u16* , int, u16* );
extern void CPSi_big_power(u16* , u16* , u16* , int, u16* );
extern void CPSi_big_montpower(u16* , u16* , u16* , int, u16* );
extern void CPSi_big_negate(u16* , int);
extern int  CPSi_big_sign(u16* , int);
extern void CPSi_big_from_char(u16* , u8* , int, int);
extern void CPSi_char_from_big(u8* , u16* , int, int);

//

//	Certificate stuff

//
typedef struct
{
    char*   dn;             // distinguished name
    int     modulus_len;    // lendth of modulus
    u8*     modulus;        // modulus
    int     exponent_len;   // length of public exponent
    u8*     exponent;       // public exponent
} CPSCaInfo;

typedef struct
{
    int     certificate_len;
    u8*     certificate;
} CPSCertificate;

typedef struct
{
    int     modulus_len;
    u8*     modulus;
    int     prime1_len;
    u8*     prime1;
    int     prime2_len;
    u8*     prime2;
    int     exponent1_len;
    u8*     exponent1;
    int     exponent2_len;
    u8*     exponent2;
    int     coefficient_len;
    u8*     coefficient;
} CPSPrivateKey;

extern void CPS_SetRootCa(CPSCaInfo ** , int);
extern void CPS_SetMyCert(CPSCertificate* , CPSPrivateKey* );

//
//	Authentication error codes
//
#define CPS_CERT_OUTOFDATE  0x8000
#define CPS_CERT_BADSERVER  0x4000

#define CPS_CERT_ERRMASK    0x00ff

enum
{
    CPS_CERT_NOERROR                = 0,
    CPS_CERT_NOROOTCA,
    CPS_CERT_BADSIGNATURE,
    CPS_CERT_UNKNOWN_SIGALGORITHM,
    CPS_CERT_UNKNOWN_PUBKEYALGORITHM
};

//

//	SSL session

//
typedef struct
{
    u8          sessionID[32];
    u8          master_secret[48];
    u32         when;
    CPSInAddr   ip;     // 0 when entry is for server
    u16         port;   // 0 when entry is for server
    u8          valid;  // non 0 if this entry is valid
    u8          padding;
} CPSSslSession;

typedef union
{
    CPSRc4Ctx   rc4_ctx;
} CPSCipherCtx;

#define CPS_MAX_DN_LEN                  255
#define CPS_MAX_CN_LEN                  79
#define CPS_MAX_RSA_LEN                 (2048 / 8)
#define CPS_MAX_RSA_PUBLICEXPONENT_LEN  (64 / 8)

//

//	SSL connection

//
typedef struct _CPSSslConnection
{
    u8  master_secret[48];
    u8  session_cached;
    u8  reuse_session;              // non 0 if session should be reused
    u16 method;                     // cipher suite
    u8  client_random[32];          // client_random and server_random must be
    u8  server_random[32];          // adjacently allocated
    union
    {
        u8  sessionID[32];
        u8  key_block[2 * (20 + 16 + 0)];
    } common1;
    u8*         send_mac;           // points to somewhere in key_block[]
    u8*         send_key;           //  ditto
    u8*         send_iv;            //  ditto
    CPSCipherCtx    send_cipher;    // cipher context for send
    u8      send_seq[8];            // 64 bit sequence
    u8*     rcv_mac;                // points to somewhere in key_block[]
    u8*     rcv_key;                //  ditto
    u8*     rcv_iv;                 //  ditto
    CPSCipherCtx    rcv_cipher;     // cipher context for receive
    u8          rcv_seq[8];         // 64 bit sequence
    CPSSha1Ctx  sha1_hash;          // hash of handshake messages in SHA1
    CPSSha1Ctx  sha1_hash_tmp;
    CPSMd5Ctx   md5_hash;           // hash of handshake messages in MD5
    CPSMd5Ctx   md5_hash_tmp;
    u8          server;             // non 0 if server type connection
    u8          state;
    u8          inbuf_decrypted;    // non 0 if inbuf (see below) is decrypted
    u8          padding2;

    //
    //	certificate
    //
    int         sig_algorithm;      // signature algorithm
    int         pub_algorithm;      // public key algorithm
    u8*         hash_start;         // start address of hash area
    u8*         hash_end;           // end address of hash area + 1
    u8          hash_val[20];       // hash value of hash_start..hash_end
    int         hash_len;           // valid length of hash_val[]
    CPSCaInfo   midca_info;         // middle ca info
    u8          modulus[CPS_MAX_RSA_LEN];   // modulus
    u32         modulus_len;                // length of modulus in bytes
    u8          exponent[CPS_MAX_RSA_PUBLICEXPONENT_LEN];

    // public exponent
    int         exponent_len;               // length of exponent in bytes
    u8*         signature;
    int         signature_len;
    u8          seen_validity;              // next string is 'subject', not 'issuer'
    u8          seen_pub_algorithm;         // next BIT STRING is public key information
    u8          seen_attr;
    u8          date_ok;
    char        issuer[CPS_MAX_DN_LEN + 1];
    char        subject[CPS_MAX_DN_LEN + 1];
    char        cn[CPS_MAX_CN_LEN + 1];
    char*       server_name;                // server name to match
    u8*         cert;                       // for auth_callback
    int         certlen;
    u32         cur_date;                   // 65536*year + 256*month + day
    int (*auth_callback) (int, struct _CPSSslConnection*, int);
    CPSCaInfo **     ca_info;
    int ca_builtins;
    CPSPrivateKey*      my_key;
    CPSCertificate*     my_certificate;

    //
    //	ssl_read()
    //
    u8*     inbuf;      // pointer to input buffer (NULL if none)
    long    inbuf_len;  // length of inbuf
    long    inbuf_pnt;  // index of current position (0..inbuf_len-1)
} CPSSslConnection;

extern void CPS_SslAddRandomSeed(void* , u32);

//
//	Just for now............

//
#define MI_CpuCopy8_inverse(d, s, n)    MI_CpuCopy8(s, d, n)

#ifdef __cplusplus

} /* extern "C" */
#endif

/* NITROWIFI_SSL_H_ */
#endif
