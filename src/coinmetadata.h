// Copyright (c) 2015 The B&C Exchange developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SRC_COINMETADATA_H_
#define SRC_COINMETADATA_H_

#include <string>
#include "util.h"

std::string GetAssetSymbol(uint64 gid);
std::string GetAssetName(uint64 gid);

#endif /* SRC_COINMETADATA_H_ */
