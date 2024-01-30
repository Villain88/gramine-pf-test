#include <iostream>
#include <encrypt.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>


#define PERM_rw_______	0600
#define PERM_rw_r_____	0640
#define PERM_rw_r__r__	0644
#define PERM_rw_rw_r__	0664
#define PERM_rw_rw_rw_	0666

int PfCrypt::encryptFile(const std::string &src, const std::string &dst, const pf_key_t* pf_key)
{
    //const std::lock_guard<std::mutex> lock(pfMutex);
    return pf_encrypt_file(src.c_str(), dst.c_str(), pf_key);
}

int PfCrypt::encryptFileEx(const std::string &src, const std::string &dst, const pf_key_t* pf_key) {
    int ret = -1;
    int input = -1;
    int output = -1;
    pf_context_t* pf = NULL;
    char* norm_output_path = NULL;

    void* chunk = malloc(PF_NODE_SIZE);
    uint64_t input_size = 0;
    uint64_t input_offset = 0;

    pf_handle_t handle;
    pf_status_t pfs;

    input = open(src.c_str(), O_RDONLY);
    if (input < 0) {
        printf("Failed to open input file '%s': %s\n", src.c_str(), strerror(errno));
        goto out;
    }

    output = open(dst.c_str(), O_RDWR | O_CREAT, PERM_rw_rw_r__);
    if (output < 0) {
        printf("Failed to create output file '%s': %s\n", dst.c_str(), strerror(errno));
        goto out;
    }
    handle = (pf_handle_t)&output;
    {
        //const std::lock_guard<std::mutex> lock(pfMutex);
        pfs = pf_open(handle, dst.c_str(), /*size=*/0, PF_FILE_MODE_WRITE,
                                /*create=*/true, pf_key, &pf);

    }
    if (PF_FAILURE(pfs)) {
        printf("Failed to open output PF: %s\n", pf_strerror(pfs));
        goto out;
    }

    /* Process file contents */
    input_size = get_file_size(input);
    if (input_size == (uint64_t)-1) {
        printf("Failed to get size of input file '%s': %s\n", src.c_str(), strerror(errno));
        goto out;
    }

    while (true) {
        ssize_t chunk_size = read(input, chunk, PF_NODE_SIZE);
        if (chunk_size == 0) // EOF
            break;

        if (chunk_size < 0) {
            if (errno == -EINTR)
                continue;

            printf("Failed to read file '%s': %s\n", src.c_str(), strerror(errno));
            goto out;
        }
        {
            //const std::lock_guard<std::mutex> lock(pfMutex);
            pfs = pf_write(pf, input_offset, chunk_size, chunk);
        }
        if (PF_FAILURE(pfs)) {
            printf("Failed to write to output PF: %s\n", pf_strerror(pfs));
            goto out;
        }

        input_offset += chunk_size;
    }

    ret = 0;

out:
    if (pf) {
        //const std::lock_guard<std::mutex> lock(pfMutex);
        if (PF_FAILURE(pf_close(pf))) {
            printf("failed to close PF\n");
            ret = -1;
        }
    }

    free(chunk);

    if (input >= 0)
        close(input);
    if (output >= 0)
        close(output);
    return ret;
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