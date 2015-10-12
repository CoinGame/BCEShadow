// Copyright (c) 2015 The B&C Exchange developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SRC_COINMETADATA_H_
#define SRC_COINMETADATA_H_

#include <boost/tuple/tuple.hpp>
#include <string>
#include "util.h"

using std::string;

typedef boost::tuple<string, string, unsigned char> coin_metadata;
typedef std::map<uint64, coin_metadata> coin_metadata_map;

extern coin_metadata_map const COIN_METADATA;

string GetAssetSymbol(uint64 gid);
string GetAssetName(uint64 gid);
unsigned char GetAssetUnitExponent(uint64 gid);

#endif /* SRC_COINMETADATA_H_ */
