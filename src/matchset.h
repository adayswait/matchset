#ifndef __MATCHSET_H__
#define __MATCHSET_H__

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "skiplist.h"

class Player
{
public:
    Player(std::uint64_t uid, std::uint64_t score) : uid_(uid), score_(score)
    {
    }

    std::int64_t operator-(const Player &player) const
    {
        if (uid_ == player.GetUid())
        {
            return 0;
        }
        return GetScore() - player.GetScore();
    }
    uint64_t GetUid() const
    {
        return uid_;
    }
    uint64_t GetScore() const
    {
        return score_;
    }

private:
    std::uint64_t uid_;
    std::uint64_t score_;
};

struct PlayerComparator
{
    int operator()(const Player &a, const Player &b) const
    {
        if (a.GetUid() == b.GetUid())
        {
            return 0;
        }
        if (a.GetScore() < b.GetScore())
        {
            return -1;
        }
        else if (a.GetScore() > b.GetScore())
        {
            return +1;
        }
        else
        {
            return a.GetUid() - b.GetUid();
        }
    }
};

class MatchSet
{
public:
    MatchSet(PlayerComparator pcmp, Player &head)
        : sl_(new SkipList<Player, PlayerComparator>(pcmp, head))
    {
    }
    std::array<std::uint64_t, 2> Match(std::uint64_t uid,
                                       std::uint64_t score,
                                       std::uint64_t limit)
    {

        dict_[uid] = new Player(uid, score);
        auto peer = sl_->Match(*dict_[uid], limit);
        if (peer == nullptr)
        {
            return {uid, 0};
        }
        const uint64_t ret = peer->key.GetUid();
        sl_->Delete(*(dict_[uid]));
        sl_->Delete(peer->key);
        delete dict_[ret];
        dict_.erase(ret);

        return {uid, ret};
    }

    void Cancel(std::uint64_t uid)
    {
        if (dict_.find(uid) != dict_.end())
        {
            sl_->Delete(*dict_[uid]);
            delete dict_[uid];
            dict_.erase(uid);
        }
    }

private:
    SkipList<Player, PlayerComparator> *sl_;
    std::unordered_map<std::uint64_t, Player *> dict_;
};
#endif