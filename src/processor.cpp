#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
	long activeJiffies = LinuxParser::ActiveJiffies();
	long totalJiffies = LinuxParser::Jiffies();
	return (float) activeJiffies/totalJiffies;
}