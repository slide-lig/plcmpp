/*
 * Config.hpp
 *
 *  Created on: 19 févr. 2014
 *      Author: etienne
 */

#pragma once

class Config {
public:
	static bool AVOID_OVER_ALLOC;
	static double OVER_ALLOC_THRESHOLD;
	static double MEMORY_USAGE_CHECK_DELAY; 	// milliseconds

};
