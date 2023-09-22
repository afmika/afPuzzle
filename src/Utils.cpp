#include "Utils.h"

std::vector<std::string> doSplit(std::string s, std::string delimiter)
{
    std::vector<std::string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    result.push_back(s);
    return result;
}

std::string doubleToStr(double n)
{
    std::stringstream stream;
    stream << "" << n;
    return stream.str();
}

std::vector<std::string> readDirectoryContent(std::string path)
{
    std::vector<std::string> fileList;
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            std::string dirName = entry->d_name;
            if (dirName.compare("..") != 0 && dirName.compare(".") != 0)
            {
                fileList.push_back(dirName);
            }
        }
        closedir(dir);
    }
    else
    {
        perror("");
    }
    return fileList;
}
