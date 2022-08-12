#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256

/* kservice optimization */


/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_MEMHEAP_AS_HEAP
#define RT_USING_MEMHEAP_AUTO_BINDING
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 512
#define RT_CONSOLE_DEVICE_NAME "uart1"
#define RT_VER_NUM 0x40101

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_LEGACY
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FILESYSTEM_TYPES_MAX 4
#define DFS_FD_MAX 16
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_RB_BUFSZ 512
#define RT_USING_CPUTIME
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_RTC
#define RT_USING_SOFT_RTC

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_POLL
#define RT_USING_POSIX_SELECT
#define RT_USING_POSIX_SOCKET

/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */


/* Network */

#define RT_USING_SAL
#define SAL_INTERNET_CHECK

/* Docking with protocol stacks */
#define RT_USING_POSIX
#define PIPE_BUFSZ 512

#define SAL_USING_AT
#define SAL_USING_POSIX
#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0
#define RT_USING_AT
#define AT_DEBUG
#define AT_USING_CLIENT
#define AT_CLIENT_NUM_MAX 1
#define AT_USING_SOCKET
#define AT_USING_CLI
//#define AT_PRINT_RAW_CMD
#define AT_CMD_MAX_LEN 128
#define AT_SW_VERSION_NUM 0x10301

/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_USING_ISR_LOG
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_TIME_USING_TIMESTAMP
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_BACKEND_USING_FILE

/* RT-Thread online packages */

/* IoT - internet of things */

// #define PKG_USING_PAHOMQTT
// //#define PAHOMQTT_PIPE_MODE
// #define PAHOMQTT_UDP_MODE
// #define RT_PKG_MQTT_THREAD_STACK_SIZE 4096
// #define PKG_PAHOMQTT_SUBSCRIBE_HANDLERS 1
// #define MQTT_DEBUG
// #define PKG_USING_PAHOMQTT_V110

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_AT_DEVICE
#define AT_DEVICE_USING_ESP32
#define AT_DEVICE_ESP32_SOCKET
#define AT_DEVICE_ESP32_SAMPLE
#define ESP32_SAMPLE_WIFI_SSID "HiWiFi_2AE9FA"
#define ESP32_SAMPLE_WIFI_PASSWORD "13188818820"
#define ESP32_SAMPLE_CLIENT_NAME "uart3"
#define ESP32_SAMPLE_RECV_BUFF_LEN 512
#define PKG_USING_AT_DEVICE_LATEST_VERSION
#define PKG_AT_DEVICE_VER_NUM 0x99999

/* IoT Cloud */

#define PKG_USING_TCPSERVER
#define TCPSERVER_NAME "tcpserv"
#define TCPSERVER_STACK_SIZE 4096
#define TCPSERVER_PRIO 12
#define TCPSERVER_SOCKET_MAX 512
#define TCPSERVER_CLI_NUM 5
#define PKG_USING_TCPSERVER_LATEST_VERSION
#define PKG_USING_ZFTP
#define PKG_USING_ZFTP_LATEST_VERSION

/* security packages */


/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */

#define PKG_USING_CJSON
#define PKG_USING_CJSON_V102

/* XML: Extensible Markup Language */

#define RT_LWIP_NETIF_LOOPBACK
/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */

#define PKG_USING_LVGL
#define PKG_LVGL_THREAD_PRIO 20
#define PKG_LVGL_THREAD_STACK_SIZE 4096*10
#define PKG_LVGL_DISP_REFR_PERIOD 5
#define PKG_LVGL_USING_V08030
#define PKG_LVGL_VER_NUM 0x08030
#define PKG_USING_LV_MUSIC_DEMO

/* u8g2: a monochrome graphic library */


/* PainterEngine: A cross-platform graphics application framework written in C language */


/* tools packages */


/* system packages */

/* enhanced kernel services */


/* acceleration: Assembly language or algorithmic acceleration packages */


/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */


/* Micrium: Micrium software products porting for RT-Thread */


/* peripheral libraries and drivers */


/* AI packages */


/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */


/* entertainment: terminal games and other interesting software packages */


/* Hardware Drivers Config */

#define BSP_USING_4MFLASH
#define SOC_MIMXRT1062DVL6A

/* Onboard Peripheral Drivers */

#define BSP_USING_LCD
#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define LCD_HFP 4
#define LCD_VFP 4
#define LCD_HBP 8
#define LCD_VBP 2
#define LCD_HSW 40
#define LCD_VSW 10
#define LCD_BL_PIN 63
#define LCD_RST_PIN 2
#define BSP_USING_TOUCHPAD
#define DEMO_PANEL_RK043FN66HS
#define BSP_USING_LVGL
#define BSP_USING_LVGL_DEMO
#define BSP_USING_LVGL_SQUARELINE

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_I2C
#define BSP_USING_I2C1
#define HW_I2C1_BADURATE_100kHZ
#define BSP_USING_LPUART
#define BSP_USING_LPUART1
#define BSP_USING_LPUART3
#define BSP_USING_PXP
#define BSP_USING_CACHE

/* Board extended module Drivers */
#define PKG_USING_NETUTILS
#define PKG_NETUTILS_NTP
#define NTP_USING_AUTO_SYNC
#define NTP_AUTO_SYNC_FIRST_DELAY 30
#define NTP_AUTO_SYNC_PERIOD 3600
#define NETUTILS_NTP_HOSTNAME "cn.ntp.org.cn"
#define NETUTILS_NTP_HOSTNAME2 "ntp.rt-thread.org"
#define NETUTILS_NTP_HOSTNAME3 "edu.ntp.org.cn"
#define PKG_USING_NETUTILS_LATEST_VERSION
#define PKG_NETUTILS_VER_NUM 0x99999

#define PKG_USING_UMQTT
#define PKG_USING_UMQTT_EXAMPLE
#define PKG_UMQTT_SUBRECV_DEF_LENGTH 4
#define PKG_UMQTT_INFO_DEF_SENDSIZE 1024
#define PKG_UMQTT_INFO_DEF_RECVSIZE 1024
#define PKG_UMQTT_USING_DEBUG
#define PKG_UMQTT_INFO_DEF_RECONNECT_MAX_NUM 5
#define PKG_UMQTT_INFO_DEF_RECONNECT_INTERVAL 60
#define PKG_UMQTT_INFO_DEF_KEEPALIVE_MAX_NUM 5
#define PKG_UMQTT_INFO_DEF_HEARTBEAT_INTERVAL 30
#define PKG_UMQTT_INFO_DEF_CONNECT_TIMEOUT 4
#define PKG_UMQTT_INFO_DEF_RECV_TIMEOUT_MS 100
#define PKG_UMQTT_INFO_DEF_SEND_TIMEOUT 4
#define PKG_UMQTT_INFO_DEF_THREAD_STACK_SIZE 2048
#define PKG_UMQTT_INFO_DEF_THREAD_PRIORITY 8
#define PKG_UMQTT_MSG_QUEUE_ACK_DEF_SIZE 4
#define PKG_UMQTT_CONNECT_KEEPALIVE_DEF_TIME 0xffff
#define PKG_UMQTT_PUBLISH_RECON_MAX 3
#define PKG_UMQTT_QOS2_QUE_MAX 1
#define PKG_USING_UMQTT_V100

#endif
