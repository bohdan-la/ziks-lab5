#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#define BUFFLEN 2048

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(const unsigned char *plaintext, int plaintext_len, const unsigned char *key,
                const unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    int len;
    int ciphertext_len;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key,
                const unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    int len;
    int plaintext_len;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
        handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

int main(int argc, char *argv[])
{
  int opt;
  char *key = NULL;
  char *vector = NULL;
  char *filename = NULL;
  bool decrypt = 0;
  
  while ((opt = getopt(argc, argv, "k:v:f:d")) != -1) {
    switch (opt) {
      case 'k':
        key = optarg;
        break;
      case 'v':
        vector = optarg;
        break;
      case 'f':
        filename = optarg;
        break;
      case 'd':
        decrypt = 0;
        break;
      case '?':
        fprintf(stderr, "Unknown option: %c\n", optopt);
        exit(1);
    }
  }
  
  if (key == NULL || vector == NULL) {
    fprintf(stderr, "Usage: $s -k key -v vector [-f file] [string]\n", argv[0]);
    exit(1);
  }

  char buff[BUFFLEN+1];
  int text_len;
  
  if (filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
      fprintf(stderr, "Error opening file\n");
      exit(1);
    }
    text_len = fread(buff, 1, BUFFLEN, file);
    fclose(file);
    buff[text_len] = '\0';
  }
  else if (optind < argc) {
    strncpy(buff, argv[optind], BUFFLEN - 1);
    text_len = strlen(buff);
  }
  else {
    text_len = fread(buff, 1, BUFFLEN, stdin);
    buff[text_len] = '\0';
  }
  printf("%s", buff);

  return 0;
}
