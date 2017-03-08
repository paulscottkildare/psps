/*
 * psps.cpp
 *
 *  Created on: Aug 6, 2013
 *      Author: Paul Scott - paul.scott.kildare@live.com
 *   Copyright: (c) 2013 Paul W Scott
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

struct Lines {
  string name;
  string parameters;
  int pid;
};
Lines lines;
vector<Lines> processes;
string myName;

void getProcIdByName(string procName) {
  vector<string> bits;
  int pid = -1;
  int count = 0;
  DIR *dp = opendir("/proc");
  if (dp != NULL) {
    struct dirent *dirp;
    while ((dirp = readdir(dp))) {
      int id = atoi(dirp->d_name);
      if (id > 0) {
        string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
        ifstream cmdFile(cmdPath.c_str());
        string cmdLine;
        getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
          string cmd = cmdLine;
          if (cmd.find(procName) != string::npos) {
            pid = id;
            string tempString;
            int zeroCount = 0;
            lines.pid = id;
            for (size_t i = 0; i < cmdLine.size(); i++) {
              const char c = cmdLine[i];
              if (c != '\x0') {
                tempString.append(1, c);
              }
              else {
                if (tempString.substr(0, 2) == "./")
                  tempString = tempString.substr(2);
                switch (zeroCount) {
                  case 0:
                    lines.name = tempString;
                    break;
                  case 1:
                    lines.parameters = tempString;
                    break;
                }
                zeroCount++;
                tempString = "";
              }
            }
            if ((lines.name != myName) && (myName != "./"+lines.name)) {
              processes.push_back(lines);
            }
            lines.name = "";
            lines.parameters = "";
            lines.pid = 0;
            tempString = "";
            count++;
          }
        }
      }
    }
  }
  closedir(dp);
}

int main(int argc, char* argv[]) {
  cout << endl;
  cout << "Paul Scott's Process Summary" << endl;
  cout << "Copyright (c) 2013 Paul Scott" << endl << endl;
  if (argc < 2) {
    cout << "No process name specified." << endl;
    return -1;
  }
  myName = argv[0];
  getProcIdByName(string(argv[1]));
  for (unsigned int i = 0; i < processes.size(); i++) {
    cout << "Process " << processes[i].pid << endl;
    cout << "Command: " << processes[i].name << endl;
    cout << "Parameter: " << processes[i].parameters << endl;
    cout << endl;
  }
  cout << "Total processes found: " << processes.size() << endl << endl;
  return 0;
}
