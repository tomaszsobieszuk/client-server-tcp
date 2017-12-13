/*
 ============================================================================
 Name        : client_tcp.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> //socket
#include<arpa/inet.h> //inet_addr
#include<string.h>//strlen
#include <unistd.h> //close
#include <netdb.h>
#include <netinet/in.h>


int main(int argc, char *argv[]) {

	int socket_desc = -1;
	int port_number;
	struct sockaddr_in server_addr;
	struct hostent *server;
	char buffer[256];
	int flag_RxTx;





	socket_desc = socket(AF_INET , SOCK_STREAM , 0);

	if (socket_desc == -1) {
		printf("Bład utworzenia gniazda \n");
	} else {
		printf("Utworzono gniazdo o numerze: %d \n", socket_desc);
	}

	server = gethostbyname(argv[1]);
		if(server == NULL) {
			puts("Host error");
			exit(1);
	}

		bzero((char *) &server_addr,sizeof(server_addr));
		//bzero((char *) &server_addr, sizeof(server_addr));
		bcopy((char *) &server_addr.sin_addr.s_addr,(char *) server->h_addr,server->h_length);

		//bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);

	port_number = atoi(argv[2]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons( port_number );

	//Polacz z serwerem zdalnym (TCP)
	if (connect(socket_desc , (struct sockaddr *)&server_addr , sizeof(server_addr)) < 0) {
		puts("Blad polaczenia");
		return 1;
	}

	puts("Połączono");

	sleep(2);

	puts("Dostepne operacje:\n");
	puts("\tdodaj liczba1 liczba2 ... liczba(n)\n");
	puts("\todejmij liczba1 liczba2 ... liczba(n)\n");
	puts("\tsortujm liczba1 liczba2 ... liczba(n)\n");
	puts("\tsortujg liczba1 liczba2 ... liczba(n)\n");
	puts("\tkoniec\n");
	while(1) {

		//wysylanie danych

		printf("Co chcesz zrobic?: ");
		bzero((char *) &buffer,sizeof(buffer));
		fgets(buffer,256,stdin);
		if(strstr(buffer, "  ") != NULL) {
					puts("Bledne dane");
					continue;
		}


		if( (flag_RxTx = write(socket_desc,buffer,strlen(buffer))) < 0 ) {

			printf("Błąd wysyłania do serwera");
			exit(1);

		} //else printf("wyslano dane: %d",atoi(buffer));

		bzero(buffer,sizeof(buffer));

		if ((flag_RxTx = read(socket_desc, buffer, 255)) < 0) {

			printf("Błąd odebrania od serwera");
			exit(1);

		}

		printf("Odebrano od serwera: %s\n", buffer);
		if (strstr(buffer, "Koniec na dzis") != NULL){
			exit(0);
		}





	}


	//puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	return EXIT_SUCCESS;
}
