#include "PicManager.h"

bool isValid(std::string val)
{
    std::vector<std::string> chunks = doSplit(val, ".");
    if (chunks.size() < 2)
    {
        std::cout << " INVALID FILE TYPE ! " << val << '\n';
        return false;
    }

    std::string format(chunks[chunks.size() - 1]);
    if (format.compare("af") != 0 && format.compare("AF") != 0)
    {
        std::cout << " ERROR. FILE EXTENSION \"" << format << "\" NOT RECOGNIZED " << val << '\n';
        return false;
    }

    return true;
}

std::vector<std::string> loadPictures()
{
    std::string path = "./ressources/art";
    std::vector<std::string> content = readDirectoryContent(path);
    std::vector<std::string> valid;

    for (const auto &value : content)
    {
        if (isValid(value))
        {
            std::string tmp = path;
            tmp.append("/");
            tmp.append(value);
            valid.push_back(tmp);
        }
    }
    return valid;
}