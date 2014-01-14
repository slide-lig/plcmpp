#pragma once

#include <cstdint>
#include <vector>
using namespace std;

#include "util/shortcuts.h"

namespace util {

/**
 * Itemsets and patterns are represented by classic integer arrays
 * Aside static utility methods, instanciate it to create arrays
 * without knowing their length beforehand
 */
class ItemsetsFactory
{
public:
    static void extendRename(vector<int32_t>& closure, int32_t extension, vector<int32_t>& pattern, array_int32& renaming, vector<int32_t>& result);
};

}
