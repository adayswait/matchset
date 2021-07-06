#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__
#include <cassert>
#include <vector>
#include "random.h"

template <typename Key, class Comparator>
class SkipList
{
private:
    struct Node;

public:
    explicit SkipList(Comparator cmp, Key &headKey);
    SkipList(const SkipList &) = delete;
    SkipList &operator=(const SkipList &) = delete;
    ~SkipList()
    {
        auto currNode = head_;
        while (currNode != nullptr)
        {
            auto nextNode = currNode->Next(0);
            delete currNode;
            currNode = nextNode;
        }
    }
    void Insert(const Key &key);
    void Delete(const Key &key);
    bool Contains(const Key &key) const;
    Node *Match(const Key &key);
    class Iterator
    {
    public:
        explicit Iterator(const SkipList *list);
        bool Valid() const;
        const Key &key() const;
        void Next();
        void Prev();
        void Seek(const Key &target);
        void SeekToFirst();
        void SeekToLast();

    private:
        const SkipList *list_;
        Node *node_;
    };

private:
    int RandomHeight();
    bool Equal(const Key &a, const Key &b) const
    {
        return (compare_(a, b) == 0);
    }
    bool KeyIsAfterNode(const Key &key, Node *n) const;
    Node *FindGreaterOrEqual(const Key &key, Node **prev) const;
    Node *FindLessThan(const Key &key) const;
    Node *FindLast() const;

    enum
    {
        kMaxHeight = 12
    };
    int max_height_ = 1;
    Comparator const compare_;
    Node *const head_;
    Random rnd_;
};

template <typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node
{
    explicit Node(const Key &k, int height) : key(k), height_(height)
    {
        next_.reserve(height);
    }
    Key const key;
    bool invalid_ = false;

    Node *Next(int n)
    {
        assert(n >= 0);
        return next_[n];
    }
    void SetNext(int n, Node *x)
    {
        assert(n >= 0);
        next_[n] = x;
    }

private:
    std::vector<Node *> next_;
    int height_;
};

template <typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator cmp, Key &headKey)
    : compare_(cmp),
      head_(new Node(headKey, kMaxHeight)),
      max_height_(1),
      rnd_(0xdeadbeef)
{
    for (int i = 0; i < kMaxHeight; i++)
    {
        head_->SetNext(i, nullptr);
    }
}

template <typename Key, class Comparator>
int SkipList<Key, Comparator>::RandomHeight()
{
    static const unsigned int kBranching = 4;
    int height = 1;
    while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0))
    {
        height++;
    }
    assert(height > 0);
    assert(height <= kMaxHeight);
    return height;
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(const Key &key)
{
    Node *prev[kMaxHeight];
    Node *x = FindGreaterOrEqual(key, prev);

    assert(x == nullptr || !Equal(key, x->key));

    int height = RandomHeight();
    if (height > max_height_)
    {
        for (int i = max_height_; i < height; i++)
        {
            prev[i] = head_;
        }
        max_height_ = height;
    }

    x = new Node(key, height);
    for (int i = 0; i < height; i++)
    {
        x->SetNext(i, prev[i]->Next(i));
        prev[i]->SetNext(i, x);
    }
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Delete(const Key &key)
{
    Node *prev[kMaxHeight];
    for (int i = 0; i < kMaxHeight; i++)
    {
        prev[i] = nullptr;
    }
    Node *x = FindGreaterOrEqual(key, prev);
    if (x == nullptr || !Equal(key, x->key))
    {
        return;
    }
    for (int i = 0; i < kMaxHeight; i++)
    {
        if (prev[i] == nullptr)
        {
            break;
        }
        prev[i]->SetNext(i, x->Next(i));
    }
    delete x;
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *
SkipList<Key, Comparator>::Match(const Key &key)
{
    Node *lt = FindLessThan(key);
    Node *gt = nullptr;
    if (lt == nullptr)
    {
        gt = FindGreaterOrEqual(key, nullptr);
    }
    else
    {
        gt = lt->Next(0);
    }

    if (gt != nullptr && compare_(gt->key, key) == 0)
    {
        gt = gt->Next(0);
    }
    if (gt != nullptr && lt != nullptr)
    {
        if ((gt->key - key) <= (key - lt->key))
        {
            gt->invalid_ = true;
            return gt;
        }
        else
        {
            lt->invalid_ = true;
            return lt;
        }
    }
    if (gt != nullptr)
    {
        gt->invalid_ = true;
        return gt;
    }
    if (lt != nullptr)
    {
        lt->invalid_ = true;
        return lt;
    }
    Insert(key);
    return nullptr;
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key &key, Node *n) const
{
    return (n != nullptr) && (compare_(n->key, key) < 0);
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *
SkipList<Key, Comparator>::FindGreaterOrEqual(const Key &key,
                                              Node **prev) const
{
    Node *x = head_;
    int level = max_height_ - 1;
    while (true)
    {
        Node *next = x->Next(level);
        if (KeyIsAfterNode(key, next))
        {
            x = next;
        }
        else
        {
            if (prev != nullptr)
                prev[level] = x;
            if (level == 0)
            {
                return next;
            }
            else
            {
                level--;
            }
        }
    }
}
template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node *
SkipList<Key, Comparator>::FindLessThan(const Key &key) const
{
    Node *x = head_;
    int level = max_height_ - 1;
    while (true)
    {
        assert(x == head_ || compare_(x->key, key) < 0);
        Node *next = x->Next(level);
        if (next == nullptr || compare_(next->key, key) >= 0)
        {
            if (level == 0)
            {
                if (x == head_)
                {
                    return nullptr;
                }
                return x;
            }
            else
            {
                level--;
            }
        }
        else
        {
            x = next;
        }
    }
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(const Key &key) const
{
    Node *x = FindGreaterOrEqual(key, nullptr);
    if (x != nullptr && Equal(key, x->key))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename Key, class Comparator>
inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList *list)
{
    list_ = list;
    node_ = nullptr;
}

template <typename Key, class Comparator>
inline bool SkipList<Key, Comparator>::Iterator::Valid() const
{
    return node_ != nullptr;
}

template <typename Key, class Comparator>
inline const Key &SkipList<Key, Comparator>::Iterator::key() const
{
    assert(Valid());
    return node_->key;
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Next()
{
    assert(Valid());
    node_ = node_->Next(0);
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Prev()
{
    assert(Valid());
    node_ = list_->FindLessThan(node_->key);
    if (node_ == list_->head_)
    {
        node_ = nullptr;
    }
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::Seek(const Key &target)
{
    node_ = list_->FindGreaterOrEqual(target, nullptr);
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToFirst()
{
    node_ = list_->head_->Next(0);
}

template <typename Key, class Comparator>
inline void SkipList<Key, Comparator>::Iterator::SeekToLast()
{
    node_ = list_->FindLast();
    if (node_ == list_->head_)
    {
        node_ = nullptr;
    }
}

#endif
