#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/

#include "lvgl.h"
#include <stdio.h>
#include <time.h>

#define LOG_TAG              "lvgl"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"

LV_FONT_DECLARE(lv_font_siyuanheiti_20)
void timer_cb(struct _lv_timer_t *timer);
void app_close_cb(lv_event_t * e);


char device_selected_id = 0;


// device_cfg_t dev_attr.cfg[] =
//     {
//         {.name = "A1",
//         .now_temperature = 26,
//         .now_humidity = 48,
//         .switch_status = {1,0,1},
//          .alarm_switch = 1,
//          .alarm_temperature = 30,
//          .alarm_humidity = 20},
//         {.name = "A2",
//         .now_temperature = 27,
//         .now_humidity = 49,
//         .switch_status = {0,0,1},
//             .alarm_switch = 0,
//          .alarm_temperature = 32,
//          .alarm_humidity = 25}};

lv_obj_t *sn_screen = NULL;
lv_obj_t *lock_gui = NULL;
lv_obj_t *desk_gui = NULL;
lv_obj_t *device_status_gui = NULL;

/*报警弹窗*/
void msg_box_alarm_up(char *buff)
{
    lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Alarm", buff, NULL, true);
    lv_obj_center(mbox1);
    lv_obj_set_width(mbox1,400);
}

/*邮箱ftp界面*/
#if 1
static lv_obj_t *list1;
lv_obj_t *port_ta;
lv_obj_t *addr_ta;
lv_obj_t *mail_dd;
lv_obj_t *mail_svr_ta;
lv_obj_t *mail_name_ta;
lv_obj_t *mail_passwd_ta;
lv_obj_t *mail_recvier_ta;
static const char * opts = "smtp.163.com\n""smtp.qq.com\n""smtp.gmail.com\n";
static void ta_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);
    if (code == LV_EVENT_FOCUSED)
    {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if (code == LV_EVENT_DEFOCUSED)
    {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}
static void up_btn_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        int i = lv_event_get_user_data(e);
        printf("i:%d\n",i);
        char *txt;
        char buff[20];
        int index;
        switch (i)
        {
        case 1:
            lv_dropdown_get_selected_str(mail_dd,buff,20);
            printf("buff:%s\n", buff);
            // index = lv_dropdown_get_selected(mail_dd);
            // printf("index:%d\n", index);
            // printf("opts:%s\n", opts[index]);
            txt = lv_textarea_get_text(mail_svr_ta);
            printf("txt:%s\n", txt);
            if(strlen(txt) > 0)
            {
                rt_sprintf(g_mail_cfg.server,"%s",txt);
            }
            break;
        case 2:
            txt = lv_textarea_get_text(mail_name_ta);
            printf("txt:%s\n", txt);
            if(strlen(txt) > 0)
            {
                rt_sprintf(g_mail_cfg.name,"%s",txt);
            }
            txt = lv_textarea_get_text(mail_passwd_ta);
            printf("txt:%s\n", txt);
            if(strlen(txt) > 0)
            {
                rt_sprintf(g_mail_cfg.passwd,"%s",txt);
            }
            break;
        case 3:
            txt = lv_textarea_get_text(mail_recvier_ta);
            printf("txt:%s\n", txt);
            if(strlen(txt) > 0)
            {
                rt_sprintf(g_mail_cfg.to,"%s",txt);
            }
        case 4:
            txt = lv_textarea_get_text(addr_ta);
            printf("txt:%s", txt);
            if(strlen(txt) > 0)
            {
                rt_sprintf(g_ftp_cfg.ip,"%s",txt);
            }
            txt = lv_textarea_get_text(port_ta);
            printf("txt:%s", txt);
            if(strlen(txt) > 0)
            {
                g_ftp_cfg.port = atoi(txt);
            }
            LOG_D("FTP Server: %s:%d", g_ftp_cfg.ip,g_ftp_cfg.port);
            ftp_upload("uf.log");
            break;
        default:
            break;
        }
        LV_LOG_USER("Clicked");

    }
}
void create_net_up()
{
    sn_screen = lv_scr_act();
    lv_obj_t *title = lv_obj_create(sn_screen);
    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_radius(&style1, 0);
    lv_style_set_bg_color(&style1, lv_color_make(242, 242, 242));
    // lv_style_set_border_color(&style1, lv_color_make(44, 44, 44));
    lv_style_set_border_width(&style1, 0);
    lv_style_set_outline_width(&style1, 0);
    lv_obj_add_style(title, &style1, 0);
    lv_obj_set_size(title, 480, 36);
    lv_obj_set_scrollbar_mode(title, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *label_tmp = lv_label_create(title);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_obj_set_style_text_color(label_tmp, lv_color_make(24, 24, 24), 0);
    lv_label_set_text(label_tmp, "远程同步"); //远程同步
    lv_obj_align(label_tmp, LV_ALIGN_TOP_MID, -4, -8);
    lv_obj_t *img = lv_img_create(title);
    lv_img_set_src(img, LV_SYMBOL_CLOSE);
    lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, 0, 5);
    lv_obj_add_flag(img,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, app_close_cb, LV_EVENT_CLICKED, 2);

    lv_obj_t *tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_LEFT, 80);
    lv_obj_set_height(tabview, 272 - 36 - 5);
    lv_obj_align_to(tabview, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "mail");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "ftp");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "log");
    lv_obj_set_scrollbar_mode(tab1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(tab2, LV_SCROLLBAR_MODE_OFF);


    lv_obj_t *tabview2;
    tabview2 = lv_tabview_create(tab1, LV_DIR_TOP, 32);
    lv_obj_set_size(tabview2, 380, 240);
    lv_obj_align_to(tabview2, title, LV_ALIGN_OUT_BOTTOM_LEFT, 85, 5);
    //lv_obj_align_to(tabview, list1, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab21 = lv_tabview_add_tab(tabview2, "server");
    lv_obj_t *tab22 = lv_tabview_add_tab(tabview2, "user");
    lv_obj_t *tab23 = lv_tabview_add_tab(tabview2, "receiver");

    lv_obj_set_scrollbar_mode(tab21, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(tab22, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scrollbar_mode(tab23, LV_SCROLLBAR_MODE_OFF);

    label_tmp = lv_label_create(tab21);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "服务器");
    lv_obj_set_pos(label_tmp,0,0);
    

    mail_dd = lv_dropdown_create(tab21);
    lv_obj_set_width(mail_dd,140);
    lv_dropdown_set_options_static(mail_dd, opts);
    //lv_obj_set_pos(dd,60,0);
    lv_obj_align_to(mail_dd,label_tmp,LV_ALIGN_OUT_RIGHT_MID,5,0);
    label_tmp = lv_label_create(tab21);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "端口");
    lv_obj_set_pos(label_tmp,0,45);
    lv_obj_t *kb = lv_keyboard_create(tab21);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    /*Create a text area. The keyboard will write here*/
    mail_svr_ta = lv_textarea_create(tab21);
    lv_textarea_set_one_line(mail_svr_ta, true);
    lv_obj_set_width(mail_svr_ta, lv_pct(30));
    lv_obj_align_to(mail_svr_ta,mail_dd,LV_ALIGN_OUT_BOTTOM_LEFT,0,5);
    lv_obj_add_event_cb(mail_svr_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(mail_svr_ta, "25");

    lv_obj_t *up_btn = lv_btn_create(tab21);
    lv_obj_add_event_cb(up_btn, up_btn_cb, LV_EVENT_ALL, 1);
    lv_obj_align(up_btn, LV_ALIGN_RIGHT_MID, -10, -25);
    label_tmp = lv_label_create(up_btn);
    lv_label_set_text(label_tmp, "Upload");
    lv_obj_center(label_tmp);

/**/
    kb = lv_keyboard_create(tab22);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    label_tmp = lv_label_create(tab22);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "用户名");
    lv_obj_set_pos(label_tmp,0,0);
    mail_name_ta = lv_textarea_create(tab22);
    lv_textarea_set_one_line(mail_name_ta, true);
    lv_obj_set_width(mail_name_ta, lv_pct(55));
    lv_obj_align_to(mail_name_ta,label_tmp,LV_ALIGN_OUT_RIGHT_MID,5,0);
    lv_obj_add_event_cb(mail_name_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(mail_name_ta, g_mail_cfg.name);
    label_tmp = lv_label_create(tab22);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "密码");
    lv_obj_set_pos(label_tmp,0,45);
    mail_passwd_ta = lv_textarea_create(tab22);
    lv_textarea_set_one_line(mail_passwd_ta, true);
    lv_obj_set_width(mail_passwd_ta, lv_pct(55));
    lv_obj_align_to(mail_passwd_ta,mail_name_ta,LV_ALIGN_OUT_BOTTOM_LEFT,0,5);
    lv_obj_add_event_cb(mail_passwd_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(mail_passwd_ta, g_mail_cfg.passwd);
    up_btn = lv_btn_create(tab22);
    lv_obj_add_event_cb(up_btn, up_btn_cb, LV_EVENT_ALL, 2);
    lv_obj_align(up_btn, LV_ALIGN_RIGHT_MID, -5, -25);
    label_tmp = lv_label_create(up_btn);
    lv_label_set_text(label_tmp, "Upload");
    lv_obj_center(label_tmp);

/**/
    kb = lv_keyboard_create(tab23);
    label_tmp = lv_label_create(tab23);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "收件人设置");
    lv_obj_set_pos(label_tmp,0,0);
    mail_recvier_ta = lv_textarea_create(tab23);
    lv_textarea_set_one_line(mail_recvier_ta, true);
    lv_obj_set_width(mail_recvier_ta, lv_pct(55));
    lv_obj_align_to(mail_recvier_ta,label_tmp,LV_ALIGN_OUT_RIGHT_MID,5,0);
    lv_obj_add_event_cb(mail_recvier_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(mail_recvier_ta, g_mail_cfg.to);
    up_btn = lv_btn_create(tab23);
    lv_obj_add_event_cb(up_btn, up_btn_cb, LV_EVENT_ALL, 3);
    lv_obj_align(up_btn, LV_ALIGN_RIGHT_MID, -5, -25);
    label_tmp = lv_label_create(up_btn);
    lv_label_set_text(label_tmp, "Upload");
    lv_obj_center(label_tmp);
/**/

    kb = lv_keyboard_create(tab2);
    /*Create a text area. The keyboard will write here*/
    addr_ta = lv_textarea_create(tab2);
    lv_textarea_set_one_line(addr_ta, true);
    lv_obj_set_width(addr_ta, lv_pct(40));
    lv_obj_set_pos(addr_ta, 5, 20);
    lv_obj_add_event_cb(addr_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(addr_ta, g_ftp_cfg.ip);
    lv_obj_t *addr_label = lv_label_create(tab2);
    lv_label_set_text(addr_label, "Address:");
    lv_obj_align_to(addr_label, addr_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    port_ta = lv_textarea_create(tab2);
    lv_textarea_set_one_line(port_ta, true);
    lv_textarea_set_password_mode(port_ta, false);
    lv_obj_set_width(port_ta, lv_pct(40));
    lv_obj_add_event_cb(port_ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_align(port_ta, LV_ALIGN_TOP_RIGHT, -5, 20);
    char buf[20];
    sprintf(buf, "%d", g_ftp_cfg.port);
    lv_textarea_set_placeholder_text(port_ta, buf);
    lv_obj_t *port_label = lv_label_create(tab2);
    lv_label_set_text(port_label, "Port:");
    lv_obj_align_to(port_label, port_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    up_btn = lv_btn_create(tab2);
    lv_obj_add_event_cb(up_btn, up_btn_cb, LV_EVENT_ALL, 4);
    lv_obj_align_to(up_btn, port_ta, LV_ALIGN_OUT_BOTTOM_MID, 5, 5);
    label_tmp = lv_label_create(up_btn);
    lv_label_set_text(label_tmp, "Upload");
    lv_obj_center(label_tmp);

    label_tmp = lv_label_create(tab3);
    char txt_buf[1024*10];
    int read_len;
    int total_len;
    file_read(RT_NULL,"uf.log",txt_buf,10*1024,0,&read_len,&total_len);
    lv_label_set_text(label_tmp, txt_buf);
}
#endif

/*报警界面*/
#if 1

lv_obj_t *slider_label;
lv_obj_t *alram_cb;
lv_obj_t *temp_dd;
lv_obj_t *humi_slider;
lv_obj_t *alarm_label_title;
void para_update()
{
    char id = device_selected_id;
    if (dev_attr[id].cfg.alarm_switch)
    {
        lv_obj_add_state(alram_cb, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_clear_state(alram_cb, LV_STATE_CHECKED);
    }
    lv_dropdown_set_selected(temp_dd, dev_attr[id].cfg.alarm_temperature - 25);
    lv_slider_set_value(humi_slider, dev_attr[id].cfg.alarm_humidity, LV_ANIM_ON);
    char buf[16];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(humi_slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, humi_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_snprintf(buf, sizeof(buf), "%s", dev_attr[id].cfg.name);
    lv_checkbox_set_text(alram_cb, buf);

    if(alarm_label_title)
    {
        char buff[24];
        sprintf(buff,"报警配置(%s)", dev_attr[device_selected_id].cfg.name);
        lv_label_set_text(alarm_label_title,buff);
    }
}
static void temp_dd_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
      
        int index = lv_dropdown_get_selected(obj);
        LV_LOG_USER("index: %d", index);
        dev_attr[device_selected_id].cfg.alarm_temperature = index +25;
    }
}
static void humi_slider_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    dev_attr[device_selected_id].cfg.alarm_humidity = (int)lv_slider_get_value(slider);
}

static void ck_alram_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";
        LV_LOG_USER("%s: %s", txt, state);
        dev_attr[device_selected_id].cfg.alarm_switch = lv_obj_get_state(obj);   
    }
}

static void dev_btn_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        // LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
        int id = lv_event_get_user_data(e);
        LV_LOG_USER("id: %d", id);
        device_selected_id = id;
        para_update();

        lv_obj_t *parent = lv_obj_get_parent(obj);
        for (char i = 0; i < lv_obj_get_child_cnt(parent); i++)
        {
            lv_obj_t *child = lv_obj_get_child(parent, i);
            if (child == obj)
            {
                LV_LOG_USER("id107: %d", id);
                lv_obj_add_state(child, LV_STATE_CHECKED);
            }
            else
            {
                LV_LOG_USER("id112: %d", id);
                lv_obj_clear_state(child, LV_STATE_CHECKED);
            }
        }
    }
}
void create_alarm()
{
    sn_screen = lv_scr_act();
    lv_obj_t *title = lv_obj_create(sn_screen);
    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_radius(&style1, 0);
    lv_style_set_bg_color(&style1, lv_color_make(242, 242, 242));
    // lv_style_set_border_color(&style1, lv_color_make(44, 44, 44));
    lv_style_set_border_width(&style1, 0);
    lv_style_set_outline_width(&style1, 0);
    lv_obj_add_style(title, &style1, 0);
    lv_obj_set_size(title, 480, 36);
    lv_obj_set_scrollbar_mode(title, LV_SCROLLBAR_MODE_OFF);

    alarm_label_title = lv_label_create(title);
    lv_obj_set_style_text_font(alarm_label_title, &lv_font_siyuanheiti_20, 0);
    lv_obj_set_style_text_color(alarm_label_title, lv_color_make(24, 24, 24), 0);
    lv_label_set_text(alarm_label_title, "报警配置"); //报警配置
    lv_obj_align(alarm_label_title, LV_ALIGN_TOP_MID, -4, -8);
    lv_obj_t *img = lv_img_create(title);
    lv_img_set_src(img, LV_SYMBOL_CLOSE);
    lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, 0, 5);
    lv_obj_add_flag(img,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, app_close_cb, LV_EVENT_CLICKED, 1);

    lv_obj_t *dev_list = lv_list_create(sn_screen);
    lv_obj_set_size(dev_list, 120, 220);
    lv_obj_align_to(dev_list, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
    lv_obj_t *btn;
    lv_obj_t *l_text = lv_list_add_text(dev_list, "device list");
    lv_obj_set_height(l_text, 24);
  
    for (char i = 0; i < dev_num; i++)
    {
        char buff[16];
        sprintf(buff, "%s", dev_attr[i].cfg.name);
        btn = lv_list_add_btn(dev_list, NULL, buff);
        lv_obj_add_event_cb(btn, dev_btn_cb, LV_EVENT_CLICKED, i);
    }

    alram_cb = lv_checkbox_create(sn_screen);
    lv_checkbox_set_text(alram_cb, "ON/OFF");
    lv_obj_add_event_cb(alram_cb, ck_alram_cb, LV_EVENT_ALL, NULL);
    lv_obj_align_to(alram_cb, dev_list, LV_ALIGN_OUT_RIGHT_TOP, 20, 5);
    lv_obj_t *label_tmp = lv_label_create(sn_screen);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "报警开关"); //报警温度设置
    lv_obj_align_to(label_tmp, alram_cb, LV_ALIGN_OUT_RIGHT_MID, -40, 0);

    label_tmp = lv_label_create(sn_screen);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "报警温度设置:"); //报警温度设置
    lv_obj_align_to(label_tmp, alram_cb, LV_ALIGN_OUT_BOTTOM_MID, 20, 20);

    temp_dd = lv_dropdown_create(sn_screen);
    lv_dropdown_set_options(temp_dd, "25\n"
                                     "26\n"
                                     "27\n"
                                     "28\n"
                                     "29\n"
                                     "30\n"
                                     "31\n"
                                     "32\n"
                                     "33\n"
                                     "34\n"
                                     "35\n"
                                     "36\n"
                                     "37\n"
                                     "38\n"
                                     "39\n"
                                     "40");

    lv_obj_align_to(temp_dd, label_tmp, LV_ALIGN_OUT_RIGHT_MID, 40, 0);
    lv_obj_add_event_cb(temp_dd, temp_dd_cb, LV_EVENT_ALL, NULL);
    
    label_tmp = lv_label_create(sn_screen);
    lv_obj_set_style_text_font(label_tmp, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(label_tmp, "报警湿度设置:"); //报警湿度设置
    lv_obj_align_to(label_tmp, alram_cb, LV_ALIGN_OUT_BOTTOM_MID, 20, 70);
    
    humi_slider = lv_slider_create(sn_screen);
    lv_obj_add_event_cb(humi_slider, humi_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_align_to(humi_slider, label_tmp, LV_ALIGN_OUT_BOTTOM_RIGHT, 180, 30);
    // /*Create a label below the slider*/
    slider_label = lv_label_create(sn_screen);
    lv_label_set_text(slider_label, "0%");

    lv_obj_align_to(slider_label, humi_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    para_update();
}
#endif

/*设备状态*/
#if 1
lv_obj_t *dev_temp_meter = NULL;
lv_meter_indicator_t *dev_tmp_indic=NULL;
lv_obj_t * dev_temp_label;
lv_obj_t *dev_humi_bar=NULL;
lv_obj_t *dev_sw[3] = {NULL};
lv_obj_t *dev_label_title  = NULL;
void dev_status_update()
{
    if(dev_label_title)
    {
        char buff[24];
        sprintf(buff,"设备状态(%s)", dev_attr[device_selected_id].cfg.name);
        lv_label_set_text(dev_label_title,buff);
    }

    if(dev_temp_meter)
    {
        lv_meter_set_indicator_value(dev_temp_meter, dev_tmp_indic, dev_attr[device_selected_id].cfg.now_temperature);
        char buff[12];
        sprintf(buff,"%d℃", dev_attr[device_selected_id].cfg.now_temperature);
        lv_label_set_text(dev_temp_label,buff);
    }

    if(dev_humi_bar)
    {
        lv_bar_set_value(dev_humi_bar, dev_attr[device_selected_id].cfg.now_humidity, LV_ANIM_OFF);
    }
    if(dev_sw[0])
    {
        for (char i = 0; i < 3; i++)
        {
            if(dev_attr[device_selected_id].cfg.switch_status[i])
            {
                lv_obj_add_state(dev_sw[i], LV_STATE_CHECKED);
            }
            else
            {
                lv_obj_clear_state(dev_sw[i], LV_STATE_CHECKED);
            }
        }
    }
}
static void list_btn_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
        int a = lv_event_get_user_data(e);
        printf("list_btn_cb: %d", a);
        device_selected_id = a;
        dev_status_update();
    }
}
static void set_humi_cb(lv_event_t *e)
{
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
    if (dsc->part != LV_PART_INDICATOR)
        return;

    lv_obj_t *obj = lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if (lv_area_get_width(dsc->draw_area) > txt_size.x + 20)
    {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else
    {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    //lv_draw_label(&txt_area, dsc->clip_area, &label_dsc, buf, NULL);
    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}
static void sw_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
        int a = lv_event_get_user_data(e);
        LV_LOG_USER("a: %d\n", a);
        dev_attr[device_selected_id].cfg.switch_status[a] = lv_obj_has_state(obj, LV_STATE_CHECKED);
        //控制灯光开关 发送给节点 todo
        udp_send_command(device_selected_id);
    }
}
void create_device_status()
{
    sn_screen = lv_scr_act();
    lv_obj_t *device_status_gui = lv_obj_create(sn_screen);
    lv_obj_set_scrollbar_mode(sn_screen, LV_SCROLLBAR_MODE_OFF);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 0);
    lv_obj_add_style(device_status_gui, &style, 0);
    lv_obj_set_size(device_status_gui, 480, 272);

    lv_obj_t *title = lv_obj_create(sn_screen);

    static lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_radius(&style1, 0);
    lv_style_set_bg_color(&style1, lv_color_make(242, 242, 242));
    lv_style_set_border_width(&style1, 0);
    lv_style_set_outline_width(&style1, 0);
    lv_obj_add_style(title, &style1, 0);
    lv_obj_set_size(title, 480, 36);
    lv_obj_set_scrollbar_mode(title, LV_SCROLLBAR_MODE_OFF);

    dev_label_title = lv_label_create(title);
    lv_obj_set_style_text_font(dev_label_title, &lv_font_siyuanheiti_20, 0);
    lv_obj_set_style_text_color(dev_label_title, lv_color_make(24, 24, 24), 0);
    lv_label_set_text(dev_label_title, "设备状态"); //设备状态
    lv_obj_align(dev_label_title, LV_ALIGN_TOP_MID, -4, -8);

    lv_obj_t *img = lv_img_create(title);
    lv_img_set_src(img, LV_SYMBOL_CLOSE);
    lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, 0, 5);
    lv_obj_add_flag(img,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, app_close_cb, LV_EVENT_CLICKED, 0);

    list1 = lv_list_create(sn_screen);
    lv_obj_set_size(list1, 120, 220);
    lv_obj_align_to(list1, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);

    lv_obj_t *btn;
    lv_obj_t *l_text = lv_list_add_text(list1, "device list");
     lv_obj_set_height(l_text, 24);
    for (char i = 0; i < dev_num; i++)
    {
        btn = lv_list_add_btn(list1, NULL, dev_attr[i].cfg.name);
        lv_obj_add_event_cb(btn, list_btn_cb, LV_EVENT_CLICKED, i);
    }

    lv_obj_t *tabview;
    tabview = lv_tabview_create(sn_screen, LV_DIR_TOP, 32);
    lv_obj_set_size(tabview, 380, 220);
    lv_obj_align_to(tabview, list1, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "temperature");
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "humidity");
    lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "switch");


    dev_temp_meter = lv_meter_create(tab1);
    lv_obj_t *meter = dev_temp_meter;
    lv_obj_center(meter);
    lv_obj_set_size(meter, 180, 180);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 10, 4, 15, lv_color_black(), 10);
    lv_meter_set_scale_range(meter, scale, -20, 60, 270, 135);

    lv_meter_indicator_t *indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter, indic, -20);
    lv_meter_set_indicator_end_value(meter, indic, 0);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, -20);
    lv_meter_set_indicator_end_value(meter, indic, 0);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter, indic, 40);
    lv_meter_set_indicator_end_value(meter, indic, 80);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 40);
    lv_meter_set_indicator_end_value(meter, indic, 80);

    /*Add a needle line indicator*/

    dev_tmp_indic = lv_meter_add_needle_line(meter, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    lv_meter_set_indicator_value(meter, dev_tmp_indic, 26);


    dev_temp_label = lv_label_create(tab1);
    lv_obj_set_style_text_font(dev_temp_label, &lv_font_siyuanheiti_20, 0);
    lv_label_set_text(dev_temp_label, "36℃");


    dev_humi_bar = lv_bar_create(tab2);
    lv_obj_t *bar = dev_humi_bar;
    lv_obj_add_event_cb(bar, set_humi_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_center(bar);
    lv_bar_set_value(bar, 20, LV_ANIM_OFF);

    lv_obj_t *label;

    uint32_t i;
    for (i = 0; i < 3; i++)
    {
        label = lv_label_create(tab3);
        lv_label_set_text_fmt(label, "switch%d", i);
        lv_obj_align(label, LV_ALIGN_TOP_MID, -40, i * 50 + 20);

        dev_sw[i] = lv_switch_create(tab3);
        lv_obj_add_event_cb( dev_sw[i], sw_cb, LV_EVENT_ALL, i);
        lv_obj_align( dev_sw[i], LV_ALIGN_TOP_MID, 40, i * 45 + 15);
    }

}
#endif

/*主界面*/
#if 1
static void my_event_cb_1(lv_event_t *event);
/* 桌标图标声明 */
LV_IMG_DECLARE(image_ray);
LV_IMG_DECLARE(image_device_info);           // 00 LV_100ASK_MENU_ITEM_COUNT
LV_IMG_DECLARE(image_alram_set);          // 01
LV_IMG_DECLARE(image_remote_up);            // 02

/* 桌标图标数组 */
static const lv_img_dsc_t *app_icon[] =
{

        &image_device_info,
        &image_alram_set,
        &image_remote_up,
};

lv_obj_t *label_time;
lv_obj_t *weather_label;
/* 回调函数指针数组*/
static void (*lv_sn_event_cb[])(lv_obj_t *obj, lv_event_t e) =
{

};

static void app_click_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED)
    {
        char app_id = lv_event_get_user_data(e);
        printf("clicked:%d\n",app_id);
         lv_obj_del(desk_gui);
        lv_obj_t *child = lv_obj_get_child(sn_screen, NULL);
        while (child)
        {
            if (child != sn_screen)
            {
                printf("lv_obj_del\n");
                lv_obj_del(child);
            }
            child++;
            child = lv_obj_get_child(sn_screen, child);
        }
        label_time = NULL;
        weather_label = NULL;
        switch (app_id)
        {
        case 0:
            create_device_status();
            break;
        case 1:
            create_alarm();
            break;
        case 2:
            create_net_up();
            break;
        default:
            break;
        }
    }
}

static const char *app_name[] =
{
    "设备状态", "报警配置", "远程同步",
};

void create_desk()
{
    sn_screen = lv_scr_act();
    desk_gui = lv_obj_create(sn_screen);
    // lv_obj_set_style_local_bg_color(sn_screen, 1, LV_PART_INDICATOR | LV_STATE_FOCUSED);
    lv_obj_set_size(desk_gui, 480, 272);
    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    lv_obj_add_style(desk_gui, &style_bg, 0);

    lv_obj_t *img1 = lv_img_create(desk_gui);

    lv_img_set_src(img1, &image_ray);
    lv_obj_align(img1, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_size(img1, 120, 186);

    lv_obj_t *label_app_name; // app图标下方的文字
    lv_obj_t *obj_menu_item;  // 用于遍历初始化app图标的指针
    for (int i = 0; i < 3; i++)
    {
        obj_menu_item = lv_img_create(desk_gui);
        label_app_name = lv_label_create(desk_gui);
        //lv_obj_set_click(obj_menu_item, 1);            // 允许点击
        lv_obj_add_flag(obj_menu_item,LV_OBJ_FLAG_CLICKABLE);
        lv_img_set_src(obj_menu_item, app_icon[i]); // 使用数组文件

        lv_obj_align(obj_menu_item, LV_ALIGN_TOP_LEFT, 150 + i * 100, 130);
        lv_obj_fade_in(obj_menu_item, 0, 100);
        //lv_obj_set_event_cb(obj_menu_item, lv_sn_event_cb[i]);    /* 分配事件回调处理函数 */
        lv_obj_add_event_cb(obj_menu_item, app_click_cb, LV_EVENT_ALL, i);

        /* 创建 app name 标签 */
        // lv_obj_set_style_local_text_font(label_app_name,LV_STATE_DEFAULT, &lv_font_montserrat_10);  	// 设置字号
        lv_obj_set_style_text_font(label_app_name, &lv_font_siyuanheiti_20, 0);
        //lv_obj_set_style_text_color(label_app_name, lv_color_hex(0xB8BCC6), 0);
        lv_label_set_text(label_app_name, app_name[i]);
        lv_obj_center(label_app_name);
        lv_obj_align_to(label_app_name, obj_menu_item, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    }

    lv_obj_t *status_obj = lv_obj_create(desk_gui);
    lv_obj_set_size(status_obj, 300, 90);
    lv_obj_set_style_bg_color(status_obj, lv_color_make(242, 242, 242), 0);
    lv_obj_align(status_obj, LV_ALIGN_TOP_LEFT, 140, 20);
    lv_obj_set_scrollbar_mode(status_obj, LV_SCROLLBAR_MODE_OFF);
    /*Create an array for the points of the line*/
    static lv_point_t line_points[] = {{1, 2}, {1, 12}};

    /*Create style*/
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 4);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line, true);

    /*Create a line and apply the new style*/
    lv_obj_t *line1;
    line1 = lv_line_create(status_obj);
    lv_line_set_points(line1, line_points, 2); /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);

    lv_obj_t *label_key1 = lv_label_create(status_obj);
    lv_obj_set_style_text_font(label_key1, &lv_font_siyuanheiti_20, 0);
    //lv_obj_set_style_text_color(label_key1, lv_color_hex(0xB8BCC6), 0);
    lv_label_set_text(label_key1, "天气:");
    lv_obj_align_to(label_key1, line1, LV_ALIGN_OUT_RIGHT_MID, 4, -2);

    weather_label= lv_label_create(status_obj);
    lv_obj_set_style_text_font(weather_label, &lv_font_siyuanheiti_20, 0);
    //lv_obj_set_style_text_color(label_value1, lv_color_hex(0xB8BCC6), 0);
    lv_label_set_text(weather_label, "晴");
    lv_obj_align_to(weather_label, label_key1, LV_ALIGN_OUT_RIGHT_MID, 30, 0);

    static lv_style_t style_line2;
    lv_style_init(&style_line2);
    lv_style_set_line_width(&style_line2, 4);
    lv_style_set_line_color(&style_line2, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_line_rounded(&style_line2, true);
    static lv_point_t line_points2[] = {{1, 30}, {1, 40}};
    lv_obj_t *line2 = lv_line_create(status_obj);
    lv_line_set_points(line2, line_points2, 2); /*Set the points*/
    lv_obj_add_style(line2, &style_line2, 0);

    lv_obj_t *label_key2 = lv_label_create(status_obj);
    lv_obj_set_style_text_font(label_key2, &lv_font_siyuanheiti_20, 0);
    //lv_obj_set_style_text_color(label_key2, lv_color_hex(0xB8BCC6), 0);
    lv_label_set_text(label_key2, "时间:");
    lv_obj_align_to(label_key2, line2, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 2);


    label_time = lv_label_create(status_obj);
    lv_obj_set_style_text_font(label_time, &lv_font_siyuanheiti_20, 0);
   //lv_obj_set_style_text_color(label_value2, lv_color_hex(0xB8BCC6), 0);
    lv_label_set_text(label_time, "2022/07/20 16:23:25");
    lv_obj_align_to(label_time, label_key2, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
}
#endif
/*锁屏界面*/
#if 1
lv_obj_t *systime_task_label = NULL;
lv_obj_t *sysdate_task_label = NULL;
lv_obj_t *meter = NULL;
lv_meter_indicator_t *indic_min = NULL;
lv_meter_indicator_t *indic_hour = NULL;
static void anim_read_cb(void *var, int32_t v)
{
    printf("anim_read_cb:%d\n", 1);

    lv_obj_t *child = lv_obj_get_child(sn_screen, NULL);
    while (child)
    {
        if (child != sn_screen)
        {
            lv_obj_del(child);
        }
        child = lv_obj_get_child(sn_screen, child);
    }
    systime_task_label = NULL;
    sysdate_task_label = NULL;
    meter = NULL;
    indic_min = NULL;
    indic_hour = NULL;
    create_desk();
}
static void anim_x_cb(void *var, int32_t v)
{
    lv_obj_set_y(var, v);
}
static void my_event_cb_1(lv_event_t *event)
{

    lv_event_code_t code = lv_event_get_code(event);
    if (code < LV_EVENT_HIT_TEST)
        printf("code:%d\n", code);

    if ((code == 256) || (code == 7))
    {
        int dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        printf("dir:%d\n", dir);
        if (dir == LV_DIR_TOP) //����
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, lock_gui);
            lv_anim_set_values(&a, 0, -272);
            lv_anim_set_time(&a, 200);
            lv_anim_set_exec_cb(&a, anim_x_cb);
            lv_anim_set_ready_cb(&a, anim_read_cb);
            lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
            lv_anim_start(&a);
        }
    }

    // printf("Clicked\n");
}
static void my_event_cb_2(lv_event_t *event)
{
    int dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    printf("dir2:%d\n", dir);
    if (dir == LV_DIR_TOP) //����
    {
    }
}

void lvgl_sn_interface_show()
{

    lv_timer_t *sn_timer = lv_timer_create(timer_cb, 1000, NULL);
    // create_net_up();
    // create_alarm();
    // create_device_status();
    // create_desk();
    // return;
    sn_screen = lv_scr_act();
    lock_gui = lv_obj_create(sn_screen);
    // lv_obj_set_style_local_bg_color(sn_screen, 1, LV_PART_INDICATOR | LV_STATE_FOCUSED);
    lv_obj_set_size(lock_gui, 480, 272);
    static lv_style_t style_bg;
    lv_style_init(&style_bg);
    LV_IMG_DECLARE(lock);
    lv_style_set_bg_img_src(&style_bg, &lock);
    lv_obj_add_style(lock_gui, &style_bg, 0);
    // lv_obj_fade_in(lock_gui,0, 500);

    lv_obj_add_event_cb(lock_gui, my_event_cb_1, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(lock_gui, my_event_cb_2, LV_EVENT_GESTURE, NULL);
    //

    // static lv_style_t style_indic;
    // lv_style_init(&style_indic);
    // lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    // lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));

    // lv_obj_remove_style_all(lv_scr_act());  /*To have a clean start*/
    // lv_obj_add_style(lv_scr_act(), &style_indic, 0);

    meter = lv_meter_create(lock_gui);
    lv_obj_set_size(meter, 200, 200);
    lv_obj_center(meter);
    lv_obj_align_to(meter, lock_gui, LV_ALIGN_RIGHT_MID, -10, -5);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY)); /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10);       /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);                            /*[1..12] values in an almost full circle*/

    LV_IMG_DECLARE(img_hand)

    /*Add a the hands from images*/
    indic_min = lv_meter_add_needle_img(meter, scale_min, &img_hand, 5, 5);
    indic_hour = lv_meter_add_needle_img(meter, scale_min, &img_hand, 3, 3);

    lv_meter_set_indicator_end_value(meter, indic_min, 43);
    lv_meter_set_indicator_end_value(meter, indic_hour, 27);
    lv_obj_fade_in(meter, 0, 500);

    /* label time style */
    static lv_style_t style_label_time;
    lv_style_init(&style_label_time);
    lv_style_set_text_font(&style_label_time, &lv_font_montserrat_48);
    lv_style_set_bg_opa(&style_label_time, LV_OPA_0);                   // 设置色彩透明度
    lv_style_set_text_color(&style_label_time, lv_color_hex(0xFFFFFF)); //设置颜色

    /* label date style */
    static lv_style_t style_label_time_date;
    lv_style_init(&style_label_time_date);
    lv_style_set_text_font(&style_label_time_date, &lv_font_montserrat_22);
    lv_style_set_bg_opa(&style_label_time_date, LV_OPA_0);                   // 设置色彩透明度
    lv_style_set_text_color(&style_label_time_date, lv_color_hex(0xFFFFFF)); //设置颜色

    /* 系统时间标签 */

    systime_task_label = lv_label_create(lock_gui); /* 创建标签 */
    lv_obj_add_style(systime_task_label, &style_label_time, 0);
    lv_label_set_text(systime_task_label, "00:00");
    lv_obj_align(systime_task_label, LV_ALIGN_TOP_LEFT, 20, 25);
    lv_obj_fade_in(systime_task_label, 0, 100);

    /* 日期标签 */
    sysdate_task_label = lv_label_create(lock_gui); /* 创建标签 */
    lv_obj_add_style(sysdate_task_label, &style_label_time_date, 0);
    // lv_label_set_text(sysdate_task_label, "Tuesday,December 22");
    lv_label_set_text(sysdate_task_label, "2022/12/23");
    lv_obj_align_to(sysdate_task_label, systime_task_label, LV_ALIGN_OUT_BOTTOM_LEFT, 5, 0);
    lv_obj_fade_in(sysdate_task_label, 0, 100);

    /* label app name style */
    static lv_style_t style_label_app_name;
    lv_style_init(&style_label_app_name);
    lv_style_set_text_font(&style_label_app_name, &lv_font_montserrat_12);
    lv_style_set_bg_opa(&style_label_app_name, LV_OPA_0);                   // 设置色彩透明度
    lv_style_set_text_color(&style_label_app_name, lv_color_hex(0xFFFFFF)); //设置颜色
    static lv_style_t style_bottom_menu;
    lv_style_init(&style_bottom_menu);
    lv_style_set_text_font(&style_label_app_name, &lv_font_montserrat_12);
    lv_style_set_bg_opa(&style_bottom_menu, LV_OPA_80);                // 设置色彩透明度
    lv_style_set_radius(&style_bottom_menu, 10);                       // 设置圆角
    lv_style_set_bg_color(&style_bottom_menu, lv_color_hex(0xC9C9CE)); //设置颜色

    // lv_obj_t *  label_tip = lv_label_create(lock_gui);
    // lv_obj_add_style(label_tip, &style_label_app_name, 0);
    // lv_label_set_text(label_tip, "What is your main focus for today?");
    // lv_obj_align_to(label_tip, lock_gui, LV_ALIGN_LEFT_MID, 20, 50);
#define CANVAS_LEFT_LINE_WIDTH 240
#define CANVAS_LEFT_LINE_HEIGHT 40
    static lv_color_t cbuf_letf_line[LV_CANVAS_BUF_SIZE_INDEXED_1BIT(CANVAS_LEFT_LINE_WIDTH, CANVAS_LEFT_LINE_HEIGHT)];
    lv_obj_t *canvas_letf_tip_window = lv_canvas_create(lock_gui);
    lv_canvas_set_buffer(canvas_letf_tip_window, cbuf_letf_line, CANVAS_LEFT_LINE_WIDTH, CANVAS_LEFT_LINE_HEIGHT, LV_IMG_CF_INDEXED_1BIT);
    lv_obj_align_to(canvas_letf_tip_window, NULL, LV_ALIGN_TOP_LEFT, 0, 150);
    lv_obj_add_style(canvas_letf_tip_window, &style_bottom_menu, 0);
    lv_obj_fade_in(canvas_letf_tip_window, 0, 50);
    //lv_obj_set_style_local_value_str(canvas_letf_tip_window, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,\
            "What is your main focus for today?"); // What do you want to do today?
    lv_obj_t *label_tip = lv_label_create(canvas_letf_tip_window);
    lv_obj_add_style(label_tip, &style_label_app_name, 0);
    lv_label_set_text(label_tip, "What is your main focus for today?");
    lv_obj_align_to(label_tip, canvas_letf_tip_window, LV_ALIGN_CENTER, 0, 0);
}
#endif

void app_close_cb(lv_event_t * e)
{
    int count = lv_obj_get_child_cnt(sn_screen);
    printf("count:%d\n", count);

    // for (char i = 0; i < count; i++)
    // {
    //     lv_obj_t * child = lv_obj_get_child(sn_screen, i);
    //     if (child != sn_screen)
    //     {
    //         printf("lv_obj_del:%d\n",child);
    //         if(child)
    //         {
    //             lv_obj_del(child);
    //         }
    //     }
    // }

    // lv_obj_t *child = lv_obj_get_child(sn_screen, NULL);
    // while (child)
    // {
    //     if (child != sn_screen)
    //     {
    //         printf("lv_obj_del\n");
    //         lv_obj_del(child);
    //     }
    //     child++;
    //     child = lv_obj_get_child(sn_screen, child);
    // }
    lv_obj_clean(sn_screen);
    int num = lv_event_get_user_data(e);
    printf("close num:%d\n", num);
    switch (num)
    {
    case 0:
        dev_label_title = NULL;
        dev_temp_meter =NULL;
        dev_tmp_indic =NULL;
        dev_temp_label;
        dev_humi_bar=NULL;
        dev_sw[0] = NULL;
        dev_sw[1] = NULL;
        dev_sw[2] = NULL;
        break;

    default:
        break;
    }

    create_desk();
}

void timer_cb(struct _lv_timer_t *timer)
{
    time_t times;
    struct tm *now;

    time(&times);
    now = localtime(&times);
    //printf("asctime:%s", asctime(now));

    char buff[24];
    sprintf(buff, "%02d:%02d", now->tm_hour, now->tm_min);
    if (systime_task_label)
    {
        lv_label_set_text(systime_task_label, buff);
    }

    sprintf(buff, "%04d/%02d/%02d", now->tm_year + 1900, now->tm_mon, now->tm_mday);

    if (sysdate_task_label)
    {
        lv_label_set_text(sysdate_task_label, buff);
    }
    if (meter)
    {
        lv_meter_set_indicator_end_value(meter, indic_min, now->tm_min);
        if (now->tm_hour > 12)
        {
            now->tm_hour = now->tm_hour - 12;
        }
        lv_meter_set_indicator_end_value(meter, indic_hour, now->tm_hour * 5 + (now->tm_min / 12));
    }

    if (label_time)
    {
        sprintf(buff, "%04d/%02d/%02d  %02d:%02d:%02d", now->tm_year + 1900, now->tm_mon, now->tm_mday,now->tm_hour, now->tm_min, now->tm_sec);
        lv_label_set_text(label_time, buff);
    }
    if(weather_label)
    {
        lv_label_set_text(weather_label, weather_txt);
    }
    dev_status_update();
}
//转用户名收件人设置件远程时间天气温度湿度日期开发中邮箱服务器地址端口号密码账户设备状态报警配置同步输出晴多云大雨雪小中阴天灯光开关空调开关加速器开关 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~℃
