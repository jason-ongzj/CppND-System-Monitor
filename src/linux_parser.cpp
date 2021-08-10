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
      linestream >> key >> value;
      if (key == "PRETTY_NAME") {
        std::replace(value.begin(), value.end(), '_', ' ');
        break;
      }
    }
  }
  filestream.close();
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
  stream.close();
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

float LinuxParser::MemoryUtilization() { 
  string filename = kProcDirectory + kMeminfoFilename;
  float mem_total = LinuxParser::FindValueByKey<float>(filename, "MemTotal");
  float mem_free = LinuxParser::FindValueByKey<float>(filename, "MemFree");
  return (mem_total - mem_free)/mem_total;
}

long LinuxParser::UpTime() { 
  string uptime, idle;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  filestream.close();
  return stol(uptime);
}

long LinuxParser::Jiffies() { 
  string cpuid, line;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpuid >> user >> nice >> system >> idle >> iowait; 
    linestream >> irq >> softirq >> steal >> guest >> guest_nice; 
  }
  filestream.close();
  return stol(user) + stol(nice) + stol(system) + stol(idle) + stol(iowait) + stol(irq) 
      + stol(softirq) + stol(steal) + stol(guest) + stol(guest_nice);
}

long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long utime = 0;
  long stime = 0;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
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
  }
  filestream.close();
  return utime + stime;
}

long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_util = CpuUtilization();
  if (cpu_util.size() != 0){
    return stol(cpu_util[0]) + stol(cpu_util[1]) + stol(cpu_util[2]) + stol(cpu_util[5]) 
      + stol(cpu_util[6]) + stol(cpu_util[7]);
  }
  return 0; 
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpu_util = CpuUtilization();
  // 3 - Idle, 4 - Iowait
  if (cpu_util.size()!= 0){
    return stol(cpu_util[3]) + stol(cpu_util[4]);
  }
  return 0; 
}

vector<string> LinuxParser::CpuUtilization() { 
  string cpuid, line;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpuid >> user >> nice >> system >> idle >> iowait; 
    linestream >> irq >> softirq >> steal >> guest >> guest_nice;  
  }
  filestream.close();
  return {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};
}

int LinuxParser::TotalProcesses() { 
  string filename = kProcDirectory + kStatFilename;
  return LinuxParser::FindValueByKey<int>(filename, "processes");
}

int LinuxParser::RunningProcesses() { 
  string filename = kProcDirectory + kStatFilename;
  return LinuxParser::FindValueByKey<int>(filename, "procs_running");
}

string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
   if (filestream.is_open()){
    std::getline(filestream, line);
  }
  filestream.close();
  return line.substr(0, 40); 
}

string LinuxParser::Ram(int pid) { 
  string filename = kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  string val = LinuxParser::FindValueByKey<string>(filename, "VmRSS");
  return val.substr(0, val.size()-3);
}

string LinuxParser::Uid(int pid) { 
  string filename = kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  return LinuxParser::FindValueByKey<string>(filename, "Uid");
}

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
        break;
      }
    }
  }
  filestream.close();
  return user;
}

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
  }
  filestream.close();
  long upTimePid = UpTime() - stol(value)/sysconf(_SC_CLK_TCK);
  return upTimePid;
}

template<typename T>
T LinuxParser::FindValueByKey(std::string const &file, std::string const &keyword) {
  string key, line;
  T value;
  std::ifstream filestream(file);
  if (filestream.is_open()){
    while(std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == keyword) {
        break;
      }
    }
  }
  filestream.close();
  return value;
}
