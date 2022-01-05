#include <winsock2.h>
#include <stdio.h>
#include <conio.h>		// getch
#include <stdlib.h>		// stderr
 
#define DEFAULT_PORT        8888
 
 
int main(void)
{
   WSADATA wsd;
   SOCKET sClient;
   int consume, ch;
   struct sockaddr_in server;
 
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      fprintf(stderr, "Failed to load Winsock library!\n");
      return 1;
   }
   
   puts("Winsock nitialised.");
 
 
   sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sClient == INVALID_SOCKET) {
      fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
      return 1;
   }
   
   puts("Socket created.");
   
   server.sin_family = AF_INET;
   server.sin_port = htons(DEFAULT_PORT);
   server.sin_addr.s_addr = inet_addr("127.0.0.1");
 
   
   if (connect(sClient, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
      fprintf(stderr, "connect() failed: %d\n", WSAGetLastError());
      return 1;
   }
   
   puts("Connected.");
 
	while(1)
	{
      ch = getch();
      if (ch == '\r')		 
         ch = '\n';
      putchar(ch);
      consume = send(sClient, (char *) &ch, 1, 0);
      if (consume == 0)
         break;
      if (consume == SOCKET_ERROR) {
         fprintf(stderr, "send() failed: %d\n", WSAGetLastError());
         break;
      }
      if (ch == '\x1b')		
         break;
    }
 
   closesocket(sClient);
 
   WSACleanup();
 
   return 0;
}
