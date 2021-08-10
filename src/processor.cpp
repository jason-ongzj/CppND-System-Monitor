#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() { 
	long activeJiffies = LinuxParser::ActiveJiffies();
	long totalJiffies = LinuxParser::Jiffies();
	return (float) activeJiffies/totalJiffies;
}