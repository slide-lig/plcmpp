/*
 * Config.cpp
 *
 *  Created on: 20 févr. 2014
 *      Author: etienne
 */

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

