#include "utils.h"
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psputility.h>
#include <pspnet_apctl.h>
#include <string.h>

int run = 1;

int select_netconfig()
{
    #define MAX_CONFIG 10
    struct {
        int config_n;
        char name[20];
    } net_list[MAX_CONFIG];
    
    /* Read net config list */
    int i, used = 0;
    for (i = 1; i <= MAX_CONFIG; ++i) {
        if (sceUtilityCheckNetParam(i) == 0) {
            netData data;
            sceUtilityGetNetParam(i, PSP_NETPARAM_NAME, &data);
            net_list[used].config_n = i;
            strncpy (net_list[used].name, data.asString, 20);
            ++used;
        }
    }
    
    printf("Select an access point:\n");
    
    int selected = 0, last_y = pspDebugScreenGetY();
    SceCtrlData pad, old_pad;
    sceCtrlPeekBufferPositive(&pad, 1);
    old_pad = pad;
    
    while (run) {
        sceCtrlPeekBufferPositive(&pad, 1);
        for (i = 0; i < used; ++i) {
            if (i == selected) printf("-> %s\n", net_list[i].name);
            else               printf("   %s\n", net_list[i].name);
        }
        if (pad.Buttons & PSP_CTRL_UP & ~old_pad.Buttons) {
            --selected;
            if (selected < 0) selected = used-1;    
        } else if (pad.Buttons & PSP_CTRL_DOWN & ~old_pad.Buttons) {
            ++selected;
            if (selected >= used) selected = 0;    
        }
        if (pad.Buttons & PSP_CTRL_CROSS & ~old_pad.Buttons) break;
        
        old_pad = pad;
        pspDebugScreenSetXY(0, last_y);
        sceDisplayWaitVblankStart();
    }
    
    return net_list[selected].config_n;
}

int connect_ap(int conn_n)
{
    sceNetApctlConnect(conn_n);
    int con_state = 0, last_con_state = 0;
    while (run) {
        sceNetApctlGetState(&con_state);
        if (con_state != last_con_state) {
            switch (con_state) {
            case PSP_NET_APCTL_STATE_DISCONNECTED:
                printf("Disconnected\n"); break;
            case PSP_NET_APCTL_STATE_SCANNING:
                printf("Scanning...\n"); break;
            case PSP_NET_APCTL_STATE_JOINING:
                printf("Joining...\n"); break;
            case PSP_NET_APCTL_STATE_GETTING_IP:
                printf("Getting IP...\n"); break;
            case PSP_NET_APCTL_STATE_GOT_IP:
                printf("Connected!\n"); return 1;
            case PSP_NET_APCTL_STATE_EAP_AUTH:
                printf("Authentification\n"); break;
            case PSP_NET_APCTL_STATE_KEY_EXCHANGE:
                printf("Key exchange...\n"); break;
            }
            last_con_state = con_state;   
        }
        sceKernelDelayThread(50*1000);
    }
    return 0;
}

int get_ip(char *ip)
{
    union SceNetApctlInfo info_ip;
    if (sceNetApctlGetInfo(PSP_NET_APCTL_INFO_IP, &info_ip) != 0) {
        return 0;
    }
    memcpy(ip, info_ip.ip, 16);
    return 1;
}


int ExitCallback(int arg1, int arg2, void *common)
{ 
    run = 0;
    return 0; 
} 

int CallbackThread(SceSize args, void *argp)
{ 
    int cbid; 
    cbid = sceKernelCreateCallback("Exit Callback", ExitCallback, NULL); 
    sceKernelRegisterExitCallback(cbid); 
    sceKernelSleepThreadCB(); 
    return 0; 
} 
 
int SetupExitCallback()
{ 
    int thid = 0;
    thid = sceKernelCreateThread("Callback Update Thread", CallbackThread, 0x11, 0xFA0, THREAD_ATTR_USER, 0); 
    if(thid >= 0) { 
        sceKernelStartThread(thid, 0, 0); 
    } 
    return thid; 
}
