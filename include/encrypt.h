#include <string>
#include <stdexcept>
#include <mutex>

extern "C" {
    #include <common/util.h>
    #include <common/pf_util.h>
}

class PfCrypt {
public:
    PfCrypt() {
        if (pf_init() != 0) {
            throw std::runtime_error("Failed to initialize protected files");
        }
    };

    int encryptFile(const std::string &src, const std::string &dst, const pf_key_t* pf_key);
    int encryptFileEx(const std::string &src, const std::string &dst, const pf_key_t* pf_key);
    int getKey(pf_key_t* pf_key);
private:
    std::mutex pfMutex;
};
