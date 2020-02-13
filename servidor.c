#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <stdio.h>       
#include <string.h>    	 
#include <stdbool.h>	 
#include <pthread.h>     
#include <wiringPi.h>    
#include <stdlib.h>		 
#include <ctype.h>		 

#define TF_MENSAGEM    	   1     
#define PORTA 		   13260   
#define PINORELE_1 	   8	   

void *comunicacaoSocket(void*);
int close(int);
void setaPinos(void);			  
bool preparaSocket(int *);		   
void iniciaServidor(int);		   

int main(int argc , char *argv[])
{
  int socket_servidor;
  
  setaPinos();
  
  if(!preparaSocket(&socket_servidor)) 
	  return 0;
 
  iniciaServidor(socket_servidor); 
}

void setaPinos()
{
	wiringPiSetup();            
	pinMode(PINORELE_1, OUTPUT);         
	
	digitalWrite(PINORELE_1,HIGH);
	
}

bool preparaSocket(int *conexao)
{
  struct sockaddr_in servidor;
  
  *conexao = socket(AF_INET, SOCK_STREAM , 0); 
  
  if (*conexao == -1)
  {
    puts("Nao foi possivel criar o socket");
    return false;
  }
  puts("Socket criado");
  
  servidor.sin_family = AF_INET;
  servidor.sin_addr.s_addr = INADDR_ANY;
  servidor.sin_port = htons(PORTA);
  
  if(bind((*conexao),(struct sockaddr *)&(servidor) , sizeof(servidor)) < 0)
  {
    puts("Falha na ligacao");
    return false;
  }
  puts("Ligacao efetuada");
  return true;
}


void iniciaServidor(int socket_servidor)
{
  int socket_cliente, *novo_socket; 
  
  listen(socket_servidor, 3); 
  
  puts("Aguardando novas conexoes...");
  
  //loop infinito, para cada conexão criada uma thread que recebe as mensagens
  while((socket_cliente = accept(socket_servidor, (struct sockaddr *)0, (socklen_t*)0)))
  {
    puts("Conexao aceita");
    
    pthread_t thread;
    novo_socket = malloc(sizeof(*novo_socket));
    *novo_socket = socket_cliente;
    
    if(pthread_create(&thread,NULL , comunicacaoSocket, (void*) novo_socket) < 0)
    {
      puts("Nao foi possivel criar a thread.");
      close(*novo_socket);
      free(novo_socket);
      return;
    }
    puts("Thread criada");
  }
  if (socket_cliente < 0)
  {
    puts("Falha na aceitacao");
  }
  
  close(socket_servidor);
  return;
}

void ativaRele(int pino)
{
	digitalWrite(pino,LOW);
}
void desativaRele(int pino)
{
	digitalWrite(pino,HIGH);
}

void *comunicacaoSocket(void *conexao)
{
  puts("Thread de conexao iniciada");
  int tamanhoMensagemRecebida, i;
  char mensagem[TF_MENSAGEM];
  mensagem[0] = '\0';
  
  while(1) 
  {
	  tamanhoMensagemRecebida = recv(*(int*)conexao ,&mensagem, sizeof(mensagem), MSG_PEEK);
	  mensagem[tamanhoMensagemRecebida] = '\0';
	  printf("\nRecebido: %s\n",mensagem);
	  
	  for(i=0; i<tamanhoMensagemRecebida; i++)
	  {
	     mensagem[i] = toupper(mensagem[i]);
	  }
	  
	  if(strcmp(mensagem,"L")==0)
		ativaRele(PINORELE_1);
	  else
	  if(strcmp(mensagem,"D")==0)
		desativaRele(PINORELE_1);
	  
	  break; 
  }
  puts("Thread finalizada e socket fechado.");
  free(conexao);
  return 0;
}
