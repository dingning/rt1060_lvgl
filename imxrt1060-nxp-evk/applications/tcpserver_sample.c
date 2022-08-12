/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-07-11     flybreak     the first version
 */

#include "tcpserver.h"
#include <string.h>

#define DBG_TAG   "tcpserv"
#define DBG_LVL   DBG_LOG
#include <rtdbg.h>

#define LOG_TAG              "tcpserver"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"


device_attribute_t dev_attr[5];
int dev_num = 0;

#define MAX_CLIENT_COUNT 5

static struct tcpserver *serv;
static char buf[1024];

int send_command(int index)
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

    int ret =tcpserver_send( dev_attr[index].client,buff,16,5000);
    if(ret != 16)
    {
        LOG_W("send command to %s failed.",dev_attr[index].cfg.name);
    }
    else
    {
        LOG_W("send command to %s success, command:%d %d %d.",dev_attr[index].cfg.name, buff[5], buff[6], buff[7]);
    }
}
int data_process(tcpclient_t client, char * buf, int len)
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
    for (unsigned char i = 0; i < dev_num; i++)
    {
        if(dev_attr[i].client->sock == client->sock)
        {   
            memcpy(&dev_attr[i].cfg.name[0],&buf[1],2);
            dev_attr[i].cfg.name[2] = '\0';
            dev_attr[i].cfg.now_temperature = buf[3];
            dev_attr[i].cfg.now_humidity = buf[4];
            dev_attr[i].cfg.switch_status[0] = buf[5];
            dev_attr[i].cfg.switch_status[1] = buf[6];
            dev_attr[i].cfg.switch_status[2] = buf[7];

            // mqtt_alarm_up(i,i%2);
            // mail_alarm_up(i,i%2);
            LOG_I("device %s report data: %d %d %d %d %d", dev_attr[i].cfg.name, buf[3], buf[4], buf[5], buf[6], buf[7]);
            return RT_TRUE;
        }
    }
    return RT_FALSE;
}
static void tcpserver_event_notify(tcpclient_t client, rt_uint8_t event)
{
    int ret;
    switch (event)
    {
    case TCPSERVER_EVENT_CONNECT:
        LOG_I("new device connect, socket fd:%d", client->sock);
        dev_attr[dev_num].client = client;
        dev_num ++;
        break;
    case TCPSERVER_EVENT_RECV:
        ret = tcpserver_recv(client, buf, 1024, -1);
        if (ret > 0)
        {
            ret = data_process(client, buf, ret);
        }
        break;
    case TCPSERVER_EVENT_DISCONNECT:
        LOG_D("client disconnect, socket fd:%d", client->sock);
        for (unsigned char i = 0; i < dev_num; i++)
        {
            if(dev_attr[i].client->sock == client->sock)
            {   
                LOG_I("device %s disconnect.", dev_attr[i].cfg.name);
                for(int j=i;j<dev_num-1;j++)
                {
                    dev_attr[j] = dev_attr[j+1];
                    memset(&dev_attr[j+1],0,sizeof(device_attribute_t));
                }
                dev_num --;
                if(dev_num < 0 )
                {
                    dev_num = 0;
                }
            }
        }
        break;
    default:
        break;
    }
    rt_kprintf("dev_num:%d\n",dev_num);
    for (unsigned char i = 0; i < dev_num; i++)
    {
        rt_kprintf("dev_attr[%d].client->sock:%d\n",i,dev_attr[i].client->sock);
        rt_kprintf("dev_attr[%d].client->cfg.name:%s\n",i,dev_attr[i].cfg.name);
        rt_kprintf("dev_attr[%d].cfg.now_temperature:%d\n",i,dev_attr[i].cfg.now_temperature);
        rt_kprintf("dev_attr[%d].cfg.now_humidity:%d\n",i,dev_attr[i].cfg.now_humidity);
        rt_kprintf("dev_attr[%d].cfg.switch_status[0]:%d\n",i,dev_attr[i].cfg.switch_status[0]);
        rt_kprintf("dev_attr[%d].cfg.switch_status[1]:%d\n",i,dev_attr[i].cfg.switch_status[1]);
        rt_kprintf("dev_attr[%d].cfg.switch_status[2]:%d\n",i,dev_attr[i].cfg.switch_status[2]);
    }
    
}

int tcpserver(int argc, char **argv)
{
    // if (argc != 3)
    // {
    //     rt_kprintf("Usage:./select_server [ip] [port]\n");
    //     return -1;
    // }
    serv = tcpserver_create("192.168.199.185", 2263);
    LOG_I("create tcp server success.");
    tcpserver_set_notify_callback(serv, tcpserver_event_notify);

    return 0;
}
MSH_CMD_EXPORT(tcpserver, server start)

static int tcpserver_stop(int argc, char **argv)
{
    tcpserver_destroy(serv);
    return 0;
}
MSH_CMD_EXPORT(tcpserver_stop, tcpserver stop)
