#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>  

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using namespace std;

Processor& System::Cpu() { 
	return cpu_; 
}

vector<Process>& System::Processes() { 
	processes_.clear();
	vector<int> pid_list = LinuxParser::Pids();
	for(size_t i=0; i<pid_list.size(); i++){
		processes_.push_back(pid_list[i]);
	}
	std::sort(processes_.begin(), processes_.end());
	std::reverse(processes_.begin(), processes_.end());
	return processes_; 
}

string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }