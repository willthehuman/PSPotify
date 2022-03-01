#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psputility.h>
#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "utils.h"

PSP_MODULE_INFO("pspnetcat", PSP_MODULE_USER, 0, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#define PORT 4444
#define IP "192.168.1.33"

void cleanup();
void net_apctl_event_handler(int oldState, int newState, int event, int error, void *pArg);

int main(int argc, char *argv[])
{
    pspDebugScreenInit();
    SetupExitCallback();
    
    printf("pspnetcat | netcat client\n");
    
    sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
    
    sceNetInit(64*1024, 32, 2*1024, 32, 2*1024);
    sceNetInetInit();
    sceNetApctlInit(0x2000, 20);
    sceNetApctlAddHandler(net_apctl_event_handler, NULL);

    int net = select_netconfig();
    connect_ap(net);
    printf("Connected to " IP " on port %i\n", PORT);
    char psp_ip[16];
    if (get_ip(psp_ip))
        printf("PSP's IP: %s\n", psp_ip);
    else
        printf("Could not get PSP IP address\n");
    
    int sock = sceNetInetSocket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in sa_dst;
    memset(&sa_dst, 0, sizeof(struct sockaddr_in));
    sa_dst.sin_family = AF_INET;
    sa_dst.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &sa_dst.sin_addr.s_addr);
    
    sceNetInetConnect(sock, (struct sockaddr *) &sa_dst, sizeof(struct sockaddr_in));
    
    char str[] = "Hello from the PSP!\n";
    sceNetInetSend(sock, str, sizeof(str), 0);
    
    char buf[512];
    while (1) {
        memset(buf, 0, sizeof(buf));
        sceNetInetRecv(sock, buf, 512, 0);
        printf("Received: %s", buf);
        sceDisplayWaitVblankStart();
    }
    
    sceNetInetClose(sock);
    
    cleanup();
    sceKernelExitGame();
    return 0;
}

void net_apctl_event_handler(int oldState, int newState, int event, int error, void *pArg)
{
    if (newState == PSP_NET_APCTL_STATE_DISCONNECTED) {
        printf("Disconnected!\n");  
    }
}

void cleanup()
{
    sceNetApctlTerm();
    sceNetInetTerm();
    sceNetTerm();
    sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);
    sceUtilityUnloadNetModule(PSP_NET_MODULE_INET);
}
