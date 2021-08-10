#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id) : pid_(id) {
	long activeJiffies = LinuxParser::ActiveJiffies(pid_);
	long startTime = LinuxParser::UpTime(pid_);
	long sys_uptime = LinuxParser::UpTime();
	long seconds = sys_uptime - (long) startTime/sysconf(_SC_CLK_TCK);
	utilization_ = (float) (activeJiffies/sysconf(_SC_CLK_TCK))/seconds;

	ram_ = LinuxParser::Ram(pid_);
	user_ = LinuxParser::User(pid_);
	uptime_ = LinuxParser::UpTime(pid_);
}

int Process::Pid() { 
	return pid_; 
}

float Process::CpuUtilization() const { 
	return utilization_;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const { 
	return this->CpuUtilization() < a.CpuUtilization() ;
}