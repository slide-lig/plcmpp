/*
 * shortcuts.h
 *
 *  Created on: 13 déc. 2013
 *      Author: etienne
 */

#pragma once

#include <vector>
#include <cstdint>
#include <memory>
using namespace std;

#include "util/RawArray.hpp"

namespace util {

typedef RawArray<int32_t> array_int32;
typedef shared_ptr<array_int32> shp_array_int32;
typedef array_int32* p_array_int32;

typedef vector<int32_t> vec_int32;
typedef shared_ptr<vec_int32> shp_vec_int32;
typedef vec_int32* p_vec_int32;

}
