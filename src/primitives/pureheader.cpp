// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/pureheader.h"

#include "hash.h"
#include "crypto/hashskein.h"
#include "crypto/hashqubit.h"
#include "crypto/hashblake.h"
#include "crypto/hashX11.h"
#include "crypto/yescrypt/yescrypt.h"
#include "crypto/lyra2/lyra2RE.h"
#include "crypto/hashargon2d.h"
#include "utilstrencodings.h"

uint256 CPureBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}

uint256 CPureBlockHeader::GetPoWHash(int algo, const Consensus::Params& consensusParams) const
{
    switch (algo)
    {
        case ALGO_SLOT1:
            if(nTime >= consensusParams.nTimeLyra2RE2Start)
            {
                uint256 thash;
                lyra2re2_hash(BEGIN(nVersion), BEGIN(thash));
                return thash;
            }
            else
                return HashBlake(BEGIN(nVersion), END(nNonce));
            
        case ALGO_SLOT2:
            return HashSkein(BEGIN(nVersion), END(nNonce));
            
        case ALGO_SLOT3:
            if(nTime >= consensusParams.nTimeArgon2dStart)
                return HashArgon2d(BEGIN(nVersion), END(nNonce));
            else
                return HashQubit(BEGIN(nVersion), END(nNonce));
            
        case ALGO_SLOT4:
            {
                uint256 thash;
                yescrypt_hash(BEGIN(nVersion), BEGIN(thash));
                return thash;
            }
            
        case ALGO_SLOT5:
            return HashX11(BEGIN(nVersion), END(nNonce));
            
    }
    // catch-all if above doesn't match anything to algo
    if(nTime >= consensusParams.nTimeLyra2RE2Start)
    {
        uint256 thash;
        lyra2re2_hash(BEGIN(nVersion), BEGIN(thash));
        return thash;            
    }
    else
        return HashBlake(BEGIN(nVersion), END(nNonce));
}

void CPureBlockHeader::SetBaseVersion(int32_t nBaseVersion, int32_t nChainId)
{
    assert(nBaseVersion >= 1 && nBaseVersion < VERSION_AUXPOW);
    assert(!IsAuxpow());
    nVersion = nBaseVersion | (nChainId * VERSION_CHAIN_START);
}

int GetAlgo(int nVersion)
{
    switch (nVersion & BLOCK_VERSION_ALGO)
    {
        case 0:
            return ALGO_SLOT1;
        case BLOCK_VERSION_SLOT2:
            return ALGO_SLOT2;
        case BLOCK_VERSION_SLOT3:
            return ALGO_SLOT3;
        case BLOCK_VERSION_SLOT4:
            return ALGO_SLOT4;
        case BLOCK_VERSION_SLOT5:
            return ALGO_SLOT5;
    }
    return ALGO_SLOT1;
}

std::string GetAlgoName(int Algo, uint32_t time, const Consensus::Params& consensusParams)
{
    switch (Algo)
    {
        case ALGO_SLOT1:
            if(time >= consensusParams.nTimeLyra2RE2Start)
                return std::string("Lyra2RE2");
            else
                return std::string("Blake");
            
        case ALGO_SLOT2:
            return std::string("Skein");
            
        case ALGO_SLOT3:
            if(time >= consensusParams.nTimeArgon2dStart)
                return std::string("Argon2d");
            else
                return std::string("Qubit");
            
        case ALGO_SLOT4:
            return std::string("Yescrypt");
            
        case ALGO_SLOT5:
            return std::string("X11");
            
    }
    return std::string("Unknown");    
}