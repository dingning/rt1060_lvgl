#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define LOG_TAG              "mail"
#define LOG_LVL              LOG_LVL_DBG
#include "global.h"

#define SOCKET_ERROR -1
#define IPSTR "220.181.12.12"
#define PRINT(x) printf("%s\n", (x))

int sockfd;
int ret;
char *send_data;
char *recv_data;
char From[128];
char To[128];
char Date[128];
char Subject[64];
char Context[1024] = {0};
char buffer[1024];

int connectHost(const char *smtpaddr)
{
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Create socket error!\n");
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(25);

    if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error!\n");
        return -1;
    };

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Connect failed... \n");
        return -1;
    }
    printf("Connect to %s.... \n", IPSTR);

    memset(buffer, 0, sizeof(buffer));

    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0)
    {
        printf("receive failed... \n");
        return -1;
    }
    else
    {
        printf("%s\n", buffer);
    }
    return sockfd;
}

int getResponse()
{
    memset(buffer, 0, sizeof(buffer));
    ret = recv(sockfd, buffer, 1024, 0);
    if (ret == SOCKET_ERROR)
    {
        printf("receive nothing\n");
        return -1;
    }
    buffer[ret] = '\0';

    if (*buffer == '5')
    {
        printf("the order is not support smtp host\n ");
        return -1;
    }

    printf("recieved buffer:%s\n", buffer);

    return 0;
}

int login(char *username, char *password)
{
    printf("login.....sockfd: %d\n", sockfd);
    char ch[100];
    if (username == "" || password == "")
        return -1;

    send_data = "HELO 163.com\r\n";
    printf("%s\n", send_data);
    ret = send(sockfd, send_data, strlen(send_data), 0);
    if (ret == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }
    if (getResponse() < 0)
        return -1;

    send_data = "AUTH LOGIN\r\n";
    printf("%s\n", send_data);
    ret = send(sockfd, send_data, strlen(send_data), 0);
    if (ret == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }
    if (getResponse() < 0)
        return -1;
    sprintf(ch, "%s\r\n", username);
    printf("%s\n", ch);
    ret = send(sockfd, (char *)ch, strlen(ch), 0);
    if (ret == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }

    if (getResponse() < 0)
        return -1;

    sprintf(ch, "%s\r\n", password);
    printf("%s\n", ch);
    ret = send(sockfd, (char *)ch, strlen(ch), 0);
    if (ret == SOCKET_ERROR)
    {

        close(sockfd);
        return -1;
    }

    if (getResponse() < 0)
        return -1;

    return 0;
}

int sendmail(char *from, char *to, char *subject, char *context)
{
    if (from == "" || to == "" || subject == "" || context == "")
    {
        printf("arguments error!\n");
        return -1;
    }

    sprintf(From, "MAIL FROM: <%s>\r\n", from);
    printf("%s\n", From);

    if ((ret = send(sockfd, From, strlen(From), 0)) == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }

    if (getResponse() < 0)
        return -1;

    sprintf(To, "RCPT TO: <%s>\r\n", to);
    if ((ret = send(sockfd, To, strlen(To), 0)) == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }
    if (getResponse() < 0)
        return -1;

    send_data = "DATA\r\n";
    if ((ret = send(sockfd, send_data, strlen(send_data), 0)) == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }
    if (getResponse() < 0)
        return -1;

    memset(Subject, 0, sizeof(Subject));
    sprintf(Subject, "Subject: %s\r\n", subject);
    strcat(Context, Subject);
    strcat(Context, "\r\n\r\n");
    strcat(Context,context);
    strcat(Context, "\r\n.\r\n");
    if ((ret = send(sockfd, Context, strlen(Context), 0)) == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }

    printf("ret size:%d\n", ret);

    memset(buffer, '\0', sizeof(buffer));

    if (getResponse() < 0)
        return -1;

    printf("QUIT!!!!!!!!!!!!!!!!\n");
    if ((ret = send(sockfd, "QUIT\r\n", strlen("QUIT\r\n"), 0)) == SOCKET_ERROR)
    {
        close(sockfd);
        return -1;
    }
    if (getResponse() < 0)
        return -1;

    printf("Send Mail Successful!\n");
    return 0;
}

int send_mail(char *server, char *name, char *passwd, char *from, char *to, char *subject, char *context)
{

    printf("server:%s\n",server);
    printf("name:%s\n",name);
    printf("passwd:%s\n",passwd);
    printf("from:%s\n",from);
    printf("to:%s\n",to);
    printf("subject:%s\n",subject);
    printf("context:%s\n",context);

    char *name_base64 = utl_base64_encode(name , strlen(name));
    char *passwd_base64 = utl_base64_encode(passwd , strlen(passwd));
    printf("name_base64:%s\n",name_base64);
    printf("passwd_base64:%s\n",passwd_base64);

    if(connectHost(server) < 0)
    {
        printf("Can Not LOGIN !\n");
        return -1;
    }

    if (login(name_base64, passwd_base64) < 0)
    {
        fprintf(stderr, "Can Not LOGIN !\n");
        return -1;
    }
    sendmail(from, to, subject, context);
    return 0;
}
#if 0
int main()
{
    char from[128] = "dingningwy@163.com";
    char to[128] = "dingningwy@163.com";

    char subject[512] = "test....";
    char context[6000] = "Just a test.........\n";
    char server[56] = "smtp.163.com";

    char *name = "ZGluZ25pbmd3eUAxNjMuY29t";   //"dingningwy@163.com";//base64
    char *passwd = "TU1VU0lLT09OS0ZaTlJPSQ=="; // MMUSIKOONKFZNROI";//base64

    if (connectHost(server) < 0)
    {
        printf("Can Not LOGIN !\n");
        return -1;
    }

    if (login(name, passwd) < 0)
    {
        fprintf(stderr, "Can Not LOGIN !\n");
        return -1;
    }
    sendmail(from, to, subject, context);
    return 0;
}
#endif 

mail_cfg_t g_mail_cfg = 
{
    .server = "smtp.163.com",
    .name = "dingningwy@163.com",
    .passwd = "MMUSIKOONKFZNROI",
    .from = "dingningwy@163.com",
    .to = "dingningwy@163.com",
    .subject = "test....",
    .contex = "Just a test.........\n",
};

void mail_alarm_up(int devindex, int type)
{
    if(devindex > dev_num)
    {
        return;
    }
    char title[16] = {0};
    char buff[64] = {0};
    switch (type)
    {
    case 0:
        sprintf(title,"%s","温度");
        sprintf(buff,"报警温度：%d\n当前温度：%d\n",dev_attr[devindex].cfg.alarm_temperature,
                dev_attr[devindex].cfg.now_temperature);
        break;
    case 1:
        sprintf(title,"%s","湿度");
        sprintf(buff,"报警湿度：%d\n当前湿度：%d\n",dev_attr[devindex].cfg.alarm_humidity,
                dev_attr[devindex].cfg.now_humidity);
        break;
    default:
        break;
    }
    //printf("title:%s\n",title);

    sprintf(g_mail_cfg.subject,"%s报警",title);
    //printf("g_mail_cfg.subject:%s\n",g_mail_cfg.subject);

    sprintf(g_mail_cfg.contex,"网关ID:%s\n设备ID:%s\n报警类型:%s\n%s",
            GW_ID,dev_attr[devindex].cfg.name,title,buff);
    
    //printf("g_mail_cfg.contex:%s\n",g_mail_cfg.contex);

    int ret = send_mail(g_mail_cfg.server, g_mail_cfg.name, g_mail_cfg.passwd, g_mail_cfg.from, 
        g_mail_cfg.to, g_mail_cfg.subject, g_mail_cfg.contex);
    
    if(ret < 0)
    {
        LOG_W("send alarm mail to %s failed.", g_mail_cfg.to);  
    }
    else
    {
        LOG_I("send alarm mail to %s success.", g_mail_cfg.to);
    }
}