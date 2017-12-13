/*
 ============================================================================
 Name        : server_tcp.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>

int split (const char *str, char c, char ***arr)

{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
}

typedef struct Node
{
    int data;
    struct Node *next;
}NODE;

NODE *sort( NODE * );
NODE *list_switch( NODE *, NODE * );


void dodaj_do_listy(struct Node **start_ref, int data);

void swap(struct Node *a, struct Node *b);
void operacja(char** arr, int arguments_value);
void printList(struct Node *start);
int dodawanie(struct Node *start);
int odejmowanie(struct Node *start);
void reverse(struct Node** head_ref);
void wyslij_liste(struct Node *start);
void deleteList(struct Node** head_ref);
void rosnaco(struct Node *start);




int sock_desc, newsock_desc, port_number;

int main(int argc, char *argv[]) {


    socklen_t clilen;
    char buffer[256];
    char* new_buffer = malloc(30* sizeof(char));
    struct sockaddr_in serv_addr, cli_addr;
    int flag_RxTx;
    int arguments_value = 0;
    char** arr = NULL;
    double wynik_operacji;

    struct Node *start = NULL;

    if (argc < 2) {

    	fprintf(stderr, "ERROR, no port provided\n");
         exit(1);
    }

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_desc < 0)
         printf("ERROR opening socket");


    bzero((char *) &serv_addr, sizeof(serv_addr));

    port_number = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sock_desc, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        printf("ERROR on binding");


    listen(sock_desc, 5);

    clilen = sizeof(cli_addr);
    //Below code is modified to handle multiple clients using fork
    //------------------------------------------------------------------
    int pid;
    while (1) {
         newsock_desc = accept(sock_desc, (struct sockaddr *) &cli_addr, &clilen);

         if (newsock_desc < 0)
                printf("ERROR on accept");

         //fork new process
         pid = fork();


         if (pid < 0) {
              error("ERROR in new process creation");
         }
         if (pid == 0) {
        	 printf("Połączono z klientem numer: %d\n",newsock_desc);
            //child process
            //close(sock_desc);
            //do whatever you want
        	 while(1){
				 bzero(buffer,256);

				flag_RxTx = read(newsock_desc, buffer, 255);

				if (flag_RxTx < 0) {
					printf("ERROR reading from socket");
					exit(1);
				}

				if (flag_RxTx == 0) {

					close(newsock_desc);
					break;
				}


				if(strstr(buffer, "koniec") != NULL) {
					printf("Klient %d konczy program\n",newsock_desc);
					write(newsock_desc, "Koniec na dzis",14);
					//close(newsock_desc);
					exit(0);
				}
				//przetwazanie danych z bufora*****************
				arguments_value = split(buffer, ' ', &arr);

				printf("znaleziono elementow: %d\n",arguments_value);

//				if(strcmp(arr[0], "add") == 0){
//					int suma=0;
//					puts("dodawanie");
//					for(int i=1;i<arguments_value;i++){
//
//						suma = suma + atof(arr[i]);
//
//					}
//					sprintf(buffer, "%d", suma);
//					//write(newsock_desc,buffer,strlen(buffer));
//				}




				//*****************************
				operacja(arr,arguments_value);
				perror("operacja stats-> ");
				//printf("wynik dodawania = %c\n", new_buffer);


				//printf("Here is the message: %s\n", buffer);
				//flag_RxTx = write(newsock_desc, new_buffer, strlen(new_buffer));

				//perror("wysylanie->");
//				if (flag_RxTx < 0)
//					 printf("ERROR writing to socket");
//				//close(newsock_desc);
//				free(new_buffer);
        	}
         }
        	 else {
             //parent process
             //close(newsockfd);
          }
    }
    //-------------------------------------------------------------------
   return 0;
}


void dodaj_do_listy(struct Node **start_ref, int data)
{
    struct Node *ptr1 = (struct Node*)malloc(sizeof(struct Node));
    ptr1->data = data;
    ptr1->next = *start_ref;
    *start_ref = ptr1;
}

//void dodaj_do_char(char** arr, int arguments_value) {
//
//	struct Node *ptr = start;
//		int suma = 0;
//		puts("dodawanie");
//
//		while(ptr != NULL) {
//
//
//			suma += ptr->data;
//			ptr = ptr->next;
//
//		}
//
//}


/* function to swap data of two nodes a and b*/
void swap(struct Node *a, struct Node *b) {
    int temp = a->data;
    a->data = b->data;
    b->data = temp;
}

void operacja(char** arr, int arguments_value) {

	struct Node *start = NULL;
	char wynik[256];
	int wynik_operacji = 0;


	for(int i=1;i<arguments_value;i++){

		dodaj_do_listy(&start,atof(arr[i]));

	}
	if(strcmp(arr[0], "sort") == 0){
//		printf("\n Linked list before sorting ");
		    printList(start);
		    rosnaco(start);
		    printList(start);
		    wyslij_liste(start);

//		printf("\n Linked list after sorting ");
		//printList(start);


	} else if(strcmp(arr[0], "dodaj") ==0){

		wynik_operacji = dodawanie(start);
		perror("operation dodawanie status => ");
		sprintf(wynik, "%d", wynik_operacji);
		//wynik = (char)wynik_operacji;

		perror("sprintf status => ");
		write(newsock_desc, wynik, strlen(wynik));

	} else if(strcmp(arr[0], "odejmij") == 0) {

		wynik_operacji  = odejmowanie(start);
		perror("operation odejmowanie status => ");
		sprintf(wynik, "%d", wynik_operacji);
		write(newsock_desc, wynik, strlen(wynik));

	}

	deleteList(&start);




}

void printList(struct Node *start)
{
    struct Node *temp = start;
    printf("\n");
    while (temp!=NULL)
    {
        printf("%d \n", temp->data);
        temp = temp->next;
    }
}

int dodawanie(struct Node *start) {

	struct Node *ptr = start;
	int suma = 0;
	puts("dodawanie");

	while(ptr != NULL) {


		suma += ptr->data;
		ptr = ptr->next;

	}
	printf("suma wynosi: %d\n",suma);
	perror("dodawanie status-> ");
	return suma;



}

int odejmowanie(struct Node *start){


	reverse(&start);
	struct Node *ptr = start;

	int roznica = ptr->data;
	puts("odejmowanie");
	ptr=ptr->next;
	while(ptr != NULL) {

		roznica -= ptr->data;
		ptr=ptr->next;

	}

	return roznica;
}



void reverse(struct Node** head_ref)
{
    struct Node* prev   = NULL;
    struct Node* current = *head_ref;
    struct Node* next;
    while (current != NULL)
    {
        next  = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    *head_ref = prev;
}

//NODE *sort( NODE *start )
//{
//	NODE *p, *q, *top;
//    int changed = 1;
//
//    /*
//    * We need an extra item at the top of the list just to help
//    * with assigning switched data to the 'next' of a previous item.
//    * It (top) gets deleted after the data is sorted.
//    */
//
//    if( (top = (NODE *)malloc(sizeof(NODE))) == NULL) {
//        fprintf( stderr, "Memory Allocation error.\n" );
//        // In Windows, replace following with a return statement.
//        exit(1);
//    }
//
//    top->next = start;
//    if( start != NULL && start->next != NULL ) {
//        /*
//        * This is a survival technique with the variable changed.
//        *
//        * Variable q is always one item behind p. We need q, so
//        * that we can make the assignment q->next = list_switch( ... ).
//        */
//
//        while( changed ) {
//            changed = 0;
//            q = top;
//            p = top->next;
//            while( p->next != NULL ) {
//                /* push bigger items down */
//                if( p->data > p->next->data ) {
//                    q->next = list_switch( p, p->next );
//                    changed = 1;
//                }
//                q = p;
//                if( p->next != NULL )
//                    p = p->next;
//            }
//        }
//    }
//    p = top->next;
//    free( top );
//    return p;
//}

NODE *sort( NODE *start )
{
	//char buffer[256];

    if( start == NULL ) return NULL;
    /* First push the larger items down */
    if( start->next !=NULL && start->data > start->next->data )
        start = list_switch( start, start->next );
    /* Always sort from second item on */
    start->next = sort(start->next);
    /* bubble smaller items up */
    if( start->next !=NULL && start->data > start->next->data ) {
        start = list_switch( start, start->next );
        start->next = sort(start->next);
    }

//    struct Node *ptr = start;
//        int i = 0;
//        while(ptr != NULL) {
//
//    		sprintf(buffer[i],"%d",ptr->data);
//        	//buffer[i]= ptr->data;
//    		ptr = ptr->next;
//
//    	}
//        write(newsock_desc, buffer, strlen(buffer));
    return start;
}

NODE *list_switch( NODE *l1, NODE *l2 )
{
    l1->next = l2->next;
    l2->next = l1;
    return l2;
}

void wyslij_liste(struct Node *start) {
	struct Node *ptr = start;
	//char str[] = "Posortowane liczby: ";
	perror("posortowane liczby status => ");
	//write(newsock_desc, str, strlen(str));
	char str2[256];
	//bzero(str2,strlen(str2));
	perror("przed petla while status=>");
	while (ptr != NULL) {
		sprintf(str2, "%d ", ptr->data);
		//write(newsock_desc, str2, strlen(str2));
		perror("write while status=>");

		write(newsock_desc, str2, strlen(str2));
		ptr = ptr->next;
		bzero(str2,strlen(str2));
		perror("petla while status=>");

	}
	//str2[256] = NULL;

	//free(str2);
	//free(str);
}

void deleteList(struct Node** head_ref) {
	/* deref head_ref to get the real head */
	struct Node* current = *head_ref;
	struct Node* next;
	while (current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	*head_ref = NULL;
}

void rosnaco(struct Node *start) {
	int swapped, i;
	struct Node *ptr1;
	struct Node *lptr = NULL;
	if (ptr1 == NULL)
		return;
	do {
		swapped = 0;
		ptr1 = start;

		while (ptr1->next != lptr) {
			if (ptr1->data > ptr1->next->data) {
				swap(ptr1, ptr1->next);
				swapped = 1;
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	} while (swapped);
}

