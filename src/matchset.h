#ifndef __MATCHSET_H__
#define __MATCHSET_H__

#include <array>
#include <cstdint>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>
#include "skiplist.h"

class Player
{
public:
    Player(std::uint64_t uid,
           std::uint64_t score,
           std::vector<std::string> &&attrs,
           std::vector<std::string> &&bans)
        : uid_(uid),
          score_(score),
          attrs_(std::move(attrs)),
          bans_(std::move(bans))
    {
        std::sort(attrs_.begin(), attrs_.end());
        std::sort(bans_.begin(), bans_.end());
    }

    std::int64_t operator-(const Player &player) const
    {
        if (uid_ == player.GetUid())
        {
            return 0;
        }
        return GetScore() - player.GetScore();
    }
    int operator%(const Player &player) const
    {
        std::vector<std::string> intersaction;
        std::set_intersection(attrs_.begin(),
                              attrs_.end(),
                              player.GetBans().begin(),
                              player.GetBans().end(),
                              std::back_inserter(intersaction));
        return intersaction.size();
    }
    uint64_t GetUid() const
    {
        return uid_;
    }
    uint64_t GetScore() const
    {
        return score_;
    }
    const std::vector<std::string> &GetAttrs() const
    {
        return attrs_;
    }
    const std::vector<std::string> &GetBans() const
    {
        return bans_;
    }
    void UpdateScore(std::uint64_t score)
    {
        score_ = score;
    }

private:
    std::uint64_t uid_;
    std::uint64_t score_;
    std::vector<std::string> attrs_;
    std::vector<std::string> bans_;
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
    MatchSet(PlayerComparator &pcmp, Player &head)
        : sl_(new SkipList<Player, PlayerComparator>(pcmp, head))
    {
    }
    std::array<std::uint64_t, 2> Match(std::uint64_t uid,
                                       std::uint64_t score,
                                       std::uint64_t limit,
                                       std::vector<std::string> &&attrs,
                                       std::vector<std::string> &&bans)
    {
        bool fresher = true;
        Player *self = nullptr;
        if (dict_.find(uid) == dict_.end())
        {
            self = new Player(uid, score, std::move(attrs), std::move(bans));
        }
        else
        {
            self = dict_[uid];
            if (self->GetScore() != score)
            {
                sl_->Delete(*dict_[uid]);
                self->UpdateScore(score);
            }
            fresher = false;
        }
        auto peer = sl_->Match(*self, limit);
        if (peer == nullptr)
        {
            if (fresher)
            {
                dict_[uid] = self;
            }
            return {uid, 0};
        }
        const uint64_t peerid = peer->key.GetUid();
        sl_->Delete(*self);
        sl_->Delete(peer->key);
        delete dict_[peerid];
        dict_.erase(peerid);
        if (fresher)
        {
            delete self;
        }
        else
        {
            delete dict_[uid];
        }
        dict_.erase(uid);

        return {uid, peerid};
    }

    bool Cancel(std::uint64_t uid)
    {
        if (dict_.find(uid) != dict_.end())
        {
            sl_->Delete(*dict_[uid]);
            delete dict_[uid];
            dict_.erase(uid);
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    SkipList<Player, PlayerComparator> *sl_;
    std::unordered_map<std::uint64_t, Player *> dict_;
};
#endif