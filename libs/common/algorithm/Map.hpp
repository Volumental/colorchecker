#pragma once

#include <common/Logging.hpp>

#include "Vector.hpp"

namespace komb {

/// like map.at(key) but dies (instead of throws) if it fails to find the key.
template<class MapType, typename KeyType>
const typename MapType::mapped_type& getOrDie(const MapType& map, const KeyType& key)
{
    auto it = map.find(key);
    CHECK(it != map.end()) << "Failed to find key in map: '" << key << "'";
    return it->second;
}

/// like map.at(key) but dies (instead of throws) if it fails to find the key.
template<class MapType, typename KeyType>
typename MapType::mapped_type& getOrDie(MapType& map, const KeyType& key)
{
    auto it = map.find(key);
    CHECK(it != map.end()) << "Failed to find key in map: '" << key << "'";
    return it->second;
}

/// like map[key] but does not insert anything.
template<class MapType, typename KeyType>
typename MapType::mapped_type getOrDefault(const MapType& map, const KeyType& key)
{
    auto it = map.find(key);
    if (it != map.end())
    {
        return it->second;
    }
    return {};
}

template<typename Map>
auto keys(const Map& input)
{
    return mapVector(input, [](const auto& it) { return it.first; });
}

template<typename Map>
auto values(const Map& input)
{
    return mapVector(input, [](const auto& it) { return it.second; });
}

} // namespace komb
