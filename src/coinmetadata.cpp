// Copyright (c) 2015 The B&C Exchange developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <map>
#include <string>

#include "coinmetadata.h"
#include "util.h"

#define COIN_SYMBOL_INDEX (0)
#define COIN_NAME_INDEX (1)
#define COIN_UNIT_EXP_INDEX (2)

using std::string;
using boost::assign::map_list_of;

// TODO maybe move in a config file
coin_metadata_map const COIN_METADATA = map_list_of
        (AssetGlobalId(0, 0), coin_metadata("BKS", "Blockshares", 4))
        (AssetGlobalId(0, 1), coin_metadata("BKC", "Blockcredits", 4))
        (AssetGlobalId(1, 0), coin_metadata("BTC", "Bitcoin", 8))
        (AssetGlobalId(2, 0), coin_metadata("LTC", "Litecoin", 8))
        (AssetGlobalId(3, 0), coin_metadata("DOGE", "Dogecoin", 8))
        (AssetGlobalId(4, 0), coin_metadata("RDD", "Reddcoin", 8))
        (AssetGlobalId(5, 0), coin_metadata("DASH", "Dash", 8))
        (AssetGlobalId(6, 0), coin_metadata("PPC", "Peercoin", 6))
        (AssetGlobalId(7, 0), coin_metadata("NMC", "Namecoin", 8))
        (AssetGlobalId(8, 0), coin_metadata("FTC", "Feathercoin", 8))
        (AssetGlobalId(9, 0), coin_metadata("XCP", "Counterparty", 0)) // TODO check the unit exponent
        (AssetGlobalId(10, 0), coin_metadata("BLK", "Blackcoin", 8))
        (AssetGlobalId(11, 0), coin_metadata("NSR", "NuShares", 0)) // TODO the unit will change so we show the satoshis now
        (AssetGlobalId(11, 1), coin_metadata("NBT", "NuBits", 4))
        (AssetGlobalId(12, 0), coin_metadata("ETH", "Ethereum", 0)) // TODO check the unit exponent
        (AssetGlobalId(13, 0), coin_metadata("MZC", "Mazacoin", 8))
        (AssetGlobalId(14, 0), coin_metadata("VIA", "Viacoin", 8))
        (AssetGlobalId(15, 0), coin_metadata("XCH", "ClearingHouse", 0)) // TODO check the unit exponent
        (AssetGlobalId(16, 0), coin_metadata("RBY", "Rubycoin", 8))
        (AssetGlobalId(17, 0), coin_metadata("GRS", "Groestlcoin", 8))
        (AssetGlobalId(18, 0), coin_metadata("DGC", "Digitalcoin", 8))
        (AssetGlobalId(19, 0), coin_metadata("CCN", "Cannacoin", 8))
        (AssetGlobalId(20, 0), coin_metadata("DGB", "DigiByte", 8))
        (AssetGlobalId(21, 0), coin_metadata("NVC", "Novacoin", 6))
        (AssetGlobalId(22, 0), coin_metadata("MONA", "Monacoin", 8))
        (AssetGlobalId(23, 0), coin_metadata("CLAM", "Clams", 6))
        (AssetGlobalId(24, 0), coin_metadata("XPM", "Primecoin", 6))
        (AssetGlobalId(25, 0), coin_metadata("NEOS", "Neoscoin", 8))
        (AssetGlobalId(26, 0), coin_metadata("JBS", "Jumbucks", 8))
        (AssetGlobalId(27, 0), coin_metadata("PND", "Pandacoin", 8))
        (AssetGlobalId(28, 0), coin_metadata("VTC", "Vertcoin", 8))
        (AssetGlobalId(29, 0), coin_metadata("NXT", "NXT", 8))
        (AssetGlobalId(30, 0), coin_metadata("BURST", "Burst", 8))
        (AssetGlobalId(31, 0), coin_metadata("VPN", "Vpncoin", 8))
        (AssetGlobalId(32, 0), coin_metadata("CDN", "Canada eCoin", 8))
        (AssetGlobalId(33, 0), coin_metadata("SDC", "ShadowCash", 8))
        ;

std::string GetAssetSymbol(uint32_t gid)
{
    coin_metadata_map::const_iterator itMetadata = COIN_METADATA.find(gid);
    if (itMetadata == COIN_METADATA.end())
        return "";
    coin_metadata metadata = itMetadata->second;
    return metadata.get<COIN_SYMBOL_INDEX>();
}

std::string GetAssetName(uint32_t gid)
{
    coin_metadata_map::const_iterator itMetadata = COIN_METADATA.find(gid);
    if (itMetadata == COIN_METADATA.end())
        return "";
    coin_metadata metadata = itMetadata->second;
    return metadata.get<COIN_NAME_INDEX>();
}

unsigned char GetAssetUnitExponent(uint32_t gid)
{
    coin_metadata_map::const_iterator itMetadata = COIN_METADATA.find(gid);
    if (itMetadata == COIN_METADATA.end())
        return 0;
    coin_metadata metadata = itMetadata->second;
    return metadata.get<COIN_UNIT_EXP_INDEX>();
}
