// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "assert.h"
#include "core.h"
#include "protocol.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

using namespace boost::assign;

//
// Main network
//

unsigned int pnSeed[] =
{
};

class CMainParams : public CChainParams {
public:
    CMainParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xc5;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc6;
        pchMessageStart[3] = 0x9d;
        vAlertPubKey = ParseHex("040CAAF716F6AFBF6821C73B9F0BB32D991AABC1838B12C2683F24C1B0D53B269C280F08C4742F1B6735BA57DC5411B29EACB07262E37847EDF1D84529229B967D");
        nDefaultPort = 50603;
        nRPCPort = 50604;
        bnProofOfWorkLimit[ALGO_BLAKE] = CBigNum(~uint256(0) >> 20);
        bnProofOfWorkLimit[ALGO_SKEIN] = CBigNum(~uint256(0) >> 20);
        bnProofOfWorkLimit[ALGO_QUBIT]  = CBigNum(~uint256(0) >> 20);
        bnProofOfWorkLimit[ALGO_YESCRYPT] = CBigNum(~uint256(0) >> 16);
        bnProofOfWorkLimit[ALGO_X11] = CBigNum(~uint256(0) >> 20);
        //nSubsidyHalvingInterval = 210000;

        // Build the genesis block. Note that the output of the genesis coinbase cannot
        // be spent as it did not originally exist in the database.
        //
        // CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
        //   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
        //     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
        //     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
        //   vMerkleTree: 4a5e1e
        const char* pszTimestamp = "Nasa’s NuSTAR probe takes first spectacular, Christmassy picture of the sun";
        CTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 100 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime    = 1419368971;
        genesis.nBits    = 0x1e0fffff;
        genesis.nNonce   = 7659762;
		
        hashGenesisBlock = genesis.GetHash();
	
        assert(hashGenesisBlock == uint256("0xd8a2b2439d013a59f3bfc626a33487a3d7d27e42a3c9e0b81af814cd8e592f31"));
        assert(genesis.hashMerkleRoot == uint256("0x7e86d7e6d123ba67684125d094d010a6e04ab753fd5d601c024c155c0a4eb51e"));

        vSeeds.push_back(CDNSSeedData("seed1.unitus.info", "seed1.unitus.info"));
        vSeeds.push_back(CDNSSeedData("seed2.unitus.info", "seed2.unitus.info"));
        vSeeds.push_back(CDNSSeedData("seed3.unitus.info", "seed3.unitus.info"));
        vSeeds.push_back(CDNSSeedData("seed4.unitus.info", "seed4.unitus.info"));
        vSeeds.push_back(CDNSSeedData("seed5.unitus.info", "seed5.unitus.info"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(68);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(10);
        base58Prefixes[SECRET_KEY] =     list_of(132);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x88)(0xB2)(0x1E);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x88)(0xAD)(0xE4);

        // Convert the pnSeeds array into usable address objects.
        for (unsigned int i = 0; i < ARRAYLEN(pnSeed); i++)
        {
            // It'll only connect to one or two seed nodes because once it connects,
            // it'll get a pile of addresses with newer timestamps.
            // Seed nodes are given a random 'last seen time' of between one and two
            // weeks ago.
            const int64_t nOneWeek = 7*24*60*60;
            struct in_addr ip;
            memcpy(&ip, &pnSeed[i], sizeof(ip));
            CAddress addr(CService(ip, GetDefaultPort()));
            addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
            vFixedSeeds.push_back(addr);
        }
    }

    virtual const CBlock& GenesisBlock() const { return genesis; }
    virtual Network NetworkID() const { return CChainParams::MAIN; }

    virtual const vector<CAddress>& FixedSeeds() const {
        return vFixedSeeds;
    }
protected:
    CBlock genesis;
    vector<CAddress> vFixedSeeds;
};
static CMainParams mainParams;


//
// Testnet (v3)
//
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        // The message start string is designed to be unlikely to occur in normal data.
        // The characters are rarely used upper ASCII, not valid as UTF-8, and produce
        // a large 4-byte int at any alignment.
        pchMessageStart[0] = 0xc6;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc7;
        pchMessageStart[3] = 0x9d;
        vAlertPubKey = ParseHex("04302390343f91cc401d56d68b123028bf52e5fca1939df127f63c6467cdf9c8e2c14b61104cf817d0b780da337893ecc4aaff1309e536162dabbdb45200ca2b0a");
        nDefaultPort = 60603;
        nRPCPort = 60604;
        strDataDir = "testnet";

        // Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1419370259;
        genesis.nNonce = 1875768;
        hashGenesisBlock = genesis.GetHash();

        assert(hashGenesisBlock == uint256("0x79b11e9472e5876fa6b6fac3efd46d63ee19e6f700d9048364e0b4ddeab0b58b"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("testseed1.unitus.info", "testseed1.unitus.info"));
        vSeeds.push_back(CDNSSeedData("testseed2.unitus.info", "testseed2.unitus.info"));

        base58Prefixes[PUBKEY_ADDRESS] = list_of(130);
        base58Prefixes[SCRIPT_ADDRESS] = list_of(192);
        base58Prefixes[SECRET_KEY]     = list_of(239);
        base58Prefixes[EXT_PUBLIC_KEY] = list_of(0x04)(0x35)(0x87)(0xCF);
        base58Prefixes[EXT_SECRET_KEY] = list_of(0x04)(0x35)(0x83)(0x94);
    }
    virtual Network NetworkID() const { return CChainParams::TESTNET; }
};
static CTestNetParams testNetParams;


//
// Regression test
//
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        pchMessageStart[0] = 0xc7;
        pchMessageStart[1] = 0xab;
        pchMessageStart[2] = 0xc8;
        pchMessageStart[3] = 0x9d;
//        nSubsidyHalvingInterval = 150;
        bnProofOfWorkLimit[ALGO_BLAKE] = CBigNum(~uint256(0) >> 1);
        bnProofOfWorkLimit[ALGO_SKEIN] = CBigNum(~uint256(0) >> 1);
        bnProofOfWorkLimit[ALGO_QUBIT]  = CBigNum(~uint256(0) >> 1);
        bnProofOfWorkLimit[ALGO_YESCRYPT] = CBigNum(~uint256(0) >> 1);
        bnProofOfWorkLimit[ALGO_X11] = CBigNum(~uint256(0) >> 1);
        genesis.nTime = 1419370259;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2;
        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 18444;
        strDataDir = "regtest";
	
        assert(hashGenesisBlock == uint256("0x2273ea4189b9aedb13f7051f0c42049bcb8d2549410a34d5d5d9eb1511c43268"));

        vSeeds.clear();  // Regtest mode doesn't have any DNS seeds.
    }

    virtual bool RequireRPCPassword() const { return false; }
    virtual Network NetworkID() const { return CChainParams::REGTEST; }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = &mainParams;

const CChainParams &Params() {
    return *pCurrentParams;
}

void SelectParams(CChainParams::Network network) {
    switch (network) {
        case CChainParams::MAIN:
            pCurrentParams = &mainParams;
            break;
        case CChainParams::TESTNET:
            pCurrentParams = &testNetParams;
            break;
        case CChainParams::REGTEST:
            pCurrentParams = &regTestParams;
            break;
        default:
            assert(false && "Unimplemented network");
            return;
    }
}

bool SelectParamsFromCommandLine() {
    bool fRegTest = GetBoolArg("-regtest", false);
    bool fTestNet = GetBoolArg("-testnet", false);

    if (fTestNet && fRegTest) {
        return false;
    }

    if (fRegTest) {
        SelectParams(CChainParams::REGTEST);
    } else if (fTestNet) {
        SelectParams(CChainParams::TESTNET);
    } else {
        SelectParams(CChainParams::MAIN);
    }
    return true;
}
