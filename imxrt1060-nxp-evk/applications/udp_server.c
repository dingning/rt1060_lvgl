#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>
#include <string.h>
#include <finsh.h>
#include <sys/time.h>

#define LOG_TAG              "udps"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"


#define ARGC_MAX 10
#define ARGV_LEN_MAX 64
struct msh_para
{
    char argc;
    char argv[ARGC_MAX][ARGV_LEN_MAX + 1];
};

struct msh_para udp_para;

#define BUFSZ 4096
#define UDP_IP "255.255.255.255"
#define UDP_PORT 2205

device_attribute_t dev_attr[5];
int dev_num = 0;
int sock;
struct sockaddr_in server_addr;

int udp_send_command(int index)
{
    char buff[16]= {0};
    buff[0] = 0xD1;
    buff[1] = dev_attr[index].cfg.name[0];
    buff[2] = dev_attr[index].cfg.name[1];
    buff[3] = 0xff;
    buff[4] = 0xff;
    buff[5] = dev_attr[index].cfg.switch_status[0];
    buff[6] = dev_attr[index].cfg.switch_status[1];
    buff[7] = dev_attr[index].cfg.switch_status[2];
    buff[8] = 0xff;
    buff[9] = 0xff;
    buff[10] = 0xff;
    buff[11] = 0xff;
    buff[12] = 0xff;
    buff[13] = 0xff;
    buff[14] = 0xff;
    buff[15] = 0xff;

    int ret = sendto(sock, buff, 16, 0,(struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if(ret != 16)
    {
        LOG_I("send command to %s failed.",dev_attr[index].cfg.name);
    }
    else
    {
        LOG_I("send command to %s success, command:%d %d %d.",dev_attr[index].cfg.name, buff[5], buff[6], buff[7]);
    }
}
int udp_data_process(char index, char * buf, int len)
{
    if(len != 16)
    {
        LOG_W("device upload len error.");
        return RT_FALSE;
    }
    if(buf[0] != 0xD0)
    {
        LOG_W("device upload header error.");
        return RT_FALSE;
    }
    //for (unsigned char i = 0; i < dev_num; i++)
    {
        //if((dev_attr[i].client_addr.sin_addr.s_addr == client_addr->sin_addr.s_addr) && (dev_attr[i].client_addr.sin_port == client_addr->sin_port) )
        {   
            //memcpy(&dev_attr[i].cfg.name[0],&buf[1],2);
           // dev_attr[i].cfg.name[2] = '\0';
            dev_attr[index].cfg.now_temperature = buf[3];
            dev_attr[index].cfg.now_humidity = buf[4];
            dev_attr[index].cfg.switch_status[0] = buf[5];
            dev_attr[index].cfg.switch_status[1] = buf[6];
            dev_attr[index].cfg.switch_status[2] = buf[7];

            // mqtt_alarm_up(i,i%2);
            // mail_alarm_up(i,i%2);
            LOG_I("device %s report data: %d %d %d %d %d", dev_attr[index].cfg.name, buf[3], buf[4], buf[5], buf[6], buf[7]);
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}
int get_dev_index(char *buff)
{
    for (char  i = 0; i < dev_num; i++)
    {
        if((dev_attr[i].cfg.name[0] == buff[1]) && (dev_attr[i].cfg.name[1] == buff[2]) )
        {
            return i;
        }
    }

    return -1;
    
}
void udptest_entry(void *para)
{
    struct msh_para *cpara = (struct msh_para *)para;
    //LOG_I("cpara->argc:%d\n", cpara->argc);
    for (int i = 0; i < cpara->argc; i++)
    {
        LOG_I("cpara->argv[%d]:%s\n", i, cpara->argv[i]);
    }

    char host_name[64];
    int host_port;

    if (cpara->argc == 1)
    {
        memcpy(host_name, UDP_IP, strlen(UDP_IP));
        host_name[strlen(UDP_IP)] = 0;
        host_port = UDP_PORT;
    }
    else if (cpara->argc == 2)
    {
        memcpy(host_name, cpara->argv[1], strlen(cpara->argv[1]));
        host_name[strlen(cpara->argv[1])] = 0;
        host_port = UDP_PORT;
    }
    else if (cpara->argc == 3)
    {
        memcpy(host_name, cpara->argv[1], strlen(cpara->argv[1]));
        host_name[strlen(cpara->argv[1])] = 0;
        host_port = atoi(cpara->argv[2]);
    }
    else
    {
        memcpy(host_name, UDP_IP, strlen(UDP_IP));
        host_name[strlen(UDP_IP)] = 0;
        host_port = UDP_PORT;
    }

    LOG_I("udp:%s,%d", host_name, host_port);

    struct hostent *host;
UDP_INIT:
    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    
    while ((host = gethostbyname(host_name)) == RT_NULL)
    {
        LOG_I("gethostbyname error!");
        return;
    }

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
   
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        LOG_I("Socket error");
        return;
    }
    LOG_I("creact socket:%d success.", sock);

    /* 初始化服务端地址 */
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(host_port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    //LOG_I("server url: %x:%x\r\n", server_addr.sin_addr.s_addr, server_addr.sin_port);
    //LOG_I("url: %s\r\n", inet_ntoa(server_addr.sin_addr));

    /* 绑定socket到服务端地址 */
    // if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    // {
    //     /* 绑定地址失败 */
    //     LOG_I("Bind error\n");
    //     closesocket(sock);
    //     return;
    // }
    socklen_t addr_len = sizeof(struct sockaddr);
    //LOG_I("UDPServer Waiting for client on port %d...\n",host_port);
	sendto(sock, "heleo", strlen("heleo"), 0,(struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    
    addr_len = sizeof(struct sockaddr);
    char recv_data[BUFSZ + 1];
   
    

    while (1)
    {
        int bytes_recv;
    
        struct sockaddr_in client_addr;
        /* 从sock中收取最大BUFSZ 字节数据 */
        bytes_recv = recvfrom(sock, recv_data, BUFSZ , 0, (struct sockaddr *)&client_addr, &addr_len);
        /* UDP不同于TCP，它基本不会出现收取的数据失败的情况，除非设置了超时等待 */
        recv_data[bytes_recv] = '\0'; /* 把末端清零 */

        /* 输出接收的数据 */
        //LOG_I("(%s , %d) said :%s\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),recv_data);
        
        int index = get_dev_index(recv_data);
        LOG_I("get index:%d\n",index);
        if(index < 0)
        {
            dev_attr[dev_num].client_addr = client_addr;
            dev_attr[dev_num].cfg.name[0] = recv_data[1];
            dev_attr[dev_num].cfg.name[1] = recv_data[2];
            dev_attr[dev_num].cfg.name[2] = '\0';
            index = dev_num;
            dev_num++;
        }
        udp_data_process(index,recv_data,bytes_recv);



#if defined(UDP_ECHO)
        sendto(sock,recv_data,bytes_read,0,(struct sockaddr *)&client_addr,sizeof(struct sockaddr));
#endif // UDP_ECHO

        rt_thread_mdelay(100);
    }
}
void udptest(int argc, char **argv)
{
    udp_para.argc = argc;
    for (char i = 0; i < argc; i++)
    {
        char size = strlen(argv[i]);
        if (size > 64)
        {
            LOG_I("argv[%d] over len.\n", i);
            return;
        }

        memcpy(udp_para.argv[i], argv[i], size);
    }
    rt_thread_t tid = rt_thread_create("udptest", (void (*)(void *parameter))udptest_entry, &udp_para, 8192, 15, 20);

    if (tid == RT_NULL)
    {
        LOG_I("rt_thread_create udptest failed!");
        return;
    }

    /* if you create a thread, never forget to start it */
    int result = rt_thread_startup(tid);
    if (result != RT_EOK)
    {
        LOG_I("rt_thread_startup udptest failed!");
        return;
    }
    return;
}
MSH_CMD_EXPORT(udptest, a udptest sample);
