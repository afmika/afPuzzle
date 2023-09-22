#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <dirent.h>

std::vector<std::string> doSplit(std::string s, std::string delimiter);
std::string doubleToStr(double n);
std::vector<std::string> readDirectoryContent(std::string path);
