#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h> 
#include "main_API.h"
#include <curl/curl.h>
#include<json-c/json.h>
 
#ifdef WIN32
#include <io.h>
#define READ_3RD_ARG unsigned int
#else
#include <unistd.h>
#define READ_3RD_ARG size_t
#endif
 
#if LIBCURL_VERSION_NUM < 0x070c03
#error "upgrade your libcurl to no less than 7.12.3"
#endif

#define LOGIN "LOGIN="
#define PASSWORD "PASSWORD="

/* This function is used to store in a structure the data returned by the API
*  @return: size_t size total of the data get.
*/
static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
 {
     //compute the new size
    size_t realsize = size * nmemb;
    //get a ptr on the old struct
    struct memory *mem = (struct memory *)userp;
 
    //get some memory in more
    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if(ptr == NULL){
        return 0;  /* out of memory! */
    }
    //start of the block of memory
    mem->response = ptr;
    //copy the old response to the new
    memcpy(&(mem->response[mem->size]), data, realsize);
    //add the size to the old
    mem->size += realsize;
    mem->response[mem->size] = 0;
 
   return realsize;
 }
/* This function return the number of digit of a number, for example, for 5 will return 1, for 58 will return 2, for 153 will return 3,...
* @return int number of digit in the int
*/
 int nbDigit (long int n) {
    if (n < 0){
        return nbDigit (-n); // -589 has the same number of digit as 589
    }
    if (n < 10){
        return 1;
    }
    return 1 + nbDigit (n / 10);
}

/* This function is used to create the url to interrogate the API, using different parameters.
*  @return: char* ptr to the full url to use
* TODO: Add authentication
*/
//ex: https://api.grid5000.fr/stable/sites/lyon/metrics?metrics=wattmetre_power_watt&nodes=taurus-1&start_time=1651129202&end_time=1651129202
char *createURL(char* login, char* password, char* site, char* node, long int startTime, long int endTime){
    //https://api.grid5000.fr/stable/sites//metrics?metrics=wattmetre_power_watt&nodes=&start_time=&end_time=  ==> static part, size = 103
    int sizeReq = strlen(login) + strlen(password) + strlen(site) + strlen(node) + nbDigit(startTime) + nbDigit(endTime) + 103;
    char *urlReturn = calloc(1, (sizeof (*urlReturn) * sizeReq)+1);
    strcat(urlReturn, "https://api.grid5000.fr/stable/sites/");
    strcat(urlReturn, site);
    strcat(urlReturn,"/metrics?metrics=wattmetre_power_watt&nodes=");
    strcat(urlReturn, node);
    strcat(urlReturn, "&start_time=");
    char *tTmp = malloc (sizeof (*tTmp) * 32);
    sprintf(tTmp, "%ld", startTime);  
    strcat(urlReturn, tTmp);
    strcat(urlReturn, "&end_time=");
    sprintf(tTmp, "%ld", endTime);  
    strcat(urlReturn, tTmp);
    free(tTmp);
    printf("URL:  %s   ", urlReturn);
    return urlReturn;

}

int main(int argc, char** argv){
    pid_t pid = fork();
    if (pid == -1) { //error
        perror("fork");
        exit(EXIT_FAILURE);
    }
    long int tStampStart = time(NULL);
    if (pid == 0) { //child 
        printf("printed from child process - %d\n", getpid());
        //waste tme
        int k = 0;
        for(int i = 0 ; i < 1000000; i++){
            printf("k= %d\n", (k += 3) % 50);
        }
        exit(EXIT_SUCCESS);
    } else { //father, wait for child end and make API ASK
        printf("printed from parent process - %d\n", getpid());
        wait(NULL);
        //GET the timestamp
        long int tStampStop = time(NULL);
        printf("\nLes deux timestamp sont: \n %ld  - start \n %ld  - stop \n ", tStampStart, tStampStop);

        //ask API
        printf("Time to ask the API:\n");
        //retrieve login and password
        char *login = malloc (sizeof (*login) * 256);
        char *password = malloc (sizeof (*password) * 256);
        char *buffer = malloc (sizeof (*buffer) * 262); // LOGIN= size = 6 + 256 = 262
        //to init string for Valgrind
        for(int i = 0; i < 256; i++){
            login[i] = '\0';
        }
        for(int i = 0; i < 256; i++){
            password[i] = '\0';
        }
        for(int i = 0; i < 256; i++){
            buffer[i] = '\0';
        }
        FILE* fichierCred = NULL;
        fichierCred = fopen("credential", "r");

        while(fgets(buffer, 262, fichierCred) != NULL){
            //comparaison:
            int i;
            for(i = 0; buffer[i] == LOGIN[i] && LOGIN[i] == '\0'; i++);
            if(!(buffer[i] > LOGIN[i] || buffer[i] < LOGIN[i])){
                
                //printf("Login reconnue");
                int lenL = strlen(LOGIN);
                while(buffer[i+lenL] != '\n' && buffer[i+lenL] != '\0'){
                    //printf("Login: Copie de %d, case %d : vers %c case %d\n", buffer[i+lenL], i+lenL, login[i], i);
                    login[i] = buffer[i+lenL];
                    i++;
                }
                login[i] = '\0';
            }
            for(i = 0; buffer[i] == PASSWORD[i] && PASSWORD[i] == '\0'; i++);

            if(!(buffer[i] > PASSWORD[i] || buffer[i] < PASSWORD[i])){
                //printf("Password reconnue");
                int lenP = strlen(PASSWORD);
                while(buffer[i+lenP] != '\n' && buffer[i+lenP] != '\0'){
                    //printf("Password: Copie de %d vers %c\n", buffer[i+lenP], password[i]);
                    password[i] = buffer[i+lenP];
                    i++;
                }
                password[i] = '\0';
            }
        }
        free(buffer);
        fclose(fichierCred);
        //we can now use login and password to access the API


        //to ask the API
        CURL *curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl) {
             //init the struct to 0
            struct memory chunk = {0};
            
            char* site = "lyon";
            char* node = "taurus-1";

            char* urlToSend = createURL(login, password, site, node, tStampStart, tStampStop);
            //curl_easy_setopt(curl, CURLOPT_URL, "https://back.overstats.fr/");
            //curl_easy_setopt(curl, CURLOPT_URL, "https://socket.rakura.fr/getRooms?connect=rakura");
            curl_easy_setopt(curl, CURLOPT_URL, urlToSend);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); //follow redirections
            free(urlToSend);
            //to create a string like login:password for authentication
            char *cred = malloc(strlen(login) + 1 + strlen(password) + 1); // +2 for the ":" and the '\0' | strlen doesn't count \0 at the end
            for(int i = 0; i < sizeof(cred); i++){
                cred[i] = '\0';
            }
            strcat(cred, login);
            strcat(cred, ":");
            strcat(cred, password);
            curl_easy_setopt(curl, CURLOPT_USERPWD, cred);
            free(cred);
            

            // send all data to this function
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
            // we pass our 'chunk' struct to the callback function
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);


            /* Perform the request, res will get the return code */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if(res != CURLE_OK){
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            /* always cleanup */
            curl_easy_cleanup(curl);

            printf("\nPrint the response\n");
            sleep(10);
            //printf("----------%s", chunk.response);
            printf("End of response writing \n");
            struct json_object *parsed_json;
            parsed_json = json_tokener_parse(chunk.response);
            size_t len_json = json_object_array_length(parsed_json);
            printf("nombre de données recuperés: %ld \n", len_json);
            
            struct json_object *data;
    
            enum json_type data2;
            struct json_object *value;
            double mean = 0;
            //printf("The json string:\n\n%s\n\n", json_object_to_json_string(parsed_json));
   
            printf("The json object to string:\n\n%s\n", json_object_to_json_string_ext(parsed_json, JSON_C_TO_STRING_PRETTY));

            //data = json_object_get_int(json_object_array_get_idx(parsed_json, 2));
            for (int i=0; i<len_json; i++)
            {
                data = json_object_get_string(json_object_array_get_idx(parsed_json, i));
                //data2 = json_object_get_int(json_object_array_get_idx(parsed_json, 2));
                printf("The value at %i position is: %s\n", i, data);
                json_object *name = json_object_object_get_ex(data, "value", value);
                name = json_object_object_get(data, "name");
                printf("La value vaut: %s \n", json_object_get_string(name));
                printf("La value V2 vaut: %s \n", json_object_get_string(value));
                //value = json_object_object_get(data, "value");
                
            }
            json_object_put(parsed_json);
            
            /*
            for(int i = 0; i < len_json; i++){
                json_object_object_get_ex(parsed_json, i, &data);
                //printf("Data = %s", data);
                value = json_object_array_get_idx(data, i);
                printf("Age: %d\n", json_object_get_int(value));
                //mean += json_object_get_int(value);
            }
            printf("\n End for \n");
            //printf("Users: %s\n", json_object_get_string(value));
            */
            
        }
        free(login);
        free(password);
    }
    
   
    return 0;
}
