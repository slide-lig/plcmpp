/*
 * Helpers.cpp
 *
 *  Created on: 17 déc. 2013
 *      Author: etienne
 */

#include <ctime>

#include <util/Helpers.h>

namespace util {

string Helpers::formatted_time() {

	time_t t = std::time(nullptr);
    tm tm = *std::localtime(&t);

    // not very beautiful, but libstd++ does not implement std::put_time() yet.
    char formatted_time[20];
    std::strftime(formatted_time, sizeof(formatted_time), "%F %T", &tm);

    return string(formatted_time);
}

} /* namespace util */

