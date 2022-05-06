#include <iostream>
#include "BuildSSM.h"

int main() {

    std::cout << "[BuildSSM] included successfully\n";

    std::string male_dir = "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/data/male";
    BuildSSM ssm(male_dir);
    std::size_t p = male_dir.find_last_of('/');
    std::cout << "p: " << p << "\n";
    
    ssm.createModelsFromFiles();
    std::cout << "[BuildSSM] created models from files\n";

    ssm.createDeformationFields();
    // std::string test_str = "/aa/bb/cc.json";
    // std::string delim = ".json";
    // std::size_t pos = test_str.find(delim);
    // std::cout << "pos: " << pos << "\n";
    // if (pos >= 0 && pos < test_str.size()) {
    //     std::string s = test_str.substr(pos);
    //     std::cout << "s: " << s << "\n";
    //     std::cout << "splitted: " << s << std::endl;
    // } else {
    //     std::cout << "nothing found\n";
    // }

    return 0;
}