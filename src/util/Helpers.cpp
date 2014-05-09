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

double Helpers::precise_time() {
	struct timespec curr_time;
	clock_gettime(CLOCK_MONOTONIC,  &curr_time);

	return curr_time.tv_sec + ((double) curr_time.tv_nsec) * 1e-9;
}

} /* namespace util */

