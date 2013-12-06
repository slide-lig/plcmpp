#pragma once

#include <cstdint>
#include <vector>

using namespace std;

namespace util {

/**
 * Itemsets and patterns are represented by classic integer arrays
 * Aside static utility methods, instanciate it to create arrays
 * without knowing their length beforehand
 */
class ItemsetsFactory : public vector<int32_t>
{
    static void extend(vector<int32_t>& pattern, int32_t extension, vector<int32_t>& closure, vector<int32_t>& result);
    static void extendRename(vector<int32_t>& closure, int32_t extension, vector<int32_t>& pattern, vector<int32_t>& renaming, vector<int32_t>& result);
    static void extend(vector<int32_t>& pattern, int32_t extension, vector<int32_t>& closure, vector<int32_t>& ignoreItems, vector<int32_t>& result);
    static void extend(vector<int32_t>& closure, int32_t extension, vector<int32_t>& result);
};

}
