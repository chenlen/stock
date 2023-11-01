// © 2023 Lei Cheng

#pragma once

#include <boost/functional/hash.hpp>

namespace lei
{
    enum class DataFrequency;
}

template<>
struct std::hash<std::pair<std::string, lei::DataFrequency>>
{
    std::size_t operator()(const std::pair<std::string, lei::DataFrequency>& key) const noexcept
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, key.first);
        boost::hash_combine(seed, key.second);
        return seed;
    }
};
