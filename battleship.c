/*
    C ECHO client example using sockets
*/
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <stdlib.h> 
#include <fcntl.h> // for open
#include <unistd.h> // for close


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //CREANDO SOCKET
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("No se ha podido crear el socket");
    }
    puts("Socket creado");
    
    if (argc<2)
    {
        printf("Modo de uso: battleship <direccion ip servidor>\n");
        exit(1);
    }

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connectarse al socket remoto
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Error la coneccion ha fallado");
        return 1;
    }
     
    puts("Conectado\n");
     
    //Mantener coneccion con el servidor
    while(1)
    {
        printf("Enter message : ");
        scanf("%s" , message);
         
        //Enviar mensaje
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Envio fallido");
            return 1;
        }
         
        //Recibir mensajes desde el servidor
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("Recepcion fallida");
            break;
        }
         
        //puts("Server reply :");
        puts(server_reply);
    }
     
    close(sock);
    return 0;
}