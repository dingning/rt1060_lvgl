#define LOG_TAG              "alarm_timer"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"


static void alram_timer_cb(void *parameter)
{
    LOG_D("alram_timer start\n");
    char buff[128] = {0};
    int last_alram_time = 0;
    int now_time = 0;
    while (1)
    {
        /* code */
        for (unsigned char i = 0; i < dev_num; i++)
        {
            if(dev_attr[i].cfg.alarm_switch)
            {   
                if(dev_attr[i].cfg.alarm_temperature <= dev_attr[i].cfg.now_temperature)
                {
                    rt_memset(buff, 0, sizeof(buff));
                    rt_sprintf(buff, "device:%s,now temperature(%d) exceeds the alarm temperature(%d),start alarm!",
                            dev_attr[i].cfg.name, dev_attr[i].cfg.now_temperature, dev_attr[i].cfg.alarm_temperature);
                    LOG_I(buff);
                    now_time = rt_tick_get();
                    if( (now_time - last_alram_time) >= 60 * 1000) 
                    {
                        mqtt_alarm_up(i,0);
                        mail_alarm_up(i,0);
                        //报警弹窗
                        msg_box_alarm_up(buff);
                        last_alram_time =  rt_tick_get();
                    }
                }
                else
                {}
                if(dev_attr[i].cfg.alarm_humidity >= dev_attr[i].cfg.now_humidity)
                {
                    rt_memset(buff, 0, sizeof(buff));
                    rt_sprintf(buff, "device:%s,now humidity() is lower than the alarm humidity(),start alarm!",
                            dev_attr[i].cfg.name, dev_attr[i].cfg.now_humidity, dev_attr[i].cfg.alarm_humidity);
                    LOG_I(buff);
                    now_time = rt_tick_get();
                    if( (now_time - last_alram_time) >= 60 * 1000) 
                    {
                        mqtt_alarm_up(i,1);
                        mail_alarm_up(i,1);
                        //报警弹窗
                        msg_box_alarm_up(buff);
                        last_alram_time =  rt_tick_get();
                    }
                    
                }
                else
                {}
            }
        }

        rt_thread_mdelay(1000);
    }
}

int alram_timer_start(void)
{
    
    rt_thread_t  alram_timer = rt_thread_create("alram_timer", alram_timer_cb, RT_NULL, 1024, 25, 10);

   
    if (alram_timer != RT_NULL) 
    rt_thread_startup(alram_timer);

    return RT_TRUE;
}