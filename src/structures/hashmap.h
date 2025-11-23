#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <functional>

using namespace std;

template<typename Key, typename T>
class HashMap {
private:
    static const size_t INITIAL_BUCKETS = 16;
    static constexpr double LOAD_FACTOR = 0.75;

    struct Node {
        Key key;
        T value;
        Node(const Key& k, const T& v) : key(k), value(v) {}
    };

    vector<list<Node>> buckets;
    size_t size_;

    size_t get_bucket_index(const Key& key) const {
        hash<Key> hasher;
        return hasher(key) % buckets.size();
    }

    void rehash() {
        vector<list<Node>> old_buckets = buckets;
        buckets.resize(buckets.size() * 2);
        for (auto& bucket : buckets) bucket.clear();
        size_ = 0;

        for (const auto& bucket : old_buckets) {
            for (const auto& node : bucket) {
                insert(node.key, node.value);
            }
        }
    }

public:
    HashMap() : buckets(INITIAL_BUCKETS), size_(0) {}

    void insert(const Key& key, const T& value) {
        if ((double)size_ / buckets.size() >= LOAD_FACTOR) {
            rehash();
        }

        size_t index = get_bucket_index(key);
        for (auto& node : buckets[index]) {
            if (node.key == key) {
                node.value = value;
                return;
            }
        }

        buckets[index].emplace_back(key, value);
        size_++;
    }

    T& operator[](const Key& key) {
        size_t index = get_bucket_index(key);
        for (auto& node : buckets[index]) {
            if (node.key == key) {
                return node.value;
            }
        }

        if ((double)size_ / buckets.size() >= LOAD_FACTOR) {
            rehash();
            index = get_bucket_index(key);
        }

        buckets[index].emplace_back(key, T());
        size_++;
        return buckets[index].back().value;
    }

    bool contains(const Key& key) const {
        size_t index = get_bucket_index(key);
        for (const auto& node : buckets[index]) {
            if (node.key == key) return true;
        }
        return false;
    }

    void erase(const Key& key) {
        size_t index = get_bucket_index(key);
        auto& bucket = buckets[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                size_--;
                return;
            }
        }
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    // Simple iterator to traverse all elements
    class iterator {
        typename vector<list<Node>>::iterator bucket_it;
        typename vector<list<Node>>::iterator bucket_end;
        typename list<Node>::iterator list_it;

    public:
        iterator(typename vector<list<Node>>::iterator start, typename vector<list<Node>>::iterator end)
            : bucket_it(start), bucket_end(end) {
            if (bucket_it != bucket_end) {
                list_it = bucket_it->begin();
                advance_to_valid();
            }
        }

        iterator() : bucket_it(), bucket_end() {} // End iterator

        void advance_to_valid() {
            while (bucket_it != bucket_end && list_it == bucket_it->end()) {
                ++bucket_it;
                if (bucket_it != bucket_end) {
                    list_it = bucket_it->begin();
                }
            }
        }

        pair<const Key&, T&> operator*() {
            return {list_it->key, list_it->value};
        }

        iterator& operator++() {
            ++list_it;
            advance_to_valid();
            return *this;
        }

        bool operator!=(const iterator& other) const {
            if (bucket_it == bucket_end && other.bucket_it == other.bucket_end) return false;
            return bucket_it != other.bucket_it || (bucket_it != bucket_end && list_it != other.list_it);
        }
    };

    class const_iterator {
        typename vector<list<Node>>::const_iterator bucket_it;
        typename vector<list<Node>>::const_iterator bucket_end;
        typename list<Node>::const_iterator list_it;

    public:
        const_iterator(typename vector<list<Node>>::const_iterator start, typename vector<list<Node>>::const_iterator end)
            : bucket_it(start), bucket_end(end) {
            if (bucket_it != bucket_end) {
                list_it = bucket_it->begin();
                advance_to_valid();
            }
        }

        const_iterator() : bucket_it(), bucket_end() {} // End iterator

        void advance_to_valid() {
            while (bucket_it != bucket_end && list_it == bucket_it->end()) {
                ++bucket_it;
                if (bucket_it != bucket_end) {
                    list_it = bucket_it->begin();
                }
            }
        }

        pair<const Key&, const T&> operator*() const {
            return {list_it->key, list_it->value};
        }

        const_iterator& operator++() {
            ++list_it;
            advance_to_valid();
            return *this;
        }

        bool operator!=(const const_iterator& other) const {
            if (bucket_it == bucket_end && other.bucket_it == other.bucket_end) return false;
            return bucket_it != other.bucket_it || (bucket_it != bucket_end && list_it != other.list_it);
        }
    };

    iterator begin() {
        return iterator(buckets.begin(), buckets.end());
    }

    iterator end() {
        return iterator(buckets.end(), buckets.end());
    }

    const_iterator begin() const {
        return const_iterator(buckets.begin(), buckets.end());
    }

    const_iterator end() const {
        return const_iterator(buckets.end(), buckets.end());
    }
};

#endif
