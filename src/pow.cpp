// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

/* unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Only change once per difficulty adjustment interval
    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
    {
        if (params.fPowAllowMinDifficultyBlocks)
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 2* 10 minutes
            // then allow mining of a min-difficulty block.
            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
                return nProofOfWorkLimit;
            else
            {
                // Return the last non-special-min-difficulty-rules-block
                const CBlockIndex* pindex = pindexLast;
                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
                    pindex = pindex->pprev;
                return pindex->nBits;
            }
        }
        return pindexLast->nBits;
    }

    // Go back by what we want to be 14 days worth of blocks
    int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
    assert(nHeightFirst >= 0);
    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
    assert(pindexFirst);

    return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
} */

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, int algo, const Consensus::Params& params)
{
    const arith_uint256 nProofOfWorkLimit = UintToArith256(params.powLimit[algo]);

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit.GetCompact();

    // find previous block with same algo
    const CBlockIndex* pindexPrev = GetLastBlockIndexForAlgo(pindexLast, algo);
    if (pindexPrev == NULL)
        return nProofOfWorkLimit.GetCompact();
    
    const CBlockIndex* pindexFirst = pindexPrev;
   
    // Go back by what we want to be nAveragingInterval blocks
    for (int i = 0; pindexFirst && i < params.nPoWAveragingInterval - 1; i++)
    {
        pindexFirst = pindexFirst->pprev;
        pindexFirst = GetLastBlockIndexForAlgo(pindexFirst, algo);
        if (pindexFirst == NULL)
            return nProofOfWorkLimit.GetCompact();
    }

    const CBlockIndex* pindexFirstPrev;
    for ( ;; )
    {
        // check blocks before first block for time warp
        pindexFirstPrev = pindexFirst->pprev;
        if (pindexFirstPrev == NULL)
            return nProofOfWorkLimit.GetCompact();
        pindexFirstPrev = GetLastBlockIndexForAlgo(pindexFirstPrev, algo);
        if (pindexFirstPrev == NULL)
            return nProofOfWorkLimit.GetCompact();
        // take previous block if block times are out of order
        if (pindexFirstPrev->GetBlockTime() > pindexFirst->GetBlockTime())
        {
            if (fDebug)
            {
                LogPrintf("GetNextWorkRequired(Algo=%d): First blocks out of order times, swapping:   %d   %d\n", algo, pindexFirstPrev->GetBlockTime(), pindexFirst->GetBlockTime());
            }
            pindexFirst = pindexFirstPrev;
        }
        else
            break;
    }
    
    // change of algo from Qubit to Argon2d
    // because the difficulty of Qubit will be much higher than Argon2d on change, we will return the proof-of-work limit
    // once the changeover occurs, then start adjusting once nAveragingInterval blocks have occured. Will result in a short
    // insta-mine, but difficulty changes will quickly take care of this.
    
    if(pindexLast->GetBlockTime() >= params.nTimeArgon2dStart && pindexFirst->GetBlockTime() < params.nTimeArgon2dStart && algo == ALGO_SLOT3)
    {
        if (fDebug)
            LogPrintf("GetNextWorkRequired(Algo=%d): nTimeArgon2dStart has been passed, but insufficient blocks to calculate new target. Returning nProofOfWorkLimit\n", algo);
        return nProofOfWorkLimit.GetCompact();
    }
    
    int64_t nMinActualTimespan;
    int64_t nMaxActualTimespan;
    
    if(pindexLast->nHeight < 1999)
    {
        // Unitus intial mining phase, allow up to 20% difficulty change per block
        nMinActualTimespan = params.nPoWAveragingTargetTimespan() * (100 - 20) / 100;
        nMaxActualTimespan = params.nPoWAveragingTargetTimespan() * (100 + 20) / 100;
    }
    else
    {
        nMinActualTimespan = params.nPoWAveragingTargetTimespan() * (100 - params.nMaxAdjustUp) / 100;
        nMaxActualTimespan = params.nPoWAveragingTargetTimespan() * (100 + params.nMaxAdjustDown) / 100;
    }
    
    // Limit adjustment step
    int64_t nActualTimespan = pindexPrev->GetBlockTime() - pindexFirst->GetBlockTime();
    
    if (fDebug)
        LogPrintf("GetNextWorkRequired(Algo=%d): nActualTimespan = %d before bounds   %d   %d\n", algo, nActualTimespan, pindexLast->GetBlockTime(), pindexFirst->GetBlockTime());
    
    if (nActualTimespan < nMinActualTimespan)
        nActualTimespan = nMinActualTimespan;
    if (nActualTimespan > nMaxActualTimespan)
        nActualTimespan = nMaxActualTimespan;
    
    if (fDebug)
        LogPrintf("GetNextWorkRequired(Algo=%d): nActualTimespan = %d after bounds   %d   %d\n", algo, nActualTimespan, nMinActualTimespan, nMaxActualTimespan);
    
    // Retarget
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexPrev->nBits);
    arith_uint256 bnOld;
    bnOld = bnNew;
    
    bnNew *= nActualTimespan;
    bnNew /= params.nPoWAveragingTargetTimespan();
    if(bnNew > nProofOfWorkLimit)
        bnNew = nProofOfWorkLimit;
    
    if (fDebug)
    {
        LogPrintf("GetNextWorkRequired(Algo=%d) RETARGET\n", algo);
        LogPrintf("GetNextWorkRequired(Algo=%d): nTargetTimespan = %d    nActualTimespan = %d\n", algo, params.nPoWAveragingTargetTimespan(), nActualTimespan);
        LogPrintf("GetNextWorkRequired(Algo=%d): Before: %08x  %s\n", algo, pindexLast->nBits, bnOld.ToString());
        LogPrintf("GetNextWorkRequired(Algo=%d): After:  %08x  %s\n", algo, bnNew.GetCompact(), bnNew.ToString());
    }
    
    return bnNew.GetCompact();
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, int algo, const Consensus::Params& params)
{
    if (params.fPowNoRetargeting)
        return pindexLast->nBits;

    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit[algo]);
    arith_uint256 bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, int algo, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit[algo]))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
