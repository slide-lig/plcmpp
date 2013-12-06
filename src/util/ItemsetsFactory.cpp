
#include <algorithm>

#include <util/ItemsetsFactory.hpp>

void util::ItemsetsFactory::extend(vector<int32_t>& pattern, int32_t extension,
		vector<int32_t>& closure, vector<int32_t>& result) {
	result.reserve(pattern.size() + 1 + closure.size());
	copy(pattern.begin(), pattern.end(), result.begin());
	result.push_back(extension);
	copy(closure.begin(), closure.end(), result.end());
}

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

	copy(pattern.begin(), pattern.end(), result.end());
}

void util::ItemsetsFactory::extend(vector<int32_t>& pattern, int32_t extension,
		vector<int32_t>& closure, vector<int32_t>& ignoreItems,
		vector<int32_t>& result) {

	result.reserve(pattern.size() + 1 + closure.size() + ignoreItems.size());

	copy(pattern.begin(), pattern.end(), result.begin());
	result.push_back(extension);
	copy(closure.begin(), closure.end(), result.end());
	copy(ignoreItems.begin(), ignoreItems.end(), result.end());
	// TODO: this is strange (was strange in jLCM)
	// but this method seems not used.
	copy(closure.begin(), closure.end(), result.end());
}

void util::ItemsetsFactory::extend(vector<int32_t>& closure, int32_t extension,
		vector<int32_t>& result) {
	result.reserve(closure.size() + 1);
	copy(closure.begin(), closure.end(), result.begin());
	result.push_back(extension);
}
