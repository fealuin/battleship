
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>   
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> 
#define TAMANO_TABLERO 10
#define TAMANO_MSG 20000

typedef struct          //estructura de matriz contiene nro de filas columnas, y puntero doble
{
  char barcos[TAMANO_TABLERO][TAMANO_TABLERO];
  char disparos[TAMANO_TABLERO][TAMANO_TABLERO];
} tablero;

int numeroJugadores,turno,jugadores[2];
tablero t[2];

void limpiaTablero(tablero t[2]){
    int i,j;
    for(i=0;i<TAMANO_TABLERO;i++){
        for (j=0;j<TAMANO_TABLERO;j++)
        {
            t[0].barcos[i][j]=' ';
            t[0].disparos[i][j]=' ';
            t[1].barcos[i][j]=' ';
            t[1].disparos[i][j]=' ';
        }
    }
}

void muestra_matriz(char p_matriz[TAMANO_TABLERO][TAMANO_TABLERO])
{
   int i, j;
   
   for(i=0;i<TAMANO_TABLERO;i++)
   {
      printf("---------------------------------------------------------------------------------\n");
      for(j=0;j<TAMANO_TABLERO;j++)
      {
         printf("|   %c   ", p_matriz[i][j]);//\t
      }
      printf("|\n");
   }
   printf("---------------------------------------------------------------------------------\n");
}

void escribe_matriz(char p_matriz[TAMANO_TABLERO][TAMANO_TABLERO],int sock)
{
   int i, j;
   char message[20000];
   char aux[300];
   sprintf(message,"");
   for(i=0;i<TAMANO_TABLERO;i++)
   {
      strcat(message,"---------------------------------------------------------------------------------\n");
      //write(sock,message,strlen(message));
      for(j=0;j<TAMANO_TABLERO;j++)
      {
        sprintf(aux,"|   %c   ", p_matriz[i][j]);
         strcat(message,aux);//\t
         //write(sock,message,strlen(message));
      }
      strcat(message,"|\n");
      //write(sock,message,strlen(message));
   }
   strcat(message,"---------------------------------------------------------------------------------\n");
   write(sock,message,10000);

}





int transformaLetra(char letra){
    char * letras="ABCDEFGHIJ";
    return (int)(strchr(letras,letra)-letras);
}

int nroJugador(int sock){
    if (jugadores[0]==sock)
    {
        return 0;
    }
    else
        return 1;
}

int enemigo(int jugador){
    if(jugador==0)
        return 1;
    else   
        return 0;
}



void dispara(char * coordenada,int sock){
    char message[20000];
    int fila=coordenada[2]-'0';
    int columna=transformaLetra(coordenada[0]);
    char vh=coordenada[3];
    int jug=nroJugador(sock);
    if (t[enemigo(nroJugador(sock))].barcos[fila][columna]=='B')
    {
        t[nroJugador(sock)].disparos[fila][columna]='X';
        sprintf(message,"TOCADO!!\n");
        write(sock,message,10000);    
    }else{
        t[nroJugador(sock)].disparos[fila][columna]='0';
        sprintf(message,"AGUA!!\n");
        write(sock,message,10000); 
    }
}

void escribeBarco(char * coordenada,int sock,int largo){
int i,j;
int fila=coordenada[2]-'0';
int columna=transformaLetra(coordenada[0]);
char vh=coordenada[3];
int jug=nroJugador(sock);

    for(i=0;i<largo;i++){
        if(vh=='V'){
            t[jug].barcos[(fila+i)][columna]='B';
        }else{
            t[jug].barcos[fila][(columna+i)]='B';
        }
    }
}



// thread que maneja la coneccion de los clientes
void *connection_handler(void *socket_desc)
{

    int sock = *(int*)socket_desc;
    int read_size;
    char message[20000] , client_message[20000];
     
    //Verifica si ambos jugadores estan conectados, si no, busy waiting
    if(jugadores[0]<0 || jugadores[1]<0){
        sprintf(message, "Esperando al otro jugador\n");
        write(sock , message , 10000);
        while(jugadores[0]<0 || jugadores[1]<0);
    }
        sprintf(message,"Por favor ingrese la posicion portaviones de la forma coordenada V Vertical H Horizontal, por ejemplo A-0V\n");
        write(sock , message , 10000);
        
        sprintf(message, "$");
        write(sock , message , 10000);

        read_size=recv(sock , client_message , 10000 , 0);
        escribeBarco(client_message,sock,4);

        escribe_matriz(t[nroJugador(sock)].barcos,sock);

                sprintf(message,"Por favor ingrese la posicion barco de la forma coordenada V Vertical H Horizontal, por ejemplo A-0V\n");
        write(sock , message , 10000);
        
        sprintf(message, "$");
        write(sock , message , 10000);

        read_size=recv(sock , client_message , 10000 , 0);
        escribeBarco(client_message,sock,3);

        escribe_matriz(t[nroJugador(sock)].barcos,sock);

while(1){
        sprintf(message, "Preparados para disparar! por favor anotar coordenada por ejemplo A-0\n");
        write(sock , message , 10000);
        sprintf(message, "$");
        write(sock , message , 10000);
        read_size=recv(sock , client_message , 10000 , 0);
        dispara(client_message,sock);
        escribe_matriz(t[nroJugador(sock)].disparos,sock);
        //escribe_matriz(t[nroJugador(sock)].barcos,sock);
}
           
    if(read_size == 0)
    {
        puts("Client disconnected");
        numeroJugadores--;
        if(sock==jugadores[0])
            jugadores[0]=-1;
        else
            jugadores[1]=-1;
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    free(socket_desc);
     
    return 0;
}
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    numeroJugadores=0;
    jugadores[0]=-1;
    jugadores[1]=-1;
     limpiaTablero(t);
    
    //Creando socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("No se ha podido crear el socket");
    }
    puts("Socket creado");
     

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind ha fallado");
        return 1;
    }
    puts("bind ok");
     
    //Listen
    listen(socket_desc , 3);
     
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        if(numeroJugadores<2){
        puts("Coneccion aceptada");
         printf("numero de jugadores %d",numeroJugadores);

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(1);
        *new_sock = client_sock;
        jugadores[numeroJugadores]=client_sock;

        printf("Socket jugadores activos %d y %d\n",jugadores[0],jugadores[1]);
         
        printf("clien socket %d", client_sock);
        numeroJugadores++;
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         }
         else{
        printf("Ya hay dos jugadores\n");
}
    }
     
     
    if (client_sock < 0)
    {
        perror("Error");
        return 1;
    }
     
    return 0;
}
 
