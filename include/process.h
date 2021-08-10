#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization() const;            
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const; 
  Process(int id);

 private:
    int pid_;
    float utilization_;
    long int uptime_;
    std::string ram_;
    std::string user_;
};

#endif