#pragma once
#include <map>
//#include <unordered_map>
#include <shared_mutex>
#include <mutex>

template<typename Key, typename Val>
class ThreadSafeMap
{
    std::map<Key, Val> map_;
    mutable std::shared_mutex mutex_;

public:
    using iterator = typename std::map<Key, Val>::iterator;

    template<typename ...Args>
    std::pair<iterator, bool> get_or_create(const Key& key, Args&&... args) {
        {
            std::shared_lock lock{mutex_};
            auto it = map_.find(key);
            if(it != map_.end()) {
                return {it, false};
            }
        }
        {
            std::scoped_lock lock{mutex_};
            return map_.try_emplace(key, std::forward<Args>(args)...);
        }
    }

    template<class Func>
    void for_each(Func func) const {
        std::shared_lock lock{mutex_};
        for(const auto& e : map_) {
            func(e.first, e.second);
        }
    }    
};
