#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "paho_mqtt.h"

#define LOG_TAG              "mqtt"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"
/**
 * MQTT URI farmat:
 * domain mode
 * tcp://iot.eclipse.org:1883
 *
 * ipv4 mode
 * tcp://192.168.10.1:1883
 * ssl://192.168.10.1:1884
 *
 * ipv6 mode
 * tcp://[fe80::20c:29ff:fe9a:a07e]:1883
 * ssl://[fe80::20c:29ff:fe9a:a07e]:1884
 */

#define MQTT_URI                "tcp://192.168.199.185:1883"
#define MQTT_USERNAME           "admin"
#define MQTT_PASSWORD           "admin"

#define MQTT_WILLMSG            "Goodbye!"

/* define MQTT client context */
static MQTTClient client;
static int is_started = 0;
char send_buff[128] = {0};
char *build_status_json_str()
{
    cJSON *root =  cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "devcount", dev_num);
    cJSON *list = cJSON_AddArrayToObject(root, "list");
    for (char i = 0; i < dev_num; i++)
    {
		cJSON *item = cJSON_CreateObject();
        cJSON_AddItemToArray(list, item);
        cJSON_AddStringToObject(item, "name", dev_attr[i].cfg.name);
        cJSON_AddNumberToObject(item, "now_temp", dev_attr[i].cfg.now_temperature);
        cJSON_AddNumberToObject(item, "now_humi", dev_attr[i].cfg.now_humidity);
        cJSON_AddItemToObject(item,"switch", cJSON_CreateIntArray(dev_attr[i].cfg.switch_status, 3));
        cJSON_AddBoolToObject(item,"alarm_enable",dev_attr[i].cfg.alarm_switch);
        cJSON_AddNumberToObject(item, "alarm_temp", dev_attr[i].cfg.alarm_temperature);
        cJSON_AddNumberToObject(item, "alarm_humi", dev_attr[i].cfg.alarm_humidity);
    }
    char *buff = cJSON_Print(root);
    //printf("buff:%s\n",buff);

    cJSON_Delete(root);
    return buff;
}
void mqtt_alarm_up(int devindex, int type)
{   
    cJSON *root =  cJSON_CreateObject();
    switch (type)
    {
    case 0:
        cJSON_AddStringToObject(root, "type", "temp");
        cJSON_AddNumberToObject(root, "alarm_temp", dev_attr[devindex].cfg.alarm_temperature);
        cJSON_AddNumberToObject(root, "now_temp", dev_attr[devindex].cfg.now_temperature);
        break;
    case 1:
        cJSON_AddStringToObject(root, "type", "humi");
        cJSON_AddNumberToObject(root, "alarm_humi", dev_attr[devindex].cfg.alarm_humidity);
        cJSON_AddNumberToObject(root, "now_humi", dev_attr[devindex].cfg.now_humidity);
        break;
    default:
        break;
    }
    char *buff = cJSON_Print(root);
    //printf("buff:%s\n",buff);
    cJSON_Delete(root);
    memset(send_buff,0,sizeof(send_buff));
    memcpy(send_buff,buff,strlen(buff));
    //printf("send_buff:%s\n",send_buff);

    char alarm_topic[16] = {0};
    rt_snprintf(alarm_topic,16,"%s/%s",MQTT_PUBALRAMPIC, dev_attr[devindex].cfg.name);
    LOG_I("mqtt alarm pub %s:\n%s",alarm_topic,send_buff);
    paho_mqtt_publish(&client, QOS1, alarm_topic, send_buff);
}
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_I("mqtt sub callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);

    char *buff = build_status_json_str();
    memset(send_buff,0,sizeof(send_buff));
    memcpy(send_buff,buff,strlen(buff));
   // printf("send_buff:%s\n",send_buff);
    free(buff);
    LOG_I("mqtt pub %s:\n%s",MQTT_PUBTOPIC,send_buff);
    paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, send_buff);
    
}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt sub default callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

int mqtt_start(int argc, char **argv)
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };

    if (argc != 1)
    {
        rt_kprintf("mqtt_start    --start a mqtt worker thread.\n");
        return -1;
    }

    if (is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;

        /* generate the random client ID */
        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
        /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));
        client.condata.clientID.cstring = cid;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 1024;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;

    return 0;
}

static int mqtt_stop(int argc, char **argv)
{
    if (argc != 1)
    {
        rt_kprintf("mqtt_stop    --stop mqtt worker thread and free mqtt client object.\n");
    }

    is_started = 0;

    return paho_mqtt_stop(&client);
}

static int mqtt_publish(int argc, char **argv)
{
    if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    if (argc == 2)
    {
        paho_mqtt_publish(&client, QOS1, MQTT_PUBTOPIC, argv[1]);
    }
    else if (argc == 3)
    {
        paho_mqtt_publish(&client, QOS1, argv[1], argv[2]);
    }
    else
    {
        rt_kprintf("mqtt_publish <topic> [message]  --mqtt publish message to specified topic.\n");
        return -1;
    }

    return 0;
}

static void mqtt_new_sub_callback(MQTTClient *client, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    LOG_D("mqtt new subscribe callback: %.*s %.*s",
               msg_data->topicName->lenstring.len,
               msg_data->topicName->lenstring.data,
               msg_data->message->payloadlen,
               (char *)msg_data->message->payload);
}

static int mqtt_subscribe(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("mqtt_subscribe [topic]  --send an mqtt subscribe packet and wait for suback before returning.\n");
        return -1;
    }
	
	if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_subscribe(&client, QOS1, argv[1], mqtt_new_sub_callback);
}

static int mqtt_unsubscribe(int argc, char **argv)
{
    if (argc != 2)
    {
        rt_kprintf("mqtt_unsubscribe [topic]  --send an mqtt unsubscribe packet and wait for suback before returning.\n");
        return -1;
    }
	
	if (is_started == 0)
    {
        LOG_E("mqtt client is not connected.");
        return -1;
    }

    return paho_mqtt_unsubscribe(&client, argv[1]);
}

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(mqtt_start, startup mqtt client);
MSH_CMD_EXPORT(mqtt_stop, stop mqtt client);
MSH_CMD_EXPORT(mqtt_publish, mqtt publish message to specified topic);
MSH_CMD_EXPORT(mqtt_subscribe,  mqtt subscribe topic);
MSH_CMD_EXPORT(mqtt_unsubscribe, mqtt unsubscribe topic);
#endif /* FINSH_USING_MSH */

