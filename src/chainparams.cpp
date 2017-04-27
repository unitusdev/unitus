// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "consensus/merkle.h"
#include "primitives/pureheader.h"
#include "tinyformat.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Nasa’s NuSTAR probe takes first spectacular, Christmassy picture of the sun";
    const CScript genesisOutputScript = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 800;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 227931;
        consensus.BIP34Hash = uint256S("0x000000000000024b89b42a942fe0d9fea3bb44ab7bd1b19115dd6a759c0808b8");
        consensus.BIP65Height = 388381; // 000000000000000004c2b624ed5d7756c508d90fd0da2c7c679febfa6c4735f0
        consensus.BIP66Height = 363725; // 00000000000000000379eaa19dce8c9b722d46ae6a57c2f1a988119488b50931
        consensus.powLimit[ALGO_SLOT1] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT2] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT3] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT4] = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT5] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        // consensus.nPowTargetSpacing = 10 * 60;
        
        consensus.nPowTargetSpacing = 60;                 // 60 second block time
        consensus.nPoWAveragingInterval = 10;             // 10 block averaging interval
        consensus.nMaxAdjustUp = 5;                       // 5% adjustment up
        consensus.nMaxAdjustDown = 5;                     // 5% adjustment down

        consensus.nBlockSequentialAlgoMaxCountV1 = 3;     // maximum sequential blocks of same algo V1
        consensus.nBlockSequentialAlgoMaxCountV2 = 5;     // maximum sequential blocks of same algo V2
        consensus.nStartAuxPow = 0;
        consensus.nAuxpowChainId = 0x9B;                  // unitus chain id 155
        consensus.fStrictChainId = false;
        consensus.nLegacyBlocksBefore = -1;
        
        consensus.nTimeLyra2RE2Start = 1456099764;                  // block time where blake hash is replaced with Lyra2RE2 (Mon, 22 Feb 2016 00:09:24 GMT)
        consensus.nTimeArgon2dStart = 1495238400;                   // block time where Qubit hash is replaced with Argon2d (Sat, 20 May 2017 00:00:00 GMT)
        consensus.nBlockAlgoNormalisedWorkDecayV2Start = 25300;     // block where weight decay v2 starts
        consensus.nGeometricAverageWork_Start = 450000;           // block where geometric average work calculation kicks in
        consensus.nBlockSequentialAlgoRule2Start = 456000;           // block where sequential algo rule V2 starts
        
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1916; // 95% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // Disabled
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 0; // Disabled

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0; // Disabled
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000003f94d1ad391682fe038bf5");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000000000000000013176bf8d7dfeab4e1db31dc93bc311b436e82ab226b90"); //453354

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc5;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0x9d;
        nDefaultPort = 50603;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1419368971, 7659762, 0x1e0fffff, 1, 100 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xd8a2b2439d013a59f3bfc626a33487a3d7d27e42a3c9e0b81af814cd8e592f31"));
        assert(genesis.hashMerkleRoot == uint256S("0x7e86d7e6d123ba67684125d094d010a6e04ab753fd5d601c024c155c0a4eb51e"));

        vSeeds.push_back(CDNSSeedData("seed1.unitus.online", "seed1.unitus.online", true));
        vSeeds.push_back(CDNSSeedData("seed2.unitus.online", "seed2.unitus.online", true));
        vSeeds.push_back(CDNSSeedData("seed3.unitus.online", "seed3.unitus.online", true));
        vSeeds.push_back(CDNSSeedData("seed4.unitus.online", "seed4.unitus.online", true));
        vSeeds.push_back(CDNSSeedData("seed5.unitus.online", "seed5.unitus.online", true));
        vSeeds.push_back(CDNSSeedData("seed1.unitus.nutty.one", "seed1.unitus.nutty.one", true));
        vSeeds.push_back(CDNSSeedData("seed2.unitus.nutty.one", "seed2.unitus.nutty.one", true));
        vSeeds.push_back(CDNSSeedData("seed3.unitus.nutty.one", "seed3.unitus.nutty.one", true));
        vSeeds.push_back(CDNSSeedData("seed4.unitus.nutty.one", "seed4.unitus.nutty.one", true));
        vSeeds.push_back(CDNSSeedData("seed5.unitus.nutty.one", "seed5.unitus.nutty.one", true));
        // Note that of those with the service bits flag, most only support a subset of possible options
        // vSeeds.push_back(CDNSSeedData("bitcoin.sipa.be", "seed.bitcoin.sipa.be", true)); // Pieter Wuille, only supports x1, x5, x9, and xd
        // vSeeds.push_back(CDNSSeedData("bluematt.me", "dnsseed.bluematt.me", true)); // Matt Corallo, only supports x9
        // vSeeds.push_back(CDNSSeedData("dashjr.org", "dnsseed.bitcoin.dashjr.org")); // Luke Dashjr
        // vSeeds.push_back(CDNSSeedData("bitcoinstats.com", "seed.bitcoinstats.com", true)); // Christian Decker, supports x1 - xf
        // vSeeds.push_back(CDNSSeedData("bitcoin.jonasschnelli.ch", "seed.bitcoin.jonasschnelli.ch", true)); // Jonas Schnelli, only supports x1, x5, x9, and xd

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,68);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,10);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,132);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;

        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0xd8a2b2439d013a59f3bfc626a33487a3d7d27e42a3c9e0b81af814cd8e592f31"))
            (15005, uint256S("0x240c146f076b5c3dc7573a3016cf7349c52a4cda4cad9ea2c67bf23bd9543e68"))
            (162799, uint256S("0xe1e1f56dc314c99f156d06e37cca0f9f5d594affe867f2a38728986ade66562f"))
            (426229, uint256S("0x92431424fb06129bafe00ff2e4fa65f071c28067eb42d0ce523f4850d5c58ac8"))
            (450014, uint256S("0xb6236b74c586bf00d2a115ef8df215b4e3399a0be9b46385082cc985677874b9"))
            (451899, uint256S("0xd971c1fca5c3ae08e318e4844e6465d96c93a37187c1e1e25b2a706fde00cdd6"))
            (456015, uint256S("0x07e873fa202ee09798b96d1e070153eb3f6753ca958532ed7d46e13e0625edbb"))
            (456402, uint256S("0x9da02fb278c8385d500c169eb37ba8c4368c6f002cd769e4ebba7bb27e90338f"))
            (466070, uint256S("0x3da12fb3a17eaa3680e122d6f5b852fdcdc2c13748985d070a4312bc04faed6c"))
            (494719, uint256S("0xa6405ca38116e868e1ec5110bbbf0633d3b53d17bf208cb310980152434e1d8e"))
            (528660, uint256S("0x8fb47753ef6c513dce414187d75635365395571e4881b516172ca24f215a7c44"))
            (563725, uint256S("0x8e0421cf747f0c711ca5f86fb9ef28ad6decd8ecafd4c8182f332f9365279707"))
            (816665, uint256S("0xf3072f771c12d617950a335ecb7367220d2dc2f6bdb63249dbf9beeaa0fb8101"))
        };

        chainTxData = ChainTxData{
            1483028128, // * UNIX timestamp of last checkpoint block
            1000639,   // * total number of transactions between genesis and last checkpoint
                        //   (the tx=... number in the SetBestChain debug.log lines)
            0.0168     // * estimated number of transactions per second after that timestamp
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 210000;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 100;
        consensus.BIP34Height = 21111;
        consensus.BIP34Hash = uint256S("0x0000000023b3a96d3484e5abb3755c413e7d41500f8e2a5c3f0dd01299cd8ef8");
        consensus.BIP65Height = 581885; // 00000000007f6655f22f98e72ed80d8b06dc761d5da09df0fa1dc4be4f861eb6
        consensus.BIP66Height = 330776; // 000000002104c8c45e99a8853285a3b592602a3ccde2b832481da85e9e4ba182
        consensus.powLimit[ALGO_SLOT1] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT2] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT3] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT4] = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT5] = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        // consensus.nPowTargetSpacing = 10 * 60;
        
        consensus.nPowTargetSpacing = 60; // 60 second block time
        consensus.nPoWAveragingInterval = 10; // 10 block averaging interval
        consensus.nMaxAdjustUp = 5;                       // 5% adjustment up
        consensus.nMaxAdjustDown = 5;                     // 5% adjustment down

        consensus.nBlockSequentialAlgoMaxCountV1 = 3;     // maximum sequential blocks of same algo V1
        consensus.nBlockSequentialAlgoMaxCountV2 = 5;     // maximum sequential blocks of same algo V2
        consensus.nStartAuxPow = 0;
        consensus.nAuxpowChainId = 0x9B;                  // unitus chain id 155
        consensus.fStrictChainId = false;
        consensus.nLegacyBlocksBefore = -1;
        
        consensus.nTimeLyra2RE2Start = 1456099764; // block time where blake hash is replaced with Lyra2RE2 (Mon, 22 Feb 2016 00:09:24 GMT)
        consensus.nTimeArgon2dStart = 1491705000; // block time where Qubit hash is replaced with Argon2d (Sun, 09 Apr 2017 02:30:00 GMT)
        consensus.nBlockAlgoNormalisedWorkDecayV2Start = 40;     // block where weight decay v2 starts
        consensus.nGeometricAverageWork_Start = 60;           // block where geometric average work calculation kicks in
        consensus.nBlockSequentialAlgoRule2Start = 70;           // block where sequential algo rule V2 starts
        
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601; // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999; // December 31, 2008

        // Deployment of BIP68, BIP112, and BIP113.
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0; // Disabled
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 0; // Disabled

        // Deployment of SegWit (BIP141, BIP143, and BIP147)
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0; // Disabled
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 0; // Disabled

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000000001f057509eba81aed91");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00000000000128796ee387cf110ccb9d2f36cffaf7f73079c995377c65ac0dcc"); //1079274

        pchMessageStart[0] = 0xc6;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc7;
        pchMessageStart[3] = 0x9d;
        nDefaultPort = 60603;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1419370259, 1875768, 0x1e0fffff, 1, 100 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x79b11e9472e5876fa6b6fac3efd46d63ee19e6f700d9048364e0b4ddeab0b58b"));
        assert(genesis.hashMerkleRoot == uint256S("0x7e86d7e6d123ba67684125d094d010a6e04ab753fd5d601c024c155c0a4eb51e"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        // vSeeds.push_back(CDNSSeedData("testnetbitcoin.jonasschnelli.ch", "testnet-seed.bitcoin.jonasschnelli.ch", true));
        // vSeeds.push_back(CDNSSeedData("petertodd.org", "seed.tbtc.petertodd.org", true));
        // vSeeds.push_back(CDNSSeedData("bluematt.me", "testnet-seed.bluematt.me"));
        // vSeeds.push_back(CDNSSeedData("bitcoin.schildbach.de", "testnet-seed.bitcoin.schildbach.de"));
        vSeeds.push_back(CDNSSeedData("testseed1.unitus.online", "testseed1.unitus.online"));
        vSeeds.push_back(CDNSSeedData("testseed2.unitus.online", "testseed2.unitus.online"));
        vSeeds.push_back(CDNSSeedData("testseed1.unitus.nutty.one", "testseed1.unitus.nutty.one"));
        vSeeds.push_back(CDNSSeedData("testseed2.unitus.nutty.one", "testseed2.unitus.nutty.one"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,130);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,192);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;


        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            ( 0, uint256S("0x79b11e9472e5876fa6b6fac3efd46d63ee19e6f700d9048364e0b4ddeab0b58b")),
        };

        chainTxData = ChainTxData{
            // Data as of block 00000000c2872f8f8a8935c8e3c5862be9038c97d4de2cf37ed496991166928a (height 1063660)
            1419370259,
            1,
            0.1
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 800;
        consensus.nMajorityWindow = 1000;
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351; // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251; // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.powLimit[ALGO_SLOT1] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT2] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT3] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT4] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.powLimit[ALGO_SLOT5] = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 14 * 24 * 60 * 60; // two weeks
        // consensus.nPowTargetSpacing = 10 * 60;
        
        consensus.nPowTargetSpacing = 60; // 60 second block time
        consensus.nPoWAveragingInterval = 10; // 10 block averaging interval
        consensus.nMaxAdjustDown = 4; // 4% adjustment downwards
        consensus.nMaxAdjustUp = 4; // %4 adjustment upwards
        
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].bit = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_CSV].nTimeout = 999999999999ULL;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_SEGWIT].nTimeout = 999999999999ULL;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xc7;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc8;
        pchMessageStart[3] = 0x9d;
        nDefaultPort = 60604;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1419370259, 2, 0x207fffff, 1, 100 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x2273ea4189b9aedb13f7051f0c42049bcb8d2549410a34d5d5d9eb1511c43268"));
        assert(genesis.hashMerkleRoot == uint256S("0x7e86d7e6d123ba67684125d094d010a6e04ab753fd5d601c024c155c0a4eb51e"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x79b11e9472e5876fa6b6fac3efd46d63ee19e6f700d9048364e0b4ddeab0b58b"))
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xCF).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();
    }

    void UpdateBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
            return mainParams;
    else if (chain == CBaseChainParams::TESTNET)
            return testNetParams;
    else if (chain == CBaseChainParams::REGTEST)
            return regTestParams;
    else
        throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

void UpdateRegtestBIP9Parameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
{
    regTestParams.UpdateBIP9Parameters(d, nStartTime, nTimeout);
}
 
