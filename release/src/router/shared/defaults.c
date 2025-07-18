/* nvram table for asuswrt, version 0.1 */

#include <epivers.h>
#include <typedefs.h>
#include <string.h>
#include <ctype.h>
#include <bcmnvram.h>
#include <wlioctl.h>
#include <stdio.h>
#include <shared.h>
#include <version.h>
#include <shutils.h>

#define XSTR(s) STR(s)
#define STR(s) #s

#if defined(RTCONFIG_OPENVPN) && defined(HND_ROUTER) && !defined(HND_ROUTER_AX_675X)
#define DEFAULT_NCP_CIPHERS "AES-256-GCM:AES-128-GCM:AES-256-CBC:AES-128-CBC:CHACHA20-POLY1305"
#else
#define DEFAULT_NCP_CIPHERS "CHACHA20-POLY1305:AES-128-GCM:AES-256-GCM:AES-128-CBC:AES-256-CBC"
#endif

// stub for wlconf, etc.
struct nvram_tuple router_defaults[] = {
	// NVRAM for restore_defaults: system wide parameters
	{ "nvramver", RTCONFIG_NVRAM_VER, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "restore_defaults", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Set to 0 to not restore defaults on boot */
#if defined(RTCONFIG_BCM_MFG) && (defined(RTAX82_XD6S) || defined(GT10) || defined(RTBE88U) || defined(TUFBE3600) || defined(RTBE58U) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(GT7))
	{ "sw_mode", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* big switch for different mode */
#elif defined(RPAC66) || defined(RPAC87) || defined(RPAC51) || defined(RTCONFIG_DEFAULT_REPEATER_MODE)
	{ "sw_mode", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* big switch for different mode */
#elif defined(RTCONFIG_DEFAULT_AP_MODE)
	{ "sw_mode", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* big switch for different mode */
#else
	{ "sw_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* big switch for different mode */
#endif
#ifdef RTCONFIG_DEMOUI
	{ "ASUS_NEW_EULA", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ASUS_NEW_EULA_time", "demo", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "ASUS_EULA", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ASUS_NEW_EULA", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "preferred_lang", "EN", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM from init_nvram: system wide parameters accodring to model and mode
	//{ "wan_ifnames", "" },
	//{ "lan_ifnames", "" },
	//{ "lan1_ifnames", "" },
	//{ "vlan_enable", "" },
	//{ "vlan0ports", "" },
	//{ "vlan1ports", "" },
	//{ "vlan2ports", "" },
	/* Guest H/W parameters */

	//Use to do force submit form request
	{ "force_change", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM for switch
	{ "switch_stb_x", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* oleg patch */
#if defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK) || defined(RTCONFIG_REALTEK)
	{ "switch_upstream", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* remember upstream port of IPTV, valid value is subset of valid value of wans_dualwan nvram variable. */
#endif
	{ "switch_wantag", "none", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for IPTV/VoIP case */
	{ "switch_wan0tagid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Wan Port */
	{ "switch_wan0prio", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "switch_wan1tagid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPTV Port */
	{ "switch_wan1prio", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "switch_wan2tagid", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* VoIP Port */
	{ "switch_wan2prio", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iptv_port_settings", "12", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "bcm_snooping", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* bcmmcastctl mode default: 2 */
#endif
	{ "wl_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Virtual Interface Names */
	/* Wireless parameters */
#ifdef CONFIG_BCMWL5
	{ "wl_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Os revision */
#endif
#ifdef RTCONFIG_WIRELESS_SWITCH
	{ "wl_HW_switch", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* siwtch WiFi slash */
#endif
	{ "wl_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Interface name */
	{ "wl_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MAC address */
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#elif defined(RTCONFIG_REALTEK)
	{ "wl_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Current wireless band (a (5 GHz) v(5GHz ac) b (2.4 GHz) or g (2.4 GHz)) */
	{ "wl0_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_phytype", "n", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Current wireless band (a (5 GHz) b (2.4 GHz) or g (2.4 GHz)) */
	{ "wl_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Current core revision */
	{ "wl_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* List of supported wireless bands (e.g. ga) */
	{ "wl_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* List of radio IDs */
#endif
	{ "w_apply", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},                               /* check wireless be applied */
	{ "wl_ssid", SSID_PREFIX, CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Service set ID (network name) */
#ifdef RTCONFIG_CONCURRENTREPEATER
	{ "wl0_ssid", DEF_SSID_2G, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RPAC92
	{ "wl1_ssid", DEF_SSID_5G, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ssid", DEF_SSID_5G, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_ssid", DEF_SSID_5G, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_RALINK) || defined(RTCONFIG_REALTEK)
	{ "wl1_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1.1_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1.1_channel", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0.1_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0.1_channel", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
#ifndef RTCONFIG_HAS_5G_2
	{ "wl1_ssid", SSID_PREFIX "_5G", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_ssid", SSID_PREFIX "_5G-1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ssid", SSID_PREFIX "_5G-2", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif	// RTCONFIG_CONCURRENTREPEATER

	{ "wl_bss_enabled", "1", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Service set Enable (1) or disable (0) radio */
	{ "wl_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Bandwidth Limiter of Guest Network set Enable (1) or disable (0) radio */
	{ "wl_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Bandwidth Limiter DownStream */
	{ "wl_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Bandwidth Limiter UpStream */

#if defined(RTCONFIG_HSPOT) && defined(RTCONFIG_HND_ROUTER_AX)
	{ "wl_hsflag", "1aa0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* Passpoint Flags */
	{ "wl_hs2cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* Passpoint Realese 2 (1) */
	{ "wl_opercls", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* Operating Class */
	{ "wl_anonai", "anonymous.com", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Anonymous NAI */
	{ "wl_wanmetrics", "1:0:0=2500>384=0>0=0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* WAN Metrics */
	{ "wl_oplist", "Wi-Fi Alliance!eng|\x57\x69\x2d\x46\x69\xe8\x81\x94\xe7\x9b\x9f!chi", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* Operator Friendly Name List */
	/* NAIHomeRealmQueryList */
	{ "wl_homeqlist", "mail.example.com:rfc4282", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* NAIHomeRealmQueryList */
	{ "wl_osu_ssid", "OSU", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* OSU SSID */

	/* OSU Friendly Name */
	{ "wl_osu_frndname", "SP Red Test Only!eng|\x53\x50\x20\xEB\xB9\xA8\xEA\xB0\x95\x20\xED\x85\x8C\xEC\x8A\xA4\xED\x8A\xB8\x20\xEC\xA0\x84\xEC\x9A\xA9!kor", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* OSU Friendly Name */
	{ "wl_osu_uri", "https://osu-server.r2-testbed.wi-fi.org/", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* OSU Server URI */
	{ "wl_osu_nai", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU NAI */
	{ "wl_osu_method", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Method */
	{ "wl_osu_icons", "icon_red_zxx.png+icon_red_eng.png", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Icons */

	/* OSU Serv Desc */
	{ "wl_osu_servdesc", "Free service for test purpose!eng|\xED\x85\x8C\xEC\x8A\xA4\xED\x8A\xB8\x20\xEB\xAA\xA9\xEC\xA0\x81\xEC\x9C\xBC\xEB\xA1\x9C\x20\xEB\xAC\xB4\xEB\xA3\x8C\x20\xEC\x84\x9C\xEB\xB9\x84\xEC\x8A\xA4!kor", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* OSU Serv Desc */
	{ "wl_concaplist", "1:0:0;6:20:1;6:22:0;6:80:1;6:443:1;6:1723:0;6:5060:0;17:500:1;17:5060:0;17:4500:1;50:0:1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* Connection Capability List */

	/* QoS Map IE */
	{ "wl_qosmapie", "35021606+8,15;0,7;255,255;16,31;32,39;255,255;40,47;255,255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* ---- Passpoint Flag ----------------------------------- */
	{ "wl_gascbdel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* GAS CB Delay */
	{ "hs2_debug_level", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* HS2.0 debeug level default (1) ERROR */

/* #ifdef __CONFIG_802_11U__ */
	{ "wl_iwnettype", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* Select Access Network Type */
	{ "wl_hessid", "50:6F:9A:00:11:22", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* Interworking HESSID */
	{ "wl_ipv4addr", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select IPV4 Address Type Availability */
	{ "wl_ipv6addr", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select IPV6 Address Type Availability */
	{ "wl_netauthlist", "accepttc=+" "httpred=httpred=https://tandc-server.wi-fi.org", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network Authentication Type List */
	{ "wl_venuegrp", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Name List */
	{ "wl_venuetype", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Type */
	{ "wl_venuelist", "57692D466920416C6C69616E63650A"
	    		  "3239383920436F7070657220526F61640A"
			  "53616E746120436C6172612C2043412039"
			  "353035312C2055534121656E677C"
			  "57692D4669E88194E79B9FE5AE9EE9AA8CE5AEA40A"
			  "E4BA8CE4B99DE585ABE4B99DE5B9B4E5BA93E69F8FE8B7AF0A"
			  "E59CA3E5858BE68B89E68B892C20E58AA0E588A9E7A68FE5B0"
			  "BCE4BA9A39353035312C20E7BE8EE59BBD21636869", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Name List */

	/* Roaming Consortium List */
	{ "wl_ouilist", "506F9A:1;001BC504BD:1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Roaming Consortium List */
	{ "wl_3gpplist", "310:260;310:310", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 3GPP Cellular Network Information List */
	{ "wl_domainlist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Domain Name List */
	{ "wl_realmlist", "mail.example.com+0+21=2,4#5,7?"
	    		 "cisco.com+0+21=2,4#5,7?"
			 "wi-fi.org+0+21=2,4#5,7;13=5,6?"
			 "example.com+0+13=5,6", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* NAI Realm List */
//#endif /* __CONFIG_802_11U__ */
#else
#ifdef __CONFIG_HSPOT__	/* See "default_get" below. */
	{ "wl_hsflag", "3159", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Passpoint Flags */
	{ "wl_hs2en", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Passpoint Enable (1) */
	{ "wl_hs2cap", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Passpoint Realese 2 (1) */
	{ "wl_opercls", "3", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Operating Class */
	{ "wl_anonai", "anonymous.com", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Anonymous NAI */

	{ "wl_oplist", "T-Mobile!eng", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Operator Friendly Name List */

	{ "wl_homeqlist", "t-mobile.com:rfc4282", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* NAIHomeRealmQueryList */
	{ "wl_wanmetrics", "1:0:0=2500>384=0>0=0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN Metrics */
	{ "wl_osu_ssid", "CellSpot_AutoConnect", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU SSID */

	{ "wl_osu_frndname", "T-Mobile!eng", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Friendly Name */

	{ "wl_osu_uri", "https://osu-server.t-mobile.com/", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Server URI */

	{ "wl_osu_nai", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU NAI */
	{ "wl_osu_method", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Method */

	{ "wl_osu_icons", "icon_red_zxx.png+icon_red_eng.png", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Icons */

	{ "wl_osu_servdesc", "T-MobilePasspointservice!eng", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Serv Desc */

	{ "wl_concaplist", "1:0:0;6:20:1;6:22:0;6:80:1;6:443:1;6:1723:0;6:5060:0;17:500:1;17:5060:0;17:4500:1;50:0:1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Connection Capability List */
	{ "wl_qosmapie", "35021606+8,15;0,7;255,255;16,31;32,39;255,255;40,47;255,255", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* QoS Map IE */

	/* ---- Passpoint Flag ----------------------------------- */
	{ "wl_gascbdel", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* GAS CB Delay */
	{ "wl_4framegas", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 4 Frame GAS */

	/* ---- temporary ----------------------------------- */
	{ "wl_osuicon_id", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OSU Provider's Icon ID */
	{ "wl_conn_id", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Connection Capability ID */

//#ifdef __CONFIG_802_11U__
	{ "wl_u11en", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11u IW Enable (1) disable (0) radio */
	{ "wl_iwint", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Internet Enable (1) disable (0) radio */
	{ "wl_iwnettype", "2", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select Access Network Type */
	{ "wl_hessid", "50:6F:9A:00:11:22", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Interworking HESSID */
	{ "wl_ipv4addr", "3", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select IPV4 Address Type Availability */
	{ "wl_ipv6addr", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select IPV6 Address Type Availability */

	{ "wl_netauthlist", "accepttc=+" "httpred=https://t-mobile.com", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network Authentication Type List */

	{ "wl_venuegrp", "2", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Name List */
	{ "wl_venuetype", "8", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Type */

	{ "wl_venuelist", "542D4D6F62696C6521656E67", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Venue Name List */

	{ "wl_ouilist", "506F9A;001BC504BD", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Roaming Consortium List */
	{ "wl_3gpplist", "310:260;310:310", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 3GPP Cellular Network Information List */
	{ "wl_domainlist", "t-mobile.com", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Domain Name List */

	{"wl_realmlist", "wlan.mnc260.mcc310.3gppnetwork.org+0+21=2,4#5,7?"	"wlan.mnc260.mcc310.3gppnetwork.org+0+13=5,6?"	"wlan.mnc310.mcc310.3gppnetwork.org+0+21=2,4#5,7?"	"wlan.mnc310.mcc310.3gppnetwork.org+0+13=5,6", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif /* __CONFIG_802_11U__ */

#endif /* __CONFIG_HSPOT__ */
#endif
#ifdef RTCONFIG_REALTEK
	{ "wl_country_code", DEF_COUNTRY_CODE, CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_HT_GI", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
//	{ "wl_country_code", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Country Code (default obtained from driver) */
#ifdef CONFIG_BCMWL5
	{ "wl_country_rev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Regrev Code (default obtained from driver) */
#endif
	{ "wl_radio", "1", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable (1) or disable (0) radio */
	{ "wl_closed", "0", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Closed (hidden) network */
	{ "wl_ap_isolate", "0", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* AP isolate mode */
#if defined(HND_ROUTER)
	{ "wl_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BCM: wl_wmf_bss_enable Ralink: IGMPSnEnable */
#else
	{ "wl_igs", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BCM: wl_wmf_bss_enable Ralink: IGMPSnEnable */
#endif
#ifdef CONFIG_BCMWL5
	{ "wl_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* WMF Enable/Disable */
	{ "wl_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MCAST REGEN Enable/Disable */

	/* operational capabilities required for stations to associate to the BSS */
#ifdef RTCONFIG_BCMWL6
	{ "wl_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rxchain powersave enable */
	{ "wl_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Quiet time for power save */
	{ "wl_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Packets per second threshold for power save */
	{ "wl_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* STAs associated before powersave */
	{ "wl_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Radio powersave enable */
	{ "wl_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Quiet time for power save */
	{ "wl_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Packets per second threshold for power save */
	{ "wl_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Radio power save level */
	{ "wl_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* STAs associated before powersave */
#endif
	{ "wl_mode", "ap", CKN_STR8, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* AP mode (ap|sta|wds) */
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#else
#if defined(RTCONFIG_BCM9)
	{ "wl_dwds", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* MBSS mode (4|8|16) */
#endif
	{ "wl_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable (lazy) WDS mode (0|1) */
	{ "wl_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* xx:xx:xx:xx:xx:xx ... */
	{ "wl_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WDS link detection interval defualt 1 sec */
	{ "wl_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WEP data encryption (enabled|disabled) */
	{ "wl_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Shared key authentication optional (0) or required (1) */
#endif
	{ "wl_key", "1", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Current WEP key */
	{ "wl_key1", "", CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key2", "", CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key3", "", CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key4", "", CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5/13 char ASCII or 10/26 char hex */
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#else
	{ "wl_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* xx:xx:xx:xx:xx:xx ... */
#endif
	{ "wl_macmode", "disabled", CKN_STR16, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* allow only */
	{ "wl_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Non-zero limit for association retries */
#ifndef RTCONFIG_BCMWL6
	{ "wl_channel", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Channel number */
#else
#ifndef RTCONFIG_BCM_MFG
	{ "wl_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Channel specification */
#if defined(BCM_BSD) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl0_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "wl2_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#else
	{ "wl_chanspec", "11", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Channel specification */
#endif
#endif
#ifdef RTCONFIG_RALINK
	{ "wl_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default or ofdm */
#elif defined(RTCONFIG_QCA)
	{ "wl_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default or ofdm */
#else
	{ "wl_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Regulatory: 802.11H(h)/802.11D(d)/off(off) */
	{ "wl_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rate (bps */
#if 0
	{ "wl_mrate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Mcast Rate (bps */
#endif
#endif
	{ "wl_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Mcast Auto rate */
#if !defined(RTCONFIG_WIFILOGO) && !defined(RTCONFIG_WIFI_QCA9994_QCA9994)
	{ "wl_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BRCM Frambursting mode (off|on) */
	{ "wl_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* BRCM frameburst override to enable/disable
						 * dynamic framebursting default on
						 */
#else
	{ "wl_frameburst", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined (RTCONFIG_BCMARM) && !defined (RTCONFIG_BCM7) && !defined(RTCONFIG_BCM9) && !defined(RTCONFIG_HND_ROUTER_AX)
	{ "frameburst_dyn", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Frameburst controlled dynamically if on */
#endif
	{ "wl_rateset", "default", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default or all or 12 */
	{ "wl_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Fragmentation threshold */
	{ "wl_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* RTS threshold */
#ifdef RTCONFIG_RALINK
	{ "wl_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_QCA)
	{ "wl_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* DTIM period */
#else
	{ "wl_dtim", "3", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DTIM period */
#endif
#endif
	{ "wl_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Beacon interval */
#if defined(CONFIG_BCMWL5)
#if defined(RTCONFIG_HND_ROUTER_AX)
	{ "wl_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Beacon rotation */
#else
	{ "wl_bcn_rotate", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Beacon rotation */
#endif
#endif
	{ "wl_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11b PLCP preamble type */
#ifdef RTCONFIG_RALINK
	{ "wl_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11n protection */
#elif defined(RTCONFIG_QCA) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
	{ "wl_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11n protection */
#else
	{ "wl_gmode", "XSTR(GMODE_AUTO)", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 54g mode */
#endif
	{ "wl_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11g RTS/CTS protection (off|auto) */

#ifdef RTCONFIG_OPTIMIZE_XBOX
	{ "wl_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Optimize WiFi packet for Xbox */
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_wme", "on", CKN_STR4, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME mode (off|on) */
#else
	{ "wl_wme", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME mode (off|on|auto) */
#endif
#ifdef CONFIG_BCMWL5
	{ "wl_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME BSS disable advertising (off|on) */
	{ "wl_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Antenna Diversity (-1|0|1|3) */
	{ "wl_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network Type (BSS/IBSS) */
//	{ "wl_interfmode", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Interference mitigation mode (2/3/4) */
#ifndef RTCONFIG_BCMWL6
	{ "wl_nbw_cap", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BW Cap; def 20inB and 40inA */
#else
	{ "wl_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BW Cap; 20 MHz */
#if defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_BW160M)
	{ "wl_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bandwidth 160MHz support */
#ifdef BCM_BSD
#if defined(BQ16) || defined(BQ16_PRO) || defined(RTBE95U) || defined(GS7_PRO)
	{ "wl0_bw_160", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BM68) || defined(ET8PRO) || defined(ET8_V2) || defined(XC5) || defined(EBA63) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "wl1_bw_160", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G)
#if defined(RTAX95Q) || defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(GTAXE11000) || defined(RTAXE7800) || defined(RTBE92U)
	{ "wl2_bw_160", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl2_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif	/* RTAX95Q, RTAXE95Q */
#endif
#if defined(BQ16)
	{ "wl1_bw_160", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif
#endif
#endif
#if defined(RTCONFIG_BW320M)
	{ "wl_bw_320", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* bandwidth 320MHz support */
#if defined(BCM6813) && defined(RTCONFIG_QUADBAND) && defined(RTCONFIG_HAS_6G_2)
	{ "wl1_bw_320", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_HND_ROUTER_BE_4916) && (defined(RTCONFIG_HAS_6G) || defined(RTCONFIG_QUADBAND))
	{ "wl2_bw_320", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(BT10) || defined(GSBE18000) || defined(GSBE12000) || defined(GT7)
	{ "wl0_bw_320", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	{ "psc6g", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* 6G Prefer Scanning Channel option toggle */
#endif
#ifndef RTCONFIG_BCMWL6
	{ "wl_nctrlsb", "lower", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-CTRL SB */
#endif
#ifdef RTCONFIG_WL_AUTO_CHANNEL
	{ "AUTO_CHANNEL", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: real channel; 1: Auto; for RT-N12HP & RT-N14UHP */
#endif
#ifdef RTCONFIG_TCODE
	{ "location_code", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: real channel; 1: Auto; for RT-N12HP & RT-N14UHP */
	{ "ui_location_code", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_RALINK
	{ "wl_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-BAND */
	{ "wl0_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz */
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz */
#elif defined(RTCONFIG_QCA)
	{ "wl_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-BAND */
	{ "wl0_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz */
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz */
#if defined(RTCONFIG_HAS_5G_2)
#if defined(ETJ)
	{ "wl2_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz */
#else
	{ "wl2_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5G-2 */
#endif
#endif
#if defined(RTCONFIG_WIGIG)
	{ "wl3_nband", "6", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11ad Wigig */
#if defined(GTAXY16000)
	{ "wl_edmg_channel", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 11ay EDMG channel (channel bonding) */
#endif
#endif	/* RTCONFIG_WIGIG */
#else
	{ "wl_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-BAND */
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_QUADBAND)
#if !defined(RTCONFIG_HAS_6G_2)
	{ "wl0_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl2_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz*/
	{ "wl3_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#else
	{ "wl0_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl1_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz-1*/
	{ "wl2_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz-2*/
	{ "wl3_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#endif
#elif defined(GT10) || defined(RTAX9000) || defined(GS7_PRO)
	{ "wl0_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl2_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#elif defined(BT10)
	{ "wl0_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz*/
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl2_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#elif defined(RTBE95U)
	{ "wl0_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl1_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz*/
	{ "wl2_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GT7)
	{ "wl0_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz*/
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz*/
	{ "wl2_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz*/
#else
	{ "wl0_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4 GHz */
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 5 GHz */
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	{ "wl2_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 6 GHz */
#else
	{ "wl2_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 5 GHz */
#endif
#endif
#endif /* #if defined(GTAXE16000)*/

	{ "wl_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MCS Index for N - rate */
	{ "wl_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-mode */
	{ "wl_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* RIFS mode advertisement */
	{ "wl_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* VLAN Priority support */
#ifdef CONFIG_BCMWL5
	{ "wl_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bluetooth coexistance(BTC) mode Disable(0) */
#endif
	{ "wl_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 100% duty cycle for LED on router */
	{ "wl_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11n Rx Streams */
	{ "wl_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 802.11n Tx Streams 0 */
	{ "wl_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default STBC TX setting */
	{ "wl_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default STBC RX setting */
	{ "wl_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default AMPDU setting */
#ifndef RTCONFIG_BCMWL6
	{ "wl_nreqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Require 802.11n support */
#endif
#endif	/* RTCONFIG_RALINK */
#ifdef RTCONFIG_RALINK
	{ "wl_mimo_preamble", "mm", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_QCA)
	{ "wl_mimo_preamble", "mm", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_BCMWL6)
//	{ "wl_mimo_preamble", "gfbcm", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Mimo PrEamble: mm/gf/auto/gfbcm */
#endif
#ifdef CONFIG_BCMWL5
#if defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM9) || defined(HND_ROUTER)
	{ "wl_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	/* Default AMPDU retry limit per-tid setting */
	{ "wl_ampdu_rtylimit_tid", "7 7 7 7 7 7 7 7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* Default AMPDU regular rate retry limit per-tid setting */
	{ "wl_ampdu_rr_rtylimit_tid", "3 3 3 3 3 3 3 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_BCMARM) && !defined(RTCONFIG_BCM7) && !defined(RTCONFIG_BCM_7114) && !defined(RTCONFIG_BCM9) && !defined(HND_ROUTER)
	{ "wl_rx_amsdu_in_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default RX AMSDU In AMPDU setting */
#endif
	{ "wl_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default OBSS Coexistence setting - OFF */
#ifdef RTCONFIG_BCMWL6
#if !defined(RTCONFIG_BCM7) && !defined(RTCONFIG_BCM_7114) && !defined(RTCONFIG_BCM9) && !defined(HND_ROUTER)
	{ "wl_ack_ratio", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BCMARM
#if defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
#ifdef RTAX92U
	{ "wl0_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_turbo_qam", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if !defined(RTCONFIG_BCM9) && !defined(RTAC56U) && !defined(RTAC56S)
	{ "wl0_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif	// RTCONFIG_BCMARM
#endif	// RTCONFIG_BCMWL6
#endif	// CONFIG_BCMWL5
#if defined(CONFIG_BCMWL5) || defined(RTCONFIG_REALTEK)
	{ "wl_amsdu", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default AMSDU setting */
#endif
	/* WPA parameters */
	{ "wl_auth_mode", "none", CKN_STR8, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network authentication mode (radius|none) */
	{ "wl_wpa_psk", "", CKN_STR64, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WPA pre-shared key */
	{ "wl_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* GTK rotation interval */
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PTK rotation interval */
#endif
	{ "wl_radius_ipaddr", "", CKN_STR64, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* RADIUS server IP address */
	{ "wl_radius_key", "", CKN_STR64, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* RADIUS shared secret */
	{ "wl_radius_port", "1812", CKN_STR8, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* RADIUS server UDP port */
#if defined(RTCONFIG_MULTILAN_CFG) //defined(RTCONFIG_BRCM_HOSTAPD)
	{ "wl_radius_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius_acct_port", "1813", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_port", "1812", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius2_acct_port", "1813", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_crypto", "tkip+aes", CKN_STR16, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WPA data encryption */
#if 0
	{ "wl_net_reauth", "3600", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network Re-auth/PMK caching duration */
#else
	{ "wl_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Network Re-auth/PMK caching duration in minutes */
#endif
	{ "wl_akm", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WPA akm list */
#if defined(RTCONFIG_BCMWL6) || defined(RTCONFIG_QCA) || defined(RTCONFIG_RALINK)
#if defined(MFP) || defined(RTCONFIG_MFP) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl_mfp", "0", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Protected Management Frame */
	{ "wl0_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* Protected Management Frame */
	{ "wl1_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* Protected Management Frame */
	{ "wl2_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* Protected Management Frame */
	{ "wl0.1_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Protected Management Frame */
#ifdef RTCONFIG_HAS_5G
	{ "wl1.1_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Protected Management Frame */
#endif
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "wl2.1_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Protected Management Frame */
#endif
#endif
	{ "wl_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default to one level repeating mode */
#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "wl_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default to disable PSR guest support */
#endif
#endif
#ifdef RTCONFIG_WPS
	/* WSC parameters */
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#else
	{ "wps_version2", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Must specified before other wps variables */
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wps_config_method", "0x784", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* config wps methods as Label PIN, Virtual Push Button, Physical Push Button, Keypad*/
#endif
	{ "wl_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enabled wps */
	{ "wl_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* config state unconfiged */
#endif
//	{ "wps_device_pin", "12345670", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// it is mapped to secret_code
#if 0
	{ "wps_modelname", "RT_BUILD_NAME", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wps_modelname", "Wi-Fi Protected Setup Router", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wps_mfstring", "ASUSTeK Computer Inc.", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "wps_device_name", RT_BUILD_NAME, 0 },
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#else
	{ "wl_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wps_sta_pin", "00000000", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "wps_modelnum", RT_BUILD_NAME},
	/* Allow or Deny Wireless External Registrar get or configure AP security settings */
	{ "wps_wer_mode", "allow", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "lan_wps_oob", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OOB state */
	{ "lan_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* For DTM 1.4 test */

	{ "lan1_wps_oob", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan1_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WPS_LED
	{ "wps_success", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* after 3 seconds under WPS success */
#endif
#endif /* __CONFIG_WPS__ */
#ifdef __CONFIG_WFI__
	{ "wl_wfi_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
	{ "wl_wfi_pinmode", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: auto pin */
#endif /* __CONFIG_WFI__ */
#ifdef __CONFIG_WAPI_IAS__
	/* WAPI parameters */
	{ "wl_wai_cert_name", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* AP certificate name */
	{ "wl_wai_cert_index", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* AP certificate index. 1:X.509 */
	{ "wl_wai_cert_status", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* AP certificate status */
	{ "wl_wai_as_ip", "", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* ASU server IP address */
	{ "wl_wai_as_port", "3810", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* ASU server UDP port */
#endif /* __CONFIG_WAPI_IAS__ */
#ifdef CONFIG_BCMWL5
	/* WME parameters (cwmin cwmax aifsn txop_b txop_ag adm_control oldest_first) */
	/* EDCA parameters for STA */
	{ "wl_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME STA AC_BE parameters */
	{ "wl_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME STA AC_BK parameters */
	{ "wl_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME STA AC_VI parameters */
	{ "wl_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME STA AC_VO parameters */

	/* EDCA parameters for AP */
	{ "wl_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME AP AC_BE parameters */
	{ "wl_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME AP AC_BK parameters */
	{ "wl_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME AP AC_VI parameters */
	{ "wl_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME AP AC_VO parameters */
#endif
	{ "wl_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME No-Acknowledgment mode */
	{ "wl_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WME APSD mode */
#ifdef RTCONFIG_RALINK
#elif defined(RTCONFIG_QCA)
#else
	{ "wl_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Max associations driver could support */
	{ "wl_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Max associations driver could support */

	{ "wl_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Seconds between association attempts */
#endif
#ifdef BCMDBG
	{ "wl_nas_dbg", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable/Disable NAS Debugging messages */
#endif
	// ASUS used only?
	{ "wl_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2 */
#if defined(RTCONFIG_BCMWL6)
#ifndef RTCONFIG_BCM_MFG
	{ "wl_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2/3 auto/20/40/80MHz */
#ifdef BCM_BSD
	{ "wl0_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* 0/1/2/3 auto/20/40/80MHz */
	{ "wl1_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* 0/1/2/3 auto/20/40/80MHz */
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "wl2_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* 0/1/2/3 auto/20/40/80MHz */
#endif
#endif
#else
	{ "wl_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* 0/1/2/3 auto/20/40/80MHz */
#endif
#ifdef RTCONFIG_QTN
	{ "wl1_bw", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2/3 auto/20/40/80MHz */
#endif
#else
#ifdef RTCONFIG_LANTIQ
	{ "wl_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2/3 auto/20/40/80MHz */
#else
	/* 2G/5G/5G2
	 * 	0/1/2/3/4/5: 20/auto/40/80/80+80/160MHz
	 * 60G, Wigig
	 * 	0: auto
	 * 	6: 2.16GHz (11ad, one channel)
	 * 	7: 4.32GHz (11ay, two channel)
	 * 	8: 6.48GHz (11ay, three channel)
	 * 	9: 8.64GHz (11ay, four channel)
	 */
	{ "wl_bw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "wl_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* default using enhanced open authentication mode for 6G band */
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
#if WL_UNIT_6G == 0 || WL_UNIT_6G_2 == 0
	{ "wl0_auth_mode_x", "owe", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if WL_UNIT_6G == 1 || WL_UNIT_6G_2 == 1
	{ "wl1_auth_mode_x", "owe", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if WL_UNIT_6G == 2 || WL_UNIT_6G_2 == 2
	{ "wl2_auth_mode_x", "owe", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if WL_UNIT_6G == 3 || WL_UNIT_6G_2 == 3
	{ "wl3_auth_mode_x", "owe", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

// open/shared/psk/wpa/radius
	{ "wl_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
// WEP data encryption 0, 1, 2 : disabled/5/13
	{ "wl_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sched", "000000", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_WL_SCHED_V2) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Passphrase */
	{ "wl_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* Total number of clients of a guest network.
	 * 1 ~ maxsta:	1 ~ maxsta
	 * otherwise:	do nothing
	 */
	{ "wl_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_AMAS_WGN) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0:router only, 1:all */
#endif	/* RTCONFIG_AMAS_WGN */
#if defined(RTCONFIG_MULTILAN_CFG)
	{ "wl_iot_max_cmpt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sched_v2", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_ssid", SSID_PREFIX, CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_hide_ssid", "0", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_security", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_bw_limit", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_sched", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_expiretime", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_maclist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_iot_max_cmpt", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_dut_list", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg_mlo", "0", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apg_disabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WIFI7
    { "apg_11be", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif  // RTCONFIG_WIFI7
#if defined(RTCONFIG_MULTILAN_MWL)
    { "apm_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_ssid", SSID_PREFIX, CKN_STR32, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_hide_ssid", "0", CKN_STR1, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_security", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_bw_limit", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_sched", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_expiretime", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_maclist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_iot_max_cmpt", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_dut_list", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "apm_mlo", "0", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apm_disabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	
#ifdef RTCONFIG_WIFI7
	{ "apm_11be", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif  // RTCONFIG_WIFI7
#endif  // RTCONFIG_MULTILAN_MWL                                                       
	{ "vlan_trunklist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ap_wifi_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ap_lanif_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vlan_trunk_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_expiretime", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mlo_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DNSPRIVACY
	{ "dot_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif	// RTCONFIG_MULTILAN_CFG
#if defined(RTCONFIG_MLO)
//#if defined(RPBE58)
//	{ "mld0_ifnames", "wl1 wl2", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#else
	{ "mld0_ifnames", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif
	{ "mld1_ifnames", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mld2_ifnames", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MLO_DEFAULT_ACTIVE)
	{ "mld_enable", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "mld_enable", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_MLO_BH_BAND)
#if defined(RTCONFIG_MLO_CONFIG_566)
	{ "mlo_bh_band", "98", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_MLO_CONFIG_556)
	{ "mlo_bh_band", "28", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "mlo_bh_band", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif // RTCONFIG_MLO_BH_BAND
#endif //RTCONFIG_MLO
#if defined(RTCONFIG_SMARTHAUL)
	{ "smart_haul_enable", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined (RTCONFIG_RALINK) || defined (RTCONFIG_BCMWL6) || defined (RTCONFIG_REALTEK)
#if defined(RTAC53U) || defined(RTAC1200G) || defined(RTAC1200GP) || defined(RPAC87) || defined(RPAC53)
	/* RT-AC53U disable txbf by default */
	{ "wl_txbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_QTN
	{ "wl1_80211h", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif
#if defined(RTCONFIG_BCMWL6)
#if defined(RTCONFIG_BCMARM)
#if defined(RTCONFIG_WIFI7)
	{ "wl_itxbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	
#else
	{ "wl_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#elif defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	{ "wl_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_RALINK_MT7622)
	{ "wl_itxbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
#ifdef RTAX92U
	{ "wl0_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
	{ "wl0_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_WIFI7)
#if defined(RTCONFIG_QUADBAND)
        { "wl3_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 2.4G MU_FEATURES OFF */
#elif defined(BT10)
        { "wl2_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 2.4G MU_FEATURES OFF */
#elif defined(RTBE95U) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
        { "wl2_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 2.4G MU_FEATURES OFF */
#else
        { "wl0_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 2.4G MU_FEATURES OFF */
#endif
#endif // RTCONFIG_WIFI7
	{ "wl_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_QTN) || defined(RTCONFIG_MUMIMO_2G) || defined(RTCONFIG_MUMIMO_5G)
	{ "wl_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_LANTIQ)
	{ "wl1_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_REALTEK) && defined(RTCONFIG_MUMIMO_5G)
	{ "wl1_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_QCA)
#if defined(RTCONFIG_WIFI_QCA9990_QCA9990) || \
    defined(RTCONFIG_WIFI_QCA9994_QCA9994) || \
    defined(RTCONFIG_WIFI_QCN5024_QCN5054) || \
    defined(RTCONFIG_QCA_AXCHIP) || \
    defined(RTCONFIG_SOC_IPQ40XX)
	{ "wl_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_turbo_qam_brcm_intop", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RT4GAC53U)
	{ "wl_txbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_itxbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MUMIMO_2G)
	{ "wl0_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_MUMIMO_5G)
	{ "wl1_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	/* Improve STA to AP direction throughput. */
	{ "wl_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "restwifi_qis", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_PCIE_QCA9888) && (defined(RTCONFIG_QCA953X) || \
					 defined(RTCONFIG_SOC_QCA9557) || \
					 defined(RTCONFIG_QCA956X) || \
					 defined(RTCONFIG_QCN550X))
	{ "wl_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_itxbf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	{ "wl_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax support */
	{ "wl_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_SOC_IPQ60XX)
	{ "wl_ofdma", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support */
#elif defined(RTCONFIG_SOC_IPQ50XX)
	{ "wl_ofdma", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support */
	{ "wl0_ofdma", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support */
	{ "wl1_ofdma", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support + HE MU-MIMO */
#else
	{ "wl_ofdma", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support */
	{ "wl0_ofdma", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support */
	{ "wl1_ofdma", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax downlink/uplink OFDMA support + HE MU-MIMO */
	/* Extended NSS:
	 * OFF: Make Extended NSS unaware client, e.g., Intel ac/ax Wireless adapter, use 160MHz to send packet. May cause IoT issue with another client, e.g., Huawei cellphone.
	 * ON: Extended NSS unaware client, e.g., Intel ac/ax Wireless adapter, will use 80MHz to send packet due to 160MHz in VHT Capabilities IE of beaconframe is zero,
	 *     but it's defined by 802.11ac 2016.
	 */
	{ "wl_ext_nss", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Extended NSS, 5G 160MHz only */
	{ "wl_precacen", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* preCACEn, Agile DFS */
#endif
#endif
#if defined(RTCONFIG_BW160M) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bandwidth 160MHz support */
#endif
#endif
#if defined(RTCONFIG_RALINK)
#if defined(RTCONFIG_BW160M)
	{ "wl_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bandwidth 160MHz support */
#endif
#endif

#if defined (RTCONFIG_WIRELESSREPEATER) || defined (RTCONFIG_PROXYSTA)
#if defined (RTCONFIG_CONCURRENTREPEATER)
	/* 2.4GHz */
	{ "wlc0_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wep", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_auth_mode", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_sbstate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_scan_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_ure_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_hide_pap", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wifipxy", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 5GHz */
	{ "wlc1_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wep", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_auth_mode", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_sbstate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_scan_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_ure_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_hide_pap", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wifipxy", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RPAC92)
	{ "wlc2_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wep", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_auth_mode", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_sbstate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_scan_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_ure_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_hide_pap", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wifipxy", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifndef RTCONFIG_BCMWL6
	{ "wlc0_nbw_cap", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_nbw_cap", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wlc0_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
	{ "wlc_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_band", "", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_wep", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_auth_mode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_crypto", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_LANTIQ
	{ "wlc_ap_mac", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifndef RTCONFIG_BCMWL6
	{ "wlc_nbw_cap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wlc_bw_cap", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wlc_ure_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_WIFI7
	{ "wlc_11be", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_WIFI7) && defined(RTCONFIG_MLO) && defined(RTCONFIG_HND_ROUTER_AX)
//#if defined(RPBE58)
//	{ "mlo_rp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#else
	{ "mlo_rp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif
	{ "mlo_mb", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_dpsta", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_AMAS
	{ "wlc0_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wep", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_auth_mode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WIFI7
	{ "wlc0_11be", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MULTILAN_CFG
	{ "wlc0_radius_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc0_radius2_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wlc1_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wep", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_auth_mode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WIFI7
	{ "wlc1_11be", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MULTILAN_CFG
	{ "wlc1_radius_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc1_radius2_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "wlc2_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wep", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_key", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wep_key", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_auth_mode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WIFI7
	{ "wlc2_11be", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MULTILAN_CFG
	{ "wlc2_radius_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_acct_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_acct_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc2_radius2_acct_port", "1813", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wlc_stopconn_count", "1800", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "wlc_express", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
#endif
#if defined(RTCONFIG_PROXYSTA) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
#if defined(RPAX58) || defined(RPBE58)
	{ "wlc_psta", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
#else
	{ "wlc_psta", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
#endif
#if defined(RTCONFIG_DPSTA) || defined(RTCONFIG_DPSR) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
#if defined(RPAX58) || defined(RPBE58)
	{ "wlc_dpsta", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wlc_dpsta", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif

	// WPA parameters
#ifdef RTCONFIG_RALINK
	{ "wl_PktAggregate", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* UI configurable */
	{ "wl_HT_OpMode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* UI configurable */
#if defined(RTAC85U) || defined(RTAC85P) || defined(RTAC65U) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750)
	{ "wl_DLSCapable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* UI configurable */
#else
	{ "wl_DLSCapable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* UI configurable */
#endif
	{ "wl_GreenAP", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* UI configurable */
	{ "wl_HT_AutoBA", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_HT_HTC", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_HT_RDG", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_HT_LinkAdapt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MT798X)
	{ "wl_HT_MpduDensity", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_HT_MpduDensity", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTAC85U) || defined(RTAC85P) || defined(RPAC87) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18)|| defined(RT4GAC86U) || defined(RT4GAX56) || defined(RTCONFIG_MT798X)
	{ "wl_HT_AMSDU", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_HT_AMSDU", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_HT_GI", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	{ "wl_HT_BAWinSize", "256", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_HT_BAWinSize", "64", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_HT_MCS", "33", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_HT_BADecline", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "wl_HT_TxStream", "2" },
//	{ "wl_HT_RxStream", "3" },
//	{ "wl0_HT_TxStream", "2" }, // move to init_nvram for model dep.
//	{ "wl0_HT_RxStream", "2" }, // move to init_nvram for model dep.
//	{ "wl1_HT_TxStream", "2" }, // move to init_nvram for model dep.
//	{ "wl1_HT_RxStream", "3" }, // move to init_nvram for model dep.
	{ "wl_HT_STBC", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// the following for ralink 5g only
	{ "wl_IEEE80211H", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_CSPeriod", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_RDRegion", "FCC", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_CarrierDetect", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ChannelGeography", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MUMIMO_2G) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "wl0_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_MUMIMO_5G)
	{ "wl1_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_CONCURRENTREPEATER)
	{ "wl1.1_mumimo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_QAM256_2G) || defined(RTCONFIG_QAM1024_5G)
#if defined(RTCONFIG_QAM256_2G)
	{ "wl0_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_CONCURRENTREPEATER)
	{ "wl0.1_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_QAM1024_5G)
	{ "wl1_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_CONCURRENTREPEATER)
	{ "wl1.1_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "wl_turbo_qam", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_MT798X)
	{ "wl_turbo_qam", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTAC85U) || defined(RTAC85P) || defined(RTN800HP) || defined(RTACRH26) || defined(TUFAC1750) || defined(RT4GAC86U) || defined(RTACRH18) || defined(RT4GAX56) || defined(RTAX54)
	{ "wl_txbf_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_txbf_en", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_QCA)
	{ "wl_HT_GI", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Short-GI */
	{ "wl_HT_STBC", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* STBC */
#endif

	{ "emf_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* it is common entry for all platform now broadcom: enable mrate */
#if defined(RTCONFIG_EMF)
	{ "emf_entry", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Static MFDB entry (mgrp:if) */
	{ "emf_uffp_entry", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Unreg frames forwarding ports */
	{ "emf_rtport_entry", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IGMP frames forwarding ports */
#ifdef RTCONFIG_BCMWL6
	{ "wl_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable Converting IGMP Query to ucast (default) */
	{ "wl_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable Sending Multicast Data to host (default) */
#ifdef RTCONFIG_BCMARM
	{ "wl_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable Converting upnp to ucast (default) */
	{ "wl_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable igmp query filter */
#endif
#endif
#endif

// WPS
	{ "wps_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_enable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_RALINK
	{ "wl_wsc_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* config state unconfiged */
#endif
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_QUADBAND) && (defined(BCM4912) || defined(BCM6813))
	{ "wps_band_x", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* 3: 2.4G */
#elif defined(GT10) || defined(RTAX9000) || defined(RTBE95U) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "wps_band_x", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2: 2.4G */
#elif defined(BT10)
	{ "wps_band_x", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2: 2.4G */
#else
	{ "wps_band_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: 2.4G */
#endif
#if defined(RTCONFIG_WPSMULTIBAND)
	{ "wps_multiband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable WPS on 2.4G and 5G both */
#else
	{ "wps_multiband", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

// Wireless WDS Mode
	{ "wl_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2(ap/wds/hybrid) */
	{ "wl_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* xxxxxxxxxxxx... */

// Wireless Mac Filter
	{ "wl_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* xxxxxxxxxxxx...xxxxxxxxxxx */

#if defined(RTCONFIG_WIFI_QCA9994_QCA9994)
	{ "wl_hwol", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, 			/* Hardware WiFi Offloading */
#elif defined(RTCONFIG_WIFI_QCN5024_QCN5054) || defined(RTCONFIG_QCA_AXCHIP)
	{ "wl_hwol", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, 			/* Hardware WiFi Offloading */
#endif
	{ "acs_band1", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(XT8PRO) || defined(BT12) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68)
	/* used for RTCONFIG_AMAS_ADTBW */
	{ "acs_band3", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "acs_band3", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "acs_ch13", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(XD4PRO)
	{ "acs_dfs", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable DFS channels for acsd by default */
#else
	{ "acs_dfs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable DFS channels for acsd by default */
#endif
	{ "acs_unii4", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_BCMARM)
#if !defined(RTCONFIG_WIFILOGO) && !defined(RTCONFIG_HND_ROUTER_AX)
	{ "wl_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Airtime Fairness */
#else
#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58)
	{ "wl_atf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTAX56_XD4) || defined(XD4PRO) || defined(CTAX56_XD4) || defined(XC5) || defined(EBA63)
	{ "wl1_atf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_BCM_7114)
	{ "wl_atf_delay_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* delay scheme to reserve air time for the idle flowring */
#endif
#elif defined(RTCONFIG_AIR_TIME_FAIRNESS)
#if defined(RT4GAX56) || defined(RTAX54)
	{ "wl_atf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },			/* disable/enable Airtime Fairness */
#endif
	{ "wl_atf_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, 		/* ATF mode: 0/1/2(by client/SSID/client+SSID) */
	{ "wl_atf_sta", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, 		/* ATF by client: <MAC>%<MAC>% */
	{ "wl_atf_ssid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, 		/* ATF by SSID: <%<%<%<% */
#endif

#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	{ "wl_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 11ax support */
	{ "wl_ofdma", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 11ax OFDMA support */
	{ "wl_mbo_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_BCMWL6
	{ "acs_ifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable wet tunnel */

#if defined(RTCONFIG_DPSTA) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "dpsta_ifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dpsta_policy", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dpsta_lan_uif", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HAS_5G_2
	{ "dpsta_band", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "dpsta_band", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef TRAFFIC_MGMT_RSSI_POLICY
	{ "wl_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable RSSI (default) */
#endif /* TRAFFIC_MGMT */

	/* Tx Beamforming */
#ifdef RTCONFIG_BCMARM
#ifdef HND_ROUTER
	/* Use "2" for wl_txbf_bfr_cap and wl_txbf_bfe_cap for non-11ax backward compatibility
	 * Use wlconf_overwrite_txbf_cap_nvram() in wlconf.c to overwrite them to "15" for 11ax
	 * TXBF support
	 */
	{ "wl_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2 for mu-mimo case */
	{ "wl_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2 for mu-mimo case */
#else
	{ "wl_txbf_bfr_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2 for mu-mimo case */
	{ "wl_txbf_bfe_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2 for mu-mimo case */
#endif
#ifndef RTCONFIG_WIFILOGO
	{ "wl_txbf_imp", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#endif
#if defined(RTCONFIG_MUMIMO) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
#ifdef HND_ROUTER
	{ "wl_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl_mu_features", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* mu-features=0x8000 when mu-mimo enabled */
#endif
#endif
#else
	{ "wl_txbf_timer", "25", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_BCM_7114
	{ "stop_gmac3", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bw_switch_160 default enabled */
#endif
	/* acsd setting */
#ifndef  HND_ROUTER
	{ "wl_acs_fcs_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd disable FCS mode */
#else
	{ "wl_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd disable FCS mode */
#endif
#ifndef  HND_ROUTER
	{ "wl_dcs_csa_unicast", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disable unicast csa */
#else
	{ "wl_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disable unicast csa */
#endif
	{ "wl_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd exclude chanspec list */
	{ "wl_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd exclude chanspec list */
	{ "wl_acs_excl_chans_dfs", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd exclude chanspec list */
	{ "wl_acs_excl_chans_dfs_2", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd exclude chanspec list */
#if defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM9) || defined(HND_ROUTER)
	{ "wl_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs disable init DFS chan */
#else
	{ "wl_acs_dfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs disable init DFS chan */
#endif
	{ "wl_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* immediate if > 3 switches last 5 minutes */
	{ "wl_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* deferred if > 5 switches in last 7 days */
	{ "wl_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* active: >10k I/O in the last 30 seconds */
	{ "wl_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs cs scan timeout */
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* acsd fcs CI scan period */
#else
	{ "wl_acs_ci_scan_timer", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs CI scan period */
#endif
	{ "wl_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs CI scan timeout */
	{ "wl_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs scan expier time */
	{ "wl_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs tx idle thld */
	{ "wl_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs chan dwell time */
	{ "wl_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs chan flip-flop time */
	{ "wl_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs sample period */
#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58)
        { "wl_intfer_cnt", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* acsd fcs sample cnt */
        { "wl_intfer_txfail", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* fcs non-TCP txfail threshold setting */
        { "wl_intfer_tcptxfail", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* fcs TCP txfail threshold setting */
#else
	{ "wl_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd fcs sample cnt */
	{ "wl_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* fcs non-TCP txfail threshold setting */
	{ "wl_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* fcs TCP txfail threshold setting */
#endif
#if (defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)) && !defined(RTCONFIG_HND_ROUTER_AX)
	{ "wl_acs_use_escan", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable acsd enhanced scan mode */
#endif
#if defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
	{ "wl_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd BGDFS toggle */
	{ "wl_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd BGDFS ahead toggle */
#if !defined(RTCONFIG_WIFI6E) && !defined(RTCONFIG_WIFI7)
	{ "wl_acs_bgdfs_idle_interval", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd traffic idle time before BGDFS */
#endif
	{ "wl_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd max frames for idle */
	{ "wl_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bgdfs avoided by default on far sta */
	{ "wl_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* full MIMO blocking CAC */
	{ "wl_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* tx blanking threshold for 3+1 */
	{ "wl_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* dyn160 toggle */
	{ "wl_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* phy_dyn_switch_th (threshold values) */
	{ "wl_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acsd_scs_dfs_scan in scs mode */
	{ "wl_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* acs_start_on_nondfs toggle */
	{ "wl_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Preference for DFS and Non-DFS channels */
#ifndef RTCONFIG_HND_ROUTER_AX
	{ "wl_acs_cs_high_pwr_pref", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Preference for channel power */
#endif
#ifdef XT12
	{ "wl1_acs_ch_avail_thresh", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* criteria for txop + tx + ibss to make candidate channel as invalid */
#endif
#endif
#ifdef HND_ROUTER
	{ "wl_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable acsd full operation */
#endif
	{ "acs_2g_ch_no_restrict", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: only pick from channel 1, 6, 11 */
#if !defined(RTCONFIG_WIFI6E)
	{ "acs_no_restrict_align", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: only aligned chanspec(few) can be picked (non-20Hz) */
#endif
	{ "acs_ctrl_chan_adjust", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 1: select control chan with max AP number for neighbor friendliness for non-20MHz bw chanspec */
#if defined(RTCONFIG_WIFI6E)
	{ "acs_def_plcy_intf_threshold", "30", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(GTAXE11000)
	{ "wl2_acs_chanim_poll", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_BCM_7114
	/* bgn itf BSS BUSY INTF I-ADJ FCS TXP NOISE TOT CNS ADJ*/
	{ "wl0_acs_pol", "0 100 -20 -15 -18 -1 -10 30 -1 1 1 0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acs default policy */
	{ "wl1_acs_pol", "0 100 -20 -15 -18 -1 -10 35 -1 1 1 0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acs default policy */
#if defined(RTAC5300)
	{ "wl2_acs_pol", "0 100 -20 -15 -18 -1 -10 35 -1 1 1 0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* acs default policy */
#endif
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},		/* acs segment chanim readings */
	{ "wl_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* acs chanim number of segments */
	{ "wl_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "acs_version", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},			/* To boot up on acsdv2 */
	{ "wl_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* enable acs traffic config */
	{ "wl_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},		/* Ignore txfail events */
	{ "wl_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* Access category enable for VI and VO */
	{ "wl_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},      /* txfail threshold limit */
	{ "wl_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},       /* no of secs to hold the data */
	{ "wl_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},        /* txfail threshold limit */
	{ "wl_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0}, /* no of secs to hold the data */
	{ "wl_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},     /* txfail threshold limit */
	{ "wl_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},      /* no of secs to hold the data */
#endif
#ifdef RTCONFIG_BCMARM
#if defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM9) || defined(HND_ROUTER)
	{ "wl_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable PsPretend */
#else
	{ "wl_pspretend_retry_limit", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable PsPretend */
#endif
#ifndef RTCONFIG_BCM7
	{ "wl_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable PsPretend Threshold */
#else
#endif
	{ "wl_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Auto enable TAF */
#ifdef	HND_ROUTER
	{ "wl_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Disable periodic cal */
	{ "wl_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* PSTA inactivity timer */
#endif
#endif

#if defined(BCM_BSD) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "bsd_role", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable Band Steer Daemon */
						/* 0:Disable, 1:Primary, 2:Helper, 3:Standalone */
	{ "bsd_hport", "9877", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BSD helper port */
	{ "bsd_pport", "9878", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BSD Primary port */
	{ "bsd_helper", "192.168.1.2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BSD primary ipaddr */
	{ "bsd_primary", "192.168.1.1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BSD Helper ipaddr */
	{ "smart_connect_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0:Disable */
	{ "bsd_hit_cnt_2g", "2", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* rssi cnt for select sta */
	{ "bsd_hit_cnt_5g", "1", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* rssi cnt for select sta */
	{ "bsd_bounce_detect", "60 2 180", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "bsd_aclist_timeout", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "bsd_scheme", "2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "bsd_msglevel", "0x000010", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BSD_DEBUG_STEER */
	{ "bsd_dbg", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_BCMBSD_V2
#ifdef RTCONFIG_MULTILAN_MWL
	{ "smart_connect_selif", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "wl_bsd_steering_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bsd_sta_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bsd_if_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bsd_if_qualify_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
#ifdef RTCONFIG_QUADBAND
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)/* don't invole 6GHz band in smart connect by default */
	{ "smart_connect_selif", "7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
#else
	{ "smart_connect_selif", "15", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "15", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
#endif
#elif defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G) /* don't invole 6GHz band in smart connect by default */
	{ "smart_connect_selif", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
#else	// Triband Non-6G models
	{ "smart_connect_selif", "7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
#endif
#else	// Dual band
	{ "smart_connect_selif", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
	{ "smart_connect_selif_x", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* selected interface to join smart connect */
#endif // RTCONFIG_QUADBAND
#endif // RTCONFIG_MULTILAN_MWL
#endif // RTCONFIG_BCMBSD_V2


#ifndef RTCONFIG_BCMBSD_V2
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	// Tri-Band or Quad-Band Models
	// bsd_ifnames
#if defined(GTAC5300) || defined(GTAX11000) || defined(GTAXE11000) || defined(GTAX11000_PRO)
	{ "bsd_ifnames", "eth6 eth7 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAXE16000)
	{ "bsd_ifnames", "eth7 eth8 eth9 eth10", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTBE98) || defined(GTBE98_PRO)
	{ "bsd_ifnames", "wl0 wl1 wl2 wl3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC3200)
	{ "bsd_ifnames", "eth2 eth1 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX92U)
	{ "bsd_ifnames", "eth5 eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAXE7800)
	{ "bsd_ifnames", "eth5 eth7 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10)
	{ "bsd_ifnames", "eth6 eth4 eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX9000)
	{ "bsd_ifnames", "eth8 eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX95Q) || defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(ET12) || defined(XT12)
	{ "bsd_ifnames", "eth4 eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT10)
	{ "bsd_ifnames", "wl2 wl1 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "bsd_ifnames", "wl2 wl1 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT12) || defined(RTBE96U) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(GTBE19000AI) || defined(GTBE96) || defined(GTBE96_AI)
	{ "bsd_ifnames", "wl0 wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16)
	{ "bsd_ifnames", "wl3 wl0 wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16_PRO)
	{ "bsd_ifnames", "wl3 wl0 wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "bsd_ifnames", "eth1 eth2 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_steering_policy
#if defined(RTCONFIG_QUADBAND)
	{ "wl0_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy", "0 5 3 -82 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_steering_policy", "0 5 3 -62 0 0 0x22", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10) || defined(RTAX9000)
	{ "wl0_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy", "0 5 3 -82 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_steering_policy", "0 5 3 -62 0 0 0x22", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl0_bsd_steering_policy", "0 5 3 -62 0 0 0x22", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_steering_policy", "0 5 3 -82 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif // defined(RTCONFIG_QUADBAND)

	// bsd_sta_select_policy
#if defined(RTCONFIG_QUADBAND)
	{ "wl0_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_sta_select_policy", "30 -62 0 0 0 1 1 0 0 0 0x122", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10) || defined(RTAX9000)
	{ "wl0_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_sta_select_policy", "30 -62 0 0 0 1 1 0 0 0 0x122", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl0_bsd_sta_select_policy", "30 -62 0 0 0 1 1 0 0 0 0x122", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif //defined(RTCONFIG_QUADBAND)

	// bsd_if_select_policy
#if defined(GTAC5300) || defined(GTAX11000) || defined(GTAXE11000) || defined(GTAX11000_PRO)
	{ "wl0_bsd_if_select_policy", "eth8 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth8 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth7 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAXE16000)
	{ "wl0_bsd_if_select_policy", "eth10 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 5GL, 7
	{ "wl1_bsd_if_select_policy", "eth10 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 5GH, 8
	{ "wl2_bsd_if_select_policy", "eth10 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 6G, 9
	{ "wl3_bsd_if_select_policy", "eth9 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 2.4G, 10
#elif defined(GTBE98) || defined(GTBE98_PRO)
        { "wl0_bsd_if_select_policy", "wl3 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // 5GL, 7
        { "wl1_bsd_if_select_policy", "wl3 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // 5GH, 8
        { "wl2_bsd_if_select_policy", "wl3 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // 6G, 9
        { "wl3_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    // 2.4G, 10
#elif defined(GT10)
	{ "wl0_bsd_if_select_policy", "eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth4 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth5 eth4", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX9000)
	{ "wl0_bsd_if_select_policy", "eth7 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth6 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth7 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC3200)
	{ "wl0_bsd_if_select_policy", "eth3 eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth3 eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth2 eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX92U)
	{ "wl0_bsd_if_select_policy", "eth7 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth5 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAXE7800)
        { "wl0_bsd_if_select_policy", "eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "wl1_bsd_if_select_policy", "eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "wl2_bsd_if_select_policy", "eth5 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX95Q) || defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(ET12) || defined(XT12)
	{ "wl0_bsd_if_select_policy", "eth6 eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth4 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth4 eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT10) || defined(GSBE18000) || defined(GSBE12000) || defined(GT7)
	{ "wl0_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl0 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GS7_PRO)
	{ "wl0_bsd_if_select_policy", "wl0 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTBE95U)
	{ "wl0_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl2 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl1 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT12) || defined(RTBE96U) || defined(GTBE96) || defined(GTBE19000) || defined(RTBE92U) || defined(GTBE19000AI) || defined(GTBE96_AI)
	{ "wl0_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl0 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16)
	{ "wl3_bsd_if_select_policy", "wl0 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl3 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16_PRO)
	{ "wl3_bsd_if_select_policy", "wl0 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_select_policy", "wl2 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl3 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl0_bsd_if_select_policy", "eth3 eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth3 eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "eth2 eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_if_qualify_policy
#if defined(RTCONFIG_QUADBAND)
	{ "wl0_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },			// 6G, null
	{ "wl3_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT10) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "wl2_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_qualify_policy", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTBE95U)
	{ "wl2_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10) || defined(RTAX9000)
	{ "wl0_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },			// 6G, null
#else
	{ "wl0_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_qualify_policy", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif


	// ---------- 5GHz Only Mode ----------
	// bsd_ifnames_x
#if defined(RTAC5300)
	{ "bsd_ifnames_x", "eth2 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX92U)
	{ "bsd_ifnames_x", "eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAXE7800)
        { "bsd_ifnames_x", "eth7 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX95Q) || defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(ET12) || defined(XT12)
	{ "bsd_ifnames_x", "eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT10) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "bsd_ifnames_x", "wl1 wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTBE95U)
	{ "bsd_ifnames_x", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT12) || defined(GTBE96) || defined(GTBE96_AI)
	{ "bsd_ifnames_x", "wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16)
	{ "bsd_ifnames_x", "wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16_PRO)
	{ "bsd_ifnames_x", "wl1 wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAC5300) || defined(GTAX11000) || defined(GTAXE11000) || defined(GTAX11000_PRO) || defined(GTAXE16000)
	{ "bsd_ifnames_x", "eth7 eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC3200)
	{ "bsd_ifnames_x", "eth1 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10)
	{ "bsd_ifnames_x", "eth4 eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTBE98)
	{ "bsd_ifnames_x", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX9000)
	{ "bsd_ifnames_x", "eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "bsd_ifnames_x", "eth2 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_steering_policy_x
#if (defined(RTCONFIG_QUADBAND) && !defined(RTCONFIG_HAS_6G_2)) || defined(GT10) || defined(RTAX9000)
	{ "wl0_bsd_steering_policy_x", "0 5 3 0 0 600 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy_x", "0 5 3 0 700 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_sta_select_policy_x", "30 0 0 500 0 1 1 0 0 0 0x60", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy_x", "30 0 900 0 0 1 1 0 0 0 0x68", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_bsd_steering_policy_x", "0 5 3 0 0 600 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_steering_policy_x", "0 5 3 0 700 0 0x28", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy_x", "30 0 0 500 0 1 1 0 0 0 0x60", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_sta_select_policy_x", "30 0 900 0 0 1 1 0 0 0 0x68", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_if_select_policy_x
#if defined(RTAC5300)
	{ "wl1_bsd_if_select_policy_x", "eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX92U)
	{ "wl1_bsd_if_select_policy_x", "eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAXE7800)
        { "wl1_bsd_if_select_policy_x", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "wl2_bsd_if_select_policy_x", "eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX95Q) || defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(ET12) || defined(XT12)
	{ "wl1_bsd_if_select_policy_x", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT12) || defined(BT10) || defined(GTBE96) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7) || defined(GTBE96_AI)
	{ "wl1_bsd_if_select_policy_x", "wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTBE95U)
	{ "wl0_bsd_if_select_policy_x", "wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16)
	{ "wl1_bsd_if_select_policy_x", "wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BQ16_PRO)
	{ "wl1_bsd_if_select_policy_x", "wl2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAC5300) || defined(GTAX11000) || defined(GTAXE11000) || defined(GTAX11000_PRO) || defined(GTAXE16000)
	{ "wl1_bsd_if_select_policy_x", "eth8", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC3200)
	{ "wl1_bsd_if_select_policy_x", "eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10)
	{ "wl0_bsd_if_select_policy_x", "eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy_x", "eth4", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GS7_PRO)
	{ "wl1_bsd_if_select_policy_x", "wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_select_policy_x", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTBE98)
	{ "wl0_bsd_if_select_policy_x", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy_x", "wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX9000)
	{ "wl0_bsd_if_select_policy_x", "eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy_x", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_bsd_if_select_policy_x", "eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy_x", "eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_if_qualify_policy_x
#if (defined(RTCONFIG_QUADBAND) && !defined(RTCONFIG_HAS_6G_2)) || defined(GT10) || defined(RTAX9000)
	{ "wl0_bsd_if_qualify_policy_x", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy_x", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl1_bsd_if_qualify_policy_x", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_qualify_policy_x", "0 0x4 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "bsd_bounce_detect_x", "60 2 180", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "bsd_aclist_timeout", "3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },


#else 	//!RTCONFIG_HAS_5G_2 && !RTCONFIG_HAS_6G_2
	// Dual-Band Models
	// bsd_ifnames
#ifdef RTAX82_XD6S
	{ "bsd_ifnames", "eth2 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC86U) || defined(GTAC2900) || defined(BCM6750) || defined(BCM63178) || defined(RTAX56U) || defined(RTAX68U) || defined(RTAC68U_V4) || defined(TUFAX3000_V2)
	{ "bsd_ifnames", "eth5 eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX88U) || defined(RTAX86U) || defined(GTAX6000) || defined(RTAX86U_PRO) || defined(RTAX88U_PRO)
	{ "bsd_ifnames", "eth6 eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX56_XD4) || defined(XD4PRO) || defined(CTAX56_XD4) || defined(XC5) || defined(EBA63) || defined(RTBE88U) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE86U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(RPBE58) || defined(RTBE58_GO)
	{ "bsd_ifnames", "wl0 wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63)
        { "bsd_ifnames", "eth2 eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "bsd_ifnames", "eth1 eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_if_select_policy
#ifdef RTAX82_XD6S
	{ "wl0_bsd_if_select_policy", "eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAC86U) || defined(GTAC2900) || defined(BCM6750) || defined(BCM63178) || defined(RTAX56U) || defined(RTAX68U) || defined(RTAC68U_V4) || defined(TUFAX3000_V2)
	{ "wl0_bsd_if_select_policy", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth5", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX88U) || defined(RTAX86U) || defined(GTAX6000) || defined(RTAX86U_PRO) || defined(RTAX88U_PRO)
	{ "wl0_bsd_if_select_policy", "eth7", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth6", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX56_XD4) || defined(XD4PRO) || defined(CTAX56_XD4) || defined(XC5) || defined(EBA63) || defined(RTBE88U) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE86U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(RTBE58_GO)
	{ "wl0_bsd_if_select_policy", "wl1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "wl0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63)
	{ "wl0_bsd_if_select_policy", "eth3", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wl0_bsd_if_select_policy", "eth2", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "eth1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// bsd_steering_policy 
	{ "wl0_bsd_steering_policy", "0 5 3 -62 0 0 0x22", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// bsd_sta_select_policy
	{ "wl0_bsd_sta_select_policy", "30 -62 0 0 0 1 1 0 0 0 0x122", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// bsd_if_qualify_policy
	{ "wl0_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 2.4G, all allowed
	{ "wl1_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// 5G, all allowed
#endif // RTCONFIG_HAS_5G_2 || RTCONFIG_HAS_6G_2
#endif // !RTCONFIG_BCMBSD_V2
#endif // BCM_BSD

#if defined(BCM_APPEVENTD)
	{ "appeventd_enable", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(RTCONFIG_BCM_CEVENTD)
        /* cevent: Connectivity event generation support. Enabled by default */
        { "ceventd_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Toggle ceventd service (started on rc) */
        { "ceventd_out", "/tmp/ce0.log", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* ceventd output file path */
	/* events to be logged by cevent  daemon. Below events are set by default
	 * 1, 2 , 3 , 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 17, 23, 25, 40, 46, 136, 180
	*/
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
	{ "ceventd_eventmask", "0x1000000000010000000000000000000040410002831fff", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "ceventd_eventmask", "0x1000000000010000000000000000000000410002831fff", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ceventd_log_syslog", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Toggle cevent in driver (per primary-iface) */
#endif /* BCM_CEVENT */

#ifdef RTCONFIG_BRCM_HOSTAPD
	{ "hapd_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Toggle hostapd service */
#endif

#ifdef RTCONFIG_BCMARM
#if !defined(RTCONFIG_BCM7) && !defined(RTCONFIG_BCM_7114) && !defined(HND_ROUTER)
	{ "acsd_foreground", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* daemon foreground mode control */
	{ "nas_foreground", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* daemon foreground mode control */
	{ "eapd_foreground", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* daemon foreground mode control */
#endif
#endif
#ifdef __CONFIG_LBR_AGGR__
	{ "wl_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* per tid/ac mask disable by default */
	{ "wl_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default aggregate len */
	{ "wl_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default release timeout in msec */
#endif /* __CONFIG_LBR_AGGR__ */
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "wl_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#if defined (RTCONFIG_BCM_7114) || defined(HND_ROUTER)
#ifndef RTCONFIG_HND_ROUTER_AX
	{ "wl_flr_lfrag_max", "400", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_flr_lfrag_txpkts_min", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_flr_lfrag_txpkts_adjust", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(BCM_SSD) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "ssd_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable SSID Steer Daemon */
	{ "wl_ssd_type", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* default ssd_type "disabled" */
#endif
#if defined(HND_ROUTER) || defined(RTCONFIG_HND_ROUTER_AX)
	{"wl_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* DFS reduce bandwidth scheme */
#endif
#if defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM9) || defined(HND_ROUTER)
	{ "wl_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DFS Preferred channel value */
	{ "wl_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MAC filter based probe response */
	{ "wl_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* SW probe response */
#if defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
	{ "wl_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* VHT mode */
#endif
	{ "wl_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* VHT features */
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "vht_adv_2g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* enable 2G VHT advertisement */
#endif
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "wl_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 11ax support */
#ifdef RTCONFIG_INTEL_EVO
	{ "wl_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* OFDMA OFF & MU-MIMO OFF */
#else
#if defined(GTAXE11000) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "wl_ofdma", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DL/UL OFDMA + MU-MIMO */
#if defined(RTCONFIG_QUADBAND)
	{ "wl3_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4G OFDMA OFF & MU-MIMO OFF */
#elif defined(BT10)
	{ "wl2_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4G OFDMA OFF & MU-MIMO OFF */
#elif defined(GT10) || defined(RTAX9000) || defined(RTBE95U) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "wl2_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4G OFDMA OFF & MU-MIMO OFF */
#else
	{ "wl0_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4G OFDMA OFF & MU-MIMO OFF */
#endif
#else //!defined(GTAXE11000) && !defined(RTCONFIG_HND_ROUTER_AX_6756)
	{ "wl_ofdma", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DL OFDMA ONLY */
	{ "wl0_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 2.4G OFDMA OFF & MU-MIMO OFF */
#endif
#endif // RTCONFIG_INTEL_EVO
#ifdef RTAX92U
	{ "wl0_11ax", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11ax support */
	{ "wl1_11ax", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11ax support */
	{ "wl2_11ax", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11ax support */
#elif defined RPAX56
	{ "wl0_11ax", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11ax support */
	{ "wl1_11ax", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11ax support */
	{ "wl0_ofdma", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 2.4G OFDMA OFF & MU-MIMO OFF */
	{ "wl1_ofdma", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* 11ax downlink/uplink OFDMA support */
	{ "wl0_he_features", "15", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
	{ "wl1_he_features", "15", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
#else
	{ "wl0_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
	{ "wl1_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "wl2_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
#endif
#if defined(RTCONFIG_QUADBAND)
	{ "wl3_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* HE features */
#endif
#endif
	{ "wl_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* override amsdu in ampdu */
	{ "wl_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "wl_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif // RTCONFIG_HND_ROUTER_AX
#if defined(RTCONFIG_WIFI7)
#if defined(RPBE58)
	{ "wl_11be", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11be support */
#else
	{ "wl_11be", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11be support */
#endif
#if defined(RPBE58)
	{ "wl0.1_11be", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11be support */
	{ "wl1.1_11be", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* 11be support */
	{ "dpsr_mb", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* dual band mb mode */
#endif
	{ "wl_eht_features", "-1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* 11be + OFDMA + MU-MIMO */
#endif // RT-BE96U

#if defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM9) || defined(HND_ROUTER)
	{ "wl_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Dynamic BWSW disable defaults */
#endif
#ifdef RTCONFIG_BCM_7114
	{ "wl_tpc_db", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* TPC Mitigation (db) */
	{ "dhd_msg_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "crash_log_backup_dir", "/data", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* dhd_monitor crash log backup path */
	{ "wl_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* Generate PMKs locally */
	{ "wl_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_VIF, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "wl_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#endif
#if defined(RTCONFIG_HND_ROUTER_BE_4916)
#if defined(RTCONFIG_QUADBAND) || defined(RTBE95U)
	{ "wl0_amrts_obss", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl0_amrts_glitch", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl0_amrts_badplcp", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
	{ "wl1_amrts_obss", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl1_amrts_glitch", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl1_amrts_badplcp", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#if !defined(RTBE88U) && !defined(RTBE86U) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(RTBE58_GO)
	{ "wl2_amrts_obss", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl2_amrts_glitch", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "wl2_amrts_badplcp", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#endif
#ifdef __CONFIG_VISUALIZATION__
	{ "vis_dcon_ipaddr", "127.0.0.1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Visualization Dcon IP address */
	{ "vis_do_remote_dcon", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Visualization remote debugging enabled flag */
	{ "vis_debug_level", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},		/* Visualization debug messages to be printed */
	{ "vis_disable_animate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* Visualization site survey animation on or off */
	{ "wl_rrm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},			/* Set rrm caps per interface */
#endif /* __CONFIG_VISUALIZATION__ */
#ifdef RTCONFIG_BCMASPMD
	{ "aspm_config", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef BCM_EVENTD
	{ "eventd_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

	// make sure its purpose
	// advanced-ctnf
	{ "ct_tcp_timeout", "", CKN_STR100, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ct_udp_timeout", "30 180", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ct_timeout", "", CKN_STR20, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_AMAS
	{ "sta_ifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "amas_ethernet", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BHCOST_OPT
	{ "amas_wlc_target_bssid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "amas_eth_bhmode", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTAX95Q) || defined(XT8PRO) || defined(BT12) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2)
/* multiple uplink port */
/* index:0, priority:1 use:1; */
/* priority of eth_priority and sta_priority should be unique */
	{ "amas_ethif_type", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "eth_priority", "0 1 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "amas_ethif_type", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "eth_priority", "0 1 1 1 2 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "amas_costmode", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
	{ "amas_wifi_bhmode", "80", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "amas_wifi_bhmode", "48", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "amas_eap_bhmode", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_AMAS_WDS) && defined(RTCONFIG_BHCOST_OPT)
	{ "amas_wds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
#endif //RTCONFIG_BHCOST_OPT
#if defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2) || defined(RTCONFIG_HAS_6G)
/* multiple uplink port */
/* index:0, priority:1 use:1; */
/* priority of eth_priority and sta_priority should be unique */
#if defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2)
	{ "sta_priority", "2 0 4 1 5 1 3 1 6 2 2 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "sta_priority", "2 0 5 1 5 1 4 0 5 2 3 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
#if defined(RTAX95Q) || defined(XT8PRO) || defined(BT12) || defined(BM68) || defined(XT8_V2)
	{ "sta_priority", "2 0 4 1 5 1 3 0 5 2 2 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "sta_priority", "2 0 4 1 5 1 3 1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef AMASDB
	{ "amasdb_fromprb", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "amasdb_size", "200", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "re_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "max_guest_index", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "enable_diag", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "diag_local_data", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "diag_interval", "60", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "diag_dbg", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "diag_syslog2", "5", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "chksta_mac", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "chksta_band", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_STA_AP_BAND_BIND
	{ "sta_binding_list", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MSSID_PRELINK
#if defined(GT10) || defined(BQ16) || defined(BQ16_PRO)
	{ "plk_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(BT10)	/* 6G is prelink ssid, unit = 0 */
	{ "plk_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "plk_unit", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "plk_cap_subunit", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "plk_re_subunit", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "plk_closed", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_DWB) && defined(RTCONFIG_BHCOST_OPT)
#if (defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_QUADBAND)) || defined(GT10) || defined(RTAX9000) || defined(RTBE95U)
	{ "dwb_band", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#elif defined(BT10) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "dwb_band", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#else
	{ "dwb_band", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#endif
#ifdef RTCONFIG_FRONTHAUL_DWB
#if defined(RTCONFIG_DEF_AUTO_FRONTHAUL)
	{ "fh_ap_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},  // Auto
#elif defined(RTCONFIG_DEF_ON_FRONTHAUL)
	{ "fh_ap_enabled", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},  // On
#elif defined(RTCONFIG_DEF_OFF_FRONTHAUL) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
	{ "fh_ap_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},  // Off
#else
#if (defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)) && !defined(RTCONFIG_QUADBAND)
	{ "fh_ap_enabled", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},  // On
#else
	{ "fh_ap_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},  // Off
#endif
#endif
	{ "fh_cap_mssid_subunit", "5", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fh_re_mssid_subunit", "6", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif //RTCONFIG_DWB
#ifdef RTCONFIG_AMAS_CHANNEL_PLAN
	{ "channel_plan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_set_channel", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Channel number */
	{ "wl_set_bw", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 20/40/80/160 MHz */
	{ "wl_set_nctrlsb", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* N-CTRL SB */
#endif
#endif //RTCONFIG_AMAS
#ifdef RTCONFIG_LANTIQ
	{ "ctf_disable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ctf_disable_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef CONFIG_BCMWL5
#ifdef HND_ROUTER
	{ "fc_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fc_pt_war", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fc_pt_udp_war", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "runner_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	{ "watchdog_new", "30000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "watchdog", "6000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "bcn_stuck_watchdog", "10000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
	{ "ctf_disable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ctf_disable_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ctf_nonat_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpncoppp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BCMARM
#if 0
#ifdef RTCONFIG_USB_MODEM
	// TODO: for the bad CTF. After updating CTF, need to mark these codes.
	{ "ctf_disable_modem", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "ctf_pt_udp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BCMFA
	{ "ctf_fa_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif	// RTCONFIG_BCMARM
#endif	// HND_ROUTER
#ifndef RTCONFIG_BCMARM
#if defined(RTAC53U)
	// RT-AC53U disable gro can get better usb performance
	{ "gro_disable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "gro_disable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
	{ "gro_disable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif	// RTCONFIG_BCMARM
#endif	// CONFIG_BCMWL5
#if defined(RTCONFIG_LED_BTN) || defined(RTCONFIG_WPS_ALLLED_BTN) || defined(RTCONFIG_TURBO_BTN) || defined(RTCONFIG_SW_CTRL_ALLLED) || (!defined(RTCONFIG_WIFI_TOG_BTN) && !defined(RTCONFIG_QCA))
	{ "AllLED", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_SW_CTRL_ALLLED
	{ "AllLED_brightness", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "led_val", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_SW_BTN)
	{ "btnsw_onoff", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btnsw_vpn_idx", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btnsw_sdn_idx", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// NVRAM for start_lan:
// LAN H/W parameters
	{ "lan_hwnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN interface MAC address */

	// LAN TCP/IP parameters
	{ "lan_unit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DEFAULT_AP_MODE) || defined(RTCONFIG_CONCURRENTREPEATER)
	{ "lan_proto", "dhcp", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP server [static|dhcp] */
#else
	{ "lan_proto", "static", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP server [static|dhcp] */
#endif
#if defined(RTN300) || defined(RTCONFIG_ALL_DEF_LAN50) || defined(RTN300)
	{ "lan_ipaddr", "192.168.50.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN IP address */
	{ "lan_ipaddr_rt", "192.168.50.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_WIFI_SON) || defined(RTCONFIG_LYRA_HIDE)
	{ "lan_ipaddr", "192.168.72.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN IP address */
	{ "lan_ipaddr_rt", "192.168.72.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "lan_ipaddr", "192.168.1.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN IP address */
	{ "lan_ipaddr_rt", "192.168.1.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_ATEFROMWAN
	{ "wan_ate_ipaddr", "169.254.1.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IP address for device only wan port use*/
#endif
	{ "lan_netmask", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN netmask */
	{ "lan_netmask_rt", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_gateway", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN Gateway */

	{ "lan_wins", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "lan_hostname", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN hostname */
	{ "lan_domain", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN domain name */
	{ "lan_lease", "86400", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN lease time in seconds */
	{ "lan_stp", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN spanning tree protocol */
#if defined(EBG15) || defined(EBG19)
	{ "lan_stp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN spanning tree protocol */
#endif
#ifdef RTCONFIG_LACP
#ifdef GTAC5300
	{ "lacp_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* LACP enable or not */
#else
	{ "lacp_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LACP enable or not */
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	{ "lacp_ifnames_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },  /* manually assign LACP slaves */
#endif
	{ "bonding_policy", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* bonding policy */
#if defined(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033)
	/* 0/1/2: auto (if 10G base-T is one of WAN port)/PPE + NSS/NSS only */
	{ "aqr_hwnat_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 0/1000/2500/5000/10000: auto/1G/2.5G/5G/10G, 10G base-T link speed */
	{ "aqr_link_speed", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 96 (default)/128: Interpacket gap of 10G base-T */
	{ "aqr_ipg", "96", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 0/1/2: auto (if 10G SFP+ is one of WAN port)/PPE + NSS/NSS only */
	{ "sfpp_hwnat_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 0/1000/10000: auto/1G/10G, SFP+ maximum link speed */
	{ "sfpp_max_speed", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sfpp_force_on", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* auto/force SFP+ TX clock */
#endif
#if defined(RTCONFIG_BONDING_WAN)
	{ "bond_wan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN bonding enable or not */
#if defined(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033) \
 || defined(RTCONFIG_SWITCH_MT7986_MT7531)
	/* Port numbers that are used to specify slave ports in bond1, same as enum bs_port_id.
	 * 0: WAN, 1~8: LAN1 ~ 8, 30: 10G base-T (RJ-45), 31: 10G SFP+,  rest bits are not defined.
	 * e.g. "1 2" = LAN1, LAN2.
	 */
	{ "wanports_bond", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN aggregation */
#endif
#endif
#if 0
#if defined(RTAX88U) || defined(GTAX11000)
	{ "lacp_4ports", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* 4 ports link aggregation */
#endif
#endif
#endif // RTCONFIG_LACP
	{ "lan_route", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

#if defined(RTCONFIG_DEFAULT_AP_MODE) || defined(RTCONFIG_CONCURRENTREPEATER)
	{ "lan_dnsenable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "lan_dnsenable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "lan_dns1_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "lan_dns2_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "jumbo_frame_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* Guest TCP/IP parameters */

	/* Guest H/W parameters */
	{ "lan1_hwnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN driver names (e.g. et0) */
	{ "lan1_hwaddr", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN interface MAC address */

	{ "lan1_proto", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP client [static|dhcp] */
	{ "lan1_ipaddr", "192.168.2.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN IP address */
	{ "lan1_netmask", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN netmask */
	{ "lan1_gateway", "192.168.2.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN gateway */
	{ "lan1_wins", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "lan1_domain", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN domain name */
	{ "lan1_lease", "86400", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN lease time in seconds */
	{ "lan1_stp", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* LAN spanning tree protocol */
	{ "lan1_route", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

	// NVRAM for start_dhcpd
	// DHCP server parameters
	{ "dhcp_enable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTN300) || defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_ALL_DEF_LAN50) || defined(RTN300)
	{ "dhcp_start", "192.168.50.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_end", "192.168.50.254", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_WIFI_SON) || defined(RTCONFIG_LYRA_HIDE)
	{ "dhcp_start", "192.168.72.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_end", "192.168.72.254", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "dhcp_start", "192.168.1.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_end", "192.168.1.254", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dhcp_lease", "86400", CKN_STR7, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_gateway_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_dns1_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_dns1_x", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_wins_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp_static_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_EXTEND_LIMIT)
	{ "dhcp_staticlist", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "dhcp_staticlist", "", CKN_STR5500, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dhcpd_lmax", "253", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcpc_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DHCP_OVERRIDE
	{ "dnsqmode", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_REDIRECT_DNAME
	{ "redirect_dname", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// NVRAM for start_dhcpd
	// Guest DHCP server parameters
#if 0 // unused
	{ "dhcp1_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_start", "192.168.2.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_end", "192.168.2.254", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_lease", "86400", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_gateway_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_dns1_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_wins_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_static_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcp1_staticlist", "", CKN_STR2500, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "time_zone", "GMT0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "time_zone_dst", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "time_zone_dstoff", "M3.2.0/2,M10.2.0/2", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT },	/* 0 */
	{ "ntp_server1", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ntp_server0", "pool.ntp.org", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM for do_startic_routes
	{ "sr_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sr_rulelist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dr_enable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* oleg patch */
	{ "mr_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0:disable, 1:igmp, 2:mld, 3:igmp+mld */
	{ "mr_qleave_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mr_altnet_x", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_IMPROXY) || defined(HND_ROUTER)
	{ "mr_igmp_ver", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPV6
	{ "mr_mld_ver", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_MULTISERVICE_WAN)
	{ "mr_mswan_idx", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MOCA
	{ "moca_mps_walktime", "120", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "moca_privacy_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "moca_password", "12345678901234567", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "moca_epassword", "12345678901234567", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "moca_sceu_mode", "7", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "moca_ifname", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	// Domain Name
	{ "local_domain", DUT_DOMAIN_NAME, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM for switch control
//#ifdef RTCONFIG_SWITCH_CONTROL_8367
#if defined(RTCONFIG_RALINK) || \
    defined(RTCONFIG_SWITCH_RTL8370M_PHY_QCA8033_X2) || \
    defined(RTCONFIG_SWITCH_RTL8370MB_PHY_QCA8033_X2)
	{ "switch_ctrlrate_unknown_unicast", "20", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "switch_ctrlrate_unknown_multicast", "20", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "switch_ctrlrate_multicast", "20", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "switch_ctrlrate_broadcast", "20", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTN56U
	{ "switch_wanport_force_1g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

#ifdef RTCONFIG_RALINK
#if defined (W7_LOGO) || defined (WIFI_LOGO) || defined(RTCONFIG_DSL) || defined(RTCONFIG_RALINK_MT7628)
	{ "hwnat", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "hwnat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_QCA
	{ "qca_sfe", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_SOC_IPQ8064) || defined(RTCONFIG_SOC_IPQ8074)
	/* Define QCA specific nvram variable to enable/disable GRO on
	 * LAN and WAN interface temporarilly.
	 * GRO is not turn on if WAN is PPPoE, PPTP, L2TP, USB
	 */
	{ "qca_gro", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_SOC_QCA9557)
	{ "traffic_5g", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

	// NVRAM for start_wan
#if defined(RTCONFIG_MULTISERVICE_WAN)
#ifdef RTCONFIG_MULTIWAN_IF
	{ "wan_unit", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wan_unit", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#else
	{ "wan_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Last configured connection */
#endif
	{ "wan_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// NVRAM for start_wan_if
	/* WAN H/W parameters */
	{ "wan_hwname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN driver name (e.g. et1) */
	{ "wan_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN interface MAC address */
	{ "wan_phytype", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* WAN TCP/IP parameters */
#ifdef RTCONFIG_DEMOUI
	{ "wan_proto", "static", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* [static|dhcp|pppoe|pptp/l2tp|disabled] */
	{ "wan_dhcpenable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* replace x_DHCPClient */
	{ "wan_ipaddr_x", "192.168.2.80", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN IP address *//* Should be changed by DUT */
	{ "wan_ipaddr", "192.168.2.80", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_netmask_x", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN netmask */
	{ "wan_gateway_x", "192.168.2.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN gateway */
	{ "wan_gateway", "192.168.2.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dnsenable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dns1_x", "192.168.2.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "wan_dns2_x", "8.8.8.8", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wan_proto", "dhcp", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* [static|dhcp|pppoe|pptp/l2tp|disabled] */
	{ "wan_dhcpenable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* replace x_DHCPClient */
	{ "wan_ipaddr_x", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN IP address */
	{ "wan_ipaddr", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_netmask_x", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN netmask */
	{ "wan_gateway_x", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN gateway */
	{ "wan_gateway", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dnsenable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dns1_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "wan_dns2_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wan_dns", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_wins", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* x.x.x.x x.x.x.x ... */
	{ "wan_heartbeat_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* VPN Server */
	{ "wan_hostname", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN hostname */
	{ "wan_hwaddr_x", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Cloned mac */
	{ "wan_nat_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_gw_mac", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef TODO
	{ "wan_domain", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN domain name */
	{ "wan_lease", "86400", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN lease time in seconds */
#endif

	/* Auth parameters */
	{ "wan_auth_x", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN authentication type */

	/* PPPoE parameters */
	{ "wan_pppoe_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PPP username */
	{ "wan_pppoe_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PPP password */
	{ "wan_pppoe_idletime", "0", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* oleg patch */
	{ "wan_pppoe_mru", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Negotiate MRU to this value */
	{ "wan_pppoe_mtu", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Negotiate MTU to the smaller of this value or the peer MRU */
	{ "wan_pppoe_service", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PPPoE service name */
	{ "wan_pppoe_ac", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PPPoE access concentrator name */
	{ "wan_pppoe_hostuniq", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PPPoE host-uniq */
	{ "wan_pppoe_options_x", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* oleg patch */
#ifdef RTCONFIG_SPECIFIC_PPPOE
	{ "wan_pppoe_specific", "0", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_pppoe_holdoff_min", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_pppoe_holdoff_max", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MULTI_PPP
	{ "wan_ppp_conn", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wan_pptp_options_x", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* oleg patch */
	{ "wan_pppoe_auth", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ppp_echo", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ppp_echo_interval", "6", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ppp_echo_failure", "10", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dhcp_qry", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // dhcpc_mode for multiple wan
	{ "wan_dhcpfilter_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MT798X)
	{ "wan_force_link", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // force eth link speed 0:auto 1:10Mbps 2:100Mbps 3:1000Mbps 4:2500Mbps 5:10Gbps
#endif

#ifdef RTCONFIG_DUALWAN
#ifdef DSL_AX82U
	{ "dns_probe", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "dns_probe", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "dns_probe_host", "dns.msftncsi.com", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* host to resolve */
#if 0
#ifdef RTCONFIG_IPV6
	{ "dns_probe_content", "131.107.255.255 112.4.20.71 fd3e:4f5a:5b81::1", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* resolve target addr or wildcard */
#else
	{ "dns_probe_content", "131.107.255.255 112.4.20.71", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* resolve target addr or wildcard */
#endif
#else
	{ "dns_probe_content", "*", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* resolve target addr or wildcard */
#endif
	{ "dns_delay_round", "2", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_DNSPRIVACY
	{ "dnspriv_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: None 1: DNS-over-TLS */
	{ "dnspriv_profile", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Opportunistic 1: Strict */
	{ "dnspriv_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* ... */
#endif

	/* Misc WAN parameters */
	{ "wan_desc", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WAN connection description */
	{ "wan_upnp_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* upnp igd */
	{ "wan_pppoe_relay", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// VPN+DHCP, a sperated nvram to control this function
	{ "wan_vpndhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// DHCP option
	{ "wan_clientid_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_clientid", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_vendorid", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "wan_mtu", "1500", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#if defined(RTCONFIG_MULTISERVICE_WAN)
	{ "wan_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dot1p", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dscp", "-1", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_SWITCH_RTL8370M_PHY_QCA8033_X2) || \
    defined(RTCONFIG_SWITCH_RTL8370MB_PHY_QCA8033_X2)
	/* VLAN, IPTV must be disabled. */
	{ "wan_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 0/1: disable/enable VLAN on WAN port */
	{ "wan_vid", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* 3~4094: VLAN ID, 0~2,4095 are reserved. */
#endif

	// For miniupnpd, so far default value only
#ifndef RTCONFIG_BCM_MFG
	{ "upnp_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "upnp_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "upnp_secure", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_port", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_ssdp_interval", "60", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_mnp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_clean", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_clean_interval", "600", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_clean_threshold", "20", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_AUPNPC
	{ "aupnpc_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_DUALWAN // RTCONFIG_DUALWAN
#if (defined(RTCONFIG_INTERNAL_GOBI) || defined(RTCONFIG_WANPORT2)) && !defined(RTAD7200) && !defined(RTCONFIG_NO_WANPORT) && !defined(GTAXY16000) && !defined(RTAX89U)
	{ "wans_mode", "lb", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(DSL_AX82U)
	{ "wans_mode", "fb", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wans_mode", "fb", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* off/failover/failback/loadbance(off/fo/fb/lb) */
#endif
#ifdef RTCONFIG_DSL
	{ "wans_dualwan", "dsl " DEF_SECOND_WANIF, CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif (defined(RTCONFIG_INTERNAL_GOBI) && defined(RTCONFIG_NO_WANPORT))
	{ "wans_dualwan", DEF_SECOND_WANIF " none", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_INTERNAL_GOBI)
	{ "wans_dualwan", DEF_SECOND_WANIF " wan", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wans_dualwan", "wan " DEF_SECOND_WANIF, CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_USB_WAN_BACKUP
	{ "wans_usb_bk", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wans_usb_bk_act", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wans_standby", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wans_lanport", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_WANPORT2)
	{ "wans_lb_ratio", "1:1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* only support two wan simultaneously */
#else
	{ "wans_lb_ratio", "3:1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* only support two wan simultaneously */
#endif
	{ "wans_routing_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wans_routing_rulelist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "wandog_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wandog_target", "", CKN_STR100, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_EXTPHY
	{ "ping_target", "www.google.com", CKN_STR100, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_INTERNAL_GOBI
	{ "wandog_interval", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Be the same with lteled's interval. */
	{ "wandog_maxfail", "12", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wandog_interval", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wandog_maxfail", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dns_probe_timeout", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTBE88U)
	{ "wandog_delay_only1", "30", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wandog_delay_only1", "5", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wandog_delay", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wandog_fb_count", "4", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wandog_fb_restart", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif // RTCONFIG_DUALWAN
#ifdef RTCONFIG_MULTIWAN_PROFILE
	{ "wans_mt_ioport", "wan", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wans_mt_default_wan", "", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Set which wan unit as default WAN
	//{ "wans_mt_dev_rl", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// rule of specific LAN device or interface
	{ "mtwan_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mtwan_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mtwan_mt_group", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mtwan_mt_weight", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mtwan_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Fail-over if any wan disconnect
	{ "mtwan_fb", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Allow fail-back
	{ "mtwan_order", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// group index of Fail-Over order​
	{ "mtwan_group", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// current wan group index
	{ "mtwan_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// wan scheduler
#endif
#ifdef RTCONFIG_MULTIWAN_IF
	{ "wan_wandog_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_wandog_target", "", CKN_STR100, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_wandog_interval", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_wandog_maxfail", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_wandog_fb_count", "4", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dns_probe", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_dns_probe_host", "dns.msftncsi.com", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* host to resolve */
#endif
#ifdef BLUECAVE
	{ "bc_ledLv", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Central led control
#endif
#if defined(LANTIQ_BSD) || defined(RTCONFIG_QCA_LBD) || defined(RTCONFIG_MTK_BSD)
        { "smart_connect_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* Disable Band Steer Daemon */
#endif

#ifdef RTCONFIG_EXTPHY_BCM84880
	{ "wans_extwan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* configure bcm84880 as LAN/WAN port */
#endif

#if defined(GTAXE16000) || defined(RTBE96U) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTAX11000_PRO) || defined(RTBE88U) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(RTBE86U) || defined(XT12) || defined(ET12) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(BD4D5) || defined(BD4_OD) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	{ "wan_ifname_x", "eth0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* manually select specific port as WAN port */
#elif defined(RTBE58_GO)
	{ "wan_ifname_x", "eth1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* manually select specific port as WAN port */
#endif

#ifdef RTCONFIG_DSL
	{ "dslx_modulation", "5", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* multiple mode */
	{ "dslx_snrm_offset", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Paul add 2012/9/24 */
	{ "dslx_sra", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Paul add 2012/10/15 */
	{ "dslx_bitswap", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Paul add 2013/10/23 */
	{ "dslx_adsl_rx_agc", "Default", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Renjie add 2014/12/23 */
	{ "dslx_adsl_esnp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enhanced Sudden Noise Protection */
#ifdef RTCONFIG_DSL_ANNEX_B //Paul add 2012/8/21
	{ "dslx_annex", "6", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Annex BJM (EnumAdslTypeB_J_M) */
#else
	{ "dslx_testlab", "disable", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Country-Specific Setting for AU or GB */
	{ "dslx_annex", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Annex AIJLM(EnumAdslTypeA_I_J_L_M) */
#endif
	{ "dslx_ginp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dslx_ginp_try_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_ginp_try_enable_disp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dslx_dla_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_diag_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_diag_duration", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_diag_state", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_diag_end_uptime", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_VDSL
	{ "dslx_vdsl_bitswap", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_vectoring", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_nonstd_vectoring", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_target_snrm", "32767", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_tx_gain_off", "32767", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdtxpwrtestmode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_rx_agc", "65535", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_upbo", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_esnp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enhanced Sudden Noise Protection */
	{ "dslx_vdsl_profile", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_rmvlan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_vdsl_ginp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Stability Adjustment specifically for VDSL G.INP mode */
#endif
#ifdef RTCONFIG_DSL_HOST
	{ "dc_waitsec", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

// Viz add dsl transmode
#ifdef RTCONFIG_VDSL
	{ "dslx_transmode", "ptm", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* atm */
	{ "dsltmp_transmode", "ptm", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* atm */
#else
	{ "dslx_transmode", "atm", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* atm */
	{ "dsltmp_transmode", "atm", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* atm */
#endif
#ifdef RTCONFIG_DSL_BCM
	{ "dslx_transmode_auto", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if !defined(DSL_AX82U) // for old qis/wan
// the following variables suppose can be removed
	{ "dslx_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_upnp_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_link_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_DHCPClient", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_dhcp_clientid_type", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: MAC 1: Node-specific RFC4361 */
	{ "dslx_dhcp_clientid", "", CKN_STR65, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Required by some ISP using RFC 1483 MER. */
	{ "dslx_dhcp_vendorid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP option 60 */
	{ "dslx_dhcp_hostname", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP option 12 */
	{ "dslx_ipaddr", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IP address */
	{ "dslx_netmask", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* netmask */
	{ "dslx_gateway", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* gateway */
	{ "dslx_dnsenable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_dns1", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_dns2", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_auth", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// this one is no longer to use
	//{ "dslx_pppoe_dial_on_demand", "" },
	{ "dslx_pppoe_idletime", "0", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_mtu", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_mru", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_service", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_ac", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_hostuniq", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_pppoe_options", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dslx_hwaddr", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
//
	{ "dsl_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl_enable", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* Paul modify 2012/8/6, set default Service Category to UBR without PCR, with PCR, SCR and MBS set to 0. */
	{ "dsl_svc_cat", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pcr", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_scr", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_mbs", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_VDSL) || defined(RTCONFIG_DSL_TCLINUX) || defined(RTCONFIG_DSL_HOST)
	{ "dsl_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_upnp_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_link_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_DHCPClient", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_dhcp_clientid_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: MAC 1: Node-specific RFC4361 */
	{ "dsl_dhcp_clientid", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Required by some ISP using RFC 1483 MER. */
	{ "dsl_dhcp_vendorid", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP option 60 */
	{ "dsl_dhcp_hostname", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DHCP option 12 */
	{ "dsl_dhcp_qry", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* DHCP query frequency */
	{ "dsl_ipaddr", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IP address */
	{ "dsl_netmask", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* netmask */
	{ "dsl_gateway", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* gateway */
	{ "dsl_dnsenable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_dns1", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_dns2", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_auth", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_idletime", "0", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_mtu", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_mru", "1492", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_service", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_ac", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_hostuniq", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_pppoe_options", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl_hwaddr", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

// those PVC need to init first so that QIS internet/IPTV PVC setting could write to NVRAM
	{ "dsl0_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl0_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl0_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl0_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl1_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl1_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl1_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl1_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl2_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl2_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl2_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl2_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl3_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl3_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl3_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl3_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl4_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl4_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl4_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl4_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl5_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl5_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl5_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl5_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl6_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl6_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl6_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl6_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl7_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_vpi", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_vci", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_encap", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_svc_cat", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_pcr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_scr", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_mbs", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsl7_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl7_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dsl7_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

// number of PVC , program generated
	{ "dslx_config_num", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
// for debug , program generated
	{ "dslx_debug", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_VDSL
	{ "dsl8_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8_proto", "dhcp", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.1_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.2_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.3_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.4_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.5_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.6_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_dot1p", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl8.7_mtu", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//
	{ "dsl9_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl9_proto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl9_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsl9_vid", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#endif

	// NVRAM for start_firewall/start_qos
	// QOS
	// Maybe removed later
	/* Usage:
	 * 	qos0_YYY ==> WAN0, qos1_YYY ==> WAN1, qos2_YYY ==> WAN2, except
	 * 	qos_type and qos_enable.
	 */
#if defined(RTCONFIG_MULTIWAN_CFG)
	{ "qos_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Last configured QoS */
#endif
	{ "qos_rulelist", "<Web Surf>>80>tcp>0~512>0<HTTPS>>443>tcp>0~512>0<File Transfer>>80>tcp>512~>3<File Transfer>>443>tcp>512~>3", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "qos_orates", "80-100,10-100,5-100,3-100,2-95,0-0,0-0,0-0,0-0,0-0", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"qos_irates", "100,100,100,100,100,0,0,0,0,0", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* "0: off */
#if defined(RTCONFIG_HNS) || defined(RTCONFIG_BWDPI) && !defined(MAPAC1750) && !defined(RTAC59_CD6R) && !defined(RTAC59_CD6N) && !defined(RTAX56_XD4) && !defined(XD4PRO) && !defined(CTAX56_XD4) &&!defined(RTAX53U) &&!defined(XD4S)  && !defined(XC5) && !defined(EBA63)
	{ "qos_type", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* "0: Traditional */
#else
	{ "qos_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Traditional */
#endif
	{ "qos_method", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_sticky", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_ack", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_syn", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_fin", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_rst", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_icmp", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_reset", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_obw", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_ibw", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DUALWAN)
	{ "qos_obw1", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* ibw/obw of WAN1 */
	{ "qos_ibw1", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif	/* RTCONFIG_DUALWAN */
	{ "qos_orules", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_burst0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_burst1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_default", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "qos_bw_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// TriggerList
	{ "autofw_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "autofw_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// VSList
	{ "vts_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vts_rulelist", "", CKN_STR8192, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MULTIWAN_CFG)
	/* If dualwan are enabled and wans_mode = lb,
	 * vts_rulelist defines virtual server of primary WAN and
	 * vts1_rulelist defines virtual server of secondary WAN.
	 * Otherwise, singlewan or dualwan + fo/fb,
	 * vts_rulelist defines virtual server of all WAN.
	 */
	{ "vts1_rulelist", "", CKN_STR8192, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "vts_upnplist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vts_ftpport", "2021", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	//Game VSList
#if defined(RTCONFIG_OPEN_NAT)
	{ "game_vts_rulelist", "", CKN_STR8192, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	// DMZ
	{ "dmz_ip", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MULTIWAN_CFG)
	/* If dualwan are enabled and wans_mode = lb,
	 * dmz_ip is exposed station of primary WAN and
	 * dmz1_ip is exposed station of secondary WAN.
	 * Otherwise, singlewan or dualwan + fo/fb,
	 * dmz_ip is exposed station of all WAN.
	 */
	{ "dmz1_ip", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "sp_battle_ips", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM for start_ddns
	{ "ddns_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_server_x", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_username_x", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_passwd_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_hostname_x", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_wildcard_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_INADYN
	{ "ddns_realip_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ddns_check_retry", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_regular_check", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_regular_period", "60", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_transfer", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_act", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_MULTIWAN_CFG) || defined(RTCONFIG_DUALWAN)
	/* If dualwan + load-balance is chosen, let user select WAN unit for DDNS.
	 * -1: Auto, select first/lowest connected WAN unit.
	 *  0: Select WAN unit 0.
	 *  1: Select WAN unit 1.
	 *  Otherwise: Same as -1.
	 * In dualwan + fail-over/fall-back or singlewan, always use primary WAN unit
	 * reported by wan_primary_ifunit() for DDNS.
	 */
	{ "ddns_wan_unit", "-1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_IPV6
	{ "ddns_ipv6_update", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// NVRAM for start_firewall
	// Firewall
#ifdef RTCONFIG_PARENTALCTRL
	{ "MULTIFILTER_ALL", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_ENABLE", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_MAC", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_DEVICENAME", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_MACFILTER_DAYTIME", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_PC_SCHED_V3
	{ "MULTIFILTER_MACFILTER_DAYTIME_V2", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_BLOCK_ALL", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "MULTIFILTER_URL_ENABLE", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_URL", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "MULTIFILTER_TMP", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_PC_REWARD
	{ "MULTIFILTER_REWARD", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "OPTUS_MULTIFILTER_ALL", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "OPTUS_MULTIFILTER_ENABLE", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "OPTUS_MULTIFILTER_MAC", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_ISP_OPTUS)
	{ "optus_url_whitelist", "cdn.optusdigital.com>messaging.optus.com.au>moa.optusnet.com.au>gateway.optus.com.au>branch-api.apps.aws.optus.com.au", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_HNS)
	{ "wrs_protect_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wrs_mals_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// mals switch
	{ "wrs_cc_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// cc   switch
	{ "wrs_vp_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// vp   switch
	{ "wrs_enable",		"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web content filter
	{ "wrs_rulelist",	"", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web content filter : <switch1>MAC1>Adult1>...<switch2>MAC2>Adult2>...
	{ "wrs_app_rulelist",	"", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// app patrol : <switch1>MAC1>Commercial1>...<switch2>MAC2>Commercial2>...
	{ "wrs_mals_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// mals timestamp for query
	{ "wrs_cc_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// cc   timestamp for query
	{ "wrs_vp_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// vp   timestamp for query
	{ "bwdpi_rsa_check",	"0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },// signature update check
	{ "bwdpi_wh_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web history
	{ "bwdpi_wh_stamp",	"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web history timestamp
	{ "sig_update_t",	"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// signature upgrade timestamp
	{ "TM_EULA",		"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// EULA of Trend Micro
	{ "bwdpi_app_rulelist", "9,20<8<4<0,5,6,15,17<4,13<13,24<1,3,14<7,10,11,21,23<", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_BWDPI)
	// TrendMicro module
	{ "wrs_protect_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// AiProtection v2.0 total switch
#if defined(RTCONFIG_BUSINESS)
	{ "wrs_mals_enable",    "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // mals switch
	{ "wrs_cc_enable",      "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // cc   switch
	{ "wrs_vp_enable",      "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   // vp   switch
#else
	{ "wrs_mals_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// mals switch
	{ "wrs_cc_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// cc   switch
#if defined(MAPAC1750) || defined(RTAC59_CD6R) || defined(RTAC59_CD6N)
	{ "wrs_vp_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// vp   switch
#else
	{ "wrs_vp_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// vp   switch
#endif
#endif
	{ "wrs_enable",		"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web content filter
	{ "wrs_rulelist",	"", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web content filter : <switch1>MAC1>Adult1>...<switch2>MAC2>Adult2>...
	{ "wrs_app_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// app patrol
	{ "wrs_app_rulelist",	"", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// app patrol : <switch1>MAC1>Commercial1>...<switch2>MAC2>Commercial2>...
	{ "wrs_mail_bit",	"7", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// alert mail option, default : mals + vp + cc
	{ "wrs_mals_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// mals timestamp for query
	{ "wrs_cc_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// cc   timestamp for query
	{ "wrs_vp_t",		"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// vp   timestamp for query
	{ "bwdpi_db_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// traffic analyzer switch
	{ "bwdpi_rsa_check",	"0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },// signature update check
	{ "bwdpi_alive",	"10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },// check dpi engine alive timeout
	{ "bwdpi_app_rulelist", "9,20<8<4<0,5,6,15,17<4,13<13,24<1,3,14<7,10,11,21,23<", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "bwdpi_sig_ver",	"", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },// dpi engine signature version
	{ "TM_EULA",		"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// EULA of Trend Micro
	{ "apps_analysis",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Apps Analysis in bandwidth monitor (Adaptive QoS first page)
	{ "bwdpi_wh_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web history
	{ "bwdpi_wh_stamp",	"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// web history timestamp
	{ "sig_update_t",	"0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// signature upgrade timestamp
	{ "bwdpi_game_list",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Mobile Game Mode device list
	{ "bwdpi_stream_list",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Mobile Stream Mode device list
	{ "bwdpi_wfh_list",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Mobile Work From Home device list
#endif
#endif	/* RTCONFIG_PARENTALCTRL */
#ifdef RTCONFIG_YANDEXDNS
	{ "yadns_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Disable */
	{ "yadns_mode", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Basic */
	{ "yadns_rulelist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* List client modes <devname>hh:ww:aa:dd:dd:rr>mode>enable... */
#endif /* RTCONFIG_YANDEXDNS */
	{ "fw_enable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_dos_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_log_x", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_pptp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_l2tp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_ipsec", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_rtsp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_h323", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_pt_sip", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(BRTAC828)
	{ "fw_pt_sip_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: kernel SIP ALG w/o Cisco; 1: OpenWRT SIP ALG w/ Cisco */
#endif
	{ "fw_pt_pppoerelay", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DUALWAN)
	{ "pppoerelay_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Select WAN unit if dualwan + load-balanced. */
#endif
	{ "misc_http_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "misc_httpport_x", "8080", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HTTPS
	{ "misc_httpsport_x", "8443", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "misc_ping_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "misc_lpr_x", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// UrlList
	{ "url_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "url_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "url_sched", "000000", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "url_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// KeywordList
	{ "keyword_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "keyword_sched", "000000", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "keyword_rulelist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// LWFilterListi
	{ "fw_lw_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lw_date_x", "1111111", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lw_time_x", "00002359", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lw_time2_x", "00002359", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lw_default_x", "ACCEPT", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lw_icmp_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_lwlist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "fw_wl_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wl_date_x", "1111111", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wl_time_x", "00002359", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wl_time2_x", "00002359", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wl_default_x", "ACCEPT", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wl_icmp_x", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "filter_wllist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// NVRAM for start_usb
	{ "usb_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_SOC_IPQ8074
	// for quad core CPU
	{ "usb_user_core", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_USB_XHCI
#if defined(MAPAC1300) || defined(MAPAC2200) || defined(VZWAC1300) || defined(SHAC1300)
/* MODEL DON'T USB USB3 but NEED XHCI */
#else
	{ "usb_usb3", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_XHCIMODE
	{ "xhcimode_waitsec", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif
	{ "usb_usb2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// remain default setting control as tomato
	{ "usb_uhci", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_INTERNAL_GOBI
	{ "usb_gobi", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RT4GAC86U)
	{ "modem_model", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // 0: gobi, 1: Quectel
	{ "modem_method", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RT4GAX56)
	{ "modem_model", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // 0: gobi, 1: Quectel
	{ "modem_method", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_INTERNAL_GOBI) && !defined(RTCONFIG_USB_MULTIMODEM)
	{ "usb_ohci", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "usb_ohci", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_irq_thresh", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_storage", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_printer", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_ext_opt", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_fat_opt", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_OPEN_NTFS3G_2016)
	{ "usb_ntfs_opt", "big_writes", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "usb_ntfs_opt", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_hfs_opt", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_fs_ext3", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_fs_fat", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_NTFS
	{ "usb_fs_ntfs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_PARAGON_NTFS)
	{ "usb_fs_ntfs_sparse", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_HFS
	{ "usb_fs_hfs", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_APFS
	{ "usb_fs_apfs", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_USB_CDROM
	{ "usb_fs_iso", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_fs_udf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_automount", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "script_usbhotplug", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "smbd_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "smbd_autoshare", "1" },
	{ "smbd_cpage", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "smbd_cset", "utf8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "smbd_custom", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "smbd_master", "1" },
//	{ "smbd_passwd", "" },
//	{ "smbd_shares", "share</mnt<Default Share<1<0>root$</Hidden Root<0<1" },
	{ "smbd_user", "nas", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "smbd_wgroup", "WORKGROUP" },
	{ "smbd_wins", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "log_ipaddr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "log_port", "514", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "log_size", "256", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "log_path", "/jffs", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "log_level", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* < LOG_INFO */
	{ "console_loglevel", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* < KERN_NOTICE */
	{ "noconsole", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#if defined(RTCONFIG_JFFS2) || defined(RTCONFIG_BRCM_NAND_JFFS2)
	{ "jffs2_on", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_UBIFS
	{ "ubifs_on", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_YAFFS
	{ "yaffs_on", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_FLASH_TYPE_EMMC
	{ "ext4fs_on", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_REBOOT_SCHEDULE
	{ "reboot_schedule_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "reboot_schedule", "10000000300", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_REBOOT_SCHEDULE_V2
	{ "reboot_schedule_month", "010300", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "reboot_schedule_type", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_USB
#ifndef RTCONFIG_PERMISSION_MANAGEMENT
	{ "acc_num", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "acc_list", "admin>admin", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#endif
	{ "st_samba_mode", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "st_samba_force_mode", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_FTP
	{ "st_ftp_mode", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "st_ftp_force_mode", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "enable_ftp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ftp_lang", "EN", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ftp_wanac", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_FTP_SSL
	{ "ftp_tls", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ftp_pasvport", "57530", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "enable_samba", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "enable_samba_tuxera", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "st_max_user", "5", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "computer_name", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "st_samba_workgroup", "WORKGROUP", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

//#ifdef RTCONFIG_WEBDAV
	{ "enable_webdav", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Disable */
	{ "st_webdav_mode", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: http */
	{ "webdav_proxy", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webdav_aidisk", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webdav_http_port", "8082", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webdav_https_port", "443", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "acc_webdavproxy", "admin>1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Only show USBDisk */
	{ "enable_webdav_captcha", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
	{ "enable_webdav_lock", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
	{ "webdav_acc_lock", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: unlock account */
	{ "webdav_lock_interval", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webdav_lock_times", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webdav_last_login_info", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif

//#ifdef RTCONFIG_CLOUDSYNC
	{ "enable_cloudsync", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cloud_sync", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "share_link_param", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "share_link_result", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "share_link_host", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif

#if defined(RTCONFIG_MULTILAN_CFG) && defined(RTCONFIG_CAPTIVE_PORTAL)
	{ "cp_type_rl",              "1>1<2>2<3>3<4>4",   CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp1_profile",			"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp1_local_auth_profile",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp2_profile",			"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp2_local_auth_profile",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp3_profile",			"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp3_local_auth_profile",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp4_profile",			"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp4_local_auth_profile",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp_mtu", "1496", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "chilli_mtu", "1496", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_CAPTIVE_PORTAL
	{ "captive_portal",			"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_2g",			"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g",			"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g_2",		"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_2g_if",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g_if",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g_2_if",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_2g_if_temp",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g_if_temp",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_5g_2_if_temp",	"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_enable",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_enable",		"off", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_idle_timeout",		"300", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_profile",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_local_clientlist",	"", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_wl_en",	"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_wl_dis",	"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_2g_ssid",			"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_5g_ssid",			"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_adv_5g_2_ssid",		"", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "captive_portal_passcode",		"", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_COOVACHILLI
	{"chilli_net", "192.168.182.0/24", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_CPFREE
	{"chilli_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HK_CPFREE
	{"chilli_radius", "cpfree.hk", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{"chilli_radius", "cpfree.com", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{"chilli_backup", "aroundwifi.com", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HK_CPFREE
	{"chilli_url", "http://www.cpfree.hk/loginIndex.action", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{"chilli_url", "http://www.cpfree.com/loginIndex.action", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{"chilli_pass", "cp27nlaz09s8a", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_uamsecret", "ua8k69fo92n", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#else
	{"chilli_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_radius", "127.0.0.1", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_backup", "127.0.0.1", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_url", "https://192.168.1.1/Uam", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_pass", "radius", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_uamsecret", "chillispot", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#endif
	{"chilli_nowifibridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_dns1", "8.8.8.8", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_interface", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_radiusnasid", "radius", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_uamanydns", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_uamallowed", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_macauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_macpasswd", "password", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_802.1Xauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_additional", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_protocol", "http", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_authport", "http", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_awaytime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_authtime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_authtype", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_sessiontime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_bandwidth", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_bandwidthMaxUp", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_bandwidthMaxDown", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"chilli_lease", "600", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* for captive portal */
	{"cp_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_interface", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_sessiontime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_awaytime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_authtime", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_authtype", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_radius", "127.0.0.1", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_backup", "127.0.0.1", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_radiusport", "1812", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_radiusauthport", "1812", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_radiusnasid", "radius", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_dns1", "8.8.8.8", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_uamsecret", "chillispot", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
	{"cp_radiussecret", "radius", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_uamanydns", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_uamallowed", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_nowifibridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_net", "192.168.183.0/24", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_macauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_macpasswd", "password", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_802.1Xauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_additional", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_protocol", "http", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_authport", "http", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_bandwidthMaxUp", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_bandwidthMaxDown", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"cp_lease", "600", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

        /* for hotspotsystem nvram*/
       	{"hotss_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_uamenable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_loginonsplash", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_customsplash", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_uamallowed", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_operatorid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_locationid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_interface", "eth1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_net", "192.168.182.0/24", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_customuam", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_customuamproto", "1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_remotekey", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"hotss_nowifibridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_FREERADIUS
	{"radius_serv_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"radius_serv_list", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	//NVRAM for fbwifi
#ifdef RTCONFIG_FBWIFI
	{ "fbwifi_enable", "off", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_2g", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_5g", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HAS_5G_2
	{ "fbwifi_5g_2", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_ssid_2_temp", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_auth_mode_x_2_temp", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_crypto_2_temp", "aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_wpa_psk_2_temp", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_macmode_2_temp", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_lanaccess_2_temp", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_expire_2_temp", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_5g_2_temp", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "fbwifi_secret", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_id", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_ssid", "ASUS-FBWiFi", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_auth_mode_x", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_crypto", "aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_ssid_0_temp", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_auth_mode_x_0_temp", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_crypto_x_0_temp", "aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_wpa_psk_0_temp", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_macmode_0_temp", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_lanaccess_0_temp", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_expire_0_temp", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_ssid_1_temp", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_auth_mode_x_1_temp", "open", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_crypto_1_temp", "aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_wpa_psk_1_temp", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_macmode_1_temp", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_lanaccess_1_temp", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_expire_1_temp", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_2g_temp", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_5g_temp", "off", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_http_port", "2060", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fbwifi_https_port", "2061", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_DISK_MONITOR
	{ "diskmon_policy", "disk", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* all */
	{ "diskmon_part", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* sda1 */
	{ "diskmon_force_stop", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */

	{ "diskmon_usbport", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 1 */
	{ "diskmon_freq", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
	{ "diskmon_freq_time", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DAY>WEEK>HOUR */
	{ "usb_path1_diskmon_freq", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_diskmon_freq_time", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_diskmon_freq", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_diskmon_freq_time", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if !defined(RT4GAC55U) && !defined(RT4GAC68U)
	{ "usb_path3_diskmon_freq", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_diskmon_freq_time", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "diskformat_file_system", "tfat", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, //tfat, tntfs, thfsplus
	{ "diskformat_label", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifndef RTCONFIG_BCMARM
	{ "diskremove_bad_device", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
#else
	{ "diskremove_bad_device", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif

#ifdef RTCONFIG_MEDIA_SERVER
	{ "dms_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 1
	{ "dms_rebuild", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "dms_rescan", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "dms_port", "8200", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dbdir", "/var/cache/minidlna", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dir", "/mnt", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_tivo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_stdlna", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_sas", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dir_x", "</mnt", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dir_type_x", "<AVP", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dir_manual", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_friendly_name", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "daapd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "daapd_friendly_name", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef DM
	{ "apps_dl", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_dl_share", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_dl_seed", "24", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_dms", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_caps", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_comp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_pool", "harddisk/part0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_share", "share", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_ver", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_seed", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_upload_max", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "apps_dms_usb_port", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_running", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_dl_share_port_from", "10001", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_dl_share_port_to", "10050", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "apps_installed", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "sh_num", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if !defined(RTCONFIG_BCMARM) && !defined(RTCONFIG_QCA)
	{ "apps_ipkg_old", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "apps_ipkg_old", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "apps_ipkg_server", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_wget_timeout", "30", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_local_space", "/rom", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined (RTAC1200) || defined (RTAC1200V2) || defined (RTAC1200GA1) || defined (RTAC1200GU) || defined(RTAC85U) || defined(RTAC85P) || defined(RTACRH26) || defined(TUFAC1750) || defined(RTACRH18)|| defined(RT4GAC86U) || defined(RT4GAX56)
	{ "apps_swap_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_swap_size", "67000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "apps_swap_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_swap_size", "33000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 32M */
#endif
	{ "apps_swap_threshold", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 72M */
	{ "apps_swap_file", ".swap", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_autofix", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "http_username", "admin", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DEMOUI
	{ "http_passwd", "demoui", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#else
	{ "http_passwd", "admin", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#endif
	{ "http_passwd_app", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
#ifdef RTCONFIG_DEMOUI
	{ "http_autologout", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "http_autologout", "30", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "http_client", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "http_clientlist", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_SECURE_BY_DEFAULT
	{ "http_enable", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0:http 1:https 2:both */
#else
	{ "http_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: http */
#endif
	{ "http_lanport", "80", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_HTTPS
	{ "https_lanport", "8443", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "https_crt_save", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "https_crt_gen", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "https_crt_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_last_info", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_FRS_LIVE_UPDATE
	{ "webs_update_trigger", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_update_ts", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_AUTO_FW_UPGRADE
	{ "webs_update_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_update_time", "02:00", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_BETA_UPGRADE
	{ "webs_update_beta", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
	{ "live_update_pause", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "temp_lang", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wollist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "httpd_die_reboot", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_DEMOUI
	{ "x_Setting", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is any setting set */
	{ "w_Setting", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is wilreess set */
#else
	{ "x_Setting", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is any setting set */
	{ "w_Setting", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is wilreess set */
#endif
	{ "r_Setting", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is repeater set */
	{ "p_Setting", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* is passwd set */
#if defined(RTCONFIG_WIFI_SON)
	{ "qis_Setting", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* QIS finish */
	{ "lyra_disable_wifi_drv", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disable wifi drv to reduce boot time */
	{ "lyra_ap_steering", "35", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wifison_ready", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* wifison flag */
#endif
#if defined(MAPAC2200) || defined(RTAC95U)
	{ "ncb_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* normal client blocking in backhaul */
#endif
	{ "asus_mfg", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "asus_mfg_flash", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_ASUSCTRL
	{ "asusctrl_flags", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "asusctrl_chg_sku", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "territory_code", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "btn_rst", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_ez", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_ez_radiotoggle", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Turn WPS into radio toggle */
#ifdef DSL_AX82U
	{ "btn_ez_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "btn_ez_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Turn WPS into led toggle. 0: wps */
#endif
	//TODO: maybe
	//1: led. led on/off
	//2: radio. Wifi radio on/off (btn_ez_radiotoggle=1) ?
	//3: reset to default (RTCONFIG_WPS_RST_BTN: RT-N11P) ?
#ifdef RTCONFIG_WIFI_TOG_BTN
	{ "btn_wifi_toggle", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_LED_BTN
	{ "btn_led", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_led_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTAC87U
	{ "LED_switch_count", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_TURBO_BTN
	{ "btn_turbo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(GTAC2900)
	{ "turbo_mode", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BOOST_AURA_RGB_SW */
#else
#if defined(GTAXE16000) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	{ "turbo_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },   /* LED on/off */
#else
	{ "turbo_mode", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BOOST_AURA_RGB_SW */
#endif
#endif
#endif
#if defined(RTCONFIG_SWMODE_SWITCH)
#if defined(PLAC66U)
	{ "switch_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTBE58_GO)
	{ "hw_switch_mode", "mode_0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* manually select specific port as WAN port */
#endif

	 /* APCLI/STA parameters */
#ifdef RTCONFIG_CHECK_PAP_WPAPSK
	{ "sta_band", "", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_bssid", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_auth_mode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_crypto", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sta_channel", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "record_lanaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#if (defined(RTCONFIG_BCM_MFG) && (defined(RPAX56) || defined(RPAX58) || defined(RPBE58))) || (defined(RTCONFIG_DEMOUI))
//	{ "telnetd_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#else
//	{ "telnetd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif
#ifdef RTCONFIG_SSH
#ifdef RTCONFIG_DEMOUI
	{ "sshd_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled 1: enabled 2: enabled for LAN only */
	{ "sshd_port", "1068", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "sshd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled 1: enabled 2: enabled for LAN only */
	{ "sshd_port", "22", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "sshd_pass", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sshd_authkeys", "", CKN_STR2999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "shell_username", "", CKN_STR20, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "shell_passwd", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
	{ "shell_timeout", "1200", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* telnet/ssh autologout idle timeout, sec. */
#ifdef RTCONFIG_POWER_SAVE
#if defined(RTCONFIG_SOC_IPQ8074)
	{ "pwrsave_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 1: auto (ondemand), 2: power save, otherwise: performance */
#else
	{ "pwrsave_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 1: auto (ondemand), 2: power save, otherwise: performance */
#endif
#endif
#if defined(RTCONFIG_SOC_IPQ8074)
	{ "pagecache_ratio", "90", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Percentage of maximum amount of page-cache. (15% ~ 90%) */
#endif
	{ "enable_acc_restriction", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "restrict_rulelist", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_USB_MODEM
	{ "stop_wait_usb_modules", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_qmi", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Dev3G", "AUTO", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_android", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
	{ "modem_autoapn", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
	{ "modem_roaming", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
	{ "modem_roaming_mode", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: automatically */
	{ "modem_roaming_isp", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* operator at the long format. */
	{ "modem_roaming_imsi", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MCC+MNC */
	{ "modem_roaming_scantime", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* second. */
	{ "modem_roaming_scanlist", "/tmp/cops.txt", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* station lists. */
	{ "modem_pincode", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_country", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_pdp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: PDP-IP(IPv4) */
	{ "modem_isp", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_spn", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_apn", "internet", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_dialnum", "*99#", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_authmode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_user", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_pass", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_apn_v6", "internet6", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_authmode_v6", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_user_v6", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_pass_v6", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_ttlsid", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_mode", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: auto */
	{ "modem_lte_band", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* auto */
	{ "modem_running", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_USB_MODEM_PIN
	{ "modem_pincode_opt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_USB_MODEM) && defined(RTCONFIG_INTERNAL_GOBI)
	{ "modem_sim_num", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi1", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi2", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi3", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi4", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi5", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi6", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi7", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi8", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi9", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sim_imsi10", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "modem_limit_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: GBytes */
	{ "modem_warning_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: GBytes */
	{ "modem_sms_limit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable */
	{ "modem_sms_phone", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sms_message1", "This is a alert about the data usage is over:", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_sms_message2", "The data usage has reached the limit:", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_reg_time", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_JFFS2) || defined(RTCONFIG_BRCM_NAND_JFFS2) || defined(RTCONFIG_UBIFS) || defined(RTCONFIG_JFFS_PARTITION)
	{ "modem_bytes_data_cycle", "31", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_bytes_data_limit", "0", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
	{ "modem_bytes_data_warning", "0", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
	{ "modem_bytes_data_save", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* the interval to save the data usage. need to be 30 times. */
#endif
#if defined(RT4GAC53U)
	{ "modem_mtu", "1480", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: auto */
#else
	{ "modem_mtu", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: auto */
#endif
	/* Cherry added temporarily or mobile broadband web implementation in 2014/8/20.*/
	{ "data_usage", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "data_usage_cycle", "30", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "data_usage_limit", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "data_usage_warning", "6", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_idletime", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_MODEM_BRIDGE
	{ "modem_bridge", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disabled */
#endif
#ifdef RTCONFIG_INTERNAL_GOBI
	{ "modem_gobi_path", RT_BUILD_NAME"_LTE", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "modem_tftp_size", "10240", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RT4GAC86U) || defined(RT4GAX56)
	{ "stop_atlock", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0  },
#endif
#endif

	{ "udpxy_enable_x", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "udpxy_clients", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "udpxy_if_alt", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	/* traffic monitor - added by jerry5 2009/07 */
	{ "rstats_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_path", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_new", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_stime", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_offset", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_data", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "rstats_colors", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_exclude", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "rstats_sshut", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rstats_bak", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "http_id", "TIDe855a6487043d70a", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "env_path", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "debug_logeval", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "debug_cprintf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "debug_cprintf_file", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "debug_logrc", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "debug_ovrc", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "debug_abrst", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(HND_ROUTER)
	{ "debug_cprintf", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "upnp_min_port_int", "1024", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_max_port_int", "65535", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_min_port_ext", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_max_port_ext", "65535", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "mfp_ip_monopoly", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_PPTPD) || defined(RTCONFIG_ACCEL_PPTPD)
	{ "pptpd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_broadcast", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_ms_network", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_chap", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0/1/2(Auto/MS-CHAPv1/MS-CHAPv2) */
	{ "pptpd_mppe", "13", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 1|4|8(MPPE-128|MPPE-40|No Encryption) */
	{ "pptpd_dns1", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_dns2", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_wins1", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_wins2", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_server", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_clients", "192.168.10.2-11", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_clientlist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
	{ "pptpd_mru", "1450", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_mtu", "1450", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pptpd_sr_rulelist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_VPNC)
	{ "vpnc_proto", "disable", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_pppoe_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_pppoe_passwd", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_heartbeat_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_dnsenable_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_defroute_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_pppoe_options_x", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_pptp_options_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_state_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_sbstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_clientlist", "", CKN_STR8192, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_connect_row", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_auto_conn", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_pptp_options_x_list", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_VPN_FUSION)
	{ "vpnc_default_wan", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_default_wan_tmp", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_unit", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_dev_policy_list", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_dev_policy_list_tmp", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_policy_unit", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpnc_max_conn", "2", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_PPTPD) || defined(RTCONFIG_ACCEL_PPTPD) || defined(RTCONFIG_OPENVPN)
	{ "VPNServer_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "VPNServer_mode", "pptpd", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "VPNClient_rule", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_VPNC) || defined(RTCONFIG_OPENVPN)
	{ "VPNClient_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "VPNClient_rule", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_OPENVPN
	{ "vpn_upload_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_upload_unit", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_upload_type", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// openvpn
	{ "vpn_loglevel", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_serverx_start", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT },	/* 0 */
//	{ "vpn_serverx_dns", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_serverx_clientlist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },

	{ "vpn_server_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_port", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "vpn_server_firewall", "auto", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_cipher", "AES-128-CBC", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "vpn_server_ncp_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_digest", "SHA1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_dhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_r1", "192.168.1.50", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_r2", "192.168.1.55", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_sn", "10.8.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPV6
	{ "vpn_server_sn6", "fd00:ac68:1::/64", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "vpn_server_local", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_remote", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_local6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_remote6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_ccd", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_c2c", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_ccd_excl", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_ccd_val", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_pdns", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_custom", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_igncrt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_tls_keysize", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_ip6", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_nat6", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_static", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_ca", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_ca_key", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_crt", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_key", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_client_crt", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_client_key", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_dh", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_crl", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_server_extra","", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},

	{ "vpn_client_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_clientx_eas", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_rg", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_digest", "SHA1", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_custom", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_password", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_static", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_ca", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_crt", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_key", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_crl", "", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_crt_client_extra","", CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},

	{ "vpn_client1_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_password", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_password", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client3_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_password", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_password", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_password", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_TPVPN
	{ "vpn_client_tp", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_tp_region", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_tp_proto", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

#ifdef RTCONFIG_NOTIFICATION_CENTER
	/* nt_center */
	{ "nc_ver", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nc_setting_conf", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nc_web_app_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nc_mail_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* actMail */
	{ "NOTIFY_MAIL_TO", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_ACCOUNT_BINDING
	{ "oauth_type", "" , CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oauth_auth_status", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oauth_user_email", "" , CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oauth_dm_user_ticket", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oauth_dm_refresh_ticket", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oauth_dm_cusid", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "aae_area", "" , CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "aae_portal", "" , CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "http_oauth_clientlist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_replace_status", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "aae_awsiot_desc_updated", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "aws_ca_download_status", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "oauth_user_id", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
    { "oauth_service_type", "", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_UPLOADER
	{ "uploader_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_PUSH_EMAIL) || defined(RTCONFIG_NOTIFICATION_CENTER)
	{ "PM_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable Push Mail feature. */
	{ "PM_type", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Send the tracking to the of xDSL team's official gmail. */
	{ "PM_SMTP_SERVER", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_SMTP_PORT", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_MY_NAME", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_MY_EMAIL", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_USE_TLS", "true", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_SMTP_AUTH", "LOGIN", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_SMTP_AUTH_USER", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_SMTP_AUTH_PASS", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
	{ "PM_MAIL_SUBJECT", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_LETTER_CONTENT", "", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_LETTER_PATH", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_MAIL_FILE", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_MAIL_TARGET", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_title", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The title of mail. */
	{ "PM_target", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The address of Mail Server. */
	{ "PM_restart", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* reset the Push Mail Service. */
	{ "PM_freq", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0:daily */
	{ "PM_mon", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* months since January (0 to 11). */
	{ "PM_day", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* days since Sunday (0 to 6 Sunday=0). */
	{ "PM_hour", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* hours since midnight (0 to 23). */
	{ "pushnotify_httplogin", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pushnotify_diskmonitor", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "PM_state", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_FRS_FEEDBACK)
	{ "fb_attach_syslog", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_attach_cfgfile", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_attach_modemlog", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_attach_wlanlog", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_country", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_email", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_contact_type", "", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_phone", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_ptype", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_pdesc", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_when_occur", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_which_band", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_unstable_conn", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_serviceno", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_tech_account", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_email_provider", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_comment", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_email_dbg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* send to email address */
	{ "fb_state", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_browserInfo", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_transid", "123456789ABCDEF0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_feedbackcount", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_total_size", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_split_files", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_req_cnt", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_gen_tarball", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DBLOG
	{ "dblog_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_tousb", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_adj_syslog", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_service", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_duration", "0", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_state", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_remaining", "0", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_usb_path", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_log_path", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_msqid_to_d", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dblog_transid", "0123456789ABCDEF", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_DBLOG */
#ifdef RTCONFIG_DSL
	// for web ui udentify , 0=notificaton, 1=close
	{ "fb_experience", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_ISP", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_Subscribed_Info", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_attach_iptables", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fb_availability", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ewan_dot1q", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ewan_vid", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ewan_dot1p", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_DSL */
#endif /* RTCONFIG_FRS_FEEDBACK */
#if defined(RTCONFIG_FRS_LIVE_UPDATE) && defined(RTCONFIG_AFC_POSITIONING)
	{ "afc_req_cnt", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_AHS
	{ "ahs_bhc_log", "/jffs/bhc_chg.log", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ahs_bhc_log_1", "/jffs/bhc_chg.log.1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_AHS */
#endif /* RTCONFIG_FRS_LIVE_UPDATE */
#ifdef RTCONFIG_DNS_PING
	{ "dns_ping_list", "<Cloudflare>1.1.1.1<Cloudflare>1.0.0.1<Google>8.8.8.8<Google>8.8.4.4<HiNet>168.95.1.1", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_ping_db_path", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_ping_tbl_name", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_ping_state", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef __CONFIG_NORTON__
	{ "nga_lickey", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_user", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_pass", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_devname", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_ncw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nga_gact", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "NGA_MID", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ngap_0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ngap_1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* __CONFIG_NORTON__ */

#ifdef RTCONFIG_IPV6
	// IPv6 parameters
	{ "ipv6_service", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disabled/ipv6pt/dhcp6/6in4/6to4/6rd/other */
	{ "ipv6_ifdev", "ppp", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_prefix", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The global-scope IPv6 prefix to route/advertise */
	{ "ipv6_prefix_length", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The bit length of the prefix. Used by dhcp6c. For radvd */
	{ "ipv6_dhcp_pd", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable DHCP-PD (DHCP Prefix Delegation) by default */
	{ "ipv6_rtr_addr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* defaults to $ipv6_prefix::1 */
	{ "ipv6_prefix_len_wan", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv6_ipaddr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv6_gateway", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv6_radvd", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable Router Advertisement (radvd) */
	{ "ipv6_relay", "192.88.99.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 Anycast Address */
	{ "ipv6_tun_v4end", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Foreign IPv4 endpoint of SIT tunnel */
	{ "ipv6_tun_addr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 address to assign to local tunnel endpoint */
	{ "ipv6_tun_addrlen", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* CIDR prefix length for tunnel's IPv6 address */
	{ "ipv6_tun_peer", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 address of remote tunnel endpoint */
	{ "ipv6_tun_mtu", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Tunnel MTU */
	{ "ipv6_tun_ttl", "255", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Tunnel TTL */
	{ "ipv6_6rd_dhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_6rd_router", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_6rd_ip4size", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_6rd_prefix", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_6rd_prefixlen", "32", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_SOFTWIRE46
	{ "ipv6_s46_peer", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* BR / AFTR IPv6 address */
	{ "ipv6_s46_addr6", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 address */
	{ "ipv6_s46_addr4", "", CKN_STR20, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv4 address */
	{ "ipv6_s46_offset", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID Offset */
	{ "ipv6_s46_psidlen", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID Length */
	{ "ipv6_s46_psid", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID */
	{ "ipv6_s46_mtu", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Tunnel MTU, 0 - auto */
	{ "ipv6_s46_elim", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Tunnel Encap Limit, empty - none */
	{ "ipv6_s46_ttl", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Tunnel Hop Limit, 0 - auto */
	{ "wan_s46_peer_x", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rule BR / AXFR IPv6 address */
	{ "wan_s46_prefix6_x", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rule IPv6 prefix */
	{ "wan_s46_prefix6len_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rule IPv6 prefix length */
	{ "wan_s46_prefix4_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Rule IPv4 prefix */
	{ "wan_s46_prefix4len_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Rule IPv4 prefix length */
	{ "wan_s46_ealen_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* EA-bit Length */
	{ "wan_s46_offset_x", "6", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID Offset */
	{ "wan_s46_psidlen_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID Length */
	{ "wan_s46_psid_x", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSID */
	/* DS-Lite */
	{ "ipv6_s46_aftr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* AFTR IPv6 address/Domain */
	{ "ipv6_s46_b4addr", "192.0.0.2", CKN_STR20, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* B4 address */
	{ "wan_s46_dslite_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Auto/Manual */
#endif
#if 0
	{ "ipv6_dns", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DNS server(s) IPs */
#else
	{ "ipv6_dns1", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_dns2", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_dns3", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ipv6_get_dns", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DNS IP address which got by dhcp6c */
	{ "ipv6_dnsenable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_get_domain", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* domain name which got by dhcp6c */
	{ "ipv6_debug", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_autoconf_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 LAN auconfiguration type */
	{ "ipv6_dhcp_lifetime", "86400", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* valid lifetimes of IPv6 prefix in seconds */
	{ "ipv6_dhcp_start", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* start of address pool */
	{ "ipv6_dhcp_end", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* end of address pool */
	{ "ipv6_fw_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default FORWARD table to DROP packets */
	{ "ipv6_fw_rulelist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 allowed forward rules */
	{ "ipv6_prefix_s", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 6in4 */
	{ "ipv6_prefix_length_s", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 6in4/other */
	{ "ipv6_rtr_addr_s", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 other */
	{ "ipv6_accept_defrtr", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Accept Default Route */
#if defined(RTAX82_XD6) || defined(RTAX82_XD6S) || defined(XD6_V2) || defined(ET12)
	{ "ipv6_only", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* IPv6 only */
#endif
#endif

#if defined(RTCONFIG_IPV6) && defined(RTCONFIG_DUALWAN)
	// IPv6 parameters
	{ "ipv61_service", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disabled/dhcp6/ipv6pt/6in4/6to4/6rd/other */
	{ "ipv61_ifdev", "ppp", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_prefix", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The global-scope IPv6 prefix to route/advertise */
	{ "ipv61_prefix_length", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* The bit length of the prefix. Used by dhcp6c. For radvd */
	{ "ipv61_dhcp_pd", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable DHCP-PD (DHCP Prefix Delegation) by default */
	{ "ipv61_rtr_addr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* defaults to $ipv6_prefix::1 */
	{ "ipv61_prefix_len_wan", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv61_ipaddr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv61_gateway", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* used in ipv6_service other */
	{ "ipv61_radvd", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable Router Advertisement (radvd) */
	{ "ipv61_relay", "192.88.99.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 Anycast Address */
	{ "ipv61_tun_v4end", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Foreign IPv4 endpoint of SIT tunnel */
	{ "ipv61_tun_addr", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 address to assign to local tunnel endpoint */
	{ "ipv61_tun_addrlen", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* CIDR prefix length for tunnel's IPv6 address */
	{ "ipv61_tun_peer", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 address of remote tunnel endpoint */
	{ "ipv61_tun_mtu", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Tunnel MTU */
	{ "ipv61_tun_ttl", "255", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Tunnel TTL */
	{ "ipv61_6rd_dhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_6rd_router", "0.0.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_6rd_ip4size", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_6rd_prefix", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_6rd_prefixlen", "32", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "ipv61_dns", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DNS server(s) IPs */
#else
	{ "ipv61_dns1", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_dns2", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_dns3", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ipv61_get_dns", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* DNS IP address which got by dhcp6c */
	{ "ipv61_dnsenable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv61_get_domain", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* domain name which got by dhcp6c */
	{ "ipv61_autoconf_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* IPv6 LAN auconfiguration type */
	{ "ipv61_dhcp_lifetime", "86400", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* valid lifetimes of IPv6 prefix in seconds */
	{ "ipv61_dhcp_start", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* start of address pool */
	{ "ipv61_dhcp_end", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* end of address pool */
	{ "ipv61_prefix_s", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 6in4 */
	{ "ipv61_prefix_length_s", "64", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 6in4/other */
	{ "ipv61_rtr_addr_s", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ipv6 other */
	{ "ipv61_accept_defrtr", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Accept Default Route */
#if defined(RTAX82_XD6) || defined(RTAX82_XD6S) || defined(XD6_V2) || defined(ET12)
	{ "ipv61_only", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* IPv6 only */
#endif
#endif

	{ "web_redirect", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* treat no phy, no internet as disconnect */
	{ "disiosdet", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nat_redirect_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_FANCTRL
	/* QCA:  0: auto, 1~3: state 1~3, -1: off
	 * BRCM: 0: auto, 1~4: state 1~4
	 */
	{ "fanctrl_dutycycle", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* OFF_TP_TEMP>OFF_TP_HYST<LOW_TP_TEMP>LOW_TP_HYST<MEDIUM_TP_TEMP>MEDIUM_TP_HYST<HIGH_TP_TEMP>HIGH_TP_HYST */
	{ "fanctrl_trip_points", "45>4<50>5<93>8<105>6", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* ONOFF<START_HOUR:START_MINUTE<END_HOUR:END_MINUTE , auto mode only. */
	{ "fanctrl_inact_time", "00:00<00:00", CKN_STR12, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_SHP
	{ "lfp_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_ISP_METER
	{ "isp_meter", "disable", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_limit", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_day_rx", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_day_tx", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_month_rx", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_month_tx", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_limit_time", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "isp_connect_time", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "Ate_version", "1.0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_power_on_off_ver", "2.4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_power_on_off_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_reboot_count", "100", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_rc_check", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_dev_check", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_boot_check", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_total_fail_check", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_dev_fail", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_boot_fail", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_total_fail", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Ate_continue_fail", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dev_fail_reboot", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef HND_ROUTER
	{ "Ate_check_delay", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#else
	{ "Ate_check_delay", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#if defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_QCA)
	{ "Ate_temp_cpu_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_2G_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_5G_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_cpu_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_2G_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_5G_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_cpu_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_2G_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_5G_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#ifdef RTCONFIG_HAS_5G_2
	{ "Ate_temp_5G2_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_5G2_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_5G2_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	{ "Ate_temp_6G_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_6G_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_6G_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#ifdef RTCONFIG_HAS_6G_2
	{ "Ate_temp_6G2_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_6G2_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_6G2_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#if defined(RTCONFIG_EXTPHY_BCM84880) \
 || defined(RTCONFIG_SWITCH_QCA8075_QCA8337_PHY_AQR107_AR8035_QCA8033)
	{ "Ate_temp_phy_max", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_phy_limit", "115", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "Ate_temp_phy_over_sec", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#if defined(RTCONFIG_WIFI_QCN5024_QCN5054) && !defined(RTCONFIG_SOC_IPQ60XX)
	{ "Ate_temp_run_in_phase1_t", "60", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#endif
#if defined(RTCONFIG_HND_ROUTER_AX)
	{ "Ate_runin_skip_stress", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	// Wireless parameters

#ifdef RTCONFIG_SNMPD
	{ "snmpd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sysName", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sysContact", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sysLocation", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "roCommunity", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rwCommunity", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "v3_auth_type", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "v3_priv_type", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "v3_auth_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "v3_priv_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_TIMEMACHINE
	{ "timemachine_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_device_name", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_vol_size", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_partition_num", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_ui_setting", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_usb_path_vid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_usb_path_pid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_usb_path_serial", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tm_debug", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_TR069)
#if defined(RT4GAX56)
	{ "tr_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_discovery", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_show_ui", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "tr_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_discovery", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "tr_inform_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_inform_interval", "86400", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_acs_url", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_username", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_passwd", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_conn_username", "admin", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_conn_passwd", "admin", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_conn_port", "7547", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_ca_cert", "", CKN_STR2999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_client_cert", "", CKN_STR2999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_client_key", "", CKN_STR2999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_client_key", "", CKN_STR2999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pvgcode", "ASUS", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tr_acs_acl", "", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "Ate_fw_fail", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_SOC_IPQ40XX)
	{ "Ate_reboot_delay", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "Ate_reboot_delay", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if (defined(RTCONFIG_USER_LOW_RSSI) || defined(RTCONFIG_NEW_USER_LOW_RSSI)) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
#if defined(RTCONFIG_AMAS)
	{ "wl_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* enabled by default for amas */
#else
	{ "wl_user_rssi", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disabled by default */
#endif
#ifndef RTCONFIG_BCMARM
	{ "wl_lrc", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wl_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* sensitivity level, 0:lazy, 1:normal, 2:sensitive */
#if defined(RTCONFIG_ADV_RAST) && !defined (RTAC3200) && !defined (RTAC66U)
	{ "wl_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* 0:rssi, 1:legacy */
	{ "wl_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* static client list */
	{ "rast_static_cli_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},	/* enable/disable static client list */
	{ "rast_aclist_timeout", "3", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* timeout of mac filter */
	{ "rast_weak_rssi_diff", "10", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* rssi delta to allow roam */
#endif
	{ "rast_idlrt", "20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* roaming assistant idle rate (Kbps) */
#endif
	{ "custom_clientlist", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for customize device name */
	{ "asus_device_list", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nmp_client_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ttl_inc_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable TTL increment */
	{ "ttl_spoof_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable TTL spoofing */
#ifdef RTCONFIG_TOR
	{ "Tor_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* enable Tor Transparent Proxy */
	{ "Tor_socksport", "9050", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Tor_transport", "9040", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Tor_dnsport", "9053", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "Tor_redir_list", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_JFFS2USERICON
	{ "custom_usericon", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "custom_usericon_del", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_CONNDIAG
	{ "custom_card_list", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ex_db_backup_enable", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ex_db_backup_path", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ex_db_backup_size", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "diag_ss_act", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif	
#ifdef RTCONFIG_TRAFFIC_LIMITER
	{ "tl_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl_cycle", "1", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl_date_start", "", CKN_STR512, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl0_alert_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl0_alert_max", "40", CKN_STR7, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl0_limit_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl0_limit_max", "45", CKN_STR7, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl1_alert_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl1_alert_max", "40", CKN_STR7, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl1_limit_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tl1_limit_max", "45", CKN_STR7, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_CLOUDCHECK
	{ "enable_cloudcheck", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_RESTRICT_GUI
	{ "fw_restrict_gui", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "fw_restrict_gui_mac", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_KEY_GUARD
	{ "kg_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_wan_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_powersaving_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_wl_radio_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_wl_radio", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_device_enable", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_devicename", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "kg_mac", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_WTFAST
	{ "wtf_username", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wtf_passwd", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wtf_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "guard_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPSEC
	{ "ipsec_profile_item",		"", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_1",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_2",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_3",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_4",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_5",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_list_1",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_list_2",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_list_3",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_list_4",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_list_5",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_1",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_2",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_3",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_4",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_5",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_server_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_client_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_1_ext",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_2_ext",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_3_ext",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_4_ext",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_5_ext",		"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_1_ext",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_2_ext",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_3_ext",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_4_ext",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_profile_client_5_ext",	"", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_log_level",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* 0: both internet and intranet are available, 1: only internet is available */
	{ "ipsec_block_intranet",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_INSTANT_GUARD
	{ "ipsec_ig_enable",	"0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ig_client_list",	"", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ig_guest_client_list",	"", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_IG_SITE2SITE
	{ "s2s_oauth_linklist",	"", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_SOC_IPQ8064)
	{ "ipsec_hw_crypto_enable",	"1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ipsec_isakmp_port", "500", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_nat_t_port", "4500", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_UPNPC_NEW
	{ "ipsec_isakmp_ext_port", "3500", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipsec_nat_t_ext_port", "4500", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_UPNPC_NEW */
#endif
#ifdef RTCONFIG_QUAGGA
	{ "quagga_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Disable */
	{ "zebra_passwd", "zebra", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },	/* Set password for vty interface. If there is no password */
	{ "zebra_enpasswd", "zebra", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },	/* Set enable password for VTY.(default: zebra) */
	{ "rip_hostname", "ripd", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Set hostname of the ripd.(default: ripd) */
	{ "rip_passwd", "zebra", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },	/* Set password for vty interface. If there is no password */
#endif
#if defined(RTCONFIG_QCA)
	/* 0: MSG_INFO (default debug level in hostapd/wpa_supplicant), 1: MSG_DEBUG, 2: MSG_MSGDUMP, 3: MSG_EXCESSIVE */
	{ "hapd_dbg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wsup_dbg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_SWITCH_RTL8370M_PHY_QCA8033_X2) || defined(RTCONFIG_SWITCH_RTL8370MB_PHY_QCA8033_X2)
	{ "lan_trunk_0", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_trunk_1", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_trunk_type", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: disable, 1: static, 2: LACP */
	{ "lan_hash_algorithm", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0: Source Port, 1: Layer2, 2: Layer2+3, 3: Layer3, 4: Layer3+4 */
#endif
#if defined(RTCONFIG_DUALWAN)
	{ "lb_skip_port", "<HTTPS>443>TCP<HTTPS>8443>TCP<QUIC>443>UDP<QUIC>80>UDP", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

//#if defined(RTCONFIG_PORT_BASED_VLAN)
	{ "vlan_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_AMAS_WGN) || defined(RTCONFIG_MULTILAN_CFG) || (defined(RTCONFIG_NOWL) && defined(RTCONFIG_AMAS))
#if defined(RTCONFIG_QUADBAND) && (defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2)) && (defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7))
	{ "vlan_rulelist",\
      // <enable>vid>prio>wanportset>lanportset>wl2gset>wl5gset>subnet_name>internet>public_vlan>type>
      "<1>501>0>0>FFFF>0002>0000>192.168.101.1/24>1>0>1>"\
      "<1>502>0>0>FFFF>0000>0002>192.168.102.1/24>1>0>1>"\
      "<1>503>0>0>FFFF>0000>00000002>192.168.103.1/24>1>0>1>"\
      "<1>504>0>0>FFFF>0000>000000000002>192.168.104.1/24>1>0>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
	},
	{ "subnet_rulelist",\
      // <ip>netmask>dhcp_enable>dhcp_start>dhcp_end>dhcp_lease>domainname>dns>wins>ema>macipbinding>type>
      "<192.168.101.1>255.255.255.0>1>192.168.101.2>192.168.101.254>86400>>>>>>1>"\
      "<192.168.102.1>255.255.255.0>1>192.168.102.2>192.168.102.254>86400>>>>>>1>"\
      "<192.168.103.1>255.255.255.0>1>192.168.103.2>192.168.103.254>86400>>>>>>1>"\
      "<192.168.104.1>255.255.255.0>1>192.168.104.2>192.168.104.254>86400>>>>>>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
	},
#elif !defined(RTCONFIG_QUADBAND) && (defined(RTCONFIG_HAS_5G_2) || defined(RTCONFIG_HAS_6G_2)) || defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
	{ "vlan_rulelist",\
      // <enable>vid>prio>wanportset>lanportset>wl2gset>wl5gset>subnet_name>internet>public_vlan>type>
      "<1>501>0>0>FFFF>0002>0000>192.168.101.1/24>1>0>1>"\
      "<1>502>0>0>FFFF>0000>0002>192.168.102.1/24>1>0>1>"\
      "<1>503>0>0>FFFF>0000>000000000002>192.168.103.1/24>1>0>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
	},
#else
    { "vlan_rulelist",\
      // <enable>vid>prio>wanportset>lanportset>wl2gset>wl5gset>subnet_name>internet>public_vlan>type>
      "<1>501>0>0>FFFF>0002>0000>192.168.101.1/24>1>0>1>"\
      "<1>502>0>0>FFFF>0000>0002>192.168.102.1/24>1>0>1>"\
      "<1>503>0>0>FFFF>0000>00000002>192.168.103.1/24>1>0>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
    },
#endif
	{ "subnet_rulelist",\
      // <ip>netmask>dhcp_enable>dhcp_start>dhcp_end>dhcp_lease>domainname>dns>wins>ema>macipbinding>type>
      "<192.168.101.1>255.255.255.0>1>192.168.101.2>192.168.101.254>86400>>>>>>1>"\
      "<192.168.102.1>255.255.255.0>1>192.168.102.2>192.168.102.254>86400>>>>>>1>"\
      "<192.168.103.1>255.255.255.0>1>192.168.103.2>192.168.103.254>86400>>>>>>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
	},
#else 
    { "vlan_rulelist",\
      // <enable>vid>prio>wanportset>lanportset>wl2gset>wl5gset>subnet_name>internet>public_vlan>type>
      "<1>501>0>0>FFFF>0002>0000>192.168.101.1/24>1>0>1>"\
      "<1>502>0>0>FFFF>0000>0002>192.168.102.1/24>1>0>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
    },
    { "subnet_rulelist",\
      // <ip>netmask>dhcp_enable>dhcp_start>dhcp_end>dhcp_lease>domainname>dns>wins>ema>macipbinding>type>
      "<192.168.101.1>255.255.255.0>1>192.168.101.2>192.168.101.254>86400>>>>>>1>"\
      "<192.168.102.1>255.255.255.0>1>192.168.102.2>192.168.102.254>86400>>>>>>1>"\
      "\0",\
      CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0\
    },
#endif
#else 	// RTCONFIG_AMAS_WGN || RTCONFIG_MULTILAN_CFG	
	{ "vlan_rulelist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "subnet_rulelist", "", CKN_STR5500, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif	// RTCONFIG_AMAS_WGN || RTCONFIG_MULTILAN_CFG
	{ "subnet_rulelist_ext", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vlan_pvid_list", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0  },
	{ "gvlan_rulelist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//#endif

#ifdef RTCONFIG_DISABLE_NETWORKMAP
	{ "networkmap_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if (defined(PLN12) || defined(PLAC56) || defined(PLAC66U))
	{ "plc_sleep_enabled", "1" },
#endif

#ifdef RTCONFIG_AUTOCOVER_SIP
	{ "atcover_sip", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "atcover_sip_ip", "192.168.1.1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "atcover_sip_type", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_LETSENCRYPT
	{ "le_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_ecc", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_auth", "http", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_renew_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_logpath", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_debug", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_acme_stage", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_CFGSYNC
	{ "cfg_wifi_quality", "-65", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, 0 },
	{ "cfg_group", "", CFGSYNC_GROUPID_LEN, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cfg_key", "", CFGSYNC_KEY_LEN, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cfg_alias", "", CFGSYNC_GROUPID_LEN, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_MASTER_DET
	{ "cfg_master", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "cfg_recount", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cfg_relist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cfg_relist_x", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BHCOST_OPT
	{ "cfg_tcso", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "cfg_tbrelist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_AMAS_CENTRAL_OPTMZ
	{ "cfg_opt_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
#ifdef RTCONFIG_AMAS_CENTRAL_ADS
	{ "cfg_ads_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
#endif
	{ "cfg_pause", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cfg_rekeylist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_BT_CONN
	{ "bt_wanport", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "bt_user_place", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_BT_CONN */
#ifdef RTCONFIG_LP5523
	{ "lp55xx_lp5523_user_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_user_col", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_user_beh", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_user_brightness", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_sch_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_sch", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_sch_col", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_sch_beh", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lp55xx_lp5523_sch_brightness", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(RTCONFIG_ZENWIFI_RGBLED)
	{ "lp55xx_lp5523_user_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif /* RTCONFIG_LP5523 */
#ifdef RTCONFIG_RGBLED
	{ "sb_flash_update", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },			/* 0: need to update 1: updated*/
	{ "aurargb_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Aura RGB ON/OFF */
	/* R,G,B,Mode,Speed,Direction; aurargb config by user, default: static red */
#if defined(GTAC2900)
	{ "aurargb_val", "255,0,0,5,0,0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "aurargb_val", "255,0,0,1,0,0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#if defined(RTCONFIG_AURASYNC)
	{ "aurasync_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* aurasync enalbe or not */
	{ "aurasync_set", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* if aurasync is set from server */
	{ "aurasync_val", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* last aurasync value from server */
#endif
#ifdef RTCONFIG_NVRAM_ENCRYPT
	{ "nvram_encrypt", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvram_encrypt_ver", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_HD_SPINDOWN
#ifdef LINUX26
	{ "usb_idle_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_idle_timeout", "300", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_idle_exclude", "", CKN_STR10, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

#ifdef RTCONFIG_TUNNEL
	{ "aae_disable_force", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_DNSQUERY_INTERCEPT//RTCONFIG_NFCM
	{ "nfcm_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_dpi_trf_analysis", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_dpi_apps_analysis", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dns_dpi_bwmon_calc", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_ROUTERBOOST
	{ "rb_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "re_rb_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "uiFlag", "1", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "rog_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* standalone gear accelerator enable */
	{ "rog_clientlist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ROG device */

#ifdef RTCONFIG_GEFORCENOW
	{ "nvgfn_enable"      , "1", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_scansuppress", "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_scaninterval", "120", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_ibw_r"       , "" , CKN_STR16 , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_obw_r"       , "" , CKN_STR16 , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_mcs"         , "1<6<vht<2", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_getudppacketdrop", "0", CKN_STR16  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nvgfn_ch_rulelist" , "audio>udp>49003<mic>udp>49004<video>udp>49005<control>tcp>49006", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_GN_WBL
	{ "wl_gn_wbl_enable" , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_gn_wbl_type"   , "1", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_gn_wbl_rule"   , "",  CKN_STR1024  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_ALEXA)
	{ "alexa_new_binding", "0" , CKN_STR1 , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "amazon_alexa_skill_user_id", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "label_mac", "" , CKN_STR_DEFAULT , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPERF3
	{ "iperf3_svr_port", "5201", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_svr_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_host", "192.168.1.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_port", "5201", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_time", "10", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_parallel", "2", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_window", "29200", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_omit", "1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_buf_len", "131072", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_interval", "2", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_reverse", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "iperf3_cli_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_CAPTCHA
	{ "captcha_enable"   , "1",  CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "httpd_captcha_list", "",  CKN_STR1024  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "httpd_force_lock", "",  CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef GTAX6000
	{ "antled_scheme"   , "1",  CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTAX82U) || defined(DSL_AX82U) || defined(GSAX3000) || defined(GSAX5400) || defined(TUFAX5400) || defined(GTAX11000_PRO) || defined(GTAXE16000) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTAX6000) || defined(GT10) || defined(RTAX82U_V2) || defined(TUFAX5400_V2) || defined(TUFAX6000) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7) || defined(GTBE96_AI) || defined(RTCONFIG_BCMLEDG)
#if defined(TUFAX5400) || defined(TUFAX5400_V2) || defined(GTAX6000) || defined(GT10) || defined(TUFAX6000) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7) || defined(GTBE96_AI)
	{ "ledg_scheme", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "ledg_scheme", "6", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(GSAX3000) || defined(GSAX5400)
	{ "ledg_rgb1", "0,100,128,0,50,128,0,0,128,15,0,128,30,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb2", "128,0,0,128,0,0,128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "0,0,128,0,0,128,0,0,128,0,0,128,0,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "0,0,128,0,0,128,0,0,128,0,0,128,0,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAXE16000) || defined(GTAX11000_PRO)
	{ "ledg_rgb1", "20,0,128,110,0,100,128,0,80", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb2", "148,0,128,148,0,128,148,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "20,0,128,110,0,100,128,0,80", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "20,0,128,110,0,100,128,0,80", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTBE98_PRO) || defined(GTBE98) || defined(GTBE96)
        { "ledg_rgb1", "100,0,0,120,0,30,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb2", "128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb3", "100,0,0,120,0,40,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb6", "100,0,0,120,0,30,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb7", "100,0,0,120,0,40,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
        { "ledg_rgb1", "100,0,0,120,0,30,100,0,128,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb2", "128,0,0,128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb3", "100,0,0,120,0,40,100,0,128,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb6", "100,0,0,120,0,30,100,0,128,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_rgb7", "100,0,0,120,0,40,100,0,128,100,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GTAX6000) || defined(GT7)
	{ "ledg_rgb1", "128,0,0,128,20,0,128,50,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb2", "128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GT10)
	{ "ledg_rgb1", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb2", "128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
	{ "ledg_rgb1", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb2", "128,128,128,128,128,128,128,128,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "128,0,10,128,0,40,128,0,100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif (TUFAX6000)
	{ "ledg_rgb2", "128,115,0,128,115,0,128,115,0,128,115,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb3", "128,115,0,128,115,0,128,115,0,128,115,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "ledg_rgb1", "0,100,128,0,50,128,0,0,128,30,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(TUFAX5400) || defined(TUFAX5400_V2)
	{ "ledg_rgb2", "128,55,0,128,55,0,128,55,0,128,55,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "ledg_rgb2", "128,0,0,128,0,0,128,0,0,128,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "ledg_rgb3", "0,0,128,0,0,128,0,0,128,0,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ledg_rgb7", "0,0,128,0,0,128,0,0,128,0,0,128", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(GTBE98_PRO) || defined(GTBE98) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(RTCONFIG_BCMLEDG_NIGHT)
        { "ledg_night_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(GTBE98_PRO) || defined(GTBE98) || defined(GTBE96)
        { "ledg_night_rgb1", "10,0,0,12,0,3,10,0,12", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb2", "1,0,0,1,0,0,1,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb3", "50,0,0,58,0,32,50,0,64", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb7", "50,0,0,58,0,32,50,0,64", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#elif (GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
        { "ledg_night_rgb1", "10,0,0,12,0,3,10,0,12,10,0,12", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb2", "1,0,0,1,0,0,1,0,0,1,0,0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb3", "50,0,0,58,0,32,50,0,64,50,0,64", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
        { "ledg_night_rgb7", "50,0,0,58,0,32,50,0,64,50,0,64", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_SDN_PRIORITY
	{ "ledg_sdn", "0<0<0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_SINGLE_LED_NIGHT_MODE
	{ "single_led_night_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_TURBO_BTN) && defined(RTCONFIG_BCM_CLED)
	{ "ledg_led_enable", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTAC86U) || defined(GTAC2900)
	{ "cpu_wait", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_TCPLUGIN
	{ "tencent_qmacc_enable"    , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tencent_eula_check"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

/* Merlin-specific settings */

	{ "ct_max", "300000", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, //All the models we support would default to this value
	{ "dns_fwd_local", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Forward queries for local domain to upstream DNS server
	{ "dhcp_dns2_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcpd_querylog", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcpd_dns_router", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcpd_send_wpad", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Send empty WPAD
	{ "wan0_mtu", "1500", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan1_mtu", "1500", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_refresh_x", "21", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Forced refresh period (in days)
	{ "smbd_master", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "log_level", "7", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },			// <  LOG_DEBUG
	{ "message_loglevel", "5", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// KERN_INFO
	{ "http_dut_redir", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      // 1: redir lan_ip to router.asus.com
	{ "dns_norebind", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		// dnsmasq DNS rebind protection
	{ "dns_priv_override", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Override DoH - 0: auto, 1: yes, 2: disable

// miniupnpd - PCP-related values
	{ "upnp_min_lifetime", "120", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_max_lifetime", "86400", CKN_STR6, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "upnp_pinhole_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_BCMARM
	{ "qos_overhead", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // Overhead, in bytes
	{ "qos_atm", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // 2 = PTM (Cake only), 1 = ATM, 0 = Ethernet
	{ "qos_mpu", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, // MPU overhead for Cake, in bytes
#endif

#ifdef RTCONFIG_DNSFILTER
#ifdef HND_ROUTER
       { "dnsfilter_rulelist1", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
       { "dnsfilter_rulelist2", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
       { "dnsfilter_rulelist3", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
       { "dnsfilter_rulelist4", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
       { "dnsfilter_rulelist5", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

#ifdef RTCONFIG_USB
	{ "smbd_simpler_naming", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "smbd_protocol", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 0 = SMB1, 1 = SMB2, 2 = SMB1+SMB2 */
#endif

#ifdef RTCONFIG_NFS
	{ "nfsd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nfsd_enable_v2", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nfsd_exportlist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_JFFS2) || defined(RTCONFIG_BRCM_NAND_JFFS2) || defined(RTCONFIG_UBIFS)
	{ "jffs2_scripts", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_UBIFS
	{ "ubifs_format", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "jffs2_format", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

#ifdef RTCONFIG_MEDIA_SERVER
	{ "dms_web","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_MERLINUPDATE
	{ "firmware_check_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_SSH
	{ "sshd_forwarding","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_OPENVPN
// Start instances of stock settings
	{ "vpn_client1_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_rg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client2_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_rg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client3_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_rg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client4_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_rg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client5_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_bridge", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_addr", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_connretry", "15", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_rg", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_cipher", "default", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_local", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_remote", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_reneg", "-1", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_adns", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_rgw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_gw", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_cn", "", CKN_STR255, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_tlsremote", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_userauth", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_useronly", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_server1_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_port", "1194", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_cipher", "AES-128-CBC", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_dhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_r1", "192.168.1.50", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_r2", "192.168.1.55", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_sn", "10.8.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPV6
	{ "vpn_server1_sn6", "fd00:ac68:1::/64", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "vpn_server1_local", "10.8.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_remote", "10.8.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_local6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_remote6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_ccd", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_c2c", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_ccd_excl", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_ccd_val", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_pdns", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_igncrt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_tls_keysize", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_ip6", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_nat6", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_server2_if", "tun", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_proto", "udp", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_port", "1195", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_crypt", "tls", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_comp", "-1", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_cipher", "AES-128-CBC", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_dhcp", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_r1", "192.168.1.50", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_r2", "192.168.1.55", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPV6
	{ "vpn_server2_sn6", "fd00:ac68:2::/64", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "vpn_server2_sn", "10.16.0.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_nm", "255.255.255.0", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_local", "10.16.0.1", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_remote", "10.16.0.2", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_local6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_remote6", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_hmac", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_ccd", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_c2c", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_ccd_excl", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_ccd_val", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_pdns", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_igncrt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_verb", "3", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_tls_keysize", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_ip6", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_nat6", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

// End instances

// These openvpn settings are unique to AM
	{ "vpn_server_ncp_ciphers",	DEFAULT_NCP_CIPHERS , CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_custom3",	 	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server_userpass_auth",	"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpndirector_rulelist",	""		, CKN_STR7999, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	// Client access:  0 = LAN, 1 = WAN, 2 = WAN+LAN
	{ "vpn_server_client_access",	"0"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "vpn_server1_client_access",	"0"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},
	{ "vpn_server2_client_access",	"0"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0},

	{ "vpn_server1_ncp_ciphers",	DEFAULT_NCP_CIPHERS, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_userpass_auth",	"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_custom3", 	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_ncp_ciphers",	DEFAULT_NCP_CIPHERS, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_userpass_auth",	"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "vpn_client_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_desc",		""		, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_enforce",		"0"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_custom3",		""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_desc", 		"Client 1"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_enforce","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_desc", 		"Client 2"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_enforce","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_desc", 		"Client 3"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_enforce","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_desc", 		"Client 4"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_enforce","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_ncp_ciphers",	DEFAULT_NCP_CIPHERS	, CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_digest",		"default"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_desc", 		"Client 5"	, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_enforce","0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_custom3",	""		, CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_fw",		"1"		, CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_IPV6
	{ "ipv6_dhcp6s_enable",	"1" , CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// DHCP6 Server for LAN
	{ "ipv6_ns_drop", "0" , CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Filter out neighbour solicitation flood on Comcast network
	{ "ipv6_dhcp6c_release", "1" , CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Should odhcp6c release prefix on exit
#ifdef RTCONFIG_DUALWAN
	{ "ipv61_dhcp6c_release", "1" , CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Should odhcp6c release prefix on exit
#endif
#endif

#ifdef RTCONFIG_SNMPD
	{ "snmpd_wan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_NTPD
	{ "ntpd_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ntpd_server_redir", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "amng_custom", "", CKN_STR8192, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	// Dummy variable
/* End of Merlin-specific settings */

	{ "tencent_download_enable"	, "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tencent_download_device"	, "", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tencent_download_folder"	, ".INO", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tencent_download_path"	, "", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_BCM_OAM
	//3ah
	{ "oam_3ah_enable"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_3ah_id"          , "1", CKN_STR10  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_auto_event"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_var_retrvl"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_link_event"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_remote_lb"       , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam_active_mode"     , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// 1731 or 1ag
	{ "oam0_srv_enable"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_mode"            , "1", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, //1:Y.1731, 0: 802.1ag
	{ "oam0_id"              , "ASUS", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_md_name"         , "asus", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_level"           , "2", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_lmep_id"         , "2", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_lmep_vid"        , "0", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_rmep_id"         , "0", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam0_ccm_itvl"        , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_srv_enable"      , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_mode"            , "1", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, //1:Y.1731, 0: 802.1ag
	{ "oam1_id"              , "ASUS1", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_md_name"         , "asus1", CKN_STR64  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_level"           , "3", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_lmep_id"         , "3", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_lmep_vid"        , "0", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_rmep_id"         , "0", CKN_STR4  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "oam1_ccm_itvl"        , "0", CKN_STR1  , CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef BCM_KF_NETFILTER
	{ "nat_type", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		//0: Symmetric  1: Full cone
#endif
#ifdef RTCONFIG_DNSFILTER
	{ "dnsfilter_enable_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dnsfilter_mode", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Default to no global filtering (only per client rules) */
	{ "dnsfilter_rulelist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* List client modes <devname>hh:ww:aa:dd:dd:rr>mode... */
	{ "dnsfilter_custom1", "8.8.8.8", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* User-defined DNS filter 1 */
	{ "dnsfilter_custom2", "8.8.8.8", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* User-defined DNS filter 2 */
	{ "dnsfilter_custom3", "8.8.8.8", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* User-defined DNS filter 3 */
#if defined(HND_ROUTER) && defined(RTCONFIG_IPV6)
	{ "dnsfilter_custom61", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* IPV6 User-defined DNS filter 1 */
	{ "dnsfilter_custom62", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* IPV6 User-defined DNS filter 2 */
	{ "dnsfilter_custom63", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* IPV6 User-defined DNS filter 3 */
#endif
#endif
#ifdef RTCONFIG_TIME_QUOTA
	{ "time_quota_profile", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Group_ID>Weekday>Time_quota(second)>Allowed_background_traffic (bytes per second)>Gentle pause (Limited bandwidth :bytes per second) */
#endif
#ifdef RTCONFIG_DNSSEC
	{ "dnssec_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dnssec_check_unsigned_x", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_TPVPN
	{ "tpvpn_provider", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_region", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_conntype", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_WIREGUARD
	{ "wgs_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_addr", "10.6.0.1/32", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_port", "51820", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_alive", "25", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_dns", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_nat6", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_psk", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_priv", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_pub", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs_lanaccess", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "wgsc_unit", "1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_addr", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_aips", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_caips", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_priv", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_pub", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_psk", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_name", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_caller", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgsc_extinfo", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ "wgc_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_desc", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_enforce", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_fw", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_addr", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_ep_addr", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_ep_port", "", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_alive", "25", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_aips", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_nat", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_dns", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_priv", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_ppub", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_psk", "", CKN_STR63, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_mtu", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_TPVPN
	{ "wgc_tp", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc_tp_region", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#ifdef RTCONFIG_NORDVPN
	{ "nordvpn_token", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_SURFSHARK
	{ "surfshark_username", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "surfshark_password", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "surfshark_otp", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_QCA_PLC2
	{ "cfg_plc_master", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* MAC address of the PLC master device */
#endif	/* RTCONFIG_QCA_PLC2 */
#ifdef RTCONFIG_MULTILAN_CFG
	/* idx, name, enable, vlan_idx, subnet_idx, apg_idx,
           vpnc_idx, vpns_idx, dnsf_idx, urlf_idx, nwf_idx, cp_idx, gre_idx, fw_idx, killsw_sw, ahs_sw, wan_idx, ppprelay_sw, wan6_idx, createby, mtwan_idx, mswan_idx, priority */
	{ "sdn_rl", "<0>DEFAULT>1>0>0>0>0>0>0>0>0>0>0>0>0>0>0>0>0>WEB>0>0>0", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vlan_rl", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "subnet_rl", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "radius_list", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp_type_rl", "1>1<2>2<3>3<4>4", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpns_rl", "<>WireGuard>1>0>1", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sdn_access_rl", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dhcpres_rl", "", CKN_STR5500, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "urlf_ctrl", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "urlf_rl", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nwf_ctrl", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nwf_rl", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	/* for remove */
	{ "sdn_rl_x", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vlan_rl_x", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "subnet_rl_x", "", CKN_STR_MAX, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "radius_list_x", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_GRE
	{ "l2gre_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_remote", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_vlanid", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_ka_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_ka_period", "10", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_ka_retries", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l2gre_state", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_unit", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_remote", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_addr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_routes", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_ka_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_ka_period", "10", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_ka_retries", "2", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "l3gre_state", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if defined(RTCONFIG_IFTTT) || defined(RTCONFIG_ALEXA) || defined(RTCONFIG_GOOGLE_ASST)
	{ "ifttt_token", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ifttt_stoken", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ifttt_timestamp", "", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(EBG15) || defined(EBG19)
	{ "wl0_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_excl_chans_base", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_excl_chans_cfg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_akm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_auth_mode", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_qualify_policy", "0 0x400 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_if_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x0020", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bss_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_closed", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_country_code", "US", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_country_rev", "634", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_crypto", "tkip+aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_key", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_key1", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_key2", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_key3", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_key4", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mode", "ap", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_owe_transition_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rrm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sched_v2_converted", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_vifs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_excl_chans_base", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_excl_chans_cfg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_akm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_auth_mode", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_qualify_policy", "0 0x200 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_if_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x0020", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bss_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_closed", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_country_code", "US", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_country_rev", "634", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_crypto", "tkip+aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_key", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_key1", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_key2", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_key3", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_key4", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mode", "ap", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_nband", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_owe_transition_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rrm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sched_v2_converted", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_vifs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_excl_chans_base", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_excl_chans_cfg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_akm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_auth_mode", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_if_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_sta_select_policy", "30 -82 0 0 0 1 1 0 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsd_steering_policy", "0 5 3 -82 0 0 0x20", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bss_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_closed", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_country_code", "US", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_country_rev", "634", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_crypto", "tkip+aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_key", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_key1", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_key2", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_key3", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_key4", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mode", "ap", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_nband", "4", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_owe_transition_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rrm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sched_v2_converted", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_vifs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_excl_chans_base", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_excl_chans_cfg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_akm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_auth_mode", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_if_qualify_policy", "0 0x0 -100", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_if_select_policy", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_sta_select_policy", "30 -62 0 0 0 1 1 0 0 0 0x122", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsd_steering_policy", "0 5 3 -62 0 0 0x22", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bss_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_closed", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_country_code", "US", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_country_rev", "634", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_crypto", "tkip+aes", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dtim", "1", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_he_features", "3", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_key", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_key1", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_key2", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_key3", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_key4", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mode", "ap", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_owe_transition_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rrm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sched_v2_converted", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_ssid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_vifs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_11ax", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_access_category_en", "2 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ap_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ap_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ap_inttrf_total_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ap_inttrf_total_thresh", "250", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_ahead", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_avoid_on_far_sta", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_enab", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_fallback_blocking_cac", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_idle_frames_thld", "36000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_bgdfs_txblank_threshold", "50", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_boot_only", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_chan_dwell_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_chan_flop_period", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_chanim_num_segments", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ci_scan_timeout", "300", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ci_scan_timer", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_cs_dfs_pref", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_cs_scan_timer", "900", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_dfs_reentry", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_dfsr_activity", "30 10240", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_dfsr_deferred", "604800 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_dfsr_immediate", "300 3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_excl_chans", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_excl_chans_valid", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_excl_chans_dfs", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_excl_chans_dfs_2", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_ignore_txfail", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_scan_entry_expire", "3600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_segment_chanim", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_sta_inttrf_numsecs", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_sta_inttrf_thresh", "150", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_start_on_nondfs", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_traffic_thresh_en", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acs_tx_idle_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_acsd_scs_dfs_scan", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_akm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_mpdu", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_rr_rtylimit_tid", "2 2 2 2 2 2 2 2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_rts", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ampdu_rtylimit_tid", "5 5 5 5 5 5 5 5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_antdiv", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ap_isolate", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_assoc_retry_max", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_atf", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_auth", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_auth_mode", "none", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_auth_mode_x", "open", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bcn", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bcn_rotate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bss_enabled", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bss_maxassoc", "128", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bss_opmode_cap_reqd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bsscolor", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_btc_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_160", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_cap", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_dl", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_enabled", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_switch_160", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_bw_ul", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_cevent", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_chanspec", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_closed", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_corerev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_country_code", "US", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_country_rev", "634", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_crypto", "tkip+aes", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dfs_bw_fallback", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dfs_pref", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dpp_conf_recvd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dpp_role", "configurator", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dtim", "", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_dyn160", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_expire", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_fbt_generate_local", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_frag", "2346", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_frameburst", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_frameburst_override", "on", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_gmode_protection", "auto", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_guest_num", "10", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_hwaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ifnames", "eth6 eth7 eth8 eth9", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_igs", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_infra", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_intfer_cnt", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_intfer_period", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_intfer_tcptxfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_intfer_txfail", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_itxbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_key", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_key1", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_key2", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_key3", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_key4", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_lanaccess", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_lazywds", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_lbr_aggr_en_mask", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_lbr_aggr_len", "16", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_lbr_aggr_release_timeout", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_leddc", "0x640000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_macmode", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_maxassoc", "128", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mbo_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mbss", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mcast_regen_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mfp", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mode", "ap", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mrate_x", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mu_features", "0x8000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_mumimo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_nband", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_nmcsidx", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_nmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_nmode_protection", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_nmode_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_noisemitigation", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_obss_coex", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_obss_dyn_bw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_oce_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ofdma", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_oob_rnr_list_src", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_optimizexbox", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_owe_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_owe_ptk_workaround", "1", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_phrase_x", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_phy_dyn_switch_th", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_phytype", "v", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_phytypes", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_plcphdr", "long", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_pmk_cache", "60", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_prev_mbss", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_probresp_mf", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_probresp_sw", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_psr_mbss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_psr_mrpt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_date_x", "1111111", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_pwrsave_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_pwrsave_level", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_time2_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radio_time_x", "00002359", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radioids", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius_key", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_radius_port", "1812", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rast_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rast_sens_level", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rate", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rateset", "ofdm", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_reg_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rifs_advert", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rts", "2347", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rx_amsdu_in_ampdu_ovr", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rxchain_pwrsave_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rxchain_pwrsave_pps", "10", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rxchain_pwrsave_quiet_time", "1800", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rxchain_pwrsave_stas_assoc_check", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rxstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sae_anti_clog_threshold", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sae_groups", "19 20 21", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sae_pwe", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sae_sync", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sched_v2_converted", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_ssid", SSID_PREFIX, CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sta_mpc_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sta_retry_time", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_stbc_rx", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_stbc_tx", "auto", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_subunit", "-1", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sync_node", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_taf_enable", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_timesched", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_trf_mgmt_rssi_policy", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_turbo_qam", "2", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_twt", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txbf", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txbf_bfe_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txbf_bfr_cap", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txbf_imp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txpower", "100", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txq_thresh", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_txstreams", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_unit", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_user_rssi", "-70", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_version", EPI_VERSION_STR, CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_vht_features", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_vhtmode", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_vifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_vlan_prio_mode", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wds_timeout", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wdsapply_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wdslist", "", CKN_STR256, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wep", "disabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wep_x", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wet_tunnel", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_ap_be", "15 63 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_ap_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_ap_vi", "7 15 1 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_ap_vo", "3 7 1 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_apsd", "on", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_bss_disable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_no_ack", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_sta_be", "15 1023 3 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_sta_bk", "15 1023 7 0 0 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_sta_vi", "7 15 2 6016 3008 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wme_sta_vo", "3 7 2 3264 1504 off off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wmf_bss_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wmf_igmpq_filter", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wmf_mdata_sendup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wmf_ucast_upnp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wmf_ucigmp_query", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wpa_gtk_rekey", "3600", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wpa_psk", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wpa_ptk_rekey", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wps_config_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wps_mode", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_wps_reg", "enabled", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl0_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl1_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl2_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl3_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_maclist", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_maclist_x", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_rast_static_client", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sched", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wl_sched_v2", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wans_extwan", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ifname_x", "eth0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "noconsole", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "psc6g", "0", CKN_STR2, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_version2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_config_method", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_modelname", "Wi-Fi Protected Setup Router", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_mfstring", "ASUSTeK Computer Inc.", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_sta_pin", "00000000", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wps_wer_mode", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "acs_def_plcy_intf_threshold", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "smart_connect_selif", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    
	{ "smart_connect_selif_x", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "firmware_path", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_wifi_toggle", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_turbo", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "turbo_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_s46_peer", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* BR / AFTR IPv6 address */
	{ "ipv6_s46_addr6", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* IPv6 address */
	{ "ipv6_s46_addr4", "", CKN_STR20, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* IPv4 address */
	{ "ipv6_s46_offset", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* PSID Offset */
	{ "ipv6_s46_psidlen", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* PSID Length */
	{ "ipv6_s46_psid", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },        /* PSID */
	{ "ipv6_s46_mtu", "0", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },         /* Tunnel MTU, 0 - auto */
	{ "ipv6_s46_elim", "", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },         /* Tunnel Encap Limit, empty - none */
	{ "ipv6_s46_ttl", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },         /* Tunnel Hop Limit, 0 - auto */
	{ "wan_s46_peer_x", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* Rule BR / AXFR IPv6 address */
	{ "wan_s46_prefix6_x", "", CKN_STR39, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* Rule IPv6 prefix */
	{ "wan_s46_prefix6len_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Rule IPv6 prefix length */
	{ "wan_s46_prefix4_x", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* Rule IPv4 prefix */
	{ "wan_s46_prefix4len_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* Rule IPv4 prefix length */
	{ "wan_s46_ealen_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },      /* EA-bit Length */
	{ "wan_s46_offset_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },     /* PSID Offset */
	{ "wan_s46_psidlen_x", "0", CKN_STR3, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },    /* PSID Length */
	{ "wan_s46_psid_x", "0", CKN_STR5, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },       /* PSID */
	{ "s2s_oauth_linklist", "", CKN_STR4096, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_AUTO_WANPORT
	{ "autowan_enable", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{"forget_it", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_SVR, 0 },
	{"app_mnt", "0", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"app_mnt_ts", "0", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"httpd_nonce_list", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"app_cnonce_list", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_GEARUPPLUGIN
	{"gearup_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"gearup_support", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"gearup_block", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_BCM_AFC
	{"afcd_mode", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"afcd_read_raw_loc", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ 0, 0, 0, 0, 0, 0, 0 }
}; // router_defaults

// nvram for system control state
struct nvram_tuple router_state_defaults[] = {
	{ "rc_service", "", CKN_STR128, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "asus_mfg", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_rst", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "btn_ez", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_WIFI_TOG_BTN
	{ "btn_wifi_toggle", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_WIRELESS_SWITCH
	{ "btn_wifi_sw", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_LED_BTN
	{ "btn_led", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RT4GAC55U
	{ "btn_lte", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "link_internet", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#if defined(RTCONFIG_QCA)
	{ "old_resolve", "1", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wan_primary", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Always first run in WAN port. */
	{ "wan0_primary", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DUALWAN) || defined(RTCONFIG_USB_MODEM)
	{ "wan1_primary", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wan_pppoe_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_state_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_sbstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* record substate for each wan state */
	{ "wan_auxstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* which is maintained by wanduck */
	{ "wan_proto_t", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_GETREALIP
	{ "wan_realip_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_realip_ip", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "nat_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "link_wan", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DUALWAN) || defined(RTCONFIG_USB_MODEM)
	{ "link_wan1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_AUTO_WANPORT
	{ "autowan_live_wanunit", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "autowan_detected_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_IPV6
	{ "wan_6rd_router", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_6rd_ip4size", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_6rd_prefix", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_6rd_prefixlen", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_state_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ipv6_sbstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "lan_state_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_sbstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* record substate for each wan state */
	{ "lan_auxstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* whcih is mtainted by wanduck */

	{ "autodet_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "autodet_auxstate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "invoke_later", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#if defined (RTCONFIG_WIRELESSREPEATER) || defined (RTCONFIG_PROXYSTA)
	// Wireless Client State
	{ "wlc_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlc_sbstate", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "wlc_scan_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 }, /* AM uses it in router mode too */
	{ "wlc_mode", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "wlc_scan_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_MEDIA_SERVER
	{ "dms_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dms_dbcwd", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_USBRESET
	{ "usbreset_active", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usbreset_num", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	// USB state
#if defined(RTCONFIG_USB_MODEM) && defined(RTCONFIG_INTERNAL_GOBI)
	{ "modem_sim_order", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if 0
	{ "usb_path1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_act", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_vid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_pid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_manufacturer", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_product", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_serial", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_speed", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_removed", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DISK_MONITOR
	{ "usb_path1_pool_error", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_path1_fs_path0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "usb_path1_fs_path1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_fs_path15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path1_label15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "usb_path2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_act", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_vid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_pid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_manufacturer", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_product", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_serial", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_speed", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_removed", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DISK_MONITOR
	{ "usb_path2_pool_error", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_path2_fs_path0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "usb_path2_fs_path1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_fs_path15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path2_label15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "usb_path3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_act", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_vid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_pid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_manufacturer", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_product", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_serial", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_speed", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_removed", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DISK_MONITOR
	{ "usb_path3_pool_error", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ "usb_path3_fs_path0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if 0
	{ "usb_path3_fs_path1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_fs_path15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label0", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label1", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label2", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label3", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label4", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label5", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label6", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label7", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label8", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label9", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label10", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label11", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label12", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label13", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label14", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "usb_path3_label15", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif

	{ "apps_dev", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_mounted_path", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_action", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_autorun", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_install", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_remove", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_switch", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_stop", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_enable", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_update", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_upgrade", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_state_error", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_download_file", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_download_percent", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_depend_do", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_depend_action", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apps_depend_action_target", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_DISK_MONITOR
	{ "diskmon_status", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "webs_state_update", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_upgrade", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_error", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_info", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_notif_flag", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if RTCONFIG_AI_SERVICE
	{ "webs_state_ai_update", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_ai_upgrade", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_ai_error", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_ai_info", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_ai_REQinfo", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_state_ai_flag", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_fw_version", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_odmpid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	// following is default config
	{ "ai_fw_path", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_lan_ip", "192.168.50.213", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_lan_netmask", "255.255.255.0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_lan_gateway", "192.168.50.1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_lan_proto", "static", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_ssh_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ai_dns", "192.168.50.1>8.8.4.4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#if RTCONFIG_ASUSCTRL
	{ "webs_chg_sku", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "webs_SG_mode", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "ftp_ports", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_OPENVPN
	{ "vpn_server1_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server1_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server2_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client1_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client2_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client3_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client4_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client5_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

#ifdef RTCONFIG_DSL
// name starting with 'dsl' are reserved for dsl unit
// for temp variable please use dsltmp_xxx
	{ "dsltmp_autodet_state", "down", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_autodet_vpi", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_autodet_vci", "33", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_autodet_encap", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_autodet_wan_type", "ATM", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
// manually config
	{ "dsltmp_cfg_country", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_ispname", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_vpi", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_vci", "35", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_prctl", "0", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_encap", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_dot1q", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_vid", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_rmvlan", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_mr", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_idx", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_num_pvc", "0", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_pvclist", "", CKN_STR2048, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_cfg_iptv_enable", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

// tmp variable for QIS , it will write to dsl0_xxx after finish page
	{ "dsltmp_qis_reboot", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* tmp for reboot flag before implement proper service without reboot */
	{ "dsltmp_qis_vpi", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_vci", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_proto", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_encap", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_dial_on_demand", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_idletime", "", CKN_STR32, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_mtu", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_mru", "", CKN_STR8, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_service", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_options", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_DHCPClient", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_ipaddr", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_netmask", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_gateway", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_dnsenable", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_dns1", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_dns2", "", CKN_STR15, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_svc_cat", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pcr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_scr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_mbs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_pppoe_relay", "", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_hwaddr", "", CKN_STR17, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_admin_username", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_admin_passwd", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/*tmp not enc till new qis*/
	{ "dsltmp_qis_admin_passwd_set", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_qis_dsl_pvc_set", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
// for DSL driver and tool
	{ "dsltmp_tc_resp_to_d", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_adslatequit", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_tcbootup", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_adslsyncsts", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_adslsyncsts_detail", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
// for web ui identify , 1=old ui, 2=asuswrt
	{ "dsltmp_web_ui_ver", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	log sync status
	{ "dsltmp_syncloss", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_syncloss_apply", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_syncup_cnt", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	dla for notification
	{ "dsltmp_dla_modified", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	diagnostic
	{ "dsltmp_diag_log_path", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_diag_confxdsl", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsltmp_syncup_short", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#if defined(RTCONFIG_DSL_BCM)
	{ "dsltmp_config_xtm", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#if defined(RTCONFIG_DSL_TCLINUX) || defined(RTCONFIG_DSL_BCM)
	{ "dsllog_fwver", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_drvver", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_linestate", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* dsltmp_adslsyncsts */
	{ "dsllog_opmode", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_adsltype", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_snrmargindown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_snrmarginup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_attendown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_attenup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_wanlistmode", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_dataratedown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_datarateup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_attaindown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_attainup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_powerdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_powerup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_crcdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_crcup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_farendvendorid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_pathmodedown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_pathmodeup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_interleavedepthdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_interleavedepthup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_xdslmode", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_tcm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_tcmdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_tcmup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_vdslcurrentprofile", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpshinedown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpshineup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpreindown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_inpreinup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_snrmpbds", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_snrmpbus", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_latndown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_latnup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_satndown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_satnup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL_TCLINUX
	{ "dsllog_sysvid", "2605443544e00", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_sysvmid", "00000000000000000000000000000000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_modemvid", "26005443434E5A01", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_DSL_BCM
	{ "dsllog_vectoringstate", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_fecdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_fecup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_hecdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_hecup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_esdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_esup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_sesdown", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dsllog_sesup", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#endif
#endif

#ifdef RTCONFIG_LETSENCRYPT
	{ "le_state", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_state_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_sbstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_auxstate_t", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_rc_notify", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "le_restart_httpd", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "ddns_cache", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_ipaddr", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_IPV6
	{ "ddns_ipv6_ipaddr", "", CKN_STR64, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_ipv6_service_old", "disabled", CKN_STR16, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* disabled/ipv6pt/dhcp6/6in4/6to4/6rd/other */
#endif
	{ "ddns_status", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_server_x_old", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_hostname_old", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_hostname_x_old", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_return_code", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_return_code_chk", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "ddns_last_wan_unit", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "reboot_time", "70", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

#ifdef RTCONFIG_TPVPN
	{ "tpvpn_provider", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_region", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_conntype", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_serverid", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_filename", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_state", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "tpvpn_errno", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif

	{ "app_access", "0", CKN_STR1, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wlnband_list", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};

// defined runtime create prefix nvram, used for nvram_restore_new()
struct nvram_tuple router_prefix_defaults[] = {
	{ "wl", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_server", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "vpn_client", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgs", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wgc", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_DSL
	{ "dsl", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
#ifdef RTCONFIG_MULTILAN_CFG
	{ "dhcpres", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apg", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "apm", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "urlf", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "nwf", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "dot", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "cp", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ 0, 0, 0, 0, 0, 0, 0 }
}; // End router_prefix_defaults

#ifdef REMOVE
const defaults_t if_generic[] = {
	{ "lan_ifname", "br0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_ifnames", "eth0 eth2 eth3 eth4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef DETWAN
	{ "wan_ifname", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ifnames", "", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#else
	{ "wan_ifname", "eth1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ifnames", "eth1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#endif
	{ 0, 0, 0, 0, 0, 0, 0 }
};

const defaults_t if_vlan[] = {
	{ "lan_ifname", "br0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "lan_ifnames", "vlan0 eth1 eth2 eth3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ifname", "vlan1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "wan_ifnames", "vlan1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};
#endif

#ifdef RTCONFIG_BCMWL6
#ifndef RTCONFIG_BCMARM
struct nvram_tuple bcm4360ac_defaults[] = {
	{ "pci/2/1/aa2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/aa5g", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/aga0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/aga1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/aga2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/agbg0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/agbg1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/agbg2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/antswitch", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/cckbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/cckbw20ul2gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/dot11agofdmhrbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/femctrl", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/papdcap2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tworangetssi2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pdgain2g", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/epagain2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tssiposslope2g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/gainctrlsph", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/papdcap5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tworangetssi5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pdgain5g", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/epagain5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tssiposslope5g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/maxp2ga0", "76", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/maxp2ga1", "76", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/maxp2ga2", "76", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/mcsbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/mcsbw402gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/measpower", "0x7f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/measpower1", "0x7f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/measpower2", "0x7f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl2ga0", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl2ga1", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl2ga2", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gha0", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gha1", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gha2", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gla0", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gla1", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gla2", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gma0", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gma1", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gma2", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gua0", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gua1", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/noiselvl5gua2", "31", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/ofdmlrbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pa2ga0", "0xfe72,0x14c0,0xfac7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pa2ga1", "0xfe80,0x1472,0xfabc", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pa2ga2", "0xfe82,0x14bf,0xfad9", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/pcieingress_war", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/phycal_tempdelta", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rawtempsense", "0x1ff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgainerr2g", "0xffff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgainerr5g", "0xffff,0xffff,0xffff,0xffff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gelnagaina0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gelnagaina1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gelnagaina2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrelnabypa0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrelnabypa1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrelnabypa2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrisoa0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrisoa1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/rxgains2gtrisoa2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/sar2g", "18", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/sar5g", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/sromrev", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/subband5gver", "0x4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tempcorrx", "0x3f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tempoffset", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/temps_hysteresis", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/temps_period", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tempsense_option", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tempsense_slope", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/tempthresh", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/txchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "pci/2/1/ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },

	{ 0, 0, 0, 0, 0, 0, 0 }
};
#else
#ifdef RTAC3200
struct nvram_tuple bcm4360ac_defaults[] = {
	{ "devpath1", "pcie/1/4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:devpath1", "sb/1/", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:boardrev", "0x1421", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:boardvendor", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:devid", "0x43bb", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:sromrev", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:boardflags", "0x20001000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:boardflags2", "0x00100002", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:venvid", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:boardflags3", "0x4000005", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:aa2g", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:agbg0", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:agbg1", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:agbg2", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:txchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:antswitch", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:femctrl", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tssiposslope2g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:epagain2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdgain2g", "21", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tworangetssi2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:papdcap2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:gainctrlsph", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempoffset", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rawtempsense", "0x1ff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempsense_slope", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempcorrx", "0x3f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempsense_option", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:xtalfreq", "40000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:phycal_tempdelta", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset2g40ma0", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset2g40ma1", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset2g40ma2", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset2g40mvalid", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset40ma0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset40ma1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset40ma2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset80ma0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset80ma1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:pdoffset80ma2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:cckbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:cckbw20ul2gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:dot11agofdmhrbw202gpo", "0x2000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:ofdmlrbw202gpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:dot11agduphrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:dot11agduplrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:maxp2ga0", "102", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gelnagaina0", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrisoa0", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:maxp2ga1", "102", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gelnagaina1", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrisoa1", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:maxp2ga2", "102", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gelnagaina2", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrisoa2", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:rxgains2gtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "devpath0", "pcie/1/3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:devpath0", "sb/1/", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:boardrev", "0x1421", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:boardvendor", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:devid", "0x43bc", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:sromrev", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:boardflags", "0x30040000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:boardflags2", "0x00220102", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:venid", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:boardflags3", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:aa5g", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:aga0", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:aga1", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:aga2", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:txchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:antswitch", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:femctrl", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tssiposslope5g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:epagain5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdgain5g", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tworangetssi5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:papdcap5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:gainctrlsph", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempthresh", "125", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempoffset", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rawtempsense", "0x1ff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempsense_slope", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempcorrx", "0x3f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempsense_option", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:xtalfreq", "40000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:phycal_tempdelta", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset40ma0", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset40ma1", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset40ma2", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset80ma0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset80ma1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdoffset80ma2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:subband5gver", "0x4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcsbw1605glpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcsbw1605gmpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcsbw1605ghpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcslr5glpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcslr5gmpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:mcslr5ghpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:dot11agduphrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:dot11agduplrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:maxp5ga0", "94,94,90,90", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:maxp5ga1", "94,94,90,90", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gmtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5ghtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxgains5gtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:maxp5ga2", "94,94,90,90", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "devpath2", "pcie/2/1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:devpath2", "sb/1/", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:boardrev", "0x1421", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:boardvendor", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:devid", "0x43bc", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:sromrev", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:boardflags", "0x30040000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:boardflags2", "0x00220102", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:venid", "0x14e4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:boardflags3", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:aa5g", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:aga0", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:aga1", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:aga2", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:txchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxchain", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:antswitch", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:femctrl", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tssiposslope5g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:epagain5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdgain5g", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tworangetssi5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:papdcap5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:gainctrlsph", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tempoffset", "255", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rawtempsense", "0x1ff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tempsense_slope", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tempcorrx", "0x3f", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:tempsense_option", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:xtalfreq", "40000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:phycal_tempdelta", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset40ma0", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset40ma1", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset40ma2", "4369", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset80ma0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset80ma1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:pdoffset80ma2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:subband5gver", "0x4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcsbw1605glpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcsbw1605gmpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcsbw1605ghpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcslr5glpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcslr5gmpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:mcslr5ghpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:dot11agduphrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:dot11agduplrpo", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gelnagaina0", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrisoa0", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:maxp5ga0", "90,90,106,106", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gelnagaina1", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:maxp5ga1", "90,90,106,106", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gmtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5ghtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gelnagaina2", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrisoa2", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:rxgains5gtrelnabypa2", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:maxp5ga2", "90,90,106,106", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "2:ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};
#elif defined(RTAC88U) || defined(RTAC3100) || defined(RTAC5300) || defined(GTAC5300) || defined(RTAX88U) || defined(RTAC86U) || defined(GTAC2900) || defined(GTAX11000) || defined(RTAX92U) || defined(RTAX95Q) || defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(RTAX86U) || defined(RTAX68U) || defined(GTAXE11000) || defined(RTAC68U_V4) || defined(RTAXE7800) || defined(GT10) || defined(RTAX9000)
struct nvram_tuple bcm4360ac_defaults[] = {
	{ 0, 0, 0, 0, 0, 0, 0 }
};
#elif defined(RTAC1200G) || defined(RTAC1200GP)
struct nvram_tuple bcm4360ac_defaults[] = {
	{ "0:xtalfreq", "20000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:opo", "68", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:aa2g", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ag0", "0x2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ag1", "0x2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ag2", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ag3", "0xff", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:bxa2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rssisav2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rssismc2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rssismf2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tri2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxpo2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:txchain", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:rxchain", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:antswitch", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tssipos2g", "0x1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "0:extpagain2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:pdetrange2g", "0x3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:triso2g", "0x4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:antswctl2g", "0x0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:elna2g", "0x2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:itt2ga0", "0x20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:itt2ga1", "0x20", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ledbh0", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ledbh1", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ledbh2", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:ledbh3", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:leddc", "0xFFFF", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:phycal_tempdelta", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempoffset", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/aa2g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/aa5g", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/aga0", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/aga1", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/aga2", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/txchain", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxchain", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/antswitch", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/femctrl", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/subband5gver", "4", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/gainctrlsph", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/papdcap5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/tworangetssi5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/pdgain5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "sb/1/epagain5g", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/tssiposslope5g", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/AvVmid_c0", "2,135,2,135,2,135,2,135,2,135", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/AvVmid_c1", "2,145,2,145,2,145,2,145,2,145", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/paparambwver", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "paparambwver", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/pdoffset5gsubbanda0", "0x0000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/pdoffset5gsubbanda1", "0x0000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5gelnagaina0", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5gtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "sb/1/rxgains5gtrisoa0", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5gmelnagaina0", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5gmtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "sb/1/rxgains5gmtrisoa0", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5ghelnagaina0", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5ghtrelnabypa0", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{ "sb/1/rxgains5ghtrisoa0", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "sb/1/rxgains5gelnagaina1", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rxgains5gtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{"sb/1/rxgains5gtrisoa1", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rxgains5gmelnagaina1", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rxgains5gmtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{"sb/1/rxgains5gmtrisoa1", "8", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rxgains5ghelnagaina1", "3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rxgains5ghtrelnabypa1", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
//	{"sb/1/rxgains5ghtrisoa1", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gl_c0", "-3,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gml_c0", "-4,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gmu_c0", "-4,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gh_c0", "-4,0,-6,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gl_c1", "-3,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gml_c1", "-4,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gmu_c1", "-4,0,-5,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_5gh_c1", "-4,0,-6,0,-5,0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_2g_c0", "4,5,4,5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/rssi_delta_2g_c1", "2,3,2,3", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/ledbh0", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/ledbh1", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/ledbh2", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/ledbh3", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/ledbh11", "0x7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/leddc", "0xFFFF", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/phycal_tempdelta", "15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{"sb/1/tempoffset", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{0, 0, 0 }
};
#else
struct nvram_tuple bcm4360ac_defaults[] = {
	{ "0:ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:ledbh10", "7", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:phycal_tempdelta", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "0:tempoffset", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:temps_period", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempthresh", "120", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:temps_hysteresis", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:phycal_tempdelta", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ "1:tempoffset", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};
#endif
#endif
#endif

#if defined(CONFIG_BCMWL5)
/* nvram override default setting for Media Router */
struct nvram_tuple router_defaults_override_type1[] = {
	{ "router_disable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* lan_proto=static lan_stp=0 wan_proto=disabled */
	{ "lan_stp", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* LAN spanning tree protocol */
	{ "wl_wmf_bss_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* WMF Enable for IPTV Media or WiFi+PLC */
	{ "wl_reg_mode", "h", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Regulatory: 802.11H(h) */
#ifndef RTCONFIG_BCM7
	{ "wl_taf_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Enable TAF */
	{ "wl_taf_rule", "0x15", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Default TAF rule on SSID, RATE and AID */

	/* EBOS feature Media router default */
	{ "wl_ebos_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* EBOS feature on */
	{ "wl_ebos_flags", "0x68", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* 104(0x68) video links */
	{ "wl_ebos_transit", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* transit limit for video links */
	{ "wl_ebos_prr_flags", "0xa41", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* pseudo-round robin data links */
	{ "wl_ebos_prr_threshold", "0x0f000000", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* pseudo-round robin threshold */
	{ "wl_ebos_prr_transit", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* pseudo-round robin transit limit */

	/* Airtime fairness */
	{ "wl_atf", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* ATF feature on */
#else
	{ "wl_taf_enable", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Disable TAF */
#endif
#ifdef __CONFIG_EMF__
	{ "emf_enable", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Enable EMF by default */
	{ "wl_wmf_ucigmp_query", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable Converting IGMP Query to ucast */
#ifdef RTCONFIG_BCMARM
	{ "wl_wmf_ucast_upnp", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable upnp to ucast conversion */
	{ "wl_wmf_igmpq_filter", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable igmp query filter */
#endif
#endif
#ifdef RTCONFIG_BCMWL6
	{ "wl_acs_fcs_mode", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable acsd fcs mode */
#ifndef RTCONFIG_BCM7
	{ "wl_acs_dfs", "2", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Enable first DFS chan Selection */
#endif
	{ "wl_dcs_csa_unicast", "1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Enable unicast CSA */
	/* Exclude ACSD to select 140l, 144u, 140/80, 144/80 to compatible with Ducati 11N */
	{ "wl_acs_excl_chans", "0xd98e,0xd88e,0xe28a,0xe38a", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },
#ifdef RTCONFIG_BCMARM
#ifndef RTCONFIG_BCM7
	{ "frameburst_dyn", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Frameburst controlled dynamically if on */
#endif
	{ "wl_pspretend_retry_limit", "5", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Enable PsPretend */
#ifndef RTCONFIG_BCM7
	{ "wl_pspretend_threshold", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Disable PsPretend Threshold */
#endif
#endif
#endif
#ifdef HND_ROUTER
	{ "wl_cal_period", "-1", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* Disable periodic cal */
#else
	{ "wl_cal_period", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Disable periodic cal */
#endif
	{ "wl_amsdu", "off", CKN_STR4, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Default IPTV AMSDU setting */
#ifdef RTCONFIG_BCMARM
#if !defined(RTCONFIG_BCM_7114) && !defined(HND_ROUTER)
	{ "wl_rx_amsdu_in_ampdu", "off", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },/* Media RX AMSDU In AMPDU setting */
#endif
#ifdef HND_ROUTER
	{ "wl_psta_inact", "600", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* PSTA inactivity timer */
#else
	{ "wl_psta_inact", "0", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* PSTA inactivity timer */
#endif
	{ "wl_srl", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Short Retry Limit */
	{ "wl_lrl", "11", CKN_STR_DEFAULT, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },		/* Long Retry Limit */
#endif
	{ "rog_clientlist", "", CKN_STR1024, CKN_TYPE_DEFAULT, CKN_ACC_LEVEL_DEFAULT, CKN_ENC_DEFAULT, 0 },	/* for ROG device */
	{ 0, 0, 0, 0, 0, 0, 0 }
};
#endif	/* CONFIG_BCMWL5 */

/* Translates from, for example, wl0_ (or wl0.1_) to wl_. */
/* Only single digits are currently supported */

static void
fix_name(const char *name, char *fixed_name)
{
	char *pSuffix = NULL;

	/* Translate prefix wlx_ and wlx.y_ to wl_ */
	/* Expected inputs are: wld_root, wld.d_root, wld.dd_root
	 * We accept: wld + '_' anywhere
	 */
	pSuffix = strchr(name, '_');

	if ((strncmp(name, "wl", 2) == 0) && isdigit(name[2]) && (pSuffix != NULL)) {
		strcpy(fixed_name, "wl");
		strcpy(&fixed_name[2], pSuffix);
		return;
	}

	/* No match with above rules: default to input name */
	strcpy(fixed_name, name);
}

extern char *tcode_default_get(const char *name);

/*
 * Find nvram param name; return pointer which should be treated as const
 * return NULL if not found.
 *
 * NOTE: This routine special-cases the variable wl_bss_enabled. It will
 * return the normal default value if asked for wl_ or wl0_. But it will
 * return 0 if asked for a virtual BSS reference like wl0.1_.
 */
char *
nvram_default_get(const char *name)
{
	int idx;
	char fixed_name[NVRAM_MAX_VALUE_LEN];
	char *lan_ipaddr;

#ifdef RTCONFIG_BCMBSD_V2
	int smartconnect_selif = nvram_get_int("smart_connect_selif");

	// router_defaults value is only for case selif(2G_5G1_5G2) in bcmbsd_def_policy-table-3
	if (smartconnect_selif > 0) {
		if (strstr(name, "_bsd_steering_policy") ||
	    	strstr(name, "_bsd_sta_select_policy") ||
	    	strstr(name, "_bsd_if_qualify_policy")) 
		{
			snprintf(fixed_name, sizeof(fixed_name), "%s_def", name);
			if(*nvram_safe_get(fixed_name))
				return nvram_safe_get(fixed_name);
		}
		if (strstr(name, "_bsd_if_select_policy"))
		{
			if(*nvram_safe_get(name))
				return nvram_safe_get(name);
		}
	}
#endif

#ifdef RTCONFIG_ISP_CUSTOMIZE
	char *value = find_customize_setting_by_name(name);
	if (value)
		return value;
#endif

	fix_name(name, fixed_name);
	if (strcmp(fixed_name, "wl_bss_enabled") == 0) {
		if (name[3] == '.' || name[4] == '.') { /* Virtual interface */
			return "0";
		}
	}

	if(!strcmp(fixed_name, "lan_ipaddr")){
		if((lan_ipaddr = nvram_get("IpAddr_Lan")) && strcmp(lan_ipaddr, "NONE")){
			return lan_ipaddr;
		}
	}

#ifdef RTCONFIG_BCMARM
#ifndef RTCONFIG_BCM7
#ifdef __CONFIG_HSPOT__
	if (strcmp(fixed_name, "wl_bss_hs2_enabled") == 0) {
		if (name[3] == '.' || name[4] == '.') { /* Virtual interface */
			return "0";
		}
	}
#endif /* __CONFIG_HSPOT__ */
#endif
#ifndef HND_ROUTER
	if (!strcmp(nvram_safe_get("devicemode"), "1")) {
		for (idx = 0; router_defaults_override_type1[idx].name != NULL; idx++) {
			if (strcmp(router_defaults_override_type1[idx].name, fixed_name) == 0) {
				return router_defaults_override_type1[idx].value;
			}
		}
	}
#endif
#endif

#ifdef RTCONFIG_TCODE
	/* varaible can be any, no skipping is possible */
	{
		char *value = tcode_default_get(fixed_name);
		if (value) {
			return value;
		}
	}
#endif

	/* check name wlx_xxx first */
	if ((strncmp(name, "wl", 2) == 0) && isdigit(name[2]) && (name[3] == '_'))
	for (idx = 0; router_defaults[idx].name != NULL; idx++) {
		if (strcmp(router_defaults[idx].name, name) == 0) {
			return router_defaults[idx].value;
		}
	}

	for (idx = 0; router_defaults[idx].name != NULL; idx++) {
		if (strcmp(router_defaults[idx].name, fixed_name) == 0) {
			return router_defaults[idx].value;
		}
	}

	return NULL;
}

#ifdef RTCONFIG_MSSID_PRELINK
int pass_prelink_interface(char *config_value)
{
	if(config_value == NULL)
		return 1;

	int prelink_unit = -1, prelink_subunit = -1;
	char prefix[]="wlXXXXXX_";

	prelink_unit = get_prelink_unit();

	if (nvram_get_int("re_mode") == 1)
		prelink_subunit = nvram_get_int("plk_re_subunit");
	else
		prelink_subunit = nvram_get_int("plk_cap_subunit");

	snprintf(prefix, sizeof(prefix), "wl%d.%d_", prelink_unit, prelink_subunit);

	if(!strncmp(config_value, prefix, strlen(prefix)))
		return 1;
	else
		return 0;
}
#endif

#if defined(CONFIG_BCMWL5)
/* validate/restore all per-interface related variables */
void
nvram_validate_all(char *prefix, bool restore)
{
	struct nvram_tuple *t;
	char tmp[100];
	char *v;

	return;

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strlcat_r(prefix, &t->name[3], tmp, sizeof(tmp));
			if (!restore && nvram_get(tmp))
				continue;
			v = nvram_get(t->name);
			nvram_set(tmp, v ? v : t->value);
		}
	}

#ifndef HND_ROUTER
	/* override router type1 nvram setting */
	if (!strcmp(nvram_safe_get("devicemode"), "1")) {
		for (t = router_defaults_override_type1; t->name; t++) {
			if (!strncmp(t->name, "wl_", 3)) {
				strlcat_r(prefix, &t->name[3], tmp, sizeof(tmp));
				if (!restore && nvram_get(tmp))
					continue;
				v = nvram_get(t->name);
				nvram_set(tmp, v ? v : t->value);
			}
		}
	}
#endif
}

void
nvram_initialize_all(char *prefix)
{
	struct nvram_tuple *t;
	char tmp[100];

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3) && (t->type & CKN_TYPE_VIF)) {
			strlcat_r(prefix, &t->name[3], tmp, sizeof(tmp));
			if (nvram_get(tmp))
				continue;
			nvram_set(tmp, t->value);
			break;
		}
	}
}

/* restore specific per-interface variable */
void
nvram_restore_var(char *prefix, char *name)
{
	struct nvram_tuple *t;
	char tmp[100];

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3) && !strcmp(&t->name[3], name)) {
			nvram_set(strlcat_r(prefix, name, tmp, sizeof(tmp)), t->value);
			break;
		}
	}

#ifndef HND_ROUTER
	/* override router type1 setting */
	if (!strcmp(nvram_safe_get("devicemode"), "1")) {
		for (t = router_defaults_override_type1; t->name; t++) {
			if (!strncmp(t->name, "wl_", 3) && !strcmp(&t->name[3], name)) {
				nvram_set(strlcat_r(prefix, name, tmp, sizeof(tmp)), t->value);
				break;
			}
		}
	}
#endif
}
#endif	/* CONFIG_BCMWL5 */
