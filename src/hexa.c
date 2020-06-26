
#include "includes/plague.h"

void print_hex(const unsigned char *str, unsigned int size)
{
    unsigned int i = 0;

    while (i < size)
    {
        printf("%02x", *str);
        str++;
        i++;
    }
    printf("\n");
}

char *encode_hex(const unsigned char *key, int size)
{
    char *hex;
    int i = 0;

    if (!(hex = malloc((size * 2 + 1) * sizeof(char))))
        handleError();
    while (i < size)
    {
        sprintf((char *)(hex + (i * 2)),"%02x", key[i]);
        i++;
    }
    hex[i * 2] = '\0';
    return hex;
}

char *bin2hex(const unsigned char *bin, size_t len)
{
    char   *out;
    size_t  i;

    if (bin == NULL || len == 0)
        return NULL;

    out = malloc(len*2+1);
    for (i=0; i<len; i++) {
        out[i*2]   = "0123456789abcdef"[bin[i] >> 4];
        out[i*2+1] = "0123456789abcdef"[bin[i] & 0x0F];
    }
    out[len*2] = '\0';

    return out;
}


unsigned char    *decode_hex(char *hex, int size)
{
    unsigned char *decoded_key;
    char *tmp;
    int i = 0;

    if (!(decoded_key = malloc(((KEY_SIZE) + 1) * sizeof(char))))
        handleError();
    if (!(tmp = malloc((3) * sizeof(char))))
        handleError();
    while (i < size)
    {
        tmp = strncpy(tmp, hex + (i * 2), 2);
        tmp[2] = 0;
        decoded_key[i] = (char)strtol(tmp, NULL, 16);
        i++;
    }
    decoded_key[i] = 0;
    if (tmp)
        free(tmp);
    return decoded_key;
}
