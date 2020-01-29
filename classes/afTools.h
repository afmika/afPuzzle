#ifndef AFTOOLS_H
#define AFTOOLS_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <dirent.h>

using namespace std;

vector<string> doSplit(string s, string delimiter);
double strToDouble(string str);
int strToInt(string str);
int strToUnsignedInt(string str);
float strToFloat(string str);
string doubleToStr(double n);

vector<string> readDirectoryContent(const char* path);
void sort_str_vector(vector<string>& vec);

template<typename Numeric> string numberToStr(Numeric n);

#endif // AFTOOLS_H
