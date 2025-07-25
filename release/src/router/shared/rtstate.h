#ifndef __RTSTATE_H__
#define __RTSTATE_H__
#include <rtconfig.h>
#include <bcmnvram.h>

enum {
	SW_MODE_NONE=0,
	SW_MODE_ROUTER,
	SW_MODE_REPEATER,	/* Ralink/MTK/QCA: if wlc_psta = 1, Media bridge mode. */
	SW_MODE_AP,		/* Broadcom:       if wlc_psta = 1, Media bridge mode. */
	SW_MODE_HOTSPOT
};

enum wan_unit_e {
	WAN_UNIT_NONE=-1,
	WAN_UNIT_FIRST=0,
#if defined(RTCONFIG_DUALWAN) || defined(RTCONFIG_USB_MODEM)
	WAN_UNIT_SECOND,
#endif
	WAN_UNIT_MAX
};

enum {
	MODEM_UNIT_NONE=-1,
	MODEM_UNIT_FIRST=0,
#ifdef RTCONFIG_USB_MULTIMODEM
	MODEM_UNIT_SECOND,
#endif
	MODEM_UNIT_MAX
};

#ifdef RTCONFIG_MULTICAST_IPTV
enum {
        WAN_UNIT_IPTV=10,
        WAN_UNIT_VOIP,
        WAN_UNIT_MULTICAST_IPTV_MAX
};
#endif

#ifdef RTCONFIG_MULTISERVICE_WAN
///design for WAN_UNIT_MAX < 10
///extend additional 9 services for original wan unit
#define WAN_MULTISRV_MAX             8
enum {
	WAN_UNIT_MULTISRV_BASE=100,
	WAN_UNIT_FIRST_MULTISRV_BASE=100,
	WAN_UNIT_FIRST_MULTISRV_START=101,
	WAN_UNIT_FIRST_MULTISRV_END=107,
#if defined(RTCONFIG_DUALWAN) || defined(RTCONFIG_USB_MODEM)
	WAN_UNIT_SECOND_MULTISRV_BASE=110,
	WAN_UNIT_SECOND_MULTISRV_START=111,
	WAN_UNIT_SECOND_MULTISRV_END=117,
	WAN_UNIT_TMP_MULTISRV_BASE=120,
	WAN_UNIT_TMP_MULTISRV_START=121,
	WAN_UNIT_TMP_MULTISRV_END=127,
#endif
	WAN_UNIT_MULTISRV_MAX
};

#ifdef RTCONFIG_MULTIWAN_IF
enum {
	WAN_UNIT_MTWAN_MS_BASE=1000,
	WAN_UNIT_MTWAN0_MS_BASE=1500,
	WAN_UNIT_MTWAN0_MS_START=1501,
	WAN_UNIT_MTWAN0_MS_END=1507,
#if MAX_MULTI_WAN_NUM > 1
	WAN_UNIT_MTWAN1_MS_BASE=1510,
	WAN_UNIT_MTWAN1_MS_START=1511,
	WAN_UNIT_MTWAN1_MS_END=1517,
#if MAX_MULTI_WAN_NUM > 2
	WAN_UNIT_MTWAN2_MS_BASE=1520,
	WAN_UNIT_MTWAN2_MS_START=1521,
	WAN_UNIT_MTWAN2_MS_END=1527,
#if MAX_MULTI_WAN_NUM > 3
	WAN_UNIT_MTWAN3_MS_BASE=1530,
	WAN_UNIT_MTWAN3_MS_START=1531,
	WAN_UNIT_MTWAN3_MS_END=1537,
#if MAX_MULTI_WAN_NUM > 4
	WAN_UNIT_MTWAN4_MS_BASE=1540,
	WAN_UNIT_MTWAN4_MS_START=1541,
	WAN_UNIT_MTWAN4_MS_END=1547,
#endif	//MAX_MULTI_WAN_NUM > 4
#endif	//MAX_MULTI_WAN_NUM > 3
#endif	//MAX_MULTI_WAN_NUM > 2
#endif	//MAX_MULTI_WAN_NUM > 1
	WAN_UNIT_MTWAN_MS_MAX
};
#endif
#endif

#ifdef RTCONFIG_MULTI_PPP
#define WAN_MULTIPPP_MAX             8
enum {
	WAN_UNIT_MULTIPPP_BASE=30,
	WAN_UNIT_FIRST_MULTIPPP_BASE=30,
	WAN_UNIT_FIRST_MULTIPPP_START=31,
	WAN_UNIT_FIRST_MULTIPPP_END=37,
#if defined(RTCONFIG_DUALWAN)
	WAN_UNIT_SECOND_MULTIPPP_BASE=40,
	WAN_UNIT_SECOND_MULTIPPP_START=41,
	WAN_UNIT_SECOND_MULTIPPP_END=47,
#endif
	WAN_UNIT_MULTIPPP_MAX
};
#endif

enum {
	WAN_STATE_INITIALIZING=0,
	WAN_STATE_CONNECTING,
	WAN_STATE_CONNECTED,
	WAN_STATE_DISCONNECTED,
	WAN_STATE_STOPPED,
	WAN_STATE_DISABLED,
	WAN_STATE_STOPPING
};

enum {
	WAN_STOPPED_REASON_NONE=0,
	WAN_STOPPED_REASON_PPP_NO_RESPONSE,
	WAN_STOPPED_REASON_PPP_AUTH_FAIL,
	WAN_STOPPED_REASON_DHCP_DECONFIG,
	WAN_STOPPED_REASON_INVALID_IPADDR,
	WAN_STOPPED_REASON_MANUAL,
	WAN_STOPPED_REASON_SYSTEM_ERR,
	WAN_STOPPED_REASON_IPGATEWAY_CONFLICT,
	WAN_STOPPED_REASON_METER_LIMIT,
	WAN_STOPPED_REASON_PINCODE_ERR,
	WAN_STOPPED_REASON_PPP_LACK_ACTIVITY, // 10
	WAN_STOPPED_REASON_DATALIMIT,
	WAN_STOPPED_REASON_USBSCAN
};

enum {
	NAT_STATE_INITIALIZING=0,
	NAT_STATE_REDIRECT,
	NAT_STATE_NORMAL,
	NAT_STATE_UPDATE
};

// the following state is maintained by wan duck

enum {
	WAN_AUXSTATE_NONE=0,		// STATE FOR NO ERROR or OK
	WAN_AUXSTATE_NOPHY,
	WAN_AUXSTATE_NO_INTERNET_ACTIVITY
};

#ifdef RTCONFIG_IPV6
enum {
	WAN6_STATE_INITIALIZING=0,
	WAN6_STATE_CONNECTING,
	WAN6_STATE_CONNECTED,
	WAN6_STATE_DISCONNECTED,
	WAN6_STATE_STOPPED,
	WAN6_STATE_DISABLED
};

enum {
	WAN6_STOPPED_REASON_NONE=0,
	WAN6_STOPPED_REASON_DHCP_DECONFIG,
};
#ifdef RTCONFIG_SOFTWIRE46
enum S46_HGW_CASE {
	S46_CASE_INIT		= 0,
	S46_CASE_MAP_HGW_ON	= 2,
	S46_CASE_MAP_HGW_OFF	= 3,
	S46_CASE_MAP_CE_ON	= 6
};

enum {
	WAN46DET_STATE_INITIALIZING=0,
	WAN46DET_STATE_NOLINK,
	WAN46DET_STATE_UNKNOW,
	WAN46DET_STATE_V6PLUS,
	WAN46DET_STATE_HGW_V6PLUS,
	WAN46DET_STATE_OCNVC,
	WAN46DET_STATE_DSLITE_XPASS,
	WAN46DET_STATE_DSLITE_TRANSIX,
	WAN46DET_STATE_V6OPTION,
	WAN46DET_STATE_HGW_V6OPTION
};
#endif
#endif

enum {
	AUTODET_STATE_INITIALIZING=0,
	AUTODET_STATE_CHECKING,
	AUTODET_STATE_FINISHED_OK,
	AUTODET_STATE_FINISHED_FAIL,
	AUTODET_STATE_FINISHED_NOLINK,
	AUTODET_STATE_FINISHED_NODHCP,
	AUTODET_STATE_FINISHED_WITHPPPOE
};


enum {
	LAN_STATE_INITIALIZING=0,
	LAN_STATE_CONNECTING,
	LAN_STATE_CONNECTED,
	LAN_STATE_DISCONNECTED,
	LAN_STATE_STOPPED,
	LAN_STATE_DISABLED
};

enum {
	LAN_STOPPED_REASON_NONE=0,
	LAN_STOPPED_REASON_DHCP_DECONFIG,
	LAN_STOPPED_REASON_SYSTEM_ERR
};

#if defined(CONFIG_BCMWL5) || defined(RTCONFIG_RALINK) || defined(RTCONFIG_QCA) || defined(RTCONFIG_REALTEK) || defined(RTCONFIG_ALPINE) || defined(RTCONFIG_LANTIQ)
enum { 
	WLC_STATE_INITIALIZING=0,
	WLC_STATE_CONNECTING,
	WLC_STATE_CONNECTED,
	WLC_STATE_STOPPED
};

enum {
	WLC_STOPPED_REASON_NONE=0,
	WLC_STOPPED_REASON_NO_SIGNAL,
	WLC_STOPPED_REASON_AUTH_FAIL,
	WLC_STOPPED_REASON_MANUAL
};

enum {
	WLCSCAN_STATE_STOPPED=0,
	WLCSCAN_STATE_INITIALIZING,
	WLCSCAN_STATE_2G,
	WLCSCAN_STATE_5G,
	WLCSCAN_STATE_5G_2,
	WLCSCAN_STATE_FINISHED,
};
#endif

// the following state is maintained by wan duck
#define WEBREDIRECT_FLAG_NOLINK 1
#define WEBREDIRECT_FLAG_NOINTERNET 2

// the following flag is used for noticing service that will be invoked after getting lan ip
#define INVOKELATER_DMS	1

enum {
	FW_INIT=0,
	FW_UPLOADING,
	FW_UPLOADING_ERROR,
	FW_WRITING,
	FW_WRITING_ERROR,
	FW_WRITE_SUCCESS,
	FW_TRX_CHECK_ERROR
};

enum {
	_ATE_FW_NOT_ATE=0,
	_ATE_FW_START,
	_ATE_FW_WRITING,
	_ATE_FW_UNEXPECT_ERROR,
	_ATE_FW_FAILURE,
	_ATE_FW_COMPLETE
};

#ifdef RTCONFIG_USB
#if defined(RTCONFIG_ALPINE) || defined(RTCONFIG_LANTIQ)
#define USB_XHCI_PORT_1 "2-1"
#define USB_XHCI_PORT_2 "2-2"
#define USB_EHCI_PORT_1 "1-1"
#define USB_EHCI_PORT_2 "1-2"
#define USB_OHCI_PORT_1 "3-1"
#define USB_OHCI_PORT_2 "3-2"
#define USB_EHCI_PORT_3 "1-3"
#define USB_OHCI_PORT_3 "3-3"
#endif

enum {
	USB_HOST_NONE=0,
	USB_HOST_OHCI,
	USB_HOST_EHCI,
	USB_HOST_XHCI
};

enum {
	APPS_AUTORUN_INITIALIZING=0,
	APPS_AUTORUN_CHECKING_DISK,
	APPS_AUTORUN_CREATING_SWAP,
	APPS_AUTORUN_EXECUTING,
	APPS_AUTORUN_FINISHED
};

enum {
	APPS_INSTALL_INITIALIZING=0,
	APPS_INSTALL_CHECKING_PARTITION,
	APPS_INSTALL_CHECKING_SWAP,
	APPS_INSTALL_DOWNLOADING,
	APPS_INSTALL_INSTALLING,
	APPS_INSTALL_FINISHED
};

enum {
	APPS_REMOVE_INITIALIZING=0,
	APPS_REMOVE_REMOVING,
	APPS_REMOVE_FINISHED
};

enum {
	APPS_SWITCH_INITIALIZING=0,
	APPS_SWITCH_STOPPING_APPS,
	APPS_SWITCH_STOPPING_SWAP,
	APPS_SWITCH_CHECKING_PARTITION,
	APPS_SWITCH_EXECUTING,
	APPS_SWITCH_FINISHED
};

enum {
	APPS_STOP_INITIALIZING=0,
	APPS_STOP_STOPPING,
	APPS_STOP_REMOVING_SWAP,
	APPS_STOP_FINISHED
};

enum {
	APPS_ENABLE_INITIALIZING=0,
	APPS_ENABLE_SETTING,
	APPS_ENABLE_FINISHED
};

enum {
	APPS_UPDATE_INITIALIZING=0,
	APPS_UPDATE_UPDATING,
	APPS_UPDATE_FINISHED
};

enum {
	APPS_UPGRADE_INITIALIZING=0,
	APPS_UPGRADE_DOWNLOADING,
	APPS_UPGRADE_REMOVING,
	APPS_UPGRADE_INSTALLING,
	APPS_UPGRADE_FINISHED
};

enum {
	APPS_ERROR_NO=0,
	APPS_ERROR_INPUT,
	APPS_ERROR_MOUNT,
	APPS_ERROR_POOLSIZE,
	APPS_ERROR_BASEAPPS,
	APPS_ERROR_NET,
	APPS_ERROR_SERVER,
	APPS_ERROR_FILE,
	APPS_ERROR_NONINSTALL,
	APPS_ERROR_REMOVE,
	APPS_ERROR_DISKIO
};

enum {
	DISKMON_IDLE=0,
	DISKMON_START,
	DISKMON_UMOUNT,
	DISKMON_SCAN,
	DISKMON_REMOUNT,
	DISKMON_FINISH,
	DISKMON_FORCE_STOP,
	DISKMON_FORMAT
};

#define DISKMON_FREQ_DISABLE 0
#define DISKMON_FREQ_MONTH 1
#define DISKMON_FREQ_WEEK 2
#define DISKMON_FREQ_DAY 3

#define DISKMON_SAFE_RANGE 10 // min.
#define DISKMON_DAY_HOUR 24
#define DISKMON_HOUR_SEC 3600

#if defined(RT4GAC55U) || defined(RT4GAC68U)
#define MAX_USB_PORT 2
#else
#define MAX_USB_PORT 3
#endif
#define MAX_USB_HUB_PORT 6
#define MAX_USB_DISK_NUM 8
#define MAX_USB_PART_NUM 16
#define MAX_USB_PRINTER_NUM 2
#define MAX_USB_TTY_NUM 10
#endif	// RTCONFIG_USB

#ifdef RTCONFIG_BROOP
enum {
	BROOP_IDLE,
	BROOP_DETECT
};

#endif

#ifdef RTCONFIG_ASUSCTRL
/* Always append new definition to end of enumeration. */
enum {
	ASUSCTRL_DFS_BAND2 = 1,
	ASUSCTRL_DFS_BAND3 = 2,
	ASUSCTRL_CHG_PWR = 3,
	ASUSCTRL_CHG_SKU = 4,
	ASUSCTRL_EG_MODE = 5,
	ASUSCTRL_SG_MODE = 6,
	ASUSCTRL_ACS_IGNORE_BAND2 = 7,
	ASUSCTRL_ACS_IGNORE_BAND3 = 8,
	ASUSCTRL_GEARUP_SUPPORT = 9,
	ASUSCTRL_GEARUP_BLOCK = 10,
	ASUSCTRL_ACS_IGNORE_BAND1 = 11,
	ASUSCTRL_ACS_IGNORE_BAND4 = 12,
	ASUSCTRL_ID_MODE = 13,
	ASUSCTRL_BE92U_WAR = 14, //0x4000
	ASUSCTRL_MAX
};
#endif

#ifdef RTCONFIG_MOCA
#define MAX_MOCA_NODES 16
#define MAX_MOCA_NUM_CHANNELS	5
enum	//moca_dev_state
{
	MOCA_STATE_UNKNOW,
	MOCA_STATE_INIT_ENV,
	MOCA_STATE_INIT_DEV,
	MOCA_STATE_DEV_READY,
	MOCA_STATE_ERR_DEV_FAIL,
	MOCA_STATE_ERR,
};

enum	//moca_conn_state
{
	MOCA_CONN_STATE_UNKNOWN,
	MOCA_CONN_STATE_PAIRED,
	MOCA_CONN_STATE_UNPAIRED,
};

enum //moca_mps_report
{
	MOCA_MPS_REPORT_INVALID,
	MOCA_MPS_REPORT_SCANNING,
	MOCA_MPS_REPORT_IN_PROGRESS,
	MOCA_MPS_REPORT_MPS_TRIGGER_FWD_NC,
	MOCA_MPS_REPORT_MPS_PRIVACY_CHANGE,
	MOCA_MPS_REPORT_SUCCESS,
	MOCA_MPS_REPORT_TIMEOUT,
	MOCA_MPS_REPORT_FAIL,
};

typedef struct _MOCA_NODE_INFO
{
	int active;
	int node_id;
	char macaddr[18];
	char moca_ver[8];
	int phyrate[MAX_MOCA_NODES];
	char node_mac[MAX_MOCA_NODES][18];
	char node_moca_ver[MAX_MOCA_NODES][8];
	unsigned char rx_snr[MAX_MOCA_NODES][MAX_MOCA_NUM_CHANNELS];
}MOCA_NODE_INFO;

typedef struct _MOCA_MIB_DATA
{
	unsigned int tx_bytes;
	unsigned int rx_bytes;
	unsigned int tx_packets;
	unsigned int rx_packets;
	unsigned int rx_crc;
}MOCA_MIB_DATA;
#endif

// the following definition is for wans_cap
#define WANSCAP_DSL	0x01
#define WANSCAP_WAN	0x02
#define WANSCAP_LAN	0x04
#define WANSCAP_2G	0x08
#define WANSCAP_5G	0x10
#define WANSCAP_USB	0x20
#define WANSCAP_WAN2	0x40
#define WANSCAP_SFPP	0x80	/* SFP+ */
#define WANSCAP_6G	0x100

// the following definition is for wans_dualwan
#define WANS_DUALWAN_IF_NONE    0
#define WANS_DUALWAN_IF_DSL     1
#define WANS_DUALWAN_IF_WAN     2
#define WANS_DUALWAN_IF_LAN     3
#define WANS_DUALWAN_IF_USB     4
#define WANS_DUALWAN_IF_2G      5
#define WANS_DUALWAN_IF_5G      6
#define WANS_DUALWAN_IF_WAN2	7
#define WANS_DUALWAN_IF_USB2    8
#define WANS_DUALWAN_IF_SFPP	9

#ifdef RTCONFIG_MULTIWAN_IF
#define MULTI_WAN_START_IDX 50
//#define MAX_MULTI_WAN_NUM //defined by MTWANIF
#ifdef RTCONFIG_MULTIWAN_PROFILE
#define MAX_MTWAN_PROFILE_NUM	4
#endif
#endif

// the following definition is for free_caches()
#define FREE_MEM_NONE  "0"	/* kernel < v2.6.39 */
#define FREE_MEM_PAGE  "1"
#define FREE_MEM_INODE "2"
#define FREE_MEM_ALL   "3"

#ifdef RTCONFIG_DEFAULT_REPEATER_MODE
#define sw_mode()            (nvram_get("sw_mode") ? nvram_get_int("sw_mode") : SW_MODE_REPEATER)
#else
#define sw_mode()            (nvram_get("sw_mode") ? nvram_get_int("sw_mode") : atoi(nvram_default_get("sw_mode")))
#endif
#define is_routing_enabled() (sw_mode()==SW_MODE_ROUTER||sw_mode()==SW_MODE_HOTSPOT)
#define is_router_mode()     (sw_mode()==SW_MODE_ROUTER)
#if defined(RTCONFIG_DUALWAN)
extern int is_nat_enabled(void);
#else
#define is_nat_enabled()     ((sw_mode()==SW_MODE_ROUTER||sw_mode()==SW_MODE_HOTSPOT)&&nvram_get_int("wan0_nat_x")==1)
#endif
#define is_lan_connected()   (nvram_get_int("lan_state")==LAN_STATE_CONNECTED)
#ifdef RTCONFIG_WIRELESSWAN
#define is_wirelesswan_enabled() (sw_mode()==SW_MODE_HOTSPOT)
#endif
// todo: multiple wan

extern int wan_primary_ifunit(void);
#ifdef RTCONFIG_REALTEK
/* The fuction is avoiding watchdog segfault on RP-AC68U.
 * This is a workaround solution.
**/
extern int rtk_wan_primary_ifunit(void);
#endif
extern int wan_primary_ifunit_ipv6(void);
extern int get_wan_state(int unit);
extern int get_wan_sbstate(int unit);
extern int get_wan_auxstate(int unit);
extern char *link_wan_nvname(int unit, char *buf, int size);
extern int is_internet_connect(int unit);
extern int is_wan_connect(int unit);
extern int is_phy_connect(int unit);
extern int is_phy_connect2(int unit);
extern int is_ip_conflict(int unit);
extern int get_wan_unit(char *ifname);
extern char *get_wan_ifname(int unit);
#ifdef RTCONFIG_IPV6
extern char *get_wan6_ifname(int unit);
extern int get_wan6_unit(char* ifname);
#endif
extern int get_ports_status(unsigned int port_status);
extern int get_wanports_status(int wan_unit);
extern char *get_usb_ehci_port(int port);
extern char *get_usb_ohci_port(int port);
extern int get_usb_port_number(const char *usb_port);
extern int get_usb_port_host(const char *usb_port);
extern void set_wanscap_support(char *feature);
#ifdef RTCONFIG_DUALWAN
extern void add_wanscap_support(char *feature);
extern int get_wans_cap(void);
extern int get_wans_dualwan_str(char *wancaps, int size);
extern int get_wans_dualwan(void);
extern int get_dualwan_by_unit(int unit);
extern int get_wanunit_by_type(int wan_type);
extern char *get_wantype_str_by_unit(int unit);
extern int get_dualwan_primary(void);
extern int get_dualwan_secondary(void);
#else
static inline int get_wanunit_by_type(int wan_type){
#ifdef RTCONFIG_USB_MODEM
	if(wan_type == WANS_DUALWAN_IF_USB)
		return WAN_UNIT_SECOND;
	else
#endif
		return WAN_UNIT_FIRST;
}
#endif

char *usb_modem_prefix(int modem_unit, char *prefix, int size);
extern int get_modemunit_by_dev(const char *dev);
extern int get_modemunit_by_node(const char *usb_node);
extern int get_modemunit_by_type(int wan_type);
extern int get_wantype_by_modemunit(int modem_unit);

extern char *get_userdns_r(const char *prefix, char *buf, size_t buflen);

extern int asus_ctrl_en(int cid);

int is_bridged(const char *brif, const char *ifname);
int del_from_bridge(const char* ifname);
#ifdef RTCONFIG_BROOP
int netlink_broop(char ctrl, int val);
int detect_broop();
#endif

#ifdef RTCONFIG_MULTI_PPP
int is_mtppp_base_unit(int wan_unit);
int is_mtppp_unit(int wan_unit);
int get_mtppp_base_unit(int wan_unit);
int get_mtppp_wan_unit(int base_wan_unit, int idx);
int get_mtppp_idx_by_wan_unit(int wan_unit);
#endif

#ifdef RTCONFIG_MULTISERVICE_WAN
int is_ms_base_unit(int wan_unit);
int is_ms_wan_unit(int wan_unit);
int get_ms_base_unit(int wan_unit);
int get_ms_wan_unit(int base_wan_unit, int idx);
int get_ms_idx_by_wan_unit(int wan_unit);
#endif

#endif	/* !__RTSTATE_H__ */
