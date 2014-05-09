/*******************************************************************************
 * Copyright (c) 2014 Etienne Dublé, Martin Kirchgessner, Vincent Leroy, Alexandre Termier, CNRS and Université Joseph Fourier.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the LICENSE.txt file joined with this program.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/
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
