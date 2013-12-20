
#include <algorithm>
#include <iostream>

#include "util/Helpers.h"

#include <util/ItemsetsFactory.hpp>

void util::ItemsetsFactory::extendRename(vector<int32_t>& closure,
		int32_t extension, vector<int32_t>& pattern, vector<int32_t>& renaming,
		vector<int32_t>& result) {
	result.reserve(pattern.size() + 1 + closure.size());
	vector<int32_t>::iterator it;
	for(it = closure.begin(); it != closure.end(); it++)
	{
		result.push_back(renaming[*it]);
	}
	result.push_back(renaming[extension]);

	copy(pattern.begin(), pattern.end(), back_inserter(result));
}
