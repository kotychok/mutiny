// Slightly modified from:
// https://stackoverflow.com/a/671389

#include <fstream>
#include <iostream>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
//
// On failure, returns 0.0, 0.0

class MemoryHelper {
  public:
    static void printMemUsageIfChanged() {
      double vm;
      double rss;
      MemoryHelper::processMemUsage(vm, rss);
      static double prevVM { vm };
      static double prevRSS { rss };
      if (prevVM != vm || prevRSS != rss) {
        std::cout << "VM: " << vm << ", RSS: " << rss << std::endl;
      }
      prevVM = vm;
      prevRSS = rss;
    };

    static void processMemUsage(double& vm_usage, double& resident_set)
    {
      using std::ios_base;
      using std::ifstream;
      using std::string;

      vm_usage     = 0.0;
      resident_set = 0.0;

      // 'file' stat seems to give the most reliable results
      //
      ifstream stat_stream("/proc/self/stat",ios_base::in);

      // dummy vars for leading entries in stat that we don't care about
      //
      string pid, comm, state, ppid, pgrp, session, tty_nr;
      string tpgid, flags, minflt, cminflt, majflt, cmajflt;
      string utime, stime, cutime, cstime, priority, nice;
      string O, itrealvalue, starttime;

      // the two fields we want
      //
      unsigned long vsize;
      long rss;

      stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
        >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
        >> utime >> stime >> cutime >> cstime >> priority >> nice
        >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

      stat_stream.close();

      long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
      vm_usage     = vsize / 1024.0;
      resident_set = rss * page_size_kb;
    }
};
