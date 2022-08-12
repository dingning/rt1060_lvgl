/*
 * File      : httpclient.c
 *
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date             Author      Notes
 * 2018-07-20     flybreak     first version
 * 2018-09-05     flybreak     Upgrade API to webclient latest version
 */

#include <webclient.h>  /* 使用 HTTP 协议与服务器通信需要包含此头文件 */
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#include <netdb.h>
#include <cJSON.h>
#include <finsh.h>

#include "global.h"
char weather_txt[16]="晴";

#define GET_HEADER_BUFSZ 1024 //头部大小
#define GET_RESP_BUFSZ 1024   //响应缓冲区大小
#define GET_URL_LEN_MAX 256   //网址最大长度
//#define GET_URI                 "http://www.weather.com.cn/data/cityinfo/%s.html" //获取天气的 API
#define AREA_ID "101120101" //济南
#define GET_URI "http://api.k780.com:88/?app=weather.today&weaid=%s&appkey=10003&sign=b59bc3ef6191eb9f747dd4e83c99f2a4&format=json"
/* 天气数据解析 */
void weather_data_parse(rt_uint8_t *data)
{
    cJSON *root = RT_NULL, *object = RT_NULL, *item = RT_NULL;

    root = cJSON_Parse((const char *)data);
    if (!root)
    {
        rt_kprintf("No memory for cJSON root!\n");
        return;
    }
    object = cJSON_GetObjectItem(root, "result");

    item = cJSON_GetObjectItem(object, "cityno");
    rt_kprintf("\ncityName:%s ", item->valuestring);

    item = cJSON_GetObjectItem(object, "citynm");
    rt_kprintf("\ntemp1    :%s ", item->valuestring);

    item = cJSON_GetObjectItem(object, "weather");
    rt_kprintf("\nwd      :%s ", item->valuestring);
    rt_sprintf(weather_txt, "%s", item->valuestring);
    if (root != RT_NULL)
        cJSON_Delete(root);
}
void weather(int argc, char **argv)
{
    rt_uint8_t *buffer = RT_NULL;
    int resp_status;
    struct webclient_session *session = RT_NULL;
    char *weather_url = RT_NULL;
    int content_length = -1, bytes_read = 0;
    int content_pos = 0;

    /* 为 weather_url 分配空间 */
    weather_url = rt_calloc(1, GET_URL_LEN_MAX);
    if (weather_url == RT_NULL)
    {
        rt_kprintf("No memory for weather_url!\n");
        goto __exit;
    }
    /* 拼接 GET 网址 */
    rt_snprintf(weather_url, GET_URL_LEN_MAX, GET_URI, AREA_ID);

    /* 创建会话并且设置响应的大小 */
    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (session == RT_NULL)
    {
        rt_kprintf("No memory for get header!\n");
        goto __exit;
    }

    /* 发送 GET 请求使用默认的头部 */
    if ((resp_status = webclient_get(session, weather_url)) != 200)
    {
        rt_kprintf("webclient GET request failed, response(%d) error.\n", resp_status);
        goto __exit;
    }
    rt_kprintf("get ok\n");

    /* 分配用于存放接收数据的缓冲 */
    buffer = rt_calloc(1, GET_RESP_BUFSZ);
    if (buffer == RT_NULL)
    {
        rt_kprintf("No memory for data receive buffer!\n");
        goto __exit;
    }

    content_length = webclient_content_length_get(session);
    rt_kprintf("content_length:%d\n", content_length);

    if (content_length < 0)
    {
        /* 返回的数据是分块传输的. */
        do
        {
            bytes_read = webclient_read(session, buffer, GET_RESP_BUFSZ);
            if (bytes_read <= 0)
            {
                break;
            }
        } while (1);
    }
    else
    {
        do
        {
            bytes_read = webclient_read(session, buffer,
                                        content_length - content_pos > GET_RESP_BUFSZ ? GET_RESP_BUFSZ : content_length - content_pos);
            if (bytes_read <= 0)
            {
                break;
            }
            content_pos += bytes_read;
        } while (content_pos < content_length);
    }

    /* 天气数据解析 */
    rt_kprintf("weather_data_parse: buffer %s\n", buffer);
    weather_data_parse(buffer);

__exit:
    /* 释放网址空间 */
    if (weather_url != RT_NULL)
        rt_free(weather_url);
    /* 关闭会话 */
    if (session != RT_NULL)
        webclient_close(session);
    /* 释放缓冲区空间 */
    if (buffer != RT_NULL)
        rt_free(buffer);
}

MSH_CMD_EXPORT(weather, Get weather by webclient);
