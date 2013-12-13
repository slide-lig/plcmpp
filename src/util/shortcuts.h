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

namespace util {

typedef vector<int32_t> vec_int32;
typedef shared_ptr<vec_int32> shp_vec_int32;
typedef vec_int32* p_vec_int32;

}
