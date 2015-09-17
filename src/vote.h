// Copyright (c) 2014-2015 The Nu developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef VOTE_H
#define VOTE_H

#include "serialize.h"
#include "base58.h"
#include "exchange.h"
#include "util.h"

class CBlock;
class CBlockIndex;

static const int64 COIN_PARK_RATE = 100000 * COIN; // Park rate internal encoding precision. The minimum possible rate is (1.0 / COIN_PARK_RATE) coins per parked coin
static const int64 MAX_PARK_RATE = 1000000 * COIN_PARK_RATE;
static const unsigned char MAX_COMPACT_DURATION = 30; // about 2000 years
static const int64 MAX_PARK_DURATION = 1000000000; // about 1900 years

// Limit the blockchain and asset ids. If more ids are needed a protocol update is needed
static const int MAX_BLOCKCHAIN_ID = 100000;
static const int MAX_ASSET_ID = 100000;

// Confirmations is a unsigned short
static const int MAX_CONFIRMATIONS = 65535;

// Signers is a 8bit uint
static const int MAX_SIGNERS = 255;

// The minimum M-of-N signers is 2-of-3 as it is the smallest that provide redunduncy and trust minimization
static const int MIN_REQ_SIGNERS = 2;
static const int MIN_TOTAL_SIGNERS = 3;


class CCustodianVote
{
public:
    unsigned char cUnit;
    bool fScript;
    uint160 hashAddress;
    int64 nAmount;

    CCustodianVote() :
        cUnit('?'),
        fScript(false),
        hashAddress(0),
        nAmount(0)
    {
    }

    bool IsValid(int nProtocolVersion) const;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(cUnit);
        if (nVersion >= 20200) // version 0.2.2
            READWRITE(fScript);
        else if (fRead)
            const_cast<CCustodianVote*>(this)->fScript = false;
        READWRITE(hashAddress);
        READWRITE(nAmount);
    )

    inline bool operator==(const CCustodianVote& other) const
    {
        return (cUnit == other.cUnit &&
                fScript == other.fScript &&
                hashAddress == other.hashAddress &&
                nAmount == other.nAmount);
    }
    inline bool operator!=(const CCustodianVote& other) const
    {
        return !(*this == other);
    }

    class CDestinationVisitor : public boost::static_visitor<bool>
    {
        private:
            CCustodianVote *custodianVote;
        public:
            CDestinationVisitor(CCustodianVote *custodianVote) : custodianVote(custodianVote) { }

            bool operator()(const CNoDestination &dest) const {
                custodianVote->fScript = false;
                custodianVote->hashAddress = 0;
                return false;
            }

            bool operator()(const CKeyID &keyID) const {
                custodianVote->fScript = false;
                custodianVote->hashAddress = keyID;
                return true;
            }

            bool operator()(const CScriptID &scriptID) const {
                custodianVote->fScript = true;
                custodianVote->hashAddress = scriptID;
                return true;
            }
    };

    void SetAddress(const CBitcoinAddress& address)
    {
        cUnit = address.GetUnit();
        CTxDestination destination = address.Get();
        boost::apply_visitor(CDestinationVisitor(this), destination);
    }

    CBitcoinAddress GetAddress() const
    {
        CBitcoinAddress address;
        if (fScript)
            address.Set(CScriptID(hashAddress), cUnit);
        else
            address.Set(CKeyID(hashAddress), cUnit);
        return address;
    }

    bool operator< (const CCustodianVote& other) const
    {
        if (cUnit < other.cUnit)
            return true;
        if (cUnit > other.cUnit)
            return false;
        if (nAmount < other.nAmount)
            return true;
        if (nAmount > other.nAmount)
            return false;
        if (fScript < other.fScript)
            return true;
        if (fScript > other.fScript)
            return false;
        if (hashAddress < other.hashAddress)
            return true;
        return false;
    }
};

inline bool CompactDurationRange(unsigned char nCompactDuration)
{
    return (nCompactDuration < MAX_COMPACT_DURATION);
}

inline bool ParkDurationRange(int64 nDuration)
{
    return (nDuration >= 1 && nDuration <= MAX_PARK_DURATION);
}

inline bool ParkRateRange(int64 nRate)
{
    return (nRate >= 0 && nRate <= MAX_PARK_RATE);
}

inline int64 CompactDurationToDuration(unsigned char nCompactDuration)
{
    return 1 << nCompactDuration;
}

class CParkRate
{
public:
    unsigned char nCompactDuration;
    int64 nRate;

    CParkRate() :
        nCompactDuration(0),
        nRate(0)
    {
    }

    CParkRate(unsigned char nCompactDuration, int64 nRate) :
        nCompactDuration(nCompactDuration),
        nRate(nRate)
    {
    }

    bool IsValid() const;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nCompactDuration);
        READWRITE(nRate);
    )

    int64 GetDuration() const
    {
        if (!CompactDurationRange(nCompactDuration))
            throw std::runtime_error("Park rate compact duration out of range");
        return CompactDurationToDuration(nCompactDuration);
    }

    friend bool operator==(const CParkRate& a, const CParkRate& b)
    {
        return (a.nCompactDuration == b.nCompactDuration &&
                a.nRate == b.nRate);
    }

    bool operator<(const CParkRate& other) const
    {
        if (nCompactDuration < other.nCompactDuration)
            return true;
        if (nCompactDuration > other.nCompactDuration)
            return false;
        return nRate < other.nRate;
    }
};

class CParkRateVote
{
public:
    unsigned char cUnit;
    std::vector<CParkRate> vParkRate;

    CParkRateVote() :
        cUnit('?')
    {
    }

    void SetNull()
    {
        cUnit = '?';
        vParkRate.clear();
    }

    bool IsValid() const;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(cUnit);
        READWRITE(vParkRate);
    )

    CScript ToParkRateResultScript() const;

    friend bool operator==(const CParkRateVote& a, const CParkRateVote& b)
    {
        return (a.cUnit     == b.cUnit &&
                a.vParkRate == b.vParkRate);
    }

    std::string ToString() const;
};

class CReputationVote
{
public:
    bool fScript;
    uint160 hashAddress;
    char nWeight;

    CReputationVote() :
        fScript(false),
        hashAddress(0),
        nWeight(0)
    {
    }

    CReputationVote(const CBitcoinAddress& address, char nWeight) :
        nWeight(nWeight)
    {
        SetAddress(address);
    }

    bool IsValid(int nProtocolVersion) const;

    IMPLEMENT_SERIALIZE
    (
        READWRITE(fScript);
        READWRITE(hashAddress);
        READWRITE(nWeight);
    )

    inline bool operator==(const CReputationVote& other) const
    {
        return (fScript == other.fScript &&
                hashAddress == other.hashAddress &&
                nWeight == other.nWeight);
    }
    inline bool operator!=(const CReputationVote& other) const
    {
        return !(*this == other);
    }

    class CDestinationVisitor : public boost::static_visitor<bool>
    {
        private:
            CReputationVote *reputationVote;
        public:
            CDestinationVisitor(CReputationVote *reputationVote) : reputationVote(reputationVote) { }

            bool operator()(const CNoDestination &dest) const {
                reputationVote->fScript = false;
                reputationVote->hashAddress = 0;
                return false;
            }

            bool operator()(const CKeyID &keyID) const {
                reputationVote->fScript = false;
                reputationVote->hashAddress = keyID;
                return true;
            }

            bool operator()(const CScriptID &scriptID) const {
                reputationVote->fScript = true;
                reputationVote->hashAddress = scriptID;
                return true;
            }
    };

    void SetAddress(const CBitcoinAddress& address)
    {
        if (address.GetUnit() != '8')
            throw std::runtime_error("Invalid unit on reputation vote");
        CTxDestination destination = address.Get();
        boost::apply_visitor(CDestinationVisitor(this), destination);
    }

    CBitcoinAddress GetAddress() const
    {
        CBitcoinAddress address;
        if (fScript)
            address.Set(CScriptID(hashAddress), '8');
        else
            address.Set(CKeyID(hashAddress), '8');
        return address;
    }

    bool operator< (const CReputationVote& other) const
    {
        if (nWeight < other.nWeight)
            return true;
        if (nWeight > other.nWeight)
            return false;
        if (fScript < other.fScript)
            return true;
        if (fScript > other.fScript)
            return false;
        if (hashAddress < other.hashAddress)
            return true;
        return false;
    }
};

class CAssetVote
{
public:
    int nBlockchainId;
    int nAssetId;
    uint16_t nNumberOfConfirmations;
    uint8_t nRequiredDepositSigners;
    uint8_t nTotalDepositSigners;
    uint8_t nMaxTradeExpParam;

    CAssetVote() :
            nBlockchainId(-1),
            nAssetId(-1),
            nNumberOfConfirmations(0),
            nRequiredDepositSigners(0),
            nTotalDepositSigners(0),
            nMaxTradeExpParam(0)
    {
    }

    bool IsValid(int nProtocolVersion) const
    {
        return (nProtocolVersion >= PROTOCOL_V3_1 &&
                nBlockchainId >= 0 && nBlockchainId <= MAX_BLOCKCHAIN_ID &&
                nAssetId >= 0 && nAssetId <= MAX_ASSET_ID &&
                nNumberOfConfirmations > 0 &&
                nRequiredDepositSigners >= MIN_REQ_SIGNERS &&
                nTotalDepositSigners >= MIN_TOTAL_SIGNERS &&
                nRequiredDepositSigners < nTotalDepositSigners &&
                nMaxTradeExpParam > 0 && nMaxTradeExpParam <= EXP_SERIES_MAX_PARAM);
    }

    inline uint64 GetGlobalId() const
    {
        return AssetGlobalId(nBlockchainId, nAssetId);
    }

    inline int64 GetMaxTrade() const
    {
        return nMaxTradeExpParam > 0 && nMaxTradeExpParam <= EXP_SERIES_MAX_PARAM ?
               pnExponentialSeries[nMaxTradeExpParam] : 0;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nBlockchainId);
        READWRITE(nAssetId);
        READWRITE(nNumberOfConfirmations);
        READWRITE(nRequiredDepositSigners);
        READWRITE(nTotalDepositSigners);
        READWRITE(nMaxTradeExpParam);
    )

    inline bool operator==(const CAssetVote& other) const
    {
        return (nBlockchainId == other.nBlockchainId &&
                nAssetId == other.nAssetId &&
                nNumberOfConfirmations == other.nNumberOfConfirmations &&
                nRequiredDepositSigners == other.nRequiredDepositSigners &&
                nTotalDepositSigners == other.nTotalDepositSigners &&
                nMaxTradeExpParam == other.nMaxTradeExpParam);
    }
    inline bool operator!=(const CAssetVote& other) const
    {
        return !(*this == other);
    }
    bool operator< (const CAssetVote& other) const
    {
        if (nBlockchainId != other.nBlockchainId)
            return nBlockchainId < other.nBlockchainId;
        if (nAssetId != other.nAssetId)
            return nAssetId < other.nAssetId;
        if (nNumberOfConfirmations != other.nNumberOfConfirmations)
            return nNumberOfConfirmations < other.nNumberOfConfirmations;
        if (nRequiredDepositSigners != other.nRequiredDepositSigners)
            return nRequiredDepositSigners < other.nRequiredDepositSigners;
        if (nTotalDepositSigners != other.nTotalDepositSigners)
            return nTotalDepositSigners < other.nTotalDepositSigners;
        if (nMaxTradeExpParam != other.nMaxTradeExpParam)
            return nMaxTradeExpParam < other.nMaxTradeExpParam;
        return false;
    }

    bool ProducesAsset(const CAsset& asset) const
    {
        return (nBlockchainId == asset.nBlockchainId &&
                nAssetId == asset.nAssetId &&
                nNumberOfConfirmations == asset.nNumberOfConfirmations &&
                nRequiredDepositSigners == asset.nRequiredDepositSigners &&
                nTotalDepositSigners == asset.nTotalDepositSigners &&
                GetMaxTrade() == asset.nMaxTrade);
    }
};

class CVote
{
public:
    int nVersionVote;
    std::vector<CCustodianVote> vCustodianVote;
    std::vector<CParkRateVote> vParkRateVote;
    std::vector<uint160> vMotion;
    std::map<unsigned char, uint32_t> mapFeeVote;
    std::vector<CReputationVote> vReputationVote;
    std::vector<CAssetVote> vAssetVote;

    int64 nCoinAgeDestroyed;

    CVote() :
        nVersionVote(PROTOCOL_VERSION),
        nCoinAgeDestroyed(0)
    {
    }

    void SetNull()
    {
        nVersionVote = 0;
        vCustodianVote.clear();
        vParkRateVote.clear();
        vMotion.clear();
        mapFeeVote.clear();
        vReputationVote.clear();
        vAssetVote.clear();
        nCoinAgeDestroyed = 0;
    }

    template<typename Stream>
    unsigned int ReadWriteSingleMotion(Stream& s, int nType, int nVersion, CSerActionGetSerializeSize ser_action) const
    {
        uint160 hashMotion;
        return ::SerReadWrite(s, hashMotion, nType, nVersion, ser_action);
    }

    template<typename Stream>
    unsigned int ReadWriteSingleMotion(Stream& s, int nType, int nVersion, CSerActionSerialize ser_action) const
    {
        uint160 hashMotion;
        switch (vMotion.size())
        {
            case 0:
                hashMotion = 0;
                break;
            case 1:
                hashMotion = vMotion[0];
                break;
            default:
                printf("Warning: serializing multiple motion votes in a vote structure not supporting it. Only the first motion is serialized.\n");
                hashMotion = vMotion[0];
                break;
        }
        return ::SerReadWrite(s, hashMotion, nType, nVersion, ser_action);
    }

    template<typename Stream>
    unsigned int ReadWriteSingleMotion(Stream& s, int nType, int nVersion, CSerActionUnserialize ser_action)
    {
        uint160 hashMotion;
        unsigned int result = ::SerReadWrite(s, hashMotion, nType, nVersion, ser_action);
        vMotion.clear();
        if (hashMotion != 0)
            vMotion.push_back(hashMotion);
        return result;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nVersionVote);
        // Before v2.0 the nVersionVote was used to specify the serialization version
        if (nVersionVote < PROTOCOL_V2_0)
        {
            nVersion = nVersionVote;
            if (fRead) const_cast<CVote*>(this)->nVersionVote = 0;
        }

        READWRITE(vCustodianVote);
        READWRITE(vParkRateVote);
        if (nVersion >= PROTOCOL_V0_5)
            READWRITE(vMotion);
        else
            ReadWriteSingleMotion(s, nType, nVersion, ser_action);
        if (nVersion >= PROTOCOL_V2_0)
            READWRITE(mapFeeVote);
        else if (fRead)
            const_cast<CVote*>(this)->mapFeeVote.clear();

        if (nVersion >= PROTOCOL_V4_0)
        {
            READWRITE(vReputationVote);
            READWRITE(vAssetVote);
        }
        else if (fRead)
        {
            const_cast<CVote*>(this)->vReputationVote.clear();
            const_cast<CVote*>(this)->vAssetVote.clear();
        }
    )

    CScript ToScript(int nProtocolVersion) const;

    bool IsValid(int nProtocolVersion) const;
    bool IsValidInBlock(int nProtocolVersion) const;

    inline bool operator==(const CVote& other) const
    {
        return (nVersionVote == other.nVersionVote &&
                vCustodianVote == other.vCustodianVote &&
                vParkRateVote == other.vParkRateVote &&
                vMotion == other.vMotion &&
                mapFeeVote == other.mapFeeVote &&
                vReputationVote == other.vReputationVote) &&
                vAssetVote == other.vAssetVote;
    }
    inline bool operator!=(const CVote& other) const
    {
        return !(*this == other);
    }
};

class CUserVote : public CVote
{
public:
    int nVersion;

    CUserVote() :
        nVersion(PROTOCOL_VERSION)
    {
    }

    CUserVote(const CVote& vote, int nVersion = PROTOCOL_VERSION) :
        CVote(vote),
        nVersion(nVersion)
    {
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        nSerSize += SerReadWrite(s, *(CVote*)this, nType, nVersion, ser_action);
    )

    void Upgrade()
    {
        nVersion = PROTOCOL_VERSION;
    }

    CVote GenerateBlockVote(int nProtocolVersion) const;

    inline bool operator==(const CUserVote& other) const
    {
        return (nVersion == other.nVersion &&
                CVote::operator==(other));
    }
    inline bool operator!=(const CUserVote& other) const
    {
        return !(*this == other);
    }
};

bool IsVote(const CScript& scriptPubKey);
bool ExtractVote(const CScript& scriptPubKey, CVote& voteRet, int nProtocolVersion);
bool ExtractVote(const CBlock& block, CVote& voteRet, int nProtocolVersion);
bool ExtractVotes(const CBlock &block, const CBlockIndex *pindexprev, unsigned int nCount, std::vector<CVote> &vVoteResult);

bool IsParkRateResult(const CScript& scriptPubKey);
bool ExtractParkRateResult(const CScript& scriptPubKey, CParkRateVote& parkRateResultRet);
bool ExtractParkRateResults(const CBlock& block, std::vector<CParkRateVote>& vParkRateResultRet);

bool CalculateParkRateVote(const std::vector<CVote>& vVote, std::vector<CParkRateVote>& results);
bool LimitParkRateChangeV0_5(std::vector<CParkRateVote>& results, const std::map<unsigned char, std::vector<const CParkRateVote*> >& mapPreviousVotes);
bool LimitParkRateChangeV2_0(std::vector<CParkRateVote>& results, const std::map<unsigned char, const CParkRateVote*>& mapPreviousVotedRate);
bool CalculateParkRateResults(const CVote &vote, const CBlockIndex* pindexprev, int nProtocolVersion, std::vector<CParkRateVote>& vParkRateResult);
int64 GetPremium(int64 nValue, int64 nDuration, unsigned char cUnit, const std::vector<CParkRateVote>& vParkRateResult);

bool GenerateCurrencyCoinBases(const std::vector<CVote> vVote, const std::map<CBitcoinAddress, CBlockIndex*>& mapAlreadyElected, std::vector<CTransaction>& vCurrencyCoinBaseRet);

int GetProtocolForNextBlock(const CBlockIndex* pPrevIndex);
bool IsProtocolActiveForNextBlock(const CBlockIndex* pPrevIndex, int nSwitchTime, int nProtocolVersion, int nRequired=PROTOCOL_SWITCH_REQUIRE_VOTES, int nToCheck=PROTOCOL_SWITCH_COUNT_VOTES);

bool CalculateVotedFees(CBlockIndex* pindex);

bool CalculateReputationResult(const CBlockIndex* pindex, std::map<CBitcoinAddress, int64>& mapReputation);

bool ExtractAssetVoteResult(const CBlockIndex *pindex, std::vector<CAsset> &vAssets);
bool CalculateVotedAssets(CBlockIndex* pindex);

#endif
