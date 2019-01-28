#include "mycompress.h"

#include <malloc.h>

#include "lzo/lzoconf.h"
#include "lzo/lzo1x.h"

using namespace std;

MyCompress::MyCompress(/* args */)
{
}

MyCompress::~MyCompress()
{
}

//压缩
int MyCompress::Compress(const unsigned char *in, uint64_t inlen, unsigned char *out, uint64_t *outlen)
{
    int r = -1;
    void *wrkmem = (lzo_voidp)malloc(LZO1X_1_MEM_COMPRESS);
    if (wrkmem == NULL)
    {
        return r;
    }
    lzo_uint ol=*outlen;
    r = lzo1x_1_compress(in, (lzo_uint)inlen, out, &ol, wrkmem);
    *outlen = ol;
    free(wrkmem);
    return r;
}

//解压
int MyCompress::DeCompress(const unsigned char *in, uint64_t inlen, unsigned char *out, uint64_t *outlen)
{
    int r = 0;
    lzo_uint ol = *outlen;
    r = lzo1x_decompress(in, (lzo_uint)inlen, out, &ol, NULL);
    *outlen = ol;
    return r;
}

int MyCompress::Init()
{
    int r = -1;
    r = lzo_init();
    return r;
}

int MyCompress::CleanUp()
{
    return 0;
}