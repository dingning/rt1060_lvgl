#include <rtthread.h>
#include <rtdevice.h>
#include "zFTP.h"

#define LOG_TAG "ftp"
#define LOG_LVL LOG_LVL_DBG
#include "global.h"


#define FTP_SVR_PATH    "/"
#define FTP_USER_NAME   "anonymous"
#define FTP_PASSWORD    "dingning@"
#define FTP_FILENAME    "devmgr.cfg"
#define FIRMWARE_FLASH_PARTITION  "firmware"
#define FIRMWARE_STORE_ADDR        0

ftp_cfg_t g_ftp_cfg =
{
    .ip = "192.168.199.185" ,
    .port = 21,
};

/*********************************************************************************************************
** Function name��      file_read()
** Descriptions:        read file from flash 
** input parameters��   ftp, buf, len, file_pos, total_len
** output parameters��  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_read(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t *read_len, rt_uint32_t *total_len)
{
    struct stat file_stat;
    int  ret = stat(file_name, &file_stat);
    if(ret == 0)
    {
        LOG_D("%s size=%d", file_name, file_stat.st_size);
        *total_len =  file_stat.st_size;
    }
    else
    {
        LOG_W("%s not fonud.", file_name);
        return RT_FALSE;
    }
    int fd = open(file_name,O_RDONLY);
    if( fd < 0)
    {
        LOG_W("open %s failed.",file_name);
        return RT_FALSE;
    } 
    lseek(fd, file_pos, SEEK_SET);

    *read_len = read(fd, buf, len);
    close(fd);

    return RT_EOK;

}     

/*********************************************************************************************************
** Function name��      file_write()
** Descriptions:        write file into flash 
** input parameters��   ftp, buf, len, file_pos, total_len
** output parameters��  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_write(void * handle, char *file_name, rt_uint8_t *buf, rt_uint32_t len, rt_uint32_t file_pos, rt_uint32_t total_len)
{
    return RT_EOK;
}

/*********************************************************************************************************
** Function name��      file_write()
** Descriptions:        write file into flash 
** input parameters��   ftp, buf, len, file_pos, total_len
** output parameters��  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
int file_erase(rt_uint32_t file_len)
{
    return RT_EOK;
}

int ftp_upload(char *filename)
{
    LOG_D("ftp_upload");
    zftp_client *ftp;
    rt_uint32_t file_size = 0;
    
    ftp = zFTP_login(FTP_USER_NAME, FTP_PASSWORD, g_ftp_cfg.ip);

    if(ftp == RT_NULL)
    {
        LOG_W("zFTP login fail!");
    } 
    else
    {
        LOG_D("zFTP login success!");
        zFTP_set_callback(ftp, file_write, file_read, RT_NULL);

        zFTP_change_path(ftp, FTP_SVR_PATH);
        if(RT_EOK == zFTP_upload_file(ftp, filename))
        {
            LOG_D("zFTP upload  %s success!", filename);
        }
        else
        {
            LOG_W("zFTP upload  %s failed!", filename);
        }  
    }
}
/*********************************************************************************************************
** Function name��      cmd_ftp_download()
** Descriptions:        ftp download command line
** input parameters��   argc, argv
** output parameters��  None
** Returned value:      RT_OK, RT_ERROR
*********************************************************************************************************/
// int cmd_ftp(int argc, char **argv)
// {
//     zftp_client *ftp;
//     rt_uint32_t file_size = 0;

//     rt_kprintf("cmd_ftp!\n");
//     ftp = zFTP_login(FTP_USER_NAME, FTP_PASSWORD, FTP_SVR_ADDR);

//     if(ftp == RT_NULL)
//     {
//         rt_kprintf("zFTP login fail!\n");
//     } 
//     else
//     {
//         rt_kprintf("zFTP login success!\n");
//         zFTP_set_callback(ftp, file_write, file_read, RT_NULL);

//         zFTP_change_path(ftp, FTP_SVR_PATH);

//         if(rt_strcmp(argv[2], "upload") == 0)
//         {
//             if(RT_EOK == zFTP_upload_file(ftp, argv[1]))
//             {
//                 rt_kprintf("zFTP upload success!\n");
//             }
//             else
//             {
//                 rt_kprintf("zFTP upload  %s faile!\n", argv[1]);
//             }  
//         }
//         else
//         {
//             if(RT_EOK == zFTP_get_filesize(ftp, argv[1], &file_size))
//             {
//               rt_kprintf("zFTP file %s size:%d!\n", argv[1], file_size);
//               file_erase(file_size);

//               if(RT_EOK == zFTP_download_file(ftp, argv[1]))
//               {
//                   rt_kprintf("zFTP download success len: %d!\n", file_size);
//               }
                    
//             }
//         }
        
//         zFTP_quit(ftp);

//     }  
    
//     return 0;
// }

// #ifdef FINSH_USING_MSH
// #include <finsh.h>
// MSH_CMD_EXPORT_ALIAS(cmd_ftp ,ftp,  ftp download file);
// #endif



