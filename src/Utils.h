#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <dirent.h>

using namespace std;

vector<string> doSplit(string s, string delimiter);
string doubleToStr(double n);
vector<string> readDirectoryContent(std::string path);
