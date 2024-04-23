#include "json.cpp"

int main(){
    try{

        json j;
        json j2;
        json j3;
        std::ifstream f("input.txt", std::ios::in);
        std::ofstream o("output.txt", std::ios::out);
        f >> j;
        f >> j2;
        std::cout << j << std::endl << "-----------------------------------------" << std::endl;
        std::cout << j2 << std::endl << "-----------------------------------------" << std::endl;
        std::cout << j3 << std::endl;



    }
    catch(const json_exception& error){
        std::cout << std::endl << "-----------------------------------------"
                  << std::endl << error.msg << std::endl << "-----------------------------------------" << std::endl;
    }

    return 0;
}