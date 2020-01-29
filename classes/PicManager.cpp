#include "PicManager.h"

bool is_af(string val) {
    vector<string> tmp = doSplit(val, ".");
    bool invalidSize = tmp.size() < 2;

    if(invalidSize) {
        cout << " INVALID FILE TYPE ! " << val << endl;
        return false;
    }
    string format( tmp[tmp.size() - 1] );
    bool invalidFormat = format.compare("af") != 0  && format.compare("AF") != 0;
    if(invalidFormat) {
        cout << " ERROR. FILE EXTENSION MUST BE 'AF', FOUND " << val << endl;
        return false;
    }
    return true;
}

vector<string> loadPictures() {
    const char* path = "./puzzle_afmika_data/art";
    vector<string> tmp = readDirectoryContent(path);
    vector<string> valid;
    sort_str_vector(tmp);
    for(unsigned int i=0; i < tmp.size(); i++) {
        string value(tmp[i]);
        // cout << "File at index " << i << " : "<< tmp[i] << endl;
        if(is_af(value)) {
            tmp[i] = path;
            tmp[i].append("/");
            tmp[i].append(value);
            valid.push_back(tmp[i]);
        }
    }
    return valid;
}
