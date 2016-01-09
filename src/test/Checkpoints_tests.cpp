// Copyright (c) 2011-2013 The Bitcoin Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
// Unit tests for block-chain checkpoints
//

#include "checkpoints.h"

#include "uint256.h"

#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p15005 = uint256("0x240c146f076b5c3dc7573a3016cf7349c52a4cda4cad9ea2c67bf23bd9543e68");
    uint256 p162799 = uint256("0xe1e1f56dc314c99f156d06e37cca0f9f5d594affe867f2a38728986ade66562f");
    BOOST_CHECK(Checkpoints::CheckBlock(15005, p15005));
    BOOST_CHECK(Checkpoints::CheckBlock(162799, p162799));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(15005, p162799));
    BOOST_CHECK(!Checkpoints::CheckBlock(162799, p15005));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(15005+1, p162799));
    BOOST_CHECK(Checkpoints::CheckBlock(162799+1, p15005));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 162799);
}    

BOOST_AUTO_TEST_SUITE_END()
