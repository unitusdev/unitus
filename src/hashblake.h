// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef HASH_BLAKE
#define HASH_BLAKE

#include "uint256.h"
#include "serialize.h"
#include "sph_blakefast.h"

#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <vector>


template<typename T1>
inline uint256 HashBlake(const T1 pbegin, const T1 pend)

{
    sph_blakefast256_context     ctx_blake;
    static unsigned char pblank[1];
    uint256 hash1;
    sph_blakefast256_init(&ctx_blake);
    sph_blakefast256 (&ctx_blake, (pbegin == pend ? pblank : static_cast<const void*>(&pbegin[0])), (pend - pbegin) * sizeof(pbegin[0]));
    sph_blakefast256_close(&ctx_blake, static_cast<void*>(&hash1));
    return hash1;
}


#endif
