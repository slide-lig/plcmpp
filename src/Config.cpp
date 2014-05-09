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


#include <sstream>
#include <cstdlib>
using namespace std;

#include "Config.hpp"

#define DEFAULT_OVER_ALLOC_THRESHOLD 		2.0
#define DEFAULT_MEMORY_USAGE_CHECK_DELAY 	500.0	// milliseconds
#define DEFAULT_AVOID_OVER_ALLOC 			false

template <typename T>
static T get_from_env_or_default(const char *env_name, T def)
{
	char *s = getenv(env_name);
	T result;

	if (s != nullptr)
	{
		istringstream iss(s);
		iss >> result;
		return result;
	}
	else
	{
		return def;
	}
}

#define GET(var_name) 								\
		get_from_env_or_default(#var_name, DEFAULT_ ## var_name)

bool Config::AVOID_OVER_ALLOC = GET(AVOID_OVER_ALLOC);
double Config::OVER_ALLOC_THRESHOLD = GET(OVER_ALLOC_THRESHOLD);
double Config::MEMORY_USAGE_CHECK_DELAY = GET(MEMORY_USAGE_CHECK_DELAY);

