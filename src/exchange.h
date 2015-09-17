// Copyright (c) 2015 The B&C Exchange developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "serialize.h"
#include "util.h"

class CAsset
{
public:
    int nBlockchainId;
    int nAssetId;
    unsigned short nNumberOfConfirmations;
    unsigned char nRequiredDepositSigners;
    unsigned char nTotalDepositSigners;
    int64 nMaxTrade;

    CAsset() :
        nBlockchainId(-1),
        nAssetId(-1),
        nNumberOfConfirmations(0),
        nRequiredDepositSigners(0),
        nTotalDepositSigners(0),
        nMaxTrade(0)
    {
    }

    void SetNull()
    {
        nBlockchainId = -1;
        nAssetId = -1;
        nNumberOfConfirmations = 0;
        nRequiredDepositSigners = 0;
        nTotalDepositSigners = 0;
        nMaxTrade = 0;
    }

    IMPLEMENT_SERIALIZE
    (
            READWRITE(nBlockchainId);
            READWRITE(nAssetId);
            READWRITE(nNumberOfConfirmations);
            READWRITE(nRequiredDepositSigners);
            READWRITE(nTotalDepositSigners);
            READWRITE(nMaxTrade);
    )

    uint64 GetGlobalId() const
    {
        return AssetGlobalId(nBlockchainId, nAssetId);
    }

    inline bool operator==(const CAsset& other) const
    {
        return (nBlockchainId == other.nBlockchainId &&
                nAssetId == other.nAssetId &&
                nNumberOfConfirmations == other.nNumberOfConfirmations &&
                nRequiredDepositSigners == other.nRequiredDepositSigners &&
                nTotalDepositSigners == other.nTotalDepositSigners &&
                nMaxTrade == other.nMaxTrade);
    }
    inline bool operator!=(const CAsset& other) const
    {
        return !(*this == other);
    }
};

#endif //EXCHANGE_H
