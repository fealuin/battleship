#include <stdio.h> 
#include <string.h>
#include <sys/socket.h>  
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> 


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[20000] , server_reply[20000];
     
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
     
    puts("Bienvenido al juego battleship\n");
     
    //Mantener coneccion con el servidor
    while(1)
    {

        //Recibir mensajes desde el servidor
        while( recv(sock , server_reply , 10000 , 0) > 0)
        {
            if(strchr(server_reply,'$')>0){ //Si servidor requiere respuesta
                //puts("servidor requiere respuesta:");
                
                scanf("%s" , message);
                if( send(sock , message , 10000 , 0) < 0)
                {
                    puts("Envio fallido");
                    return 1;
                }
            } else{
            puts(server_reply);
            }   
            //puts("Recepcion fallida");
            //break;
        }
        //puts("Server reply :");
              sprintf(server_reply,"");


        //printf("Enter message : ");
        
         
        //Enviar mensaje

         


    }
     
    close(sock);
    return 0;
}