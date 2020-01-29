#include "afTools.h"

using namespace std;
vector<string> doSplit(string s, string delimiter) {
    vector<string> result;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        //cout << token << endl;
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    //cout << s << endl;
    result.push_back(s);
    return result;
}

double strToDouble(string str) {
    istringstream iss(str);
    double f = 0;
    iss >> f;
    return f;
}
int strToInt(string str) {
    istringstream iss(str);
    int f = 0;
    iss >> f;
    return f;
}
int strToUnsignedInt(string str) {
    istringstream iss(str);
    unsigned int f = 0;
    iss >> f;
    return f;
}
float strToFloat(string str) {
    istringstream iss(str);
    float f = 0;
    iss >> f;
    return f;
}
string doubleToStr(double n) {
    stringstream stream;
	stream << "" << n;
	return stream.str();
}
template<typename Numeric> string numberToStr(Numeric n) {
    stringstream stream;
	stream << "" << n;
	return (string) stream.str();
}
vector<string> readDirectoryContent(const char* path){
    vector<string> file_list;
    DIR *dir;
    struct dirent *entry;
    if ( (dir = opendir (path) ) != NULL) {
      while ((entry = readdir (dir)) != NULL) {
        string temp = entry->d_name;
        // enleve .. et . de la liste
        if(temp.compare("..") != 0 && temp.compare(".") != 0 ) {
            file_list.push_back( entry->d_name );
        }
      }
      closedir(dir);
    } else {
    // error on opening the directory
      perror ("");
    }
    return file_list;
}
void sort_str_vector(vector<string>& vec) {
    for(unsigned int i = 0; i < vec.size(); i++) {
        if(i + 1 < vec.size()) {
            if(vec[i].compare(vec[i+1]) > 0) {
                string tmp = vec[i];
                vec[i] = vec[i+1];
                vec[i+1] = tmp;
                sort_str_vector(vec);
            }
        }
    }
}
