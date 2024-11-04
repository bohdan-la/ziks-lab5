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

  if (filename) {
    FILE *file = fopen(filename, "r");
    char buff[BUFFLEN];
    if (file == NULL) {
      fprintf(stderr, "Error opening file\n");
      exit(1);
    }
    while (fgets(buff, sizeof(buff), file)) {
      printf("%s", buff);
    }
  }
  else if (optind < argc) {
    char *string_arg = argv[optind];
    printf("%s\n", string_arg);
  }
  else {
    char buff[BUFFLEN];
    while (fgets(buff, sizeof(buff), stdin)) {
      printf("%s", buff);
    }
  } else {
    printf("Error: no input text was provided\n");
  }

  return 0;
}
