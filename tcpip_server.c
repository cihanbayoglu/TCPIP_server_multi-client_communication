#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
 
#define DEFAULT_PORT        8888
 
 
DWORD WINAPI ClientThread(LPVOID lpParam)
{
   SOCKET sock =(SOCKET)lpParam;
   int  consume; // 
   char ch;
 
   while(1)
   {
      consume = recv(sock, &ch, 1, 0);
      if (consume == 0)               
         break;
      if (consume == SOCKET_ERROR) {
         fprintf(stderr, "recv() failed: %d\n", WSAGetLastError());
         break;
      }
      if (ch == '\x1b') //esc
         break;
      putchar(ch);
      
   }
   return 0;
}
 
int main(void)
{
   WSADATA wsd;
   SOCKET sListen,sClient;
   int addrSize;
   // bir Win32 nesnesini, örneðin bir dosyayý, bir muteksi, bir olayý vb. Temsil etmek için kullanýlan opak bir türdür.
   HANDLE hThread;
   //double word 4 byte ya da 32 bitlik bir hafýza alanýný temsil eder.
   DWORD dwThreadId;
   struct sockaddr_in local, client;
 
 	//WSAStartup,winsock kütüphanesini baþlatýr.2parametre alýr,ilki yüklemek istediðimiz versiyon,
   //ikincisi winsock yüklendikten sonra ek bilgi tutacak bir WSADATA yapýsýdýr.
   if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
      fprintf(stderr, "Failed to load Winsock!\n");
      return 1;
   }
   puts("Winsock nitialised.");
 
   // Create our listening socket
   //
 
   sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
   if (sListen == SOCKET_ERROR) {
      fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
      return 1;
   }
   puts("Socket created.");
 
 
   local.sin_addr.s_addr = inet_addr("127.0.0.1");
   local.sin_family = AF_INET;
   local.sin_port = htons(DEFAULT_PORT);
 
   if (bind(sListen, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) {
      fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
      return 1;
   }
   puts("Bind done.");
   
   listen(sListen, 8);
   
   puts(" Listening...");
 
   while(1)
   {
      addrSize = sizeof(client);
      sClient = accept(sListen, (struct sockaddr *) &client, &addrSize);        
      if (sClient == INVALID_SOCKET) {        
         fprintf(stderr, "accept() failed: %d\n", WSAGetLastError());
         break;
      }
      fprintf(stderr, "Accepted client: (IP:PORT) %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
 
 	  //CreateThread Güvenlik öznitelikleri üzerinde denetim saðlanmasýna yardýmcý olur.
	  //Bu iþlevi, askýya alýnmýþ durumda bir iþ parçacýðý baþlatmak için kullanabilirsiniz.
	  // birinci parametre: güvenlik bilgilerine iliþkindir NULL geçilebilir.
	  // ikinci parametre:  thread'ýn stack alanýdýr. O geçilirse 1 mb default deðer alýr.
	  // üçüncü parametre: thread fonksiyonunun baþlangýç adresini alýr.
	  // dördüncü parametre: thread fonksiyonuna geçirilecek olan parametre girilir.
	  //beþinci parametre: thread fonksiyonunun thread yaratýlýr yaratýlmaz çalýþýp çalýþmayacaðýný belirler. 0 geçilirse thread çalýþýr.
	  // altýncý parametre: thread'in  ID deðerinin yerleþeceði adrestir.
      hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sClient, 0, &dwThreadId);
      if (hThread == NULL) {
         fprintf(stderr, "CreateThread() failed: %d\n", GetLastError());
         break;
      }
      //ExitThread fonksiyonu yalnýzca thread akýþýný sonlandýrýr. Thread database olarak adlandýrýlan bu alan thread sonlanmýþ olsa bile
      // baþka amaçlar için hala kullanýlabilir. Bu yüzden dinamik veri alaný CloseHandle fonksiyonu ile sonlandýrýlýr.
      CloseHandle(hThread);
   }
   closesocket(sListen);
 
   WSACleanup();
 
   return 0;
}
