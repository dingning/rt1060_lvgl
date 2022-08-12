/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-29     tyustli      first version
 */

#include "MIMXRT1062.h"
#include <rtdevice.h>
#include "drv_gpio.h"
#include "core_cm7.h"

#define LOG_TAG "main"
#define LOG_LVL LOG_LVL_DBG
#include "global.h"
#include <ulog_be.h>

struct ulog_file_be g_file_be;

/* defined the LED pin: GPIO1_IO9 */
#define LED0_PIN GET_PIN(1, 8)

int main(void)
{
    ulog_file_backend_init(&g_file_be, "uf", "/", 1, 20 * 1024, 100);
    ulog_file_backend_enable(&g_file_be);

    LOG_I("%s start ...", GW_ID);
    rt_thread_mdelay(5000);
    udptest(0, RT_NULL);
    umqtt_ex_start(1,RT_NULL);
    alram_timer_start();
    weather(0, RT_NULL);

#ifndef PHY_USING_KSZ8081
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(100);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(100);
    }
#endif
}

void reboot(void)
{
    NVIC_SystemReset();
}
MSH_CMD_EXPORT(reboot, reset system)

#include <dfs_fs.h>
#include "dfs_ramfs.h"

int mnt_init(void)
{
    if (dfs_mount(RT_NULL, "/", "ram", 0, dfs_ramfs_create(rt_malloc(1024 * 1024), 1024 * 1024)) == 0)
    {
        rt_kprintf("RAM file system initializated!\n");
    }
    else
    {
        rt_kprintf("RAM file system initializate failed!\n");
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);
/*
 * ????:tcp ???
 *
 * ???? tcp ??????
 * ?? tcpclient ???????
 * ??????:tcpclient URL PORT
 * URL:????? PORT::???
 * ????:????????????????,?????? 'q' ? 'Q' ???????
 */
#include <rtthread.h>
#include <sys/socket.h> /* ??BSD socket,????socket.h??? */
#include "netdb.h"

#define BUFSZ 1024

static const char send_data[] = "This is TCP Client from RT-Thread."; /* ??????? */
void tcpclient(int argc, char **argv)
{
    int ret;
    char *recv_data;
    struct hostent *host;
    int sock, bytes_received;
    struct sockaddr_in server_addr;
    const char *url;
    int port;

    if (argc < 3)
    {
        rt_kprintf("Usage: tcpclient URL PORT\n");
        rt_kprintf("Like: tcpclient 192.168.12.44 5000\n");
        return;
    }

    url = argv[1];
    port = strtoul(argv[2], 0, 10);

    /* ????????url??host??(?????,??????) */
    host = gethostbyname(url);

    /* ????????????? */
    recv_data = rt_malloc(BUFSZ);
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* ????socket,???SOCKET_STREAM,TCP?? */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* ??socket?? */
        rt_kprintf("Socket error\n");

        /* ?????? */
        rt_free(recv_data);
        return;
    }

    /* ???????????? */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* ?????? */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* ???? */
        rt_kprintf("Connect fail!\n");
        closesocket(sock);

        /*?????? */
        rt_free(recv_data);
        return;
    }

    while (1)
    {
        /* ?sock???????BUFSZ - 1???? */
        bytes_received = recv(sock, recv_data, BUFSZ - 1, 0);
        if (bytes_received < 0)
        {
            /* ????,?????? */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");

            /* ?????? */
            rt_free(recv_data);
            break;
        }
        else if (bytes_received == 0)
        {
            /* ?? recv ?????,????0??????,?????? */
            closesocket(sock);
            rt_kprintf("\nreceived error,close the socket.\r\n");

            /* ?????? */
            rt_free(recv_data);
            break;
        }

        /* ??????,????? */
        recv_data[bytes_received] = '\0';

        if (strncmp(recv_data, "q", 1) == 0 || strncmp(recv_data, "Q", 1) == 0)
        {
            /* ???????q?Q,?????? */
            closesocket(sock);
            rt_kprintf("\n got a 'q' or 'Q',close the socket.\r\n");

            /* ?????? */
            rt_free(recv_data);
            break;
        }
        else
        {
            /* ???????????? */
            rt_kprintf("\nReceived data = %s ", recv_data);
        }

        /* ?????sock?? */
        ret = send(sock, send_data, strlen(send_data), 0);
        if (ret < 0)
        {
            /* ????,?????? */
            closesocket(sock);
            rt_kprintf("\nsend error,close the socket.\r\n");

            rt_free(recv_data);
            break;
        }
        else if (ret == 0)
        {
            /* ??send??????0????? */
            rt_kprintf("\n Send warning,send function return 0.\r\n");
        }
    }
    return;
}
MSH_CMD_EXPORT(tcpclient, a tcp client sample);

/*
 * 程序清单：tcp 服务�?
 *
 * 这是一�? tcp 服务�?的例�?
 * 导出 tcpserv 命令到控制终�?
 * 命令调用格式：tcpserv
 * 无参�?
 * 程序功能：作为一�?服务�?，接收并显示客户�?发来的数�? ，接收到 exit 退出程�?
 */
#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文�? */
#include "netdb.h"

//#define BUFSZ       (1024)

static const char send_data2[] = "This is TCP Server from RT-Thread."; /* 发送用到的数据 */
static void tcpserv(int argc, char **argv)
{
    char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求�?用内�? */
    socklen_t sin_size;
    int sock, connected, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    rt_bool_t stop = RT_FALSE; /* 停�?�标�? */
    int ret;

    recv_data = rt_malloc(BUFSZ + 1); /* 分配接收用的数据缓冲 */
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* 一个socket在使用前，需要�?�先创建出来，指定SOCK_STREAM为TCP的socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建失败的错�?处理 */
        rt_kprintf("Socket error\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 初�?�化服务�?地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000); /* 服务�?工作的�??�? */
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务�??地址 */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 绑定失败 */
        rt_kprintf("Unable to bind\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 在socket上进行监�? */
    //    if (listen(sock, 5) == -1)
    //    {
    //        rt_kprintf("Listen error\n");

    //        /* release recv buffer */
    //        rt_free(recv_data);
    //        return;
    //    }

    rt_kprintf("\nTCPServer Waiting for client on port 5000...\n");
    while (stop != RT_TRUE)
    {
        sin_size = sizeof(struct sockaddr_in);

        /* 接受一�?客户�?连接socket的�?�求，这�?函数调用�?阻�?�式�? */
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        /* 返回的是连接成功的socket */
        if (connected < 0)
        {
            rt_kprintf("accept connection failed! errno = %d\n", errno);
            continue;
        }

        /* 接受返回的client_addr指向了�?�户�?的地址信息 */
        rt_kprintf("I got a connection from (%s , %d)\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        /* 客户�?连接的�?�理 */
        while (1)
        {
            /* 发送数�?到connected socket */
            ret = send(connected, send_data2, strlen(send_data2), 0);
            if (ret < 0)
            {
                /* 发送失败，关闭这个连接 */
                closesocket(connected);
                rt_kprintf("\nsend error,close the socket.\r\n");
                break;
            }
            else if (ret == 0)
            {
                /* 打印send函数返回值为0的�?�告信息 */
                rt_kprintf("\n Send warning,send function return 0.\r\n");
            }

            /* 从connected socket�?接收数据，接收buffer�?1024大小，但并不一定能够收�?1024大小的数�? */
            bytes_received = recv(connected, recv_data, BUFSZ, 0);
            if (bytes_received < 0)
            {
                /* 接收失败，关�?这个connected socket */
                closesocket(connected);
                break;
            }
            else if (bytes_received == 0)
            {
                /* 打印recv函数返回值为0的�?�告信息 */
                rt_kprintf("\nReceived warning,recv function return 0.\r\n");
                closesocket(connected);
                break;
            }

            /* 有接收到数据，把�?�?清零 */
            recv_data[bytes_received] = '\0';
            if (strcmp(recv_data, "q") == 0 || strcmp(recv_data, "Q") == 0)
            {
                /* 如果�?首字母是q或Q，关�?这个连接 */
                closesocket(connected);
                break;
            }
            else if (strcmp(recv_data, "exit") == 0)
            {
                /* 如果接收的是exit，则关闭整个服务�? */
                closesocket(connected);
                stop = RT_TRUE;
                break;
            }
            else
            {
                /* 在控制终�?显示收到的数�? */
                rt_kprintf("RECEIVED DATA = %s \n", recv_data);
            }
        }
    }

    /* 退出服�? */
    closesocket(sock);

    /* 释放接收缓冲 */
    rt_free(recv_data);

    return;
}
MSH_CMD_EXPORT(tcpserv, a tcp server sample);
#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含sockets.h头文�? */
#include "netdb.h"

const char send_datau[] = "This is UDP Client from RT-Thread.\n"; /* 发送用到的数据 */
void udpclient(int argc, char **argv)
{
    int sock, port, count;
    struct hostent *host;
    struct sockaddr_in server_addr;
    const char *url;

    if (argc < 3)
    {
        rt_kprintf("Usage: udpclient URL PORT [COUNT = 10]\n");
        rt_kprintf("Like: tcpclient 192.168.12.44 5000\n");
        return;
    }

    url = argv[1];
    port = strtoul(argv[2], 0, 10);

    if (argc > 3)
        count = strtoul(argv[3], 0, 10);
    else
        count = 10;

    /* 通过函数入口参数url获得host地址（�?�果�?域名，会做域名解析） */
    host = (struct hostent *)gethostbyname(url);

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }

    /* 初�?�化预连接的服务�?地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 总�?�发送count次数�? */
    while (count)
    {
        /* 发送数�?到服务远�? */
        sendto(sock, send_datau, strlen(send_datau), 0,
               (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

        struct sockaddr_in client_addr;
        /* 从sock�?收取最�?BUFSZ - 1字节数据 */
        char recv_data[100];
        int addr_len;
        int bytes_read = recvfrom(sock, recv_data, BUFSZ - 1, 0,
                                  (struct sockaddr *)&client_addr, &addr_len);
        /* UDP不同于TCP，它基本不会出现收取的数�?失败的情况，除非设置了超时等�? */

        recv_data[bytes_read] = '\0'; /* 把末�?清零 */

        /* 输出接收的数�? */
        rt_kprintf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
        rt_kprintf("%s", recv_data);

        /* 线程休眠一段时�? */
        rt_thread_delay(50);

        /* 计数值减一 */
        count--;
    }

    /* 关闭这个socket */
    closesocket(sock);
}
MSH_CMD_EXPORT(udpclient, a udp client sample);

/*
 * 程序清单：udp 服务�?
 *
 * 这是一�? udp 服务�?的例�?
 * 导出 udpserv 命令到控制终�?
 * 命令调用格式：udpserv
 * 无参�?
 * 程序功能：作为一�?服务�?，接收并显示客户�?发来的数�? ，接收到 exit 退出程�?
 */
#include <rtthread.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文�? */
#include "netdb.h"

#define BUFSZ 1024

static void udpserv(int argc, char **argv)
{
    int sock;
    int bytes_read;
    char *recv_data;
    socklen_t addr_len;
    struct sockaddr_in server_addr, client_addr;

    /* 分配接收用的数据缓冲 */
    recv_data = rt_malloc(BUFSZ);
    if (recv_data == RT_NULL)
    {
        /* 分配内存失败，返�? */
        rt_kprintf("No memory\n");
        return;
    }

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");

        /* 释放接收用的数据缓冲 */
        rt_free(recv_data);
        return;
    }

    struct hostent *host;
    host = (struct hostent *)gethostbyname("255.255.255.255");

    /* 初�?�化服务�?地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    // server_addr.sin_addr.s_addr = inet_addr();
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务�??地址 */
    // if (bind(sock, (struct sockaddr *)&server_addr,
    //          sizeof(struct sockaddr)) == -1)
    // {
    //     /* 绑定地址失败 */
    //     rt_kprintf("Bind error\n");

    //     /* 释放接收用的数据缓冲 */
    //     rt_free(recv_data);
    //     return;
    // }

    addr_len = sizeof(struct sockaddr);
    rt_kprintf("UDPServer Waiting for client on port 5000...\n");
    sendto(sock, send_datau, strlen(send_datau), 0,
           (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    while (1)
    {
        /* 从sock�?收取最�?BUFSZ - 1字节数据 */
        bytes_read = recvfrom(sock, recv_data, BUFSZ - 1, 0,
                              (struct sockaddr *)&client_addr, &addr_len);
        /* UDP不同于TCP，它基本不会出现收取的数�?失败的情况，除非设置了超时等�? */

        recv_data[bytes_read] = '\0'; /* 把末�?清零 */

        /* 输出接收的数�? */
        rt_kprintf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
        rt_kprintf("%s", recv_data);

        /* 如果接收数据是exit，退�? */
        if (strcmp(recv_data, "exit") == 0)
        {
            closesocket(sock);

            /* 释放接收用的数据缓冲 */
            rt_free(recv_data);
            break;
        }
    }

    return;
}
MSH_CMD_EXPORT(udpserv, a udp server sample);
