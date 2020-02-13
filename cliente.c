#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <stdio.h>       
#include <string.h>    	 
#include <stdbool.h>	 
#include <stdlib.h>		 

#define TF_MENSAGEM    1      
#define PORTA 		   13260   

void close(int); 			   

bool conecta(int *conexao)
{
	struct sockaddr_in client;
	
	*conexao = socket(AF_INET , SOCK_STREAM , 0);
	
	if (*conexao == -1)
	  return false;
	  
	puts("Socket criado");
	
	client.sin_addr.s_addr = inet_addr("");
	client.sin_family = AF_INET;
	client.sin_port = htons(PORTA);

	if (connect(*conexao , (struct sockaddr *)&client, sizeof(client)) < 0)
	  return false;

	puts("Conectado\n");
	return true;
}

int main()
{
	int conexao;
	char msg[TF_MENSAGEM];
	puts("Digite a mensagem:\n[1] - Para sair\n");
	fflush(stdin);
	gets(msg);
	while(strcmp(msg,"1")!=0)
	{
		if(!conecta(&conexao))
		{
			puts("Erro de conexão");
			break;
		}
		if(send(conexao ,&msg, TF_MENSAGEM, 0) < 0) 
			puts("Falha no envio\n");
		else
			puts("Enviada!\n");
		
		puts("Digite a mensagem:\n[1] - Para sair\n");
		fflush(stdin);
		gets(msg);
	}
	close(conexao);
	return 0;
}
