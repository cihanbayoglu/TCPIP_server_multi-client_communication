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
   // bir Win32 nesnesini, �rne�in bir dosyay�, bir muteksi, bir olay� vb. Temsil etmek i�in kullan�lan opak bir t�rd�r.
   HANDLE hThread;
   //double word 4 byte ya da 32 bitlik bir haf�za alan�n� temsil eder.
   DWORD dwThreadId;
   struct sockaddr_in local, client;
 
 	//WSAStartup,winsock k�t�phanesini ba�lat�r.2parametre al�r,ilki y�klemek istedi�imiz versiyon,
   //ikincisi winsock y�klendikten sonra ek bilgi tutacak bir WSADATA yap�s�d�r.
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
 
 	  //CreateThread G�venlik �znitelikleri �zerinde denetim sa�lanmas�na yard�mc� olur.
	  //Bu i�levi, ask�ya al�nm�� durumda bir i� par�ac��� ba�latmak i�in kullanabilirsiniz.
	  // birinci parametre: g�venlik bilgilerine ili�kindir NULL ge�ilebilir.
	  // ikinci parametre:  thread'�n stack alan�d�r. O ge�ilirse 1 mb default de�er al�r.
	  // ���nc� parametre: thread fonksiyonunun ba�lang�� adresini al�r.
	  // d�rd�nc� parametre: thread fonksiyonuna ge�irilecek olan parametre girilir.
	  //be�inci parametre: thread fonksiyonunun thread yarat�l�r yarat�lmaz �al���p �al��mayaca��n� belirler. 0 ge�ilirse thread �al���r.
	  // alt�nc� parametre: thread'in  ID de�erinin yerle�ece�i adrestir.
      hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sClient, 0, &dwThreadId);
      if (hThread == NULL) {
         fprintf(stderr, "CreateThread() failed: %d\n", GetLastError());
         break;
      }
      //ExitThread fonksiyonu yaln�zca thread ak���n� sonland�r�r. Thread database olarak adland�r�lan bu alan thread sonlanm�� olsa bile
      // ba�ka ama�lar i�in hala kullan�labilir. Bu y�zden dinamik veri alan� CloseHandle fonksiyonu ile sonland�r�l�r.
      CloseHandle(hThread);
   }
   closesocket(sListen);
 
   WSACleanup();
 
   return 0;
}
