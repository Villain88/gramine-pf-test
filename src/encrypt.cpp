#include <iostream>
#include <encrypt.h>


int PfCrypt::encryptFile(const std::string &src, const std::string &dst, const pf_key_t* pf_key)
{
    return pf_encrypt_file(src.c_str(), dst.c_str(), pf_key);
}

int PfCrypt::getKey(pf_key_t* pf_key)
{
    if (!pf_key) {
        return -1;
    }

    for (uint8_t i = 0; i < sizeof(*pf_key); i++) {
        (*pf_key)[i] = i;
    }

    return 0;
}