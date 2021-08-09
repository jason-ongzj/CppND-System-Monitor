#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float mem_util = 0;
  float mem_total, mem_free;
  string mem_capacity;
  string memtotal_str, memfree_str, mem_str;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> mem_str >> mem_capacity;
      if (mem_str == "MemTotal:") {
        mem_total = stof(mem_capacity);
      } else if (mem_str == "MemFree:") {
        mem_free = stof(mem_capacity);
      }
    }
    mem_util = mem_total - mem_free;
    return mem_util/mem_total;
  }
  return 0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime, idle;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
    return stol(uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string cpuid, line;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpuid >> user >> nice >> system >> idle >> iowait; 
    linestream >> irq >> softirq >> steal >> guest >> guest_nice; 
    return stol(user) + stol(nice) + stol(system) + stol(idle) + stol(iowait) + stol(irq) 
      + stol(softirq) + stol(steal) + stol(guest) + stol(guest_nice);
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    long utime = 0;
    long stime = 0;
    string::size_type start = 0;
    string::size_type last = line.find_first_of(" ");
    int count = 1;
    while(last != string::npos) {
      switch (count){
        case 14: utime = stol(line.substr(start, last-start));
                 break;
        case 15: stime = stol(line.substr(start, last-start));
                 break;
      }
      start = ++last;
      count++;
      last = line.find_first_of(" ", last);
    }
    return utime + stime;
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_util = CpuUtilization();
  if (cpu_util.size() != 0){
    return stol(cpu_util[0]) + stol(cpu_util[1]) + stol(cpu_util[2]) + stol(cpu_util[5]) 
      + stol(cpu_util[6]) + stol(cpu_util[7]);
  }
  return 0; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpu_util = CpuUtilization();
  // 3 - Idle, 4 - Iowait
  if (cpu_util.size()!= 0){
    return stol(cpu_util[3]) + stol(cpu_util[4]);
  }
  return 0; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string cpuid, line;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpuid >> user >> nice >> system >> idle >> iowait; 
    linestream >> irq >> softirq >> steal >> guest >> guest_nice; 
    return {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
   if (filestream.is_open()){
    std::getline(filestream, line);
    return line;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string key, value, line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
   if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return std::to_string(stoi(value)/1000);
      }
    }
  }
  return "0"; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string key, value, line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string val = Uid(pid);
  string pwd_val, user, x, line;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> pwd_val;
      if (pwd_val == val) {
        if(user.size() > 6){
          user.erase(6);
        }
        return user;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string value, line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    string::size_type start = 0;
    string::size_type last = line.find_first_of(" ");
    int count = 1;
    while(last != string::npos) {
      if (count == 22) {
        value = line.substr(start, last-start);
        break;
      }
      start = ++last;
      count++;
      last = line.find_first_of(" ", last);
    }
    return stol(value)/sysconf(_SC_CLK_TCK);
  }
  return 0; 
}
