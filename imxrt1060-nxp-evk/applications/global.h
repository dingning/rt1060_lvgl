#ifndef GLOBAL_H
#define GLOBAL_H

#include <rtthread.h>
#include "cJSON.h"
#include "tcpserver.h"
#include "utl_base64.h"
#include <ulog.h>
#include <dfs_posix.h> /* 当需要使用文件操作时，需要包�?这个头文�? */

#define GW_ID "GW01"

#define MQTT_URI                "tcp://192.168.199.185:1883"
#define MQTT_SUBTOPIC           "/status/"GW_ID"/get"
#define MQTT_PUBTOPIC           "/status/"GW_ID"/put"
#define MQTT_PUBALRAMPIC        "/alarm/"GW_ID

typedef struct device_cfg
{
    char name[8];
    int switch_status[3]; //开关状�?
    int now_temperature;   //当前温度
    int now_humidity;      //当前湿度
    char alarm_switch;     //报�?�开�?
    int alarm_temperature; //报�?�温�?
    int alarm_humidity;    //报�?�湿�?
} device_cfg_t;

typedef struct device_attribute
{
	struct sockaddr_in client_addr;
    tcpclient_t client;
    device_cfg_t cfg;
}device_attribute_t;

typedef struct mail_cfg
{
    char server[24];
    char name[24];
    char passwd[24];
    char from[24];
    char to[24];
    char subject[24];
    char contex[128];
}mail_cfg_t;

typedef struct ftp_cfg
{
    char ip[16];
    int port;
}ftp_cfg_t;


extern device_attribute_t dev_attr[5];
extern int dev_num;
extern mail_cfg_t g_mail_cfg;
extern ftp_cfg_t g_ftp_cfg;
extern char weather_txt[16];

// extern int mqtt_start(int argc, char **argv);
// extern int tcpserver(int argc, char **argv);
extern int send_mail(char *server, char *name, char *passwd, char *from, char *to, char *subject, char *context);
extern void mqtt_alarm_up(int devindex, int type);
extern void mail_alarm_up(int devindex, int type);
extern int alram_timer_start(void);
extern int udp_send_command(int index);
extern int ftp_upload(char *filename);
extern void udptest(int argc, char **argv);
extern int umqtt_ex_start(int argc, char **argv);
extern void weather(int argc, char **argv);
int file_read(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t *read_len, rt_uint32_t *total_len);
#endif