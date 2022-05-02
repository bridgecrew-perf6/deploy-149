#include <stddef.h>

struct memory {
   char *response;
   size_t size;
 };


 static size_t cb(void *data, size_t size, size_t nmemb, void *userp);
 char *createURL(char* login, char* password, char* site, char* node, long int startTime, long int endTime);
 int nbDigit (long int n);