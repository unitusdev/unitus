// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef HASH_ARGON2D
#define HASH_ARGON2D

#include "uint256.h"
#include "serialize.h"

#include "argon2/argon2.h"

#include <vector>

template<typename T1>
inline uint256 HashArgon2d(const T1 pbegin, const T1 pend)
{
    uint32_t t_cost = 1; // 1 iteration
    uint32_t m_cost = 4096; // use 4MB
    uint32_t parallelism = 1; // 1 thread, 2 lanes
    
    static unsigned char pblank[1];
    size_t pwdlen = (pend - pbegin) * sizeof(pbegin[0]);
    
    uint256 hash;
    size_t hashlen = 32;
    
    argon2d_hash_raw(t_cost, m_cost, parallelism, (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), pwdlen,
                    (pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), pwdlen, (unsigned char*)&hash, hashlen);
    
    return hash;
}

#endif
