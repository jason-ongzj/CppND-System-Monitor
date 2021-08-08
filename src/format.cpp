#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
	int hours = seconds/3600;
	int mins = (seconds - hours*3600)/60;
	int secs = (seconds - hours * 3600)%60;

	string elapsed_time = "";
	if (hours < 10) {
		elapsed_time += "0" + std::to_string(hours) + ":";
	} else {
		elapsed_time += std::to_string(hours) + ":";
	}
	if (mins < 10) {
		elapsed_time += "0" + std::to_string(mins) + ":";
	} else {
		elapsed_time += std::to_string(mins) + ":";
	}
	if (secs < 10) {
		elapsed_time += "0" + std::to_string(secs);
	} else {
		elapsed_time += std::to_string(secs);
	}
	
	return elapsed_time;
}