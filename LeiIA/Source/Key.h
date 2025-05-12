// © 2023 Lei Cheng

#pragma once

#include <string>
#include <utility>
#include <functional>

namespace lei
{
    enum class DataFrequency;
}

template<>
struct std::hash<std::pair<std::string, lei::DataFrequency>>
{
    std::size_t operator()(const std::pair<std::string, lei::DataFrequency>& key) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(key.first);
        std::size_t h2 = std::hash<std::underlying_type_t<lei::DataFrequency>>{}(static_cast<std::underlying_type_t<lei::DataFrequency>>(key.second));

        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
