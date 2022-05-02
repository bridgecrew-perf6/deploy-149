#include <stdio.h>
#include <stdlib.h>
#include "main.h"


static void init(void)
{
//for Windows user, we use a special function to init dll
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif


}

static void end(void)
{
//free the dll for Windows
#ifdef WIN32
    WSACleanup();
#endif
}

int main(int argc, char** argv){
//no arguments are expected
init();
//socket creation
SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
if(sock == INVALID_SOCKET)
{
    perror("socket()");
    exit(errno);
}
//socket connection
struct hostent *hostinfo = NULL;
SOCKADDR_IN sin = { 0 }; /* initialise la structure avec des 0 */
const char *hostname = "www.api.grid5000.fr";

hostinfo = gethostbyname(hostname); /* on récupère les informations de l'hôte auquel on veut se connecter */
if (hostinfo == NULL) /* l'hôte n'existe pas */
{
    fprintf (stderr, "Unknown host %s.\n", hostname);
    exit(EXIT_FAILURE);
}
printf("Socket created \n");
uint16_t PORT = 80;
sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr; /* l'adresse se trouve dans le champ h_addr de la structure hostinfo */
sin.sin_port = htons(PORT); /* on utilise htons pour le port */
sin.sin_family = AF_INET;

if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
{
    perror("connect()");
    exit(errno);
}
printf("Socket connected \n");


//close the socket:
closesocket(sock);
printf("Socket closed \n");

end();
}