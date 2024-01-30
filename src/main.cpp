#include <encrypt.h>
#include <thread>
#include <vector>
#include <iostream>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr << "Wrong args count" << std::endl;
        return -1;
    }

    int threadsCount = std::stoi(argv[2]);

    PfCrypt pfCrypt;
    pf_key_t pfKey;
    pfCrypt.getKey(&pfKey);

    std::vector<std::thread> threads(threadsCount);
    const std::string srcFile = argv[1];
    for (int i = 0; i < threadsCount; i++) {
        std::string destFile = srcFile + ".encrypted." + std::to_string(i);
        threads.push_back(std::thread(&PfCrypt::encryptFileEx, &pfCrypt, std::cref(srcFile), destFile, &pfKey));
    }

    for (std::thread & th : threads) {
        if (th.joinable())
            th.join();
    }

    return 0;
}