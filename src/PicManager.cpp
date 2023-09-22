#include "PicManager.h"

bool isValid(string val)
{
    vector<string> tmp = doSplit(val, ".");
    bool invalidSize = tmp.size() < 2;

    if (invalidSize)
    {
        cout << " INVALID FILE TYPE ! " << val << endl;
        return false;
    }

    string format(tmp[tmp.size() - 1]);
    bool invalidFormat = format.compare("af") != 0 && format.compare("AF") != 0;
    if (invalidFormat)
    {
        cout << " ERROR. FILE EXTENSION \"" << format << "\" NOT RECOGNIZED " << val << endl;
        return false;
    }

    return true;
}

vector<string> loadPictures()
{
    std::string path = "./ressources/art";
    vector<string> tmp = readDirectoryContent(path);
    vector<string> valid;

    for (int i = 0; i < tmp.size(); i++)
    {
        string value(tmp[i]);
        // cout << "File at index " << i << " : "<< tmp[i] << endl;
        if (isValid(value))
        {
            tmp[i] = path;
            tmp[i].append("/");
            tmp[i].append(value);
            valid.push_back(tmp[i]);
        }
    }
    return valid;
}