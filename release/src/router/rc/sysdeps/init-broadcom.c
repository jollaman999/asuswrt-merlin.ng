/*

	Copyright 2005, Broadcom Corporation
	All Rights Reserved.

	THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
	KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
	SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.

*/

#include "rc.h"
#include "shared.h"
#include "version.h"
#include "interface.h"

#include <sched.h>
#include <termios.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <time.h>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <sys/klog.h>
#ifdef LINUX26
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#endif
#include <wlutils.h>
#include <bcmdevs.h>

#include <linux/major.h>

#ifdef RTCONFIG_CFGSYNC
#include <cfg_lib.h>
#endif

#ifdef RTCONFIG_BCMWL6
/* phy types, PhyVersion:PhyType field */
#define PHY_TYPE_A              0       /* A-Phy value */
#define PHY_TYPE_B              1       /* B-Phy value */
#define PHY_TYPE_G              2       /* G-Phy value */
#define PHY_TYPE_N              4       /* N-Phy value */
#define PHY_TYPE_LP             5       /* LP-Phy value */
#define PHY_TYPE_SSN            6       /* SSLPN-Phy value */
#define PHY_TYPE_HT             7       /* 3x3 HTPhy value */
#define PHY_TYPE_LCN            8       /* LCN-Phy value */
#define PHY_TYPE_LCNXN          9       /* LCNXN-Phy value */
#define PHY_TYPE_LCN40          10      /* LCN40-Phy value */
#define PHY_TYPE_AC             11      /* AC-Phy value */
#define PHY_TYPE_AX		13
#define PHY_TYPE_NULL           0xf     /* Invalid Phy value */

#ifdef RTCONFIG_HND_ROUTER_AX
#define WLCONF_PHYTYPE2STR(phy)	((phy) == PHY_TYPE_A ? "a" : \
				 (phy) == PHY_TYPE_B ? "b" : \
				 (phy) == PHY_TYPE_G ? "g" : \
				 (phy) == PHY_TYPE_HT ? "h" : \
				 (phy) == PHY_TYPE_AC ? "v" : \
				 (phy) == PHY_TYPE_LCN ? "c" : "n")
#else
#define WLCONF_PHYTYPE2STR(phy)	((phy) == PHY_TYPE_A ? "a" : \
				 (phy) == PHY_TYPE_B ? "b" : \
				 (phy) == PHY_TYPE_LP ? "l" : \
				 (phy) == PHY_TYPE_G ? "g" : \
				 (phy) == PHY_TYPE_SSN ? "s" : \
				 (phy) == PHY_TYPE_HT ? "h" : \
				 (phy) == PHY_TYPE_AC ? "v" : \
				 (phy) == PHY_TYPE_LCN ? "c" : "n")
#endif
#endif

#ifdef RTCONFIG_BCMFA
#include <sysdeps.h>
#endif

#ifdef RTCONFIG_BRCM_HOSTAPD
#include <sys/utsname.h> /* for uname */
#endif
#ifdef RTCONFIG_AMAS
#include <amas-utils.h>
#ifdef RTCONFIG_AMAS_CHANNEL_PLAN
#include <amas_path.h>
#endif
#ifdef RTCONFIG_AVBLCHAN
#include <libasuslog.h>
//#include <encrypt_main.h>
#define AVBL_DBG_LOG    "cfg_abl.log"

#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
#include <mxl_br_vlan.h>
#include <host_adapt.h>
#include <gsw_device.h>
#include <gsw.h>
#include <gsw_api.h>
#endif

#define RC_DBG_ABL(fmt, arg...) do {\
	if (!strcmp(nvram_safe_get("cfg_abl"), "1")) \
		syslog(LOG_NOTICE, "[%s(%d)]:"fmt"\n", __FUNCTION__, __LINE__ , ##arg); \
}while(0)

#endif
#endif

#ifdef RTCONFIG_BCMARM
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(BCA_HNDROUTER) && (defined(RTCONFIG_HND_WL) || defined(BCM4912))
void wl_thread_affinity_update(void);
#endif
#endif

#ifdef BCM_BSD
void smart_connect_sync_config(int unit);
#endif

int wan_phyid = -1;

#if defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XT8_V2) || defined(ET8PRO) || defined(ET8_V2)
#define GPY211_IF_NAME "eth0"
#endif

void init_devs(void)
{
}

void generate_switch_para(void)
{
	int model, cfg;
	char lan[SWCFG_BUFSIZE], wan[SWCFG_BUFSIZE];
#ifdef RTCONFIG_GMAC3
	char glan[2*SWCFG_BUFSIZE];
	char var[32], *lists, *next;

	int gmac3_enable = nvram_get_int("gmac3_enable");
	memset(glan, 0, sizeof(glan));
#endif

	// generate nvram nvram according to system setting
	model = get_model();

	if (is_routing_enabled()) {
		cfg = nvram_get_int("switch_stb_x");
		if (cfg < SWCFG_DEFAULT || cfg > SWCFG_STB34)
			cfg = SWCFG_DEFAULT;
#ifdef RTCONFIG_MULTICAST_IPTV
		if (cfg == 7)
			cfg = SWCFG_STB3;
#endif
	}
	/* don't do this to save ports */
	//else if (sw_mode() == SW_MODE_REPEATER ||
	//	((sw_mode() == SW_MODE_AP) && (nvram_get_int("wlc_psta")))
	//	cfg = SWCFG_PSTA;
	else if (sw_mode() == SW_MODE_AP
#if defined(RTCONFIG_AMAS)
		&& ((nvram_get_int("re_mode") == 0)
		|| (enable_ETH_U(0) == 0 && nvram_get_int("re_mode") == 1))
#endif
	)
		cfg = SWCFG_BRIDGE;
	else
		cfg = SWCFG_DEFAULT;	// keep wan port, but get ip from bridge

	switch(model) {
		/* BCM5325 series */
		case MODEL_APN12HP:
		{					/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 5 };
			/* TODO: switch_wantag? */

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
			//if (!is_routing_enabled())
			//	nvram_set("lan_ifnames", "eth0 eth1");	// override
			switch_gen_config(lan, ports, SWCFG_BRIDGE, 0, "*");
			switch_gen_config(wan, ports, SWCFG_BRIDGE, 1, "u");
			nvram_set("vlan0ports", lan);
			nvram_set("vlan1ports", wan);
#ifdef RTCONFIG_LANWAN_LED
			// for led, always keep original port map
			cfg = SWCFG_DEFAULT;
#endif
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, cfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
			break;
		}

		/* BCM5325 series */
		case MODEL_RTN14UHP:
		{
			/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 0, 1, 2, 3, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

#ifdef RTCONFIG_DUALWAN
			int wan1cfg = nvram_get_int("wans_lanport");

			nvram_unset("vlan1ports");
			nvram_unset("vlan1hwname");
			nvram_unset("vlan2ports");
			nvram_unset("vlan2hwname");

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
			// The first WAN port.
			if (get_wans_dualwan()&WANSCAP_WAN) {
				switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
				nvram_set("vlan1ports", wan);
				nvram_set("vlan1hwname", "et0");
			}

			// The second WAN port.
			if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
				wan1cfg += WAN1PORT1-1;
				if (wancfg != SWCFG_DEFAULT) {
					gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
					nvram_set("vlan0ports", lan);
					gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
					nvram_set("lanports", lan);
				} else {
					switch_gen_config(lan, ports, wan1cfg, 0, "*");
					nvram_set("vlan0ports", lan);
					switch_gen_config(lan, ports, wan1cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
				if (get_wans_dualwan()&WANSCAP_WAN) {
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				} else {
					nvram_set("vlan1ports", wan);
					nvram_set("vlan1hwname", "et0");
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				nvram_set("vlan0ports", lan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				nvram_set("lanports", lan);
			}

			int unit;
			char prefix[8], nvram_ports[16];

			for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
				memset(prefix, 0, 8);
				sprintf(prefix, "%d", unit);

				memset(nvram_ports, 0, 16);
				sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

				if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, NULL);
					nvram_set(nvram_ports, wan);
				} else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
					switch_gen_config(wan, ports, wan1cfg, 1, NULL);
					nvram_set(nvram_ports, wan);
				} else
					nvram_unset(nvram_ports);
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan0ports", lan);
			nvram_set("vlan1ports", wan);
#ifdef RTCONFIG_LANWAN_LED
			// for led, always keep original port map
			wancfg = SWCFG_DEFAULT;
#endif
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		/* BCM5325 series */
		case MODEL_RTN53:
		case MODEL_RTN12:
		case MODEL_RTN12B1:
		case MODEL_RTN12C1:
		case MODEL_RTN12D1:
		case MODEL_RTN12VP:
		case MODEL_RTN12HP:
		case MODEL_RTN12HP_B1:
		case MODEL_RTN10P:
		case MODEL_RTN10D1:
		case MODEL_RTN10PV2:
		{					/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan1ports");
				nvram_unset("vlan1hwname");
				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan1ports", wan);
					nvram_set("vlan1hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan0ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan0ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
					else {
						nvram_set("vlan1ports", wan);
						nvram_set("vlan1hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan0ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan0ports", lan);
				nvram_set("vlan1ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan0ports", lan);
			nvram_set("vlan1ports", wan);
#ifdef RTCONFIG_LANWAN_LED
			// for led, always keep original port map
			cfg = SWCFG_DEFAULT;
#endif
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_RTN10U:
		{					/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 0, 4, 3, 2, 1, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan1ports");
				nvram_unset("vlan1hwname");
				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan1ports", wan);
					nvram_set("vlan1hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan0ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan0ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
					else {
						nvram_set("vlan1ports", wan);
						nvram_set("vlan1hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan0ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan0ports", lan);
				nvram_set("vlan1ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan0ports", lan);
			nvram_set("vlan1ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		/* BCM53125 series */
		case MODEL_RTN15U:
		{					/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 8 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			int wan1cfg = nvram_get_int("wans_lanport");

			nvram_unset("vlan2ports");
			nvram_unset("vlan2hwname");
			nvram_unset("vlan3ports");
			nvram_unset("vlan3hwname");

			// The first WAN port.
			if (get_wans_dualwan()&WANSCAP_WAN) {
				switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
				nvram_set("vlan2ports", wan);
				nvram_set("vlan2hwname", "et0");
			}

			// The second WAN port.
			if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
				wan1cfg += WAN1PORT1-1;
				if (wancfg != SWCFG_DEFAULT) {
					gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
					nvram_set("vlan1ports", lan);
					gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
					nvram_set("lanports", lan);
				} else {
					switch_gen_config(lan, ports, wan1cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, wan1cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
				if (get_wans_dualwan()&WANSCAP_WAN) {
					nvram_set("vlan3ports", wan);
					nvram_set("vlan3hwname", "et0");
				} else {
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				nvram_set("vlan1ports", lan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				nvram_set("lanports", lan);
			}

			int unit;
			char prefix[8], nvram_ports[16];

			for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
				memset(prefix, 0, 8);
				sprintf(prefix, "%d", unit);

				memset(nvram_ports, 0, 16);
				sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

				if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, NULL);
					nvram_set(nvram_ports, wan);
				} else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
					switch_gen_config(wan, ports, wan1cfg, 1, NULL);
					nvram_set(nvram_ports, wan);
				} else
					nvram_unset(nvram_ports);
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_RTN16:
		{					/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 0, 4, 3, 2, 1, 8 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et0");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_RTAC3200:
		{					/* WAN L1 L2 L3 L4 CPU */	/*vision: WAN L4 L3 L2 L1 */
			int ports[SWPORT_COUNT] = { 0, 4, 3, 2, 1, 5 };
#ifdef RTCONFIG_GMAC3
			if (gmac3_enable)
				ports[SWPORT_COUNT-1] = 8;
#endif

			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0]; // record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et0");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}


		case MODEL_RTAC68U:						/* 0  1  2  3  4 */
		case MODEL_RTN18U:						/* 0  1  2  3  4 */
		case MODEL_RTAC53U:
		{				/* WAN L1 L2 L3 L4 CPU */	/*vision: WAN L1 L2 L3 L4 */
			const int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
			nvram_unset("mvlan_vid0");
			nvram_unset("mvlan_vid1");
			nvram_unset("mvlan");
#ifdef RTCONFIG_DUALWAN
#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 1 && enable_ETH_U(0) == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);

				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_set("mvlan_vid0", "1");
				nvram_set("mvlan_vid1", "2");
				nvram_set("mvlan", "1");
			}
			else
#endif
#ifdef RTAC68U
			if (is_dpsta_repeater() && !nvram_get_int("x_Setting"))
			{
				nvram_set("vlan1ports", "1 2 3 4 5*");
				nvram_set("vlan2ports", "0 5u");
				nvram_set("lanports", "1 2 3 4");
				nvram_set("wanports", "0");
			}
			else
#endif
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et0");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			} else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 1 && enable_ETH_U(0) == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);

				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_set("mvlan_vid0", "1");
				nvram_set("mvlan_vid1", "2");
				nvram_set("mvlan", "1");
			}
			else
#endif
			{
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "u");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
#endif
			break;
		}

		case MODEL_DSLAC68U:
		{
			const int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");
				int unit;
				char nv[16];
				int vid;

				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				switch_gen_config(wan, ports, wancfg, 1, "t");
				nvram_set("vlan2ports", wan);
				nvram_set("vlan2hwname", "et0");

				// port for DSL
				if (get_wans_dualwan()&WANSCAP_DSL) {
					switch_gen_config(wan, ports, wancfg, 1, "t");
					char buf[32];
					snprintf(buf, sizeof(buf), "%sports", DSL_WAN_VIF);
					nvram_set(buf, wan);
					snprintf(buf, sizeof(buf), "%shwname", DSL_WAN_VIF);
					nvram_set(buf, "et0");
				}

				// port for LAN/WAN
				if (nvram_match("ewan_dot1q", "1"))
					vid = nvram_get_int("ewan_vid");
				else
					vid = 4;
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, "t");
					snprintf(nv, sizeof(nv), "vlan%dports", vid);
					nvram_set(nv, wan);
					snprintf(nv, sizeof(nv), "vlan%dhwname", vid);
					nvram_set(nv, "et0");
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);

					snprintf(nv, sizeof(nv), "vlan%dports", vid);
					nvram_unset(nv);
					snprintf(nv, sizeof(nv), "vlan%dhwname", vid);
					nvram_unset(nv);
				}

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					if (unit == WAN_UNIT_FIRST)
						snprintf(nv, sizeof(nv), "wanports");
					else
						snprintf(nv, sizeof(nv), "wan%dports", unit);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nv, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nv, wan);
					}
					else
						nvram_unset(nv);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "t");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_RTAC87U:						/* 0  1  2  3  4 */
		{				/* WAN L1 L2 L3 L4 CPU */	/*vision: WAN L1 L2 L3 L4 */
			int ports[SWPORT_COUNT] = { 0, 5, 3, 2, 1, 7 };
#ifdef RTCONFIG_GMAC3
			if (gmac3_enable)
				ports[SWPORT_COUNT-1] = 8;
#endif
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et1");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et1");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et1");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_RTAC56S:						/* 0  1  2  3  4 */
		case MODEL_RTAC56U:
		{				/* WAN L1 L2 L3 L4 CPU */	/*vision: L1 L2 L3 L4 WAN  POW*/
			const int ports[SWPORT_COUNT] = { 4, 0, 1, 2, 3, 5 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et0");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}

		case MODEL_GTAC5300:
		case MODEL_RTAC86U:
		case MODEL_RTAX88U:
		case MODEL_BC109:
		case MODEL_BC105:
		case MODEL_EBG19:
		case MODEL_EBG15:
		case MODEL_EBP15:
		case MODEL_GTAX11000:
		case MODEL_RTAX92U:
		case MODEL_RTAX95Q:
		case MODEL_XT8PRO:
		case MODEL_BT12:
		case MODEL_BT10:
		case MODEL_BQ16:
		case MODEL_BQ16_PRO:
		case MODEL_BM68:
		case MODEL_XT8_V2:
		case MODEL_RTAXE95Q:
		case MODEL_ET8PRO:
		case MODEL_ET8_V2:
		case MODEL_RTAX56_XD4:
		case MODEL_XD4PRO:
		case MODEL_XC5:
		case MODEL_CTAX56_XD4:
		case MODEL_EBA63:
		case MODEL_RTAX58U:
		case MODEL_RTAX82U_V2:
		case MODEL_TUFAX5400_V2:
		case MODEL_RTAX5400:
		case MODEL_RTAX82_XD6S:
		case MODEL_XD6_V2:
		case MODEL_RTAX58U_V2:
		case MODEL_GT10:
		case MODEL_RTAX9000:
		case MODEL_RTAXE7800:
		case MODEL_TUFAX3000_V2:
		case MODEL_RTAX3000N:
		case MODEL_BR63:
		case MODEL_RTAX55:
		case MODEL_RTAX56U:
		case MODEL_RPAX56:
		case MODEL_RPAX58:
		case MODEL_RPBE58:
		case MODEL_RTBE58_GO:
		case MODEL_RTAX86U:
		case MODEL_RTAX68U:
		case MODEL_RTAC68U_V4:
		case MODEL_GTAXE11000:
		case MODEL_GTAX6000:
		case MODEL_GTAX11000_PRO:
		case MODEL_GTAXE16000:
		case MODEL_GTBE98:
		case MODEL_GTBE98_PRO:
		case MODEL_ET12:
		case MODEL_XT12:
		case MODEL_RTAX86U_PRO:
		case MODEL_RTAX88U_PRO:
		case MODEL_RTBE96U:
		case MODEL_GTBE96:
		case MODEL_RTBE88U:
		case MODEL_RTBE86U:
		case MODEL_RTBE58U:
		case MODEL_RTBE58U_V2:
		case MODEL_RTBE92U:
		case MODEL_RTBE95U:
		case MODEL_GTBE19000:
		case MODEL_RTBE82U:
		case MODEL_RTBE82M:
		case MODEL_RTBE58U_PRO:
		case MODEL_GTBE19000AI:
		case MODEL_GSBE18000:
		case MODEL_GT7:
		case MODEL_GTBE96_AI:
			break;

		case MODEL_RTAC5300:
		{
			char *hw_name = "et1";

#ifdef RTCONFIG_EXT_RTL8365MB
								/*vision:    (L5 L6 L7 L8)*/
			/* WAN L1 L2 L3 L4 (L5 L6 L7 L8) CPU */	/*vision: WAN L1 L2 L3 L4 */
			int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 5, 7 };
#else	// RTCONFIG_EXT_RTL8365MB
			/* WAN L1 L2 L3 L4 CPU */		/*vision: WAN L1 L2 L3 L4 */
			int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 7 };
#endif
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;
			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_GMAC3
			if (gmac3_enable) {
				ports[SWPORT_COUNT-1] = 8;
				hw_name = "et2";
			}
#endif

#ifdef RTCONFIG_DUALWAN
			if (get_wans_dualwan()&WANSCAP_WAN && get_wans_dualwan()&WANSCAP_LAN)
				nvram_set("vlan3hwname", hw_name);
			else
				nvram_unset("vlan3hwname");
			if (get_wans_dualwan()&WANSCAP_WAN || get_wans_dualwan()&WANSCAP_LAN)
				nvram_set("vlan2hwname", hw_name);
			nvram_set("vlan1hwname", hw_name);

#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 1 && enable_ETH_U(0) == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
			else
#endif
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan3ports");

				/* The first WAN port. */
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
				}

				/* The second WAN port. */
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN)
						nvram_set("vlan3ports", wan);
					else
						nvram_set("vlan2ports", wan);
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "u");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else	// RTCONFIG_DUALWAN
#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 1 && enable_ETH_U(0) == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
			else
#endif
			{
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "u");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan1hwname", hw_name);
				nvram_set("vlan2ports", wan);
				nvram_set("vlan2hwname", hw_name);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
#endif
			break;
		}

		case MODEL_RTAC3100:
		case MODEL_RTAC88U:
		{
			char *hw_name = "et0";

#ifdef RTCONFIG_EXT_RTL8365MB
			/* WAN L1 L2 L3 L4 (L5 L6 L7 L8) CPU */	/*vision: (L8 L7 L6 L5) L4 L3 L2 L1 WAN*/
			int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 5, 7 };
			hw_name = "et1";
#else	// RTCONFIG_EXT_RTL8365MB
			/* WAN L1 L2 L3 L4 CPU */	/*vision: WAN L1 L2 L3 L4 */
			int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 5 };
			hw_name = "et0";
#endif
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;
			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_GMAC3
			if (gmac3_enable) {
				ports[SWPORT_COUNT-1] = 8;
				hw_name = "et2";
			}
#endif

#ifdef RTCONFIG_DUALWAN
			if (get_wans_dualwan()&WANSCAP_WAN && get_wans_dualwan()&WANSCAP_LAN)
				nvram_set("vlan3hwname", hw_name);
			else
				nvram_unset("vlan3hwname");
			if (get_wans_dualwan()&WANSCAP_WAN || get_wans_dualwan()&WANSCAP_LAN)
				nvram_set("vlan2hwname", hw_name);
			nvram_set("vlan1hwname", hw_name);

#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 1 && enable_ETH_U(0) == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
			else
#endif
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan3ports");

				/* The first WAN port. */
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
				}

				/* The second WAN port. */
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN)
						nvram_set("vlan3ports", wan);
					else
						nvram_set("vlan2ports", wan);
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "u");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else	// RTCONFIG_DUALWAN
#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_DPSTA)
			if (dpsta_mode() && nvram_get_int("re_mode") == 1) {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
			}
			else
#endif
			{
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan1hwname", hw_name);
			nvram_set("vlan2ports", wan);
			nvram_set("vlan2hwname", hw_name);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
			}
#endif
			break;
		}

		case MODEL_RTN66U:
		case MODEL_RTAC66U:
		case MODEL_RTAC1200G:
		case MODEL_RTAC1200GP:
		{				/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 8 };
			int wancfg = (!nvram_match("switch_wantag", "none")&&!nvram_match("switch_wantag", "")&&!nvram_match("switch_wantag", "hinet")) ? SWCFG_DEFAULT : cfg;

			wan_phyid = ports[0];	// record the phy num of the wan port on the case
#ifdef RTCONFIG_DUALWAN
			if (cfg != SWCFG_BRIDGE) {
				int wan1cfg = nvram_get_int("wans_lanport");

				nvram_unset("vlan2ports");
				nvram_unset("vlan2hwname");
				nvram_unset("vlan3ports");
				nvram_unset("vlan3hwname");

				// The first WAN port.
				if (get_wans_dualwan()&WANSCAP_WAN) {
					switch_gen_config(wan, ports, wancfg, 1, (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4)?"":"u");
					nvram_set("vlan2ports", wan);
					nvram_set("vlan2hwname", "et0");
				}

				// The second WAN port.
				if (get_wans_dualwan()&WANSCAP_LAN && wan1cfg >= 1 && wan1cfg <= 4) {
					wan1cfg += WAN1PORT1-1;
					if (wancfg != SWCFG_DEFAULT) {
						gen_lan_ports(lan, ports, wancfg, wan1cfg, "*");
						nvram_set("vlan1ports", lan);
						gen_lan_ports(lan, ports, wancfg, wan1cfg, NULL);
						nvram_set("lanports", lan);
					}
					else {
						switch_gen_config(lan, ports, wan1cfg, 0, "*");
						nvram_set("vlan1ports", lan);
						switch_gen_config(lan, ports, wan1cfg, 0, NULL);
						nvram_set("lanports", lan);
					}

					switch_gen_config(wan, ports, wan1cfg, 1, (get_wans_dualwan()&WANSCAP_WAN)?"":"u");
					if (get_wans_dualwan()&WANSCAP_WAN) {
						nvram_set("vlan3ports", wan);
						nvram_set("vlan3hwname", "et0");
					}
					else {
						nvram_set("vlan2ports", wan);
						nvram_set("vlan2hwname", "et0");
					}
				} else {
					switch_gen_config(lan, ports, cfg, 0, "*");
					nvram_set("vlan1ports", lan);
					switch_gen_config(lan, ports, cfg, 0, NULL);
					nvram_set("lanports", lan);
				}

				int unit;
				char prefix[8], nvram_ports[16];

				for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
					memset(prefix, 0, 8);
					sprintf(prefix, "%d", unit);

					memset(nvram_ports, 0, 16);
					sprintf(nvram_ports, "wan%sports", (unit == WAN_UNIT_FIRST)?"":prefix);

					if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_WAN) {
						switch_gen_config(wan, ports, wancfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
						switch_gen_config(wan, ports, wan1cfg, 1, NULL);
						nvram_set(nvram_ports, wan);
					}
					else
						nvram_unset(nvram_ports);
				}
			}
			else {
				switch_gen_config(lan, ports, cfg, 0, "*");
				switch_gen_config(wan, ports, wancfg, 1, "");
				nvram_set("vlan1ports", lan);
				nvram_set("vlan2ports", wan);
				switch_gen_config(lan, ports, cfg, 0, NULL);
				switch_gen_config(wan, ports, wancfg, 1, NULL);
				nvram_set("lanports", lan);
				nvram_set("wanports", wan);
				nvram_unset("wan1ports");
			}
#else
			switch_gen_config(lan, ports, cfg, 0, "*");
			switch_gen_config(wan, ports, wancfg, 1, "u");
			nvram_set("vlan1ports", lan);
			nvram_set("vlan2ports", wan);
			switch_gen_config(lan, ports, cfg, 0, NULL);
			switch_gen_config(wan, ports, wancfg, 1, NULL);
			nvram_set("lanports", lan);
			nvram_set("wanports", wan);
#endif
			break;
		}
	}

#ifdef RTCONFIG_GMAC3
	/* gmac3 override */
	if (nvram_get_int("gmac3_enable") == 1) {
		lists = nvram_safe_get("vlan1ports");
		strncpy(glan, lists, strlen(lists));

		foreach(var, lists, next) {
			if (strchr(var, '*') || strchr(var, 'u')) {
				remove_from_list(var, glan, sizeof(glan));
				break;
			}
		}
		/* add port 5, 7 and 8* */
		add_to_list("5", glan, sizeof(glan));
		add_to_list("7", glan, sizeof(glan));
		add_to_list("8*", glan, sizeof(glan));
		nvram_set("vlan1ports", glan);
	}
#endif
}

void enable_jumbo_frame(void)
{
	int enable = nvram_get_int("jumbo_frame_enable");
	char word[256], ifnames[256], *next;

	if (!nvram_contains_word("rc_support", "switchctrl"))
		return;

	strlcpy(ifnames, nvram_safe_get("lan_ifnames"), sizeof(ifnames));
	if (!enable)
		foreach(word, ifnames, next)
			if (!strncmp(word, "eth", 3))
				doSystem("ifconfig %s mtu 1500", word);

#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(GTBE96_AI)
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (is_rtl8372_boardid())
#endif
	if (enable)
		system("rtkswitch 18");
	else
		system("rtkswitch 17");
#endif
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	if (enable)
		system("fapi-GSW-CfgSet nMaxPacketLen=10000");
	else
		system("fapi-GSW-CfgSet nMaxPacketLen=1532");
#endif
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
#if defined(BCM6750) || defined(BCM6756) || defined(BCM4906_504) || defined(BCM6765)
	int model = get_model();
	switch(model) {
	case MODEL_TUFAX3000_V2:
	case MODEL_RTAXE7800:
	case MODEL_ET8PRO:
	case MODEL_ET8_V2:
	case MODEL_XT8_V2:
		/* external BCM53134 */
		eval("ethswctl", "-c", "pmdioaccess", "-x", "0x4005", "-l", "2", "-d", enable ? "0x2600" : "0x5f4");
		/* fall through */
	case MODEL_XD4PRO:
	case MODEL_XC5:
	case MODEL_EBA63:
	case MODEL_RPAX58:
	case MODEL_RPBE58:
	case MODEL_RTAX3000N:
	case MODEL_BR63:
	case MODEL_RTAX82U_V2:
	case MODEL_TUFAX5400_V2:
	case MODEL_RTAX5400:
	case MODEL_XD6_V2:
	case MODEL_RTBE58U:
	case MODEL_RTBE58U_V2:
	case MODEL_RTBE92U:
	case MODEL_RTBE95U:
	case MODEL_RTBE82U:
	case MODEL_RTBE82M:
	case MODEL_RTBE58U_PRO:
	case MODEL_GSBE18000:
	case MODEL_GT7:
		/* BCM6750 / BCM6756 SF2 */
		eval("ethswctl", "-c", "regaccess", "-v", "0x4005", "-l", "2", "-d", enable ? "0x2600" : "0x5f4", "-n", "0");
		break;
	case MODEL_XT8PRO:
	case MODEL_BM68:
	case MODEL_RTBE58_GO:
		eval("ethswctl", "-c", "regaccess", "-v", "0x4005", "-l", "2", "-d", enable ? "0x2600" : "0x7d0", "-n", "0");
		eval("ethswctl", "-c", "pmdioaccess", "-x", "0x4005", "-l", "2", "-d", enable ? "0x2600" : "0x7d0");
		break;
	}
#endif
	return;
#endif
#ifndef HND_ROUTER
	switch (get_switch()) {
	case SWITCH_BCM53115:
	case SWITCH_BCM53125:
		eval("et", "robowr", "0x40", "0x01", enable ? "0x1f" : "0x00");
		break;
	case SWITCH_BCM5301x:
#ifdef RTCONFIG_BCMARM
		eval("et", "-i", "eth0", "robowr", "0x40", "0x01", enable ? "0x010001ff" : "0x00", "4");
#else
		eval("et", "robowr", "0x40", "0x01", enable ? "0x010001ff" : "0x00");
#endif
		break;
	}
#else
#ifdef RTCONFIG_EXT_BCM53134
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x4001", "-l", "4", "-d", enable ? "0x010001ff" : "0x00000000");
#endif
#ifdef RTCONFIG_EXT_RTL8367S
	// TBD
#endif
	eval("ethswctl", "-c", "regaccess", "-v", "0x4001", "-l", "4", "-d", enable ? "0x010001ff" : "0x00000000");
#endif
}

#if defined(BCM6855) || defined(GTAXE16000) || defined(GTBE98) || defined(GTBE98_PRO) || defined(RTBE96U) || defined(GTBE96) || defined(BQ16) || defined(BQ16_PRO) || defined(GTBE19000) || defined(BT10) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(RTBE88U)
void config_jumbo_frame()
{
	char value[sizeof("10240")];
	int mtusize = 0;
#ifdef BCM6855
	int rdp1 = 0;

	if (f_read_string("/proc/environment/mtusize", value, sizeof(value)) > 0)
		mtusize = atoi(value);

	if (f_read_string("/proc/environment/rdp1", value, sizeof(value)) > 0)
		rdp1 = atoi(value);

	if (nvram_get_int("jumbo_frame_enable")) {
		if (mtusize != 10240) {
			dbg("set mtusize as 10240 for BCM6753\n");
			f_write_string("/proc/nvram/set", "mtusize=10240", 0, 0);
		}

		if (rdp1 != 64) {
			dbg("set rdp1 as 64 for BCM6753\n");
			f_write_string("/proc/nvram/set", "rdp1=64", 0, 0);
		}
	} else {
		if (mtusize != 1500) {
			dbg("set mtusize as 1500 for BCM6753\n");
			f_write_string("/proc/nvram/set", "mtusize=1500", 0, 0);
		}

		if (rdp1 != 32) {
			dbg("set rdp1 as 32 for BCM6753\n");
			f_write_string("/proc/nvram/set", "rdp1=32", 0, 0);
		}
	}
#endif

#ifdef GTAXE16000
	int bufmem = 0;
	if (f_read_string("/proc/environment/mtusize", value, sizeof(value)) > 0)
		mtusize = atoi(value);

	if (f_read_string("/proc/environment/bufmem", value, sizeof(value)) > 0)
		bufmem = atoi(value);

	if (nvram_get_int("jumbo_frame_enable")) {
		if (mtusize != 10240) {
			dbg("set mtusize as 10240 for GT-AXE16000\n");
			f_write_string("/proc/nvram/set", "mtusize=10240", 0, 0);
		}

		if (bufmem != 512) {
			dbg("set bufmem as 512 for GT-AXE16000\n");
			f_write_string("/proc/nvram/set", "bufmem=512", 0, 0);
		}
	}
	else 
	{
		if (mtusize != 1500) {
			dbg("set mtusize as 1500 for GT-AXE16000\n");
			f_write_string("/proc/nvram/set", "mtusize=1500", 0, 0);
		}

		if (bufmem != 128) {
			dbg("set bufmem as 128 for GT-AXE16000\n");
			f_write_string("/proc/nvram/set", "bufmem=128", 0, 0);
		}

	}
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(RTBE96U) || defined(GTBE96) || defined(BQ16) || defined(BQ16_PRO) || defined(GTBE19000) || defined(BT10) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(RTBE88U)
	int bufmem = 0;
	if (f_read_string("/proc/environment/bufmem", value, sizeof(value)) > 0)
		bufmem = atoi(value);

	if (bufmem == 128) {
		/* BRCM suggest to allocate at least 256M once BCM_JUMBO_FRAME kernel configure is enabled */
		dbg("enlarge default bufmem from 128 to 256\n");
		f_write_string("/proc/nvram/set", "bufmem=256", 0, 0);
	}


	if (nvram_get_int("jumbo_frame_enable")) {
		if (bufmem != 512) {
			dbg("jumbo frame is enabled, enlarge bufmem as 512\n");
			f_write_string("/proc/nvram/set", "bufmem=512", 0, 0);
		}
	}
	else
	{
		if (bufmem != 256) {
			dbg("jumbo frame is disabled, reduce bufmem as 256\n");
			f_write_string("/proc/nvram/set", "bufmem=256", 0, 0);
		}
	}

#endif
}
#endif

#if defined(RTCONFIG_EXT_BCM53134) && !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
/*
The default value is to dropping such packets with the DA is 01:80:c2:00:00:02~0F multicast packet.
It will cause multicast protocol can't work, ex: LLDPD packet.
So we set 0x002f bit1 to 0.
*/
void accept_multicast_forward(void)
{
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x002f", "-l", "1", "-d", "0x00");
}
#endif

void ether_led()
{
	int model;

	model = get_model();
	switch(model) {
//	case MODEL_RTAC68U:
	/* refer to 5301x datasheet page 2770 */
	case MODEL_RTAC56S:
	case MODEL_RTAC56U:
		eval("et", "robowr", "0x00", "0x10", "0x3000");
		break;
	case MODEL_RTN16:
		eval("et", "robowr", "0", "0x18", "0x01ff");
		eval("et", "robowr", "0", "0x1a", "0x01ff");
		break;
	case MODEL_RTAC1200G:
	case MODEL_RTAC1200GP:
		eval("et", "robowr", "0", "0x12", "0x24");
		break;
	}
}

#ifdef HND_ROUTER
#ifdef RTCONFIG_HND_ROUTER_AX
#define ETHCTL_EEE_ON		0x0001
#define ETHCTL_APD_OFF		0x0002
#endif
#define ETHCTL_WIRESPEED_OFF	0x0004
#define ETHCTL_PHYRESET		0x0008
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
#define ETHCTL_CABLE_DIAG	0x0010
#endif
#define ETHCTL_PWRDOWNUP	0x0020
#define PHYMODE_AUTO		0
#define PHYMODE_10HD		1
#define PHYMODE_10FD		2
#define PHYMODE_100HD		3
#define PHYMODE_100FD		4
#define PHYMODE_1000FD		5
#define PHYMODE_2500FD		6
#define PHYMODE_5000FD		7
#define PHYMODE_10000FD		8
#define IPTV_BR_INDEX		101

void GPY211_INIT_SPEED()
{
#if defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XT8_V2) || defined(ET8PRO) || defined(ET8_V2)
	_dprintf("[%s][%d] skip\n", __FUNCTION__, __LINE__);
	return;
#endif
	if (nvram_get_int("ext_phy_model") != EXT_PHY_GPY211) {
		_dprintf("[%s(%d)] not support EXT_PHY_GPY211\n", __FUNCTION__, __LINE__);
		//logmessage("GPY211", "not support EXT_PHY_GPY211\n");
		return;
	}

	// sleep 2
	sleep(2);

	// switch to speed 1G
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x4", "0xd81");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x9", "0x200");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x70020", "0x4002");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x10000", "0x0058");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x0", "0x1340");

	// sleep 8
	sleep(8);

	// switch to speed auto
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x4", "0xd81");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x9", "0x200");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x70020", "0x40a2");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x10000", "0x2058");
	eval("ethctl", "phy", "ext", EXTPHY_GPY_ADDR_STR, "0x0", "0x3240");

	_dprintf("[%s(%d)] GPY211 speed war!\n", __FUNCTION__, __LINE__);
	logmessage("GPY211", "GPY211 speed war!\n");
}

#if defined(CONFIG_BCMWL5) && defined(HND_ROUTER)
void reset_ext_phy()
{
#if defined(GTAXE16000)
	if (!nvram_contains_word("wan_ifnames", "eth5")) {
		eval("ethctl", "eth5", "phy-power", "down");
		eval("ethctl", "eth5", "phy-power", "up");
	}
	if (!nvram_contains_word("wan_ifnames", "eth6")) {
		eval("ethctl", "eth6", "phy-power", "down");
		eval("ethctl", "eth6", "phy-power", "up");
	}
#elif defined(XT12) || defined(ET12) 
		eval("ethctl", "eth3", "phy-power", "down");
		eval("ethctl", "eth3", "phy-power", "up");
#elif defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (!is_rtl8372_boardid()) {
		eval("ethctl", "eth6", "phy-power", "down");
		eval("ethctl", "eth6", "phy-power", "up");
	} else {
		eval("ethctl", "eth3", "phy-power", "down");
		eval("ethctl", "eth3", "phy-power", "up");
	}
#elif defined(RTAX86U_PRO) || defined(RTAX88U_PRO) || defined(GTAX11000_PRO) || defined(GTAX6000)
	_dprintf("%s: power recycle eth5...\n", __func__);
	eval("ethctl", "eth5", "phy-power", "down");
	eval("ethctl", "eth5", "phy-power", "up");
#endif
}
#endif

void init_switch_pre()
{
	char word[64], ifnames[64], *next;
	int ethctl;
	int ethctl_wan = (int)strtoul(nvram_safe_get("ethctl_wan"), NULL, 0);
	int ethctl_lan = (int)strtoul(nvram_safe_get("ethctl_lan"), NULL, 0);
	int phymode;
	int phymode_wan = (int)strtoul(nvram_safe_get("phymode_wan"), NULL, 0);
	int phymode_lan = (int)strtoul(nvram_safe_get("phymode_lan"), NULL, 0);
#if defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(ET8PRO) || defined(ET8_V2)
	char *lan_if_array[] = { "eth1", "eth2", "eth3" };
	int lan_if_idx = 0;
#elif defined(BT12BAK)
	char *lan_if_array[] = { "eth1", "eth2", "eth3", "eth4" };
	int lan_if_idx = 0;
#elif defined(BT10BAK)
	char *lan_if_array[] = { "eth1", "eth2" };
	int lan_if_idx = 0;
#elif defined(BQ16BAK)
	char *lan_if_array[] = { "eth1", "eth2", "eth3", "eth4" };
	int lan_if_idx = 0;
	
#elif defined(BQ16_PROBAK)
	char *lan_if_array[] = { "eth1", "eth2", "eth3", "eth4" };
	int lan_if_idx = 0;
	
#endif
	char *wan_ifeth = NULL;
#if defined(BT10)
	int is_re_mode = nvram_get_int("re_mode");
#endif

#ifdef GT10
	system("ethswctl -c deleteport -p 5");
	doSystem("ethswctl -c createport -p 5 -w %d -i eth0", is_router_mode());
#endif

#if defined(GTAC5300)
	// clean the egress port first to avoid the 2nd WAN's DHCP.
	system("ethswctl -c regaccess -l 4 -v 0x3102 -d 0x60");
	system("ethswctl -c regaccess -l 4 -v 0x3100 -d 0x50");
	system("ethswctl -c regaccess -l 4 -v 0x3106 -d 0x140");
	system("ethswctl -c regaccess -l 4 -v 0x3104 -d 0x60");
	system("ethswctl -c regaccess -l 4 -v 0x310e -d 0x140");
#elif defined(RTAC86U) || defined(GTAC2900)
	// clean the egress port first to avoid the 2nd WAN's DHCP.
	system("ethswctl -c regaccess -l 4 -v 0x3106 -d 0x1c0");
	system("ethswctl -c regaccess -l 4 -v 0x3104 -d 0xe0");
	system("ethswctl -c regaccess -l 4 -v 0x3102 -d 0xe0");
	system("ethswctl -c regaccess -l 4 -v 0x3100 -d 0xd0");
#elif defined(RTAX86U_PRO)
	dbg("%s: set 'pause' to all ethernet ports...\n", __func__);
	system("ethswctl -cpause -n0 -p0 -v2");
	system("ethswctl -cpause -n0 -p1 -v2");
	system("ethswctl -cpause -n0 -p2 -v2");
	system("ethswctl -cpause -n0 -p3 -v2");
	system("ethswctl -cpause -n0 -p4 -v2");
	system("ethswctl -cpause -n0 -p5 -v2");
#elif defined(RTAX86U) || defined(RTAX68U)
	dbg("%s: set 'pause' to all ethernet ports...\n", __func__);
	system("ethswctl -cpause -n0 -p3 -v2");
	system("ethswctl -cpause -n1 -p0 -v2");
	system("ethswctl -cpause -n1 -p1 -v2");
	system("ethswctl -cpause -n1 -p2 -v2");
	system("ethswctl -cpause -n1 -p3 -v2");
#if defined(RTAX86U)
	if(strcmp(get_productid(), "RT-AX86S"))
		system("ethswctl -cpause -n1 -p7 -v2");
#endif
#elif defined(EBA63)
	system("ethswctl -cpause -n0 -p0 -v2");
	system("ethswctl -cpause -n0 -p8 -v2");
#elif defined(RTAX55) || defined(RTAX1800)
	system("ethswctl -c pause -p 0 -v 2");
#elif defined(GTAX6000)
	system("ethswctl -c pause -p 0 -v 2");
	system("ethswctl -c pause -p 1 -v 2");
	system("ethswctl -c pause -p 2 -v 2");
	system("ethswctl -c pause -p 3 -v 2");
	system("ethswctl -c pause -p 5 -v 2");
	system("ethswctl -c pause -p 6 -v 2");
#elif defined(RTBE58_GO)
	system("ethswctl -c pause -p 0 -v 2");
	system("ethswctl -c pause -p 1 -v 2");
#elif defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(RPBE58)
	system("ethswctl -c pause -p 0 -v 2");
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
	system("ethswctl -c pause -p 6 -v 2");
#else
	system("ethswctl -c pause -p 0 -v 2");
#endif
	system("ethswctl -c pause -p 5 -v 2");
	system("ethswctl -c pause -p 8 -v 2");
#elif defined(TUFAX3000_V2) || defined(RTAXE7800)
	system("ethswctl -c pause -p 0 -v 2");
	system("ethswctl -c pause -p 8 -v 2");
#elif defined(BCM6750) || defined(BCM63178)
	system("ethswctl -c pause -p 0 -v 2");
	system("ethswctl -c pause -p 1 -v 2");
#ifndef RTAX82_XD6S
	system("ethswctl -c pause -p 2 -v 2");
	system("ethswctl -c pause -p 3 -v 2");
	system("ethswctl -c pause -p 4 -v 2");
	system("ethswctl -c pause -p 8 -v 2");
#endif
#elif defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	system("ethswctl -c pause -p 1 -v 2");
	system("ethswctl -c pause -p 5 -v 2");
	system("ethswctl -c pause -p 6 -v 2");
	system("ethswctl -c pause -p 7 -v 2");
	if(!is_rtl8372_boardid()) {
		system("ethswctl -c pause -p 8 -v 2");
		system("ethswctl -c pause -p 9 -v 2");
		system("ethswctl -c pause -p 10 -v 2");
	}
#endif

#if defined(XT12) || defined(ET12)
	system("ethswctl -c pause -p 5 -v 2");
	system("ethswctl -c pause -p 6 -v 2");
#endif
#if defined(RTBE88U)
	system("ethswctl -c pause -p 5 -v 2");
	system("ethswctl -c pause -p 6 -v 2");
	system("ethswctl -c pause -p 7 -v 2");
	system("ethswctl -c pause -p 8 -v 2");
	system("ethswctl -c pause -p 9 -v 2");
	system("ethswctl -c pause -p 10 -v 2");
#endif

	memset(ifnames, 0, sizeof(ifnames));
	add_to_list("eth0", ifnames, sizeof(ifnames));
#if !defined(RTAX55) && !defined(RTAX1800) && !defined(RPAX56) && !defined(RPAX58) && !defined(RPBE58) && !defined(RTAX58U_V2) && !defined(RTAX3000N) && !defined(BR63) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7)
	add_to_list("eth1", ifnames, sizeof(ifnames));
#endif
#if !defined(RTAX56_XD4) && !defined(XD4PRO) && !defined(CTAX56_XD4) && !defined(RTAX55) && !defined(RTAX1800) && !defined(RTAX82_XD6S) && !defined(RPAX56) && !defined(RPAX58) && !defined(RPBE58) && !defined(RTBE58_GO) && !defined(RTAX58U_V2) && !defined(RTAX3000N) && !defined(BR63) && !defined(EBA63) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7)
	add_to_list("eth2", ifnames, sizeof(ifnames));
#endif
#if !defined(RTAX56_XD4) && !defined(XD4PRO) && !defined(CTAX56_XD4) && !defined(RTAX55) && !defined(RTAX1800) && !defined(RTAX82_XD6) && !defined(RTAX82_XD6S) && !defined(RPAX56) && !defined(RPAX58) && !defined(RPBE58) && !defined(RTBE58_GO) && !defined(RTAX58U_V2) && !defined(RTAX3000N) && !defined(BR63) && !defined(XD6_V2) && !defined(XC5) && !defined(EBA63) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7)
	add_to_list("eth3", ifnames, sizeof(ifnames));
#endif
#if !defined(RTAX95Q) && !defined(XT8PRO) && !defined(BT12) && !defined(BT10) && !defined(BQ16) && !defined(BQ16_PRO) && !defined(BM68) && !defined(XT8_V2) && !defined(RTAXE95Q) && !defined(ET8PRO) && !defined(ET8_V2) && !defined(RTAX56_XD4) && !defined(XD4PRO) && !defined(CTAX56_XD4) && !defined(RTAX55) && !defined(RTAX1800) && !defined(RTAX82_XD6S) && !defined(RPAX56) && !defined(RPAX58) && !defined(RPBE58) && !defined(RTBE58_GO) && !defined(RTAX58U_V2) && !defined(ET12) && !defined(XT12) && !defined(GT10) && !defined(RTAX3000N) && !defined(BR63) && !defined(GTBE98) && !defined(GTBE98_PRO) && !defined(XC5) && !defined(EBA63) && !defined(GTBE96) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(GTBE19000) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(GTBE19000AI) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7) && !defined(GTBE96_AI)
	add_to_list("eth4", ifnames, sizeof(ifnames));
#endif
#if defined(RTCONFIG_EXT_BCM53134) || defined(RTCONFIG_EXTPHY_BCM84880)
#if !defined(RTAX95Q) && !defined(XT8PRO) && !defined(BT12) && !defined(BT10) && !defined(BQ16) && !defined(BQ16_PRO) && !defined(BM68) && !defined(XT8_V2) && !defined(RTAXE95Q) && !defined(ET8PRO) && !defined(ET8_V2) && !defined(ET12) && !defined(XT12) && !defined(TUFAX3000_V2) && !defined(RTAXE7800) && !defined(GT10) && !defined(GTBE98) && !defined(GTBE98_PRO) && !defined(GTBE96) && !defined(RTBE86U) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(GTBE19000) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(RTBE58_GO) && !defined(GTBE19000AI) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7) && !defined(GTBE96_AI)
#if defined(RTAX86U)
	if(strcmp(get_productid(), "RT-AX86S"))
#endif
	add_to_list("eth5", ifnames, sizeof(ifnames));
#endif
#endif

#if defined(GTAXE16000)
	add_to_list("eth6", ifnames, sizeof(ifnames));
#endif

#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (!is_rtl8372_boardid()) {
		add_to_list("eth4", ifnames, sizeof(ifnames));
		add_to_list("eth5", ifnames, sizeof(ifnames));
		add_to_list("eth6", ifnames, sizeof(ifnames));
	}
#endif

//#if defined(RTCONFIG_HND_ROUTER_BE_4916)
#if defined(RTBE88U) || defined(BQ16_PRO) || defined(BQ16)
	dbg("%s(%d): ifnames=%s.\n", __func__, getpid(), ifnames);

#if defined(RTBE88U)
	add_to_list("eth6", ifnames, sizeof(ifnames));
	add_to_list("eth7", ifnames, sizeof(ifnames));
	add_to_list("eth8", ifnames, sizeof(ifnames));
#elif defined(BQ16_PRO) || defined(BQ16)
	add_to_list("eth4", ifnames, sizeof(ifnames));
#endif

	ethctl = ethctl_lan;
	dbg("pwr: EEE %s\n", (ethctl & ETHCTL_EEE_ON) ? "on" : "off");
	if(!(ethctl & ETHCTL_EEE_ON)){
		eval("pwr", "config", "--eee", "off");
	}
	else{
		eval("pwr", "config", "--eee", "on");
	}

	dbg("pwr: APD %s\n", (ethctl & ETHCTL_APD_OFF) ? "off" : "on");
	if(ethctl & ETHCTL_APD_OFF){
		eval("pwr", "config", "--apd", "off");
		eval("pwr", "config", "--dgm", "off");
	}
	else{
		eval("pwr", "config", "--apd", "on");
		eval("pwr", "config", "--dgm", "on");
	}
#endif

#if defined(RTBE88U)
	if (!nvram_match("sfp_an", "on")) {
		dbg("eth9: SFP port An OFF\n");
		doSystem("ethctl eth9 media-type auto An off");
	}
#endif

	foreach(word, ifnames, next) {
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE86U) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
		if (!strncmp(word, wan_if_eth(), strlen(wan_if_eth()))) {
			dbg("%s: phy-power ctrl\n", word);
			doSystem("ethctl %s phy-power down", word);
			doSystem("ethctl %s phy-power up", word);
		}
#endif
		ethctl = !strncmp(word, wan_if_eth(), strlen(wan_if_eth())) ? ethctl_wan : ethctl_lan;
		{
			if (ethctl & ETHCTL_PHYRESET) {
				dbg("%s: phy-reset\n", word);
				doSystem("ethctl %s phy-reset", word);
			}

//#if defined(RTCONFIG_HND_ROUTER_AX) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
#if defined(RTCONFIG_HND_ROUTER_AX) && !defined(RTBE88U) && !defined(BQ16_PRO) && !defined(BQ16)
			dbg("%s: EEE %s\n", word, (ethctl & ETHCTL_EEE_ON) ? "on" : "off");
			if (!(ethctl & ETHCTL_EEE_ON)) {
#if defined(RTAX86U) || defined(RTAX68U) || defined(RTAX86U_PRO)
				eval("pwr", "config", "--eee", "off");

#ifdef RTCONFIG_EXTPHY_BCM84880
				if(!strcmp(word, "eth5") && nvram_get_int("ext_phy_model") == EXT_PHY_RTL8226) { // 0: BCM54991, 1: RTL8226
					eval("ethctl", "phy", "ext", EXTPHY_RTL_ADDR_STR, "0x07003c", "0x0000"); // bit 2:3 1:on 0:off eee (1000M/100M)
					eval("ethctl", "phy", "ext", EXTPHY_RTL_ADDR_STR, "0x07003e", "0x0000"); // bit 0 1:on 0:off eee (2500M)
				} else
#endif
#endif
				doSystem("ethctl %s eee off", word);
			}

			dbg("%s: APD %s\n", word, (ethctl & ETHCTL_APD_OFF) ? "off" : "on");
			if (ethctl & ETHCTL_APD_OFF){
#if defined(RTAX86U) || defined(RTAX68U) || defined(RTAX86U_PRO)
				eval("pwr", "config", "--apd", "off");
				eval("pwr", "config", "--dgm", "off");
#endif
				doSystem("ethctl %s apd off", word);
			}
#endif
#if defined(GTAX6000) || defined(RTAX88U_PRO) || defined(RTAX9000)
			if (strcmp(word, "eth0") && strcmp(word, "eth5"))
#elif defined(ET12) || defined(XT12)
			if (strcmp(word, "eth0") && strcmp(word, "eth3"))
#elif defined(TUFAX3000_V2) || defined(RTAXE7800) || defined(GT10)
			if (strcmp(word, "eth0"))
#elif defined(RTAX86U_PRO)
			if (strcmp(word, "eth5"))
#endif
			{
				dbg("%s: ethernet@wirespeed %s\n", word, (ethctl & ETHCTL_WIRESPEED_OFF) ? "disabled" : "enabled");
				if (!(ethctl & ETHCTL_WIRESPEED_OFF))
					doSystem("ethctl %s ethernet@wirespeed enable", word);
			}
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
			if (ethctl & ETHCTL_CABLE_DIAG)
				doSystem("ethctl %s cable-diag enable", word);
#endif
			if (ethctl & ETHCTL_PWRDOWNUP) {
				dbg("%s: power down & up\n", word);
				doSystem("ethctl %s phy-power down", word);
				doSystem("ethctl %s phy-power up", word);
			}
		}

		phymode = !strncmp(word, wan_if_eth(), strlen(wan_if_eth())) ? phymode_wan : phymode_lan;
		switch(phymode) {
			case PHYMODE_10HD:
				doSystem("ethctl %s media-type 10HD", word);
				break;
			case PHYMODE_10FD:
				doSystem("ethctl %s media-type 10FD", word);
				break;
			case PHYMODE_100HD:
				doSystem("ethctl %s media-type 100HD", word);
				break;
			case PHYMODE_100FD:
				doSystem("ethctl %s media-type 100FD", word);
				break;
			case PHYMODE_1000FD:
				doSystem("ethctl %s media-type 1000FD", word);
				break;
			case PHYMODE_2500FD:
				doSystem("ethctl %s media-type 2500FD", word);
				break;
			case PHYMODE_5000FD:
				doSystem("ethctl %s media-type 5000FD", word);
				break;
			case PHYMODE_10000FD:
				doSystem("ethctl %s media-type 10000FD", word);
				break;
			case PHYMODE_AUTO:
			default:
#if 0
				doSystem("ethctl %s media-type auto", word);
#endif
				break;
		}
	}

#ifdef BCM6813
	reset_ext_phy();
#endif

#if defined(RTCONFIG_HND_ROUTER) && !defined(RTCONFIG_HND_ROUTER_AX)
	doSystem("pwr config --ethapd %s", nvram_get_int("pwr_ethapd_off") ? "off" : "on");
	doSystem("pwr config --eee %s", nvram_get_int("pwr_eee_on") ? "on" : "off");
#endif

#if defined(RTCONFIG_EXT_BCM53134) && !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	system("ethswctl -c pmdioaccess -x 0x1030 -l 2 -d 0xf017");
	system("ethswctl -c pmdioaccess -x 0x1130 -l 2 -d 0xf017");
	system("ethswctl -c pmdioaccess -x 0x1230 -l 2 -d 0xf017");
	system("ethswctl -c pmdioaccess -x 0x1330 -l 2 -d 0xf017");
#endif

	// clear wan flag in bridge mode, or pktfwd cannot learn the address from default wan interface
#ifdef BCM4908
	wan_ifeth = wan_if_eth();

	doSystem("ethswctl -c wan -i %s -o %s", wan_ifeth, "enable");
  #if defined(BCM4906_504)
	foreach(word, nvram_safe_get("wan_ifnames"), next){
		if(strcmp(word, wan_ifeth)!=0 && strncmp(word, "eth", 3)==0){
			doSystem("ethswctl -c softswitch -i %s -o %s", word, "enable");
		}
	}
  #endif
#else
  #if defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(ET8PRO) || defined(ET8_V2)
	if(is_router_mode()){
		doSystem("ethswctl -c wan -i %s -o enable", wan_if_eth());
	}else{
		doSystem("ethswctl -c softswitch -i %s -o enable", wan_if_eth());
	}
	/* skip 6756 internal roboswitch to avoid unstable throughput test */
	for(lan_if_idx  = 0; lan_if_idx < ARRAY_SIZE(lan_if_array); lan_if_idx++) {
#if defined(RTCONFIG_MULTILAN_CFG)
		doSystem("ethswctl -c softswitch -i %s -o enable", lan_if_array[lan_if_idx]);
#else
		doSystem("ethswctl -c softswitch -i %s -o disable", lan_if_array[lan_if_idx]);
#endif
	}
  #else
#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
	doSystem("ethswctl -c wan -i %s -o %s", wan_if_eth(), "enable");
#else
	doSystem("ethswctl -c wan -i %s -o %s", wan_if_eth(), is_router_mode() ? "enable" : "disable");
#endif
  #endif
#if defined(TUFAX3000_V2) || defined(RTAXE7800)
	if (is_router_mode())
		doSystem("ethswctl -c wan -i %s -o enable", WAN_IF_ETH);
#else
	if (is_router_mode() && strcmp(WAN_IF_ETH, wan_if_eth()))
		doSystem("ethswctl -c wan -i %s -o disable", WAN_IF_ETH);
#endif
#endif

#if defined(XC5)
	doSystem("ethswctl -c softswitch -i eth2 -o enable");
#endif

#if defined(TUFAX3000_V2) || defined(RTAXE7800)
	doSystem("ethswctl -c softswitch -i %s -o %s", wan_if_eth(), is_router_mode() ? "disable" : "enable");

	char *wired_ifnames[] = { "eth0", "eth1", "eth2", "eth3", "eth4" };
	int i, lacp_enabled;
	for (i = 0; i < ARRAY_SIZE(wired_ifnames); i++) {
		if (!strcmp(wired_ifnames[i], wan_if_eth()))
			continue;
		lacp_enabled = nvram_get_int("lacp_enabled") &&
			(!strcmp(wired_ifnames[i], "eth1") || !strcmp(wired_ifnames[i], "eth2"));
		doSystem("ethswctl -c softswitch -i %s -o %s", wired_ifnames[i], lacp_enabled ? "enable" : "disable");
	}
#endif

#if (defined(BCM6750) || defined(BCM63178)) && defined(RTCONFIG_HND_ROUTER_AX_6756)
	char *wired_ifnames[] = { "eth0", "eth1", "eth2", "eth3", "eth4" };
	int i, dualwan_lanif = 0;

	if (is_router_mode())
	for (i = 0; i < ARRAY_SIZE(wired_ifnames); i++) {
		if (!strcmp(wired_ifnames[i], wan_if_eth()))
			continue;

		if (nvram_match("wans_dualwan", "wan lan"))
		foreach(word, nvram_safe_get("wan_ifnames"), next) {
			if (!strcmp(wired_ifnames[i], word)) {
				dualwan_lanif = 1;
				break;
			}
		}

		doSystem("ethswctl -c softswitch -i %s -o %s", wired_ifnames[i], dualwan_lanif ? "enable" : "disable");
        }
#endif

#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	doSystem("ethswctl -c softswitch -i eth1 -o enable");
#endif

#if (defined(BCM6750) || defined(BCM63178)) && !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	system("swmdk");
#endif

#if !defined(RTAX55) && !defined(RTAX1800) && !defined(RPAX56) && !defined(RPAX58) && !defined(RPBE58) && !defined(RTBE58_GO) && !defined(RTAX58U_V2) && !defined(RTAX3000N) && !defined(BR63) && !defined(RTBE58U) && !defined(TUFBE3600) && !defined(RTBE58U_V2) && !defined(TUFBE3600_V2) && !defined(RTBE55) && !defined(RTBE92U) && !defined(RTBE95U) && !defined(RTBE82U) && !defined(TUFBE82) && !defined(RTBE82M) && !defined(RTBE58U_PRO) && !defined(RTBE58_GO) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7)
	foreach(word, ifnames, next){
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
		if (is_rtl8372_boardid() && !strcmp(word, "eth1")) continue;
#endif
#if defined(RTAX86U)
		if(!strcmp(word, "eth5")){
			int tmctl_control = nvram_get_int("tmctl_control");
			if(tmctl_control == 2){
				dbg("%s: don't execute tmctl\n", word);
			}
			else if(tmctl_control == 1){
				dbg("%s: tmctl porttminit --devtype 0 --if %s --flag 0\n", word, word);
				doSystem("tmctl porttminit --devtype 0 --if %s --flag 0", word);
			}
			else{
				dbg("%s: tmctl porttminit --devtype 0 --if %s --flag 1\n", word, word);
				doSystem("tmctl porttminit --devtype 0 --if %s --flag 1", word);
			}
		}
		else
#endif
		{
#if defined(BCM4912) || defined(BCM6813)
			dbg("%s: tmctl porttminit --devtype 0 --if %s --flag 0\n", word, word);
			doSystem("tmctl porttminit --devtype 0 --if %s --flag 0", word);
			dbg("%s: tmctl porttminit --devtype 0 --if %s --flag 1 --numqueues 8\n", word, word);
			doSystem("tmctl porttminit --devtype 0 --if %s --flag 1 --numqueues 8", word);
#else // BCM4906_504 ?
			dbg("%s: tmctl porttminit --devtype 0 --if %s --flag 1\n", word, word);
			doSystem("tmctl porttminit --devtype 0 --if %s --flag 1", word);
#endif
		}
	}
#endif
#if defined(BCM4912) && !defined(GTAXE16000)
	char value[sizeof("255")];
	int bufmem = 0;

	if (f_read_string("/proc/environment/bufmem", value, sizeof(value)) > 0)
		bufmem = atoi(value);
	if(bufmem != 128)
	{
		dbg("unset env bufmem for BCM4912 B0\n");
		f_write_string("/proc/nvram/set", "bufmem=", 0, 0);
	}
#endif

#if defined(GT10) || defined(RTAX9000)
	if (is_router_mode())
		eval("/usr/sbin/ingress_filter_config.sh");
	else
		eval("/usr/sbin/ingress_filter_config_bridging.sh");
#endif

#if defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XT8_V2) || defined(ET8PRO) || defined(ET8_V2)
	if(hnd_boardid_cmp("XT8PRO_GPY211") == 0 ||
		hnd_boardid_cmp("ET8PRO_GPY211") == 0 ||
		hnd_boardid_cmp("ET8_V2_GPY211") == 0 ||
		hnd_boardid_cmp("BM68_GPY211") == 0 ||
		hnd_boardid_cmp("XT8_V2") == 0){
		doSystem("ethctl %s media-type 1000FD", GPY211_IF_NAME);
		sleep(10);
		doSystem("ethctl %s media-type auto", GPY211_IF_NAME);
		sleep(10);
	}
#endif

#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if(is_rtl8372_boardid()) {
		system("tmctl porttminit --devtype 0 --if eth1");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 7 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 6 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 5 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 4 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 3 --priority 0 --weight 1 --schedmode 2");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 2 --priority 0 --weight 1 --schedmode 2");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 1 --priority 0 --weight 1 --schedmode 2");
		system("tmctl setqcfg --devtype 0 --if eth1 --qid 0 --priority 0 --weight 1 --schedmode 2");
	}
#endif

#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	if (!nvram_get_int("stop_tmctl_qos"))  {
	system("tmctl porttminit --devtype 0 --if eth1");
#if defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || (GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 7 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 5 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 3 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 1 --priority 0 --weight 1 --schedmode 2 --shapingrate 10000000");
#elif defined(RTBE58U_PRO)
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 7 --priority 0 --weight 1 --schedmode 2 --shapingrate 1000000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 5 --priority 0 --weight 1 --schedmode 2 --shapingrate 1000000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 3 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 1 --priority 0 --weight 1 --schedmode 2 --shapingrate 7000000");
#else
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 7 --priority 0 --weight 1 --schedmode 2 --shapingrate 1000000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 5 --priority 0 --weight 1 --schedmode 2 --shapingrate 1000000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 3 --priority 0 --weight 1 --schedmode 2 --shapingrate 1000000");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 1 --priority 0 --weight 1 --schedmode 2 --shapingrate 2500000");
#endif
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 6 --priority 0 --weight 1 --schedmode 2");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 4 --priority 0 --weight 1 --schedmode 2");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 2 --priority 0 --weight 1 --schedmode 2");
	system("tmctl setqcfg --devtype 0 --if eth1 --qid 0 --priority 0 --weight 1 --schedmode 2");
	}
#endif
#if defined(BT10)
	if (is_router_mode()){
		if (nvram_match("wans_dualwan", "wan lan") || nvram_match("wans_dualwan", "lan wan")){
			foreach(word, nvram_safe_get("wan_ifnames"), next) {
				doSystem("ethswctl -c wan -i %s -o %s", word, "enable");
			}
		}
	}
	if(is_re_mode == 1){
		/* skip 6766 internal switch to avoid lan client working abnormal if ETH_BH/WIFI_BH coexisted */
		foreach(word, nvram_safe_get("eth_ifnames"), next){
			doSystem("ethswctl -c softswitch -i %s -o enable", word);
		}

		/* skip 6766 internal switch to avoid lan client working abnormal if ETH_BH/WIFI_BH coexisted */
		foreach(word, nvram_safe_get("wired_ifnames"), next){
			doSystem("ethswctl -c softswitch -i %s -o enable", word);
		}
	}
#endif

}

void wl_dis_illegal_160m()
{
#if defined(XT8PRO)
	if (strcmp(nvram_safe_get("model"), "ZenWiFi_XT8P") != 0){
		_dprintf("it is not XT9\n");
		nvram_set_int("wl1_bw_160", 0);
		return;
	}
#elif defined(ET8PRO)
	if (strcmp(nvram_safe_get("model"), "ZenWiFi_ET8P") != 0){
		_dprintf("it is not ET9\n");
		nvram_set_int("wl1_bw_160", 0);
		return;
	}
#endif
	return;
}

void wl_update_eu_edthresh(void)
{
#if defined(BQ16)
	if(strncmp(nvram_safe_get("territory_code"), "EU", 2) == 0) {
		_dprintf("update eu_edthresh\n");
		nvram_set("0:eu_edthresh5g", "-76");
		nvram_set("1:eu_edthresh6g", "-76");
		nvram_set("2:eu_edthresh5g", "-76");
		nvram_set("3:eu_edthresh2g", "-76");
	}
#endif
#if defined(RTBE88U)
	if(strncmp(nvram_safe_get("location_code"), "XX", 2) == 0) {
		_dprintf("[XX] update ed_thresh & eu_edthresh\n");
		nvram_unset("0:ed_thresh2g");
		nvram_unset("1:ed_thresh5g");
		nvram_unset("0:eu_edthresh2g");
		nvram_unset("1:eu_edthresh5g");
		_dprintf("[XX] update wme setting\n");
		nvram_set("wl0_wme_ap_be", "3 15 3 0 0 off off");
		nvram_set("wl0_wme_ap_vi", "3 7 1 6016 3008 off off");
	}
	else {
		nvram_set("wl0_wme_ap_be", "15 63 3 0 0 off off");
		nvram_set("wl0_wme_ap_vi", "7 15 1 6016 3008 off off");
	}
#endif
	return;
}

#define ARRAYSIZE(a)	(sizeof(a)/sizeof(a[0]))

void init_switch()
{
	int model = get_model();

#if !defined(RTBE88U) && !defined(BQ16_PRO) && !defined(BQ16)
	init_switch_pre();
#else
	_dprintf("%s(%d): skip init_switch_pre() here.\n", __func__, getpid());
#endif

	switch(model) {
		case MODEL_GTAC5300:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L5 L6 */
			int ports[6] = { 7, 1, 0, 3, 2 };
			int eth_devs[6] = { 0, 2, 1, 4, 3 };
			char *regs[5] = { "0x3102", "0x3100", "0x3106", "0x3104", "0x310e" };
			char *wan2vals[5][5] = { { "0xef", "0xdf", "0x1cf", "0xef", "0x1cf" },
						 { "0xe2", "0xdd", "0x1cd", "0xed", "0x1cd" },
						 { "0xee", "0xd1", "0x1ce", "0xee", "0x1ce" },
						 { "0xe7", "0xd7", "0x1c8", "0xe7", "0x1c7" },
						 { "0xeb", "0xdb", "0x1cb", "0xe4", "0x1cb" } };
			char buf[64], *ptr;
			int i, len, eth_dev, wancfg = 0;
			int tmp_type;

			if(get_wans_dualwan() & WANSCAP_LAN){
				eth_dev = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for(i = 1; i <= 4; ++i){
					if(eth_devs[i] == eth_dev){
						wancfg = i;
						continue;
					}

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else{
				wancfg = 0;

				nvram_set("lanports", "1 0 3 2");
			}

			for(i = 0; i < 5; ++i){ // 4 LAN ports
				snprintf(buf, sizeof(buf), "/bin/ethswctl -c regaccess -v %s -l 4 -d %s", regs[i], wan2vals[wancfg][i]);
				system(buf);
			}

			memset(buf, 0, sizeof(buf));
			for(i = WAN_UNIT_FIRST, ptr = buf; (tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE; ++i){
				len = strlen(buf);
				if(tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if(tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}

			nvram_set("wanports", buf);

			break;
		}
		case MODEL_RTAC86U:
		case MODEL_RTAX88U:
		case MODEL_BC109:
		case MODEL_BC105:
		case MODEL_EBG19:
		case MODEL_EBG15:
		case MODEL_EBP15:
		case MODEL_GTAX11000:
		case MODEL_RTAX92U:
		case MODEL_RTAX86U:
		case MODEL_RTAX68U:
		case MODEL_RTAC68U_V4:
		case MODEL_GTAXE11000:
		{
			/* set wanports in init_nvram for dualwan */
#ifdef RTCONFIG_EXTPHY_BCM84880
			/* WAN L1 L2 L3 L4 L5 */
			int ports[7] = { 4, 3, 2, 1, 0, 7 };
#define LAN_PORTS 5
#else
			/* WAN L1 L2 L3 L4 */
			int ports[6] = { 7, 3, 2, 1, 0 };
#define LAN_PORTS 4
#endif
			int lan_ports = LAN_PORTS;
#ifdef RTCONFIG_HND_ROUTER_AX
#ifdef RTCONFIG_EXTPHY_BCM84880			/*  P3       P2      P1      P0      P7 */
#if defined(RTAX86U) || defined(GTAX11000)
			char *wan2vals1[6][5] = { { "0x18f", "0xaf", "0xaf", "0x9f", "0x18f" },	// Original
						 { "0x188", "0xa7", "0xa7", "0x97", "0x187" },	// P3
						 { "0x18b", "0xa4", "0xab", "0x9b", "0x18b" },	// P2
						 { "0x18d", "0xad", "0xa2", "0x9d", "0x18d" },	// P1
						 { "0x18e", "0xae", "0xae", "0x91", "0x18e" },	// P0
						 { "0x10f", "0x2f", "0x2f", "0x1f", "0x180" } };// P7
			char *wan2vals2[5][4] = { { "0x18f", "0xaf", "0xaf", "0x9f" },
						 { "0x188", "0xa7", "0xa7", "0x97" },
						 { "0x18b", "0xa4", "0xab", "0x9b" },
						 { "0x18d", "0xad", "0xa2", "0x9d" },
						 { "0x18e", "0xae", "0xae", "0x91" } };
#else
			char *wan2vals[6][LAN_PORTS] = { { "0x18f", "0xaf", "0xaf", "0x9f", "0x18f" },	// Original
						 { "0x188", "0xa7", "0xa7", "0x97", "0x187" },	// P3
						 { "0x18b", "0xa4", "0xab", "0x9b", "0x18b" },	// P2
						 { "0x18d", "0xad", "0xa2", "0x9d", "0x18d" },	// P1
						 { "0x18e", "0xae", "0xae", "0x91", "0x18e" },	// P0
						 { "0x10f", "0x2f", "0x2f", "0x1f", "0x180" } };// P7
#endif
#else
			char *wan2vals[5][LAN_PORTS] = { { "0x18f", "0xaf", "0xaf", "0x9f" },
						 { "0x188", "0xa7", "0xa7", "0x97" },
						 { "0x18b", "0xa4", "0xab", "0x9b" },
						 { "0x18d", "0xad", "0xa2", "0x9d" },
						 { "0x18e", "0xae", "0xae", "0x91" } };
#endif
#else
			char *wan2vals[5][LAN_PORTS] = { { "0x1cf", "0xef", "0xef", "0xdf" },
						 { "0x1c8", "0xe7", "0xe7", "0xd7" },
						 { "0x1cb", "0xe4", "0xeb", "0xdb" },
						 { "0x1cd", "0xed", "0xe2", "0xdd" },
						 { "0x1ce", "0xee", "0xee", "0xd1" } };
#endif
#if defined(RTCONFIG_EXTPHY_BCM84880)
#if defined(RTAX86U) || defined(GTAX11000)
			char *regs1[5] = { "0x3106", "0x3104", "0x3102", "0x3100", "0x310e" };
			char *regs2[4] = { "0x3106", "0x3104", "0x3102", "0x3100" };
#else
			char *regs[LAN_PORTS] = { "0x3106", "0x3104", "0x3102", "0x3100", "0x310e" };
#endif
#else
			char *regs[LAN_PORTS] = { "0x3106", "0x3104", "0x3102", "0x3100" };
#endif
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

#if defined(RTAX86U)
			if(!strcmp(get_productid(), "RT-AX86S"))
				lan_ports = 4;
#endif

			if(get_wans_dualwan() & WANSCAP_LAN){
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for(i = 1; i <= lan_ports; ++i)
				{
					if(i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}
			}
			else{
				wancfg = 0;

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for(i = 1; i <= lan_ports; ++i)
				{
					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}
			}

			nvram_set("lanports", buf);

#ifdef RTCONFIG_EXTPHY_BCM84880
#if defined(RTAX86U) || defined(GTAX11000)
			int ext_phy_model = nvram_get_int("ext_phy_model"); // 0: BCM54991, 1: RTL8226
#endif
#endif

			for(i = 0; i < lan_ports; ++i)
			{
#if defined(RTAX86U) || defined(GTAX11000)
#ifdef RTCONFIG_EXTPHY_BCM84880
				if(i == (lan_ports-1) && ext_phy_model != EXT_PHY_BCM54991 && 
					(strcmp(get_productid(), "RT-AX86S") && strcmp(get_productid(), "GT-AX11000")))
					break;
#endif

				if(!strcmp(get_productid(), "RT-AX86S") || !strcmp(get_productid(), "GT-AX11000"))
					snprintf(buf, sizeof(buf), "/bin/ethswctl -c regaccess -v %s -l 4 -d %s", regs2[i], wan2vals2[wancfg][i]);
				else
					snprintf(buf, sizeof(buf), "/bin/ethswctl -c regaccess -v %s -l 4 -d %s", regs1[i], wan2vals1[wancfg][i]);
#else
				snprintf(buf, sizeof(buf), "/bin/ethswctl -c regaccess -v %s -l 4 -d %s", regs[i], wan2vals[wancfg][i]);
#endif
				system(buf);
			}
			memset(buf, 0, sizeof(buf));
			for(i = WAN_UNIT_FIRST, ptr = buf; (tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE; ++i){
				len = strlen(buf);
				if(tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if(tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);
#if defined(RTAX86U)
			setLANLedOn();
#elif defined(EBG19) || defined(EBG15)
			led_control(LED_ETHALL, LED_ON);
#ifdef EBG19
			eval("ethswctl", "-c", "setlinkstatus", "-n", "0", "-p", "5", "-x", "1", "-y", "1000", "-z", "1");
			eval("sw", "0x800c05c4", "0x0311");	// mdio freq

			eval("mknod", "/dev/rtkswitch", "c", "206", "0");
			eval("insmod", "rtl8367s");
#endif
#endif
			break;
		}
		case MODEL_RTAX95Q:
		case MODEL_XT8PRO:
		case MODEL_BT12:
		case MODEL_BT10:
		case MODEL_BQ16:
		case MODEL_BQ16_PRO:
		case MODEL_BM68:
		case MODEL_XT8_V2:
		case MODEL_RTAXE95Q:
		case MODEL_ET8PRO:
		case MODEL_ET8_V2:
		case MODEL_RTAX56_XD4:
		case MODEL_XD4PRO:
		case MODEL_XC5:
		case MODEL_CTAX56_XD4:
		case MODEL_EBA63:
		case MODEL_RTAX58U:
		case MODEL_RTAX82U_V2:
		case MODEL_TUFAX5400_V2:
		case MODEL_RTAX5400:
		case MODEL_RTAX82_XD6S:
		case MODEL_XD6_V2:
		case MODEL_RTAX58U_V2:
		case MODEL_RTAXE7800:
		case MODEL_TUFAX3000_V2:
		case MODEL_RTAX3000N:
		case MODEL_BR63:
		case MODEL_GT10:
		case MODEL_RTAX9000:
		case MODEL_RTAX55:
		case MODEL_RTAX56U:
		case MODEL_RPAX56:
		case MODEL_RPAX58:
		case MODEL_RPBE58:
		case MODEL_DSLAX82U:
		case MODEL_RTBE58U:
		case MODEL_RTBE58U_V2:
		case MODEL_RTBE58_GO:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 */
#if defined(RTAX95Q) || defined(XT8PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2)
			int ports[4] = { 0, 1, 2, 3 };
#elif defined(BT12)
			int ports[5] = { 0, 1, 2, 3, 4 };
#elif defined(BT10)
			int ports[3] = { 0, 1, 2 };
#elif defined(BQ16)
			int ports[5] = { 0, 1, 2, 3, 4 };
#elif defined(BQ16_PRO)
			int ports[5] = { 0, 1, 2, 3, 4 };
#elif defined(RTAX56_XD4)
			int ports[1] = { 0 };
#elif defined(XD4PRO) || defined(EBA63)
			int ports[1] = { 0 };
#elif defined(XC5)
			int ports[2] = { 0, 1 };
#elif defined(CTAX56_XD4)
			int ports[1] = { 0 };
#elif defined(RTAX55)
			int ports[2] = { 0, 1 };
#elif defined(RTAX82_XD6) || defined(XD6_V2)
			int ports[4] = { 4, 2, 1, 0 };
#elif defined(RTAX82_XD6S)
			int ports[2] = { 1, 0 };
#elif defined(BCM6750) || defined(BCM63178)
			int ports[5] = { 4, 3, 2, 1, 0 };
#elif defined(GT10)
			int ports[4] = { 0, 1, 2, 3 };
			if (nvram_get_int("wans_extwan")) {
				ports[0] = 1;
				ports[1] = 0;
			}
#elif defined(RTAX9000)
			int ports[6] = { 0, 1, 2, 3, 4, 5 };
#elif defined(TUFAX3000_V2) || defined(RTAXE7800)
			int ports[5] = { 0, 1, 2, 3, 4 };
			if (nvram_get_int("wans_extwan")) {
				ports[0] = 1;
				ports[1] = 0;
			}
#elif defined(RTAX1800)
			int ports[5] = { 0, 1, 2, 3, 4 };
#elif defined(RPAX56) || defined(RPAX58) || defined(RPBE58)
			int ports[1] = { 0 };
#elif defined(RTBE58_GO)
			int ports[2] = { 0, 1 };
#elif defined(BR63) && defined(NEW_SWITCH_ORDER)
			int ports[5] = { 0, 1, 2, 3, 4 };
#else // RTAX56U, RTAX55, RT-AX58U_V2, RT-AX3000N, BR63, RT-BE58U, TUF-BE3600
			int ports[5] = { 0, 4, 3, 2, 1 };
#endif
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;
#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
			eval("mknod", "/dev/rtkswitch", "c", "206", "0");
#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
			f_write_string("/sys/class/leds/led_gpio_24/brightness", "255", 0, 0);
			usleep(40*1000);
			f_write_string("/sys/class/leds/led_gpio_24/brightness", "0", 0, 0);
#endif
			eval("insmod", "rtl8367s");
#if defined(RTAX55) || defined(RTAX1800)
			eval("ethswctl", "-c", "setlinkstatus", "-n", "0", "-p", "1", "-x", "1", "-y", "1000", "-z", "1");
#endif
			system("rtkswitch 6");
#endif
			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
#if defined(RTAX1800)
				nvram_set("lanports", "1 2 3 4");
#elif defined(RPAX56) || defined(RPAX58) || defined(RPBE58) || defined(RTBE58_GO)
				nvram_set("lanports", "0");
#elif defined(BR63) && defined(NEW_SWITCH_ORDER)
				nvram_set("lanports", "1 2 3 4");
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				nvram_set("lanports", "4 3 2 1");
#elif defined(RTAX82_XD6) || defined(XD6_V2)
				nvram_set("lanports", "2 1 0");
#elif defined(RTAX82_XD6S)
				nvram_set("lanports", "0");
#elif defined(GT10)
				nvram_set("lanports", !nvram_get_int("wans_extwan") ? "1 2 3" : "0 2 3");
#elif defined(RTAX9000)
				nvram_set("lanports", "1 2 3 4 5");
#elif defined(TUFAX3000_V2) || defined(RTAXE7800)
				nvram_set("lanports", !nvram_get_int("wans_extwan") ? "1 2 3 4" : "0 2 3 4");
#else
				nvram_set("lanports", "3 2 1 0");
#endif
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);
#if defined(TUFAX3000_V2) || defined(RTAXE7800)
			bcm53134_led_control(2);
#endif
#if defined(TUFAX3000_V2) || defined(RTAXE7800) || defined(TUFAX5400_V2) || defined(RTAX5400) || defined(RTAX88U_PRO) || defined(RTBE86U)
#if defined(TUFAX3000_V2) || defined(RTAXE7800)
			if (!nvram_get_int("wans_extwan"))
#endif
			lan_phy_led_pinmux(0);
#endif
#if defined(RTAX58U) || defined(TUFAX3000) || defined(RTAX56U) || defined(RPAX56) || defined(RPAX58) || defined(RPBE58) || defined(DSL_AX82U) || defined(TUFAX3000_V2) || defined(RTAXE7800) || defined(TUFAX5400_V2) || defined(RTAX5400)
			setLANLedOn();
#endif
			break;
		}

		case MODEL_GTAX6000:
		case MODEL_RTAX88U_PRO:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 */
			int ports[6] = { 0, 1, 2, 3, 4, 5 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4 5");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}

		case MODEL_GTAX11000_PRO:
		case MODEL_RTBE96U:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 */
			int ports[6] = { 0, 1, 2, 3, 4, 5 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4 5");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}
		case MODEL_RTBE88U:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 L6 L7 L8 SFP+*/
			int ports[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4 5 6 7 8 9");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}

		case MODEL_RTBE86U:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 */
			int ports[6] = { 0, 1, 2, 3, 4 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			set_hwaddr(wan_if_eth(), (const char *) get_lan_hwaddr());

			break;
		}

		case MODEL_GTAXE16000:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 L6 */
			int ports[7] = { 0, 1, 2, 3, 4, 5, 6};
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4 5 6");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
		case MODEL_GTBE98:
		case MODEL_GTBE98_PRO:
		case MODEL_GTBE96:
		case MODEL_GTBE19000:
		case MODEL_GTBE19000AI:
		case MODEL_GTBE96_AI:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 L6 */
			int ports[7] = { 0, 1, 1, 1, 1, 2, 3 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				if(is_rtl8372_boardid())
					nvram_set("lanports", "1 1 1 1 2 3");
				else
					nvram_set("lanports", "1 2 3 4 5 6");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			if (is_rtl8372_boardid()) {
				eval("mknod", "/dev/rtkswitch", "c", "206", "0");
#if defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				f_write_string("/sys/class/leds/led_gpio_52/brightness", "255", 0, 0);
				usleep(40*1000);
				f_write_string("/sys/class/leds/led_gpio_52/brightness", "0", 0, 0);
#else
#if defined(GTBE98_PRO) || defined(GTBE98) || defined(GTBE96)
				if ((hnd_boardid_cmp("GT-BE98PRO_ICP") == 0) || (hnd_boardid_cmp("GT-BE98_ICP") == 0) || (hnd_boardid_cmp("GT-BE96_ICP") == 0))
				{
					f_write_string("/sys/class/leds/led_gpio_52/brightness", "255", 0, 0);
					usleep(40*1000);
					f_write_string("/sys/class/leds/led_gpio_52/brightness", "0", 0, 0);
				}
				else
#endif
				{
					f_write_string("/sys/class/leds/led_gpio_14/brightness", "255", 0, 0);
					usleep(40*1000);
					f_write_string("/sys/class/leds/led_gpio_14/brightness", "0", 0, 0);
				}
#endif
				eval("insmod", "rtl8372");
				int count_war = 10;
				while (((rtkswitch_serdes_status() & 0xff) == 0xe0) && (--count_war >= 0)) {
					dbg("status NG. reinit...\n");
					system("rtkswitch 451");
					sleep(3);
				}
				system("rtkswitch 6");
			}
			break;
		}
#endif
#if defined(RTBE92U) || defined(RTBE95U)
		case MODEL_RTBE92U:
		case MODEL_RTBE95U:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 */
			int ports[5] = { 0, 1, 1, 1, 1 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 1 1 1");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			eval("mknod", "/dev/rtkswitch", "c", "206", "0");
			f_write_string("/sys/class/leds/led_gpio_11/brightness", "255", 0, 0);
			usleep(40*1000);
			f_write_string("/sys/class/leds/led_gpio_11/brightness", "0", 0, 0);
			eval("insmod", "rtl8372");
			int count_war = 10;
			while (((rtkswitch_serdes_status() & 0xff) == 0xe0) && (--count_war >= 0)) {
				dbg("status NG. reinit...\n");
				system("rtkswitch 451");
				sleep(3);
			}
			if (!hnd_boardid_cmp("RT-BE92U_N"))
				system("rtkswitch 452");
			system("rtkswitch 6");

			break;
		}
#endif
#if defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
		case MODEL_RTBE82U:
		case MODEL_RTBE82M:
		case MODEL_RTBE58U_PRO:
		case MODEL_GSBE18000:
		case MODEL_GT7:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 */
#if defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			int ports[8] = { 3, 4, 5, 6, 7, 0, 1, 2 };
#elif defined(RTBE82M)
			int ports[5] = { 0, 1, 2, 3, 4 };
#elif defined(GT7)
			int ports[6] = { 0, 0, 1, 2, 3, 8 };
#else
			int ports[5] = { 0, 1, 1, 1, 1 };
#endif
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
#if defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				nvram_set("lanports", "4 5 6 7 0 1 2");
#elif defined(RTBE82M)
				nvram_set("lanports", "1 2 3 4");
#elif defined(GT7)
				nvram_set("lanports", "0 1 2 3 8");
#else
				nvram_set("lanports", "1 1 1 1");
#endif
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			eval("mknod", "/dev/mxlswitch", "c", "207", "0");
#else
			eval("mknod", "/dev/rtkswitch", "c", "206", "0");
#endif
#if defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			f_write_string("/sys/class/leds/led_gpio_28/brightness", "255", 0, 0);
#else
			f_write_string("/sys/class/leds/led_gpio_27/brightness", "255", 0, 0);
#endif
			usleep(40*1000);
#if defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			f_write_string("/sys/class/leds/led_gpio_28/brightness", "0", 0, 0);
#else
			f_write_string("/sys/class/leds/led_gpio_27/brightness", "0", 0, 0);
#endif
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			eval("insmod", "mxl");
#else
			eval("insmod", "rtl8372");
			int count_war = 10;
			while (((rtkswitch_serdes_status() & 0xff) == 0xe0) && (--count_war >= 0)) {
				dbg("status NG. reinit...\n");
				system("rtkswitch 451");
				sleep(3);
			}
			system("rtkswitch 6");
#endif
			break;
		}
#endif
		case MODEL_ET12:
		case MODEL_XT12:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 */
			int ports[6] = { 0, 1, 2, 3 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}

		case MODEL_RTAX86U_PRO:
		{
			/* set wanports in init_nvram for dualwan */
			/* WAN L1 L2 L3 L4 L5 */
			int ports[6] = { 0, 1, 2, 3, 4, 5 };
			char buf[64], *ptr;
			int i, len, wancfg;
			int tmp_type;

			if (get_wans_dualwan() & WANSCAP_LAN) {
				wancfg = nvram_get_int("wans_lanport");

				memset(buf, 0, sizeof(buf));
				ptr = buf;
				for (i = 1; i < ARRAYSIZE(ports); ++i) {
					if (i == wancfg)
						continue;

					len = strlen(buf);
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[i]);
					ptr = buf+strlen(buf);
				}

				nvram_set("lanports", buf);
			}
			else {
				wancfg = 0;
				nvram_set("lanports", "1 2 3 4 5");
			}

			memset(buf, 0, sizeof(buf));
			for (i = WAN_UNIT_FIRST, ptr = buf; ((tmp_type = get_dualwan_by_unit(i)) != WANS_DUALWAN_IF_NONE) && (i < WAN_UNIT_MAX) && (wancfg < ARRAYSIZE(ports)); ++i) {
				len = strlen(buf);
				if (tmp_type == WANS_DUALWAN_IF_WAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[0]);
				else if (tmp_type == WANS_DUALWAN_IF_LAN)
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", ports[wancfg]);
				else if (tmp_type == WANS_DUALWAN_IF_DSL)
					snprintf(ptr, sizeof(buf)-len, "%s-1", (len > 0)?" ":"");
				else // USB
					snprintf(ptr, sizeof(buf)-len, "%s%d", (len > 0)?" ":"", -1);
				ptr = buf+strlen(buf);
			}
			nvram_set("wanports", buf);

			break;
		}
	}

#ifdef RTCONFIG_EMF
	eval("insmod", "emf");
	eval("insmod", "igs");
#endif

	hnd_nat_ac_init(1);
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO) && !defined(GT7)
	enable_jumbo_frame();
#endif
#if defined(RTCONFIG_EXT_BCM53134) && !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	accept_multicast_forward();
#endif

#ifdef RTCONFIG_LACP
	config_lacp();
#endif
}

int
switch_exist(void)
{
	return 1;
}

void ext_vconfig(char *interface, int port_id)
{
	if(!interface)
		return;
#if defined(EBG19)
	if(strncmp(interface, "ethsw_", 6) == 0)
		eval("vconfig", "add", "eth5", port_id);
	else
#endif
		eval("vconfig", "add", interface, port_id);
}

#if defined(EBG19)
int get_extsw_port(char *ethsw_name)
{
	if(ethsw_name && strlen(ethsw_name) > 6)
		return atoi(ethsw_name+6);
	else
		return -1;
}

void enable_ext_53134_8021qvlan()
{
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x3400", "-l", "2", "-d", "0xe3");
}

void add_ext_53134_vlan(int vlanid, char *untag_ifnames, char *fwd_ifnames)
{
	char vid[12], untag_bits[12], fwd_bits[12], vsetting[12];
	int port=-1; 
	unsigned int bits_set = 0;
	char word[256], *next;

	snprintf(vid, sizeof(vid)-1, "0x%x", vlanid);


	foreach(word, fwd_ifnames, next) {
		port = get_extsw_port(word);
		if(port >= 0)
			bits_set |= 1<<port;
	}

	foreach(word, untag_ifnames, next) {
		port = get_extsw_port(word);
		if(port >= 0)
			bits_set |= 1<<port+9;
	}

	snprintf(vsetting, sizeof(vsetting), "0x%x", bits_set);

	//_dprintf("%s, vid:[%s], fwd:[%s], untag:[%s], v_setting:(%x)[%s]\n", __func__, vid, fwd_ifnames, untag_ifnames, bits_set, vsetting);

	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0581", "-l", "2", "-d", vid);
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0583", "-l", "4", "-d", vsetting);
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0580", "-l", "2", "-d", "0x80");
}

void read_ext_53134_vlan(int vlanid)
{
	char vid[12];

	snprintf(vid, sizeof(vid)-1, "0x%x", vlanid);

	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0581", "-l", "2", "-d", vid);
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0580", "-l", "2", "-d", "0x81");
	eval("ethswctl", "-c", "pmdioaccess", "-x", "0x0583", "-l", "4");
}

#endif

#ifdef RTCONFIG_RTL_UMGMT

#define MAX_SDN_VLAN	8
#define VPORT_MASK	0xf

enum SDN_PORT_MODE
{
	SDN_DEFAULT,
	SDN_TRUNK,
	SDN_ACCESS
};

typedef struct Sdn_ExtPorts_Vlan {
	int vid;
	int prio;
	uint32_t mbr_untags;	// 0~15: mbr; 16~31: untags;
} SDN_EXTPORTS_VLAN;

/* vlan entries */
SDN_EXTPORTS_VLAN extVlan[MAX_SDN_VLAN];

/* switch's real port number, not lan tag */
int get_extport_no(char *ifname, char *ethif)
{
	int model, pn = -1;

	switch(model = get_model()) {
#if defined(EBG19)
	case MODEL_EBG19:
		if(strncmp(ifname, "ethsw_", 6) == 0) {
			if(ethif)
				strlcpy(ethif, "eth5", 5);

			return atoi(ifname+6);
		}
		break;
#endif
#if defined(RTAX3000N)
	case MODEL_RTAX3000N:
		if(strncmp(ifname, "ethsw_", 6) == 0) {
			if(ethif)
				strlcpy(ethif, "eth1", 5);
			return atoi(ifname+6);
		}
		break;
#endif
	default:
		break;
	}

	return pn;
}

typedef enum rtk_vlan_acceptFrameType_e
{
    ACCEPT_FRAME_TYPE_ALL = 0,             /* untagged, priority-tagged and tagged */
    ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged */
    ACCEPT_FRAME_TYPE_UNTAG_ONLY,     /* untagged and priority-tagged */
    ACCEPT_FRAME_TYPE_END
} rtk_vlan_acceptFrameType_t;

/* 0 on fail, 1 on success */
int add_extrtl_vlan(int vid, int prio, char *ifname, int p_mode, char *ethif)
{
	int i, pn;
	int ret = 0;
        char mask_str[] = "0x00000000XXX";
        char *set_mask_argv[] = { "rtkswitch", "397", mask_str , NULL };	// portAccessFrameType
	unsigned int portinfo = 0;

	pn = get_extport_no(ifname, ethif);
	_dprintf("add vif:<%s>, pn=%d, ethif=[%s]\n", ifname, pn, ethif);

	for(i=0; i<MAX_SDN_VLAN; ++i) {
		if(extVlan[i].vid == vid) {
			extVlan[i].prio = prio;
			extVlan[i].mbr_untags |= (1 << pn);
			portinfo |= pn;
			if(p_mode == SDN_ACCESS) {
				extVlan[i].mbr_untags |= (1 << (pn + 16));
				portinfo |= (ACCEPT_FRAME_TYPE_UNTAG_ONLY << 4);
			} else if(p_mode == SDN_TRUNK) {
				portinfo |= (ACCEPT_FRAME_TYPE_TAG_ONLY << 4);
			} else if(p_mode == SDN_DEFAULT) {
				portinfo |= (ACCEPT_FRAME_TYPE_ALL << 4);
			}

			sprintf(mask_str, "0x%08x", portinfo);
			_eval(set_mask_argv, NULL, 0, NULL);

			ret = 1;
			break;
		}
	}

	if(i==MAX_SDN_VLAN) {
		for(i=0; i<MAX_SDN_VLAN; ++i) {
			if(!extVlan[i].vid) {
				extVlan[i].vid = vid;
				extVlan[i].prio = prio;
				extVlan[i].mbr_untags |= (1 << pn);
				portinfo |= pn;
				if(p_mode == SDN_ACCESS) {
					extVlan[i].mbr_untags |= (1 << (pn + 16));
					portinfo |= (ACCEPT_FRAME_TYPE_UNTAG_ONLY << 4);
				} else if(p_mode == SDN_TRUNK) {
					portinfo |= (ACCEPT_FRAME_TYPE_TAG_ONLY << 4);
				} else if(p_mode == SDN_DEFAULT) {
					portinfo |= (ACCEPT_FRAME_TYPE_ALL << 4);
				}

				sprintf(mask_str, "0x%08x", portinfo);
				_eval(set_mask_argv, NULL, 0, NULL);
				ret = 1;
				break;
			}
		}
	}

	if(i==MAX_SDN_VLAN)
		_dprintf("%s, exceed max vconfigs\n", __func__);


	//return extVlan[i].mbr_untags;
	return ret;
}

/* 0 on fail, 1 on success and still mbrs, 2 on success and no mbrs */
int rem_extrtl_vlan(int vid, char *ifname, char *ethif)
{
	int i, pn;
	int ret = 0;

	pn = get_extport_no(ifname, ethif);
	_dprintf("remove vif:<%s>, pn=%d, ethif=%s\n", ifname, pn, ethif);

	for(i=0; i<MAX_SDN_VLAN; ++i) {
		if(extVlan[i].vid == vid) {
			extVlan[i].mbr_untags &= ~(1 << pn);
			extVlan[i].mbr_untags &= ~(1 << (pn + 16));
			extVlan[i].mbr_untags &= 0xff;
			
			if(extVlan[i].mbr_untags == 0) {
				extVlan[i].vid = 0;
				extVlan[i].prio = 0;
				ret = 2;
			} else
				ret = 1;
			break;
		}
	}

	if(i==MAX_SDN_VLAN)
		_dprintf("%s fail rem %s.(not found)\n", __func__, ifname);

	return ret;
}

int if_mbrs(int vid)
{
	int i, ret = 0;

	for(i=0; i<MAX_SDN_VLAN; ++i) {
		if(extVlan[i].vid == vid) {
			if(extVlan[i].mbr_untags & 0xf) {
				ret = 1;
				break;
			}
		}
	}

	return ret;
}

void zero_extrtl_vlan(int vid, int prio)
{
	int i;

	for(i=0; i<MAX_SDN_VLAN; ++i)
		memset(&extVlan[i], 0, sizeof(SDN_EXTPORTS_VLAN));
}

void dump_extVlan()
{
	int i;

	_dprintf("%s:\n", __func__);
	for(i=0; i<MAX_SDN_VLAN; ++i) {
		_dprintf("[%d]:[%d][%d][0x%X]\n", i, extVlan[i].vid, extVlan[i].prio, extVlan[i].mbr_untags);
	}
	_dprintf("\n");
}

/* apply all rtl switch vlan configs */
int start_extrtl_vlan()	
{
	int i, vid, prio, vports = 0, non_vports = 0;
	unsigned int portinfo;
	char vlan_str[] = "4096XXX";
	char prio_str[] = "7XXX";
	char mask_str[] = "0x00000000XXX";
	char *set_accf_argv[] = { "rtkswitch", "395", NULL};
	char *set_vlan_argv[] = { "rtkswitch", "36", vlan_str , NULL };
	char *set_prio_argv[] = { "rtkswitch", "37", prio_str , NULL };
	char *set_mask_argv[] = { "rtkswitch", "39", mask_str , NULL };

	_eval(set_accf_argv, NULL, 0, NULL);

	for(i=0; i<MAX_SDN_VLAN; ++i) {
		if(extVlan[i].vid) {
			vid = extVlan[i].vid;
			portinfo = extVlan[i].mbr_untags;
			prio = extVlan[i].prio;
			vports |= (portinfo & 0xffff);

			if (vid > 4096) {
				_dprintf("%s: invalid vid %d\n", __func__, vid);
				continue;
			}

			if (prio > 7)
				prio = 0;

			_dprintf("%s: vid %d prio %d portinfo 0x%08x\n", __func__, vid, prio, portinfo);

			sprintf(vlan_str, "%d", vid);
			_eval(set_vlan_argv, NULL, 0, NULL);

			if (prio >= 0) {
				sprintf(prio_str, "%d", prio);
				_eval(set_prio_argv, NULL, 0, NULL);
			}

			sprintf(mask_str, "0x%08x", portinfo);
			_eval(set_mask_argv, NULL, 0, NULL);
		}
	}

	/* non-tag group */
	sprintf(vlan_str, "%d", 0);
	_eval(set_vlan_argv, NULL, 0, NULL);

	non_vports = vports ^ 0xffff & VPORT_MASK;
	sprintf(mask_str, "0x%08x", non_vports);

	_dprintf("%s vports:%x, non_vports:%x\n", __func__, vports, non_vports);
	_eval(set_mask_argv, NULL, 0, NULL);

	return 0;
}

#endif

#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
/*
#if defined(RTBE82M)
#define WAN_PORT 1
#define CPU_WAN_PORT 6
#define CPU_LAN_PORT 7
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
#define WAN_PORT 4
#define CPU_WAN_PORT 9
#define CPU_LAN_PORT 10
#else
#error "Unsupported platform"
#endif
*/

/**
 * Initialize MxL862xx VLAN.
 * All LAN ports is set to default mode.
 */
int mxls_init()
{
	int ret = 0;
	uint8_t port, portnum;
	uint8_t default_fid = 0;
	int wait;

	api_gsw_get_links("");

	portnum = get_chip_port_num();
	printf("%s: portnum=%d, (init:%d)\n", __func__, portnum, nvram_get_int("mxl_init"));  fflush(stdout);
	logmessage("VLAN", "%s: portnum=%d\n", __func__, portnum);

	if (nvram_match("mxl_init", "1") && !nvram_match("force_mxl_init", "1")) {
		printf("No need to re-init mxl switch. (f:%d)\n", nvram_get_int("force_mxl_init"));
		return 0;
	}
	
	ret = tag_vlan_init();

	if (ret) {
		LOG_ERR("Failed to re-init tag vlan: %d", ret);
	}
	printf("%s, tag_vlan_init, ret=%d\n\n", __func__, ret);

	if (wait = nvram_get_int("tag_init_wait")) {
		printf("%s, tag init wait:%d\n", __func__, wait);
		fflush(stdout);
		sleep(wait);
	}

	for (port=1; port <= portnum; port++) {
		bp_enable(map_port_idx(port), 0);
		if (port != WAN_PORT && port != CPU_WAN_PORT) {
			ret |= set_port_based_vlan_cfg(port, default_fid, false, true);
			printf("%s, set_port_based_vlan_cfg w/ default_fid:%d, ret=%d\n", __func__, default_fid, ret);
		} else {
			uint8_t wan_fid = 1;
			ret |= set_port_based_vlan_cfg(port, wan_fid, false, false);
			printf("%s, set_port_based_vlan_cfg w/ wan_fid:%d, ret=%d\n", __func__, wan_fid, ret);
		}
	}
	
	if (!ret)
		nvram_set("mxl_init", "1");
	else
		nvram_set("mxl_init", "-1");

	return ret;
}

int __remove_vlan(int vid, unsigned int mask)
{
        uint8_t fid = 0;
	uint8_t bp = 0, discard_untag, discard_tag;
        uint8_t port, portnum;
	int ret = 0, ret_1 = 0, ret_2 = 0;
	uint16_t p_fid, default_fid = 0;

	if (!vid)
		return 0;

	api_gsw_get_links("");

        portnum = get_chip_port_num();
	mask |= BIT(CPU_LAN_PORT);

	logmessage("VLAN", "%s: vid:%d, mask:0x%x\n", __func__, vid, mask);
	printf("%s(vlan_%d), portnum=%d, mask=0x%x\n", __func__, vid, portnum, mask);

	fid = get_fid_vlan(vid);
	printf("%s, vid(%d):fid=%d\n", __func__, vid, fid);

	if (fid == 0xFF) {
		LOG_ERR("%s: non-exist vlan-%d network\n", vid);
		return -1;
	}

	for (port=1; port <= portnum; port++) {
		if (mask & BIT(port)) {
			bp = get_port_fid_tag_bp(port, fid);

			printf("%s, [port:%d, vid/fid:%d/%d], get bp=%d\n", __func__, port, vid, fid, bp);
			if (bp) {
				ret_1 = reset_tag_based_vlan_cfg(bp);
				printf("%s, reset_tag_based_vlan_cfg, ret=%d\n", __func__, ret_1);
				ret_2 = port_fid_tag_bp_cfg(false, port, fid);
				printf("%s, port_fid_tag_bp_cfg, ret=%d\n", __func__, ret_2);

				ret |= ret_1 | ret_2;
			} else if (get_port_based_vlan_cfg(port, &p_fid, &discard_untag, &discard_tag) == 0 && p_fid == fid) {
				/* Change access vlan to vlan 0 (fid=0) */
				ret |= set_port_based_vlan_fid(port, default_fid);
				printf("%s, set_port_based_vlan_fid, ret=%d\n", __func__, ret);
			} else {
				LOG_ERR("%s, invalid bp(%d) p:%d, fid:%d! \n", __func__, bp, port, fid);
				printf("%s, invalid bp(%d)! p:%d, fid:%d! \n", __func__, bp, port, fid);
				//ret = -1;
			}
		}
	}
	printf("%s, ret=%d\n", __func__, ret);

	return ret;
}

int discard_packets(unsigned int mask, int discard_untag, int discard_tag)
{
	int ret = 0;
	uint8_t fid = 0;
	uint8_t port, portnum;

	api_gsw_get_links("");

	portnum = get_chip_port_num();

	printf("%s, portnum=%d, mask=0x%08x, discard_untag_tag:%d/%d\n", __func__, portnum, mask, discard_untag, discard_tag);
	logmessage("VLAN", "%s: portnum=%d, mask=0x%x, discard_untag_tag:%d/%d\n", __func__, portnum, mask, discard_untag, discard_tag);


	for (port=1; port <= portnum; port++) {
		if (mask & BIT(port)) {
			ret |= set_port_based_discard_cfg(port, discard_untag, discard_tag);
			printf("%s, set trunk port [%d] discard-untag/tag: %d/%d, ret=%d\n", __func__, port, discard_untag, discard_tag, ret);
		}
	}

	return ret;
}

/*
int isolate_ports(int enable, unsigned int mask)
{
}
*/

#endif

#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
/**
 * Setup a VLAN.
 * @vid:	VLAN ID
 * @prio:	VLAN PRIO
 * @mask:	bit31~16:	untag mask
 * 		bit15~0:	port member mask
 * @return:
 * 	0:	success
 *  otherwise:	fail
 *
 * bit definition of untag mask/port member mask
 *		RT-AX1800	RT-AX55
 * 0:	Port 0	LAN1		LAN4
 * 1:	Port 1	LAN2		LAN3
 * 2:	Port 2	LAN3		LAN2
 * 3:	Port 3	LAN4		LAN1
 * 17:	CPU(LAN), always leave vlan tag on CPU to vconfig interface
 */
int __setup_vlan(int vid, int prio, unsigned int mask)
{
// MXL switch
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
	int ret = 0, ret1 = 0, ret2 = 0;
	uint16_t _vid = (uint16_t)vid;
	uint8_t outer_pri = prio;
	uint8_t fid = 0;
	uint8_t bp = 0;
	uint8_t port, portnum;
	unsigned int member_portmask = (mask & 0xFFFF) | BIT(CPU_LAN_PORT);
	unsigned int untag_portmask  = (mask >> 16) & 0xFFFF;

	api_gsw_get_links("");

	portnum = get_chip_port_num();
	printf("%s(dbg.vlan), vid=%d, prio=%d, mask=0x%08x, chip portnum is %d, <mbr:0x%04x, untag:0x%04x>.\n", __func__, vid, prio, mask, portnum, member_portmask, untag_portmask);
	logmessage("VLAN", "%s: vid=%d, prio=%d, mask=0x%08x, chip portnum is %d, <mbr:0x%04x, untag:0x%04x>.\n", __func__, vid, prio, mask, portnum, member_portmask, untag_portmask);

	if (_vid == 0) {
		fid = 0;  /* default fid */
		for (port=1; port <= portnum; port++) {
			if (port == WAN_PORT || port == CPU_WAN_PORT)
				continue;
			if (member_portmask & BIT(port)) {
				//default forwarding (disable vlan filtering)
				//set_port_based_vlan_cfg(uint8_t port, uint16_t fid, uint8_t discard_untag, uint8_t discard_tag);
				ret |= set_port_based_vlan_cfg(port, fid, false, false);
				printf("%s, vid[%d] set mbr port [%d] discard-untag/tag: 0, 0, ret=%d\n", __func__, _vid, port, ret);
			} else {
				ret |= set_port_based_vlan_cfg(port, fid, true, false);
				printf("%s, vid[%d] set non-mbr-port [%d] discard-untag/tag: 1, 0, ret=%d\n", __func__, _vid, port, ret);
			}
		}
		return ret;
	}
	else {
		fid = get_fid_vlan(_vid);
		printf("%s, vlan[%d]->fid[%d/%02x]\n", __func__, _vid, fid, fid);

		if (fid == 0xFF && member_portmask) {
			printf("%s, re-cfg vlan fid\n", __func__);
			if ((fid = fid_vlan_cfg(true, _vid)) < 0) {
				printf("%s, invalid fid cfg\n", __func__);
				return (-1);
			}
			printf("%s, update, vlan[%d]->fid[%d/%02x]\n", __func__, _vid, fid, fid);
		}
		else {
			printf("%s, invalid params (0x%02x, 0x%04x).\n", __func__, fid, member_portmask);
			return (-1); /* invalid params */
		}
	}

	printf("\n%s, handle untag/tag...\n", __func__);
	for (port=1; port <= portnum; port++) {
		bp = get_port_fid_tag_bp(port, fid);
		printf("%s, port_fid[%d_%d]->bp[%d]\n", __func__, port, fid, bp);

		if (member_portmask & BIT(port)) {
			if (untag_portmask & BIT(port)) {
				/* untagged vlan */
				if (bp) {
					ret |= reset_tag_based_vlan_cfg(bp);
					ret |= port_fid_tag_bp_cfg(false, port, fid);
					printf("%s, [untag_mbr] reset/cfg bp[%d] for port_fid[%d_%d]\n", __func__, bp, port, fid);
				}
				ret |= set_port_based_vlan_cfg(port, fid, false, true);
				printf("%s, [untag_mbr] set (non-tag)mbr port [%d] discard-untag/tag: 0, 1, ret=%d\n", __func__, port, ret);
			}
			else {
				/* tagged vlan */
				if (!bp) {
					printf("%s, [tag_mbr] retrieve bp again\n", __func__);
					if ((bp=port_fid_tag_bp_cfg(true, port, fid)) < 0) {
						printf("%s, [tag_mbr] cannot get bp.\n", __func__);
						return (-1);
					}
				}
				ret1 = set_tag_based_vlan_cfg(bp, 0, port, fid, _vid, 0, GSW_EXTENDEDVLAN_FILTER_TPID_8021Q, 0);
				ret2 = set_tag_based_vlan_egress_priority(bp, GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL, 0, outer_pri,
							0, GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI, 0);
				ret |= (ret1 | ret2);
				printf("%s, [tag mbr] set (tag)mbr port [%d] to tag-based-vlan_cfg then prio: bp=%d, ret=%d(%d/%d)\n", __func__, port, bp, ret, ret1, ret2);
			}
		}
		else {
			if (bp) {
				ret |= reset_tag_based_vlan_cfg(bp);
				ret |= port_fid_tag_bp_cfg(false, port, fid);
				printf("%s, [non-mbr], w/ bp=%d, reset/disable its vlan\n", __func__, bp);
			}
		}
	}

	if (!member_portmask) {
		printf("%s, disable vlan due no mbr_mask\n", __func__);
		fid_vlan_cfg(false, _vid);
	}
	fflush(stdout);
#else
// RTK switch
	static int initial_vlan = 0;
	char vlan_str[] = "4096XXX";
	char prio_str[] = "7XXX";
	char mask_str[] = "0x00000000XXX";
	char *set_vlan_argv[] = { "rtkswitch", "36", vlan_str , NULL };
	char *set_prio_argv[] = { "rtkswitch", "37", prio_str , NULL };
	char *set_mask_argv[] = { "rtkswitch", "39", mask_str , NULL };

	/* initial vlan first time only */
	if(!initial_vlan) {
		system("rtkswitch 38 0");
		initial_vlan = 1;
	}

	if (vid > 4096) {
		_dprintf("%s: invalid vid %d\n", __func__, vid);
		return -1;
	}

	if (prio > 7)
		prio = 0;

	_dprintf("%s: vid %d prio %d mask 0x%08x\n", __func__, vid, prio, mask);

	if (vid >= 0) {
		sprintf(vlan_str, "%d", vid);
		_eval(set_vlan_argv, NULL, 0, NULL);
	}

	if (prio >= 0) {
		sprintf(prio_str, "%d", prio);
		_eval(set_prio_argv, NULL, 0, NULL);
	}

	sprintf(mask_str, "0x%08x", mask);
	_eval(set_mask_argv, NULL, 0, NULL);

	return 0;
#endif
}

#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
int
__setup_vlan_sdn(int vid, int prio, unsigned int mask)
{
	int ret = 0, ret1 = 0, ret2 = 0;
	uint16_t _vid = (uint16_t)vid;
	uint8_t outer_pri = (uint8_t)prio;
	uint8_t fid = 0;
	uint8_t bp = 0;
	uint8_t port, portnum;
	unsigned int member_portmask = (mask & 0xFFFF) | BIT(CPU_LAN_PORT);
	unsigned int untag_portmask  = (mask >> 16) & 0xFFFF;

	api_gsw_get_links("");

	portnum = get_chip_port_num();
	LOG_DBG("%s(dbg.vlan_sdn), vid=%d, prio=%d, mask=0x%08x, chip portnum is %d, <mbr:0x%04x, untag:0x%04x>.\n",
	__func__, vid, prio, mask, portnum, member_portmask, untag_portmask);

	if (_vid == 0) {
		fid = 0;  /* default fid */
		for (port=1; port <= portnum; port++) {
			if (port == WAN_PORT || port == CPU_WAN_PORT)
			continue;
			if (member_portmask & BIT(port)) {
				//default forwarding (disable vlan filtering)
				ret |= set_port_based_vlan_cfg(port, fid, false, false);
			} else {
				ret |= set_port_based_vlan_cfg(port, fid, true, false);
			}
		}
		return ret;
	}
	else {
		fid = get_fid_vlan(_vid);
		if (fid == 0xFF && member_portmask) {
			if ((fid = fid_vlan_cfg(true, _vid)) < 0) {
				LOG_ERR("%s, invalid fid cfg\n", __func__);
				return (-1);
			}
		}
		/*else {
			LOG_ERR("%s, invalid params (0x%02x, 0x%04x).\n", __func__, fid, member_portmask);
			return (-1); //invalid params
		}*/
		LOG_DBG("%s, vlan[%d]->fid[%d/%02x]\n", __func__, _vid, fid, fid);
	}

        for (port=1; port <= portnum; port++) {
                bp = get_port_fid_tag_bp(port, fid);
                LOG_DBG("%s, port_fid[%d_%d]->bp[%d]\n", __func__, port, fid, bp);

                if (member_portmask & BIT(port)) {
                        if (untag_portmask & BIT(port)) {
                                /* untagged vlan */
                                if (bp) {
                                        ret |= reset_tag_based_vlan_cfg(bp);
                                        ret |= port_fid_tag_bp_cfg(false, port, fid);
                                }
                                ret |= set_port_based_vlan_fid(port, fid);
                                LOG_DBG("%s, [untag_mbr] set mbr port [%d], ret=%d\n", __func__, port, ret);
                        }
                        else {
                                /* tagged vlan */
                                if (!bp) {
                                        LOG_ERR("%s, [tag_mbr] retrieve bp again\n", __func__);
                                        if ((bp=port_fid_tag_bp_cfg(true, port, fid)) < 0) {
                                                return (-1);
                                        }
                                }
                                ret1 = set_tag_based_vlan_cfg(bp, 0, port, fid, _vid, 0, GSW_EXTENDEDVLAN_FILTER_TPID_8021Q, 0);
                                ret2 = set_tag_based_vlan_egress_priority(bp, GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL, 0, outer_pri,
                                                        0, GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI, 0);
                                ret |= (ret1 | ret2);
                                LOG_DBG("%s, [tag mbr] set to tag-based-vlan_cfg then prio: bp=%d, ret=%d(%d/%d)\n", __func__, bp, ret, ret1, ret2);
                        }
                }
                else if (bp) {
                        ret |= reset_tag_based_vlan_cfg(bp);
                        ret |= port_fid_tag_bp_cfg(false, port, fid);
                        LOG_DBG("%s, [non-mbr], w/ bp=%d, reset/disable its vlan\n", __func__, bp);
                }
        }

        if (!member_portmask) {
                LOG_DBG("%s, disable vlan %d due no mbr_mask\n", __func__, _vid);
                fid_vlan_cfg(false, _vid);
        }

        return ret;
}
#endif

void vlan_forwarding(int vid, int prio, int stb, int untag)
{
	char *wan_if = "eth0";
	static int wan_idx = 1, br_idx = IPTV_BR_INDEX + 1;
	char wanVlanDev[10];	
	char vlan_cmd[255] = {0};
	unsigned int mask = 0;

	_dprintf("%s, Vid=%d, prio=%d, stb=%d, untag=%d\n", __func__, vid, prio, stb, untag);
	if (!nvram_match("skip_vlanctl", "1")) {
		snprintf(vlan_cmd, sizeof(vlan_cmd), "vlanctl --mcast --if-create %s %d", wan_if, wan_idx);
		system(vlan_cmd);
	}
	snprintf(wanVlanDev, sizeof(wanVlanDev), "eth0.v%d", wan_idx++);
	eval("ifconfig", wanVlanDev, "allmulti", "up");
	/* Forward packets from WAN to specific LAN port */
	if (!nvram_match("skip_vlanctl", "1")) {
		snprintf(vlan_cmd, sizeof(vlan_cmd), "vlanctl --if %s --rx --tags 1 --filter-vid %d 0 --set-rxif %s --rule-append", wan_if, vid, wanVlanDev);
		system(vlan_cmd);
	}
	/* same vid case */
	if(stb == 6) {
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000)
		mask |= 1 << 7;	// LAN3
		mask |= 1 << 4;	// LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
		mask |= 1 << 6;	// LAN3
		mask |= 1 << 7;	// LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
		mask |= 1 << abs(3 - 4);
		mask |= 1 << abs(4 - 4);
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
		mask |= 1 << 2;	// LAN3
		mask |= 1 << 1; // LAN4
#elif defined(RTBE82M)
		mask |= 1 << 4;	// LAN3
		mask |= 1 << 5; // LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
		mask |= 1 << 7;	// LAN3
		mask |= 1 << 8; // LAN4
#else
		mask |= 1 << (3 - 1);
		mask |= 1 << (4 - 1);
#endif
	}
	else {
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000)
		if (stb == 1)
			mask |= 1 << 5;	// LAN1
		else if (stb == 2)
			mask |= 1 << 6;	// LAN2
		else if (stb == 3)
			mask |= 1 << 7;	// LAN3
		else if (stb == 4)
			mask |= 1 << 4;	// LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
		if (stb == 1)
			mask |= 1 << 4;	// LAN1
		else if (stb == 2)
			mask |= 1 << 5;	// LAN2
		else if (stb == 3)
			mask |= 1 << 6;	// LAN3
		else if (stb == 4)
			mask |= 1 << 7;	// LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
		mask |= 1 << abs(stb - 4);
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
		mask |= 1 << abs(5 - stb);
#elif defined(RTBE82M)
		if (stb == 1)
			mask |= 1 << 2;	// LAN1
		else if (stb == 2)
			mask |= 1 << 3;	// LAN2
		else if (stb == 3)
			mask |= 1 << 4;	// LAN3
		else if (stb == 4)
			mask |= 1 << 5;	// LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
		if (stb == 1)
			mask |= 1 << 5;	// LAN1
		else if (stb == 2)
			mask |= 1 << 6;	// LAN2
		else if (stb == 3)
			mask |= 1 << 7;	// LAN3
		else if (stb == 4)
			mask |= 1 << 8;	// LAN4
#else
		mask |= 1 << (stb - 1);
#endif
	}

	/* untag mask */
	if (untag == 1) {
		mask |= mask << 16;
	}
	/* Meo(Bridge Mode) LAN4 leave tag */
	else if(untag == 2) {
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000)
		mask |= abs((1 << 4) << 16);	// LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
		mask |= abs((1 << 7) << 16);	// LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
		mask |= abs(1 << 16);
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
		mask |= abs((1 << 1) << 16);	// LAN4
#elif defined(RTBE82M)
		mask |= abs((1 << 5) << 16);	// LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
		mask |= abs((1 << 8) << 16);	// LAN4
#else
		mask |= abs((1 << 3) << 16);
#endif
	}
	
	_dprintf("[%s(%d)] vid=%d, prio=%d, mask=0x%08x, untag=%d\n", __FUNCTION__, __LINE__, vid, prio, mask, untag);
	__setup_vlan(vid, prio, mask);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "vconfig add eth1 %d", vid);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "ifconfig vlan%d allmulti up", vid);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "brctl addbr br%d", br_idx);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "bcmmcastctl mode -i br%d -p 1 -m 0", br_idx);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "bcmmcastctl mode -i br%d -p 2 -m 0", br_idx);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "ifconfig br%d up", br_idx);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "brctl addif br%d %s", br_idx, wanVlanDev);
	system(vlan_cmd);
	snprintf(vlan_cmd, sizeof(vlan_cmd), "brctl addif br%d vlan%d", br_idx++, vid);
	system(vlan_cmd);
}
#endif

#if defined(RTBE82M)
void config_switch(void)
{
	nvram_set("mxl_init", "0");
	nvram_set("apg_df_ports", "0");
#ifdef RTCONFIG_DUALWAN
	int unit = 0;
	char wan_if[10];

	if (is_router_mode() && nvram_get("wans_dualwan")) {
		for(unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
			if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
/* The LAN/WAN port mapping:
    - CPU0: port9/xpcs0
    - CPU1: port13/xpcs1
    - WAN: port1
    - LAN1: port2
    - LAN2: port3
    - LAN3: port4
    - LAN4: port5

    wans_lanport == 1 case:
	Vlan0-BID0: LAN2/LAN3/LAN4/CPU1 untag BID0 (bp3-5,13)
	Vlan0-BID1: WAN-CPU0-BID1
	Vlan2-BID2: LAN1 untag BID2 (ctp2/bp2), CPU1 tagged vid2 BID2 (ctp13/new-BP17)
*/
				if (nvram_match("wans_lanport", "1")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2");

// LAN2/LAN3/LAN4/CPU1, LAN1/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=2, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x10 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x10 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x4 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=2, blockId=16*(i-1)=0x10)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=2 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x10");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (2-5,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP3-5&13, portmap: bit3-5,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2030");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2028");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2018");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x38");
				} else if (nvram_match("wans_lanport", "2")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2");

// LAN1/LAN3/LAN4/CPU1, LAN2/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=3, blockId=16*(i-1)=0x20)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x20 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x20 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x8 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=3, blockId=16*(i-1)=0x20)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=3 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x20");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (2-5,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP3-5&13, portmap: bit 2,4,5,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2030");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2024");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2014");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x34");
				} else if (nvram_match("wans_lanport", "3")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2");

// LAN1/LAN2/LAN4/CPU1, LAN3/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=4, blockId=16*(i-1)=0x30)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x30 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x30 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=4, blockId=16*(i-1)=0x30)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=4 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x30");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (2-5,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP3-5&13, portmap: bit 2,3,5,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2028");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2024");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2c");
				} else if (nvram_match("wans_lanport", "4")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2");

// LAN1/LAN2/LAN3/CPU1, LAN4/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=5, blockId=16*(i-1)=0x40)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x40 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x40 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=5, blockId=16*(i-1)=0x40)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=5 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x40");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (2-5,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP3-5&13, portmap: bit 2,3,4,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2018");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2014");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x1c");
				} else return;

				printf("DUAL WAN: Set specific LAN as WAN\n");
				eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
				eval("vconfig", "add", "eth1", "2");
			}
		}
	}
#endif
}
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
void config_switch(void)
{
	nvram_set("mxl_init", "0");
	nvram_set("apg_df_ports", "0");
#ifdef RTCONFIG_DUALWAN
	int unit = 0;
	char wan_if[10];
	if (is_router_mode() && nvram_get("wans_dualwan")) {
		for(unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
			if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
/* The LAN/WAN port mapping:
    - CPU0: port9/xpcs0
    - CPU1: port13/xpcs1
    - WAN: port4
    - LAN1: port5
    - LAN2: port6
    - LAN3: port7
    - LAN4: port8
    - LAN5: port1
    - LAN6: port2
    - LAN7: port3

    wans_lanport == 1 case:
	Vlan0-BID0: LAN2/LAN3/LAN4/CPU1 untag BID0 (bp6-8,1-3,13)
	Vlan0-BID1: WAN-CPU0-BID1
	Vlan2-BID2: LAN1 untag BID2 (ctp2/bp2), CPU1 tagged vid2 BID2 (ctp13/new-BP17)
*/
				if (nvram_match("wans_lanport", "1")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10");

// LAN2/LAN3/LAN4/CPU1, LAN1/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86282), the blockId=160
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=5, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x40 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x40 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=160");

//CTP2 (port=5, blockId=16*(i-1)=0x40)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=5 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x40");

//xpcs1(CTP13) (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x90");

//On PORT_BPs (6-8,1-3,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP6-8&1-3&13, portmap: bit 6-8,1-3,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=6 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x218e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=7 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x214e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=8 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ce");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21cc");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21ca");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21c6");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x1ce");
				} else if (nvram_match("wans_lanport", "2")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10");

// LAN1/LAN3/LAN4/CPU1, LAN2/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86282), the blockId=160
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=6, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x50 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x50 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x40 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=160");

//CTP2 (port=6, blockId=16*(i-1)=0x50)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=6 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x50");

//xpcs1(CTP13) (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x90");

//On PORT_BPs (5,7-8,1-3,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=6 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP5,7-8&1-3&13, portmap: bit 5,7-8,1-3,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x218e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=7 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x212e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=8 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ae");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21ac");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21aa");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21a6");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x1ae");
				} else if (nvram_match("wans_lanport", "3")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10");

// LAN1/LAN2/LAN4/CPU1, LAN3/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86282), the blockId=160
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x80 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=160");

//CTP2 (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=7 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//xpcs1(CTP13) (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x90");

//On PORT_BPs (5-6,8,1-3,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=7 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP5-6,8&1-3&13, portmap: bit 5-6,8,1-3,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x214e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=6 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x212e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=8 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x206e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x216c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x216a");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2166");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x16e");
				} else if (nvram_match("wans_lanport", "4")) {
// WAN-CPU0 (It is the default settings in start config)
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10");

// LAN1/LAN2/LAN3/CPU1, LAN4/CPU1-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86282), the blockId=160
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=8, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x70 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x70 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x90 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=160 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=2 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x100 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=160");

//CTP2 (port=8, blockId=16*(i-1)=0x70)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=8 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x70");

//xpcs1(CTP13) (port=10, blockId=16*(i-1)=0x90)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x90");

//On PORT_BPs (5-7,1-3,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=8 nBridgeId=2 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP5-7&1-3&13, portmap: bit 5-7,1-3,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=5 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ce");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=6 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ae");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=7 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x206e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ec");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20ea");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20e6");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0xee");
				} else return;

				printf("DUAL WAN: Set specific LAN as WAN\n");
				eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
				eval("vconfig", "add", "eth1", "2");
			}
		}
	}
#endif
}
#elif defined(GT7)
void config_switch(void)
{
	nvram_set("mxl_init", "0");
	nvram_set("apg_df_ports", "0");
#ifdef RTCONFIG_DUALWAN
	int unit = 0;
	char wan_if[10];

	if (is_router_mode() && nvram_get("wans_dualwan")) {
		for(unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
			if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
/* The LAN/WAN port mapping:
    - CPU: port13/xpcs1
    - LAN1: port1
    - LAN2: port2
    - LAN3: port3
    - LAN4: port4
    - LAN5: port9

    wans_lanport == 1 case:
	Vlan0-BID0: LAN2/LAN3/LAN4/LAN5/CPU untag BID0 (bp2-4,9,13)
	Vlan2-BID1: LAN1 untag BID1 (ctp1/bp1), CPU tagged vid2 BID1 (ctp13/new-BP17)
*/
				if (nvram_match("wans_lanport", "1")) {
					if (nvram_get_int("wans_extwan")) return;
// LAN2/LAN3/LAN4/LAN5/CPU, LAN1/CPU-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=1, blockId=16*(i-1)=0x0)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x0 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x0 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=1 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=1, blockId=16*(i-1)=0x0)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=1 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x0");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (1-4,9,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP2-4&9&13, portmap: bit2-4,9,13 exclude itself
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2218");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2214");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x220c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x201c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21c");
				} else if (nvram_match("wans_lanport", "2")) {
// LAN1/LAN3/LAN4/LAN5/CPU, LAN2/CPU-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=2, blockId=16*(i-1)=0x10)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x10 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x10 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=1 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x4 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=2, blockId=16*(i-1)=0x10)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=2 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x10");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (1-4,9,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP1&3&4&9&13, portmap: bit1,3,4,9,13 exclude itself
if (nvram_get_int("wans_extwan")) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2210");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2208");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2018");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x218");
} else {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2218");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2212");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x220a");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x201a");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21a");
}
				} else if (nvram_match("wans_lanport", "3")) {
// LAN1/LAN2/LAN4/LAN5/CPU, LAN3/CPU-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=3, blockId=16*(i-1)=0x20)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x20 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x20 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=1 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x8 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=3, blockId=16*(i-1)=0x20)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=3 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x20");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (1-4,9,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP1&2&4&9&13, portmap: bit1,2,4,9,13 exclude itself
if (nvram_get_int("wans_extwan")) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2210");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2204");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2014");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x214");
} else {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2214");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2212");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2206");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2016");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x216");
}
				} else if (nvram_match("wans_lanport", "4")) {
// LAN1/LAN2/LAN3/LAN5/CPU, LAN4/CPU-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=4, blockId=16*(i-1)=0x30)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x30 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x30 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=2 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=2 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=1 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x10 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=4, blockId=16*(i-1)=0x30)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=4 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x30");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (1-4,9,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP1&3&4&9&13, portmap: bit1,3,4,9,13 exclude itself
if (nvram_get_int("wans_extwan")) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2208");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2204");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20c");
} else {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x220c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x220a");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2206");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200e");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20e");
}
				} else return;

				printf("DUAL WAN: Set specific LAN as WAN\n");
				eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
				eval("vconfig", "add", "eth1", "2");
			}
		}
	}
#endif
}
#else
void config_switch(void)
{
#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(GTBE96_AI) //handle dualwan on rtkswitch
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (!is_rtl8372_boardid())
		return;
#endif
#ifdef RTCONFIG_DUALWAN
	int unit = 0;
	char wan_if[10];
	if (is_router_mode()) {
		if (nvram_get("wans_dualwan")) {
			for(unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; ++unit) {
				if (get_dualwan_by_unit(unit) == WANS_DUALWAN_IF_LAN) {
					if (nvram_match("wans_lanport", "1")) {
						/* add vlan 2 as WAN from rtkswitch */
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
						if (nvram_get_int("wans_extwan")) return;
						__setup_vlan(2, 0, 0x00200020); //LAN1 as WAN
						__setup_vlan(0, 0, 0x00D000D0); //no-tag fwd mask except LAN1
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
						if (nvram_get_int("wans_extwan")) return;
						__setup_vlan(2, 0, 0x00100010); //LAN1 as WAN
						__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN1
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
						__setup_vlan(2, 0, 0x00080008); //LAN1 as WAN
						__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN1
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
						if (nvram_get_int("wans_extwan")) return;
						__setup_vlan(2, 0, 0x00100010); //LAN1 as WAN
						__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN1
#else
						__setup_vlan(2, 0, 0x00010001); //LAN1 as WAN
						__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN1
#endif
					}
					else if (nvram_match("wans_lanport", "2")) {
						/* add vlan 2 as WAN from rtkswitch */
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00400040);	//LAN2 as WAN
						__setup_vlan(0, 0, 0x00900090);	//no-tag fwd mask except LAN1/LAN2
						} else {
						__setup_vlan(2, 0, 0x00400040); //LAN2 as WAN
						__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN2
						}
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00200020);	//LAN2 as WAN
						__setup_vlan(0, 0, 0x00C000C0);	//no-tag fwd mask except LAN1/LAN2
						} else {
						__setup_vlan(2, 0, 0x00200020); //LAN2 as WAN
						__setup_vlan(0, 0, 0x00D000D0); //no-tag fwd mask except LAN2
						}
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
						__setup_vlan(2, 0, 0x00040004); //LAN2 as WAN
						__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN2
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00080008);	//LAN2 as WAN
						__setup_vlan(0, 0, 0x00060006);	//no-tag fwd mask except LAN2
						} else {
						__setup_vlan(2, 0, 0x00080008); //LAN2 as WAN
						__setup_vlan(0, 0, 0x00160016); //no-tag fwd mask except LAN2
						}
#else
						__setup_vlan(2, 0, 0x00020002); //LAN2 as WAN
						__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN2
#endif
					}
					else if (nvram_match("wans_lanport", "3")) {
						/* add vlan 2 as WAN from rtkswitch */
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00800080);	//LAN3 as WAN
						__setup_vlan(0, 0, 0x00500050);	//no-tag fwd mask except LAN1/LAN3
						} else {
						__setup_vlan(2, 0, 0x00800080); //LAN3 as WAN
						__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
						}
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00400040);	//LAN3 as WAN
						__setup_vlan(0, 0, 0x00A000A0);	//no-tag fwd mask except LAN1/LAN3
						} else {
						__setup_vlan(2, 0, 0x00400040); //LAN3 as WAN
						__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
						}
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
						__setup_vlan(2, 0, 0x00020002); //LAN3 as WAN
						__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00040004);	//LAN3 as WAN
						__setup_vlan(0, 0, 0x000A000A);	//no-tag fwd mask except LAN3
						} else {
						__setup_vlan(2, 0, 0x00040004); //LAN3 as WAN
						__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
						}
#else
						__setup_vlan(2, 0, 0x00040004); //LAN3 as WAN
						__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
					}
					else if (nvram_match("wans_lanport", "4")) {
						/* add vlan 2 as WAN from rtkswitch */
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00100010); //LAN4 as WAN
						__setup_vlan(0, 0, 0x00C000C0); //no-tag fwd mask except LAN1/LAN4
						} else {
						__setup_vlan(2, 0, 0x00100010); //LAN4 as WAN
						__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
						}
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00800080); //LAN4 as WAN
						__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN1/LAN4
						} else {
						__setup_vlan(2, 0, 0x00800080); //LAN4 as WAN
						__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
						}
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
						__setup_vlan(2, 0, 0x00010001); //LAN4 as WAN
						__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
						if (nvram_get_int("wans_extwan")) {
						__setup_vlan(2, 0, 0x00020002);	//LAN4 as WAN
						__setup_vlan(0, 0, 0x000C000C);	//no-tag fwd mask except LAN4
						} else {
						__setup_vlan(2, 0, 0x00020002); //LAN4 as WAN
						__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
						}
#else
						__setup_vlan(2, 0, 0x00080008); //LAN4 as WAN
						__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
					}
					else return;
					
					printf("DUAL WAN: Set specific LAN as WAN\n");
					eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
					eval("vconfig", "add", "eth1", "2");
					//sprintf(wan_if, "vlan2");
					//eval("ifconfig", wan_if, "allmulti", "up");
					//add_wan_phy(wan_if);
				}
			}
		}
	}
#endif
#endif
}
#endif

#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(GT7)
int vlan4094_enabled()
{
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (!is_rtl8372_boardid())
		return 0;
#endif
	if (re_mode() || is_router_mode() || !nvram_get_int("x_Setting")) {
		if (is_router_mode() &&
			(strcmp(nvram_safe_get("switch_wantag"), "none") ||
			 strcmp(nvram_safe_get("switch_stb_x"), "0")))
			return 0;

		if (nvram_get_int("wans_extwan") && ((get_wans_dualwan() & WANSCAP_LAN) && (nvram_get_int("wans_lanport") >= 2) && (nvram_get_int("wans_lanport") <= 4))) {
			return 1;
		} else {
			if (!nvram_get_int("wans_extwan") && (get_wans_dualwan() & WANSCAP_LAN))
				return 0;
			else
				return 1;
		}
	}

	return 0;
}

#ifdef GT7
void config_extwan_gt7(void)
{
	int lan_port_excl = 0;

	if (nvram_get_int("wans_extwan") && ((get_wans_dualwan() & WANSCAP_LAN) && (nvram_get_int("wans_lanport") >= 2) && (nvram_get_int("wans_lanport") <= 4))) {
		if (nvram_match("wans_lanport", "2")) {
				//LAN2 as WAN
				//no-tag fwd mask except LAN1/LAN2
				lan_port_excl = 2;
		} else if (nvram_match("wans_lanport", "3")) {
				//LAN3 as WAN
				//no-tag fwd mask except LAN1/LAN3
				lan_port_excl = 3;
		} else if (nvram_match("wans_lanport", "4")) {
				//LAN4 as WAN
				//no-tag fwd mask except LAN1/LAN4
				lan_port_excl = 4;
		}
	} else {
		if (!nvram_get_int("wans_extwan") && (get_wans_dualwan() & WANSCAP_LAN))
			return;
	}
/* The LAN/WAN port mapping:
    - CPU: port13/xpcs1
    - LAN1: port1
    - LAN2: port2
    - LAN3: port3
    - LAN4: port4
    - LAN5: port9

    wans_lanport == 1 case:
	Vlan0-BID0: LAN2/LAN3/LAN4/LAN5/CPU untag BID0 (bp2-4,9,13)
	Vlan2-BID1: LAN1 untag BID1 (ctp1/bp1), CPU tagged vid2 BID1 (ctp13/new-BP17)
*/

// LAN2/LAN3/LAN4/LAN5/CPU, LAN1/CPU-newBP
system("fapi-GSW-BridgeAlloc");

// GSW_BRIDGE_portAlloc_t bpnew = {.nBridgePortId = 17};
system("fapi-GSW-BridgePortAlloc");

//Assign new exVLAN. Must save the returned "blockId"
//In this case (86252), the blockId=112
system("fapi-GSW-ExtendedVlanAlloc nNumberOfEntries=3");

//CTP2 to set exVLAN, discard stag,dtag (port=1, blockId=16*(i-1)=0x0)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x0 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x0 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13) to set exVLAN, discard stag,dtag (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=1 eOuterVlanFilterVlanType=2 eInnerVlanFilterVlanType=3 eRemoveTagAction=3");
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=2 eOuterVlanFilterVlanType=1 eInnerVlanFilterVlanType=2 eRemoveTagAction=3");

//xpcs1(CTP13), filter single tag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=3 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=4094 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=3 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//xpcs1(CTP13), filter double tag (inner tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=0x60 nEntryIndex=4 \
	eOuterVlanFilterVlanType=0 \
	bOuterVlanFilterVidEnable=1 nOuterVlanFilterVidVal=4094 \
	eOuterVlanFilterTpid=1 \
	eInnerVlanFilterVlanType=1 \
	eRemoveTagAction=1 \
	bReassignBridgePortEnable=1 nNewBridgePortId=17");

//new-tag-BP17, treatment untag
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=0 \
	eOuterVlanFilterVlanType=3 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=4094 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment stag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=1 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=3 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=4094 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//new-tag-BP17, treatment dtag (tag transparent)
system("fapi-GSW-ExtendedVlanSet nExtendedVlanBlockId=112 nEntryIndex=2 \
	eOuterVlanFilterVlanType=2 \
	eInnerVlanFilterVlanType=2 \
	bOuterVlanActionEnable=1 \
	eOuterVlanActionTpid=3 \
	eOuterVlanActionVidMode=0 eOuterVlanActionVidVal=4094 \
	eOuterVlanActionPriorityMode=2 eOuterVlanActioneDei=2");

//Bind CTP, EXVLAN, BP together

//new-BP17, portmap=BP2
system("fapi-GSW-BridgePortConfigSet nBridgePortId=17 \
	nBridgeId=1 nDestLogicalPortId=13 \
	bMcDestIpLookupDisable=0 \
	bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2 nBridgePortMapIndex[1]=0x0 \
	bEgressExtendedVlanEnable=1 nEgressExtendedVlanBlockId=112");

//CTP2 (port=1, blockId=16*(i-1)=0x0)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=1 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x0");

//xpcs1(CTP13) (port=7, blockId=16*(i-1)=0x60)
system("fapi-GSW-CtpPortConfigSet nLogicalPortId=13 bIngressExtendedVlanEnable=1 nIngressExtendedVlanBlockId=0x60");

//On PORT_BPs (1-4,9,13) to set bridge ID and bridge port member

//BP2, portmap: bit17
system("fapi-GSW-BridgePortConfigSet nBridgePortId=1 nBridgeId=1 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x0 nBridgePortMapIndex[1]=0x2");

//BP2-4&9&13, portmap: bit2-4,9,13 exclude itself
	if (lan_port_excl == 2) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2210");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2208");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2018");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x218");
	} else if (lan_port_excl == 3) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2210");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2204");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2014");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x214");
	} else if (lan_port_excl == 4) {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2208");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2204");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x200c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x20c");
	} else {
system("fapi-GSW-BridgePortConfigSet nBridgePortId=2 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2218");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=3 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x2214");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=4 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x220c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=9 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x201c");
system("fapi-GSW-BridgePortConfigSet nBridgePortId=13 nBridgeId=0 bBridgePortMapEnable=1 nBridgePortMapIndex[0]=0x21c");
	}
}
#endif

void config_extwan(void)
{
#if !defined(RTCONFIG_BCM_MFG) && !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
	if (!hnd_boardid_cmp("GT-BE98_BCM"))
#endif
		return;
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
	if (!is_rtl8372_boardid())
		return;
#endif

	/* config_extwan will conflict with IPTV tag feature */
	if (strcmp(nvram_safe_get("switch_wantag"), "none") || strcmp(nvram_safe_get("switch_stb_x"), "0"))
		return;

	if (re_mode() || is_router_mode() || !nvram_get_int("x_Setting")) {
#ifdef GT7
		config_extwan_gt7();
#else
		if (nvram_get_int("wans_extwan") && ((get_wans_dualwan() & WANSCAP_LAN) && (nvram_get_int("wans_lanport") >= 2) && (nvram_get_int("wans_lanport") <= 4))) {
#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2)
			if (nvram_match("wans_lanport", "2")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN2 as WAN
				__setup_vlan(0, 0, 0x00060006);		//no-tag fwd mask except LAN1/LAN2
			} else if (nvram_match("wans_lanport", "3")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN3 as WAN
				__setup_vlan(0, 0, 0x000A000A);		//no-tag fwd mask except LAN1/LAN3
			} else if (nvram_match("wans_lanport", "4")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN4 as WAN
				__setup_vlan(0, 0, 0x000C000C);		//no-tag fwd mask except LAN1/LAN4
			}
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
			if (nvram_match("wans_lanport", "2")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN2 as WAN
				__setup_vlan(0, 0, 0x00C000C0);		//no-tag fwd mask except LAN1/LAN2
			} else if (nvram_match("wans_lanport", "3")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN3 as WAN
				__setup_vlan(0, 0, 0x00A000A0);		//no-tag fwd mask except LAN1/LAN3
			} else if (nvram_match("wans_lanport", "4")) {
				__setup_vlan(4094, 0, 0x00100010);	//LAN4 as WAN
				__setup_vlan(0, 0, 0x00600060);		//no-tag fwd mask except LAN1/LAN4
			}
#else
			if (nvram_match("wans_lanport", "2")) {
				__setup_vlan(4094, 0, 0x00200020);	//LAN2 as WAN
				__setup_vlan(0, 0, 0x00900090);		//no-tag fwd mask except LAN1/LAN2
			} else if (nvram_match("wans_lanport", "3")) {
				__setup_vlan(4094, 0, 0x00200020);	//LAN3 as WAN
				__setup_vlan(0, 0, 0x00500050);		//no-tag fwd mask except LAN1/LAN3
			} else if (nvram_match("wans_lanport", "4")) {
				__setup_vlan(4094, 0, 0x00200020);	//LAN4 as WAN
				__setup_vlan(0, 0, 0x00C000C0);		//no-tag fwd mask except LAN1/LAN4
			}
#endif
		} else {
			if (!nvram_get_int("wans_extwan") && (get_wans_dualwan() & WANSCAP_LAN))
				return;
#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2)
			__setup_vlan(4094, 0, 0x00100010);	//LAN1 as WAN
			__setup_vlan(0, 0, 0x000E000E);		//no-tag fwd mask except LAN1
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
			__setup_vlan(4094, 0, 0x00100010);	//LAN1 as WAN
			__setup_vlan(0, 0, 0x00E000E0);		//no-tag fwd mask except LAN1
#else
			__setup_vlan(4094, 0, 0x00200020);	//LAN1 as WAN
			__setup_vlan(0, 0, 0x00D000D0);		//no-tag fwd mask except LAN1
#endif
		}
#endif
		eval("vconfig", "set_name_type", "VLAN_PLUS_VID_NO_PAD");
		eval("vconfig", "add", "eth1", "4094");

		if (re_mode() || !nvram_get_int("x_Setting"))
			ifconfig("vlan4094", IFUP | IFF_ALLMULTI, NULL, NULL);
	}
}

void reset_extwan(void)
{
#if !defined(RTCONFIG_BCM_MFG) && !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
	if (iface_exist("vlan4094")) {
		system("vconfig rem vlan4094");
#ifndef GT7
		system("rtkswitch 382");
#endif
	}
#endif
}
#endif

#else // HND_ROUTER
#ifdef RTCONFIG_BCM_7114
void ctf_blocknat()
{
	f_write_string("/proc/net/ctfnat", "1", 0, 0);
	nvram_unset("ctf_nonat_force");
}

int vpncoppp()
{
	char tmp[100], wan_prefix[] = "wanXXXXXXXXXX_";
	int ret = 0;

	if(nvram_match("passvc", "1"))
		return 0;

	snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());

	if (!nvram_match("vpnc_proto", "disable") && nvram_match(strcat_r(wan_prefix, "proto", tmp), "pppoe"))
		ret = 1;
	else
		ret = 0;

	nvram_set_int("vpncoppp", ret);

	return ret;
}
#endif

void init_switch()
{
	generate_switch_para();

#if defined(RTCONFIG_EXT_RTL8365MB) || defined(RTCONFIG_EXT_RTL8370MB)
	eval("mknod", "/dev/rtkswitch", "c", "233", "0");
	eval("insmod", "rtl8365mb");
#ifdef RTCONFIG_RESET_SWITCH
	int i, r, gpio_nr = atoi(nvram_safe_get("reset_switch_gpio"));
	led_control(LED_RESET_SWITCH, 0);
	usleep(400 * 1000);

	for (i=0; i<10; ++i) {
		led_control(LED_RESET_SWITCH, 1);
		if ((r = get_gpio(gpio_nr)) != 1) {
			_dprintf("\n! reset LED_RESET_SWITCH failed:%d, reset again !\n", r);
			usleep(10 * 1000);
		} else {
			_dprintf("\nchk LED_RESET_SWITCH:%d\n", r);
			break;
		}
	}
#endif
#endif

#ifdef CONFIG_BCMWL5
	// ctf should be disabled when some functions are enabled
	if (IS_TQOS() || IS_BW_QOS()
#if defined(RTCONFIG_AMAS) && (defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM4708))
	|| (sw_mode() == SW_MODE_AP && nvram_match("re_mode", "1")
#ifdef RTCONFIG_BCM4708
	&& is_wgn_enabled()
#endif
	)
#endif
	|| nvram_get_int("ctf_disable_force")
#ifndef RTCONFIG_BCMARM
	|| sw_mode() == SW_MODE_REPEATER
#endif
	|| nvram_get_int("cstats_enable") == 1
	) {
		nvram_set("ctf_disable", "1");
	}
#if defined(RTCONFIG_BWDPI)
	else if (check_bwdpi_nvram_setting() && is_router_mode()) {
		nvram_set("ctf_disable", "0");
	}
#endif
	else {
		nvram_set("ctf_disable", "0");
	}
#ifdef RTCONFIG_BCMFA
	fa_nvram_adjust();
#endif
#ifdef RTCONFIG_BCM_7114
	int retv = vpncoppp();

        if( nvram_match("ctf_disable", "0") &&
	(nvram_match("ctf_nonat_force", "1") || retv))
		ctf_blocknat();
#endif

/* Requires bridge netfilter, but slows down and breaks EMF/IGS IGMP IPTV Snooping
	if (is_router_mode() && nvram_get_int("qos_enable") == 1) {
		// enable netfilter bridge only when phydev is used
		f_write_string("/proc/sys/net/bridge/bridge-nf-call-iptables", "1", 0, 0);
		f_write_string("/proc/sys/net/bridge/bridge-nf-call-ip6tables", "1", 0, 0);
		f_write_string("/proc/sys/net/bridge/bridge-nf-filter-vlan-tagged", "1", 0, 0);
	}
*/
#endif

#ifdef RTCONFIG_SHP
	if (nvram_get_int("qos_enable") == 1 || nvram_get_int("lfp_disable_force")) {
		nvram_set("lfp_disable", "1");
	} else {
		nvram_set("lfp_disable", "0");
	}

	if (nvram_get_int("lfp_disable") == 0) {
		restart_lfp();
	}
#endif

#if defined(RTCONFIG_BCMFA) && !defined(RTCONFIG_BCM_7114)
	if (!nvram_get("ctf_fa_cap")) {
		char ctf_fa_mode_bak[2];
		int nvram_ctf_fa_mode = 0;

		if (nvram_get("ctf_fa_mode"))
		{
			nvram_ctf_fa_mode = 1;
			strcpy(ctf_fa_mode_bak, nvram_safe_get("ctf_fa_mode"));
		}

		nvram_set_int("ctf_fa_mode", 2);
		eval("insmod", "et");
		FILE *fp;
		if ((fp = fopen("/proc/fa", "r"))) {
			/* FA is capable */
			fclose(fp);
			nvram_set_int("ctf_fa_cap", 1);
			ATE_BRCM_SET("ctf_fa_cap", "1");
		} else {
			nvram_set_int("ctf_fa_cap", 0);
			ATE_BRCM_SET("ctf_fa_cap", "0");
		}

		if (nvram_ctf_fa_mode)
			nvram_set("ctf_fa_mode", ctf_fa_mode_bak);
		else
			nvram_unset("ctf_fa_mode");
		nvram_commit();
		ATE_BRCM_UNSET("Ate_power_on_off_ret");
		ATE_BRCM_COMMIT();

		eval("rmmod", "et");

		dbg("FA (hardware Flow Accelarator) cap recorded, rebooting...\n");
		reboot(RB_AUTOBOOT);
		sleep(1);
	} else if (!cfe_nvram_get("ctf_fa_cap")) {
		dbg("commit original FA cap info into cfe nvram space\n");
		ATE_BRCM_SET("ctf_fa_cap", nvram_get("ctf_fa_cap"));
		ATE_BRCM_UNSET("Ate_power_on_off_ret");
		ATE_BRCM_COMMIT();
	}
#endif

#ifdef RTAC68U
	if (hardware_flag() == 0x00000020 && !nvram_get("forcegen1rc")) {
		nvram_set_int("forcegen1rc", 1);
		ATE_BRCM_SET("forcegen1rc", "1");
		nvram_commit();
		ATE_BRCM_COMMIT();

		dbg("add flag to force PCI Express Gen1 mode, rebooting...\n");
		reboot(RB_AUTOBOOT);
		sleep(1);
	}
#endif

	// ctf must be loaded prior to any other modules
	if (nvram_get_int("ctf_disable") == 0)
		eval("insmod", "ctf");

#ifdef RTCONFIG_EMF
	eval("insmod", "emf");
	eval("insmod", "igs");
#endif

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA) && (defined(RTAC68U) || defined(RTAC56U) || defined(RTAC56S) || defined(RTN18U) || defined(DSL_AC68U) || defined(RT4GAC68U))
	char psta[8];
	snprintf(psta, sizeof(psta), "psta=%d", (psta_exist() || psr_exist()));
	eval("insmod", "et", psta);
#else
	eval("insmod", "et");
#endif
	eval("insmod", "bcm57xx");

	enable_jumbo_frame();
	ether_led();

#ifdef RTCONFIG_DSL_REMOTE
	init_switch_dsl();
	config_switch_dsl();
#endif
#ifdef RTCONFIG_EXT_RTL8365MB
	/* link up rtkswitch after bcm rgmii init */
	eval("rtkswitch", "1");
#endif
#ifdef RTCONFIG_EXT_RTL8370MB
	/* link up rtkswitch after bcm rgmii init */
	eval("rtkswitch", "0");
#endif
#ifdef RTCONFIG_LACP
	config_lacp();
#endif

#if defined(RTCONFIG_GMAC3) || defined(RTCONFIG_BCMFA)
	if (nvram_get_int("gmac3_enable")
#ifdef RTCONFIG_BCMFA
		|| nvram_get_int("ctf_fa_mode") == CTF_FA_NORMAL
#endif
	) {
		eval("et", "-i", "eth0", "robowr", "0x4", "0x4", "0", "6");
	}
#endif

}

int
switch_exist(void)
{
	int ret = 1;

	if (get_switch() == SWITCH_UNKNOWN) {
		_dprintf("No switch interface!!!\n");
		ret = 0;
	}

	return ret;
}

void config_switch(void)
{
	generate_switch_para();

}
#endif // HND_ROUTER

#ifdef RTCONFIG_HND_ROUTER_AX
void eth_phypower(char *port, int onoff){

	char cmd[64];
#ifdef RTCONFIG_BONDING_WAN
	char word[64], *next;
#endif
	
#ifdef RTCONFIG_BONDING_WAN
	if(strcmp(port, "bond1") == 0){
		foreach(word, nvram_safe_get("bond_wan_ifnames"), next){
			snprintf(cmd, sizeof(cmd),
				"ethctl %s phy-power %s", word, onoff ? "up" : "down");
			system(cmd);
		}
	}
	else
#endif
	if(!strncmp(port, "br1", 3) || !strncmp(port, "vlan", 4) || !strncmp(port, "eth", 3))
	{
		snprintf(cmd, sizeof(cmd), "ethctl %s phy-power %s", wan_if_eth(), onoff ? "up" : "down");
		system(cmd);
	}
	else
	{
		snprintf(cmd, sizeof(cmd), "ethctl %s phy-power %s", port, onoff ? "up" : "down");
		system(cmd);
	}
}
#endif

#ifdef RTCONFIG_BCMWL6
extern struct nvram_tuple bcm4360ac_defaults[];

static void
set_bcm4360ac_vars(void)
{
	struct nvram_tuple *t;

	/* Restore defaults */
	for (t = bcm4360ac_defaults; t->name; t++) {
		if (!nvram_get(t->name))
			nvram_set(t->name, t->value);
	}
}
#endif

#ifndef RTCONFIG_MSSID_REALMAC
/* mac_binary[0], mac_binary[1], ... , mac_binary[5] */
/* vif_addr[5], vif_addr[4], ... , vif_addr[0] */
/* refer to wlconf.c */
unsigned char gen_mssid_hwaddr_mac0(unsigned char mac_binary[], int unit)
{
	char vif_addr[WLC_IOCTL_SMLEN];
	int max_wl_cap_mbss = 16 ; /* to meet all model, always using mbss16 to generate mac0 */
	int adjustment = 0 ;

	memcpy(vif_addr, mac_binary, ETHER_ADDR_LEN);

	if(vif_addr[0] == 96){
		if(vif_addr[5]%(max_wl_cap_mbss -1) == 0) adjustment = 1;
		else adjustment = 0;
	}
	vif_addr[0] = 96 + ((vif_addr[5]+adjustment)%(max_wl_cap_mbss -1) * 8);

	ETHER_SET_LOCALADDR(vif_addr);

	return vif_addr[0];
}
#endif

static void
reset_mssid_hwaddr(int unit)
{
	char macaddr_str[18], macbuf[13];
	char *macaddr_strp;
	unsigned char mac_binary[6];
	unsigned long long macvalue, macvalue_local;
	unsigned char *macp;
	int model = get_model();
	int idx, subunit;
#ifndef RTCONFIG_MSSID_REALMAC
	int max_mssid = (num_of_mssid_support(unit) > 3) ? 3 : num_of_mssid_support(unit);
	int max_wl_cap_mbss = wl_cap_mbss(unit);
#else
	int max_mssid = num_of_mssid_support(unit);
#endif
	char tmp[100], prefix[]="wlXXXXXXX_";
	int unit_total = num_of_wl_if();
	int max_no_vifs = wl_max_no_vifs(unit);
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
	int psr = (is_psr(unit)
			|| dpsr_mode()
#ifdef RTCONFIG_DPSTA
			|| dpsta_mode()
#endif
			|| rp_mode()
			);
#endif

#ifndef RTCONFIG_MSSID_REALMAC
	unsigned char vif_mac0 = 0;
#endif

#if defined(GTAXE16000)
const unsigned int devpath_idx[4] = {4, 2, 1, 3};    // 5G-1, 5G-2, 6G, 2.4G
#elif defined(GTBE98)
const unsigned int devpath_idx[4] = {1, 4, 2, 3};    // 5G-1, 5G-2, 6G, 2.4G
#elif defined(BT10)
const unsigned int devpath_idx[3] = {ENVRAM_6G_DEVPATH, ENVRAM_5G_DEVPATH, ENVRAM_2G_DEVPATH};    // 6G, 5G, 2G (devpath, not wlx index)
#elif defined(BQ16)
const unsigned int devpath_idx[4] = {0, 2, 1, 3};    // 5G-1, 5G-2, 6G, 2.4G
#elif defined(BQ16_PRO)
const unsigned int devpath_idx[4] = {0, 1, 2, 3};    // 5G, 6G-1, 6G-2, 2.4G
#elif defined(GTBE98_PRO)
const unsigned int devpath_idx[4] = {4, 2, 1, 3};    // 5G, 6G-1, 6G-2, 2.4G
#elif defined(GTAX11000_PRO)
const unsigned int devpath_idx[4] = {3, 4, 1, 2};    // 2.4G, 5G-1, 5G-2
#elif defined(GT10) || defined(RTAX9000)
const unsigned int devpath_idx[4] = {1, 2, 0};    // 2.4G, 5G-1, 5G-2
#elif defined(RTBE96U) || defined(GTBE19000) || defined(GTBE19000AI)
const unsigned int devpath_idx[4] = {3, 4, 2, 1};    // 2.4G, 5G, 6G
#elif defined(GTBE96)
const unsigned int devpath_idx[4] = {3, 1, 4};	// 2.4G, 5G-1, 5G-2
#elif defined(RTBE88U)
const unsigned int devpath_idx[4] = {0, 1};	// 2.4G, 5G
#elif defined(GTBE96_AI)
const unsigned int devpath_idx[4] = {3, 2, 4};  // 2.4G, 5G-1, 5G-2
#endif

	if (unit > (unit_total - 1))
		return;

#ifndef RTCONFIG_MSSID_REALMAC
	for (subunit = 1; subunit < max_wl_cap_mbss /* WL_MAXBSSCFG */; subunit++){
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
		nvram_unset(strcat_r(prefix, "hwaddr", tmp));
	}
#endif

	for (idx = 0; idx < unit_total ; idx++) {
		memset(mac_binary, 0x0, 6);
		memset(macbuf, 0x0, 13);

		switch(model) {
			case MODEL_RTN53:
			case MODEL_RTN16:
			case MODEL_RTN15U:
			case MODEL_RTN12:
			case MODEL_RTN12B1:
			case MODEL_RTN12C1:
			case MODEL_RTN12D1:
			case MODEL_RTN12VP:
			case MODEL_RTN12HP:
			case MODEL_RTN12HP_B1:
			case MODEL_APN12HP:
			case MODEL_RTN14UHP:
			case MODEL_RTN10U:
			case MODEL_RTN10P:
			case MODEL_RTN10D1:
			case MODEL_RTN10PV2:
			case MODEL_RTAC53U:
				if (unit == 0)	/* 2.4G */
					snprintf(macaddr_str, sizeof(macaddr_str), "sb/1/macaddr");
				else		/* 5G */
					snprintf(macaddr_str, sizeof(macaddr_str), "0:macaddr");
				break;
			case MODEL_RTN66U:
			case MODEL_RTAC66U:
				snprintf(macaddr_str, sizeof(macaddr_str), "pci/%d/1/macaddr", unit + 1);
				break;
			case MODEL_RTN18U:
			case MODEL_RTAC68U:
			case MODEL_RTAC3200:
			case MODEL_DSLAC68U:
			case MODEL_RTAC87U:
			case MODEL_RTAC56S:
			case MODEL_RTAC56U:
			case MODEL_RTAC5300:
			case MODEL_RTAC88U:
			case MODEL_RTAC86U:
			case MODEL_RTAC3100:
			case MODEL_RTAX95Q:
			case MODEL_XT8PRO:
			case MODEL_BT12:
			case MODEL_BM68:
			case MODEL_XT8_V2:
			case MODEL_RTAXE95Q:
			case MODEL_ET8PRO:
			case MODEL_ET8_V2:
			case MODEL_RTAX56_XD4:
			case MODEL_XD4PRO:
			case MODEL_XC5:
			case MODEL_CTAX56_XD4:
			case MODEL_RTAX58U:
			case MODEL_RTAX82U_V2:
			case MODEL_TUFAX5400_V2:
			case MODEL_RTAX5400:
			case MODEL_RTAX82_XD6S:
			case MODEL_XD6_V2:
			case MODEL_RTAX58U_V2:
			case MODEL_RTAXE7800:
			case MODEL_RTAX86U:
			case MODEL_RTAX68U:
			case MODEL_RTAC68U_V4:
			case MODEL_RTAX86U_PRO:
			case MODEL_RTBE86U:
#ifdef RTAC3200
				if (unit < 2)
					snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", 1 - unit);
				else
#endif
				snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", unit);
				break;
			case MODEL_GTAC5300:
				snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", unit + 1);
				break;
			case MODEL_RTAX55:
			case MODEL_TUFAX3000_V2:
			case MODEL_RTAX3000N:
			case MODEL_BR63:
			case MODEL_RPAX56:
			case MODEL_RPAX58:
			case MODEL_RTAX56U:
			case MODEL_EBA63:
				snprintf(macaddr_str, sizeof(macaddr_str), "sb/%d/macaddr", unit);
				break;
			case MODEL_RPBE58:
			case MODEL_RTBE58_GO:
			case MODEL_RTBE58U:
			case MODEL_RTBE58U_V2:
			case MODEL_RTBE82U:
			case MODEL_RTBE82M:
			case MODEL_RTBE58U_PRO:
				snprintf(macaddr_str, sizeof(macaddr_str), "sb/%d/macaddr", 1 - unit);
				break;
			case MODEL_RTBE92U:
				if (unit)	/* 5G / 6G */
					snprintf(macaddr_str, sizeof(macaddr_str), "sb/%d/macaddr", 2 - unit);
				else		/* 2.4G */
					snprintf(macaddr_str, sizeof(macaddr_str), "1:macaddr");
				break;
			case MODEL_RTBE95U:
				if (!unit)	/* 5G */
					snprintf(macaddr_str, sizeof(macaddr_str), "1:macaddr");
				else		/* 2.4G / 6G */
					snprintf(macaddr_str, sizeof(macaddr_str), "sb/%d/macaddr", unit - 1);
				break;
			case MODEL_RTAX88U:
			case MODEL_GTAX11000:
			case MODEL_RTAX92U:
			case MODEL_GTAXE11000:
			case MODEL_GTAX6000:
			case MODEL_ET12:
			case MODEL_XT12:
			case MODEL_RTAX88U_PRO:
					 snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", unit + 1);
				break;
#if defined(GTAXE16000) || defined(GTAX11000_PRO) || defined(GT10) || defined(RTAX9000) || defined(GTBE98) || defined(RTBE96U) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE88U) || defined(BQ16) || defined(BQ16_PRO) || defined(BT10) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
			case MODEL_GTAX11000_PRO:
			case MODEL_GTAXE16000:
			case MODEL_GTBE98:
			case MODEL_GTBE98_PRO:
			case MODEL_GT10:
			case MODEL_RTBE96U:
			case MODEL_RTAX9000:
			case MODEL_GTBE96:
			case MODEL_RTBE88U:
			case MODEL_BT10:
			case MODEL_BQ16:
			case MODEL_BQ16_PRO:
			case MODEL_GTBE19000:
			case MODEL_GTBE19000AI:
			case MODEL_GTBE96_AI:
					snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", devpath_idx[unit]);
				break;
#endif
			case MODEL_RTAC1200G:
			case MODEL_RTAC1200GP:
				if (unit == 0) 	/* 2.4G */
					snprintf(macaddr_str, sizeof(macaddr_str), "0:macaddr");
				else		/* 5G */
					snprintf(macaddr_str, sizeof(macaddr_str), "sb/1/macaddr");
				break;
			case MODEL_GSBE18000:
			case MODEL_GT7:
				snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", 2 - unit);
				break;
			default:
#ifdef RTCONFIG_BCMARM
				snprintf(macaddr_str, sizeof(macaddr_str), "%d:macaddr", unit);
#else
				snprintf(macaddr_str, sizeof(macaddr_str), "pci/%d/1/macaddr", unit + 1);
#endif
				break;
		}

		macaddr_strp = cfe_nvram_get(macaddr_str);

		if (macaddr_strp)
		{
			if (!mssid_mac_validate(macaddr_strp))
				return;

			if (idx != unit)
				continue;

			ether_atoe(macaddr_strp, mac_binary);
			sprintf(macbuf, "%02X%02X%02X%02X%02X%02X",
					mac_binary[0],
					mac_binary[1],
					mac_binary[2],
					mac_binary[3],
					mac_binary[4],
					mac_binary[5]);
			macvalue = strtoll(macbuf, (char **) NULL, 16);

#ifndef RTCONFIG_MSSID_REALMAC
			/* virtual mac address generation */
			vif_mac0 = gen_mssid_hwaddr_mac0((unsigned char *) &mac_binary, unit);
#else
			ETHER_SET_LOCALADDR(mac_binary);
#endif
			sprintf(macbuf, "%02X%02X%02X%02X%02X%02X",
					mac_binary[0],
					mac_binary[1],
					mac_binary[2],
					mac_binary[3],
					mac_binary[4],
					mac_binary[5]);
			macvalue_local = strtoll(macbuf, (char **) NULL, 16);

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
#if defined(RPBE58) || defined(RTBE58_GO)
			_dprintf("%s(%d), skip append 2g psr macval.\n", __func__, unit);
#else

			if ((unit == WL_2G_BAND) && psr) {
				/* br0 and 2.4G MAC should be different */
				macvalue++;
				macvalue_local++;
			}
#endif

#if (defined(RTAX82_XD6) || defined(RTAX82_XD6S)) && defined(RTCONFIG_MSSID_REALMAC)
			if (unit == 1) {
				macvalue += 4;
				macvalue_local += 4;

				memset(macaddr_str, 0, sizeof(macaddr_str));
				macp = (unsigned char*) &macvalue;
				sprintf(macaddr_str, "%02X:%02X:%02X:%02X:%02X:%02X", *(macp+5), *(macp+4), *(macp+3), *(macp+2), *(macp+1), *(macp+0));
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);
				nvram_set(strcat_r(prefix, "hwaddr", tmp), macaddr_str);
				nvram_set("1:macaddr", macaddr_str);
			}
#endif

#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_MULTILAN_CFG)
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			if (is_psr(unit) && nvram_match(strcat_r(prefix, "psr_mbss", tmp), "1") && dpsta_mode()) {
				max_mssid++;
				macvalue--;
				macvalue_local--;
			}
#endif
#endif

			/* including primary ssid */
#ifndef RTCONFIG_MSSID_REALMAC
			for (subunit = 1; subunit < max_wl_cap_mbss /* WL_MAXBSSCFG */; subunit++)
#else
			for (subunit = 1; subunit < max_no_vifs /* WL_MAXBSSCFG */; subunit++)
#endif
			{
				macvalue++;
				macvalue_local++;

#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_MULTILAN_CFG)
				snprintf(prefix, sizeof(prefix), "wl%d_", unit);
#endif
				if ((subunit > max_mssid)
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
					|| (is_psr(unit) &&
#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_MULTILAN_CFG)
						!nvram_match(strcat_r(prefix, "psr_mbss", tmp), "1") &&
#endif
						(subunit > 1))
#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_MULTILAN_CFG)
					|| ((unit == WL_2G_BAND) && is_psr(unit) &&
						nvram_match(strcat_r(prefix, "psr_mbss", tmp), "1") &&
						subunit == 4)
#endif
#endif
				)
					macp = (unsigned char*) &macvalue_local;
				else
					macp = (unsigned char*) &macvalue;

#ifndef RTCONFIG_MSSID_REALMAC
				/* global virtual MAC rule */
				*(macp+5) = vif_mac0;
				if (max_wl_cap_mbss == 8){
					*(macp+0) = (mac_binary[5] & 0xF8) + ((mac_binary[5] + subunit) & 0x07);
				} else{ /* 16 */
					*(macp+0) = (mac_binary[5] & 0xF0) + ((mac_binary[5] + subunit) & 0x0F);
				}

				if (psr && subunit == 1) {
					if (dpsta_mode()){
						/* wlx and wlx.1 use same MAC */
						*(macp+5) = mac_binary[0];
						*(macp+0) = mac_binary[5];
#if !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
					} else{
						/* virtual address for non-dpsta mode */
						*(macp+5) = vif_mac0;
						*(macp+0) = mac_binary[5];
#endif
					}
				}
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA) && !defined(RPBE58) && !defined(RTBE58_GO)
				if (psr && unit == WL_2G_BAND) {
					/* all 2.4G MAC increase 1, because br0 MAC and 2.4G primary MAC should be different */
					if (subunit == 1){
						/* wlx and wlx.1 use same MAC */
						if (dpsta_mode()){
							*(macp+5) = mac_binary[0];
							*(macp+0) = mac_binary[5];
						} else{
							/* 2.4G wlx is br0 MAC + 1 */
							/* virtual address for non-dpsta mode */
							*(macp+5) = vif_mac0;
							if (max_wl_cap_mbss == 8){
								*(macp+0) = (mac_binary[5] & 0xF8) + ((mac_binary[5] + subunit + 1) & 0x07);
							} else{ /* 16 */
								*(macp+0) = (mac_binary[5] & 0xF0) + ((mac_binary[5] + subunit + 1) & 0x0F);
							}
						}
					} else if (subunit > 1) {
						/* all 2.4G MAC increase 1, including virtual MAC address */
						*(macp+5) = vif_mac0;
						if (max_wl_cap_mbss == 8){
							*(macp+0) = (mac_binary[5] & 0xF8) + ((mac_binary[5] + subunit + 1) & 0x07);
						} else{
							*(macp+0) = (mac_binary[5] & 0xF0) + ((mac_binary[5] + subunit + 1) & 0x0F);
						}
#if !defined(RTCONFIG_MULTILAN_CFG)
						if (subunit == max_wl_cap_mbss - 1){
							/* There is only max_wl_cap_mbss MAC addresses for 2.4G, wl0.< max_wl_cap_mbss -1> should be 00:00:00:00:00:00 */
							*(macp+5) =  0x0;
							*(macp+4) =  0x0;
							*(macp+3) =  0x0;
							*(macp+2) =  0x0;
							*(macp+1) =  0x0;
							*(macp+0) =  0x0;
						}
#endif
					}
#if (defined(RTAX82_XD6) || defined(RTAX82_XD6S)) && !defined(RTCONFIG_MSSID_REALMAC)
				} else if(unit == 1){
					*(macp+0) = *(macp+0) + 4;
#endif
				}
#endif
#endif
				memset(macaddr_str, 0, sizeof(macaddr_str));
				sprintf(macaddr_str, "%02X:%02X:%02X:%02X:%02X:%02X", *(macp+5), *(macp+4), *(macp+3), *(macp+2), *(macp+1), *(macp+0));
				snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);

				nvram_set(strcat_r(prefix, "hwaddr", tmp), macaddr_str);
			}
		} else return;
	}
}

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
void
reset_psr_hwaddr()
{
	char macaddr_name[32], macaddr_str[18], macbuf[13];
	char *macaddr_p;
	unsigned char mac_binary[6];
	unsigned long long macvalue;
	unsigned char *macp;
	int model = get_model();
	int unit = 0;
	int restore = !(is_psr(WL_2G_BAND)
			|| dpsr_mode()
#ifdef RTCONFIG_DPSTA
			|| dpsta_mode()
#endif
#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58)
			|| nvram_match("x_Setting", "0") || rp_mode()
#endif
			);

#if defined(RTCONFIG_BCM_MFG) && (defined(RPAX56) || defined(RPAX58))
	return;
#elif defined(RPBE58) || defined(RTBE58_GO)
	_dprintf("%s: skip\n", __func__);
	return;
#endif
	memset(mac_binary, 0x0, 6);
	memset(macbuf, 0x0, 13);

	switch(model) {
		case MODEL_RTAC3200:
		case MODEL_GTAC5300:
		case MODEL_GTAX11000:
		case MODEL_RTAX88U:
		case MODEL_RTAX92U:
		case MODEL_GTAXE11000:
		case MODEL_GTAX6000:
		case MODEL_GTAX11000_PRO:
		case MODEL_GTAXE16000:
		case MODEL_ET12:
		case MODEL_XT12:
		case MODEL_RTAX88U_PRO:
		case MODEL_RTBE92U:
		case MODEL_RTBE95U:
			unit = 1;
			break;

		case MODEL_RTAX86U_PRO:
			unit = 0;
			break;

		case MODEL_GTBE98:
		case MODEL_GTBE98_PRO:
		case MODEL_RTBE96U:
		case MODEL_GTBE96:
		case MODEL_RTBE88U:
		case MODEL_GTBE19000:
		case MODEL_GTBE19000AI:
		case MODEL_GTBE96_AI:
		
			unit = WL_2G_BAND;
			break;

		case MODEL_BT10:
			unit = WL_2G_BAND;
			break;
		case MODEL_BQ16:
		case MODEL_BQ16_PRO:
			unit = 3;	/* 2.4G */
			break;
	}

	if (model == MODEL_RTAX56U || model == MODEL_RPAX56 || model == MODEL_RPAX58 || model == MODEL_RPBE58 || model == MODEL_RTBE58_GO || model == MODEL_RTAX55 || model == MODEL_TUFAX3000_V2 || model == MODEL_RTAX3000N || model == MODEL_BR63 || model == MODEL_EBA63 || model == MODEL_RTBE95U)
		snprintf(macaddr_name, sizeof(macaddr_name), "sb/%d/macaddr", unit);
	else if (model == MODEL_RTBE58U || model == MODEL_RTBE58U_V2 || model == MODEL_RTBE82U || model == MODEL_RTBE82M || model == MODEL_RTBE58U_PRO)
		snprintf(macaddr_name, sizeof(macaddr_name), "sb/%d/macaddr", 1 - unit);
	else
		snprintf(macaddr_name, sizeof(macaddr_name), "%d:macaddr", unit);

	macaddr_p = cfe_nvram_get(macaddr_name);
	if (macaddr_p)
	{
		ether_atoe(macaddr_p, mac_binary);
		sprintf(macbuf, "%02X%02X%02X%02X%02X%02X",
				mac_binary[0],
				mac_binary[1],
				mac_binary[2],
				mac_binary[3],
				mac_binary[4],
				mac_binary[5]);
		macvalue = strtoll(macbuf, (char **) NULL, 16);
		if (!restore) macvalue++;

		macp = (unsigned char*) &macvalue;
		memset(macaddr_str, 0, sizeof(macaddr_str));
		sprintf(macaddr_str, "%02X:%02X:%02X:%02X:%02X:%02X", *(macp+5), *(macp+4), *(macp+3), *(macp+2), *(macp+1), *(macp+0));
		nvram_set(macaddr_name, macaddr_str);
	}
}
#endif

#ifdef RTCONFIG_DHDAP
void load_wl()
{
	char module[80], modules[80], *next;
	int i = 0, maxunit = -1;
	int unit;
	char ifname[16] = {0};
	char instance_base[256];
	char instance_base2[256];
#if defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
	char dconpoll_str[20] = "";
#endif
#if defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(RTBE88U) || defined(RTBE96U) || defined(GTBE96) || defined(GTBE98) || defined(GTBE98_PRO) || defined(RTBE86U) || defined(GTBE19000) || defined(GTBE19000AI) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	char dhd_iface_name_str[60] = "iface_name=wl";
#else
	char dhd_iface_name_str[20] = "";
#endif

#if ((defined(HND_ROUTER) && !defined(RTCONFIG_HND_ROUTER_AX)) || defined(RTCONFIG_BCM_7114)) && !defined(RTCONFIG_MFGFW)
	char word[100], tmp[100], prefix[] = "wlXXXXXXXXXX_";
	int dhd_chk_cnt = nvram_get_int("dhd_chk_cnt");
	int org, cur, count;

	if (dhd_chk_cnt < 5) {
		nvram_set_int("dhd_chk_cnt", ++dhd_chk_cnt);

		if (nvram_get_int("dhd_chk_cnt") == 5) {
			dbg("workaround broken bcm4366...\n");
			unit = 0;
			foreach (word, nvram_safe_get("wl_ifnames"), next) {
				snprintf(prefix, sizeof(prefix), "wl%d_", unit++);
				nvram_set_int(strcat_r(prefix, "failed", tmp), 3);
			}
		}

		nvram_commit();
	}

	dbg("dhd_chk_cnt: %d\n", nvram_get_int("dhd_chk_cnt"));

	unit = 0;
	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		cur = nvram_get_int(strcat_r(prefix, "failed", tmp));
		org = nvram_get_int(strcat_r(prefix, "failed_org", tmp));

		nvram_set_int(strcat_r(prefix, "failed_org", tmp), cur);
		nvram_set_int(strcat_r(prefix, "ok", tmp), 0);

		if ((cur > org) || (dhd_chk_cnt == 5)) {
			count = (dhd_chk_cnt == 5) ? 5 : (cur - org);
			wl_fail_db(unit, 2, count);

			nvram_commit();
		}

		unit++;
	}
#endif

#if defined(RTAC88U) || defined(RTAC3100)
	int chk_reboot = 0;

	if(!*nvram_safe_get("chiprev"))
		chk_reboot = 1;
#endif

#if defined(RTCONFIG_HND_ROUTER_BE_4916)
	if(atoi(nvram_safe_get("dconpoll")) > 0)
		snprintf(dconpoll_str, sizeof(dconpoll_str), "dhd_console_ms=%s", nvram_safe_get("dconpoll"));
	else
		snprintf(dconpoll_str, sizeof(dconpoll_str), "dhd_console_ms=0");
#endif

_dprintf("load_wl(): starting...\n");
#if (defined(RTAX82_XD6) || defined(RTAX82_XD6S)) && defined(RTCONFIG_MSSID_REALMAC)
	reset_mssid_hwaddr(1);
#endif
	memset(modules, 0, sizeof(modules));
#if defined(RTAX92U) || defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || (defined(RTCONFIG_HND_ROUTER_BE_4916) && !defined(BCM6813)) || defined(RTCONFIG_HND_ROUTER_AX_6710) && !defined(BCM4912) || defined(RTBE86U) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO)
	add_to_list("wl", modules, sizeof(modules));
#endif
#ifdef RTCONFIG_DHDAP
#if defined(RTCONFIG_BCM_7114) && defined(RTCONFIG_MFGFW)
	add_to_list("dhdtest", modules, sizeof(modules));
#else
	add_to_list("dhd", modules, sizeof(modules));
#endif
#ifdef RTCONFIG_BCM_7114
	add_to_list("dhd24", modules, sizeof(modules));
#endif
#endif
#ifdef RTCONFIG_BRCM_HOSTAPD
	{
#if !defined(RTCONFIG_HND_ROUTER_AX_6756) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
		struct utsname name;
		uname(&name);
		char tmp[100];
		snprintf(tmp, sizeof(tmp), "insmod /lib/modules/%s/kernel/net/wireless/cfg80211.ko", name.release);
		/* Load cfg80211 module first. dhd.ko and wl.ko may be dependent on this */
		system(tmp);
#endif
	}
#endif
#ifdef RTBE96U
	nvram_set("3:avsmargins", "680,300"); // 2.4G
	nvram_set("4:avsmargins", "680,300"); // 5G
#endif
#if defined(BT10) || defined(GT10) || defined(RTAX9000) || defined(RTBE95U) || defined(RTBE86U) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	snprintf(modules, sizeof(modules), "dhd wl");
#endif
	foreach(module, modules, next) {
#ifdef RTCONFIG_BCM_7114
		if (strcmp(module, "dhd") == 0 && nvram_get_int("dhd24"))
			continue;
		else if (strcmp(module, "dhd24") == 0 && nvram_get_int("dhd24"))
			eval("rmmod", "dhd");
#endif
		if (strcmp(module, "dhd") == 0 || strcmp(module, "dhd24") == 0 || strcmp(module, "wl") == 0) {
#if defined(RTAX56_XD4) || defined(XD4PRO) || defined(CTAX56_XD4) || defined(XC5) || defined(EBA63)
			if(strcmp(module, "wl") == 0){
				eval("insmod", "wl", "intf_name=wl%d instance_base=0");
				continue;
			}
#elif defined(BT10)
			if(strcmp(module, "wl") == 0){
				eval("insmod", "wl", "intf_name=wl%d instance_base=1");
				continue;
			}
#endif

			/* Search for existing wl devices and the max unit number used */
#ifdef RTCONFIG_HND_ROUTER_BE_4916
			for (i = 0; i <= DEV_NUMIFS; i++) {
#else
			for (i = 1; i <= DEV_NUMIFS; i++) {
#endif
				snprintf(ifname, sizeof(ifname), WL_IF_PREFIX, i);
				if (!wl_probe(ifname)) {
					unit = -1;
					if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit))) {
						maxunit = (unit > maxunit) ? unit : maxunit;
					}
				}
			}
			memset(instance_base, 0, sizeof(instance_base));
			memset(instance_base2, 0, sizeof(instance_base2));
#if defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
			if ((strcmp(module, "dhd") == 0) || (strcmp(module, "dhd24") == 0))
#if defined(RTCONFIG_BCM_MFG) && (defined(GT10) || defined(RTAX82U_V2) || defined(TUFAX5400_V2) || defined(XD6_V2) || defined(RTAX5400))
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d dhd_msg_level=%d firmware_path=/rom/dhd/6715b0_mfg/rtecdc.bin", maxunit + 1, nvram_get_int("dhd_msg_level"));
#elif defined(RTAXE7800)
#ifdef RTCONFIG_BCM_MFG
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d dhd_msg_level=%d firmware_path=/rom/dhd/6715b0_mfg/rtecdc.bin", 1, nvram_get_int("dhd_msg_level"));
#else
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d dhd_msg_level=%d", 1, nvram_get_int("dhd_msg_level"));
#endif
#elif defined(RTBE86U)
				snprintf(instance_base, sizeof(instance_base), "%s instance_base=%d dhd_msg_level=%d %s", dhd_iface_name_str, 1, nvram_get_int("dhd_msg_level"), dconpoll_str);
#else
				snprintf(instance_base, sizeof(instance_base), "%s instance_base=%d dhd_msg_level=%d %s", dhd_iface_name_str, maxunit + 1, nvram_get_int("dhd_msg_level"), dconpoll_str);
#endif
			else
#endif
			{
				if (strtoul(nvram_safe_get("wl_msglevel"), NULL, 0))
					snprintf(instance_base, sizeof(instance_base), "msglevel=%d", (int)strtoul(nvram_safe_get("wl_msglevel"), NULL, 0));
				if (strtoul(nvram_safe_get("wl_msglevel2"), NULL, 0))
					snprintf(instance_base2, sizeof(instance_base2), "%s msglevel2=%d", instance_base, (int)strtoul(nvram_safe_get("wl_msglevel2"), NULL, 0));
				else
					strncpy(instance_base2, instance_base, sizeof(instance_base2));
#if defined(RTCONFIG_HND_ROUTER_BE_4916)
#if defined(RTBE86U)
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d %s intf_name=wl%%d", 0, instance_base2);
#else
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d %s intf_name=wl%%d", maxunit + 1, instance_base2);
#endif
#else
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d %s", maxunit + 1, instance_base2);
#endif
			}
_dprintf("load_wl(): insmod %s %s.\n", module, instance_base);
			eval("insmod", module, instance_base);
		} else {
			eval("insmod", module);
			 _dprintf("\nmodule %s loaded\n", module); 
		}
	}

#if defined(HND_ROUTER) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM4708)
	wl_driver_mode_update();
#endif

#ifdef WLCLMLOAD
	download_clmblob_files();
#endif /* WLCLMLOAD */

#if ((defined(HND_ROUTER) && !defined(RTCONFIG_HND_ROUTER_AX)) || defined(RTCONFIG_BCM_7114)) && !defined(RTCONFIG_MFGFW)
	nvram_set_int("dhd_chk_cnt", 0);
	nvram_commit();

	unit = 0;
	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		if (nvram_get_int(strcat_r(prefix, "ok", tmp)))
			wl_fail_db(unit, 1, 1);

		unit++;
	}
#endif

#if defined(RTAC88U) || defined(RTAC3100)
	int n = nvram_get_int("tryc")?:5;

	if(chk_reboot) {
		for(i=0; i<n; ++i) {
			if(nvram_get_hex("chiprev")>0 && chiprev_patch(nvram_safe_get("chiprev"))) {
				_dprintf("\n>>> reboot due chiprev\n");
				reboot(RB_AUTOBOOT);
			}
			sleep(1);
		}
	}
#endif
_dprintf("load_wl(): end.\n");

#if defined(RTBE96U) || defined(GTBE96) || defined(GTBE98) || defined(GTBE98_PRO)
	wl_ifname_align_war();
#endif
}
#else
void load_wl()
{
#ifdef RTCONFIG_HND_ROUTER_BE_4916
	int wlx = 1;
#else
	int wlx = 0;
#endif
	char instance_base[64];
	char instance_base2[64];

	if (nvram_match("pd_low", "2")) {
		_dprintf("%s, not load_wl due pd_low(2)\n", __func__);
		return;
	}

	memset(instance_base, 0, sizeof(instance_base));
	memset(instance_base2, 0, sizeof(instance_base2));

	if (strtoul(nvram_safe_get("wl_msglevel"), NULL, 0))
		snprintf(instance_base, sizeof(instance_base), "msglevel=%d", (int)strtoul(nvram_safe_get("wl_msglevel"), NULL, 0));
	if (strtoul(nvram_safe_get("wl_msglevel2"), NULL, 0))
		snprintf(instance_base2, sizeof(instance_base2), "%s msglevel2=%d", instance_base, (int)strtoul(nvram_safe_get("wl_msglevel2"), NULL, 0));
	else
		strncpy(instance_base2, instance_base, sizeof(instance_base2));

	eval("insmod", "wl", instance_base2, wlx ? "intf_name=wl%d" : NULL);
}
#endif

#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
void wlaffinity(void);
#endif

void init_wl(void)
{
#ifdef RTCONFIG_BCMWL6
	switch(get_model()) {
		case MODEL_DSLAC68U:
		case MODEL_RTAC1200G:
		case MODEL_RTAC1200GP:
		case MODEL_RTAC3100:
		case MODEL_RTAC3200:
		case MODEL_RTAC5300:
		case MODEL_GTAC5300:
		case MODEL_RTAC66U:
		case MODEL_RTAC68U:
		case MODEL_RTAC88U:
		case MODEL_RTAC86U:
		case MODEL_RTAX92U:
#ifndef RTAX95Q
		case MODEL_RTAX95Q:
#endif
#ifndef RTAXE95Q
		case MODEL_RTAXE95Q:
#endif
			set_bcm4360ac_vars();
			break;
	}
#endif
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
	system("insmod wlshared.ko");
	system("insmod firmware_class.ko");
	system("insmod cfg80211.ko");
	system("insmod hnd");
	system("insmod emf");
	system("insmod igs");
#endif
	check_wl_country();
#if defined(RTAC3200) || defined(RTAC68U) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER) || defined(DSL_AC68U)
	wl_disband5grp();
#endif
#if defined(BQ16) || defined(RTBE88U)
	wl_update_eu_edthresh();
#endif
#ifdef HND_ROUTER
	wl_dis_illegal_160m();
#endif
#if !defined(RTCONFIG_HND_ROUTER_AX)
	set_wltxpower();
#endif
#ifdef RTCONFIG_BRCM_HOSTAPD
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
#else
	eval("insmod", "cfg80211");
#endif
#endif
#ifdef RTCONFIG_DPSTA
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
#else
	eval("insmod", "dpsta");
#endif
#endif
	load_wl();
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(BCA_HNDROUTER) && (defined(RTCONFIG_HND_WL) || defined(BCM4912) || defined(BCM6813) || defined(BCM6765))
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
	wlaffinity();
#else
	wl_thread_affinity_update();
#endif
#endif
#if defined(RTCONFIG_HND_ROUTER_AX) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	set_wltxpower();
#endif
#if !defined(RTCONFIG_BCMARM) && defined(NAS_GTK_PER_STA) && defined(PROXYARP)
	eval("insmod", "proxyarp");
#endif

#ifdef RTCONFIG_BRCM_USBAP
	/* We have to load USB modules after loading PCI wl driver so
	 * USB driver can decide its instance number based on PCI wl
	 * instance numbers (in hotplug_usb())
	 */
	eval("insmod", "usbcore");

#ifdef LINUX26
	mount("usbfs", "/proc/bus/usb", "usbfs", MS_MGC_VAL, NULL);
#else
	mount("usbdevfs", "/proc/bus/usb", "usbdevfs", MS_MGC_VAL, NULL);
#endif /* LINUX26 */

	{
		char	insmod_arg[128];
		int	i = 0, maxwl_eth = 0, maxunit = -1;
		char	ifname[16] = {0};
		int	unit = -1;
		char arg1[20] = {0};
		char arg2[20] = {0};
		char arg3[20] = {0};
		char arg4[20] = {0};
		char arg5[20] = {0};
		char arg6[20] = {0};
		char arg7[20] = {0};
		const int wl_wait = 3;	/* max wait time for wl_high to up */

		/* Save QTD cache params in nvram */
		sprintf(arg1, "log2_irq_thresh=%d", nvram_get_int("ehciirqt"));
		sprintf(arg2, "qtdc_pid=%d", nvram_get_int("qtdc_pid"));
		sprintf(arg3, "qtdc_vid=%d", nvram_get_int("qtdc_vid"));
		sprintf(arg4, "qtdc0_ep=%d", nvram_get_int("qtdc0_ep"));
		sprintf(arg5, "qtdc0_sz=%d", nvram_get_int("qtdc0_sz"));
		sprintf(arg6, "qtdc1_ep=%d", nvram_get_int("qtdc1_ep"));
		sprintf(arg7, "qtdc1_sz=%d", nvram_get_int("qtdc1_sz"));

		eval("insmod", "ehci-hcd", arg1, arg2, arg3, arg4, arg5,
				arg6, arg7);

		/* Search for existing PCI wl devices and the max unit number used.
		 * Note that PCI driver has to be loaded before USB hotplug event.
		 * This is enforced in rc.c
		 */
		#define DEV_NUMIFS 8
		for (i = 1; i <= DEV_NUMIFS; i++) {
			sprintf(ifname, WL_IF_PREFIX, i);
			if (!wl_probe(ifname)) {
				if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
					sizeof(unit))) {
					maxwl_eth = i;
					maxunit = (unit > maxunit) ? unit : maxunit;
				}
			}
		}

		/* Set instance base (starting unit number) for USB device */
		sprintf(insmod_arg, "instance_base=%d", maxunit + 1);
		eval("insmod", "wl_high", insmod_arg);

		/* Hold until the USB/HSIC interface is up (up to wl_wait sec) */
		sprintf(ifname, WL_IF_PREFIX, maxwl_eth + 1);
		i = wl_wait;
		while (wl_probe(ifname) && i--) {
			sleep(1);
		}
		if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit)))
			cprintf("wl%d is up in %d sec\n", unit, wl_wait - i);
		else
			cprintf("wl%d not up in %d sec\n", unit, wl_wait);
	}
#endif /* __CONFIG_USBAP__ */
}

void init_wl_compact(void)
{
	int model = get_model();

	/* no wireless driver re-loading to keep CAC information when restarting wireless */
#if defined(RTCONFIG_BCMWL6) && !defined(RTN66U)
	if (num_of_wl_if() > 1)
		return;
#endif

	if (nvram_get_int("init_wl_re") == 0)
	{
		nvram_set_int("init_wl_re", 1);
		return;
	}

#ifdef RTCONFIG_BCMWL6
	switch(model) {
		case MODEL_DSLAC68U:
		case MODEL_RTAC1200G:
		case MODEL_RTAC1200GP:
		case MODEL_RTAC3100:
		case MODEL_RTAC3200:
		case MODEL_RTAC5300:
		case MODEL_GTAC5300:
		case MODEL_RTAC66U:
		case MODEL_RTAC68U:
		case MODEL_RTAC88U:
		case MODEL_RTAC86U:
		case MODEL_RTAX92U:
#ifndef RTAX95Q
		case MODEL_RTAX95Q:
#endif
#ifndef RTAXE95Q
		case MODEL_RTAXE95Q:
#endif
			set_bcm4360ac_vars();
			break;
	}
#endif
	check_wl_country();
#ifndef RTCONFIG_BRCM_USBAP
#if defined(RTAC3200) || defined(RTAC68U) || defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER) || defined(DSL_AC68U)
	wl_disband5grp();
#endif
#ifdef HND_ROUTER
	wl_dis_illegal_160m();
#endif
#if !defined(RTCONFIG_HND_ROUTER_AX)
	set_wltxpower();
#endif
	load_wl();
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(BCA_HNDROUTER) && (defined(RTCONFIG_HND_WL) || defined(BCM4912) || defined(BCM6813))
#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
	wlaffinity();
#else
	wl_thread_affinity_update();
#endif
#endif
#if defined(RTCONFIG_HND_ROUTER_AX) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
	set_wltxpower();
#endif
#ifndef RTCONFIG_BCMARM
#if defined(NAS_GTK_PER_STA) && defined(PROXYARP)
	eval("insmod", "proxyarp");
#endif
#endif
#endif
}

void unload_wl(void)
{
#ifndef RTCONFIG_BCMARM
#if defined(NAS_GTK_PER_STA) && defined(PROXYARP)
	system("rmmod proxyarp");
#endif
#endif

#ifndef RTCONFIG_BRCM_USBAP
	if (!(
#ifdef RTCONFIG_DPSTA
		dpsta_mode() ||
#endif
		rp_mode()
	))
	system("rmmod wl");
#endif
#ifdef RTCONFIG_DHDAP
	system("rmmod dhd");
#endif
}

void fini_wl(void)
{
	/* no wireless driver re-loading to keep CAC information when restarting wireless */
#if defined(RTCONFIG_BCMWL6) && !defined(RTN66U)
	if (num_of_wl_if() > 1)
		return;
#endif

#ifndef RTCONFIG_BCMARM
#if defined(NAS_GTK_PER_STA) && defined(PROXYARP)
	eval("rmmod", "proxyarp");
#endif
#endif

#ifndef RTCONFIG_BRCM_USBAP
        if (!(
#ifdef RTCONFIG_DPSTA
		dpsta_mode() ||
#endif
		rp_mode()
        ))
	eval("rmmod", "wl");
#endif
#ifdef RTCONFIG_DHDAP
	eval("rmmod", "dhd");
#endif
#ifdef RTCONFIG_BRCM_HOSTAPD
	eval("rmmod", "cfg80211");
#endif
}

#if defined(RTAX86U)
void fill_cfe_block();
#endif
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) \
		|| defined(RTAX86U)
void sync_cfe_mac();
#endif

void init_syspara(void)
{
	char *ptr;
	int model;

	nvram_set("firmver", rt_version);
	nvram_set("productid", rt_buildname);
	set_basic_fw_name();
	ptr = nvram_get("regulation_domain");

#ifdef HND_ROUTER
	refresh_cfe_nvram();
#endif

#ifdef RTAC86U
	hnd_cfe_check();
#endif

#ifdef RTCONFIG_ASUSCTRL
	asus_ctrl_sku_check();
#endif

	model = get_model();
	switch(model) {
		case MODEL_RTN53:
		case MODEL_RTN16:
		case MODEL_RTN15U:
		case MODEL_RTN12:
		case MODEL_RTN12B1:
		case MODEL_RTN12C1:
		case MODEL_RTN12D1:
		case MODEL_RTN12VP:
		case MODEL_RTN12HP:
		case MODEL_RTN12HP_B1:
		case MODEL_APN12HP:
		case MODEL_RTN14UHP:
		case MODEL_RTN10U:
		case MODEL_RTN10P:
		case MODEL_RTN10D1:
		case MODEL_RTN10PV2:
			if (!nvram_get("et0macaddr"))	// eth0, eth1
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			if (!nvram_get("0:macaddr"))	// eth2(5G)
				nvram_set("0:macaddr", "00:22:15:A5:03:04");
			if (nvram_get("regulation_domain_5G")) {// by ate command from asuswrt, prior than ui 2.0
				nvram_set("wl1_country_code", nvram_get("regulation_domain_5G"));
				nvram_set("0:ccode", nvram_get("regulation_domain_5G"));
			} else if (nvram_get("regulation_domain_5g")) {	// by ate command from ui 2.0
				nvram_set("wl1_country_code", nvram_get("regulation_domain_5g"));
				nvram_set("0:ccode", nvram_get("regulation_domain_5g"));
			}
			else {
				nvram_set("wl1_country_code", "US");
				nvram_set("0:ccode", "US");
			}
			nvram_set("sb/1/macaddr", nvram_safe_get("et0macaddr"));
			if (ptr && *ptr) {
				if ((strlen(ptr) == 6) && /* legacy format */
					!strncasecmp(ptr, "0x", 2))
				{
					nvram_set("wl0_country_code", ptr+4);
					nvram_set("sb/1/ccode", ptr+4);
				}
				else
				{
					nvram_set("wl0_country_code", ptr);
					nvram_set("sb/1/ccode", ptr);
				}
			} else {
				nvram_set("wl0_country_code", "US");
				nvram_set("sb/1/ccode", "US");
			}
			break;

		case MODEL_RTN66U:
		case MODEL_RTAC66U:
			if (!nvram_get("et0macaddr"))		// eth0, eth1
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			if (!nvram_get("pci/2/1/macaddr"))	// eth2(5G)
				nvram_set("pci/2/1/macaddr", "00:22:15:A5:03:04");
			if (nvram_get("regulation_domain_5G")) {
				nvram_set("wl1_country_code", nvram_get("regulation_domain_5G"));
				nvram_set("pci/2/1/ccode", nvram_get("regulation_domain_5G"));
			} else {
				nvram_set("wl1_country_code", "US");
				nvram_set("pci/2/1/ccode", "US");
			}
			nvram_set("pci/1/1/macaddr", nvram_safe_get("et0macaddr"));
			if (ptr) {
				nvram_set("wl0_country_code", ptr);
				nvram_set("pci/1/1/ccode", ptr);
			} else {
				nvram_set("wl0_country_code", "US");
				nvram_set("pci/1/1/ccode", "US");
			}
			break;

		case MODEL_RTN18U:
			if (!nvram_get("et0macaddr"))	//eth0, eth1
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			nvram_set("0:macaddr", nvram_safe_get("et0macaddr"));

			if (nvram_match("0:ccode", "0")) {
				nvram_set("0:ccode","US");
			}
			break;

		case MODEL_RTAC87U:
			if (!nvram_get("et1macaddr"))	//eth0, eth1
				nvram_set("et1macaddr", "00:22:15:A5:03:00");
			nvram_set("0:macaddr", nvram_safe_get("et1macaddr"));
			nvram_set("LANMACADDR", nvram_safe_get("et1macaddr"));
			break;

		case MODEL_DSLAC68U:
		case MODEL_RTAC68U:
		case MODEL_RTAC56S:
		case MODEL_RTAC56U:
			if (!nvram_get("et0macaddr"))	//eth0, eth1
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			if (!nvram_get("1:macaddr"))	//eth2(5G)
				nvram_set("1:macaddr", "00:22:15:A5:03:04");
			nvram_set("0:macaddr", nvram_safe_get("et0macaddr"));
			break;

		case MODEL_RTAC5300:
		case MODEL_RTAC88U:
			if (!nvram_get("lan_hwaddr"))
				nvram_set("lan_hwaddr", cfe_nvram_safe_get("et1macaddr"));

			break;

		case MODEL_RTAC3100:
		case MODEL_GTAC5300:
		case MODEL_RTAC86U:
		case MODEL_RTAX88U:
		case MODEL_BC109:
		case MODEL_BC105:
		case MODEL_EBG19:
		case MODEL_EBG15:
		case MODEL_EBP15:
		case MODEL_GTAX11000:
		case MODEL_RTAX92U:
		case MODEL_RTAX95Q:
		case MODEL_XT8PRO:
		case MODEL_BT12:
		case MODEL_BT10:
		case MODEL_BQ16:
		case MODEL_BQ16_PRO:
		case MODEL_BM68:
		case MODEL_XT8_V2:
		case MODEL_RTAXE95Q:
		case MODEL_ET8PRO:
		case MODEL_ET8_V2:
		case MODEL_RTAX56_XD4:
		case MODEL_XD4PRO:
		case MODEL_XC5:
		case MODEL_CTAX56_XD4:
		case MODEL_EBA63:
		case MODEL_RTAX58U:
		case MODEL_RTAX82U_V2:
		case MODEL_TUFAX5400_V2:
		case MODEL_RTAX5400:
		case MODEL_RTAX82_XD6S:
		case MODEL_XD6_V2:
		case MODEL_RTAX58U_V2:
		case MODEL_RTAXE7800:
		case MODEL_TUFAX3000_V2:
		case MODEL_RTAX3000N:
		case MODEL_BR63:
		case MODEL_GT10:
		case MODEL_RTAX9000:
		case MODEL_RTAX55:
		case MODEL_RTAX56U:
		case MODEL_RPAX56:
		case MODEL_RPAX58:
		case MODEL_RPBE58:
		case MODEL_RTAX86U:
		case MODEL_RTAX68U:
		case MODEL_RTAC68U_V4:
		case MODEL_GTAXE11000:
		case MODEL_GTAX6000:
		case MODEL_GTAX11000_PRO:
		case MODEL_GTAXE16000:
		case MODEL_GTBE98:
		case MODEL_GTBE98_PRO:
		case MODEL_ET12:
		case MODEL_XT12:
		case MODEL_RTAX86U_PRO:
		case MODEL_RTAX88U_PRO:
		case MODEL_RTBE96U:
		case MODEL_GTBE96:
		case MODEL_RTBE88U:
		case MODEL_RTBE86U:
		case MODEL_RTBE58U:
		case MODEL_RTBE58U_V2:
		case MODEL_RTBE92U:
		case MODEL_RTBE95U:
		case MODEL_GTBE19000:
		case MODEL_RTBE82U:
		case MODEL_RTBE82M:
		case MODEL_RTBE58U_PRO:
		case MODEL_RTBE58_GO:
		case MODEL_GTBE19000AI:
		case MODEL_GSBE18000:
		case MODEL_GT7:
		case MODEL_GTBE96_AI:
			if (!nvram_get("lan_hwaddr"))
				nvram_set("lan_hwaddr", cfe_nvram_safe_get("et0macaddr"));
			break;
		case MODEL_RTAC3200:
			if (!nvram_get("et0macaddr"))				// eth0 (ethernet)
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			nvram_set("1:macaddr", nvram_safe_get("et0macaddr"));	// eth2 (2.4GHz)
			if (!nvram_get("0:macaddr"))				// eth1(5GHz)
				nvram_set("0:macaddr", "00:22:15:A5:03:04");
			if (!nvram_get("2:macaddr"))				// eth3(5GHz)
				nvram_set("2:macaddr", "00:22:15:A5:03:08");
			break;

		case MODEL_RTAC53U:
			if (!nvram_get("et0macaddr"))	//eth0, eth1
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			if (!nvram_get("0:macaddr"))	//eth2(5G)
				nvram_set("0:macaddr", "00:22:15:A5:03:04");
			nvram_set("sb/1/macaddr", nvram_safe_get("et0macaddr"));
			break;

		case MODEL_RTAC1200G:
		case MODEL_RTAC1200GP:
			if (!nvram_get("et0macaddr"))
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
			if (!nvram_get("sb/1/macaddr"))	// (5GHz)
				nvram_set("sb/1/macaddr", "00:22:15:A5:03:04");
			nvram_set("0:macaddr", nvram_safe_get("et0macaddr")); // (2.4GHz)

		default:
#ifdef RTCONFIG_RGMII_BRCM5301X
			if (!nvram_get("lan_hwaddr"))
				nvram_set("lan_hwaddr", cfe_nvram_get("et1macaddr"));
#else
			if (!nvram_get("et0macaddr"))
				nvram_set("et0macaddr", "00:22:15:A5:03:00");
#endif
			break;
	}

#if defined(RTAX86U)
	fill_cfe_block();
#endif

#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) \
		|| defined(RTAX86U)
	sync_cfe_mac();
#endif

#ifdef RTCONFIG_ODMPID
	char odmpid[32];
	snprintf(odmpid, sizeof(odmpid), "%s", nvram_safe_get("odmpid"));
	if(!strcmp(odmpid, "ASUS") ||
			!is_valid_hostname(odmpid) ||
			!strcmp(RT_BUILD_NAME, odmpid))
		nvram_set("odmpid", "");
#endif

	char *value, devPwd[9];
	extern uint32 wps_gen_pin(char *devPwd, int devPwd_len);

	/* Validate wps_device_pin existance, embedded nvram can override it */
	value = nvram_get("secret_code");
	if (!value || !strcmp(value, "12345670")) {
		/* Generate a random device's PIN */
		nvram_set("wps_device_pin", wps_gen_pin(devPwd, sizeof(devPwd)) ? devPwd : "12345670");
	} else
		nvram_set("wps_device_pin", value);
#ifdef RTCONFIG_AMAS
	int ret = 0;

	ret = chk_acscli2_cmds("acs_restart");
	_dprintf("%s, acscli2_can_do_restart:%d\n", __func__, ret);
#endif
}

#ifdef RTCONFIG_BCMARM
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(BCA_HNDROUTER) && (defined(RTCONFIG_HND_WL) || defined(BCM4912) || defined(BCM6813) || defined(BCM6765))
void wl_thread_affinity_update(void)
{
	int i = 0;
#if !defined(BCM6750) || !defined(BCM63178)
	int map_shift = 1;
#endif
	char interrupt_string[5];
	pid_t pid_wl;
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
	pid_t pid_archer;
	char value[sizeof("255")];
#endif
	char affinity[16] = {0};
	char pid[16] = {0};
	char process_name[16];
	FILE *fp = NULL;
	char buf[128];
	char affinity_cmd[128];
	int cpu_count = 0;
	char tmp[100], prefix[]="wlXXXXXXX_", *ptr;
	int val;

	/* Check for NVRAM parameter set */
	if (atoi(nvram_safe_get("cpu_affinity_disable"))) {
		_dprintf("%s: disable by nvram!\n", __func__);
		return;
	}

#define MAX_RADIO_NUM 4
#define CPU_MAP 0x1

	if ((pid_wl = get_pid_by_thrd_name("bcmsw_rx")) > 0)
	{
		_dprintf("%s: set bcmsw_rx to be 1\n", __func__);
		sprintf(pid, "%d", pid_wl);
		eval("taskset", "-p", "0x1", pid);
	}
#if 0
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
	if ((pid_archer = get_pid_by_thrd_name("bcm_archer_us")) > 0)
	{
		_dprintf("%s: set archer\n", __func__);
		sprintf(pid, "%d", pid_archer);
		sprintf(value, "%d", CPU_MAP << (sysconf(_SC_NPROCESSORS_CONF) - 1));
		eval("taskset", "-p", value, pid);
	}
#if defined(BCM6750) || defined(BCM63178)
	if ((pid_archer = get_pid_by_thrd_name("bcmsw_recycle")) > 0)
	{
		sprintf(pid, "%d", pid_archer);
		sprintf(value, "%d", CPU_MAP << 1);
		eval("taskset", "-p", value, pid);
	}
	if ((pid_archer = get_pid_by_thrd_name("bcm_archer_wlan")) > 0)
	{
		sprintf(pid, "%d", pid_archer);
		sprintf(value, "%d", CPU_MAP << 1);
		eval("taskset", "-p", value, pid);
	}
#endif
#endif
#endif
	/* Get the CPU count */
	if ((fp = fopen("/proc/cpuinfo", "r")) != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strstr(buf, "processor") != NULL)
				cpu_count++;
		}
		fclose(fp);
	}

#if defined(RTCONFIG_HND_ROUTER_AX_6710)
	if ((pid_wl = get_pid_by_thrd_name("skb_free_task")) > 0)
	{
		_dprintf("%s: set skb_free_task to be %d\n", __func__, (CPU_MAP << (cpu_count - 1)));
		sprintf(pid, "%d", pid_wl);
		sprintf(affinity, "%d", (CPU_MAP << (cpu_count - 1)));
		eval("taskset", "-p", affinity, pid);
	}
	if ((pid_wl = get_pid_by_thrd_name("pdc_rx")) > 0)
	{
		_dprintf("%s: set pdc_rx to be 1\n", __func__);
		sprintf(pid, "%d", pid_wl);
		eval("taskset", "-p", "0x1", pid);
	}
#endif

	/* Set the Interrupt Affinities */
	if ((fp = fopen("/proc/interrupts", "r")) != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strstr(buf, WL_IF_PREFIX_2) != NULL
#if defined(RTAX95Q) || defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XT8_V2) || defined(RTAXE95Q) || defined(ET8PRO) || defined(ET8_V2) || defined(BCM6750) || defined(BCM63178)
				|| strstr(buf, "dhdpcie") != NULL
#endif
			) {
				strncpy(interrupt_string, buf, 4);
				/* Check for MAX CPU and create CMD */
#if defined(BCM6750) || defined(BCM63178)
				sprintf(affinity_cmd, "echo %d > /proc/irq/%d/smp_affinity",
					(CPU_MAP << 1), atoi(interrupt_string));
#else
				if (map_shift == cpu_count){
					printf("More interfaces exist than CPUs, Setting Interrupt Affinity to last CPU\n");
					sprintf(affinity_cmd, "echo %d > /proc/irq/%d/smp_affinity",
						(CPU_MAP << (cpu_count - 1)), atoi(interrupt_string));
				}
				else {
					sprintf(affinity_cmd, "echo %d > /proc/irq/%d/smp_affinity",
						(CPU_MAP << (map_shift++)), atoi(interrupt_string));
				}
#endif
				system(affinity_cmd);
#if defined(RTAX95Q) || defined(RTAXE95Q)
 				system("echo 2 > /proc/irq/67/smp_affinity");
#endif
			}
		}
		fclose(fp);
	}

	/* Set Affinities for the WL threads */
	for (i = 0; i < MAX_RADIO_NUM; i++) {
		/* Check for MAX CPU and set map index */
		snprintf(prefix, sizeof(prefix), "wl%d_", i);
		if ((ptr = nvram_get(strcat_r(prefix, "affinity", tmp))) && *ptr && (val = atoi(ptr)) > 0)
			snprintf(affinity, sizeof(affinity), "%d", val);
		else {
			if (i >= (cpu_count -1)) {
				printf("More WL interfaces exist than CPUs, Setting Interrupt Affinity to last CPU\n");
				sprintf(affinity, "%d", CPU_MAP << (cpu_count - 1));
			} else {
				sprintf(affinity, "%d", CPU_MAP << (i+1));
			}
		}

		sprintf(process_name, "wl%d-kthrd", i);
		if ((pid_wl = get_pid_by_thrd_name(process_name)) > 0) {
			_dprintf("%s: set wl%d-kthrd to be %s\n", __func__, i, affinity);
			sprintf(pid, "%d", pid_wl);
			eval("taskset", "-p", affinity, pid);
			eval("chrt", "-p", "5", pid);
		}

		sprintf(process_name, "wfd%d-thrd", i);
		if ((pid_wl = get_pid_by_thrd_name(process_name)) > 0) {
			_dprintf("%s: set wfd%d-thrd to be %s\n", __func__, i, affinity);
			sprintf(pid, "%d", pid_wl);
			eval("taskset", "-p", affinity, pid);
			eval("chrt", "-p", "5", pid);
		}
	}
}

#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916) || defined(RTCONFIG_HND_ROUTER_AX_6710) || defined(RTCONFIG_BCM_502L07P2)
void wlaffinity(void)
{
#if defined(BCM6750) || defined(BCM63178)
	nvram_set("BCM_MODEL", "Broadcom BCM963178");
#elif defined(BCM6755)
	nvram_set("BCM_MODEL", "Broadcom BCM947622");
#elif defined(BCM6756)
	nvram_set("BCM_MODEL", "Broadcom BCM96756");
#endif
	eval("/bin/wlaffinity", "auto");

	/* Set Affinities for the WL threads */
#if 0 //def GT10
	char word[256], *next;
	int unit;
	int pid;
	char proc_wfd_thrd_name[10] = { 0 };
	char proc_wl_kthrd_name[10] = { 0 };
	int aff_wfd_thrd[] = { 4, 2, 1 };
	int aff_wl_kthrd[] = { -1, -1, 1 };

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		if (wl_probe(word) || wl_ioctl(word, WLC_GET_INSTANCE, &unit, sizeof(unit)))
			continue;

		if (aff_wfd_thrd[unit] != -1) {
			snprintf(proc_wfd_thrd_name, sizeof(proc_wfd_thrd_name), "wfd%d-thrd", unit);
			if ((pid = get_pid_by_thrd_name(proc_wfd_thrd_name)) > 0) {
				dbg("%s set %s pid:%d to aff:%d\n", __func__, proc_wfd_thrd_name, pid, aff_wfd_thrd[unit]);
				doSystem("taskset -p %d %d", aff_wfd_thrd[unit], pid);
			}
		}

		if (aff_wl_kthrd[unit] != -1) {
			snprintf(proc_wl_kthrd_name, sizeof(proc_wl_kthrd_name), "wl%d-kthrd", unit);
			if ((pid = get_pid_by_thrd_name(proc_wl_kthrd_name)) > 0) {
				dbg("%s set %s pid:%d to aff:%d\n", __func__, proc_wl_kthrd_name, pid, aff_wl_kthrd[unit]);
				doSystem("taskset -p %d %d", aff_wl_kthrd[unit], pid);
			}
		}
	}
#endif
}
#endif

#endif /* BCA_HNDROUTER */

#ifdef HND_ROUTER
void tweak_usb_affinity(int enable)
{
	char smp[32], val_on[4], val_off[4];
	int *ptr;
#if defined(RTCONFIG_HND_ROUTER_AX_6710)
	int usb_irqs[] = {25, 26, 27, -1};	// RT-AX86U, RT-AX86S, RT-AX68U. RT-AC68U_V4
#elif defined(BCM6755)
	int usb_irqs[] = {45, 46, 47, -1};	// BCM6755
#elif defined(BCM6756)
	int usb_irqs[] = {47, 48, 49, -1};	// BCM6756
#elif defined(RTCONFIG_HND_ROUTER_AX_6756) && defined(BCM6750)
	int usb_irqs[] = {37, 38, 39, -1};	// BCM6750
#elif defined(BCM6750) || defined(BCM63178)
	int usb_irqs[] = {41, 42, 43, -1};	// BCM6750
#elif defined(RTAX86U_PRO)
	int usb_irqs[] = {56, -1};	// BCM4912
#elif defined(BCM4912)
	int usb_irqs[] = {58, -1};		// BCM4912
#elif defined(BCM6855)
	int usb_irqs[] = {34, 35, 36, -1};	// BCM6753
#elif defined(BCM6813)
	int usb_irqs[] = {16, -1};		// BCM4916
#elif defined(BCM6766)
	int usb_irqs[] = {24, -1};		// BCM6766
#elif defined(BCM6765)
	int usb_irqs[] = {10, -1};		// BCM6764, BCM6764L
#else
	int usb_irqs[] = {28, 29, 30, -1};	// BCM4906, BCM4908. BCM4906_504 ?
#endif
	int on, off, i;
	int cpu_num = sysconf(_SC_NPROCESSORS_CONF);

#if defined(RTCONFIG_HND_ROUTER_AX_675X) || defined(BCM6756) || defined(BCM6750)
	if(cpu_num > 2)
		on = 1 << (cpu_num - 2);
	else
#endif
	on = 1 << (cpu_num - 1);
	snprintf(val_on, sizeof(val_on), "%x", on);

	char buf[8];
	if (f_read_string("/proc/irq/default_smp_affinity", buf, sizeof(buf)) > 0)
		snprintf(val_off, sizeof(val_off), "%s", buf);
	else {
		off = 0;
		for (i = 0; i < cpu_num; i++)
			off |= (1 << i);
		snprintf(val_off, sizeof(val_off), "%x", off);
	}

#if !defined(BCM6750) && !defined(BCM6755) && !defined(BCM6756) && !defined(BCM6855)
	ptr = usb_irqs;
	while(*ptr != -1){
		snprintf(smp, sizeof(smp), "/proc/irq/%d/smp_affinity", *ptr);
		f_write_string(smp, enable ? val_on : val_off, 0, 0);

		++ptr;
	}
#endif
}

void tweak_process_affinity(pid_t pid, unsigned int cpumask)
{
	cpu_set_t cpuset;
	int i = 0;

	CPU_ZERO(&cpuset);
	do {
		if (cpumask & 1)
			CPU_SET(i, &cpuset);
		i++;
		cpumask >>= 1;
	} while (cpumask != 0);

	sched_setaffinity(pid, sizeof(cpu_set_t), &cpuset);
}

void init_others(void)
{
#if defined(RTCONFIG_HND_ROUTER_AX_6756) || defined(RTCONFIG_HND_ROUTER_BE_4916)
#else
	eval("insmod", "hnd");
#endif

#if defined(RTCONFIG_HND_ROUTER_BE_4916)
	eval("insmod", "xt_SKIPLOG");
#endif

	//tweak UDP socket buffer
	f_write_string("/proc/sys/net/core/wmem_default", "524288", 0, 0);
	f_write_string("/proc/sys/net/core/wmem_max", "524288", 0, 0);
	f_write_string("/proc/sys/net/core/rmem_default", "524288", 0, 0);
	f_write_string("/proc/sys/net/core/rmem_max", "524288", 0, 0);

#ifdef GTAC2900
	update_cfe_ac2900();
#endif
#if defined(RTAC86U) || defined(GTAC2900)
	if (!nvram_get_int("cpu_wait"))
		system("pwr config --wait off");
#endif
#if defined(RTAX88U) || defined(RTAX92U) || defined(BC109)
	if(nvram_match("HwVer", "1.0")) {
		system("pwr config --cpuwait off");
	}
#endif
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
	update_cfe_675x();
#endif
#if defined(BCM6750) && !defined(RTAX82_XD6) && !defined(RTAX82_XD6S) && !defined(TUFAX5400) && !defined(RTAX82U_V2) && !defined(TUFAX5400_V2) && !defined(XD6_V2) && !defined(RTAX5400)
	update_cfe_ax58u();
#endif
#if defined(RTCONFIG_BCM_MFG) && (defined(RTAX55) || defined(RTAX1800) || defined(TUFAX5400))
	update_cfe_ax55();
#endif
#if defined(DSL_AX82U)
	update_misc1();
	update_cfe_ax82u();
	check_wlfeature();
	{
		pid_t pid;
		if ((pid = get_pid_by_thrd_name("bcmxtm_rx")) > 0)
			tweak_process_affinity(pid, 1);
		if ((pid = get_pid_by_thrd_name("bcmxtm_recycle")) > 0)
			tweak_process_affinity(pid, 2);
	}
#endif
#ifdef GTAX6000
	update_cfe_ax6000();
#endif
#ifdef RTAX56U
	/* restore LED 16 active high/low status (refer cfe) */
	system("sw 0xff803014 0xffffffff");
#endif
#if defined(RTAX56_XD4) || defined(CTAX56_XD4)
	/* restore LED 16 active high/low status (refer cfe) */
	system("sw 0xff803014 0xffffffff");

	/* high performance */
	system("echo 255 > /sys/class/leds/22/brightness");

	setAllLedOn();

	if(nvram_match("HwId", "B") || nvram_match("HwId", "D")){
		if(nvram_get_int("x_Setting") == 0)
			nvram_set("disable_ui", "1");
		else
			nvram_set("disable_ui", "0");
	}
#endif
#if defined(ET12) || defined(XT12)
	setAllLedOn();
#endif
#if defined(RTAX58U_V2) || defined(TUFAX3000_V2) || defined(GT10) || defined(BR63) || defined(RTAX9000)
	/* set pinmux of GPIO 80 as 4 to enable GPIO mode */
	system("sw 0xff800554 0");
	system("sw 0xff800558 0x4050");
	system("sw 0xff80055c 0x21");
	/* restore USB power */
	f_write_string("/sys/class/leds/led_gpio_80/brightness", "255", 0, 0);
#endif
#if defined(BR63)
	/* set pinmux of GPIO 82 as 4 to enable GPIO mode */
	system("sw 0xff800554 0");
	system("sw 0xff800558 0x4052");
	system("sw 0xff80055c 0x21");
	/* restore USB power */
	f_write_string("/sys/class/leds/led_gpio_82/brightness", "255", 0, 0);
#endif
#if defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	/* set pinmux of GPIO 67 as 4 to enable GPIO mode */
	system("sw 0xff800554 0");
	system("sw 0xff800558 0x4043");
	system("sw 0xff80055c 0x21");
	/* restore USB power */
	f_write_string("/sys/class/leds/led_gpio_67/brightness", "255", 0, 0);
#endif
#if defined(BT12) || defined(BQ16) || defined(BQ16_PRO)
	/* set parallel led register */
	system("sw 0xff803018 0x011C0000");
	/* disable GPIO 50 (LED24) */
	system("sw 0xff8031a0 0x0000");
	system("sw 0xff80301c 0x01000000");
#endif
#if defined(BT10)
	/* set parallel led register */
	// system("sw 0xff803018 0x01280010");
#endif
#if defined(GTBE19000AI) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GTBE96_AI)
	button_pressed(BTN_WAKE);
#endif
#if defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)	//WAR
	f_write_string("/sys/class/leds/led_gpio_3/brightness", "255", 0, 0);
	usleep(25*1000);
	f_write_string("/sys/class/leds/led_gpio_3/brightness", "0", 0, 0);
#endif
#if defined(RTBE92U) || defined(RTBE95U) || defined(RTBE58U_PRO)
	if (!nvram_get_int("bcmspu_reqd"))
	eval("rmmod", "bcmspu");
#endif
#ifdef RTBE92U
	config_avs();
#endif
}
#else // HND_ROUTER

#define ASUS_TWEAK

#ifdef RTCONFIG_BCM_7114
#define SMP_AFFINITY_WL	"1"
#else
#define SMP_AFFINITY_WL "3"
#endif

void tweak_smp_affinity(int enable_samba)
{
#ifndef RTCONFIG_BCM7
	if (nvram_get_int("stop_tweak_wl") == 1)
#endif
		return;

#ifdef RTCONFIG_GMAC3
	if (nvram_match("gmac3_enable", "1"))
		return;
#endif

#ifdef RTCONFIG_BCM_7114
	if (enable_samba) {
		f_write_string("/proc/irq/163/smp_affinity", SMP_AFFINITY_WL, 0, 0);
		f_write_string("/proc/irq/169/smp_affinity", SMP_AFFINITY_WL, 0, 0);
	}
	else
#endif
	{
		f_write_string("/proc/irq/163/smp_affinity", "2", 0, 0);
		f_write_string("/proc/irq/169/smp_affinity", "2", 0, 0);
	}
}

void init_others(void)
{
#ifdef SMP
	int fd;

	if ((fd = open("/proc/irq/163/smp_affinity", O_RDWR)) >= 0) {
		close(fd);
#ifdef RTCONFIG_GMAC3
		if (nvram_match("gmac3_enable", "1")) {
			if (nvram_match("asus_tweak_usb_disable", "1")) {
				char *fwd_cpumap;

				/* Place network interface vlan1/eth0 on CPU hosting 5G upper */
				fwd_cpumap = nvram_get("fwd_cpumap");

				if (fwd_cpumap == NULL) {
					/* BCM4709acdcrh: Network interface GMAC on Core#0
					 * [5G+2G:163 on Core#0] and [5G:169 on Core#1].
					 * Bind et2:vlan1:eth0:181 to Core#0
					 * Note, USB3 xhci_hcd's irq#112 binds Core#1
					 * bind eth0:181 to Core#1 impacts USB3 performance
					 */
					f_write_string("/proc/irq/181/smp_affinity", "1", 0, 0);
				} else {
					char cpumap[32], *next;

					foreach(cpumap, fwd_cpumap, next) {
						char mode, chan;
						int band, irq, cpu;

						/* Format: mode:chan:band#:irq#:cpu# */
						if (sscanf(cpumap, "%c:%c:%d:%d:%d",
							&mode, &chan, &band, &irq, &cpu) != 5) {
							break;
						}
						if (cpu > 1) {
							break;
						}
						/* Find the single 5G upper */
						if ((chan == 'u') || (chan == 'U')) {
							char command[128];
							snprintf(command, sizeof(command),
								"echo %d > /proc/irq/181/smp_affinity",
								1 << cpu);
							system(command);
							break;
						}
					}
				}
			}
			else
#ifdef RTCONFIG_BCM_7114
				f_write_string("/proc/irq/181/smp_affinity", "1", 0, 0);
#else
				f_write_string("/proc/irq/181/smp_affinity", "3", 0, 0);
#endif
		} else
#endif
		{
#ifdef ASUS_TWEAK
#ifdef RTCONFIG_BCM_7114
			f_write_string("/proc/irq/180/smp_affinity", "1", 0, 0);
#endif
#ifndef RTCONFIG_BCM7
			if (nvram_match("asus_tweak", "1")) {	/* dbg ref ? */
				f_write_string("/proc/irq/179/smp_affinity", "1", 0, 0);	// eth0
				f_write_string("/proc/irq/163/smp_affinity", "2", 0, 0);	// eth1 or eth1/eth2
				f_write_string("/proc/irq/169/smp_affinity", "2", 0, 0);	// eth2 or eth3
			} else
#endif	// RTCONFIG_BCM7
				tweak_smp_affinity(0);
#endif	// ASUS_TWEAK
		}

		if (!nvram_get_int("stop_tweak_usb")) {
			f_write_string("/proc/irq/111/smp_affinity", "2", 0, 0);		// ehci, ohci
			f_write_string("/proc/irq/112/smp_affinity", "2", 0, 0);		// xhci
		}
	}
#endif	// SMP

#ifdef ASUS_TWEAK
#ifdef RTCONFIG_BCM_7114
	nvram_unset("txworkq");
#else
	if (nvram_match("enable_samba", "1") &&
#ifdef RTCONFIG_USB_XHCI
		nvram_match("usb_usb3", "1") &&
#endif
		!factory_debug()) {
		nvram_set("txworkq", "1");
#if !defined(RTCONFIG_BCM9) && !defined(RTCONFIG_BCM7)
		nvram_set("txworkq_wl", "1");
#endif
	} else {
		nvram_unset("txworkq");
#if !defined(RTCONFIG_BCM9) && !defined(RTCONFIG_BCM7)
		nvram_unset("txworkq_wl");
#endif
	}
#endif
#endif // ASUS_TWEAK

#if defined(RTAC68U) && !defined(RTAC68A) && !defined(RT4GAC68U)
	update_cfe();
#endif
#ifdef RTAC3200
	update_cfe_ac3200();
	update_cfe_ac3200_128k();
#endif
#ifdef RTAC68U
	ac68u_cofs();
#endif
}
#endif	// HND_ROUTER
#endif	// RTCONFIG_BCMARM

#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
void init_others_post(void)
{
	mxl_fw_check();

	enable_jumbo_frame();

	/* enable LLDP pass-through */
	system("fapi-GSW-PceRuleDisable pattern.nIndex=12");

	/* enable LACP pass-through */
	system("fapi-GSW-PceRuleDisable pattern.nIndex=13");

	/* Set Serdes0 to XFI mode to workaround possible no Tx issue */
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	system("fapi-GSW-Sfp-Set nPortId=0 nOption=0 nMode=1 nSpeed=2");
	system("fapi-GSW-Sfp-Set nPortId=1 nOption=0 nMode=1 nSpeed=2");
#endif
}
#endif

void chanspec_fix_5g(int unit)
{
	char tmp[100], prefix[]="wlXXXXXXX_";
	int channel;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	snprintf(tmp, sizeof(tmp), "wl%d_nband", unit);
	if (nvram_get_int(tmp) != WLC_BAND_5G)
		return;

	channel = nvram_get_int(strcat_r(prefix, "channel", tmp));

	if ((channel == 36) || (channel == 44) || (channel == 52) || (channel == 60) || (channel == 100) || (channel == 108) || (channel == 116) || (channel == 124) || (channel == 132) || (channel == 149) || (channel == 157))
	{
		dbG("fix nctrlsb of channel %d as %s\n", channel, "lower");
		nvram_set(strcat_r(prefix, "nctrlsb", tmp), "lower");
	}
	else if ((channel == 40) || (channel == 48) || (channel == 56) || (channel == 64) || (channel == 104) || (channel == 112) || (channel == 120) || (channel == 128) || (channel == 136) || (channel == 153) || (channel == 161))
	{
		dbG("fix nctrlsb of channel %d as %s\n", channel, "upper");
		nvram_set(strcat_r(prefix, "nctrlsb", tmp), "upper");
	}
}

// this function is used to jutisfy which band(unit) to be forced connected.
int is_ure(int unit)
{
	// forced to connect to which band
	// is it suitable
	if (sw_mode() == SW_MODE_REPEATER) {
		if (nvram_get_int("wlc_band") == unit) return 1;
	}
	return 0;
}

#if defined (RTCONFIG_WISP)
int is_wisp(int unit)
{
	if (wisp_mode()) {
		if (nvram_get_int("wlc_band") == unit) return 1;
	}
	return 0;
}
#endif

int is_ap(int unit)
{
	if (unit < 0) return 0;

	if (is_ure(unit) 
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
	   || is_psta(unit) || is_psr(unit)
#endif
	)
		return 0;

	return 1;
}

#ifdef BCM_BSD
#define BSD_STA_SELECT_POLICY_NVRAM		"bsd_sta_select_policy"
#define BSD_STA_SELECT_POLICY_FLAG_NON_VHT	0x00000008	/* NON VHT STA */
#define BSD_IF_QUALIFY_POLICY_NVRAM		"bsd_if_qualify_policy"
#define BSD_QUALIFY_POLICY_FLAG_NON_VHT		0x00000004	/* NON VHT STA */
#ifdef RTCONFIG_HAS_5G_2
#define BSD_STA_SELECT_POLICY_NVRAM_X		"bsd_sta_select_policy_x"
#define BSD_IF_QUALIFY_POLICY_NVRAM_X		"bsd_if_qualify_policy_x"
#endif

int get_bsd_nonvht_status(int unit)
{
	char tmp[100], prefix[]="wlXXXXXXX_";
	char *str;
	int num;
	unsigned int i1,i2,i3,i4,i5,i6,i7,i8,i9,ia;
	unsigned int flags;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
#ifdef RTCONFIG_HAS_5G_2
	if (nvram_get_int("smart_connect_x") == SMRTCONN_5G_ONLY) // 5GHz Only
		str = nvram_get(strcat_r(prefix, BSD_STA_SELECT_POLICY_NVRAM_X, tmp));
	else
#endif
		str = nvram_get(strcat_r(prefix, BSD_STA_SELECT_POLICY_NVRAM, tmp));
	if (str) {
		num = sscanf(str, "%d %d %d %d %d %d %d %d %d %d %x",
			&i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &ia, &flags);
		if ((num == 11) && (flags & BSD_STA_SELECT_POLICY_FLAG_NON_VHT))
			return 1;
	}
#ifdef RTCONFIG_HAS_5G_2
	if (nvram_get_int("smart_connect_x") == SMRTCONN_5G_ONLY) // 5GHz Only
		str = nvram_get(strcat_r(prefix, BSD_IF_QUALIFY_POLICY_NVRAM_X, tmp));
	else
#endif
		str = nvram_get(strcat_r(prefix, BSD_IF_QUALIFY_POLICY_NVRAM, tmp));
	if (str) {
		num = sscanf(str, "%d %x", &i1, &flags);
		if ((num == 2) && (flags & BSD_QUALIFY_POLICY_FLAG_NON_VHT))
			return 1;
	}

	return 0;
}
#endif

int find_user_unit(char *ustr)
{
	char tmp[100], prefix[]="wlXXXXXXX_";
	char word[256], *next;
	int unit = 0;

        foreach (word, nvram_safe_get("wl_ifnames"), next) {
		snprintf(prefix, sizeof(prefix), "wlc%d_", unit);
		if(*nvram_safe_get(strcat_r(prefix, ustr, tmp))) {
			return unit;
		}
                unit++;
        }

	_dprintf("%s: no results\n", __func__);
	return 0;
}

/* adjust for triband 43684/6715 backhaul connection */
void adjust_txbf_bfe_cap(int unit, char *orig_txbf_bfe_cap)
{
	int i = 0;
	int is_re_mode = 0;
	char tmp[100];
	char prefix_bh[]="wlXXXXXXX_";
	char prefix_fh[]="wlXXXXXXX_";
#if defined(RTAX95Q) || defined(XT8PRO) || defined(XT12)
	/* wl2 */
	int band_to_config[] = {2};
#elif defined(BT10)
	/* wl0 */
	int band_to_config[] = {WLIF_6G};
#elif defined(BT12)
	/* wl0, wl1, wl2 */
	int band_to_config[] = {0, 1, 2};
#elif defined(BQ16)
	/* wl3, wl0, wl1, wl2 */
	int band_to_config[] = {3, 0, 1, 2};
#elif defined(BQ16_PRO)
	/* wl3, wl0, wl1, wl2 */
	int band_to_config[] = {3, 0, 1, 2};
#elif defined(GT10) || defined(RTAX9000)
	/* wl1 */
	int band_to_config[] = {1};
#elif defined(RTBE95U)
	/* wl0 */
	int band_to_config[] = {0};
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	/* wl0 */
	int band_to_config[] = {0};
#else
	int band_to_config[] = {-1};
#endif
	is_re_mode = nvram_get_int("re_mode");

	if(is_re_mode == 0){	/* CAP */
		snprintf(prefix_fh, sizeof(prefix_fh), "wl%d_", unit);
	}else{	/* RE */
		snprintf(prefix_bh, sizeof(prefix_bh), "wl%d_", unit);
		snprintf(prefix_fh, sizeof(prefix_fh), "wl%d.1_", unit);
	}

	for( i = 0 ; i < ((int)(sizeof(band_to_config) / sizeof(band_to_config[0]))) ; i++){
		if(band_to_config[i] == unit){
			if(is_re_mode == 0){ /* CAP */
				if(atoi(orig_txbf_bfe_cap) == 15){ /* enable txbf_bfe */
					nvram_set(strcat_r(prefix_fh, "txbf_bfe_cap", tmp), "143");
				}else{ /* disable txbf_bfe */
					nvram_set(strcat_r(prefix_fh, "txbf_bfe_cap", tmp), orig_txbf_bfe_cap);
				}
			}else{	/* RE */
				if(atoi(orig_txbf_bfe_cap) == 15){ /* enable txbf_bfe */
					nvram_set(strcat_r(prefix_bh, "txbf_bfe_cap", tmp), "143");
					nvram_set(strcat_r(prefix_fh, "txbf_bfe_cap", tmp), "143");
				}else{ /* disable txbf_bfe */
					nvram_set(strcat_r(prefix_bh, "txbf_bfe_cap", tmp), orig_txbf_bfe_cap);
					nvram_set(strcat_r(prefix_fh, "txbf_bfe_cap", tmp), orig_txbf_bfe_cap);
				}
			}
		}
	}
}

void generate_wl_para(char *ifname, int unit, int subunit)
{
	dbG("unit %d subunit %d\n", unit, subunit);

	char tmp[100], prefix[]="wlXXXXXXX_";
	char tmp2[100], prefix2[]="wlXXXXXXX_";
	char prefix3[]="wlXXXXXXX_", *tmp3;
	int unit2;
	char *list, *list2;
	int list_size;
	char *nv, *nvp, *b;
#ifndef RTCONFIG_BCMWL6
	int match;
	char word[256], *next;
#endif
	int i, mcast_rate;
	char interface_list[NVRAM_MAX_VALUE_LEN];
	int interface_list_size = sizeof(interface_list);
	char nv_interface[NVRAM_MAX_PARAM_LEN];
	int ure;
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
#ifdef RTCONFIG_AMAS
	int max_no_vifs = 8;
#else
	int max_no_vifs = wl_max_no_vifs(unit);
#endif
	char lan_ifnames[NVRAM_MAX_PARAM_LEN] = "lan_ifnames";
	bool psta = 0, psr = 0;
#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_DPSTA)
	char prefix_local[]="wlXXXXXXX_";
#endif
#endif
#ifdef RTCONFIG_BCMWL6
	int phytype;
	char buf[8];
	int bss_opmode_cap_reqd = 0;
#endif
#if defined(RTCONFIG_HND_ROUTER_AX)
	int cap_11ax = wl_cap(unit, "11ax");
	int ax_en = 0;
#endif
#if defined(RTCONFIG_WIFI7)
	int cap_11be = wl_cap(unit, "11be");
	int eht_feats = 0;
#endif
#if defined(RTCONFIG_BW320M)
	int cap_320m = wl_cap(unit, "320");
#endif

#ifdef GTAXE16000
	int wlc_idx_align[4] = {1, 2, 3, 0}; // wl0(wlc1), w1(wlc2), wl2(wlc3), wl3(wlc0);
#elif defined(GTBE98) || defined(BQ16) || defined(BQ16_PRO)
	int wlc_idx_align[4] = {1, 2, 3, 0}; // wl0(wlc1), w1(wlc2), wl2(wlc3), wl3(wlc0);
#elif defined(GTBE98_PRO)
	int wlc_idx_align[4] = {1, 2, 3, 0}; // wl0(wlc1), w1(wlc2), wl2(wlc3), wl3(wlc0);
#elif defined(BT10)
	int wlc_idx_align[3] = {WLIF_2G, WLIF_5G1, WLIF_6G};    // wl0(wlc2), wl1(wlc1), wl2(wlc0);
#elif defined(GT10) || defined(RTAX9000)
	int wlc_idx_align[3] = {1, 2, 0};    // wl0(wlc1), wl1(wlc2), wl2(wlc0);
#elif defined(RTBE95U)
	int wlc_idx_align[3] = {1, 2, 0};    // wl0(wlc1), wl1(wlc2), wl2(wlc0);
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	int wlc_idx_align[3] = {2, 1, 0};    // wl0(wlc1), wl1(wlc2), wl2(wlc0);
#endif
	char *p;

	_dprintf("%s, [%s: unit:%d, subunit:%d], cap_11be=%d\n", __func__, ifname, unit, subunit, cap_11be);

	if (subunit == -1)
	{
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

#ifdef RTCONFIG_BCMWL6
		if (!wl_ioctl(ifname, WLC_GET_PHYTYPE, &phytype, sizeof(phytype))) {
			memset(buf, 0, sizeof(buf));
			snprintf(buf, sizeof(buf), "%s", WLCONF_PHYTYPE2STR(phytype));
			nvram_set(strcat_r(prefix, "phytype", tmp), buf);
		}
#endif

#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
		if (nvram_get_int(strcat_r(prefix, "nband", tmp)) == 4) { // 6G band
			nvram_set(strcat_r(prefix, "wps_mode", tmp), "disabled");
			if (!strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "open")) {
				nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "owe");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
				nvram_set(strcat_r(prefix, "mfp", tmp), "2");
			}
			else if(strlen(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp))) > 0 &&
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "sae") &&
#ifdef RTCONFIG_WPA3_ENTERPRISE
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "wpa3") &&
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "suite-b") &&
#endif
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "owe")) {
				nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "sae");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
			}
		}
		else {	// for 2.4G & 5G band, enable oob discovery
			nvram_set(strcat_r(prefix, "nbr_discovery_cap", tmp), "1");
		}
#endif
		nvram_set("lan_wps_oob", nvram_match("w_Setting", "1") ? "disabled" : "enabled");
#ifdef RTCONFIG_MULTILAN_MWL
		if (nvram_get_int("w_Setting") && get_fh_if_prefix_by_unit(unit, prefix2, sizeof(prefix2))) {
			trim_space(prefix2);
			strncat(prefix2, "_", 1);
		} else
			snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);
#ifdef RTCONFIG_AMAS
		if (is_router_mode() && !nvram_get_int("x_Setting") && nvram_get_int("amesh_wps_enr"))
			p = prefix;
		else
#endif
		p = prefix2;
#else
		p = prefix;
#endif
		nvram_set(strcat_r(p, "wps_mode", tmp), (nvram_match("wps_enable", "1") && (is_ap(unit)
#ifdef RTCONFIG_PROXYSTA
			|| (((unit == WL_2G_BAND) || unit == nvram_get_int("wps_band_x")) && (is_dpsr(unit)
#ifdef RTCONFIG_DPSTA
				|| is_dpsta(unit)
#endif
#ifdef RTCONFIG_AMAS
				|| (is_router_mode() && !nvram_get_int("x_Setting") && nvram_get_int("amesh_wps_enr"))
#endif
			))
#endif
			)) ? "enabled" : "disabled");

#ifdef RTCONFIG_MULTILAN_MWL
		if (nvram_get_int("w_Setting") && strcmp(prefix, prefix2) && !strcmp(p, prefix2))
			nvram_set(strcat_r(prefix, "wps_mode", tmp), "disabled");
#endif

#if defined(RTCONFIG_DPSTA) && defined(RTCONFIG_BRCM_HOSTAPD)
		if(dpsta_mode() && nvram_get_int("re_mode") == 1) {
			nvram_set(strcat_r(prefix, "assoc_retry_max", tmp), "0");
		}
#endif

#ifdef BCM_BSD
		smart_connect_sync_config(unit);
#endif
#if defined(RTBE96U) || defined(GTBE96) || defined(GTBE98) || defined(GTBE98_PRO)
		smart_connect_sel_ifnames_override();
#endif


			ure = is_ure(unit);
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
			psta = is_psta(unit);
			psr = is_psr(unit);
#endif

			if (ure
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
				|| psta || psr
#endif
			) {
				nvram_set(strcat_r(prefix, "nmode_x", tmp), "0");
#ifdef RTCONFIG_BCMWL6
#ifdef RTCONFIG_AMAS_SYNC_2G_BW
				if ((nvram_get_int("re_mode") == 1 && unit != WL_2G_BAND) ||
					(nvram_get_int("re_mode") == 0))
				{
#endif
				nvram_set(strcat_r(prefix, "bw", tmp), "0");
#ifdef RTCONFIG_AMAS_SYNC_2G_BW
				}
#endif
#else
				nvram_set(strcat_r(prefix, "bw", tmp), "1");
#endif
				nvram_set(strcat_r(prefix, "chanspec", tmp), "0");
#if defined(RTCONFIG_HND_ROUTER_AX) && defined(RTCONFIG_PROXYSTA)
				if (psta ||
					(psr
						&& !(dpsr_mode() && nvram_get_int("re_mode") == 1)
#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_DPSTA)
						&& !(dpsta_mode() && nvram_get_int("re_mode") == 1)
#endif
					)
				) {
					nvram_set_int(strcat_r(prefix, "11ax", tmp), cap_11ax ? 1 : 0);
					nvram_set_int(strcat_r(prefix, "ofdma", tmp), cap_11ax ? 3 : 0);
#ifdef RTCONFIG_WIFI7
					nvram_set_int(strcat_r(prefix, "11be", tmp), cap_11be ? 1 : 0);
#endif
				}
#endif
			}
#ifdef RPAX56
			if(nvram_match("x_Setting", "0") || nvram_match("sku_test", "1")) {
				nvram_set_int(strcat_r(prefix, "11ax", tmp), 1);
				nvram_set_int(strcat_r(prefix, "ofdma", tmp), 2);
				nvram_set_int(strcat_r(prefix, "he_features", tmp), 15);
			}
#endif

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
		/* Disable all VIFS wlX.2 onwards */
		if (is_psta(unit) || is_psr(unit))
		{
			nvram_set(strcat_r(prefix, "bss_enabled", tmp), "1");

			/* Set the wl mode for the virtual interface */
			strncpy(interface_list, nvram_safe_get(lan_ifnames), interface_list_size);
			sprintf(nv_interface, "wl%d.1", unit);
			sprintf(tmp, "wl%d.1_mode", unit);
			if (is_psta(unit) || is_psta_mlo(unit)) {
				/* For Proxy we need to remove our ap interface */
				remove_from_list(nv_interface, interface_list, interface_list_size);
				nvram_set(lan_ifnames, interface_list);

				/* Clear the ap mode */
				nvram_set(tmp, "");
			} else {
				/* For Repeater we need to add our ap interface to the bridged lan */
				add_to_list(nv_interface, interface_list, interface_list_size);
				nvram_set(lan_ifnames, interface_list);

				/* Set the ap mode */
				nvram_set(tmp, "ap");
			}

			for (i = max_no_vifs; i < WL_MAXBSSCFG; i++) {
				sprintf(tmp, "wl%d.%d_bss_enabled", unit, i);
				nvram_set(tmp, "0");
			}
		}
		else {
#if defined(RTBE58_GO)
			if (mediabridge_mode()) {
				if (!nvram_match("force_bss", "1")) {
					printf("%s:: set %sbss_enabled=0\n", __func__, prefix); fflush(stdout);
					nvram_set(strcat_r(prefix, "bss_enabled", tmp), "0");
					//if (nvram_match("unset_mode", "1"))
					//	nvram_set(strcat_r(prefix, "mode", tmp), "");
				}
			} else if (!re_mode()){
				if (!nvram_match(strcat_r(prefix, "radio", tmp), "0") && nvram_match(strcat_r(prefix, "bss_enabled", tmp), "0")) {
					printf("%s:: recover %sbss_enabled=1\n", __func__, prefix); fflush(stdout);
					nvram_set(strcat_r(prefix, "bss_enabled", tmp), "1");
				}
			}
#endif
		}
#if defined(RTCONFIG_PSR_GUEST) && (defined(RTCONFIG_BCM4708) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_HND_ROUTER)) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
		nvram_set(strcat_r(prefix, "ure_mbss", tmp), (nvram_match(strcat_r(prefix, "psr_mbss", tmp2), "1") && (dpsr_mode()
#ifdef RTCONFIG_DPSTA
			|| dpsta_mode()
#endif
			|| rp_mode()
			)) ? "1" : "0");
#else
		nvram_unset(strcat_r(prefix, "ure_mbss", tmp));
#endif
#endif

		memset(interface_list, 0, interface_list_size);
		for (i = 1; i < WL_MAXBSSCFG; i++) {
			sprintf(nv_interface, "wl%d.%d", unit, i);
			add_to_list(nv_interface, interface_list, interface_list_size);
			nvram_set(strcat_r(nv_interface, "_hwaddr", tmp), "");
		}

		reset_mssid_hwaddr(unit);
	}
	else
	{
		snprintf(prefix, sizeof(prefix), "wl%d.%d_", unit, subunit);
		snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
		if (nvram_get_int(strcat_r(prefix2, "nband", tmp)) == 4) { // 6G band
			if (!strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "open")) {
				nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "owe");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
				nvram_set(strcat_r(prefix, "mfp", tmp), "2");
			}
			else if(strlen(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp))) > 0 &&
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "sae") &&
#ifdef RTCONFIG_WPA3_ENTERPRISE
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "wpa3") &&
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "suite-b") &&
#endif
				strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "owe")) {
				nvram_set(strcat_r(prefix, "auth_mode_x", tmp), "sae");
				nvram_set(strcat_r(prefix, "crypto", tmp), "aes");
			}
		}
#endif

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
		if (is_psta(unit) || is_psta_mlo(unit)) {
			nvram_set(strcat_r(prefix, "bss_enabled", tmp), "0");
		} else if (is_psr(unit)) {
			if (subunit == 1)
			{
#if defined(RTCONFIG_AMAS_OB_DISABLE_DSAP)
				if (((nvram_get_int("re_mode") == 1 && nvram_get_int("cfg_first_sync") == 0)
					|| nvram_get_int("re_mode") == 0))
#endif
				{
				nvram_set(strcat_r(prefix, "bss_enabled", tmp), "1");
				}
#ifdef RTCONFIG_AMAS
				if ((unit == WL_2G_BAND) && nvram_match("re_mode", "1") && nvram_match("wps_enable", "1"))
					nvram_set(strcat_r(prefix, "wps_mode", tmp), "enabled");
#endif
			}
			else
#if defined(RTCONFIG_PSR_GUEST) && !defined(RTCONFIG_HND_ROUTER_BE_4916)
			if (!nvram_match(strcat_r(prefix2, "psr_mbss", tmp), "1"))
#elif defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 0)
#endif
			{
				nvram_set(strcat_r(prefix, "bss_enabled", tmp), "0");
			}
		}
#if !defined(RTCONFIG_MULTILAN_MWL)
		if(nvram_match(strcat_r(prefix2, "prev_mode", tmp), "wet") && nvram_match(strcat_r(prefix2, "mode", tmp), "ap")) {
			_dprintf("disable guestnetwork(%s) when changing from client mode to ap.\n", prefix);
			nvram_set(strcat_r(prefix, "bss_enabled", tmp), "0");
		}
#endif
#endif
#ifdef RTCONFIG_FRONTHAUL_DBG
		if(unit == WL_2G_BAND && subunit == nvram_get_int("fh_re_dbg_subunit") && nvram_match("re_mode", "1"))
			set_fh_dbg_config(unit, subunit);
#endif
	}

	// convert wlc_xxx to wlX_ according to wlc_band == unit
#if defined (RTCONFIG_WISP)
	if (is_ure(unit) || is_wisp(unit)) {
#else
	if (is_ure(unit)) {
#endif /* RTCONFIG_WISP */
		if ((nvram_match("x_Setting", "0") && !nvram_match("apply_wlc", "1"))  || !strlen(nvram_safe_get("wlc_ssid"))) {
			if (subunit == -1)
				dbg("Skip wlc profile applying\n");
		}
		else if (subunit == -1) {
			nvram_set(strcat_r(prefix, "ssid", tmp), nvram_safe_get("wlc_ssid"));
			nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_safe_get("wlc_auth_mode"));
			nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get("wlc_wep"));
			if (nvram_get_int("wlc_wep")) {
				nvram_set(strcat_r(prefix, "key", tmp), nvram_safe_get("wlc_key"));
				nvram_set(strcat_r(prefix, "key1", tmp), nvram_safe_get("wlc_wep_key"));
				nvram_set(strcat_r(prefix, "key2", tmp), nvram_safe_get("wlc_wep_key"));
				nvram_set(strcat_r(prefix, "key3", tmp), nvram_safe_get("wlc_wep_key"));
				nvram_set(strcat_r(prefix, "key4", tmp), nvram_safe_get("wlc_wep_key"));
			}
			nvram_set(strcat_r(prefix, "crypto", tmp), nvram_safe_get("wlc_crypto"));
			nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_safe_get("wlc_wpa_psk"));
		} else if (subunit == 1) {
			nvram_set(strcat_r(prefix, "bss_enabled", tmp), "1");
/*
			nvram_set(strcat_r(prefix, "ssid", tmp), nvram_safe_get("wlc_ure_ssid"));
			nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_safe_get("wlc_auth_mode"));
			nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get("wlc_wep"));
			nvram_set(strcat_r(prefix, "key", tmp), nvram_safe_get("wlc_key"));
			nvram_set(strcat_r(prefix, "key1", tmp), nvram_safe_get("wlc_wep_key"));
			nvram_set(strcat_r(prefix, "key2", tmp), nvram_safe_get("wlc_wep_key"));
			nvram_set(strcat_r(prefix, "key3", tmp), nvram_safe_get("wlc_wep_key"));
			nvram_set(strcat_r(prefix, "key4", tmp), nvram_safe_get("wlc_wep_key"));
			nvram_set(strcat_r(prefix, "crypto", tmp), nvram_safe_get("wlc_crypto"));
			nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_safe_get("wlc_wpa_psk"));
			nvram_set(strcat_r(prefix, "bw", tmp), nvram_safe_get("wlc_nbw_cap"));
*/
		}
	}
	// TODO: recover nvram from repeater
	else
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
	if (is_psta(unit) || is_psr(unit)) {
#if defined(RTCONFIG_AMAS)
		if ((dpsta_mode() || dpsr_mode()) && nvram_get_int("re_mode") == 1)
#if defined(GTAXE16000) || defined(GTBE98) || defined(GTBE98_PRO) || defined(BT10) || defined(GT10) || defined(RTAX9000) || defined(BQ16) || defined(BQ16_PRO) || defined(RTBE95U) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			snprintf(prefix2, sizeof(prefix2), "wlc%d_", wlc_idx_align[unit]);
#else
			snprintf(prefix2, sizeof(prefix2), "wlc%d_", unit);
#endif
		else
#endif
		snprintf(prefix2, sizeof(prefix2), "wlc%d_", unit ? 1 : 0);

#if !defined(RTCONFIG_MULTILAN_MWL)
		if (nvram_match("x_Setting", "1") && nvram_match("rpsync", "1") && subunit == -1 && (dpsta_mode()||rp_mode()) && !strlen(nvram_safe_get(strcat_r(prefix2, "ssid", tmp)))) {

			unit2 = find_user_unit("ssid");	
			snprintf(prefix3, sizeof(prefix3), "wlc%d_", unit2);
			//_dprintf("\n==reset %s to be as %s due no configs\n", prefix2, prefix3);

			if((tmp3 = nvram_safe_get(strcat_r(prefix3, "ssid", tmp))) && *tmp3) {	
				nvram_set(strcat_r(prefix2, "ssid", tmp), tmp3);
			} else
				nvram_set(strcat_r(prefix2, "ssid", tmp), "__emptyssid__");
			tmp3 = nvram_safe_get(strcat_r(prefix3, "wpa_psk", tmp));
			nvram_set(strcat_r(prefix2, "wpa_psk", tmp), tmp3);
			tmp3 = nvram_safe_get(strcat_r(prefix3, "crypto", tmp));
			nvram_set(strcat_r(prefix2, "crypto", tmp), tmp3);
		} else
#endif
		if ((nvram_match("x_Setting", "0") && !nvram_match("apply_wlc", "1")) ||
			((dpsr_mode()
#ifdef RTCONFIG_DPSTA
				|| dpsta_mode()
#endif
				) && !strlen(nvram_safe_get(strcat_r(prefix2, "ssid", tmp)))) ||
			(!dpsr_mode() &&
#ifdef RTCONFIG_DPSTA
			!dpsta_mode() &&
#endif
			!rp_mode() &&
				!strlen(nvram_safe_get("wlc_ssid")))) {
			if (subunit == -1) {
				dbg("Skip wlc profile applying\n");
				if (!nvram_match("x_Setting", "0"))
					nvram_set(strcat_r(prefix, "ssid", tmp), "");
			}
		}
		else if (subunit == -1) {
			if (is_dpsr(unit)
#ifdef RTCONFIG_DPSTA
				|| is_dpsta(unit)
#if defined(RPAX56)
				|| is_rp_unit(unit)
#endif
#ifdef RTCONFIG_AMAS
				|| (dpsta_mode() && nvram_get_int("re_mode") == 1)
#endif
#endif
				) {
				struct wlcsuffix_mapping_s *fix_P = NULL;
				for (fix_P = &wlcsuffix_mapping_list[0]; fix_P->name != NULL; fix_P++)
				{
					if (fix_P->wlc_trans_to_wl == 0 || strncmp(fix_P->name, "key", 3) == 0)
						continue;
					if (fix_P->converted_name)
					{
						nvram_pf_set(prefix, fix_P->name, nvram_pf_safe_get(prefix2, fix_P->converted_name));
						if (!strcmp(fix_P->converted_name, "wep") && nvram_pf_get_int(prefix2, fix_P->converted_name)) {
							nvram_pf_set(prefix, "key", nvram_pf_safe_get(prefix2, "key"));
							nvram_pf_set(prefix, "key1", nvram_pf_safe_get(prefix2, "wep_key"));
							nvram_pf_set(prefix, "key2", nvram_pf_safe_get(prefix2, "wep_key"));
							nvram_pf_set(prefix, "key3", nvram_pf_safe_get(prefix2, "wep_key"));
							nvram_pf_set(prefix, "key4", nvram_pf_safe_get(prefix2, "wep_key"));
						}
					}
					else
					{
						nvram_pf_set(prefix, fix_P->name, nvram_pf_safe_get(prefix2, fix_P->name));
					}
				}
#if defined(RTCONFIG_MULTILAN_CFG)
				nvram_set(strcat_r(prefix, "radius_acct_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_ipaddr", tmp2)));
				nvram_set(strcat_r(prefix, "radius_acct_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_key", tmp2)));
				nvram_set(strcat_r(prefix, "radius_acct_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_port", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_ipaddr", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_key", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_port", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_acct_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_ipaddr", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_acct_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_key", tmp2)));
				nvram_set(strcat_r(prefix, "radius2_acct_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_port", tmp2)));
#endif

#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_DPSTA)
				if (dpsta_mode() && nvram_get_int("re_mode") == 1)
				{
#ifdef RTCONFIG_DWB
					if((nvram_get_int("dwb_mode") == 1 || nvram_get_int("dwb_mode") == 3) && nvram_get_int("dwb_band") > 0 && unit == nvram_get_int("dwb_band"))
					{
						dbg("Don't apply wlc1 to wl1.1 or wlc2 to wl2.1, if enabled DWB mode.\n");
					}
					else
#endif
					{
						dbg("prefix_local = %s, prefix2 = %s\n", prefix_local, prefix2 );

						snprintf(prefix_local, sizeof(prefix_local), "wl%d.%d_", unit, 1);
						struct wlcsuffix_mapping_s *fix_P = NULL;
						for (fix_P = &wlcsuffix_mapping_list[0]; fix_P->name != NULL; fix_P++)
						{
							if (fix_P->wlc_trans_to_wl == 0 || strncmp(fix_P->name, "key", 3) == 0)
								continue;
							if (fix_P->converted_name)
							{
								nvram_set(strcat_r(prefix_local, fix_P->name, tmp), nvram_safe_get(strcat_r(prefix2, fix_P->converted_name, tmp2)));
								if (!strcmp(fix_P->converted_name, "wep") && nvram_pf_get_int(prefix2, fix_P->converted_name)) {
									nvram_pf_set(prefix_local, "key", nvram_pf_safe_get(prefix2, "key"));
									nvram_pf_set(prefix_local, "key1", nvram_pf_safe_get(prefix2, "wep_key"));
									nvram_pf_set(prefix_local, "key2", nvram_pf_safe_get(prefix2, "wep_key"));
									nvram_pf_set(prefix_local, "key3", nvram_pf_safe_get(prefix2, "wep_key"));
									nvram_pf_set(prefix_local, "key4", nvram_pf_safe_get(prefix2, "wep_key"));
								}
							}
							else
							{
								if(strstr(fix_P->name, "radius")
#ifdef RTCONFIG_WIFI7
								 || strstr(fix_P->name, "11be")
#endif
								 )
									nvram_set(strcat_r(prefix, fix_P->name, tmp), nvram_safe_get(strcat_r(prefix2, fix_P->name, tmp2)));
								else
									nvram_set(strcat_r(prefix_local, fix_P->name, tmp), nvram_safe_get(strcat_r(prefix2, fix_P->name, tmp2)));
							}
						}
#if defined(RTCONFIG_MULTILAN_CFG)
						nvram_set(strcat_r(prefix, "radius_acct_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_ipaddr", tmp2)));
						nvram_set(strcat_r(prefix, "radius_acct_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_key", tmp2)));
						nvram_set(strcat_r(prefix, "radius_acct_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius_acct_port", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_ipaddr", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_key", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_port", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_acct_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_ipaddr", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_acct_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_key", tmp2)));
						nvram_set(strcat_r(prefix, "radius2_acct_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius2_acct_port", tmp2)));
#endif
					}
				}
#endif
			}
			else
			{
				nvram_set(strcat_r(prefix, "ssid", tmp), nvram_safe_get("wlc_ssid"));
				nvram_set(strcat_r(prefix, "auth_mode_x", tmp), nvram_safe_get("wlc_auth_mode"));
				nvram_set(strcat_r(prefix, "wep_x", tmp), nvram_safe_get("wlc_wep"));
				if (nvram_get_int("wlc_wep")) {
					nvram_set(strcat_r(prefix, "key", tmp), nvram_safe_get("wlc_key"));
					nvram_set(strcat_r(prefix, "key1", tmp), nvram_safe_get("wlc_wep_key"));
					nvram_set(strcat_r(prefix, "key2", tmp), nvram_safe_get("wlc_wep_key"));
					nvram_set(strcat_r(prefix, "key3", tmp), nvram_safe_get("wlc_wep_key"));
					nvram_set(strcat_r(prefix, "key4", tmp), nvram_safe_get("wlc_wep_key"));
				}
				nvram_set(strcat_r(prefix, "crypto", tmp), nvram_safe_get("wlc_crypto"));
				nvram_set(strcat_r(prefix, "wpa_psk", tmp), nvram_safe_get("wlc_wpa_psk"));
			}

#if !defined(RTCONFIG_HND_ROUTER_BE_4916)
			/* early set wlx_vifs for psr mode */
			if (is_psr(unit)
#if defined(RTCONFIG_PSR_GUEST)
				&& !nvram_match(strcat_r(prefix, "psr_mbss", tmp), "1")
#endif
			) {
				sprintf(tmp2, "wl%d.1", unit);
				nvram_set(strcat_r(prefix, "vifs", tmp), tmp2);
			}
#endif
		}

		if (is_psr(unit) && (subunit == 1)) {
			/* TODO: local AP profile */
		}
	}
#endif

	memset(tmp, 0, sizeof(tmp));
	memset(tmp2, 0, sizeof(tmp2));

	wl_apply_akm_by_auth_mode(unit, subunit, NULL);

	if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "radius"))
	{
		nvram_set(strcat_r(prefix, "auth_mode", tmp), "radius");
		nvram_set(strcat_r(prefix, "key", tmp), "2");
	}
	else nvram_set(strcat_r(prefix, "auth_mode", tmp), "none");

	if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "radius"))
		nvram_set(strcat_r(prefix, "wep", tmp), "enabled");
	else if (nvram_invmatch(strcat_r(prefix, "akm", tmp), ""))
		nvram_set(strcat_r(prefix, "wep", tmp), "disabled");
	else if (nvram_get_int(strcat_r(prefix, "wep_x", tmp)) != 0)
		nvram_set(strcat_r(prefix, "wep", tmp), "enabled");
	else nvram_set(strcat_r(prefix, "wep", tmp), "disabled");

	if (nvram_match(strcat_r(prefix, "mode", tmp), "ap") &&
	    strstr(nvram_safe_get(strcat_r(prefix, "akm", tmp2)), "wpa"))
		nvram_set(strcat_r(prefix, "preauth", tmp), "1");
	else
		nvram_set(strcat_r(prefix, "preauth", tmp), "");

	if (!nvram_match(strcat_r(prefix, "macmode", tmp), "disabled")) {

		nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "maclist_x", tmp)));
		list_size = sizeof(char) * (strlen(nv)+1);
		list = (char*) malloc(list_size);
		list2 = (char*) malloc(list_size);
		list[0] = 0;

		if (nv) {
			while ((b = strsep(&nvp, "<")) != NULL) {
				if (strlen(b) == 0) continue;
				if (list[0] == 0)
					sprintf(list, "%s", b);
				else {
					sprintf(list2, "%s %s", list, b);
					strlcpy(list, list2, list_size);
				}
			}
			free(nv);
		}
		nvram_set(strcat_r(prefix, "maclist", tmp), list);
		free(list);
		free(list2);
	}
	else
		nvram_set(strcat_r(prefix, "maclist", tmp), "");

	if (subunit == -1)
	{
#ifdef RTCONFIG_BCM_7114
		/* for old fw(135x) compatibility, and don't use wlc_psta=3 afterwards */
		if (nvram_get_int("wlc_psta") == 3)
			nvram_set("wlc_psta", "1");
#endif
		// wds mode control
		if (is_ure(unit))
		{
			nvram_set(strcat_r(prefix, "mode", tmp), "wet");
		}
#if defined(RTBE58_GO) && !defined(RTCONFIG_BCM_MFG)
		else if (nvram_match("x_Setting", "0"))
		{
			nvram_set(strcat_r(prefix, "mode", tmp), "wet");
		}
#endif
		else
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
		if (is_psta(unit))
		{
#if defined(RTCONFIG_BCM4708) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_HND_ROUTER)
			nvram_set(strcat_r(prefix, "mode", tmp), "wet");
#else
			nvram_set(strcat_r(prefix, "mode", tmp), "psta");
#endif
		}
		else if (is_psr(unit))
		{
#if defined(RTCONFIG_BCM4708) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_HND_ROUTER)
			nvram_set(strcat_r(prefix, "mode", tmp), "wet");
#else
			nvram_set(strcat_r(prefix, "mode", tmp), "psr");
#endif
		}
		else
#endif
		if (nvram_match(strcat_r(prefix, "mode_x", tmp), "1") &&	// wds only
			(is_router_mode() || access_point_mode()))
			nvram_set(strcat_r(prefix, "mode", tmp), "wds");
#if defined (RTCONFIG_WISP)
		else if (is_wisp(unit))
			nvram_set(strcat_r(prefix, "mode", tmp), "sta");
#endif
		else
			nvram_set(strcat_r(prefix, "mode", tmp), "ap");

#if defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_BW160M) || defined(RTCONFIG_BW320M)
		if (!nvram_match(strcat_r(prefix, "mode", tmp), "ap") &&
			!nvram_match(strcat_r(prefix, "mode", tmp), "wds")) {
#if defined(RTCONFIG_AMAS)
			if (nvram_get_int("re_mode") == 0
#if defined(RTCONFIG_PROXYSTA)
				&& !dpsr_mode()
#endif
#if defined(RTCONFIG_DPSTA)
				&& !dpsta_mode()
#endif
			)
#endif
#if defined(RTCONFIG_BW320M)
			nvram_set(strcat_r(prefix, "bw_320", tmp), !nvram_match(strcat_r(prefix, "nband", tmp2), "4") ? "0" : "1");
#else
			nvram_set(strcat_r(prefix, "bw_160", tmp), nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? "0" : "1");
#endif
		}
#endif

#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
#ifdef RTCONFIG_BCMARM
		int dwds = 0;
		if (!nvram_get_int("dwds_ctrl")) {
#if defined(RTCONFIG_BCM4708) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_HND_ROUTER)
#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58) || defined(RTBE58_GO)
			dwds = !client_mode();
#else
			dwds = 1;
#endif
#else
			dwds = !(is_ure(unit) || is_psta(unit));
#endif
			nvram_set(strcat_r(prefix, "dwds", tmp), dwds ? "1" : "0");
		}
#endif
		if (!nvram_get_int("psr_mrpt_ctrl")) {
			nvram_set(strcat_r(prefix, "psr_mrpt", tmp), is_psr(unit) ? "1" : "0");
#if defined(RPAX58) || defined(RPBE58)
			if(client_mode() == 1)
				nvram_set(strcat_r(prefix, "psr_mrpt", tmp), "0");
#endif
		}
#endif

		// TODO use lazwds directly
		//if (!nvram_match(strcat_r(prefix, "wdsmode_ex", tmp), "2"))
		//	nvram_set(strcat_r(prefix, "lazywds", tmp), "1");
		//else nvram_set(strcat_r(prefix, "lazywds", tmp), "0");

		// TODO need sw_mode ?
		// handle wireless wds list
		if ((nvram_match(strcat_r(prefix, "mode", tmp), "ap")
		  || nvram_match(strcat_r(prefix, "mode", tmp), "wds"))
			&& !nvram_match(strcat_r(prefix, "mode_x", tmp2), "0")) {
			if (nvram_match(strcat_r(prefix, "wdsapply_x", tmp), "1")) {
				nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "wdslist", tmp)));
				list_size = sizeof(char) * (strlen(nv)+1);
				list = (char*) malloc(list_size);
				list2 = (char*) malloc(list_size);
				list[0] = 0;

				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if (strlen(b) == 0) continue;
						if (list[0] == 0)
							sprintf(list, "%s", b);
						else {
							sprintf(list2, "%s %s", list, b);
							strlcpy(list, list2, list_size);
						}
					}
					free(nv);
				}
				nvram_set(strcat_r(prefix, "wds", tmp), list);
				nvram_set(strcat_r(prefix, "lazywds", tmp), "0");
				free(list);
				free(list2);
			}
			else {
				nvram_set(strcat_r(prefix, "wds", tmp), "");
				nvram_set(strcat_r(prefix, "lazywds", tmp), "1");
			}
		} else {
			nvram_set(strcat_r(prefix, "wds", tmp), "");
			nvram_set(strcat_r(prefix, "lazywds", tmp), "0");
		}

		switch (nvram_get_int(strcat_r(prefix, "mrate_x", tmp))) {
		case 0: /* Auto */
			mcast_rate = 0;
			break;
		case 1: /* Legacy CCK 1Mbps */
			mcast_rate = 1000000;
			break;
		case 2: /* Legacy CCK 2Mbps */
			mcast_rate = 2000000;
			break;
		case 3: /* Legacy CCK 5.5Mbps */
			mcast_rate = 5500000;
			break;
		case 4: /* Legacy OFDM 6Mbps */
			mcast_rate = 6000000;
			break;
		case 5: /* Legacy OFDM 9Mbps */
			mcast_rate = 9000000;
			break;
		case 6: /* Legacy CCK 11Mbps */
			mcast_rate = 11000000;
			break;
		case 7: /* Legacy OFDM 12Mbps */
			mcast_rate = 12000000;
			break;
		case 8: /* Legacy OFDM 18Mbps */
			mcast_rate = 18000000;
			break;
		case 9: /* Legacy OFDM 24Mbps */
			mcast_rate = 24000000;
			break;
		case 10: /* Legacy OFDM 36Mbps */
			mcast_rate = 36000000;
			break;
		case 11: /* Legacy OFDM 48Mbps */
			mcast_rate = 48000000;
			break;
		case 12: /* Legacy OFDM 54Mbps */
			mcast_rate = 54000000;
			break;
		default: /* Auto */
			mcast_rate = 0;
			break;
		}
		nvram_set_int(strcat_r(prefix, "mrate", tmp), mcast_rate);

		if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "0"))		// auto => b/g/n mixed or a/n(/ac)(/ax) mixed
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "1");
#ifdef RTCONFIG_BCM_7114
			nvram_set(strcat_r(prefix, "gmode", tmp), "1");
#else
			nvram_set(strcat_r(prefix, "gmode", tmp), nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? "1" : "-1");	// 1: 54g Auto, 4: 4g Performance, 5: 54g LRS, 0: 802.11b Only
#endif
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 0;				// no requirements on joining devices
#endif
		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "1"))	// n only
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "1");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? "1" : "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 2;				// devices must advertise HT (11n) capabilities to be allowed to associate
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "4"))	// g/n mixed or a/n mixed
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? "1" : "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))
				bss_opmode_cap_reqd = 1;			// devices must advertise ERP (11g) capabilities to be allowed to associate
			else
				bss_opmode_cap_reqd = 0;			// no requirements on joining devices
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "5"))	// g only
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-2");	// legacy rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "0");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), "2");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 1;				// devices must advertise ERP (11g) capabilities to be allowed to associate
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "6"))	// b only
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-2");	// legacy rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "0");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), "0");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 0;				// no requirements on joining devices
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "7"))	// a only
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-2");	// legacy rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "0");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 0;				// no requirements on joining devices
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
#ifdef RTCONFIG_BCMWL6
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "2"))	// b/g mixed
#else
		else								// b/g mixed
#endif
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-2");	// legacy rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "0");
#ifndef RTCONFIG_BCMWL6
			nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
			nvram_set(strcat_r(prefix, "vreqd", tmp), "0");
			nvram_set(strcat_r(prefix, "gmode", tmp), "1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
#ifdef RTCONFIG_BCMWL6
			bss_opmode_cap_reqd = 0;				// no requirements on joining devices
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif
		}
#ifdef RTCONFIG_BCMWL6
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "3") && 	// ac only
			 nvram_match(strcat_r(prefix, "nband", tmp2), "1"))
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "vreqd", tmp), "1");
			nvram_set(strcat_r(prefix, "gmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
			bss_opmode_cap_reqd = 3;				// devices must advertise VHT (11ac) capabilities to be allowed to associate
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "11ax", tmp), "0");
#endif

		}
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "8"))	// n/ac/ax mixed
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "vreqd", tmp), "1");
			nvram_set(strcat_r(prefix, "gmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
			bss_opmode_cap_reqd = 2;				// devices must advertise HT (11n) capabilities to be allowed to associate
		}
#ifdef RTCONFIG_HND_ROUTER_AX
		else if (nvram_match(strcat_r(prefix, "nmode_x", tmp), "9"))	// ax only
		{
			nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-1");	// auto rate
			nvram_set(strcat_r(prefix, "nmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "vreqd", tmp), "1");
			nvram_set(strcat_r(prefix, "gmode", tmp), "-1");
			nvram_set(strcat_r(prefix, "rate", tmp), "0");
			bss_opmode_cap_reqd = 4;				// devices must advertise HE (11ax) capabilities to be allowed to associate
		}
#endif
#endif

#ifdef BCM_BSD
		if (nvram_get_int("smart_connect_x") && get_bsd_nonvht_status(unit) && (bss_opmode_cap_reqd < 3)) {
/* drop the implication for smart connect not working on unit 2 now
#ifdef RTAX92U
			if (unit == 2 && nvram_match(strcat_r(prefix, "he_features", tmp), "15"))
				bss_opmode_cap_reqd = 4;			// devices must advertise HE (11ax) capabilities to be allowed to associate
			else
#endif
*/
			bss_opmode_cap_reqd = 3;				// devices must advertise VHT (11ac) capabilities to be allowed to associate
		}
#endif

		nvram_set_int(strcat_r(prefix, "bss_opmode_cap_reqd", tmp), bss_opmode_cap_reqd);

#ifdef BCM_BSD
		if (nvram_get_int("smart_connect_x") && get_bsd_nonvht_status(unit) && (bss_opmode_cap_reqd < 3))
			bss_opmode_cap_reqd = 3;				// devices must advertise VHT (11ac) capabilities to be allowed to associate
#endif
#ifdef RTCONFIG_BCMWL6
		nvram_set_int(strcat_r(prefix, "bss_opmode_cap_reqd", tmp), bss_opmode_cap_reqd);
#endif
		if (nvram_match(strcat_r(prefix, "nband", tmp), "2") &&
			nvram_match(strcat_r(prefix, "nmode", tmp2), "-1"))
			nvram_set(strcat_r(prefix, "gmode_protection", tmp), "auto");
#ifdef RTCONFIG_BCMWL6
		if (nvram_match(strcat_r(prefix, "bw", tmp), "0"))			// Auto
		{
			if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))		// 2.4G
			{
				if (nvram_match(strcat_r(prefix, "nmode", tmp), "-1"))
					nvram_set(strcat_r(prefix, "bw_cap", tmp), "3");// 40M
				else
					nvram_set(strcat_r(prefix, "bw_cap", tmp), "1");// 20M
			}
			else
			{
#if defined(RTCONFIG_BCMARM) || defined(RTAC66U)
				if (nvram_match(strcat_r(prefix, "vreqd", tmp2), "1")
					&& nvram_match(strcat_r(prefix, "phytype", tmp), "v")
				) {
#if defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_BW160M) || defined(RTCONFIG_BW320M)
					if (nvram_match(strcat_r(prefix, "bw_320", tmp), "1"))
						nvram_set(strcat_r(prefix, "bw_cap", tmp), "31");    // 320M
					else if (nvram_match(strcat_r(prefix, "bw_160", tmp), "1"))
						nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? (hw_he_cap(unit) ? "15" : "7") : "3");	// 160M
					else if (is_psta(unit) || is_psr(unit)) {
#if defined(RTCONFIG_BW320M)
						if(cap_320m)
							nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? (hw_he_cap(unit) ? "31" : "7") : "3");// 320M
						else
#endif
							nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? (hw_he_cap(unit) ? "15" : "7") : "3");// 160M
					}
					else
#endif
					nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? "7" : "3");					// 80M
				} else
#endif
				if (nvram_match(strcat_r(prefix, "nmode", tmp), "-1"))
					nvram_set(strcat_r(prefix, "bw_cap", tmp), "3");// 40M
				else
					nvram_set(strcat_r(prefix, "bw_cap", tmp), "1");// 20M
			}
#if defined(RPAX58) || defined(RPBE58)
			nvram_set_int(strcat_r(prefix, "obss_coex", tmp),
				nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? nvram_match("no_coex", "1") ? 0 : nvram_match("no_coex", "0") ? 1 : 0 : 0);
#else
			nvram_set_int(strcat_r(prefix, "obss_coex", tmp),
#ifdef RTBE86U
				nvram_match(strcat_r(prefix, "nband", tmp2), "2") && !nvram_match("location_code", "XX") ? 1 : 0);
#else
				nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? 1 : 0);
#endif
#endif
		}
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "1") ||	// 20M
			 nvram_match(strcat_r(prefix, "nmcsidx", tmp2), "-2"))
		{
			nvram_set(strcat_r(prefix, "bw_cap", tmp), "1");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "2") &&	// 40M
			 nvram_match(strcat_r(prefix, "nmode", tmp2), "-1"))
		{
			nvram_set(strcat_r(prefix, "bw_cap", tmp), "3");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
#if defined(RTCONFIG_BCMARM) || defined(RTAC66U)
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "3") &&	// 80M
			 nvram_match(strcat_r(prefix, "vreqd", tmp2), "1"))
		{
			if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))	// 2.4G
				nvram_set(strcat_r(prefix, "bw_cap", tmp), "3");
			else
				nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? "7" : "3");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
#endif
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "4"))		// 80+80M
		{
			//TBD
		}
#if defined(RTCONFIG_HND_ROUTER_AX) || defined(RTCONFIG_BW160M)
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "5") &&	// 160M
			nvram_match(strcat_r(prefix, "vreqd", tmp2), "1"))
		{
			if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))	// 2.4G
				nvram_set(strcat_r(prefix, "bw_cap", tmp), "3");
			else
				nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? (hw_he_cap(unit) ? "15" : "7") : "3");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
#endif
#if defined(RTCONFIG_BW320M)
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "6") &&	// 320M
			 nvram_match(strcat_r(prefix, "nband", tmp), "4"))	// 6GHz
		{
			nvram_set(strcat_r(prefix, "bw_cap", tmp), hw_vht_cap() ? (hw_he_cap(unit) ? "31" : "7") : "3");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
#endif
		else
		{
			nvram_set(strcat_r(prefix, "bw_cap", tmp), "1");
			nvram_set_int(strcat_r(prefix, "obss_coex", tmp),
				nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? 1 : 0);
		}

#if defined(RTCONFIG_BCM_7114) || defined(HND_ROUTER)
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		if (nvram_match(strcat_r(prefix, "nmode", tmp), "0"))
		{
			dbG("set vhtmode -1\n");
			nvram_set(strcat_r(prefix, "vht_features", tmp), "-1");
			nvram_set(strcat_r(prefix, "vhtmode", tmp), "-1");
		}
		else if (hw_vht_cap() &&
			nvram_match(strcat_r(prefix, "vreqd", tmp), "1") &&
			nvram_get_int(strcat_r(prefix, "turbo_qam", tmp2)))
		{
			if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))
			{
				if (nvram_match(strcat_r(prefix, "turbo_qam", tmp), "1"))
#ifdef RTCONFIG_HND_ROUTER_AX
					nvram_set(strcat_r(prefix, "vht_features", tmp), nvram_match("vht_adv_2g", "1") ? "11" : "3");
#else
					nvram_set(strcat_r(prefix, "vht_features", tmp), "3");
#endif
				else if (nvram_match(strcat_r(prefix, "turbo_qam", tmp), "2"))
#ifdef RTCONFIG_HND_ROUTER_AX
					nvram_set(strcat_r(prefix, "vht_features", tmp), nvram_match("vht_adv_2g", "1") ? "15" : "7");
#else
					nvram_set(strcat_r(prefix, "vht_features", tmp), "7");
#endif
			}
			else if (nvram_match(strcat_r(prefix, "nband", tmp), "1")) {
				if (nvram_match(strcat_r(prefix, "turbo_qam", tmp), "2"))
#ifndef HND_ROUTER
					nvram_set(strcat_r(prefix, "vht_features", tmp), "4");
#else
					nvram_set(strcat_r(prefix, "vht_features", tmp), "6");
#endif
				else
					nvram_set(strcat_r(prefix, "vht_features", tmp), "2");
			}
			dbG("set vhtmode 1\n");
			nvram_set(strcat_r(prefix, "vhtmode", tmp), "1");
		}
		else
		{
			dbG("set vhtmode 0\n");
#if defined(RTAX92U)
			nvram_set(strcat_r(prefix, "vht_features", tmp), (unit == 0 || unit == 1) ? "0" : "35");
#else
			nvram_set(strcat_r(prefix, "vht_features", tmp), nvram_match(strcat_r(prefix, "nband", tmp2), "2") ? "35": "34");
#endif
			nvram_set(strcat_r(prefix, "vhtmode", tmp), "0");
		}
#endif

#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58)
		if(nvram_match("vht_def", "1")) {
			nvram_set(strcat_r(prefix, "vht_features", tmp), "-1");
			nvram_set(strcat_r(prefix, "vhtmode", tmp), "-1");
		}
#endif

#if defined(RTCONFIG_WIFI7)
		if (cap_11be && nvram_match(strcat_r(prefix, "11be", tmp), "1"))
		{
			eht_feats = 0x7;	// Enable EHT upon 6G/5G/2.4G
			if (strlen(nvram_safe_get(strcat_r(prefix, "be_ofdma", tmp))) != 0 ||
			    strlen(nvram_safe_get(strcat_r(prefix, "be_mumimo", tmp))) != 0) {
				if (nvram_match(strcat_r(prefix, "be_ofdma", tmp), "1"))		// Enable EHT DL OFDMA
					eht_feats |= 0x8;
				else if (nvram_match(strcat_r(prefix, "be_ofdma", tmp), "2"))		// Enable EHT UL OFDMA
					eht_feats |= 0x10;
				else if (nvram_match(strcat_r(prefix, "be_ofdma", tmp), "3"))		// Enable EHT DL+UL OFDMA
					eht_feats |= 0x18;

				if (nvram_match(strcat_r(prefix, "be_mumimo", tmp), "1"))		// Enable EHT DL MU-MIMO
					eht_feats |= 0x20;
				else if (nvram_match(strcat_r(prefix, "be_mumimo", tmp), "2"))		// Enable EHT UL MU-MIMO
					eht_feats |= 0x40;
				else if (nvram_match(strcat_r(prefix, "be_mumimo", tmp), "3"))		// Enable EHT DL+UL MU-MIMO
					eht_feats |= 0x60;
			}
			else {
				if (nvram_match(strcat_r(prefix, "ofdma", tmp), "1"))		// Enable EHT DL OFDMA
					eht_feats |= 0x8;
				else if (nvram_match(strcat_r(prefix, "ofdma", tmp), "2"))	// Enable EHT DL/UL OFDMA
					eht_feats |= 0x18;
				else if (nvram_match(strcat_r(prefix, "ofdma", tmp), "3"))	// Enable EHT DL/UL OFDMA+MU-MIMO
					eht_feats |= 0x78;
			}
			nvram_set_int(strcat_r(prefix, "eht_features", tmp), eht_feats);
		}
		else
			nvram_set(strcat_r(prefix, "eht_features", tmp), "0");	// Disable EHT
#endif

#ifdef RTCONFIG_HND_ROUTER_AX
		if (cap_11ax && nvram_match(strcat_r(prefix, "11ax", tmp), "1"))
		{
			if (nvram_match(strcat_r(prefix, "ofdma", tmp), "1"))		// DL OFDMA Only
				nvram_set(strcat_r(prefix, "he_features", tmp), "7");
			else if (nvram_match(strcat_r(prefix, "ofdma", tmp), "2"))	// DL+UL OFDMA
				nvram_set(strcat_r(prefix, "he_features", tmp), "15");
			else if (nvram_match(strcat_r(prefix, "ofdma", tmp), "3"))	// DL+UL OFDMA + HE MU-MIMO
				nvram_set(strcat_r(prefix, "he_features", tmp), "31");
			else								// Disable OFDMA
				nvram_set(strcat_r(prefix, "he_features", tmp), "3");
		}
		else
			nvram_set(strcat_r(prefix, "he_features", tmp), "0");
#endif

		if (nvram_match(strcat_r(prefix, "txbf", tmp), "1"))
		{
#ifdef RTCONFIG_HND_ROUTER_AX
			ax_en = nvram_match(strcat_r(prefix, "11ax", tmp), "1") && cap_11ax;
#endif
#ifdef RTCONFIG_MUMIMO
			if (nvram_match(strcat_r(prefix, "mumimo", tmp), "1"))
			{
#ifdef RTCONFIG_WIFI7
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "255");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "111");
#elif defined(RTCONFIG_HND_ROUTER_AX)
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), ax_en ? "31" : "3");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), ax_en ? "15" : "3");
				adjust_txbf_bfe_cap(unit, ax_en ? "15" : "3");
#else
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "2");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "2");
				adjust_txbf_bfe_cap(unit, "2");
#endif // RTCONFIG_WIFI7
			} else {
#endif // RTCONFIG_MUMIMO

#ifdef RTCONFIG_WIFI7
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "37");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "37");
#elif defined(RTCONFIG_HND_ROUTER_AX)
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), ax_en ? "21" : "1");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), ax_en ? "5" : "1");
				adjust_txbf_bfe_cap(unit, ax_en ? "5" : "1");
#else
				nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "1");
				nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "1");
				adjust_txbf_bfe_cap(unit, "1");
#endif				
#ifdef RTCONFIG_MUMIMO
			}
#endif
		}
		else
		{
			nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "0");
			nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "0");
			adjust_txbf_bfe_cap(unit, "0");
		}

#ifdef RTCONFIG_MUMIMO
		/* mu_feature is not enabled for client modes. */
		if (nvram_match(strcat_r(prefix, "mumimo", tmp), "1")
#if 0
			&& !is_ure(unit)
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
			&& !(is_psta(unit) || is_psr(unit))
#endif
#endif
		)
#ifdef RTCONFIG_HND_ROUTER_AX
			nvram_set(strcat_r(prefix, "mu_features", tmp), "1");
#else
			nvram_set(strcat_r(prefix, "mu_features", tmp), "0x8000");
#endif
		else
			nvram_set(strcat_r(prefix, "mu_features", tmp), "0");
#endif

#ifdef RTCONFIG_BCMARM
		nvram_set(strcat_r(prefix, "txbf_imp", tmp), nvram_safe_get(strcat_r(prefix, "itxbf", tmp2)));
#endif
#else
		if (nvram_match(strcat_r(prefix, "bw", tmp), "1"))
		{
			nvram_set(strcat_r(prefix, "nbw_cap", tmp), "1");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "1");
		}
		else if (nvram_match(strcat_r(prefix, "bw", tmp), "2"))
		{
			nvram_set(strcat_r(prefix, "nbw_cap", tmp), "1");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "0");
		}
		else
		{
			nvram_set(strcat_r(prefix, "nbw_cap", tmp), "0");
			nvram_set(strcat_r(prefix, "obss_coex", tmp), "1");
		}

		if (unit != WL_2G_BAND) chanspec_fix_5g(unit);
#ifdef RTCONFIG_WL_AUTO_CHANNEL
		else if (nvram_match("AUTO_CHANNEL", "1"))
			nvram_set("wl0_channel", "6");
#endif

		match = 0;
		foreach (word, "lower upper", next)
		{
			if (nvram_match(strcat_r(prefix, "nctrlsb", tmp), word))
			{
				match = 1;
				break;
			}
		}
/*
		if ((nvram_match(strcat_r(prefix, "channel", tmp), "0")))
			nvram_unset(strcat_r(prefix, "nctrlsb", tmp));
		else
*/
		if (!match)
			nvram_set(strcat_r(prefix, "nctrlsb", tmp), "lower");
#endif

		wl_dfs_support(unit);

#ifdef RTCONFIG_EMF
		/* Wireless IGMP Snooping */
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
		i = nvram_get_int(strcat_r(prefix, "igs", tmp)) || is_psta(unit) || is_psr(unit);
#else
		i = nvram_get_int(strcat_r(prefix, "igs", tmp));
#endif
		nvram_set_int(strcat_r(prefix, "wmf_bss_enable", tmp), i ? 1 : 0);
#ifdef RTCONFIG_BCMWL6
		nvram_set_int(strcat_r(prefix, "wmf_ucigmp_query", tmp), 1);
		nvram_set_int(strcat_r(prefix, "wmf_mdata_sendup", tmp), 1);
#ifdef RTCONFIG_BCMARM
		nvram_set_int(strcat_r(prefix, "wmf_ucast_upnp", tmp), 0);
		nvram_set_int(strcat_r(prefix, "wmf_igmpq_filter", tmp), 1);
#endif
		nvram_set_int(strcat_r(prefix, "acs_fcs_mode", tmp), i && !nvram_match(strcat_r(prefix, "reg_mode", tmp2), "h") ? 1 : 0);
		nvram_set_int(strcat_r(prefix, "dcs_csa_unicast", tmp), i ? 1 : 0);
#endif
#else // RTCONFIG_EMF
		nvram_set_int(strcat_r(prefix, "wmf_bss_enable", tmp), 0);
#ifdef RTCONFIG_BCMWL6
		nvram_set_int(strcat_r(prefix, "wmf_ucigmp_query", tmp), 0);
		nvram_set_int(strcat_r(prefix, "wmf_mdata_sendup", tmp), 0);
#ifdef RTCONFIG_BCMARM
		nvram_set_int(strcat_r(prefix, "wmf_ucast_upnp", tmp), 0);
		nvram_set_int(strcat_r(prefix, "wmf_igmpq_filter", tmp), 0);
#endif
		nvram_set_int(strcat_r(prefix, "acs_fcs_mode", tmp), 0);
		nvram_set_int(strcat_r(prefix, "dcs_csa_unicast", tmp), 0);
#endif
#endif // RTCONFIG_EMF

#ifdef RTCONIFG_HND_ROUTER_AX
		if(!nvram_match("acsd_ctrl", "1")) {
		    nvram_set_int(strcat_r(prefix, "acs_ignore_txfail", tmp), 1);
		    nvram_set_int(strcat_r(prefix, "acs_bgdfs_enab", tmp), 0);
		    nvram_set_int(strcat_r(prefix, "acs_bgdfs_ahead", tmp), 0);
		}
#endif

		/* always enable WMM for N mode */
		if (nvram_match(strcat_r(prefix, "nmode", tmp), "-1"))
			nvram_set(strcat_r(prefix, "wme", tmp), "on");

		sprintf(tmp2, "%d", nvram_get_int(strcat_r(prefix, "pmk_cache", tmp)) * 60);
		nvram_set(strcat_r(prefix, "net_reauth", tmp), tmp2);

		wl_dfs_support(unit);
#if 0
#if defined(RTCONFIG_BCM_7114) || defined(GTAC5300) || defined(GTAX11000) || defined(RTAX92U)
		wl_CE_support(unit);
#endif
#endif

#if defined(RTCONFIG_BCM4708) || defined(RTCONFIG_BCM7) || defined(RTCONFIG_BCM_7114) || (defined(HND_ROUTER) && !defined(RTCONFIG_HND_ROUTER_AX))
		if (nvram_get_int("smart_connect_x"))
			nvram_set_int(strcat_r(prefix, "probresp_sw", tmp), 1);
		else
			nvram_set_int(strcat_r(prefix, "probresp_sw", tmp), 0);
#endif

#ifdef RTCONFIG_BCMARM
#ifdef RTCONFIG_HND_ROUTER_AX
		if (nvram_match(strcat_r(prefix, "akm", tmp), "sae"))
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
		else
#endif
#ifdef RTCONFIG_WIFI7
		if (nvram_match(strcat_r(prefix, "akm", tmp), "sae sae-ext"))
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
		else
#endif
		if (is_ure(unit)
#if defined(RTCONFIG_BCMWL6) && defined(RTCONFIG_PROXYSTA)
			|| is_psta(unit) || is_psr(unit)
#endif
		)
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 1);
#endif

		dbG("bw: %s\n", nvram_safe_get(strcat_r(prefix, "bw", tmp)));
#ifdef RTCONFIG_BCMWL6
		dbG("chanspec: %s\n", nvram_safe_get(strcat_r(prefix, "chanspec", tmp)));
		dbG("bw_cap: %s\n", nvram_safe_get(strcat_r(prefix, "bw_cap", tmp)));
#else
		dbG("channel: %s\n", nvram_safe_get(strcat_r(prefix, "channel", tmp)));
		dbG("nbw_cap: %s\n", nvram_safe_get(strcat_r(prefix, "nbw_cap", tmp)));
		dbG("nctrlsb: %s\n", nvram_safe_get(strcat_r(prefix, "nctrlsb", tmp)));
#endif
		dbG("obss_coex: %s\n", nvram_safe_get(strcat_r(prefix, "obss_coex", tmp)));
	}
	else
	{
		if (nvram_match(strcat_r(prefix, "bss_enabled", tmp), "1"))
		{
			snprintf(prefix2, sizeof(prefix2), "wl%d_", unit);

			nvram_set(strcat_r(prefix, "preauth", tmp), nvram_safe_get(strcat_r(prefix2, "preauth", tmp2)));
			nvram_set(strcat_r(prefix, "bss_maxassoc", tmp), nvram_safe_get(strcat_r(prefix2, "bss_maxassoc", tmp2)));

#if !defined(RTCONFIG_AMAS_WGN) && !defined(RTCONFIG_MULTILAN_CFG)
//			/* if amazon_wss is disabled, we need to sync ap_isolate from primary interface */
//			if (!amazon_wss_ap_isolate_support(prefix))
// 			Do not override guest settings for ap isolate
//			nvram_set(strcat_r(prefix, "ap_isolate", tmp), nvram_safe_get(strcat_r(prefix2, "ap_isolate", tmp2)));
#endif	/* !RTCONFIG_AMAS_WGN */

			nvram_set(strcat_r(prefix, "net_reauth", tmp), nvram_safe_get(strcat_r(prefix2, "net_reauth", tmp2)));
#if !defined(RTCONFIG_AMAS_ENT_FRONTHAUL) && !defined(RTCONFIG_MULTILAN_CFG)
			nvram_set(strcat_r(prefix, "radius_ipaddr", tmp), nvram_safe_get(strcat_r(prefix2, "radius_ipaddr", tmp2)));
			nvram_set(strcat_r(prefix, "radius_key", tmp), nvram_safe_get(strcat_r(prefix2, "radius_key", tmp2)));
			nvram_set(strcat_r(prefix, "radius_port", tmp), nvram_safe_get(strcat_r(prefix2, "radius_port", tmp2)));
#endif
			nvram_set(strcat_r(prefix, "wpa_gtk_rekey", tmp), nvram_safe_get(strcat_r(prefix2, "wpa_gtk_rekey", tmp2)));
			nvram_set(strcat_r(prefix, "wme", tmp), nvram_safe_get(strcat_r(prefix2, "wme", tmp2)));
			nvram_set(strcat_r(prefix, "wme_bss_disable", tmp), nvram_safe_get(strcat_r(prefix2, "wme_bss_disable", tmp2)));

			nvram_set(strcat_r(prefix, "wmf_bss_enable", tmp), nvram_safe_get(strcat_r(prefix2, "wmf_bss_enable", tmp2)));
			nvram_set(strcat_r(prefix, "wmf_psta_disable", tmp), nvram_safe_get(strcat_r(prefix2, "wmf_psta_disable", tmp2)));
			nvram_set(strcat_r(prefix, "mcast_regen_bss_enable", tmp), nvram_safe_get(strcat_r(prefix2, "mcast_regen_bss_enable", tmp2)));

			nvram_set(strcat_r(prefix, "rxchain_pwrsave_enable", tmp), nvram_safe_get(strcat_r(prefix2, "rxchain_pwrsave_enable", tmp2)));
			nvram_set(strcat_r(prefix, "rxchain_pwrsave_quiet_time", tmp), nvram_safe_get(strcat_r(prefix2, "rxchain_pwrsave_quiet_time", tmp2)));
			nvram_set(strcat_r(prefix, "rxchain_pwrsave_pps", tmp), nvram_safe_get(strcat_r(prefix2, "rxchain_pwrsave_pps", tmp2)));
			nvram_set(strcat_r(prefix, "rxchain_pwrsave_stas_assoc_check", tmp), nvram_safe_get(strcat_r(prefix2, "rxchain_pwrsave_stas_assoc_check", tmp2)));

			nvram_set(strcat_r(prefix, "radio_pwrsave_enable", tmp), nvram_safe_get(strcat_r(prefix2, "radio_pwrsave_enable", tmp2)));
			nvram_set(strcat_r(prefix, "radio_pwrsave_quiet_time", tmp), nvram_safe_get(strcat_r(prefix2, "radio_pwrsave_quiet_time", tmp2)));
			nvram_set(strcat_r(prefix, "radio_pwrsave_pps", tmp), nvram_safe_get(strcat_r(prefix2, "radio_pwrsave_pps", tmp2)));
			nvram_set(strcat_r(prefix, "radio_pwrsave_level", tmp), nvram_safe_get(strcat_r(prefix2, "radio_pwrsave_level", tmp2)));
			nvram_set(strcat_r(prefix, "radio_pwrsave_stas_assoc_check", tmp), nvram_safe_get(strcat_r(prefix2, "radio_pwrsave_stas_assoc_check", tmp2)));

			if (!nvram_match(strcat_r(prefix, "macmode", tmp), "disabled") &&
				nvram_match(strcat_r(prefix, "mode", tmp2), "ap")) {
				nv = nvp = strdup(nvram_safe_get(strcat_r(prefix, "maclist_x", tmp)));
				list_size = sizeof(char) * (strlen(nv)+1);
				list = (char*) malloc(list_size);
				list2 = (char*) malloc(list_size);
				list[0] = 0;

				if (nv) {
					while ((b = strsep(&nvp, "<")) != NULL) {
						if (strlen(b) == 0) continue;
						if (list[0] == 0)
							sprintf(list, "%s", b);
						else {
							sprintf(list2, "%s %s", list, b);
							strlcpy(list, list2, list_size);
						}
					}
					free(nv);
				}
				nvram_set(strcat_r(prefix, "maclist", tmp), list);
				free(list);
				free(list2);
			}
			else
				nvram_set(strcat_r(prefix, "maclist", tmp), "");

#ifdef RTCONFIG_BCMARM
			if (!nvram_get_int("dwds_ctrl")) 
#ifdef RTCONFIG_HND_ROUTER_AX
#if defined(RPAX56) || defined(RPAX58) || defined(RPBE58) || defined(RTBE58_GO)
			{
				nvram_set(strcat_r(prefix, "dwds", tmp), client_mode()? "0": "1");
			}
#else
				nvram_set(strcat_r(prefix, "dwds", tmp), "1");
#endif
#else
				nvram_set(strcat_r(prefix, "dwds", tmp), is_ure(unit) ? "0" : "1");
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
			if (nvram_match(strcat_r(prefix, "akm", tmp), "sae"))
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 2); // Set PMF as required for WPA3-SAE
			else if (nvram_match(strcat_r(prefix, "akm", tmp), "psk2 sae"))
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 1);
#ifdef RTCONFIG_WPA3_ENTERPRISE
			else if (nvram_match(strcat_r(prefix, "akm", tmp), "suite-b")) // Set PMF as required for WPA3 suite-b 192bit mode
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
			else if (nvram_match(strcat_r(prefix, "akm", tmp), "wpa3")) // Set PMF as required for WPA3-enterprise (WPA3 EAP)
                                nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
                        else if (nvram_match(strcat_r(prefix, "akm", tmp), "wpa2 wpa3")) // Set PMF as required for WPA3-enterprise transition mode
                                nvram_set_int(strcat_r(prefix, "mfp", tmp), 1);
#endif
#ifdef RTCONFIG_WIFI7
			else if (nvram_match(strcat_r(prefix, "akm", tmp), "psk2 sae sae-ext"))
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 1);
			else if (nvram_match(strcat_r(prefix, "akm", tmp), "sae sae-ext"))
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
#endif
			else
#endif
				nvram_set_int(strcat_r(prefix, "mfp", tmp), 0);
#endif
		}
		else
		{
			nvram_set(strcat_r(prefix, "macmode", tmp), "disabled");
			nvram_set(strcat_r(prefix, "maclist", tmp), "");
		}
#ifdef RTCONFIG_WIFI7
		if (nvram_match(strcat_r(prefix2, "txbf", tmp), "1"))
		{
			nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "255");
			nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "111");
		}
		else
		{
			nvram_set(strcat_r(prefix, "txbf_bfr_cap", tmp), "0");
			nvram_set(strcat_r(prefix, "txbf_bfe_cap", tmp), "0");
		}
#endif
	}

	/* Disable nmode for WEP and TKIP for TGN spec */
	if (nvram_match(strcat_r(prefix, "wep", tmp), "enabled") ||
		(nvram_invmatch(strcat_r(prefix, "akm", tmp), "") &&
		 nvram_match(strcat_r(prefix, "crypto", tmp2), "tkip")))
	{
#ifdef RTCONFIG_BCMWL6
		if (subunit == -1)
		{
			strcpy(tmp2, nvram_safe_get(strcat_r(prefix, "chanspec", tmp)));
			if ((nvp = strchr(tmp2, '/')) || (nvp = strchr(tmp2, 'l'))
				|| (nvp = strchr(tmp2, 'u')))
			{
				*nvp = '\0';
				nvram_set(strcat_r(prefix, "chanspec", tmp), tmp2);
			}
		}
#endif
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

		nvram_set(strcat_r(prefix, "nmcsidx", tmp), "-2");	// legacy rate
		nvram_set(strcat_r(prefix, "nmode", tmp), "0");
#ifndef RTCONFIG_BCMWL6
		nvram_set(strcat_r(prefix, "nreqd", tmp), "0");
#endif
		if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))
		nvram_set(strcat_r(prefix, "gmode", tmp), "1");
		nvram_set(strcat_r(prefix, "bw", tmp), "1");		// reset to default setting
#ifdef RTCONFIG_BCMWL6
		nvram_set(strcat_r(prefix, "bw_cap", tmp), "1");
#else
		nvram_set(strcat_r(prefix, "nbw_cap", tmp), "0");
#endif
#ifdef RTCONFIG_BCMWL6
		nvram_set(strcat_r(prefix, "bss_opmode_cap_reqd", tmp), "0");	// no requirements on joining devices
#endif
	}
#ifdef RTCONFIG_QTN
	if (nvram_get_int("qtn_ready") == 1) {
		if (unit == 1) runtime_config_qtn(unit, subunit);
	}
#endif
#if 0
	/* configure mfp for wireless backhaul and guest network */
	if (nvram_get_int("re_mode") == 1
		&& (subunit == 1 || subunit == 2 || subunit == 3) ){
		if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "sae")){
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 2);
		} else if (nvram_match(strcat_r(prefix, "auth_mode_x", tmp), "psk2sae")){
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 1);
		}else{
			nvram_set_int(strcat_r(prefix, "mfp", tmp), 0);
		}
	}
#endif
}

#define BCM5325_ventry(vid, inet_vid, iptv_vid, voip_vid) ( \
	0x01000000 | (vid << 12) | (1 << ports[SWPORT_WAN]) |		\
	((vid == inet_vid) ? (0x01 << ports[SWPORT_CPU]) : 0) |		\
	((vid == iptv_vid) ? (0x41 << ports[SWPORT_LAN4]) : 0) |	\
	((vid == voip_vid) ? (0x41 << ports[SWPORT_LAN3]) : 0)		\
)

void
set_wan_tag(char *interface) {
	int model, wan_vid, iptv_vid, voip_vid, iptv_prio, voip_prio, switch_stb;
	char wan_dev[16], port_id[7];
	char tag_register[sizeof("0xffffffff")], vlan_entry[sizeof("0xffffffff")];
	int gmac3_enable = 0;
#ifdef HND_ROUTER
	char wan_if[10], ethPort1[10], ethPort2[10], ethPort3[10], ethPort4[10];
	char wanVlanDev[10], vlanDev1[10], vlanDev2[10], vlanDev3[10], vlanDev4[10];
	char wanVlanDev1[10];
	char br_dev[sizeof("br101")];
#endif

	model = get_model();
	wan_vid = nvram_get_int("switch_wan0tagid") & 0x0fff;
	iptv_vid = nvram_get_int("switch_wan1tagid") & 0x0fff;
	voip_vid = nvram_get_int("switch_wan2tagid") & 0x0fff;
	iptv_prio = nvram_get_int("switch_wan1prio") & 0x7;
	voip_prio = nvram_get_int("switch_wan2prio") & 0x7;
#ifdef RTCONFIG_MULTICAST_IPTV
	int mang_vid = nvram_get_int("switch_wan3tagid") & 0x0fff;
	int mang_prio = nvram_get_int("switch_wan3prio") & 0x7;
#endif
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
	int i, ret;
#endif

#ifdef RTCONFIG_GMAC3
	gmac3_enable = nvram_get_int("gmac3_enable");
#endif
	switch_stb = nvram_get_int("switch_stb_x");

	sprintf(wan_dev, "vlan%d", wan_vid);

	switch(model) {
				/* WAN L1 L2 L3 L4 CPU */
	case MODEL_RTN53:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12B1:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12C1:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12D1:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12VP:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12HP:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN12HP_B1:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_APN12HP:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN10P:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN10D1:	/* P4  P3 P2 P1 P0 P5 */
	case MODEL_RTN10PV2:	/* P4  P3 P2 P1 P0 P5 */
		/* Enable high bits check */
		eval("et", "robowr", "0x34", "0x3", "0x0080");
		/* Config WAN port */
		if (wan_vid) {
			eval("vconfig", "rem", "vlan1");
			eval("et", "robowr", "0x34", "0x8", "0x01001000");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
		}
		/* Set Wan prio*/
		if (!nvram_match("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_wantag", "unifi_home")) {
			/* vlan0ports= 1 2 3 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010003ae");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan500ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4030");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
			/* vlan600ports= 0 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01258051");
			eval("et", "robowr", "0x34", "0x6", "0x3258");
			/* LAN4 vlan tag */
			eval("et", "robowr", "0x34", "0x10", "0x0258");
		} else if (nvram_match("switch_wantag", "unifi_biz")) {
			/* Modify vlan500ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4030");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
		} else if (nvram_match("switch_wantag", "singtel_mio")) {
			/* vlan0ports= 2 3 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100032c");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a030");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 0 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01014051");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* vlan30ports= 1 4 */
			eval("et", "robowr", "0x34", "0x8", "0x0101e012");	/*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x301e");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x10", "0x8014");
		} else if (nvram_match("switch_wantag", "singtel_others")) {
			/* vlan0ports= 1 2 3 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010003ae");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a030");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 0 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01014051");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x10", "0x8014");
		} else if (nvram_match("switch_wantag", "m1_fiber")) {
			/* vlan0ports= 0 2 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x0100036d");	/*0011|0110|1101*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan1103ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0144f030");	/*0000|0011|0000*/
			eval("et", "robowr", "0x34", "0x6", "0x344f");
			/* vlan1107ports= 1 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01453012");	/*0000|0001|0010*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3453");
		} else if (nvram_match("switch_wantag", "maxis_fiber")) {
			/* vlan0 ports= 0 2 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x0100036d");	/*0011|0110|1101*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan621 ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0126d030");	/*0000|0011|0000*/
			eval("et", "robowr", "0x34", "0x6", "0x326d");
			/* vlan821/822 ports= 1 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01335012");	/*0000|0001|0010*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3335");
			eval("et", "robowr", "0x34", "0x8", "0x01336012");	/*0000|0001|0010*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3336");
		} else if (nvram_match("switch_wantag", "maxis_fiber_sp")) {
			/* vlan0ports= 0 2 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x0100036d");	/*0011|0110|1101*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan11ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100b030");	/*0000|0011|0000*/
			eval("et", "robowr", "0x34", "0x6", "0x300b");
			/* vlan14ports= 1 4 */
			eval("et", "robowr", "0x34", "0x8", "0x0100e012");	/*0000|0001|0010*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x300e");
		} else {	/* manual */
							/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 3, 2, 1, 0, 5 };

			if (switch_stb != SWCFG_STB4 && switch_stb != SWCFG_STB34)
				iptv_vid = 0;
			if (switch_stb != SWCFG_STB3 && switch_stb != SWCFG_STB34)
				voip_vid = 0;
			if (wan_vid) {
				sprintf(vlan_entry, "0x%x", BCM5325_ventry(wan_vid, wan_vid, iptv_vid, voip_vid));
				eval("et", "robowr", "0x34", "0x8", vlan_entry);
				eval("et", "robowr", "0x34", "0x6", "0x3001");
			}
			if (iptv_vid) {
				if (iptv_vid != wan_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(iptv_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3002");
				}
				sprintf(tag_register, "0x%x", (iptv_prio << 13) | iptv_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN4]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
			if (voip_vid) {
				if (voip_vid != wan_vid && voip_vid != iptv_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(voip_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3003");
				}
				sprintf(tag_register, "0x%x", (voip_prio << 13) | voip_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN3]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
		}
		break;

				/* WAN L1 L2 L3 L4 CPU */
	case MODEL_RTN14UHP:	/* P4  P0 P1 P2 P3 P5 */
		/* Enable high bits check */
		eval("et", "robowr", "0x34", "0x3", "0x0080");
		/* Config WAN port */
		if (wan_vid) {
			eval("vconfig", "rem", "vlan1");
			eval("et", "robowr", "0x34", "0x8", "0x01001000");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
		}
		/* Set Wan prio*/
		if (!nvram_match("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_wantag", "unifi_home")) {
			/* vlan0ports= 0 1 2 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010001e7");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan500ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4030");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
			/* vlan600ports= 3 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01258218");
			eval("et", "robowr", "0x34", "0x6", "0x3258");
			/* LAN4 vlan tag */
			eval("et", "robowr", "0x34", "0x16", "0x0258");
		} else if (nvram_match("switch_wantag", "unifi_biz")) {
			/* Modify vlan500ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4030");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
		} else if (nvram_match("switch_wantag", "singtel_mio")) {
			/* vlan0ports= 0 1 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010000E3");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a030");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 3 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01014218");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* vlan30ports= 2 4 */
			eval("et", "robowr", "0x34", "0x8", "0x0101e014");	/*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x301e");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x16", "0x8014");
		} else if (nvram_match("switch_wantag", "singtel_others")) {
			/* vlan0ports= 0 1 2 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010001e7");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a030");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 3 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01014218");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x16", "0x8014");
		} else if (nvram_match("switch_wantag", "m1_fiber")) {
			/* vlan0ports= 0 1 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010002eb");	/*0010|1110|1011*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan1103ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0144f030");	/*0000|0011|0000*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x344f");
			/* vlan1107ports= 2 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01453014");	/*0000|0001|0100*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3453");
		} else if (nvram_match("switch_wantag", "maxis_fiber")) {
			/* vlan0 ports= 0 1 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010002eb");	/*0010|1110|1011*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan621 ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0126d030");	/*0000|0011|0000*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x326d");
			/* vlan821/822 ports= 2 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01335014");	/*0000|0001|0100*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3335");
			eval("et", "robowr", "0x34", "0x8", "0x01336014");	/*0000|0001|0100*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3336");
		} else if (nvram_match("switch_wantag", "maxis_fiber_sp")) {
			/* vlan0ports= 0 1 3 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010002eb");	/*0010|1110|1011*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan11ports= 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100b030");	/*0000|0011|0000*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x300b");
			/* vlan14ports= 2 4 */
			eval("et", "robowr", "0x34", "0x8", "0x0100e014");	/*0000|0000|0101*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x300e");
		} else if (nvram_match("switch_wantag", "meo")) {
			/* vlan0ports= 0 1 2 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010001e7");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan12ports= 3 4 5 */				/* untag||forward */
			eval("et", "robowr", "0x34", "0x8", "0x0100c038");	/*0000|0011|1000*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x300c");
		} else if (nvram_match("switch_wantag", "vodafone")) {
			/* vlan0ports= 0 1 2 5t */
			eval("et", "robowr", "0x34", "0x8", "0x010000E3");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan100ports= 3t 4t 5t */
			eval("et", "robowr", "0x34", "0x8", "0x01064038");
			eval("et", "robowr", "0x34", "0x6", "0x3064");
			/* vlan101ports= 3t 4t */
			eval("et", "robowr", "0x34", "0x8", "0x01065018");
			eval("et", "robowr", "0x34", "0x6", "0x3065");
			/* vlan105ports= 2 3t 4t */
			eval("et", "robowr", "0x34", "0x8", "0x0106961C");
			eval("et", "robowr", "0x34", "0x6", "0x3069");

			/* WAN port: tag=100 & prio=1 */
			eval("et", "robowr", "0x34", "0x10", "0x2064");
			/* LAN4: tag=105 & prio=1 */
			eval("et", "robowr", "0x34", "0x16", "0x2069");
		}

		else {	/* manual */
							/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 4, 0, 1, 2, 3, 5 };

			if (switch_stb != SWCFG_STB4 && switch_stb != SWCFG_STB34)
				iptv_vid = 0;
			if (switch_stb != SWCFG_STB3 && switch_stb != SWCFG_STB34)
				voip_vid = 0;
			if (wan_vid) {
				sprintf(vlan_entry, "0x%x", BCM5325_ventry(wan_vid, wan_vid, iptv_vid, voip_vid));
				eval("et", "robowr", "0x34", "0x8", vlan_entry);
				eval("et", "robowr", "0x34", "0x6", "0x3001");
			}
			if (iptv_vid) {
				if (iptv_vid != wan_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(iptv_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3002");
				}
				sprintf(tag_register, "0x%x", (iptv_prio << 13) | iptv_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN4]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
			if (voip_vid) {
				if (voip_vid != wan_vid && voip_vid != iptv_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(voip_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3003");
				}
				sprintf(tag_register, "0x%x", (voip_prio << 13) | voip_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN3]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
		}
		break;

				/* WAN L1 L2 L3 L4 CPU */
	case MODEL_RTN10U:	/* P0  P4 P3 P2 P1 P5 */
		/* Enable high bits check */
		eval("et", "robowr", "0x34", "0x3", "0x0080");
		/* Config WAN port */
		if (wan_vid) {
			eval("vconfig", "rem", "vlan1");
			eval("et", "robowr", "0x34", "0x8", "0x01001000");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
		}
		/* Set Wan prio*/
		if (!nvram_match("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_wantag", "unifi_home")) {
			/* vlan0ports= 2 3 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100073c");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan500ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4021");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
			/* vlan600ports= 0 1 */
			eval("et", "robowr", "0x34", "0x8", "0x01258083");
			eval("et", "robowr", "0x34", "0x6", "0x3258");
			/* LAN4 vlan tag */
			eval("et", "robowr", "0x34", "0x12", "0x0258");
		} else if (nvram_match("switch_wantag", "unifi_biz")) {
			/* Modify vlan500ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4021");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
		} else if (nvram_match("switch_wantag", "singtel_mio")) {
			/* vlan0ports= 3 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x01000638");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a021");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 1 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01014083");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* vlan30ports= 2 0 */
			eval("et", "robowr", "0x34", "0x8", "0x0101e005");	/*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x301e");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x12", "0x8014");
		} else if (nvram_match("switch_wantag", "singtel_others")) {
			/* vlan0ports= 2 3 4 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100073c");
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan10ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a021");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 0 1 */
			eval("et", "robowr", "0x34", "0x8", "0x01014083");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x12", "0x8014");
		} else if (nvram_match("switch_wantag", "m1_fiber")) {
			/* vlan0ports= 1 3 4 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010006ba");	/*0110|1011|1010*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan1103ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0144f021");	/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x344f");
			/* vlan1107ports= 2 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01453005");	/*0000|0000|0101*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3453");
		} else if (nvram_match("switch_wantag", "maxis_fiber")) {
			/* vlan0 ports= 1 3 4 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010006ba");	/*0110|1011|1010*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan621 ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0126d021");	/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x326d");
			/* vlan821/822 ports= 2 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01335005");	/*0000|0000|0101*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3335");
			eval("et", "robowr", "0x34", "0x8", "0x01336005");	/*0000|0000|0101*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3336");
		} else if (nvram_match("switch_wantag", "maxis_fiber_sp")) {
			/* vlan0ports= 1 3 4 5 */				/*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010006ba");	/*0110|1011|1010*/
			eval("et", "robowr", "0x34", "0x6", "0x3000");
			/* vlan11ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100b021");	/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x300b");
			/* vlan14ports= 2 0 */
			eval("et", "robowr", "0x34", "0x8", "0x0100e005");	/*0000|0000|0101*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x300e");
		} else {	/* manual */
							/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 0, 4, 3, 2, 1, 5 };

			if (switch_stb != SWCFG_STB4 && switch_stb != SWCFG_STB34)
				iptv_vid = 0;
			if (switch_stb != SWCFG_STB3 && switch_stb != SWCFG_STB34)
				voip_vid = 0;
			if (wan_vid) {
				sprintf(vlan_entry, "0x%x", BCM5325_ventry(wan_vid, wan_vid, iptv_vid, voip_vid));
				eval("et", "robowr", "0x34", "0x8", vlan_entry);
				eval("et", "robowr", "0x34", "0x6", "0x3001");
			}
			if (iptv_vid) {
				if (iptv_vid != wan_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(iptv_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3002");
				}
				sprintf(tag_register, "0x%x", (iptv_prio << 13) | iptv_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN4]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
			if (voip_vid) {
				if (voip_vid != wan_vid && voip_vid != iptv_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(voip_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry);
					eval("et", "robowr", "0x34", "0x6", "0x3003");
				}
				sprintf(tag_register, "0x%x", (voip_prio << 13) | voip_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN3]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
		}
		break;

	case MODEL_RTN16:
		// config wan port
		if (wan_vid) {
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0101");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		// Set Wan port PRIO
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {
			// config LAN 3 = VoIP
			if (nvram_match("switch_wantag", "m1_fiber")) {
				// Just forward packets between port 0 & 3, without untag
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	// Nomo case, untag it.
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				// Set vlan table entry register
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0805");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) {
			// config LAN 4 = IPTV
			iptv_prio = iptv_prio << 13;
			sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
			eval("et", "robowr", "0x34", "0x12", tag_register);
			_dprintf("lan 4 tag register: %s\n", tag_register);
			// Set vlan table entry register
			sprintf(vlan_entry, "0x%x", iptv_vid);
			_dprintf("vlan entry: %s\n", vlan_entry);
			eval("et", "robowr", "0x05", "0x83", "0x0403");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		} else if (nvram_match("switch_stb_x", "6")) {
			// config LAN 3 = VoIP
			if (nvram_match("switch_wantag", "singtel_mio")) {
				// Just forward packets between port 0 & 3, without untag
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	// Nomo case, untag it.
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x14", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					// Set vlan table entry register
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x0C07");
					else
						eval("et", "robowr", "0x05", "0x83", "0x0805");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			// config LAN 4 = IPTV
			if (iptv_vid) {
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				// Set vlan table entry register
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x0C07");
				else
					eval("et", "robowr", "0x05", "0x83", "0x0403");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
		break;

				/* P0  P1 P2 P3 P4 P5 */
	case MODEL_RTAC3200: 	/* WAN L4 L3 L2 L1 CPU */
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0021");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) { // P3
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);

				char vlan_cmd[64];
				sprintf(vlan_cmd, "robocfg vlan 1 ports \"2 3 4 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 2 ports \"0 5\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 1t 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 1t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 1t 2\"");
				system(vlan_cmd);
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", "0x0335"); /* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", "0x0336"); /* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0805");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // P4
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between port 0 & 1, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("* vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0023");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				/* config LAN 4 = IPTV */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0403");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */ // P3
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x14", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x0C07");
					else
						eval("et", "robowr", "0x05", "0x83", "0x0805");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config LAN 4 = IPTV */ // P4
			if (iptv_vid) {
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x0C07");
				else
					eval("et", "robowr", "0x05", "0x83", "0x0403");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;

				/* P0  P1 P2 P3 P4 P5 */
	case MODEL_RTAC68U:	/* WAN L1 L2 L3 L4 CPU */
	case MODEL_RTN18U:	/* WAN L1 L2 L3 L4 CPU */
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0021");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {	// P3
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);

				char vlan_cmd[64];
				sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 2 ports \"0 5\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 3 4t\"");
				system(vlan_cmd);
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0335");	/* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0336");	/* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x1009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // P4
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between port 0 & 4, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0031");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x18", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */ // P3
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x16", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x3019");
					else
						eval("et", "robowr", "0x05", "0x83", "0x1009");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config LAN 4 = IPTV */ // P4
			if (iptv_vid) {
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x18", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x3019");
				else
					eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0021");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;

#ifdef HND_ROUTER		
				/* 4 ports based model following the rule below to match UI transmit values */
				/* ethPort1 LAN4 */
				/* ethPort2 LAN3 */
				/* ethPort3 LAN2 */
				/* ethPort4 LAN1 */
				/* vlanDev1 LAN4 */
				/* vlanDev2 LAN3 */
				/* vlanDev3 LAN2 */
				/* vlanDev4 LAN1 */

				/* 3 ports based model following the rule below to match UI transmit values */
				/* ethPort1 LAN3 */
				/* ethPort2 LAN2 */
				/* ethPort3 LAN2 */
				/* ethPort4 LAN1 */
				/* vlanDev1 LAN3 */
				/* vlanDev2 LAN2 */
				/* vlanDev3 LAN2 */
				/* vlanDev4 LAN1 */
	case MODEL_GTAC5300:
				/*      P1   P0   x3 x2 P3   P2   x1 x0	[P4(P0) P5(P1&P2) P8(P3)] */
				/* eth0 eth2 eth1 eth5  eth4 eth3 eth5				  */
				/* WAN  L1   L2   L3 L4 L5   L6   L7 L8 CPU			  */
	case MODEL_RTAC86U:
				/*      P3   P2   P1   P0   [P4(P0) P5(P1&P2) P8(P3)]		  */
				/* eth0 eth4 eth3 eth2 eth1					  */
				/* WAN  L1   L2   L3   L4   CPU					  */

	case MODEL_RTAX88U:	
	case MODEL_BC109:	
	case MODEL_BC105:	
	case MODEL_EBG19:
	case MODEL_EBG15:
	case MODEL_EBP15:
	case MODEL_GTAX11000:
	case MODEL_RTAX92U:
	case MODEL_RTAX56U:
	case MODEL_RTAX86U:
	case MODEL_RTAX68U:
	case MODEL_RTAC68U_V4:
	case MODEL_GTAXE11000:
				/* eth0 eth4 eth3 eth2 eth1					  */	
				/* WAN  L1   L2   L3   L4					  */

	case MODEL_RTAX95Q:
	case MODEL_XT8PRO:
	case MODEL_BM68:
	case MODEL_XT8_V2:
	case MODEL_RTAXE95Q:
	case MODEL_ET8PRO:
	case MODEL_ET8_V2:
				/* eth0 eth1 eth2 eth3						  */	
				/* WAN  L1   L2   L3						  */

	case MODEL_BT12:
				/* eth0 eth1 eth2 eth3 eth4						  */
				/* WAN  L1   L2   L3 L4						  */

	case MODEL_BT10:
				/* eth0 eth1 eth2						  */
				/* WAN  L1   L2						  */

	case MODEL_BQ16:
				/* eth0 eth1 eth2 eth3 eth4						  */
				/* WAN  L1   L2   L3 L4						  */

	case MODEL_BQ16_PRO:
				/* eth0 eth1 eth2 eth3 eth4						  */
				/* WAN  L1   L2   L3 L4						  */

	case MODEL_RTAX56_XD4:
	case MODEL_XD4PRO:
	case MODEL_CTAX56_XD4:
	case MODEL_EBA63:
				/* eth0 eth1						  */	
				/* WAN  L1						  */

	case MODEL_XC5:
                                /* eth0 eth1 eth2                                            */
                                /* WAN  L1   L2                                              */
 
	case MODEL_RTAX58U:
	case MODEL_RTAX82U_V2:
	case MODEL_TUFAX5400_V2:
	case MODEL_RTAX5400:
	case MODEL_XD6_V2:
				/* eth4 eth3 eth2 eth1 eth0					  */	
				/* WAN  L1   L2   L3   L4					  */
				/* XD6 / XD6_V2							  */
				/* eth4 eth2 eth1 eth0						  */
				/* WAN  L1   L2   L3						  */
	case MODEL_RTAX82_XD6S:
				/* eth1 eth0							  */
				/* WAN  L1							  */
	case MODEL_GT10:	/* nvram_get_int("wans_extwan") == 0 */
				/* eth0 eth1 eth2 eth3						*/
				/* WAN  L1   L2   L3						*/
				/* nvram_get_int("wans_extwan") == 1 */
				/* eth1 eth0 eth2 eth3						*/
				/* WAN  L1   L2   L3						*/
	case MODEL_RTAX9000:
				/* eth0 eth1 eth2 eth3 eth4 eth5				*/
				/* WAN  L1   L2   L3   L4   L5					*/
	case MODEL_TUFAX3000_V2:
	case MODEL_RTAXE7800:
				/* nvram_get_int("wans_extwan") == 0 */
				/* eth0 eth1 eth2 eth3 eth4					*/
				/* WAN  L1   L2   L3   L4					*/
				/* nvram_get_int("wans_extwan") == 1 */
				/* eth1 eth0 eth2 eth3 eth4					*/
				/* WAN  L1   L2   L3   L4					*/
	case MODEL_GTAX6000:
	case MODEL_GTAX11000_PRO:
	case MODEL_RTAX86U_PRO:
	case MODEL_RTAX88U_PRO:
	case MODEL_RTBE96U:
				/* eth0 eth1 eth2 eth3 eth4 eth5 				*/
				/* WAN	L1   L2   L3   L4   L5 					*/

	case MODEL_GTAXE16000:
				/* eth0 eth1 eth2 eth3 eth4 eth5 eth6 				*/
				/* WAN	L1   L2   L3   L4   L5   L6				*/
	case MODEL_ET12:
	case MODEL_XT12:
				/* eth0 eth1 eth2 eth3	*/
				/* WAN  L1   L2   L3	*/
	case MODEL_RTBE88U:
				/* eth0 eth1 eth2 eth3 eth4 eth5 eth6 eth7 eth8 eth9				*/
				/* WAN	L1   L2   L3   L4   L5 	 L6   L7   L8   SFP+				*/
	case MODEL_RTBE86U:
				/* eth0 eth1 eth2 eth3 eth4 */
				/* WAN  L1   L2   L3   L4   */
		/* GT-AC5300 pairing ports: L1 & L2 and L5 & L6 */
		if (model == MODEL_GTAC5300) {
			if (nvram_match("iptv_port_settings", "12")) {
				sprintf(ethPort1, "eth2");
				sprintf(ethPort2, "eth1");
				sprintf(ethPort3, "eth2");
				sprintf(ethPort4, "eth1");
				sprintf(vlanDev1, "eth2.v0");
				sprintf(vlanDev2, "eth1.v0");
				sprintf(vlanDev3, "eth2.v0");
				sprintf(vlanDev4, "eth1.v0");
			} else {
				sprintf(ethPort1, "eth4");
				sprintf(ethPort2, "eth3");
				sprintf(ethPort3, "eth4");
				sprintf(ethPort4, "eth3");
				sprintf(vlanDev1, "eth4.v0");
				sprintf(vlanDev2, "eth3.v0");
				sprintf(vlanDev3, "eth4.v0");
				sprintf(vlanDev4, "eth3.v0");
			}
		}
		/* Modles revert to 4 ports base IPTV profile */
		else if (model == MODEL_RTAC86U || model == MODEL_RTAX88U || model == MODEL_BC109 || model == MODEL_BC105 || model == MODEL_EBG19 || model == MODEL_EBG15 || model == MODEL_EBP15 || model == MODEL_RTAX92U || model == MODEL_RTAX56U || model == MODEL_GTAX11000 || model == MODEL_RTAX86U || model == MODEL_RTAX68U || model == MODEL_RTAC68U_V4) {
			sprintf(ethPort1, "eth1");
			sprintf(ethPort2, "eth2");
			sprintf(ethPort3, "eth3");
			sprintf(ethPort4, "eth4");
			sprintf(vlanDev1, "eth1.v0");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(vlanDev3, "eth3.v0");
			sprintf(vlanDev4, "eth4.v0");
		}
		else if (model == MODEL_GTAXE11000) {
			sprintf(ethPort1, "eth3");	// LAN4
			sprintf(ethPort2, "eth2");	// LAN3
			sprintf(ethPort3, "eth4");	// LAN2
			sprintf(ethPort4, "eth1");	// LAN1
			sprintf(vlanDev1, "eth3.v0");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(vlanDev3, "eth4.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Specific net devices order for RT-AX58U */
		else if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400) {
#if defined(RTAX82_XD6) || defined(XD6_V2)
			sprintf(ethPort1, "eth0");
			sprintf(ethPort2, "eth1");
			sprintf(ethPort3, "eth1");
			sprintf(ethPort4, "eth2");
			sprintf(vlanDev1, "eth0.v0");
			sprintf(vlanDev2, "eth1.v0");
			sprintf(vlanDev3, "eth1.v0");
			sprintf(vlanDev4, "eth2.v0");
#else
			sprintf(ethPort1, "eth0");
			sprintf(ethPort2, "eth1");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth3");
			sprintf(vlanDev1, "eth0.v0");
			sprintf(vlanDev2, "eth1.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth3.v0");
#endif
		}
		/* Spefici net devices order for RT-AX82_XD6S */
		else if (model == MODEL_RTAX82_XD6S) {
			sprintf(ethPort1, "eth0");
			sprintf(vlanDev1, "eth0.v0");
			sprintf(ethPort2, "eth0");
			sprintf(vlanDev2, "eth0.v0");
		}
		/* Spefici net devices order for RT-AX95Q, XT8PRO, BM68, XT8_V2, RT-AXE95Q, ET8PRO, ET8_V2 */
		else if (model == MODEL_RTAX95Q || model == MODEL_XT8PRO || model == MODEL_BM68 || model == MODEL_XT8_V2 || model == MODEL_RTAXE95Q || model == MODEL_ET8PRO || model == MODEL_ET8_V2) {
			sprintf(ethPort1, "eth3");
			sprintf(ethPort2, "eth2");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth3.v0");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
/* Spefici net devices order for BT12 */
		else if (model == MODEL_BT12) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for BT10 */
		else if (model == MODEL_BT10) {
			/* three ports special mapping : LAN4(=LAN3,eth2) > LAN3(eth2) > LAN2(eth1) > LAN1(eth0) */
			sprintf(ethPort1, "eth2");
			sprintf(vlanDev1, "eth2.v0");
			sprintf(ethPort2, "eth1");
			sprintf(vlanDev2, "eth1.v0");
			sprintf(ethPort3, "eth1");
			sprintf(vlanDev3, "eth1.v0");
			sprintf(ethPort4, "eth0");
			sprintf(vlanDev4, "eth0.v0");
		}
		/* Spefici net devices order for BQ16 */
		else if (model == MODEL_BQ16) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for BQ16_PRO */
		else if (model == MODEL_BQ16_PRO) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RTAX56_XD4 */
		else if (model == MODEL_RTAX56_XD4) {
			if(nvram_match("HwId", "A") || nvram_match("HwId", "C")){
				sprintf(ethPort1, "eth1");
				sprintf(vlanDev1, "eth1.v0");
				sprintf(ethPort2, "eth1");
				sprintf(vlanDev2, "eth1.v0");
			} else {
				sprintf(ethPort1, "");
				sprintf(vlanDev1, "");
				sprintf(ethPort2, "");
				sprintf(vlanDev2, "");
			}
		}
		/* Spefici net devices order for XD4PRO */
		else if (model == MODEL_XD4PRO) {
			sprintf(ethPort1, "eth1");
			sprintf(vlanDev1, "eth1.v0");
			sprintf(ethPort2, "eth1");
			sprintf(vlanDev2, "eth1.v0");
		}
                /* Spefici net devices order for XC5 */
                else if (model == MODEL_XC5) {
                        sprintf(ethPort1, "eth1");
                        sprintf(vlanDev1, "eth1.v0");
                        sprintf(ethPort2, "eth2");
                        sprintf(vlanDev2, "eth2.v0");
                }
		/* Spefici net devices order for CTAX56_XD4 */
		else if (model == MODEL_CTAX56_XD4) {
			sprintf(ethPort1, "eth1");
			sprintf(vlanDev1, "eth1.v0");
			sprintf(ethPort2, "eth1");
			sprintf(vlanDev2, "eth1.v0");
		}
		/* Spefici net devices order for EBA63 */
		else if (model == MODEL_EBA63) {
			sprintf(ethPort1, "eth1");
			sprintf(vlanDev1, "eth1.v0");
		}
		/* Spefici net devices order for TUF-AX3000_V2 / RT-AXE7800 */
		else if (model == MODEL_TUFAX3000_V2 || model == MODEL_RTAXE7800) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			if (!nvram_get_int("wans_extwan"))
				sprintf(ethPort4, "eth1");
			else
				sprintf(ethPort4, "eth0");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			if (!nvram_get_int("wans_extwan"))
				sprintf(vlanDev4, "eth1.v0");
			else
				sprintf(vlanDev4, "eth0.v0");
		}
		/* Spefici net devices order for GT10 */
		else if (model == MODEL_GT10) {
			sprintf(ethPort1, "eth3");
			sprintf(ethPort2, "eth2");
			sprintf(ethPort3, "eth2");
			if (!nvram_get_int("wans_extwan"))
				sprintf(ethPort4, "eth1");
			else
				sprintf(ethPort4, "eth0");
			sprintf(vlanDev1, "eth3.v0");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(vlanDev3, "eth2.v0");
			if (!nvram_get_int("wans_extwan"))
				sprintf(vlanDev4, "eth1.v0");
			else
				sprintf(vlanDev4, "eth0.v0");
		}
		/* Spefici net devices order for RT-AX9000 */
		else if (model == MODEL_RTAX9000) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for GT-AX6000 */
		else if (model == MODEL_GTAX6000) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for GT-AX11000 PRO */
		else if (model == MODEL_GTAX11000_PRO) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for GT-AXE16000 */
		else if (model == MODEL_GTAXE16000) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for GT-BE98 */
		else if (model == MODEL_GTBE98) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for ET12/XT12 */
		else if (model == MODEL_ET12 || model == MODEL_XT12) {
			sprintf(ethPort1, "eth1");
			sprintf(ethPort2, "eth2");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth1.v0");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RT-AX88U PRO */
		else if (model == MODEL_RTAX88U_PRO) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RT-BE96U */
		else if (model == MODEL_RTBE96U) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RT-BE88U */
		else if (model == MODEL_RTBE88U) {
			// TBD
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RT-BE86U */
		else if (model == MODEL_RTBE86U) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}
		/* Spefici net devices order for RT-AX86U PRO */
		else if (model == MODEL_RTAX86U_PRO) {
			sprintf(ethPort1, "eth4");
			sprintf(ethPort2, "eth3");
			sprintf(ethPort3, "eth2");
			sprintf(ethPort4, "eth1");
			sprintf(vlanDev1, "eth4.v0");
			sprintf(vlanDev2, "eth3.v0");
			sprintf(vlanDev3, "eth2.v0");
			sprintf(vlanDev4, "eth1.v0");
		}

#ifdef RTCONFIG_MULTISERVICE_WAN
		if (switch_stb && nvram_match("switch_wantag", "none") && !wan_vid && !iptv_vid && !voip_vid) {
			snprintf(br_dev, sizeof(br_dev), "br%d", IPTV_BR_INDEX);
			eval("brctl", "addbr", br_dev);
			eval("ifconfig", br_dev, "up");
			switch(switch_stb) {
				case SWCFG_STB1:
					eval("brctl", "delif", "br0", ethPort4);
					eval("brctl", "addif", br_dev, ethPort4);
					break;
				case SWCFG_STB2:
					eval("brctl", "delif", "br0", ethPort3);
					eval("brctl", "addif", br_dev, ethPort3);
					break;
				case SWCFG_STB3:
					eval("brctl", "delif", "br0", ethPort2);
					eval("brctl", "addif", br_dev, ethPort2);
					break;
				case SWCFG_STB4:
					eval("brctl", "delif", "br0", ethPort1);
					eval("brctl", "addif", br_dev, ethPort1);
					break;
				case SWCFG_STB12:
					eval("brctl", "delif", "br0", ethPort4);
					eval("brctl", "addif", br_dev, ethPort4);
					eval("brctl", "delif", "br0", ethPort3);
					eval("brctl", "addif", br_dev, ethPort3);
					break;
				case SWCFG_STB34:
					eval("brctl", "delif", "br0", ethPort2);
					eval("brctl", "addif", br_dev, ethPort2);
					eval("brctl", "delif", "br0", ethPort1);
					eval("brctl", "addif", br_dev, ethPort1);
					break;
			}
			break;
		}
#endif

		/* write default WAN net device to handle vlanctl commands */
		sprintf(wan_if, wan_if_eth());
		sprintf(wanVlanDev, "%s.v0", wan_if_eth());

		/* Using vlanctl to handle vlan forwarding */
		if ((!nvram_match("switch_wantag", "") && (wan_vid || switch_stb > 0))
		 && !nvram_match("switch_wantag", "hinet_mesh")
		 && !nvram_match("switch_wantag", "unifi_biz_voip")
		) { /* config wan port or bridge hinet IPTV traffic */
#if 0
			/* Handle no vid traffic */
#endif
			sprintf(port_id, "%d", wan_vid);
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if ((nvram_match("switch_wantag", "movistar") || 
				(switch_stb == 0 && wan_vid))
				&& !nvram_match("switch_wantag", "google_fiber")) {
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", wan_vid);
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);
			}
			else {
				if(nvram_match("switch_wantag", "free")) {
					eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				}
				else {
					/* Handle wan(internet) vlan traffic */
					eval("vlanctl", "--mcast", "--if-create", wan_if, "0");
					if ((!wan_vid || nvram_match("switch_wantag", "none")) && switch_stb > 0) { /* bridge IPTV traffic */
						/* set wan vlan interface */
						eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev, "--rule-append");
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--rule-append");
					} else {
						/* pop tag for entering CPU */
						eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", port_id, "0", "--pop-tag", "--set-rxif", wanVlanDev, "--rule-append");
						/* tx push tag */
						/* Set Wan PRIO */
						if (nvram_invmatch("switch_wan0prio", "0")) {
							eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", port_id, "0", "--set-pbits", nvram_get("switch_wan0prio"), "0", "--rule-append");
						}
						else {
							eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", port_id, "0", "--rule-append");
						}
					}
					eval("ifconfig", wanVlanDev, "allmulti", "up");
				}

				/*
					multicast iptv no need to forward vlan packets from wan to lan
				*/
				if ((switch_stb <= 6)
#ifdef RTAC86U
					|| (switch_stb == SWCFG_PASSTHROUGH)
#endif
				) {
					/* Add wan bridge */
					sprintf(br_dev, "br%d", IPTV_BR_INDEX);
					eval("brctl", "addbr", br_dev);
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
					if (!nvram_match("switch_wantag", "unifi_home"))
						eval("brctl", "stp", br_dev, "on");
					eval("ifconfig", br_dev, "up");
					if(!nvram_match("switch_wantag", "free")) {
						eval("brctl", "addif", br_dev, wanVlanDev);
						set_wan_phy("");
						add_wan_phy(br_dev);
						nvram_set("wan0_ifname", br_dev);
						nvram_set("wan0_gw_ifname", br_dev);
					}
					// enable softswitch for vlan forwarding(all internal switch)
					if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
						eval("ethswctl", "-c", "softswitch",  "-i",  wan_if, "-o", "enable");
				} else {
					set_wan_phy("");
					sprintf(wan_dev, wanVlanDev);
					add_wan_phy(wan_dev);
					nvram_set("wan0_ifname", wanVlanDev);
					nvram_set("wan0_gw_ifname", wanVlanDev);
				}
			}
		}

		if (nvram_match("switch_stb_x", "1") && nvram_match("switch_wantag", "none")) {
			/* Just forward packets between wan & vlanDev1, no tag */
			eval("brctl", "delif", "br0", ethPort4);
			eval("vlanctl", "--mcast", "--if-create", ethPort4, "0");
			/* pop tag (lan bridge) */
			eval("vlanctl", "--if", ethPort4, "--rx", "--tags", "0", "--set-rxif", vlanDev4, "--rule-append");
			eval("vlanctl", "--if", ethPort4, "--tx", "--tags", "0", "--filter-txif", vlanDev4, "--rule-append");
			eval("ifconfig", vlanDev4, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev4);
			// enable softswitch for vlan forwarding(all internal switch)
			if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_RTAX5400)
				eval("ethswctl", "-c", "softswitch",  "-i",  ethPort4, "-o", "enable");
		} else if (nvram_match("switch_stb_x", "2") && nvram_match("switch_wantag", "none")) {
			/* Just forward packets between wan & vlanDev1, no tag */
			eval("brctl", "delif", "br0", ethPort3);
			eval("vlanctl", "--mcast", "--if-create", ethPort3, "0");
			/* pop tag (lan bridge) */
			eval("vlanctl", "--if", ethPort3, "--rx", "--tags", "0", "--set-rxif", vlanDev3, "--rule-append");
			eval("vlanctl", "--if", ethPort3, "--tx", "--tags", "0", "--filter-txif", vlanDev3, "--rule-append");
			eval("ifconfig", vlanDev3, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev3);
			// enable softswitch for vlan forwarding(all internal switch)
			if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_RTAX5400)
				eval("ethswctl", "-c", "softswitch",  "-i",  ethPort3, "-o", "enable");
		} else if (nvram_match("switch_stb_x", "3")) {
			if (nvram_match("switch_wantag", "vodafone")) {
				/* Forward packets from wan:eth0 to vlanDev1 (leave tag) */
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", "101", "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort1);
				eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
				/* ---------- wan bridge lan leave tag case? ---------- */
				/* rx pop tag */
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "1", "--filter-vid", port_id, "0", "--pop-tag", "--set-rxif", vlanDev1, "--rule-append");
				/* tx push tag */
				/* Set Wan PRIO */
				if (nvram_invmatch("switch_wan0prio", "0"))
					eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", port_id, "0", "--set-pbits", nvram_get("switch_wan0prio"), "0", "--rule-append");
				else
					eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", port_id, "0", "--rule-append");
				/* bridge */
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "1", "--filter-vid", "101", "0", "--set-rxif", vlanDev1, "--rule-append");
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "1", "--filter-vid", "105", "0", "--set-rxif", vlanDev1, "--rule-append");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between WAN & vlanDev2, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between WAN & vlanDev2, without untag */
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", "0x0335", "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", "0x0336", "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "1", "--filter-vid", "0x0335", "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "1", "--filter-vid", "0x0336", "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "none")) {
				if (model == MODEL_GTAC5300) {
					if (nvram_match("iptv_port_settings", "12")) {
						sprintf(ethPort2, "eth2");
						sprintf(vlanDev2, "eth2.v0");
					} else {
						sprintf(ethPort2, "eth4");
						sprintf(vlanDev2, "eth4.v0");
					}
				}
				/* 3 ports based model switch switch_wan_tag==3 to LAN3 */
				else if (model == MODEL_RTAX95Q || model == MODEL_XT8PRO || model == MODEL_BT12 || model == MODEL_BQ16 || model == MODEL_BQ16_PRO || model == MODEL_BM68 || model == MODEL_XT8_V2 || model == MODEL_RTAXE95Q || model == MODEL_ET8PRO || model == MODEL_ET8_V2 || model == MODEL_GT10) {
					sprintf(ethPort2, "eth3");
					sprintf(vlanDev2, "eth3.v0");
				}
#if defined(RTAX82_XD6) || defined(XD6_V2)
				else if (model == MODEL_RTAX58U || model == MODEL_XD6_V2) {
					sprintf(ethPort2, "eth0");
					sprintf(vlanDev2, "eth0.v0");
				}
#endif
				else if (model == MODEL_BT10) {
					sprintf(ethPort2, "eth2");
					sprintf(vlanDev2, "eth2.v0");
				}

				/* Just forward packets between wan & vlanDev1, no tag */
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				/* pop tag (lan bridge) */
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("vlanctl", "--if", ethPort2, "--tx", "--tags", "0", "--filter-txif", vlanDev2, "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "unifi_biz_voip")) {
				unsigned char eabuf[ETHER_ADDR_LEN];
				char macaddr[32];
				int iptv_unit = WAN_UNIT_IPTV;
				char prefix[] = "wanXXX_";

				/* WAN */
				sprintf(vlan_entry, "0x%x", wan_vid);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev, "--mcast", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--set-rxif", wanVlanDev, "--filter-vid", vlan_entry, "0", "--filter-vlan-dev-mac-addr", "1", "--pop-tag", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev, "--filter-multicast", "--rule-append");
				ifconfig(wanVlanDev, IFUP, "0.0.0.0", NULL);
				set_wan_phy("");
				add_wan_phy(wanVlanDev);
				nvram_set("wan0_ifname", wanVlanDev);
				nvram_set("wan0_gw_ifname", wanVlanDev);

				/* VoIP : bridge with no tag */
				snprintf(wanVlanDev1, sizeof(wanVlanDev1), "%s.v1", wan_if);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev1, "--routed", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--set-rxif", wanVlanDev1, "--filter-vid", vlan_entry, "0", "--pop-tag", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				ifconfig(wanVlanDev1, IFUP, "0.0.0.0", NULL);
				snprintf(prefix, sizeof(prefix), "wan%d_", iptv_unit);
				nvram_pf_set(prefix, "ifname", wanVlanDev1);
				nvram_set("iptv_ifname", wanVlanDev1);

				ether_atoe(get_wan_hwaddr(), eabuf);
				eabuf[ETHER_ADDR_LEN - 1] += 1;
				ether_etoa(eabuf, macaddr);
				nvram_pf_set(prefix, "hwaddr", macaddr);
				nvram_pf_set(prefix, "proto", "bridge");
				nvram_pf_set(prefix, "enable", "1");

				/* bridge */
				eval("brctl", "delif", "br0", ethPort2);
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("brctl", "addif", br_dev, wanVlanDev1);
				eval("brctl", "addif", br_dev, ethPort2);
				eval("ifconfig", br_dev, "allmulti", "up"); // bridge interface must be up later, or brX can't receive packets
			}
			else {  /* Nomo case. */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				/* Forward packets from wan to vlanDev2 (untag) */
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				/* rx push tag */
				/* Set VOIP PRIO */
				if (nvram_invmatch("switch_wan2prio", "0"))
					eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--set-rxif", vlanDev2, "--rule-append");
				else
					eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-rxif", vlanDev2, "--rule-append");
				/* tx pop tag */
				eval("vlanctl", "--if", ethPort2, "--tx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--filter-txif", vlanDev2, "--pop-tag", "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
		} else if (nvram_match("switch_stb_x", "4")) {
			/* config ethPort1 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward wan vid packets between wan & vlanDev1, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("brctl", "delif", "br0", ethPort1);
				eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
				/* pop tag (lan bridge) */
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--pop-tag", "--set-rxif", vlanDev1, "--rule-append");
				/* tx push tag */
				eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "hinet")) {
				/* Just forward packets between wan & vlanDev1, no tag */
				eval("brctl", "delif", "br0", ethPort1);
				eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
				/* no tag (lan bridge) */
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--set-rxif", vlanDev1, "--rule-append");
				eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--rule-append");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "none")) {
				if (model == MODEL_GTAC5300) {
					if (nvram_match("iptv_port_settings", "12")) {
						sprintf(ethPort1, "eth1");
						sprintf(vlanDev1, "eth1.v0");
					} else {
						sprintf(ethPort1, "eth3");
						sprintf(vlanDev1, "eth3.v0");
					}
				}
				/* Just forward packets between wan & vlanDev1, no tag */
				eval("brctl", "delif", "br0", ethPort1);
				eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
				/* no tag (lan bridge) */
				eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--set-rxif", vlanDev1, "--rule-append");
				eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--rule-append");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "free")) {
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				sprintf(port_id, "%d", iptv_vid);
				eval("brctl", "delif", "br0", ethPort1);
				eval("vconfig", "add", wan_if, port_id);
				snprintf(wan_dev, sizeof(wan_dev), "%s.%d", wan_if, iptv_vid);
				eval("vconfig", "add", ethPort1, port_id);
				eval("ifconfig", wan_dev, "allmulti", "up");
				snprintf(vlanDev1, sizeof(vlanDev1), "%s.%d", ethPort1, iptv_vid);
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, wan_dev);
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "addif", "br0", ethPort1);
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
			else {  /* Nomo case, untag it. */
				/* config ethPort1 = IPTV */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				/* Forward packets from wan to vlanDev1 (untag) */
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort1);
				eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
				/* rx push tag */
				/* Set IPTV PRIO */
				if (nvram_invmatch("switch_wan1prio", "0"))
					eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan1prio"), "0", "--set-rxif", vlanDev1, "--rule-append");
				else
					eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-rxif", vlanDev1, "--rule-append");
				/* tx pop tag */
				eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--filter-txif", vlanDev1, "--pop-tag", "--rule-append");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				if (nvram_match("switch_wantag", "unifi_home"))
					eval("ethswctl", "-c", "hwstp",  "-i",  vlanDev1, "-o",  "disable");
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
			}
		} else if (nvram_match("switch_stb_x", "5") && nvram_match("switch_wantag", "none")) {
			if (model == MODEL_RTAC86U || model == MODEL_RTAX88U || model == MODEL_BC109 || model == MODEL_BC105 || model == MODEL_EBG19 || model == MODEL_EBG15 || model == MODEL_EBP15 || model == MODEL_RTAX92U || model == MODEL_RTAX56U || model == MODEL_GTAX11000 || model == MODEL_RTAX86U || model == MODEL_RTAX68U || model == MODEL_RTAC68U_V4 || model == MODEL_GTAXE11000) {
				sprintf(ethPort1, "eth3");
				sprintf(vlanDev1, "eth3.v0");
				sprintf(ethPort2, "eth4");
				sprintf(vlanDev2, "eth4.v0");
			}
			else if (model == MODEL_RTAX95Q || model == MODEL_XT8PRO || model == MODEL_BT12 || model == MODEL_BQ16 || model == MODEL_BQ16_PRO || model == MODEL_BM68 || model == MODEL_XT8_V2 || model == MODEL_RTAXE95Q || model == MODEL_ET8PRO || model == MODEL_ET8_V2 || model == MODEL_GTAX6000 || model == MODEL_GTAX11000_PRO || model == MODEL_GTAXE16000 || model == MODEL_RTAX86U_PRO || model == MODEL_RTAX88U_PRO || model == MODEL_GTBE98 || model == MODEL_RTBE96U || model == MODEL_RTBE88U || model == MODEL_RTBE86U) {
				/* exclude ET12/XT12 due to GPY211 LAN port can't be iptv/voip port */
				sprintf(ethPort1, "eth1");
				sprintf(vlanDev1, "eth1.v0");
				sprintf(ethPort2, "eth2");
				sprintf(vlanDev2, "eth2.v0");
			}
			else if (model == MODEL_BT10) {
				/* three ports special mapping */
				sprintf(ethPort1, "eth0");
				sprintf(vlanDev1, "eth0.v0");
				sprintf(ethPort2, "eth1");
				sprintf(vlanDev2, "eth1.v0");
			}
			else if (model == MODEL_TUFAX3000_V2 || model == MODEL_RTAXE7800) {
				if (!nvram_get_int("wans_extwan")) {
					sprintf(ethPort1, "eth1");
					sprintf(vlanDev1, "eth1.v0");
				} else {
					sprintf(ethPort1, "eth0");
					sprintf(vlanDev1, "eth0.v0");
				}
				sprintf(ethPort2, "eth1");
				sprintf(vlanDev2, "eth1.v0");
			}
			else if (model == MODEL_GT10) {
				if (!nvram_get_int("wans_extwan")) {
					sprintf(ethPort1, "eth1");
					sprintf(vlanDev1, "eth1.v0");
				} else {
					sprintf(ethPort1, "eth0");
					sprintf(vlanDev1, "eth0.v0");
				}
				sprintf(ethPort2, "eth2");
				sprintf(vlanDev2, "eth2.v0");
			}
			else if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400) {
#if defined(RTAX82_XD6) || defined(XD6_V2)
				sprintf(ethPort1, "eth2");
				sprintf(vlanDev1, "eth2.v0");
				sprintf(ethPort2, "eth1");
				sprintf(vlanDev2, "eth1.v0");
#else
				sprintf(ethPort1, "eth3");
				sprintf(vlanDev1, "eth3.v0");
				sprintf(ethPort2, "eth2");
				sprintf(vlanDev2, "eth2.v0");
#endif
			}
			/* Just forward packets between wan & vlanDev1, no tag */
			eval("brctl", "delif", "br0", ethPort1);
			eval("brctl", "delif", "br0", ethPort2);
			eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
			eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
			/* pop tag (lan bridge) */
			eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--set-rxif", vlanDev1, "--rule-append");
			eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--rule-append");
			eval("ifconfig", vlanDev1, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev1);
			eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--set-rxif", vlanDev2, "--rule-append");
			eval("vlanctl", "--if", ethPort2, "--tx", "--tags", "0", "--filter-txif", vlanDev2, "--rule-append");
			eval("ifconfig", vlanDev2, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev2);
			// enable softswitch for vlan forwarding(all internal switch)
			if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_RTAX5400) {
				eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
				eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config ethPort2 = VoIP */
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between WAN & vlanDev2, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "meo_br")) {
				/* Just forward packets between WAN & vlanDev2, untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("brctl", "delif", "br0", ethPort2);
				eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
				eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--set-rxif", vlanDev2, "--rule-append");
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				// enable softswitch for vlan forwarding(all internal switch)
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
			}
			else if (nvram_match("switch_wantag", "none")) {
				if (model == MODEL_RTAC86U || model == MODEL_RTAX88U || model == MODEL_BC109 || model == MODEL_BC105 || model == MODEL_EBG19 || model == MODEL_EBG15 || model == MODEL_EBP15 || model == MODEL_RTAX92U || model == MODEL_RTAX56U || model == MODEL_GTAX11000 || model == MODEL_RTAX86U || model == MODEL_RTAX68U || model == MODEL_RTAC68U_V4 || model == MODEL_GTAXE11000) {
					sprintf(ethPort3, "eth1");
					sprintf(vlanDev3, "eth1.v0");
					sprintf(ethPort4, "eth2");
					sprintf(vlanDev4, "eth2.v0");
				}
				else if (model == MODEL_RTAX95Q || model == MODEL_XT8PRO || model == MODEL_BT12 || model == MODEL_BQ16 || model == MODEL_BQ16_PRO || model == MODEL_BM68 || model == MODEL_XT8_V2 || model == MODEL_RTAXE95Q || model == MODEL_ET8PRO || model == MODEL_ET8_V2 || model == MODEL_GTAX6000 || model == MODEL_GTAX11000_PRO || model == MODEL_GTAXE16000 || model == MODEL_RTAX86U_PRO || model == MODEL_GT10 || model == MODEL_RTAX88U_PRO || model == MODEL_GTBE98 || model == MODEL_RTBE96U || model == MODEL_RTBE88U || model == MODEL_RTBE86U) {
				/* exclude ET12/XT12 due to GPY211 LAN port can't be iptv/voip port */
					sprintf(ethPort3, "eth2");
					sprintf(vlanDev3, "eth2.v0");
					sprintf(ethPort4, "eth3");
					sprintf(vlanDev4, "eth3.v0");
				}
				else if (model == MODEL_TUFAX3000_V2 || model == MODEL_RTAXE7800) {
					sprintf(ethPort3, "eth3");
					sprintf(vlanDev3, "eth3.v0");
					sprintf(ethPort4, "eth4");
					sprintf(vlanDev4, "eth4.v0");
				}
				else if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_RTAX5400) {
					sprintf(ethPort3, "eth1");
					sprintf(vlanDev3, "eth1.v0");
					sprintf(ethPort4, "eth0");
					sprintf(vlanDev4, "eth0.v0");
				}
				else if (model == MODEL_BT10) {
					/* three ports special mapping */
					sprintf(ethPort3, "eth1");
					sprintf(vlanDev3, "eth1.v0");
					sprintf(ethPort4, "eth2");
					sprintf(vlanDev4, "eth2.v0");
				}
				/* Just forward packets between wan & vlanDev1, no tag */
				eval("brctl", "delif", "br0", ethPort3);
				eval("brctl", "delif", "br0", ethPort4);
				eval("vlanctl", "--mcast", "--if-create", ethPort3, "0");
				eval("vlanctl", "--mcast", "--if-create", ethPort4, "0");
				/* pop tag (lan bridge) */
				eval("vlanctl", "--if", ethPort3, "--rx", "--tags", "0", "--set-rxif", vlanDev3, "--rule-append");
				eval("vlanctl", "--if", ethPort3, "--tx", "--tags", "0", "--filter-txif", vlanDev3, "--rule-append");
				eval("ifconfig", vlanDev3, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev3);
				eval("vlanctl", "--if", ethPort4, "--rx", "--tags", "0", "--set-rxif", vlanDev4, "--rule-append");
				eval("vlanctl", "--if", ethPort4, "--tx", "--tags", "0", "--filter-txif", vlanDev4, "--rule-append");
				eval("ifconfig", vlanDev4, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev4);
				if (model == MODEL_RTAX58U || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_RTAX5400) {
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort3, "-o", "enable");
					eval("ethswctl", "-c", "softswitch",  "-i",  ethPort4, "-o", "enable");
				}
			}
			else {
				if (voip_vid) {
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					/* Forward packets from wan:eth0 to vlanDev2 (untag) */
					eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
					eval("brctl", "delif", "br0", ethPort2);
					eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
					/* rx push tag */
					/* Set VOIP PRIO */
					if (nvram_invmatch("switch_wan2prio", "0"))
						eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--set-rxif", vlanDev2, "--rule-append");
					else
						eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-rxif", vlanDev2, "--rule-append");
					/* tx pop tag */
					eval("vlanctl", "--if", ethPort2, "--tx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--filter-txif", vlanDev2, "--pop-tag", "--rule-append");
					eval("ifconfig", vlanDev2, "allmulti", "up");
					eval("brctl", "addif", br_dev, vlanDev2);
					// enable softswitch for vlan forwarding(all internal switch)
					if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
						eval("ethswctl", "-c", "softswitch",  "-i",  ethPort2, "-o", "enable");
				}
			}
			/* config ethPort1 = IPTV */
			if (iptv_vid) {
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (nvram_match("switch_wantag", "meo_br")) {
					/* Just forward packets between WAN & vlanDev1, without untag */
					eval("brctl", "delif", "br0", ethPort1);
					eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
					/* rx pop tag */
					/* Set VOIP PRIO */
					eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--pop-tag", "--set-rxif", vlanDev1, "--rule-append");
					/* tx push tag */
					if (nvram_invmatch("switch_wan2prio", "0"))
						eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--rule-append");
					else
						eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");

					eval("ifconfig", vlanDev1, "allmulti", "up");
					eval("brctl", "addif", br_dev, vlanDev1);
					// enable softswitch for vlan forwarding(all internal switch)
					if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
						eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
				}
				else {
					/* Forward packets from wan:eth0 to vlanDev1 (untag) */
					eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", wanVlanDev, "--rule-append");
					eval("brctl", "delif", "br0", ethPort1);
					eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
					/* rx push tag */
					/* Set IPTV PRIO */
					if (nvram_invmatch("switch_wan1prio", "0"))
						eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan1prio"), "0", "--set-rxif", vlanDev1, "--rule-append");
					else
						eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--push-tag", "--set-vid", vlan_entry, "0", "--set-rxif", vlanDev1, "--rule-append");
					/* tx pop tag */
					eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--filter-txif", vlanDev1, "--pop-tag", "--rule-append");
					eval("ifconfig", vlanDev1, "allmulti", "up");
					eval("brctl", "addif", br_dev, vlanDev1);
					// enable softswitch for vlan forwarding(all internal switch)
					if (model == MODEL_RTAX58U || model == MODEL_RTAX82_XD6S || model == MODEL_RTAX82U_V2 || model == MODEL_TUFAX5400_V2 || model == MODEL_XD6_V2 || model == MODEL_RTAX5400)
						eval("ethswctl", "-c", "softswitch",  "-i",  ethPort1, "-o", "enable");
				}
			}
		}
#ifdef RTAC86U
		else if (nvram_get_int("switch_stb_x") == SWCFG_PASSTHROUGH && nvram_match("switch_wantag", "none")) {
			switch_stb = -1;

			sprintf(ethPort1, "eth1");
			sprintf(vlanDev1, "eth1.v0");
			sprintf(ethPort2, "eth2");
			sprintf(vlanDev2, "eth2.v0");
			sprintf(ethPort3, "eth3");
			sprintf(vlanDev3, "eth3.v0");
			sprintf(ethPort4, "eth4");
			sprintf(vlanDev4, "eth4.v0");

			/* Just forward packets between wan & vlanDev1, no tag */
			eval("brctl", "delif", "br0", ethPort1);
			eval("brctl", "delif", "br0", ethPort2);
			eval("brctl", "delif", "br0", ethPort3);
			eval("brctl", "delif", "br0", ethPort4);
			eval("vlanctl", "--mcast", "--if-create", ethPort1, "0");
			eval("vlanctl", "--mcast", "--if-create", ethPort2, "0");
			eval("vlanctl", "--mcast", "--if-create", ethPort3, "0");
			eval("vlanctl", "--mcast", "--if-create", ethPort4, "0");
			/* pop tag (lan bridge) */
			eval("vlanctl", "--if", ethPort1, "--rx", "--tags", "0", "--set-rxif", vlanDev1, "--rule-append");
			eval("vlanctl", "--if", ethPort1, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--rule-append");
			eval("ifconfig", vlanDev1, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev1);
			eval("vlanctl", "--if", ethPort2, "--rx", "--tags", "0", "--set-rxif", vlanDev2, "--rule-append");
			eval("vlanctl", "--if", ethPort2, "--tx", "--tags", "0", "--filter-txif", vlanDev2, "--rule-append");
			eval("ifconfig", vlanDev2, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev2);
			eval("vlanctl", "--if", ethPort3, "--rx", "--tags", "0", "--set-rxif", vlanDev3, "--rule-append");
			eval("vlanctl", "--if", ethPort3, "--tx", "--tags", "0", "--filter-txif", vlanDev3, "--rule-append");
			eval("ifconfig", vlanDev3, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev3);
			eval("vlanctl", "--if", ethPort4, "--rx", "--tags", "0", "--set-rxif", vlanDev4, "--rule-append");
			eval("vlanctl", "--if", ethPort4, "--tx", "--tags", "0", "--filter-txif", vlanDev4, "--rule-append");
			eval("ifconfig", vlanDev4, "allmulti", "up");
			eval("brctl", "addif", br_dev, vlanDev4);
		}
#endif

#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				if (nvram_invmatch("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", wan_dev, "allmulti", "up");
				eval("bcmmcastctl", "mode", "-i",  "br0",  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  "br0",  "-p", "2",  "-m", "0");
			}
			else if (nvram_match("switch_wantag", "hinet_mesh")) {
				unsigned char eabuf[ETHER_ADDR_LEN];
				char macaddr[32];
				int iptv_unit = WAN_UNIT_IPTV;
				char prefix[] = "wanXXX_";
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev, "--mcast", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev, "--filter-vlan-dev-mac-addr", "1", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--rule-append");
				set_wan_phy("");
				add_wan_phy(wanVlanDev);
				nvram_set("wan0_ifname", wanVlanDev);
				nvram_set("wan0_gw_ifname", wanVlanDev);

				snprintf(wanVlanDev1, sizeof(wanVlanDev1), "%s.v1", wan_if);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev1, "--mcast", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev1, "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev1, "--rule-append");
				snprintf(prefix, sizeof(prefix), "wan%d_", iptv_unit);
				nvram_pf_set(prefix, "ifname", wanVlanDev1);
				nvram_set("iptv_ifname", wanVlanDev1);

				ether_atoe(get_wan_hwaddr(), eabuf);
				eabuf[ETHER_ADDR_LEN - 1] += 1;
				ether_etoa(eabuf, macaddr);
				nvram_pf_set(prefix, "hwaddr", macaddr);
				nvram_pf_set(prefix, "proto", "bridge");
				nvram_pf_set(prefix, "enable", "1");
			}
		}
		if (switch_stb >= 8) {
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				if (nvram_invmatch("switch_wan2prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
				eval("ifconfig", wan_dev, "allmulti", "up");
			}
		}
#endif
		break;

#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) || defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(GTBE19000) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO) || defined(GTBE19000AI) || defined(GTBE96_AI) || defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
	case MODEL_RTBE82M:
	case MODEL_GSBE18000:
		for (i=0; i<4; ++i) {
			if (ret = mxls_init()) {
				printf("%s: mxls_init failed(%d)...%d, reinit again.\n", __func__, ret, i);
				sleep(1);
			} else {
				printf("%s: mxls_init ok...%d\n", __func__, i);
				break;
			}
		}
		ctp_ext_vlan_print();
#endif
	case MODEL_RTAX55:
	case MODEL_RTAX58U_V2:
	case MODEL_RTAX3000N:
	case MODEL_BR63:
	case MODEL_GTBE98:
	case MODEL_GTBE98_PRO:
	case MODEL_GTBE96:
	case MODEL_RTBE58U:
	case MODEL_RTBE58U_V2:
	case MODEL_RTBE92U:
	case MODEL_RTBE95U:
	case MODEL_RTBE82U:
	case MODEL_RTBE58U_PRO:
	case MODEL_GTBE19000:
	case MODEL_GTBE19000AI:
	case MODEL_GTBE96_AI:
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
		if(!is_rtl8372_boardid())
			break;
#endif
				/* eth0 eth1 CPU(LAN)	*/	
				/* WAN  LAN  P17	*/
				/*	rtkswitch	*/
		sprintf(wan_if, "eth0");
		sprintf(wanVlanDev, "eth0.v0");
		sprintf(vlanDev1, "eth0.v1");
		sprintf(vlanDev2, "eth0.v2");

		_dprintf("%s, switch_wantag=%s, wan_vid=%d, switch_stb=%d\n", __func__, nvram_safe_get("switch_wantag"), wan_vid, switch_stb);

		/* Using vlanctl to handle vlan forwarding */
		if ((!nvram_match("switch_wantag", "") && (wan_vid || switch_stb > 0))
		 && !nvram_match("switch_wantag", "unifi_biz_voip") && !nvram_match("switch_wantag", "unifi_home")
		 && !nvram_match("switch_wantag", "superonline")
		 && !nvram_match("switch_wantag", "m1_fiber") && !nvram_match("switch_wantag", "maxis_fiber")
		 && !nvram_match("switch_wantag", "maxis_fiber_sp") && !nvram_match("switch_wantag", "hinet_mesh")
		 && !nvram_match("switch_wantag", "maxis_cts") && !nvram_match("switch_wantag", "maxis_sacofa") && !nvram_match("switch_wantag", "maxis_tnb")
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
		 && !nvram_match("switch_wantag", "free")
		 && !nvram_match("switch_wantag", "meo_br")
#endif
		) {
			/* config wan port or bridge hinet IPTV traffic */
			sprintf(port_id, "%d", wan_vid);
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if (nvram_match("switch_wantag", "movistar") || 
				(switch_stb == 0 && wan_vid)) {
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", wan_vid);
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);
			}
			else {
				/* Handle wan(internet) vlan traffic */
				eval("vlanctl", "--mcast", "--if-create", wan_if, "0");
				if ((!wan_vid || nvram_match("switch_wantag", "none")) && switch_stb > 0) { /* bridge IPTV traffic */
					/* set wan vlan interface */
					eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev, "--rule-append");
					eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--rule-append");
				} else {
					/* pop tag for entering CPU */
					eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", port_id, "0", "--pop-tag", "--set-rxif", wanVlanDev, "--rule-append");
					/* tx push tag */
					/* Set Wan PRIO */
					if (nvram_invmatch("switch_wan0prio", "0")) {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", port_id, "0", "--set-pbits", nvram_get("switch_wan0prio"), "0", "--rule-append");
					}
					else {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", port_id, "0", "--rule-append");
					}
				}
				eval("ifconfig", wanVlanDev, "allmulti", "up");

				/*
					multicast iptv no need to forward vlan packets from wan to lan
				*/
				if (switch_stb <= 6) {
					/* Add wan bridge */
					sprintf(br_dev, "br%d", IPTV_BR_INDEX);
					eval("brctl", "addbr", br_dev);
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
					eval("brctl", "stp", br_dev, "on");
					eval("ifconfig", br_dev, "up");
					eval("brctl", "addif", br_dev, wanVlanDev);
					set_wan_phy("");
					add_wan_phy(br_dev);
					nvram_set("wan0_ifname", br_dev);
					nvram_set("wan0_gw_ifname", br_dev);
				} else {
					set_wan_phy("");
					sprintf(wan_dev, wanVlanDev);
					add_wan_phy(wan_dev);
					nvram_set("wan0_ifname", wanVlanDev);
					nvram_set("wan0_gw_ifname", wanVlanDev);
				}
			}
		}

		_dprintf("%s, chk wan: wan_dev:%s, wan0_ifname=%s\n", __func__, wan_dev, nvram_safe_get("wan0_ifname"));

		if (nvram_match("switch_stb_x", "1") && nvram_match("switch_wantag", "none")) {
			/* add vlan 1 to separate LAN and WAN bridge */
			eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
			__setup_vlan(1, 0, 0x00200020); //LAN1
			__setup_vlan(0, 0, 0x00D000D0); //no-tag fwd mask except LAN1
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
			__setup_vlan(1, 0, 0x00100010); //LAN1
			__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN1
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
			__setup_vlan(1, 0, 0x00080008); //LAN1
			__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN1
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
			__setup_vlan(1, 0, 0x00100010); //LAN1
			__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN1
#elif defined(RTBE82M)
			__setup_vlan(0, 0, 0x00380038); //no-tag fwd mask except LAN1
			__setup_vlan(1, 0, 0x00040004); //LAN1
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			__setup_vlan(0, 0, 0x01CE01CE); //no-tag fwd mask except LAN1
			__setup_vlan(1, 0, 0x00200020); //LAN1
#else
			__setup_vlan(1, 0, 0x00010001); //LAN1
			__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN1
#endif
			eval("vconfig", "add", "eth1", "1");
			eval("ifconfig", "vlan1", "allmulti", "up");
			eval("brctl", "addif", br_dev, "vlan1");
			eval("brctl", "addif", "br0", "eth1");
		} else if (nvram_match("switch_stb_x", "2") && nvram_match("switch_wantag", "none")) {
			/* add vlan 1 to separate LAN and WAN bridge */
			eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
			__setup_vlan(1, 0, 0x00400040); //LAN2
			__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN2
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
			__setup_vlan(1, 0, 0x00200020); //LAN2
			__setup_vlan(0, 0, 0x00D000D0); //no-tag fwd mask except LAN2
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
			__setup_vlan(1, 0, 0x00040004); //LAN2
			__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN2
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
			__setup_vlan(1, 0, 0x00080008); //LAN2
			__setup_vlan(0, 0, 0x00160016); //no-tag fwd mask except LAN2
#elif defined(RTBE82M)
			__setup_vlan(0, 0, 0x00340034); //no-tag fwd mask except LAN2
			__setup_vlan(1, 0, 0x00080008); //LAN2
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			__setup_vlan(0, 0, 0x01AE01AE); //no-tag fwd mask except LAN2
			__setup_vlan(1, 0, 0x00400040); //LAN2
#else
			__setup_vlan(1, 0, 0x00020002); //LAN2
			__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN2
#endif
			eval("vconfig", "add", "eth1", "1");
			eval("ifconfig", "vlan1", "allmulti", "up");
			eval("brctl", "addif", br_dev, "vlan1");
			eval("brctl", "addif", "br0", "eth1");
		} else if (nvram_match("switch_stb_x", "3")) {
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			if (nvram_match("switch_wantag", "vodafone")) {	// mxl vodfone
				if (nvram_match("switch_wantag", "vodafone")) {
					wan_vid = 100;
					iptv_vid = 101;
				}

                                sprintf(br_dev, "br%d", IPTV_BR_INDEX);
                                /* wan0 is br101 */
                                set_wan_phy("");
                                add_wan_phy(br_dev);
                                nvram_set("wan0_ifname", br_dev);
                                nvram_set("wan0_gw_ifname", br_dev);

                                eval("brctl", "addbr", br_dev);
                                eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
                                eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
                                eval("ifconfig", br_dev, "allmulti", "up");

                                eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");

#if defined(RTBE82M)
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
				if (nvram_match("voip_untag", "1") || nvram_match("switch_wantag", "vodafone")) {
                                       	__setup_vlan(100, 0, 0x00000062); // LAN4 tag, wan-port-cpu mbr

					if (iptv_vid) {	// 101
                                       		__setup_vlan(iptv_vid, 0, 0x00000062); // LAN4 tag, wan-port-cpu mbr
					}

					if (voip_vid && voip_vid != iptv_vid) {	// 105
                                       		__setup_vlan(voip_vid, 0, 0x00100072); // LAN3 untag, LAN4 tag
					}
				}
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
				if (nvram_match("voip_untag", "1") || nvram_match("switch_wantag", "vodafone")) {
                                       	__setup_vlan(100, 0, 0x00000310); // LAN4 tag, wan-port-cpu mbr

					if (iptv_vid) {	// 101
                                       		__setup_vlan(iptv_vid, 0, 0x00000310); // LAN4 tag, wan-port-cpu mbr
					}

					if (voip_vid && voip_vid != iptv_vid) {	// 105
                                       		__setup_vlan(voip_vid, 0, 0x00800390); // LAN3 untag, LAN4 tag
					}
				}
#endif
			} 
#else 
			if (nvram_match("switch_wantag", "vodafone")) {
				eval("brctl", "delif", "br0", "eth1");
				system("rtkswitch 40 1"); //leave tag case
				/* handle special case WAN vlan forwarding specific LAN*/
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000010); //LAN4 leave tag
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000080); //LAN4 leave tag
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000001); //LAN4 leave tag
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000002); //LAN4 leave tag
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000020); //LAN4 leave tag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000100); //LAN4 leave tag
#else
				__setup_vlan(100, nvram_get_int("switch_wan0prio"), 0x00000008); //LAN4 leave tag
#endif
				eval("vconfig", "add", "eth1", "100");
				eval("ifconfig", "vlan100", "allmulti", "up");
				eval("brctl", "addif", br_dev, "vlan100");
				vlan_forwarding(101, 0, 4, 0); 
			}
#endif
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				system("rtkswitch 40 1"); //leave tag case
				/* set WAN VLAN */
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");

				sprintf(port_id, "%d", wan_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.%d", wan_vid);
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);

#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.%d", voip_vid);
				if (!nvram_match("switch_wan2prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan2prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "delif", "br0", "eth1");
				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev1, "eth1.%d", voip_vid);
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(voip_vid, 0, 0x00000080); //LAN3 leave tag
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(voip_vid, 0, 0x00000040); //LAN3 leave tag
				__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(voip_vid, 0, 0x00000002); //LAN3 leave tag
				__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(voip_vid, 0, 0x00000004); //LAN3 leave tag
				__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x002C002C); //no-tag fwd mask except LAN3
				__setup_vlan(voip_vid, 0, 0x00000052); //LAN3 leave tag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x016E016E); //no-tag fwd mask except LAN3
				__setup_vlan(voip_vid, 0, 0x00000290); //LAN3 leave tag
#else
				__setup_vlan(voip_vid, 0, 0x00000004); //LAN3 leave tag
				__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				eval("brctl", "addif", "br0", "eth1");
#endif
			} else if (nvram_match("switch_wantag", "maxis_fiber")) {
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				/* set WAN VLAN */
				sprintf(port_id, "%d", wan_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.%d", wan_vid);
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				eval("brctl", "delif", "br0", "eth1");
				/* handle VoIP VLAN */
				sprintf(port_id, "%d", 821);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.821");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev1, "eth1.821");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);

				sprintf(port_id, "%d", 822);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.822");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX + 1);
				eval("brctl", "addbr", br_dev);
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev1, "eth1.822");
				eval("ifconfig", vlanDev1, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);

				system("rtkswitch 40 1"); //leave tag case
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(821, 0, 0x0000080); //LAN3 leave tag
				__setup_vlan(822, 0, 0x0000080); //LAN3 leave tag
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(821, 0, 0x0000040); //LAN3 leave tag
				__setup_vlan(822, 0, 0x0000040); //LAN3 leave tag
				__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(821, 0, 0x0000002); //LAN3 leave tag
				__setup_vlan(822, 0, 0x0000002); //LAN3 leave tag
				__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(821, 0, 0x0000004); //LAN3 leave tag
				__setup_vlan(822, 0, 0x0000004); //LAN3 leave tag
				__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
#elif defined(RTBE82M)
				__setup_vlan(0,   0, 0x002C002C); //no-tag fwd mask except LAN3
				__setup_vlan(821, 0, 0x00000052); //LAN3 leave tag
				__setup_vlan(822, 0, 0x00000052); //LAN3 leave tag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0,   0, 0x016E016E); //no-tag fwd mask except LAN3
				__setup_vlan(821, 0, 0x00000290); //LAN3 leave tag
				__setup_vlan(822, 0, 0x00000290); //LAN3 leave tag
#else
				__setup_vlan(821, 0, 0x00000004); //LAN3 leave tag
				__setup_vlan(822, 0, 0x00000004); //LAN3 leave tag
				__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				eval("brctl", "addif", "br0", "eth1");
#endif
			}
			else if (nvram_match("switch_wantag", "unifi_biz_voip")) {
				/* set WAN setting */
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				sprintf(port_id, "%d", wan_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.500");
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				sprintf(port_id, "%d", voip_vid);
				/* set WAN STB VLAN */
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.400");
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
				/* set IPTV port VLAN */
				eval("brctl", "delif", "br0", "eth1");
				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev1, "eth1.400");
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "addif", br_dev, wan_dev);
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(400, 0, 0x00800080); //LAN3 untag
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(400, 0, 0x00400040); //LAN3 untag
				__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(400, 0, 0x00020002); //LAN3 untag
				__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(400, 0, 0x00040004); //LAN3 untag
				__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x002C002C); //no-tag fwd mask except LAN3
				__setup_vlan(400, 0, 0x00100052); //LAN3 untag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x016E016E); //no-tag fwd mask except LAN3
				__setup_vlan(400, 0, 0x00800290); //LAN3 untag
#else
				__setup_vlan(400, 0, 0x00040004); //LAN3 untag
				__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				eval("brctl", "addif", "br0", "eth1");
#endif
			}
			else if (nvram_match("switch_wantag", "none")) {
				/* add vlan 1 to separate LAN and WAN bridge */
				eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(1, 0, 0x00800080); //LAN3
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(1, 0, 0x00400040); //LAN3
				__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(1, 0, 0x00020002); //LAN3
				__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(1, 0, 0x00040004); //LAN3
				__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x002C002C); //no-tag fwd mask except LAN3
				__setup_vlan(1, 0, 0x00100010); //LAN3
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x016E016E); //no-tag fwd mask except LAN3
				__setup_vlan(1, 0, 0x00800080); //LAN3
#else
				__setup_vlan(1, 0, 0x00040004); //LAN3
				__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
				eval("vconfig", "add", "eth1", "1");
				eval("ifconfig", "vlan1", "allmulti", "up");
				eval("brctl", "addif", br_dev, "vlan1");
				eval("brctl", "addif", "br0", "eth1");
			}
			else {  /* Nomo case. */
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				if (!nvram_match("switch_wantag", "vodafone")) 		// except vodafone
				{	// mxl norm/manual cases
					eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");

					 /* wan0 is br101 */
                                	set_wan_phy("");
                                	add_wan_phy(br_dev);
					nvram_set("wan0_ifname", br_dev);
					nvram_set("wan0_gw_ifname", br_dev);

					eval("brctl", "addbr", br_dev);
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
					eval("ifconfig", br_dev, "allmulti", "up");

#if defined(RTBE82M)
					__setup_vlan(0, 0, 0x002C002C); //no-tag fwd mask except LAN3
					__setup_vlan(voip_vid, 0, 0x00100052); //LAN3 untag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
					__setup_vlan(0, 0, 0x016E016E); //no-tag fwd mask except LAN3
					__setup_vlan(voip_vid, 0, 0x00800290); //LAN3 untag
#endif
				}
#else
				if (nvram_match("switch_wantag", "vodafone")) {	// non-mxl
					sprintf(vlan_entry, "%d", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					eval("vlanctl", "--mcast", "--if-create", wan_if, "2");
					eval("ifconfig", "eth0.v2", "allmulti", "up");
					eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--filter-vid", vlan_entry, "0", "--set-rxif", "eth0.v2", "--rule-append");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
					__setup_vlan(voip_vid, voip_prio, 0x00800090); //LAN3 untag LAN4 leave tag
					__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
					__setup_vlan(voip_vid, voip_prio, 0x004000C0); //LAN3 untag LAN4 leave tag
					__setup_vlan(0, 0, 0x00300030); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
					__setup_vlan(voip_vid, voip_prio, 0x00020003); //LAN3 untag LAN4 leave tag
					__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
					__setup_vlan(voip_vid, voip_prio, 0x00040006); //LAN3 untag LAN4 leave tag
					__setup_vlan(0, 0, 0x00180018); //no-tag fwd mask except LAN3 and LAN4
#else
					__setup_vlan(voip_vid, voip_prio, 0x0004000C); //LAN3 untag LAN4 leave tag
					__setup_vlan(0, 0, 0x00030003); //no-tag fwd mask except LAN3 and LAN4
#endif
					eval("vconfig", "add", "eth1", vlan_entry);
					sprintf(vlanDev2, "vlan%d", voip_vid);
					eval("ifconfig", vlanDev2, "allmulti", "up");
					sprintf(br_dev, "br%d", IPTV_BR_INDEX + 2);
					eval("brctl", "addbr", br_dev);
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
					eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
					eval("ifconfig", br_dev, "up");
					eval("brctl", "addif", br_dev, "eth0.v2");
					eval("brctl", "addif", br_dev, vlanDev2);
					eval("brctl", "addif", "br0", "eth1");
				} else {
					eval("brctl", "delif", "br0", "eth1");
					/* Forward packets from wan to vlanDev2 (untag) */

					vlan_forwarding(voip_vid, voip_prio, switch_stb, 1);
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
					__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN3
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
					__setup_vlan(0, 0, 0x00B000B0); //no-tag fwd mask except LAN3
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
					__setup_vlan(0, 0, 0x000D000D); //no-tag fwd mask except LAN3
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
					__setup_vlan(0, 0, 0x001A001A); //no-tag fwd mask except LAN3
#else
					__setup_vlan(0, 0, 0x000B000B); //no-tag fwd mask except LAN3
#endif
					eval("brctl", "addif", "br0", "eth1");

					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("stb_x(%s) : vlan entry: %s\n", nvram_safe_get("switch_stb_x"), vlan_entry);
					/* tx push tag */
					/* Set Wan PRIO */
					if (nvram_invmatch("switch_wan2prio", "0")) {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--rule-append");
					}
					else {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
					}
				}
#endif
			}
		} else if (nvram_match("switch_stb_x", "4")) {
			if (nvram_match("switch_wantag", "hinet") || nvram_match("switch_wantag", "none")) {
				/* add vlan 1 to separate LAN and WAN bridge */
				eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(1, 0, 0x00100010); //LAN4
				__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(1, 0, 0x00800080); //LAN4
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(1, 0, 0x00010001); //LAN4
				__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(1, 0, 0x00020002); //LAN4
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
				__setup_vlan(1, 0, 0x00200020); //LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
				__setup_vlan(1, 0, 0x01000100); //LAN4
#else
				__setup_vlan(1, 0, 0x00080008); //LAN4
				__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
				eval("vconfig", "add", "eth1", "1");
				eval("ifconfig", "vlan1", "allmulti", "up");
				eval("brctl", "addif", br_dev, "vlan1");
				eval("brctl", "addif", "br0", "eth1");
			}
			else if (nvram_match("switch_wantag", "superonline")) {
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
#if 0
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
                                /* wan0 is br101 */
                                set_wan_phy("");
                                add_wan_phy(br_dev);
                                nvram_set("wan0_ifname", br_dev);
                                nvram_set("wan0_gw_ifname", br_dev);
#endif
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)

				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.103");
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");
		
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");

				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "delif", "br0", "eth1");
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(iptv_vid, 0, 0x00100010); //LAN4
				__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(iptv_vid, 0, 0x00800080); //LAN4
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(iptv_vid, 0, 0x00010001); //LAN4
				__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(iptv_vid, 0, 0x00020002); //LAN4
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
				__setup_vlan(iptv_vid, 0, 0x00200062); //LAN4 untag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
				__setup_vlan(iptv_vid, 0, 0x01000310); //LAN4 untag
#else
				__setup_vlan(iptv_vid, 0, 0x00080008); //LAN4
				__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)				
				eval("vconfig", "add", "eth1", port_id);
				eval("ifconfig", "eth1.103", "allmulti", "up");
				eval("brctl", "addif", br_dev, "eth1.103");
				eval("brctl", "addif", "br0", "eth1");
#endif
			}
			/* use vconfig only for STB compatability */
			else if (nvram_match("switch_wantag", "unifi_home")) {
				/* set WAN setting */
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				sprintf(port_id, "%d", wan_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.500");
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
				set_wan_phy("");
				add_wan_phy(wan_dev);
				nvram_set("wan0_ifname", wan_dev);
				nvram_set("wan0_gw_ifname", wan_dev);

#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i", br_dev, "-p", "1", "-m", "0");
				eval("bcmmcastctl", "mode", "-i", br_dev, "-p", "2", "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");
				sprintf(port_id, "%d", iptv_vid);

				/* set WAN STB VLAN */
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "eth0.600");
				eval("ifconfig", wan_dev, "allmulti", "up");
				if (!nvram_match("switch_wan0prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
				/* set IPTV port VLAN */
				eval("brctl", "delif", "br0", "eth1");
				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev1, "eth1.600");
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");

				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "addif", br_dev, wan_dev);
#endif
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(600, 0, 0x00100010); //LAN4
				__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(600, 0, 0x00800080); //LAN4
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(600, 0, 0x00010001); //LAN4
				__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(600, 0, 0x00020002); //LAN4
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
				__setup_vlan(600, 0, 0x00200062); //LAN4 untag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
				__setup_vlan(600, 0, 0x01000310); //LAN4 untag
#else
				__setup_vlan(600, 0, 0x00080008); //LAN4
				__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
				eval("brctl", "addif", "br0", "eth1");
#endif
			}
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GS7_PRO)
			else if (nvram_match("switch_wantag", "free")) {
				system("rtkswitch 40 1"); //leave tag case
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.100");
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX+1);

				/* wan0 is br102 */
				set_wan_phy("");
				add_wan_phy(br_dev);
				nvram_set("wan0_ifname", br_dev);
				nvram_set("wan0_gw_ifname", br_dev);

				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(iptv_vid, 0, 0x00000010); // LAN4 tag
				__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(iptv_vid, 0, 0x00000080); // LAN4 tag
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(iptv_vid, 0, 0x00000001); // LAN4 tag
				__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(iptv_vid, 0, 0x00000002); // LAN4 tag
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
				__setup_vlan(iptv_vid, 0, 0x00000020); // LAN4 tag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
				__setup_vlan(iptv_vid, 0, 0x00000100); // LAN4 tag
#else
				__setup_vlan(iptv_vid, 0, 0x00000008); // LAN4 tag
				__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
				eval("vconfig", "add", "eth1", port_id);
				eval("ifconfig", "eth1.100", "allmulti", "up");
				eval("brctl", "addif", br_dev, "eth1.100");
				eval("brctl", "addif", "br0", "eth1");
			}
#endif
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			//else if (nvram_match("mxl_test", "1")) {
			else {	// mxl norm/manual case 
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");

                                /* wan0 is br101 */
                                set_wan_phy("");
                                add_wan_phy(br_dev);
                                nvram_set("wan0_ifname", br_dev);
                                nvram_set("wan0_gw_ifname", br_dev);

				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");

#if defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
				if (nvram_match("switch_wantag", "free")) {
					__setup_vlan(iptv_vid, 0, 0x00000062); // LAN4 tag
				} else {
					__setup_vlan(iptv_vid, 0, 0x00200062); // LAN4 untag
				}
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
				if (nvram_match("switch_wantag", "free")) {
					__setup_vlan(iptv_vid, 0, 0x00000310); // LAN4 tag
				} else {
					__setup_vlan(iptv_vid, 0, 0x01000310); // LAN4 untag
				}
#endif
			}
#else
			else {  /* Nomo case, untag it. */
				/* config ethPort1 = IPTV */
				eval("brctl", "delif", "br0", "eth1");
#if defined(RTBE82M)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x00EE00EE); //no-tag fwd mask except LAN4
#endif
				vlan_forwarding(iptv_vid, iptv_prio, switch_stb, 1);
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(0, 0, 0x00E000E0); //no-tag fwd mask except LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(0, 0, 0x00700070); //no-tag fwd mask except LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(0, 0, 0x000E000E); //no-tag fwd mask except LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(0, 0, 0x001C001C); //no-tag fwd mask except LAN4
#elif defined(RTBE82M)
				// ;
#else
				__setup_vlan(0, 0, 0x00070007); //no-tag fwd mask except LAN4
#endif
				eval("brctl", "addif", "br0", "eth1");

				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("stb_x(%s) : vlan entry: %s\n", nvram_safe_get("switch_stb_x"), vlan_entry);
				/* tx push tag */
				/* Set Wan PRIO */
				if (nvram_invmatch("switch_wan1prio", "0")) {
					eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan1prio"), "0", "--rule-append");
				}
				else {
					eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				}
			}
#endif
		} else if (nvram_match("switch_stb_x", "5") && nvram_match("switch_wantag", "none")) {
			/* add vlan 1 to separate LAN and WAN bridge */
			eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
			__setup_vlan(1, 0, 0x00600060); //LAN1 and LAN2
			__setup_vlan(0, 0, 0x00900090); //no-tag fwd mask except LAN1 and LAN2
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
			__setup_vlan(1, 0, 0x00300030); //LAN1 and LAN2
			__setup_vlan(0, 0, 0x00C000C0); //no-tag fwd mask except LAN1 and LAN2
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
			__setup_vlan(1, 0, 0x000C000C); //LAN1 and LAN2
			__setup_vlan(0, 0, 0x00030003); //no-tag fwd mask except LAN1 and LAN2
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
			__setup_vlan(1, 0, 0x00180018); //LAN1 and LAN2
			__setup_vlan(0, 0, 0x00060006); //no-tag fwd mask except LAN1 and LAN2
#elif defined(RTBE82M)
			__setup_vlan(0, 0, 0x00300030); //no-tag fwd mask except LAN1 and LAN2
			__setup_vlan(1, 0, 0x000C000C); //LAN1 and LAN2
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			__setup_vlan(0, 0, 0x018E018E); //no-tag fwd mask except LAN1 and LAN2
			__setup_vlan(1, 0, 0x00600060); //LAN1 and LAN2
#else
			__setup_vlan(1, 0, 0x00030003); //LAN1 and LAN2
			__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN1 and LAN2
#endif
			eval("vconfig", "add", "eth1", "1");
			eval("ifconfig", "vlan1", "allmulti", "up");
			eval("brctl", "addif", br_dev, "vlan1");
			eval("brctl", "addif", "br0", "eth1");
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config ethPort2 = VoIP */
			if (nvram_match("switch_wantag", "ooxx")) {
				_dprintf("%s, ooxx\n", __func__);
			} 
#if !defined(RTBE82M) && !defined(GSBE18000) && !defined(GSBE12000) && !defined(GS7_PRO)
			else if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between WAN & vlanDev2, without untag */
				eval("brctl", "delif", "br0", "eth1");
				system("rtkswitch 40 1"); //leave tag case
				vlan_forwarding(voip_vid, voip_prio, 3, 0);

				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("stb_x - voip(%s) : vlan entry: %s\n", nvram_safe_get("switch_stb_x"), vlan_entry);
				/* tx push tag */
				/* Set Wan PRIO */
				if (nvram_invmatch("switch_wan2prio", "0")) {
					eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--rule-append");
				}
				else {
					eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				}
			}
#endif
			else if (nvram_match("switch_wantag", "none")) {
				/* add vlan 1 to separate LAN and WAN bridge */
				eval("brctl", "delif", "br0", "eth1");
#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(1, 0, 0x00900090); //LAN3 and LAN4
				__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(1, 0, 0x00C000C0); //LAN3 and LAN4
				__setup_vlan(0, 0, 0x00300030); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(1, 0, 0x00030003); //LAN3 and LAN4
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(1, 0, 0x00060006); //LAN3 and LAN4
				__setup_vlan(0, 0, 0x00180018); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE82M)
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(1, 0, 0x00300030); //LAN3 and LAN4
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0, 0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(1, 0, 0x01800180); //LAN3 and LAN4
#else
				__setup_vlan(1, 0, 0x000C000C); //LAN3 and LAN4
				__setup_vlan(0, 0, 0x00030003); //no-tag fwd mask except LAN3 and LAN4
#endif
				eval("vconfig", "add", "eth1", "1");
				eval("ifconfig", "vlan1", "allmulti", "up");
				eval("brctl", "addif", br_dev, "vlan1");
				eval("brctl", "addif", "br0", "eth1");
			}
			else if (nvram_match("switch_wantag", "meo_br")) 
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
			{
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(vlanDev1, "eth0.%d", iptv_vid);
				if (!nvram_match("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", vlanDev1, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", vlanDev1, "allmulti", "up");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);

                             	/* wan0 is br101 */
                                //set_wan_phy("");
                              	//add_wan_phy(br_dev);
                              	//nvram_set("wan0_ifname", br_dev);
                             	//nvram_set("wan0_gw_ifname", br_dev);

				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "delif", "br0", "eth1");

#if defined(RTBE82M)
				__setup_vlan(0,  0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(12, 0, 0x00100030); // LAN3 untag, LAN4 tag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0,  0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(12, 0, 0x00800180); // LAN3 untag, LAN4 tag
#endif

				eval("vconfig", "add", "eth1", port_id);
				sprintf(vlanDev2, "eth1.%d", iptv_vid);
				eval("ifconfig", vlanDev2, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev2);
				eval("brctl", "addif", "br0", "eth1");
			}
#else
			{
				/*
				 * Because vlan_forwarding vlanctl can't handle VID with tag / untag at the same time
				 * Create new rules to handle this case directly
				 * */

				/* leave tag */
				system("rtkswitch 40 1"); //leave tag case

				/* WAN */
				sprintf(vlan_entry, "0x%x", wan_vid);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev, "--mcast", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--set-rxif", wanVlanDev, "--filter-vid", vlan_entry, "0", "--filter-vlan-dev-mac-addr", "1", "--pop-tag", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				ifconfig(wanVlanDev, IFUP, "0.0.0.0", NULL);

				/* IPTV and VoIP */
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, vlanDev1, "--routed", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "1", "--set-rxif", vlanDev1, "--filter-vid", vlan_entry, "0", "--pop-tag", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
				ifconfig(vlanDev1, IFUP, "0.0.0.0", NULL);
				nvram_set("iptv_ifname", vlanDev1);

				/* wan bridge */
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
				eval("brctl", "addbr", br_dev);
				eval("brctl", "addif", br_dev, wanVlanDev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("brctl", "stp", br_dev, "on");
				eval("ifconfig", br_dev, "allmulti", "up");

				/* set up brX as wan interface */
				set_wan_phy("");
				add_wan_phy(br_dev);
				nvram_set("wan0_ifname", br_dev);
				nvram_set("wan0_gw_ifname", br_dev);

				/* IPTV and VoIP bridge */
				sprintf(br_dev, "br%d", IPTV_BR_INDEX+1);
				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");
				eval("brctl", "addif", br_dev, vlanDev1);
				eval("brctl", "delif", "br0", "eth1");
				eval("vconfig", "add", "eth1", "12");
				eval("ifconfig", "vlan12", "allmulti", "up");
				eval("brctl", "addif", br_dev, "vlan12");
				eval("brctl", "addif", "br0", "eth1");

#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(12, 0, 0x00100090); // LAN3 tag, LAN4 untag
				__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(12, 0, 0x008000C0); // LAN3 tag, LAN4 untag
				__setup_vlan(0, 0, 0x00300030); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(12, 0, 0x00010003); // LAN3 tag, LAN4 untag
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(12, 0, 0x00020006); // LAN3 tag, LAN4 untag
				__setup_vlan(0, 0, 0x00180018); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE82M)
				__setup_vlan(0,  0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(12, 0, 0x00200030); // LAN3 tag, LAN4 untag
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				__setup_vlan(0,  0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
				__setup_vlan(12, 0, 0x01000180); // LAN3 tag, LAN4 untag
#else
				__setup_vlan(12, 0, 0x0008000C); // LAN3 tag, LAN4 untag
				__setup_vlan(0, 0, 0x00030003); //no-tag fwd mask except LAN3 and LAN4
#endif
			}
#endif
#if defined(RTBE82M) || defined(GSBE18000) ||defined(GSBE12000) || defined(GS7_PRO)
			else {	// norm case
				eval("vconfig", "set_name_type", "DEV_PLUS_VID_NO_PAD");
				sprintf(br_dev, "br%d", IPTV_BR_INDEX);
                                /* wan0 is br101 */
                                set_wan_phy("");
                                add_wan_phy(br_dev);
                                nvram_set("wan0_ifname", br_dev);
                                nvram_set("wan0_gw_ifname", br_dev);

				eval("brctl", "addbr", br_dev);
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  br_dev,  "-p", "2",  "-m", "0");
				eval("ifconfig", br_dev, "allmulti", "up");

#if defined(RTBE82M)
				if (nvram_match("switch_wantag", "singtel_mio")) {
					__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x00200062); // LAN4 untag
					__setup_vlan(voip_vid, 0, 0x00000052); // LAN3 tag

				} else if (nvram_match("switch_wantag", "singtel_mstb")) {
					__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x00300072); // LAN3 & LAN4 untag

				} else if (nvram_match("switch_wantag", "manual")) {
					__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4

					// iptv
					if (iptv_vid) {
						if (voip_vid && iptv_vid == voip_vid)
							__setup_vlan(iptv_vid, 0, 0x00300072); // LAN3, LAN4 untag
						else
							__setup_vlan(iptv_vid, 0, 0x00200062); // LAN4 untag
					}

					// voip
					if (voip_vid && voip_vid != iptv_vid)
						__setup_vlan(voip_vid, 0, 0x00100052); // LAN3 untag

				} else {
					__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x00300072); // LAN3, LAN4 are same vid(iptv)
				}
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				if (nvram_match("switch_wantag", "singtel_mio")) {
					__setup_vlan(0, 0, 0x006E006E); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x01000310); // LAN4 untag
					__setup_vlan(voip_vid, 0, 0x00000290); // LAN3 tag

				} else if (nvram_match("switch_wantag", "singtel_mstb")) {
					__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x01800390); // LAN3 & LAN4 untag

				} else if (nvram_match("switch_wantag", "manual")) {
					__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4

					// iptv
					if (iptv_vid) {
						if (voip_vid && iptv_vid == voip_vid)
							__setup_vlan(iptv_vid, 0, 0x01800390); // LAN3, LAN4 untag
						else
							__setup_vlan(iptv_vid, 0, 0x01000310); // LAN4 untag
					}

					// voip
					if (voip_vid && voip_vid != iptv_vid)
						__setup_vlan(voip_vid, 0, 0x00800290); // LAN3 untag

				} else {
					__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
					__setup_vlan(iptv_vid, 0, 0x01800390); // LAN3, LAN4 are same vid(iptv)
				}

#endif
			}
#else
			else {
				if (voip_vid && nvram_invmatch("switch_wantag", "meo_br")
				) {
					/* Forward packets from wan:eth0 to vlanDev2 (untag) */
					eval("brctl", "delif", "br0", "eth1");
					if(iptv_vid != voip_vid)
						vlan_forwarding(voip_vid, voip_prio, 3, 1);
					else {
						if (nvram_match("switch_wantag", "meo_br")) {
							vlan_forwarding(voip_vid, voip_prio, 6, 2); //same vid case LAN3 untag LAN4 leave tag
						}
						else {
							vlan_forwarding(voip_vid, voip_prio, 6, 1); //same vid case
						}
					}

					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("stb_x - voip(%s) : vlan entry: %s\n", nvram_safe_get("switch_stb_x"), vlan_entry);
					/* tx push tag */
					/* Set Wan PRIO */
					if (nvram_invmatch("switch_wan2prio", "0")) {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan2prio"), "0", "--rule-append");
					}
					else {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev1, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
					}
				}
			}
			/* config ethPort1 = IPTV */
			//if (!nvram_match("mxl_test", "1") && iptv_vid && nvram_invmatch("switch_wantag", "meo_br")
			if (iptv_vid && nvram_invmatch("switch_wantag", "meo_br")
			) {
				/* Forward packets from wan:eth0 to vlanDev1 (untag) */
				/* Skip same vid case */
				_dprintf("non-mxl case.\n");
				if(iptv_vid != voip_vid) {
					vlan_forwarding(iptv_vid, iptv_prio, 4, 1); //Forward packets from wan:eth0 to vlanDev1 (untag)

					sprintf(vlan_entry, "0x%x", iptv_vid);
					_dprintf("stb_x - iptv(%s) : vlan entry: %s\n", nvram_safe_get("switch_stb_x"), vlan_entry);
					/* tx push tag */
					/* Set Wan PRIO */
					if (nvram_invmatch("switch_wan1prio", "0")) {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev2, "--push-tag", "--set-vid", vlan_entry, "0", "--set-pbits", nvram_get("switch_wan1prio"), "0", "--rule-append");
					}
					else {
						eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", vlanDev2, "--push-tag", "--set-vid", vlan_entry, "0", "--rule-append");
					}
				}

#if defined(GTBE98) || defined(GTBE98_PRO) || defined(GTBE96) || defined(GTBE19000) || defined(GTBE19000AI) || defined(GTBE96_AI)
				__setup_vlan(0, 0, 0x00600060); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE58U_PRO)
				__setup_vlan(0, 0, 0x00300030); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTAX55) || defined(RTAX58U_V2) || defined(RTAX3000N) || defined(BR63) && !defined(NEW_SWITCH_ORDER)
				__setup_vlan(0, 0, 0x000C000C); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55)
				__setup_vlan(0, 0, 0x00180018); //no-tag fwd mask except LAN3 and LAN4
#elif defined(RTBE82M)
				// ;
#else
				__setup_vlan(0, 0, 0x00030003); //no-tag fwd mask except LAN3 and LAN4
#endif
				eval("brctl", "addif", "br0", "eth1");
			}
#endif
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if (iptv_vid) { /* config IPTV on wan port */
				_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				if (nvram_invmatch("switch_wan1prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan1prio"));
				eval("ifconfig", wan_dev, "allmulti", "up");
				eval("bcmmcastctl", "mode", "-i",  "br0",  "-p", "1",  "-m", "0");
				eval("bcmmcastctl", "mode", "-i",  "br0",  "-p", "2",  "-m", "0");
			}
			else if (nvram_match("switch_wantag", "hinet_mesh")) {
				unsigned char eabuf[ETHER_ADDR_LEN];
				char macaddr[32];
				int iptv_unit = WAN_UNIT_IPTV;
				char prefix[] = "wanXXX_";
#if defined(RTBE82M) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO)
				if (!nvram_match("v0_mcast", "1")) {
					_dprintf("%s, create vif[%s/%s] w/o mcast\n", __func__, wan_if, wanVlanDev);
					eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev, "--set-if-mode-rg");
				} else 
#endif
				{
					_dprintf("%s, create vif[%s/%s] w mcast\n", __func__, wan_if, wanVlanDev);
					eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev, "--mcast", "--set-if-mode-rg");
				}

				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev, "--filter-vlan-dev-mac-addr", "1", "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev, "--rule-append");
				set_wan_phy("");
				add_wan_phy(wanVlanDev);
				nvram_set("wan0_ifname", wanVlanDev);
				nvram_set("wan0_gw_ifname", wanVlanDev);

				snprintf(wanVlanDev1, sizeof(wanVlanDev1), "%s.v1", wan_if);
				eval("vlanctl", "--if", wan_if, "--if-create-name", wan_if, wanVlanDev1, "--mcast", "--set-if-mode-rg");
				eval("vlanctl", "--if", wan_if, "--rx", "--tags", "0", "--set-rxif", wanVlanDev1, "--rule-append");
				eval("vlanctl", "--if", wan_if, "--tx", "--tags", "0", "--filter-txif", wanVlanDev1, "--rule-append");
				snprintf(prefix, sizeof(prefix), "wan%d_", iptv_unit);
				nvram_pf_set(prefix, "ifname", wanVlanDev1);
				nvram_set("iptv_ifname", wanVlanDev1);

				ether_atoe(get_wan_hwaddr(), eabuf);
				eabuf[ETHER_ADDR_LEN - 1] += 1;
				ether_etoa(eabuf, macaddr);
				nvram_pf_set(prefix, "hwaddr", macaddr);
				nvram_pf_set(prefix, "proto", "bridge");
				nvram_pf_set(prefix, "enable", "1");
			}
		}
		if (switch_stb >= 8) {
			/* handle vlan + pppoe w/o bridge case. Using vconfig instead */
			if (voip_vid) { /* config voip on wan port */
				_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", wan_if, port_id);
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				if (nvram_invmatch("switch_wan2prio", "0"))
					eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan2prio"));
				eval("ifconfig", wan_dev, "allmulti", "up");
			}
		}
#endif
		break;
#endif
#endif

	case MODEL_RTAC5300:
				/* If enable gmac3, CPU port is 8 */
#ifdef RTCONFIG_RGMII_BRCM5301X
				/* P0  P1 P2 P3 P4 P5 		P7 */
				/* WAN L1 L2 L3 L4 L5 L6 L7 L8 	CPU*/
#else
				/* P0  P1 P2 P3 P4 P5 */
				/* WAN L1 L2 L3 L4 CPU*/
#endif
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);

			if (gmac3_enable) {
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0101");	/* 8, 0 */
			} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0081");	/* 7, 0 */
#else
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0021");	/* 5, 0 */
#endif
			}
			eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {  // L3:p3, w:p0, c:p7/p5
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);

				char vlan_cmd[64];
				if (gmac3_enable) {
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 5u 7 8t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 8t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 3 4t\"");
					system(vlan_cmd);
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 7t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 7t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 3 4t\"");
					system(vlan_cmd);
#else
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 5t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 5t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 3 4t\"");
					system(vlan_cmd);
#endif
				}
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between WAN & L3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between WAN & L3, without untag */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", "0x0335"); /* vlan id=821 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", "0x0336"); /* vlan id=822 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case. */
				voip_prio <<= 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x16", tag_register);
				_dprintf("lan(3) tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x1009");	/* un:p3, f:p0 3*/
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // lan(4) = P4
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between wan & L4, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);

				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x00111"); /* fwd: 0 4 8 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0091"); /* fwd: 0 4 7 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0031"); /* fwd: 0 4 5 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				/* config LAN 4 = IPTV */
				iptv_prio <<= 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x18", tag_register);
				_dprintf("lan(4) tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {	// lan(3)=P3, lan(4)=P4
			/* config lan(3)/P3 = VoIP */
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between WAN & lan(3), without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0009");		/* f:30 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {
				if (voip_vid) {
					voip_prio <<= 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "-i", "eth0", "robowr", "0x34", "0x16", tag_register);	/* p3 */
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* untag port map */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x3019"); /* un:43, f:430*/
					else
						eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x1009"); /* un:3 , f:30 */
					eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config lan(4)/P4 = IPTV */
			if (iptv_vid) {
				iptv_prio <<= 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x18", tag_register);	/* p4 */
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x3019"); /* un:43, f:430*/
				else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x2011"); /* un:4, f:40 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0101");	/* 8, 0 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0081");	/* 7, 0 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0021");	/* 5, 0 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0101");	/* 8, 0 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0081");	/* 7, 0 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0021");	/* 5, 0 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0101");	/* 8, 0 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0081");	/* 7, 0 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0021");	/* 5, 0 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;

	case MODEL_RTAC3100:
	case MODEL_RTAC88U:
				/* If enable gmac3, CPU port is 8 */
#ifdef RTCONFIG_RGMII_BRCM5301X
				/* P4  P3 P2 P1 P0 P5 		P7 */
				/* WAN L1 L2 L3 L4 L5 L6 L7 L8 	CPU*/
#else
				/* P4  P3 P2 P1 P0 P5 */
				/* WAN L1 L2 L3 L4 CPU*/
#endif
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);

			if (gmac3_enable) {
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0110");	/* f: 4, 8 */
			} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0090");	/* f: 4, 7 */
#else
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0030");	/* f: 4, 5 */
#endif
			}
			eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));
		if (nvram_match("switch_wantag", "stuff_fibre")) {
			eval("vconfig", "set_egress_map", wan_dev, "3", "5");
			break;
		}

		if (nvram_match("switch_stb_x", "3")) {  // L3:p1 w:p4 c:p7/p5
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);

				char vlan_cmd[64];
				if (gmac3_enable) {
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 5u 7 8t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 8t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 1 4t\"");
					system(vlan_cmd);
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 7t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 7t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 1 4t\"");
					system(vlan_cmd);
#else
					sprintf(vlan_cmd, "robocfg vlan 1 ports \"1 2 3 5t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 4t 5t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 4t\"");
					system(vlan_cmd);
					sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 1 4t\"");
					system(vlan_cmd);
#endif
				}
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between WAN & L3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0012");	/* f: 4 1 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between WAN & L3, without untag */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0012");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", "0x0335"); /* vlan id=821 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0012");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", "0x0336"); /* vlan id=822 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case. */
				voip_prio <<= 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan(3) tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0412");	/* un:1, f:41*/
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // L4:p0
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between wan & L4, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);

				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0111");	/* fwd: 0 4 8 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0091");	/* fwd: 0 4 7 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0031");	/* fwd: 0 4 5 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				/* config LAN 4 = IPTV */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x10", tag_register);
				_dprintf("lan(4) tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0211");	/* un:0 f:40 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {	// L3/p1, L4/p0
			/* config L3/p1 = VoIP */
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between WAN & L3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0012");		/* f:41 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
			else {
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "-i", "eth0", "robowr", "0x34", "0x12", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* untag port map */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0613");	/* un:10, f:410*/
					else
						eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0412");	/* un:1 , f:41 */
					eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config L4/p0 = IPTV */
			if (iptv_vid) {
				iptv_prio <<= 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "-i", "eth0", "robowr", "0x34", "0x10", tag_register);	/* p0 */
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* untag port map */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0613"); /* un:10 , f:410 */
				else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0211"); /* un:0, f:40 */
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0110");	/* f: 4, 8 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0090");	/* f: 4, 7 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0030");	/* f: 4, 5 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0110");	/* f: 4, 8 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0090");	/* f: 4, 7 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0030");	/* f: 4, 5 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
							eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				if (gmac3_enable) {
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0110");	/* f: 4, 8 */
				} else {
#ifdef RTCONFIG_RGMII_BRCM5301X
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0090");	/* f: 4, 7 */
#else
					eval("et", "-i", "eth0", "robowr", "0x05", "0x83", "0x0030");	/* f: 4, 5 */
#endif
				}
				eval("et", "-i", "eth0", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "-i", "eth0", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;
				/* P0  P1 P2 P3 P5 P7 */
	case MODEL_RTAC87U:	/* WAN L4 L3 L2 L1 CPU */
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0081");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {  // P3
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);

				char vlan_cmd[64];
				sprintf(vlan_cmd, "robocfg vlan 1 ports \"2 3 5u 7t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 2 ports \"0 7\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 100 ports \"0t 1t 7t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 101 ports \"0t 1t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 105 ports \"0t 1t 2\"");
				system(vlan_cmd);
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", "0x0335");	/* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", "0x0336");	/* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0805");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // P4
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between port 0 & 1, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("* vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0083");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0403");	/* un: 1, fwd; 0,1*/
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */ // P3
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0005");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x14", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x0C07");
					else
						eval("et", "robowr", "0x05", "0x83", "0x0805");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config LAN 4 = IPTV */ // P4
			if (iptv_vid) {
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x0C07");
				else
					eval("et", "robowr", "0x05", "0x83", "0x0403");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0081");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0081");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0081");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;
				/* P0 P1 P2 P3 P4 P5 */
	case MODEL_RTAC56S:	/* L1 L2 L3 L4 WAN CPU */
	case MODEL_RTAC56U:
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0030");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {	// P2
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);

				char vlan_cmd[64];
				sprintf(vlan_cmd, "robocfg vlan 1 ports \"0 1 2 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 2 ports \"4 5\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 100 ports \"3t 4t 5t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 101 ports \"3t 4t\"");
				system(vlan_cmd);
				sprintf(vlan_cmd, "robocfg vlan 105 ports \"2 3t 4t\"");
				system(vlan_cmd);
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between port 4 & 2, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0014");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between port 4 & 2, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0014");
				eval("et", "robowr", "0x05", "0x81", "0x0335");	/* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0014");
				eval("et", "robowr", "0x05", "0x81", "0x0336");	/* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x14", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0814");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) { // P3
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between port 0 & 4, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0038");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x1018");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */	// P2
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between port 2 & 4, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0014");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x14", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x081C");
					else
						eval("et", "robowr", "0x05", "0x83", "0x0814");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			/* config LAN 4 = IPTV */ //P3
			if (iptv_vid) {
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x081C");
				else
					eval("et", "robowr", "0x05", "0x83", "0x1018");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) {
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0030");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) {
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0030");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0030");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;

	case MODEL_RTN66U:
	case MODEL_RTAC66U:
	case MODEL_RTAC1200G:
	case MODEL_RTAC1200GP:
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0101");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {
			if (nvram_match("switch_wantag", "vodafone")) { //Config by robocfg
				iptv_vid = 105;
				iptv_prio = 1;
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);
				/* vlan 1 */
				eval("et", "robowr", "0x05", "0x83", "0x0D06");
				eval("et", "robowr", "0x05", "0x81", "0x0001");
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				/* vlan 100 */
				eval("et", "robowr", "0x05", "0x83", "0x0111");
				eval("et", "robowr", "0x05", "0x81", "0x0064");
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				/* vlan 101 */
				eval("et", "robowr", "0x05", "0x83", "0x0011");
				eval("et", "robowr", "0x05", "0x81", "0x0065");
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				/* vlan 105 */
				eval("et", "robowr", "0x05", "0x83", "0x1019");
				eval("et", "robowr", "0x05", "0x81", "0x0069");
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				break;
			}
			if (nvram_match("switch_wantag", "m1_fiber")
				|| nvram_match("switch_wantag", "maxis_fiber_sp")
				|| nvram_match("switch_wantag", "maxis_cts")
				|| nvram_match("switch_wantag", "maxis_sacofa")
				|| nvram_match("switch_wantag", "maxis_tnb")
			) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0335");	/* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0336");	/* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber_iptv")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0335");	/* vlan id=821 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0336");	/* vlan id=822 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				/* Just forward packets between port 0 & 4 & untag */
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", "0x0337");	/* vlan id=823 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", "0x0338");	/* vlan id=824 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else if (nvram_match("switch_wantag", "maxis_fiber_sp_iptv")) {
				/* Just forward packets between port 0 & 3, without untag */
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x0006");	/* vlan id= 6  */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", "0x000E");	/* vlan id= 14 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				/* Just forward packets between port 0 & 4 & untag */
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", "0x000F");	/* vlan id= 15 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", "0x0011");	/* vlan id= 17 */
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x16", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x1009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) {
			/* config LAN 4 = IPTV */
			if (nvram_match("switch_wantag", "meo")) {
				/* Just forward packets between port 0 & 1, without untag */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("* vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0111");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {  /* Nomo case, untag it. */
				/* config LAN 4 = IPTV */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x18", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between port 0 & 3, without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0009");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x16", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x3019");
					else
						eval("et", "robowr", "0x05", "0x83", "0x1009");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			if (iptv_vid) { /* config LAN 4 = IPTV */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x18", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x3019");
				else
					eval("et", "robowr", "0x05", "0x83", "0x2011");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
#ifdef RTCONFIG_MULTICAST_IPTV
		if (switch_stb >= 7) { //Maxis IPTV case
			if (iptv_vid) { /* config IPTV on wan port */
_dprintf("*** Multicast IPTV: config Maxis IPTV on wan port ***\n");
				sprintf(wan_dev, "vlan%d", iptv_vid);
				nvram_set("wan10_ifname", wan_dev);
				sprintf(port_id, "%d", iptv_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", iptv_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0101");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (iptv_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
		if (switch_stb >= 8) { //Singtel IPTV case
			if (voip_vid) { /* config voip on wan port */
_dprintf("*** Multicast IPTV: config Singtel VOIP on wan port ***\n");
				sprintf(wan_dev, "vlan%d", voip_vid);
				nvram_set("wan11_ifname", wan_dev);
				sprintf(port_id, "%d", voip_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", voip_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0101");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (voip_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)voip_prio);
				}
			}
		}
		if (switch_stb >=9) {
			if (mang_vid) { /* config tr069 on wan port */
_dprintf("*** Multicast IPTV: config Singtel TR069 on wan port ***\n");
				sprintf(wan_dev, "vlan%d", mang_vid);
				nvram_set("wan12_ifname", wan_dev);
				sprintf(port_id, "%d", mang_vid);
				eval("vconfig", "add", interface, port_id);
				sprintf(vlan_entry, "0x%x", mang_vid);
				eval("et", "robowr", "0x05", "0x83", "0x0101");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");

				if (mang_prio) { /* config priority */
					eval("vconfig", "set_egress_map", wan_dev, "0", (char *)iptv_prio);
				}
			}
		}
#endif
		break;

	case MODEL_RTN15U:
		if (wan_vid) { /* config wan port */
			eval("vconfig", "rem", "vlan2");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
			sprintf(vlan_entry, "0x%x", wan_vid);
			eval("et", "robowr", "0x05", "0x83", "0x0110");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		}
		/* Set Wan port PRIO */
		if (nvram_invmatch("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_stb_x", "3")) {
			if (nvram_match("switch_wantag", "m1_fiber")) {
				/* Just forward packets between LAN3 & WAN(port1 & 4), without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0012");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				voip_prio = voip_prio << 13;
				sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
				eval("et", "robowr", "0x34", "0x12", tag_register);
				_dprintf("lan 3 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0412");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		} else if (nvram_match("switch_stb_x", "4")) {
			/* config LAN 4 = IPTV */
			iptv_prio = iptv_prio << 13;
			sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
			eval("et", "robowr", "0x34", "0x10", tag_register);
			_dprintf("lan 4 tag register: %s\n", tag_register);
			/* Set vlan table entry register */
			sprintf(vlan_entry, "0x%x", iptv_vid);
			_dprintf("vlan entry: %s\n", vlan_entry);
			eval("et", "robowr", "0x05", "0x83", "0x0211");
			eval("et", "robowr", "0x05", "0x81", vlan_entry);
			eval("et", "robowr", "0x05", "0x80", "0x0000");
			eval("et", "robowr", "0x05", "0x80", "0x0080");
		} else if (nvram_match("switch_stb_x", "6")) {
			/* config LAN 3 = VoIP */
			if (nvram_match("switch_wantag", "singtel_mio")) {
				/* Just forward packets between LAN3 & WAN(port1 & 4), without untag */
				sprintf(vlan_entry, "0x%x", voip_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				eval("et", "robowr", "0x05", "0x83", "0x0012");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
			else {	/* Nomo case, untag it. */
				if (voip_vid) {
					voip_prio = voip_prio << 13;
					sprintf(tag_register, "0x%x", (voip_prio | voip_vid));
					eval("et", "robowr", "0x34", "0x12", tag_register);
					_dprintf("lan 3 tag register: %s\n", tag_register);
					/* Set vlan table entry register */
					sprintf(vlan_entry, "0x%x", voip_vid);
					_dprintf("vlan entry: %s\n", vlan_entry);
					if (voip_vid == iptv_vid)
						eval("et", "robowr", "0x05", "0x83", "0x0613");
					else
						eval("et", "robowr", "0x05", "0x83", "0x0412");
					eval("et", "robowr", "0x05", "0x81", vlan_entry);
					eval("et", "robowr", "0x05", "0x80", "0x0000");
					eval("et", "robowr", "0x05", "0x80", "0x0080");
				}
			}
			if (iptv_vid) { /* config LAN 4 = IPTV */
				iptv_prio = iptv_prio << 13;
				sprintf(tag_register, "0x%x", (iptv_prio | iptv_vid));
				eval("et", "robowr", "0x34", "0x10", tag_register);
				_dprintf("lan 4 tag register: %s\n", tag_register);
				/* Set vlan table entry register */
				sprintf(vlan_entry, "0x%x", iptv_vid);
				_dprintf("vlan entry: %s\n", vlan_entry);
				if (voip_vid == iptv_vid)
					eval("et", "robowr", "0x05", "0x83", "0x0613");
				else
					eval("et", "robowr", "0x05", "0x83", "0x0211");
				eval("et", "robowr", "0x05", "0x81", vlan_entry);
				eval("et", "robowr", "0x05", "0x80", "0x0000");
				eval("et", "robowr", "0x05", "0x80", "0x0080");
			}
		}
		break;

				/* WAN L1 L2 L3 L4 CPU */
	case MODEL_RTAC53U:	/* P0  P1 P2 P3 P4 P5 */
		/* Enable high bits check */
		eval("et", "robowr", "0x34", "0x3", "0x80", "0x1");
		/* Config WAN port */
		if (wan_vid) {
			eval("vconfig", "rem", "vlan2");
			eval("et", "robowr", "0x34", "0x8", "0x01002000", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3002");
			sprintf(port_id, "%d", wan_vid);
			eval("vconfig", "add", interface, port_id);
		}
		/* Set Wan prio*/
		if (!nvram_match("switch_wan0prio", "0"))
			eval("vconfig", "set_egress_map", wan_dev, "0", nvram_get("switch_wan0prio"));

		if (nvram_match("switch_wantag", "unifi_home")) {
			/* vlan1ports= 1 2 3 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010013ae", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan500ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4021", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
			/* vlan600ports= 0 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01258411", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3258");
			/* LAN4 vlan tag */
			eval("et", "robowr", "0x34", "0x18", "0x0258");
		} else if (nvram_match("switch_wantag", "unifi_biz")) {
			/* Modify vlan500ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x011f4021", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x31f4");
		} else if (nvram_match("switch_wantag", "singtel_mio")) {
			/* vlan1ports= 1 2 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010011a6", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan10ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a021", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 4 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01014411", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* vlan30ports= 3 0 */
			eval("et", "robowr", "0x34", "0x8", "0x0101e009", "0x4");/*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x301e");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x18", "0x8014");
		} else if (nvram_match("switch_wantag", "singtel_others")) {
			/* vlan1ports= 1 2 3 5 */
			eval("et", "robowr", "0x34", "0x8", "0x010013ae", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan10ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100a021", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x300a");
			/* vlan20ports= 0 4 */
			eval("et", "robowr", "0x34", "0x8", "0x01014411", "0x4");
			eval("et", "robowr", "0x34", "0x6", "0x3014");
			/* LAN4 vlan tag & prio */
			eval("et", "robowr", "0x34", "0x18", "0x8014");
		} else if (nvram_match("switch_wantag", "m1_fiber")) {
			/* vlan1ports= 1 2 4 5 */				 /*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010015b6", "0x4");/*0111|1011|0110*/
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan1103ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0144f021", "0x4");/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x344f");
			/* vlan1107ports= 3 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01453009", "0x4");/*0000|0000|1001*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3453");
		} else if (nvram_match("switch_wantag", "maxis_fiber")) {
			/* vlan1ports= 1 2 4 5 */				 /*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010015b6", "0x4");/*0111|1011|0110*/
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan621 ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0126d021", "0x4");/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x326d");
			/* vlan821/822 ports= 3 0 */
			eval("et", "robowr", "0x34", "0x8", "0x01335009", "0x4");/*0000|0000|1001*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3335");
			eval("et", "robowr", "0x34", "0x8", "0x01336009", "0x4");/*0000|0000|1001*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x3336");
		} else if (nvram_match("switch_wantag", "maxis_fiber_sp")) {
			/* vlan1ports= 1 2 4 5 */				 /*5432 1054 3210*/
			eval("et", "robowr", "0x34", "0x8", "0x010015b6", "0x4");/*0111|1011|0110*/
			eval("et", "robowr", "0x34", "0x6", "0x3001");
			/* vlan11ports= 0 5 */
			eval("et", "robowr", "0x34", "0x8", "0x0100b021", "0x4");/*0000|0010|0001*/ /*Dont untag WAN port*/
			eval("et", "robowr", "0x34", "0x6", "0x300b");
			/* vlan14ports= 3 0 */
			eval("et", "robowr", "0x34", "0x8", "0x0100e009", "0x4");/*0000|0000|1001*/ /*Just forward without untag*/
			eval("et", "robowr", "0x34", "0x6", "0x300e");
		} else {	/* manual */
							/* WAN L1 L2 L3 L4 CPU */
			const int ports[SWPORT_COUNT] = { 0, 1, 2, 3, 4, 5 };

			if (switch_stb != SWCFG_STB4 && switch_stb != SWCFG_STB34)
				iptv_vid = 0;
			if (switch_stb != SWCFG_STB3 && switch_stb != SWCFG_STB34)
				voip_vid = 0;
			if (wan_vid) {
				sprintf(vlan_entry, "0x%x", BCM5325_ventry(wan_vid, wan_vid, iptv_vid, voip_vid));
				eval("et", "robowr", "0x34", "0x8", vlan_entry, "0x4");
				eval("et", "robowr", "0x34", "0x6", "0x3002");
			}
			if (iptv_vid) {
				if (iptv_vid != wan_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(iptv_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry, "0x4");
					eval("et", "robowr", "0x34", "0x6", "0x3003");
				}
				sprintf(tag_register, "0x%x", (iptv_prio << 13) | iptv_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN4]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
			if (voip_vid) {
				if (voip_vid != wan_vid && voip_vid != iptv_vid) {
					sprintf(vlan_entry, "0x%x", BCM5325_ventry(voip_vid, wan_vid, iptv_vid, voip_vid));
					eval("et", "robowr", "0x34", "0x8", vlan_entry, "0x4");
					eval("et", "robowr", "0x34", "0x6", "0x3004");
				}
				sprintf(tag_register, "0x%x", (voip_prio << 13) | voip_vid);
				sprintf(port_id, "0x%x", 0x10 + 2*ports[SWPORT_LAN3]);
				eval("et", "robowr", "0x34", port_id, tag_register);
			}
		}
		break;
	}
	return;
}

int
wl_exist(char *ifname, int band)
{
	int unit = -1;
	int bandtype;

#ifdef RTCONFIG_QTN
	if (band == 2) {
		if (nvram_get_int("qtn_ready") == 1)
			return 1;
		else
			return -1;
	}
#endif

	if (wl_probe(ifname) || wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit)))
		goto ERROR;

	wl_ioctl(ifname, WLC_GET_BAND, &bandtype, sizeof(bandtype));

	if ((bandtype != WLC_BAND_AUTO) &&
	    ((band == 1 && bandtype != WLC_BAND_2G) ||
	     (band >= 2 && ((bandtype != WLC_BAND_5G)
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
	      && (bandtype != WLC_BAND_6G)
#endif
	      ))))
		goto ERROR;
	else
		return 1;

ERROR:
	_dprintf("No wireless %s interface!!!\n", ifname);
	return 0;
}

#ifdef RTCONFIG_AVBLCHAN
#define MAX_5G_CHANNEL_LIST_NUM		32
#define MAX_CHANS			MAX_5G_CHANNEL_LIST_NUM*4 + 30 

void add_cfgexcl_2_acsexcl(unsigned int *echx)
{
	char ex_tmp[24], word[256], *next, *asus_excl;
	int i = 0, unit = 0, exist = 0;
	unsigned int ech;
	char acsexcl_wlx[1500], chtmp[7], *sp;

	for(unit = 0; unit < 3; ++unit) {
		memset(ex_tmp, 0, sizeof(ex_tmp));
		sprintf(ex_tmp, "wl%d_acs_excl_chans", unit);
		asus_excl = nvram_safe_get(ex_tmp);

		foreach_44 (word, asus_excl, next) {
			ech = strtol(word, NULL, 16);
			exist = 0;
			if(!ech) continue;
			for(i=0; i<MAX_CHANS; ++i) {
				if(ech == *(echx + unit*MAX_CHANS + i)) {
					exist = 1;
					break;
				} else if(!*(echx + unit*MAX_CHANS + i))
					break;
			}
			if(!exist) {
				*(echx + unit*MAX_CHANS + i) = ech;
			}
		}
		memset(acsexcl_wlx, 0, sizeof(acsexcl_wlx));
		for(i=0; i<MAX_CHANS; ++i) {
			sp = acsexcl_wlx[0]?",":"";
			if(*(echx + unit*MAX_CHANS + i) && snprintf(chtmp, sizeof(chtmp), "0x%x", *(echx + unit*MAX_CHANS + i)) > 0) {
				if(strlen(acsexcl_wlx) + 6 < sizeof(acsexcl_wlx) - 1) {
					strncat(acsexcl_wlx, sp, sizeof(acsexcl_wlx)-strlen(acsexcl_wlx)-1);
					strncat(acsexcl_wlx, chtmp, sizeof(acsexcl_wlx)-strlen(acsexcl_wlx)-1);
				} else
					_dprintf("acsexcl_wlx full!(rc)\n");
			}
		}
		nvram_set(ex_tmp, acsexcl_wlx);
	}
}

void init_cfg_excl(char *cfg_excl, unsigned int *ech, int unit)
{
	char word[256], *next;
	int i = 0;
	unsigned int chan;

	foreach_44 (word, cfg_excl, next) {
		chan = strtol(word, NULL, 16);
		if(chan > 0) {
			if(i < MAX_CHANS) {
				*(ech + unit*MAX_CHANS + i) = chan;
			} else
				break;
			i++;
		}
	}
}

void dump_exclchans(unsigned int *excs, char *des) {
	int i=0 , j= 0, k=0;

	_dprintf("\n%s. dump cfg_excl_chans:\n", des);

	for(i=0; i<3; ++i) {
		_dprintf("\n<wl%d>\n", i);
		k=0;
		for(j=0; j<MAX_CHANS; ++j) {
			if(*(excs + i*MAX_CHANS + j)) {
				_dprintf("[%2x] ", *(excs + i*MAX_CHANS + j));
				++k;
				if(k%10==0)
					_dprintf("\n");
			}
		}
	}
	_dprintf("\n");
}

int reset_exclvalid()
{
	int i, ifnum=0, acs_valid=0;
	char exchans[64], exchans_valid[64], wl_tmp[64];
	char word[256], *next;

	if(nvram_get_int("re_mode") == 1)
		return 1;

	foreach (word, nvram_safe_get("wl_ifnames"), next)
		ifnum++;

	for(i=0; i<ifnum; ++i) {
		sprintf(wl_tmp, "wl%d_chanspec", i);
		acs_valid = nvram_match(wl_tmp, "0")?1:0;
		if(!acs_valid)
			continue;

		sprintf(exchans, "wl%d_acs_excl_chans", i);
		sprintf(exchans_valid, "wl%d_acs_excl_chans_valid", i);
		nvram_set(exchans_valid, nvram_safe_get(exchans));
		_dprintf("\nset exclchans valid[%d]:[%s]=[%s]\n", i, exchans_valid, nvram_safe_get(exchans_valid));
	}
}

int reset_exclbase(int ifnum)
{
	int i;
	char chans[64], chans_base[64];

	if(nvram_get_int("re_mode") == 1)
		return 1;

	for(i=0; i<ifnum; ++i) {
		sprintf(chans, "wl%d_acs_excl_chans", i);
		sprintf(chans_base, "wl%d_acs_excl_chans_base", i);
		nvram_set(chans_base, nvram_safe_get(chans));
		_dprintf("\nset exclchans base[%d]:[%s]=[%s]\n", i, chans_base, nvram_safe_get(chans_base));
	}


	if(!nvram_get_int("excbase")) {
		nvram_set("excbase", "1");
		for(i=0; i<ifnum; ++i) {
			sprintf(chans, "wl%d_acs_excl_chans_cfg", i);
			nvram_set(chans, "");
		}
		return 1;
	}
	return 0;
}

void dump_acs_excl_chans(char *desc)
{
	int i, ifnum=0;
	char word[256], *next;
	char nvname[32];

	if(!nvram_match("cfg_abl", "1"))
		return;

	foreach (word, nvram_safe_get("wl_ifnames"), next)
		ifnum++;

	for(i=0; i<ifnum; ++i) {
		sprintf(nvname, "wl%d_acs_excl_chans", i);
		RC_DBG_ABL("[%s] wl%d_acs_excl_chans:\n%s\n", desc, i, nvram_safe_get(nvname));
		sprintf(nvname, "wl%d_acs_excl_chans_cfg", i);
		RC_DBG_ABL("[%s] wl%d_acs_excl_chans_cfg:\n%s\n", desc, i, nvram_safe_get(nvname));
	}
}
#endif

#ifdef RTCONFIG_BCMWL6

void set_acs_ifnames()
{
	char acs_ifnames[64];
	char acs_ifnames2[64];
	char word[256], *next;
	char tmp[128], tmp2[128], prefix[] = "wlXXXXXXXXXX_";
	int unit = 0;
	char wlvif[] = "wlxxxx";
	char list[2048], list2[2048], list_5g_band1_chans[1024], list_5g_band2_chans[1024], list_5g_band3_chans[1024];
	char list_5g_unii4_chans[1024];
	char list_5g_band4_bw160_chans[256];
#ifdef RTCONFIG_WIFI7
	char list_5g_band4_bw80_chans[256];
#endif
#ifdef RTCONFIG_HND_ROUTER_AX
	char list3[2048];
#endif
#ifdef RTCONFIG_AVBLCHAN
	char *cfg_excl = NULL;
	unsigned int cfg_excl_chans[3][MAX_CHANS];	// 2g/5g chans num
	int ifnum = 0;
	char nvname[32];
	memset(cfg_excl_chans, 0, sizeof(cfg_excl_chans));
#endif
	int war;

	char prefix_2g[] = "wlXXXXXXXXXX_";
	char prefix_5g[] = "wlXXXXXXXXXX_";
#ifdef RTCONFIG_HAS_5G_2
	char prefix_5g2[] = "wlXXXXXXXXXX_";
#endif
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	char prefix_6g[] = "wlXXXXXXXXXX_";
#endif
#ifdef RTCONFIG_HAS_6G_2
	char prefix_6g2[] = "wlXXXXXXXXXX_";
#endif
	const char list_ch165[] = "0xd0a5,0xd8a7,0xe0ab,0xeca3"; // chanspecs 165, 165l, 165/80, 165/160
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	char list6g[1200];
	/* exclude acsd from selecting chanspec 6g1, 6g5, 6g9, 6g13, 6g17, 6g21, 6g25, 6g29 bw20/40/80/160 and 6g33-6g61 bw320, also 6g225, 6g229 bw20/40, 6g233 bw20, prevent 5G adjacent interference */
	const char list_6g_unii5_skip_non_eu[] = "0x5001,0x5002,0x5005,0x5009,0x500d,0x5011,0x5015,0x5019,0x501d,0x5803,0x5903,0x580b,0x590b,0x5813,0x5913,0x581b,0x591b,0x6007,0x6107,0x6207,0x6307,0x6017,0x6117,0x6217,0x6317,0x680f,0x690f,0x6a0f,0x6b0f,0x6c0f,0x6d0f,0x6e0f,0x6f0f";
	/* exclude acsd from selecting chanspec 6g225, 6g229 bw20/40, 6g233 bw20 */
	const char list_6g_unii8_skip_non_eu[] = "0x50e9,0x50e1,0x50e5,0x58e3,0x59e3";
#endif
#if defined(RTCONFIG_WIFI7) && defined(RTCONFIG_BW320M) && defined(RTCONFIG_HAS_6G)
	/* workaround for interoperation and interference issue, exclude influenced chanspec from auto channel selection */
	const char list_6g_unii5_10g_phy_affect_skip[] = "0x5059,0x585b,0x595b,0x6057,0x6157,0x6257,0x6357,0x684f,0x694f,0x6a4f,0x6b4f,0x6c4f,0x6d4f,0x6e4f,0x6f4f,0x7002,0x7042,0x7082,0x70c2,0x7102,0x7142,0x7182,0x71c2,0x7001,0x7041,0x7081,0x70c1,0x7101,0x7141,0x7181,0x71c1,0x7201,0x7241,0x7281,0x72c1,0x7301,0x7341,0x7381,0x73c1";
	const char list_6g_unii6_10g_phy_affect_skip[] = "0x7202,0x7242,0x7282,0x72c2,0x7302,0x7342,0x7382,0x73c2";
#endif
#if defined(RTCONFIG_QUADBAND) && (defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7))
#ifdef RTCONFIG_HAS_6G_2
	const unsigned int wl_idx[4] = {WL_2G_BAND, WL_5G_BAND, WL_6G_BAND, WL_6G_2_BAND};      // 2.4G, 5G, 6G-1, 6G-2
#else
	const unsigned int wl_idx[4] = {WL_2G_BAND, WL_5G_BAND, WL_5G_2_BAND, WL_6G_BAND};	// 2.4G, 5G-1, 5G-2, 6G
#endif
#else
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	const unsigned int wl_idx[3] = {WL_2G_BAND, WL_5G_BAND, WL_6G_BAND};			// 2.4G, 5G, 6G
#else
	const unsigned int wl_idx[3] = {WL_2G_BAND, WL_5G_BAND, WL_5G_2_BAND};			// 2.4G, 5G-1, 5G-2(6G)
#endif
#endif
#if defined(XT8PRO) || defined(BT12) || defined(BM68)
        int acs_5g_unit = 2;
#elif defined(BQ16)
		int acs_5g_unit = 1;	/* 5G high */
#elif defined(BT10)
		int acs_5g_unit = WLIF_5G1;	/* 5G */
#elif defined(BQ16_PRO)
		int acs_5g_unit = 0;	/* 5G all band */
#elif defined(XD4PRO)
        int acs_5g_unit = 1;
#endif

	snprintf(prefix_2g, sizeof(prefix_5g), "wl%d_", WL_2G_BAND);
	snprintf(prefix_5g, sizeof(prefix_5g), "wl%d_", WL_5G_BAND);
#ifdef RTCONFIG_HAS_5G_2
	snprintf(prefix_5g2, sizeof(prefix_5g2), "wl%d_", WL_5G_2_BAND);
#endif
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
	snprintf(prefix_6g, sizeof(prefix_6g), "wl%d_", WL_6G_BAND);
#endif
#ifdef RTCONFIG_HAS_6G_2
	snprintf(prefix_6g2, sizeof(prefix_6g2), "wl%d_", WL_6G_2_BAND);
#endif

	war = nvram_match(strcat_r(prefix_5g, "bw_160", tmp), "1") && nvram_match("acs_dfs", "0");

	wl_check_5g_band_group();

	memset(list, 0, sizeof(list));
	memset(list2, 0, sizeof(list2));
#ifdef RTCONFIG_HND_ROUTER_AX
	memset(list3, 0, sizeof(list3));
#endif
	memset(acs_ifnames, 0, sizeof(acs_ifnames));
	memset(acs_ifnames2, 0, sizeof(acs_ifnames2));

	wl_list_5g_chans(wl_idx[1], 1, 0, list_5g_band1_chans, sizeof(list_5g_band1_chans), 0);
	wl_list_5g_chans(wl_idx[1], 2, war, list_5g_band2_chans, sizeof(list_5g_band2_chans), 0);
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
#if defined(RTCONFIG_HAS_5G_2)  //255 or 2556
	wl_list_5g_chans(wl_idx[2], 3, 0, list_5g_band3_chans, sizeof(list_5g_band3_chans), 0);
	wl_list_5g_chans(wl_idx[2], 5, 0, list_5g_unii4_chans, sizeof(list_5g_unii4_chans), 0);
#else	// 256 or 2566
	wl_list_5g_chans(wl_idx[1], 3, 0, list_5g_band3_chans, sizeof(list_5g_band3_chans), 0);
	wl_list_5g_chans(wl_idx[1], 5, 0, list_5g_unii4_chans, sizeof(list_5g_unii4_chans), 0);
#endif
#else // 25 or 255
	wl_list_5g_chans(WL_5G_BAND, 3, 0, list_5g_band3_chans, sizeof(list_5g_band3_chans), 0);
	if (!strlen(list_5g_band3_chans))
		wl_list_5g_chans(WL_5G_2_BAND, 3, 0, list_5g_band3_chans, sizeof(list_5g_band3_chans), 0);
	wl_list_5g_chans(WL_5G_BAND, 5, 0, list_5g_unii4_chans, sizeof(list_5g_unii4_chans), 0);
	if (!strlen(list_5g_unii4_chans))
		wl_list_5g_chans(WL_5G_2_BAND, 5, 0, list_5g_unii4_chans, sizeof(list_5g_unii4_chans), 0);
#endif

#ifdef RTCONFIG_AMAS
#if (defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7))
#if defined(RTCONFIG_HAS_5G_2) // 255 or 2556
	wl_list_5g_chans(wl_idx[2], 5, 0, list_5g_band4_bw160_chans, sizeof(list_5g_band4_bw160_chans), 4);
#else	//25 or 256 or 2566
	wl_list_5g_chans(wl_idx[1], 5, 0, list_5g_band4_bw160_chans, sizeof(list_5g_band4_bw160_chans), 4);
#endif
#else
	wl_list_5g_chans((num_of_wl_if() == 3) ? wl_idx[2] : wl_idx[1], 4, 0, list_5g_band4_bw160_chans, sizeof(list_5g_band4_bw160_chans), 4);
#endif
#if defined(RTCONFIG_WIFI7) // only apply for single 5G models
	memset(list_5g_band4_bw80_chans, 0, sizeof(list_5g_band4_bw80_chans));
	wl_list_5g_chans(wl_idx[1], 4, 0, list_5g_band4_bw80_chans, sizeof(list_5g_band4_bw80_chans), 3);
#endif
	char *p = strstr(list_5g_band4_bw160_chans, "0xeca3"); // drop chanspec 165/160
	if (p) *(p-1) = '\0';
	int excl_band4_bw160 = strlen(list_5g_band4_bw160_chans) && !nvram_get_int("acs_unii4");
#endif
#ifdef RTCONFIG_AVBLCHAN
	nvram_set("wl_acs_excl_chans_dfs", "");	
	nvram_set("wl_acs_excl_chans_dfs_2", "");	
#endif

#ifdef GT10
	foreach (word, "eth6 eth4 eth5", next) {
#elif defined(RTAX9000)
	foreach (word, "eth8 eth6 eth7", next) {
#elif defined(RTBE95U)
	foreach (word, "wl2 wl0 wl1", next) {
#elif defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
	foreach (word, "wl2 wl1 wl0", next) {
#else
	foreach (word, nvram_safe_get("wl_ifnames"), next) {
#endif
#ifdef RTCONFIG_QTN
		if (!strcmp(word, "wifi0")) break;
#endif
		unit = -1;
		wl_ioctl(word, WLC_GET_INSTANCE, &unit, sizeof(unit));
		if (unit == -1)
			continue;

		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

#ifdef RTCONFIG_AVBLCHAN
		ifnum++; 
		cfg_excl = nvram_safe_get(strcat_r(prefix, "acs_excl_chans_cfg", tmp));
		if(*cfg_excl) {
			init_cfg_excl(cfg_excl, cfg_excl_chans, unit);
		}
#endif
		if (nvram_match(strcat_r(prefix, "radio", tmp), "1") &&
			((nvram_match(strcat_r(prefix, "mode", tmp), "ap") &&
			 (nvram_match(strcat_r(prefix, "chanspec", tmp), "0") ||
			 (nvram_match(strcat_r(prefix, "bw", tmp), "0") &&
			  nvram_match(strcat_r(prefix, "nband", tmp2), "2"))))
			|| dpsta_mode()
#if defined(RTCONFIG_PROXYSTA) && defined(RTCONFIG_AMAS)
			|| (dpsr_mode() && strlen(nvram_safe_get("cfg_group")) > 0)
#endif
			|| rp_mode()
		)) {
			if (is_psr(unit)) {
				snprintf(wlvif, sizeof(wlvif), "wl%d.1", unit);
#if defined(RTCONFIG_AMAS)
				if (nvram_get_int("re_mode") == 0 || (nvram_get_int("re_mode") == 1 && add_interface_for_acsd(unit)))
#endif
				snprintf(acs_ifnames2, sizeof(acs_ifnames2), "%s%s%s", acs_ifnames, strlen(acs_ifnames) ? " " : "", wlvif);
			}
			else
			{
#ifdef RTCONFIG_AMAS_CHANNEL_PLAN
				if (nvram_get_int("channel_plan") == CHANNEL_PLAN_OFF || nvram_get_int("channel_plan") == CHANNEL_PLAN_ON)
#endif
				snprintf(acs_ifnames2, sizeof(acs_ifnames2), "%s%s%s", acs_ifnames, strlen(acs_ifnames) ? " " : "", word);
			}
			strlcpy(acs_ifnames, acs_ifnames2, sizeof(acs_ifnames));
		}

#if !defined(RTCONFIG_BCM_7114) && !defined(RTCONFIG_HND_ROUTER_AX)
		nvram_set(strcat_r(prefix, "acs_pol", tmp), "-65 40 -1 -100 -100 -1 -100 50 -100 0 1 0");
#endif
		if (nvram_match(strcat_r(prefix, "nband", tmp), "2"))
			/* exclude acsd from selecting chanspec 12, 12u, 13, 13u, 14, 14u */
			nvram_set(strcat_r(prefix, "acs_excl_chans", tmp), nvram_match("acs_ch13", "1") ? "" : "0x100c,0x190a,0x100d,0x190b,0x100e,0x190c");
	}

	nvram_set("acs_ifnames", acs_ifnames);
	/* default activate acs_band3 when exist 5G band support band3 but not support band4 */
	if ( (num_of_wl_if() == 2 && !(nvram_get_hex(strcat_r(prefix_5g, "band5grp", tmp)) & WL_5G_BAND_4) && !nvram_match("location_code", "RU"))
		|| nvram_match("acs_dfs", "1")
		|| (num_of_wl_if() >= 3 && 
			((wl_get_band(nvram_safe_get(strcat_r(prefix_5g, "ifname", tmp))) == WLC_BAND_5G
			 && !(nvram_get_hex(strcat_r(prefix_5g, "band5grp", tmp)) & WL_5G_BAND_4))
#if defined(RTCONFIG_HAS_5G_2)
		      && (wl_get_band(nvram_safe_get(strcat_r(prefix_5g2, "ifname", tmp))) == WLC_BAND_5G
			 && !(nvram_get_hex(strcat_r(prefix_5g2, "band5grp", tmp)) & WL_5G_BAND_4))
#endif
			)
		   )
	)
		nvram_set("acs_band3", "1");

#if defined(RTCONFIG_HAS_5G_2) && !defined(RTCONFIG_WIFI6E) && !defined(RTCONFIG_WIFI7)
	/* band 2 */
	nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? "" : list_5g_band2_chans);
	nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? "" : list_5g_band2_chans);

	/* band 4, UNII-4 */
	if (!nvram_get_int("acs_unii4")
#ifdef RTCONFIG_AMAS_ADTBW
	&& !(nvram_match("fh_ap_enabled", "1") && nvram_match("fh_ap_up", "0"))
#endif
	) {
		snprintf(list, sizeof(list), strlen(list_5g_unii4_chans) ? "%s,%s" : "%s", list_ch165, list_5g_unii4_chans);
#ifdef RTCONFIG_AMAS
		/* exclude band4 bw160 chanspecs once fronthaul ap provides */
		if (excl_band4_bw160) {
			strlcat(list, ",", sizeof(list));
			strlcat(list, list_5g_band4_bw160_chans, sizeof(list));
		}
#endif
	}

	/* band 3 */
	snprintf(list2, sizeof(list2), strlen(list_5g_band3_chans) ? "%s,%s" : "%s", list, list_5g_band3_chans);

	nvram_set(strcat_r(prefix_5g2, "acs_excl_chans", tmp), nvram_match("acs_band3", "1") ? list : list2);
	nvram_set("wl_acs_excl_chans_dfs_2", nvram_match("acs_band3", "1") ? list : list2);
#elif (defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7))
	/* band 4, UNII-4 */
	if (!nvram_get_int("acs_unii4")
#ifdef RTCONFIG_AMAS_ADTBW
	&& !(nvram_match("fh_ap_enabled", "1") && nvram_match("fh_ap_up", "0"))
#endif
	) {
		snprintf(list, sizeof(list), strlen(list_5g_unii4_chans) ? "%s,%s" : "%s", list_ch165, list_5g_unii4_chans);
#ifdef RTCONFIG_AMAS
		/* exclude band4 bw160 chanspecs once fronthaul ap provides */
		if (excl_band4_bw160) {
			strlcat(list, ",", sizeof(list));
			strlcat(list, list_5g_band4_bw160_chans, sizeof(list));
		}
#endif
	}
#if defined(RTCONFIG_WIFI7)
	/* special requirement from RF to exclude band4 chanspec to prevent interference from 6G AFC */
	if (!strncmp(nvram_safe_get("territory_code"), "US", 2) &&
	    nvram_match(strcat_r(prefix_5g, "bw_cap", tmp), "15") &&
	    nvram_match(strcat_r(prefix_5g, "bw", tmp), "0") &&
	    strlen(list_5g_band4_bw80_chans)) {
		if (strlen(list)) {
			strlcat(list, ",", sizeof(list));
			strlcat(list, list_5g_band4_bw80_chans, sizeof(list)); // band4
		}
		else {
			snprintf(list, sizeof(list), strlen(list_5g_unii4_chans) ? "%s,%s,%s" : "%s,%s", list_ch165, list_5g_band4_bw80_chans, list_5g_unii4_chans); // band4
		}
	}
#endif
	/* band 3 */
	snprintf(list2, sizeof(list2), strlen(list_5g_band3_chans) ? "%s,%s" : "%s", list, list_5g_band3_chans); // band4 + band3
	/* band 2 */
	snprintf(list3, sizeof(list3), strlen(list_5g_band2_chans) ? "%s,%s" : "%s", list2, list_5g_band2_chans); // band4 + band3 + band2
#ifdef RTCONFIG_HAS_5G_2
	nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? "" : list3);        // 5GL
	nvram_set(strcat_r(prefix_5g2, "acs_excl_chans", tmp), nvram_match("acs_band3", "1") ? list : list2);   // 5GH
#else
	nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? list : list3);      // 5G full band
#endif
	nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? list : list2) : list3);


#ifdef RTCONFIG_HAS_6G
#if defined(RTCONFIG_WIFI7) && defined(RTCONFIG_BW320M)
	if (!strncmp(nvram_safe_get("territory_code"), "EU", 2) || !strncmp(nvram_safe_get("territory_code"), "AU", 2) || !strncmp(nvram_safe_get("territory_code"), "JP", 2) || \
	    !strncmp(nvram_safe_get("territory_code"), "AA", 2) || !strncmp(nvram_safe_get("territory_code"), "TW", 2) || !strncmp(nvram_safe_get("territory_code"), "KR", 2)) {
	}
#else // WIFI6E
	if (!strncmp(nvram_safe_get("territory_code"), "EU", 2)) {
	}
#endif
	else {
#if defined(RTCONFIG_WIFI7) && defined(RTCONFIG_BW320M)
		if (hnd_boardid_cmp("RT-BE96U") == 0) {   // for 6726x A1 hardware only, also exclude 10G port affected chanspecs
			snprintf(list6g, sizeof(list6g), "%s,%s,%s,%s",
				list_6g_unii5_skip_non_eu,              // 6g1-6g29 bw20/40/80/160
				list_6g_unii8_skip_non_eu,		// 6g225, 6g229 bw20/40, 6g233
				list_6g_unii5_10g_phy_affect_skip,	// 10G port WAR - UNII5
				list_6g_unii6_10g_phy_affect_skip);	// 10G port WAR - UNII6
		}
		else 
		{
			if (!strncmp(nvram_safe_get("territory_code"), "US", 2))
				snprintf(list6g, sizeof(list6g), "%s",
					list_6g_unii8_skip_non_eu);		// 6g225, 6g229 bw20/40, 6g233
			else
				snprintf(list6g, sizeof(list6g), "%s,%s",
					list_6g_unii5_skip_non_eu,              // 6g1-6g29 bw20/40/80/160
					list_6g_unii8_skip_non_eu);		// 6g225, 6g229 bw20/40, 6g233
		}

		nvram_set(strcat_r(prefix_6g, "acs_excl_chans", tmp), list6g);
#ifdef RTCONFIG_HAS_6G_2
		snprintf(list6g, sizeof(list6g), "%s",
			list_6g_unii8_skip_non_eu);	// 6g225, 6g229 bw20/40, 6g233
		nvram_set(strcat_r(prefix_6g2, "acs_excl_chans", tmp), list6g);
#endif
		nvram_set("wl_acs_excl_chans_dfs_2", list6g);
#else // WIFI6E

		snprintf(list6g, sizeof(list6g), "%s,%s",
			list_6g_unii5_skip_non_eu,		// 6g1-6g29 bw20/40/80/160
			list_6g_unii8_skip_non_eu);		// 6g225, 6g229 bw20/40, 6g233
		nvram_set(strcat_r(prefix_6g, "acs_excl_chans", tmp), list6g);
		nvram_set("wl_acs_excl_chans_dfs_2", list6g);
#endif //defined(RTCONFIG_WIFI7) && defined(RTCONFIG_BW320M)
	}
#endif //RTCONFIG_HAS_6G
#else
	if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "7")) {		// EU, JP, UA
#ifdef RTAC66U
		if (!nvram_match(strcat_r(prefix_5g, "dfs", tmp), "1"))
			nvram_set("acs_dfs", "0");
#endif
		snprintf(list, sizeof(list), "%s,%s", list_5g_band2_chans, list_5g_band3_chans);
		if (!strncmp(nvram_safe_get("territory_code"), "UA", 2) || nvram_match("location_code", "RU")) {
			/* exclude acsd from selecting chanspec 100, 100l, 100/80, 100/160, 104, 104u, 104/80, 104/160, 108, 108l, 108/80, 108/160, 112, 112u, 112/80, 112/160, 116, 116l, 116/80, 116/160, 120, 120u, 120/80, 120/160, 124, 124l, 124/80, 124/160, 128, 128u, 128/80, 128/160, 132, 132l, 136, 136u, 140 */
			nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? "" : list_5g_band3_chans) : list);
			nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? "" : list_5g_band3_chans) : list);
		} else {
#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX86U) || defined(TUFAX5400) || defined(TUFAX5400_V2) || defined(GTAX6000) || defined(RTAX86U_PRO) || defined(RTAX3000N) || defined(BR63) || defined(RTAX88U_PRO) || defined(RTAX5400) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(RTBE58_GO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			if (!strncmp(nvram_safe_get("territory_code"), "JP", 2)
				|| !strncmp(nvram_safe_get("territory_code"), "KR", 2)) {
				/* exclude acsd from selecting chanspec 132/80 136/80 140l 140/80 144 144u 144/80 by default */
				nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? (nvram_match("acs_dfs_144", "1") ? "" :  "0xe08a,0xe18a,0xe28a,0xd88e,0xd090,0xe38a,0xd98e") : list);
				nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? (nvram_match("acs_dfs_144", "1") ? "" :  "0xe08a,0xe18a,0xe28a,0xd88e,0xd090,0xe38a,0xd98e") : list);
			} else {
#endif
				/* exclude acsd from selecting chanspec 52, 52l, 52/80, 52/160, 56, 56u, 56/80, 56/160, 60, 60l, 60/80, 60/160, 64, 64u, 64/80, 64/160, 100, 100l, 100/80, 100/160, 104, 104u, 104/80, 104/160, 108, 108l, 108/80, 108/160, 112, 112u, 112/80, 112/160, 116, 116l, 116/80, 116/160, 120, 120u, 120/80, 120/160, 124, 124l, 124/80, 124/160, 128, 128u, 128/80, 128/160, 132, 132l, 136, 136u, 140 */
				nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? "" : list);
				nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? "" : list);
#if defined(RTAX55) || defined(RTAX1800) || defined(RTAX86U) || defined(TUFAX5400) || defined(TUFAX5400_V2) || defined(GTAX6000) || defined(RTAX86U_PRO) || defined(RTAX3000N) || defined(BR63) || defined(RTAX88U_PRO) || defined(RTAX5400) || defined(RTBE58U) || defined(TUFBE3600) || defined(RTBE58U_V2) || defined(TUFBE3600_V2) || defined(RTBE55) || defined(RTBE92U) || defined(RTBE95U) || defined(RTBE82U) || defined(TUFBE82) || defined(RTBE82M) || defined(RTBE58U_PRO) || defined(RTBE58_GO) || defined(GSBE18000) || defined(GSBE12000) || defined(GS7_PRO) || defined(GT7)
			}
#endif
		}
	} else if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "9")) {	// US, CA, TW, SG, KR, AU (FCC)
		if (nvram_match(strcat_r(prefix_5g, "country_code", tmp), "US") ||
		    nvram_match(strcat_r(prefix_5g, "country_code", tmp), "Q1") || 
		    nvram_match(strcat_r(prefix_5g, "country_code", tmp), "Q2") ||
		    nvram_match(strcat_r(prefix_5g, "country_code", tmp), "SG") ||
		    !strncmp(nvram_safe_get("territory_code"), "US", 2) ||
		    !strncmp(nvram_safe_get("territory_code"), "AU", 2))
			// enable band 1 for US region
			nvram_set("acs_band1", "1");

		/* exclude acsd from selecting chanspec 36, 36l, 36/80, 36/160, 40, 40u, 40/80, 40/160, 44, 44l, 44/80, 44/160, 48, 48u, 48/80, 48/160, 165 for non-US region by default */
		snprintf(list, sizeof(list), "%s,0xd0a5", list_5g_band1_chans);
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_band1", "1") ? "0xd0a5" : list);
	} else if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "b")) {
		snprintf(list, sizeof(list), "%s,0xd0a5", list_5g_band2_chans);
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? "0xd0a5" : list);
		nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? "0xd0a5" : list);
#if defined(RTCONFIG_HND_ROUTER_AX)
	} else if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "f")) {	// AU (NEW), FCC-DFS
		/* exclude acsd from selecting chanspec 100, 100l, 100/80, 100/160, 104, 104u, 104/80, 104/160, 108, 108l, 108/80, 108/160, 112, 112u, 112/80, 112/160, 116, 116l, 116/80, 116/160, 120, 120u, 120/80, 120/160, 124, 124l, 124/80, 124/160, 128, 128u, 128/80, 128/160, 132, 132l, 136, 136u, 140, 165 */
		snprintf(list, sizeof(list), "%s,0xd0a5", list_5g_band3_chans);
		snprintf(list2, sizeof(list2), "%s,%s,0xd0a5", list_5g_band2_chans, list_5g_band3_chans);
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? "" : list) : list2);
		nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? "" : list) : list2);
	} else if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "1f")) {	// FCC-DFS including UNII-4
		/* exclude acsd from selecting chanspec 100, 100l, 100/80, 100/160, 104, 104u, 104/80, 104/160, 108, 108l, 108/80, 108/160, 112, 112u, 112/80, 112/160, 116, 116l, 116/80, 116/160, 120, 120u, 120/80, 120/160, 124, 124l, 124/80, 124/160, 128, 128u, 128/80, 128/160, 132, 132l, 136, 136u, 140, 165, 165l, 165/80, 165/160, and UNII-4 chanspecs */
		if (!nvram_get_int("acs_unii4")
#ifdef RTCONFIG_AMAS_ADTBW
		&& !(nvram_match("fh_ap_enabled", "1") && nvram_match("fh_ap_up", "0"))
#endif
		) {
			snprintf(list, sizeof(list), strlen(list_5g_unii4_chans) ? "%s,%s" : "%s", list_ch165, list_5g_unii4_chans);
#ifdef RTCONFIG_AMAS
			/* exclude band4 bw160 chanspecs once fronthaul ap provides */
			if (excl_band4_bw160) {
				strlcat(list, ",", sizeof(list));
				strlcat(list, list_5g_band4_bw160_chans, sizeof(list));
			}
#endif
		}
		snprintf(list2, sizeof(list2), strlen(list_5g_band3_chans) ? "%s,%s" : "%s", list, list_5g_band3_chans);
		snprintf(list3, sizeof(list3), strlen(list_5g_band2_chans) ? "%s,%s" : "%s", list2, list_5g_band2_chans);
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? list : list2) : list3);
		nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? (nvram_match("acs_band3", "1") ? list : list2) : list3);
#else
	} else if (nvram_match(strcat_r(prefix_5g, "band5grp", tmp), "f")) {	// AU (NEW)
		/* exclude acsd from selecting chanspec 52, 52l, 52/80, 56, 56u, 56/80, 60, 60l, 60/80, 64, 64u, 64/80, 100, 100l, 100/80, 104, 104u, 104/80, 108, 108l, 108/80, 112, 112u, 112/80, 116, 132, 132l, 136, 136u, 140, 165 */
		snprintf(list, sizeof(list), "%s,%s,0xd0a5", list_5g_band2_chans, list_5g_band3_chans);
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), nvram_match("acs_dfs", "1") ? "0xd0a5" : list);
		nvram_set("wl_acs_excl_chans_dfs", nvram_match("acs_dfs", "1") ? "0xd0a5" : list);
#endif
	} else {					// CN, AU (FCC + CE)
		/* exclude acsd from selecting chanspec 165 */
		nvram_set(strcat_r(prefix_5g, "acs_excl_chans", tmp), "0xd0a5");
	}
#endif

#if defined(XT8PRO) || defined(BT12) || defined(BT10) || defined(BQ16) || defined(BQ16_PRO) || defined(BM68) || defined(XD4PRO)
	if(nvram_match("location_code", "EU")){
		acs_excl_list_eu_10min(acs_5g_unit);
	}else{
		/* without location_code */
		if(nvram_match("territory_code", "EU/01")){
			acs_excl_list_eu_10min(acs_5g_unit);
		}
	}
#ifdef RTCONFIG_AVBLCHAN
	//dump_acs_excl_chans("rc-aft-10m");
#endif
#endif

#ifdef RTCONFIG_AVBLCHAN
	int i, excinit = 0;
	excinit = reset_exclbase(ifnum);

	RC_DBG_ABL("chk rc event of amas eid [%d]\n", nvram_get_int("abl_eid"));
	RC_DBG_ABL("%s, rc based. excinit=%d, fh_ap_enabled=%d, fh_ap_up=%d, acs_unii4=%d\n", __func__, excinit, nvram_get_int("fh_ap_enabled"), nvram_get_int("fh_ap_up"), nvram_get_int("acs_unii4"));
	_dprintf("%s, rc based. excinit=%d, fh_ap_enabled=%d, fh_ap_up=%d, acs_unii4=%d\n", __func__, excinit, nvram_get_int("fh_ap_enabled"), nvram_get_int("fh_ap_up"), nvram_get_int("acs_unii4"));

	//dump_acs_excl_chans("rc-bef-cfg");
	
	if(!excinit)
		add_cfgexcl_2_acsexcl(cfg_excl_chans);

	//dump_acs_excl_chans("rc-aft-cfg");
#endif
	nvram_set_int(strcat_r(prefix_2g, "acs_dfs", tmp), 0);
	nvram_set_int(strcat_r(prefix_5g, "acs_dfs", tmp), nvram_match(strcat_r(prefix_5g, "reg_mode", tmp2), "h") ? 1 : 0);
#ifdef RTCONFIG_HAS_5G_2
	nvram_set_int(strcat_r(prefix_5g2, "acs_dfs", tmp), nvram_match(strcat_r(prefix_5g2, "reg_mode", tmp2), "h") ? 1 : 0);
#endif

	/* workaround to 675x/49xx dual band models */
//#if defined(RTCONFIG_HND_ROUTER_AX)
//#if !defined(RTCONFIG_HAS_5G_2) && !defined(RTCONFIG_QUADBAND)
//	if(nvram_match("acs_dfs", "1") && !nvram_match("avblchan_reset", "1")){
//		syslog(LOG_NOTICE, "reset wl1_acs_excl_chans as empty due acs_dfs is 1 (war of 675x/49xx dual band models).\n");
//		nvram_set("wl1_acs_excl_chans", "");
//	}
//#endif
//#endif

#ifdef RTCONFIG_AVBLCHAN
	nvram_set("avblchan_reset", "");
#endif
}
#endif

#ifdef RTCONFIG_PORT_BASED_VLAN
int check_used_stb_voip_port(int lan)
{
	int used = 0;
	int used_port = 0;

	/* L4 L3 L2 L1 */
	/* 8  4  2  1 */
	if (!nvram_match("switch_wantag", "none") && !nvram_match("switch_wantag", "")) {
		if (!strcmp(nvram_safe_get("switch_wantag"), "unifi_home"))
			used_port = 0x8;	/* LAN4 for stb */
		if (!strcmp(nvram_safe_get("switch_wantag"), "unifi_biz"))
			used_port = 0x0;
		else if (!strcmp(nvram_safe_get("switch_wantag"), "singtel_mio"))
			used_port = 0xc;	/* LAN4 for stb, LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "singtel_others"))
			used_port = 0x8;	/* LAN4 for stb */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "m1_fiber"))
			used_port = 0x4;	/* LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber"))
			used_port = 0x4;	/* LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "maxis_fiber_sp"))
			used_port = 0x4;	/* LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "maxis_cts"))
			used_port = 0x4;	/* LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "maxis_sacofa"))
			used_port = 0x4;	/* LAN3 for voip */
		else if (!strcmp(nvram_safe_get("switch_wantag"), "maxis_tnb"))
			used_port = 0x4;	/* LAN3 for voip */
		else	/* For manual */
		{
			if (strcmp(nvram_safe_get("switch_wan1tagid"), ""))
				used_port += 0x8;	/* LAN4 for stb */

			if (strcmp(nvram_safe_get("switch_wan2tagid"), ""))
				used_port += 0x4;	/* LAN3 for voip */
		}
	}
	else	/* For none */
	{
		int stbport = 0;

		stbport = nvram_get_int("switch_stb_x");
		if (stbport < 0 || stbport > 6)
			stbport = 0;

		if (stbport >= 1 && stbport <= 4)
			used_port = 1 << (stbport - 1);
		else if (stbport == 5)	/* LAN1 & LAN2 */
			used_port = 0x3;
		else if (stbport == 6)	/* LAN3 & LAN4 */
			used_port = 0xc;
	}

	if (lan & used_port)
		used = 1;

	return used;
}

unsigned int convert_vlan_entry(int tag_enable, int portset, char *tag_reg_val)
{
	int real_portset = 0;
	int model, i;
	int port_shift_bit[] = { 0, 0, 0, 0};	/* shift bit for LAN X */
	char *port_tag_reg[] = { "0x10", "0x12", "0x14", "0x16", "0x18", "0x1a" };
	unsigned int vlan_entry = 0;

	model = get_model();

	/* P0  P1 P2 P3 P4 */
	/* WAN L4 L3 L2 L1 */
	if (model == MODEL_RTN16) {
		port_shift_bit[0] = 4;
		port_shift_bit[1] = 3;
		port_shift_bit[2] = 2;
		port_shift_bit[3] = 1;
	}
	/* P0  P1 P2 P3 P5 */
	/* WAN L4 L3 L2 L1 */
	else if (model == MODEL_RTAC87U) {
		port_shift_bit[0] = 5;
		port_shift_bit[1] = 3;
		port_shift_bit[2] = 2;
		port_shift_bit[3] = 1;
	}
	/* P0  P1 P2 P3 P4 */
	/* WAN L1 L2 L3 L4 */
	else if (model == MODEL_RTAC68U || model == MODEL_RTN18U ||
		model == MODEL_RTN66U || model == MODEL_RTAC66U ||
		model == MODEL_DSLAC68U) {
		port_shift_bit[0] = 1;
		port_shift_bit[1] = 2;
		port_shift_bit[2] = 3;
		port_shift_bit[3] = 4;
	}
	/* P0 P1 P2 P3 P4 */
	/* L1 L2 L3 L4 WAN */
	else if (model == MODEL_RTAC56S || model == MODEL_RTAC56U) {
		port_shift_bit[0] = 0;
		port_shift_bit[1] = 1;
		port_shift_bit[2] = 2;
		port_shift_bit[3] = 3;
	}
	/* P0 P1 P2 P3 P4 */
	/* L4 L3 L2 L1 WAN */
	else if (model == MODEL_RTN15U) {
		port_shift_bit[0] = 3;
		port_shift_bit[1] = 2;
		port_shift_bit[2] = 1;
		port_shift_bit[3] = 0;
	}

	/* Convert the port set of web to real port set of switch */
	_dprintf("%s: temp portset=0x%x\n", __FUNCTION__, portset);
	for (i = 0; i < sizeof(port_shift_bit)/sizeof(int); i++) {
#ifdef RTCONFIG_DUALWAN
		int wancfg = nvram_get_int("wans_lanport");
		if ((get_wans_dualwan() & WANSCAP_LAN) && wancfg >= 1 && wancfg <= 4) {
			/* Filter lan port as wan */
			if ((i == (wancfg -1)) && (portset & (1 << (wancfg - 1))))
				continue;
		}
#endif
		if (portset & (1 << i) && !check_used_stb_voip_port(1 << i))
			real_portset |= (1 << port_shift_bit[i]);
	}
	_dprintf("%s: real portset=0x%x\n", __FUNCTION__, real_portset);

	/* Set the temporary value of vlan entry for port 0 ~ port 4 */
	for (i = 0; i < sizeof(port_shift_bit)/sizeof(int); i++) {
		int port_val = real_portset & (1 << port_shift_bit[i]);

		if (port_val) {
#ifndef HND_ROUTER
			eval("et", "robowr", "0x34", port_tag_reg[port_shift_bit[i]], tag_reg_val);//
			_dprintf("%s: port tag reg=%s\n", __FUNCTION__, port_tag_reg[port_shift_bit[i]]);
#endif

			if (tag_enable)
				vlan_entry |= (1 << port_shift_bit[i]);
			else
				vlan_entry |= ((1 << (9 + port_shift_bit[i])) | (1 << port_shift_bit[i]));
		}
	}

	return vlan_entry;
}

unsigned int convert_vlan_entry_bcm5325(int tag_enable, int portset, char *tag_reg_val)
{
	int real_portset = 0;
	int model, i;
	int port_shift_bit[] = { 0, 0, 0, 0};	/* shift bit for LAN X */
	char *port_tag_reg[] = { "0x10", "0x12", "0x14", "0x16", "0x18", "0x1a" };
	unsigned int vlan_entry = 0;

	model = get_model();

	/* P0 P1 P2 P3 P4 */
	/* L4 L3 L2 L1 WAN */
	if (model == MODEL_APN12HP || model == MODEL_RTN53 || model == MODEL_RTN12 ||
		model == MODEL_RTN12B1 || model == MODEL_RTN12C1 || model == MODEL_RTN12D1 ||
		model == MODEL_RTN12VP || model == MODEL_RTN12HP || model == MODEL_RTN12HP_B1 ||
		model == MODEL_RTN10P || model == MODEL_RTN10D1 || model == MODEL_RTN10PV2) {
		port_shift_bit[0] = 3;
		port_shift_bit[1] = 2;
		port_shift_bit[2] = 1;
		port_shift_bit[3] = 0;
	}
	/* P0 P1 P2 P3 P4 */
	/* L1 L2 L3 L4 WAN */
	else if (model == MODEL_RTN14UHP || model == MODEL_RTN10U) {
		port_shift_bit[0] = 0;
		port_shift_bit[1] = 1;
		port_shift_bit[2] = 2;
		port_shift_bit[3] = 3;
	}
	/* P0  P1 P2 P3 P4 */
	/* WAN L1 L2 L3 L4 */
	else if (model == MODEL_RTAC53U) {
		port_shift_bit[0] = 1;
		port_shift_bit[1] = 2;
		port_shift_bit[2] = 3;
		port_shift_bit[3] = 4;
	}

	/* Convert the port set of web to real port set of switch */
	_dprintf("%s: temp portset=0x%x\n", __FUNCTION__, portset);
	for (i = 0; i < sizeof(port_shift_bit)/sizeof(int); i++) {
#ifdef RTCONFIG_DUALWAN
		int wancfg = nvram_get_int("wans_lanport");
		if ((get_wans_dualwan() & WANSCAP_LAN) && wancfg >= 1 && wancfg <= 4) {
			/* Filter lan port as wan */
			if ((i == (wancfg -1)) && (portset & (1 << (wancfg - 1))))
				continue;
		}
#endif
		if (portset & (1 << i) && !check_used_stb_voip_port(1 << i))
			real_portset |= (1 << port_shift_bit[i]);
	}
	_dprintf("%s: real portset=0x%x\n", __FUNCTION__, real_portset);

	/* Set the temporary value of vlan entry for port 0 ~ port 4 */
	for (i = 0; i < sizeof(port_shift_bit)/sizeof(int); i++) {
		int port_val = real_portset & (1 << port_shift_bit[i]);

		if (port_val) {
#ifndef HND_ROUTER
			eval("et", "robowr", "0x34", port_tag_reg[port_shift_bit[i]], tag_reg_val);//
			_dprintf("%s: port tag reg=0x%s\n", __FUNCTION__, port_tag_reg[port_shift_bit[i]]);
#endif

			if (tag_enable)
				vlan_entry |= (1 << port_shift_bit[i]);
			else
				vlan_entry |= ((1 << (6 + port_shift_bit[i])) | (1 << port_shift_bit[i]));
		}
	}

	return vlan_entry;
}

void set_port_based_vlan_config(char *interface)
{
	char *nv, *nvp, *b;
	//char *enable, *vid, *priority, *portset, *wlmap, *subnet_name;
	//char *portset, *wlmap, *subnet_name;
	char *enable, *desc, *portset, *wlmap, *subnet_name, *intranet;
	int set_flag = (interface != NULL) ? 1 : 0;

	/* Clean some parameters for vlan */
	//clean_vlan_ifnames();

	if (vlan_enable()) {
		nv = nvp = strdup(nvram_safe_get("vlan_rulelist"));

		if (nv) {
			int vlan_tag = 4;
			int model;
			int br_index = 1;

			model = get_model();

			if (model == MODEL_DSLAC68U)
				vlan_tag = 5;

			while ((b = strsep(&nvp, "<")) != NULL) {
				//int real_portset = 0;
				char tag_reg_val[7], vlan_id[9], vlan_entry[12];
				unsigned int vlan_entry_tmp = 0, tag_reg_val_tmp = 0;
				int i = 0, vlan_id_tmp = 0;
				int cpu_port = 0;

				//if ((vstrsep(b, ">", &enable, &vid, &priority, &portset, &wlmap, &subnet_name) != 6))
				//if ((vstrsep(b, ">", &portset, &wlmap, &subnet_name) != 3))
				if ((vstrsep(b, ">", &enable, &desc, &portset, &wlmap, &subnet_name, &intranet) != 6))
					continue;

				//_dprintf("%s: %s %s %s %s %s %s\n", __FUNCTION__, enable, vid, priority, portset, wlmap, subnet_name);
				_dprintf("%s: %s %s %s %s %s %s\n", __FUNCTION__, enable, desc, portset, wlmap, subnet_name, intranet);

				if (!strcmp(enable, "0") || strlen(enable) == 0)
					continue;

				//real_portset = atoi(portset);
				//real_portset = convert_portset(atoi(portset));
				//_dprintf("%s: real port set=0x%x\n", __FUNCTION__, real_portset);

				/* Port mapping of the switch for MODEL_RTN16 */
				/* P0  P1 P2 P3 P4 P8 */
				/* WAN L4 L3 L2 L1 CPU */
				/* Port mapping of the switch for MODEL_RTAC68U & MODEL_RTN18U */
				/* P0  P1 P2 P3 P4 P5 */
				/* WAN L1 L2 L3 L4 CPU */
				/* Port mapping of the switch for MODEL_RTAC87U */
				/* P0  P1 P2 P3 P5 P7 */
				/* WAN L4 L3 L2 L1 CPU */
				/* Port mapping of the switch for MODEL_RTAC56S & MODEL_RTAC56U */
				/* P0 P1 P2 P3 P4  P5 */
				/* L1 L2 L3 L4 WAN CPU */
				/* Port mapping of the switch for MODEL_RTN66U & MODEL_RTAC66U */
				/* P0  P1 P2 P3 P4 P8 */
				/* WAN L1 L2 L3 L4 CPU */
				/* Port mapping of the switch for MODEL_RTN15U */
				/* P0 P1 P2 P3 P4  P8 */
				/* L1 L2 L3 L4 WAN CPU */
				/* Port mapping of the switch for MODEL_DSLAC68U */
				/* P0  P1 P2 P3 P4 P5 */
				/* WAN L1 L2 L3 L4 CPU */
				if (model == MODEL_RTN16 ||
					model == MODEL_RTAC68U || model == MODEL_RTN18U ||
					model == MODEL_RTAC87U ||
					model == MODEL_RTAC56S || model == MODEL_RTAC56U ||
					model == MODEL_RTN66U || model == MODEL_RTAC66U ||
					model == MODEL_RTN15U ||
					model == MODEL_DSLAC68U) {
					/*char tag_reg_val[7], vlan_id[9], vlan_entry[7];
					unsigned int vlan_entry_tmp = 0, tag_reg_val_tmp = 0;
					int i = 0, vlan_id_tmp = 0;
					//char *port_tag_reg[] = { "0x12", "0x14", "0x16", "0x18" };
					int cpu_port = 0;*/

					/* Decide cpu port by model */
					if (model == MODEL_RTAC68U || model == MODEL_RTN18U ||
						model == MODEL_RTAC56S || model == MODEL_RTAC56U ||
						model == MODEL_DSLAC68U)
						cpu_port = 5;
					else if (model == MODEL_RTAC87U)
						cpu_port = 7;
					else if (model == MODEL_RTN16 ||
						model == MODEL_RTN66U || model == MODEL_RTAC66U ||
						MODEL_RTN15U)
						cpu_port = 8;

					if (atoi(portset) != 0) {
						vlan_id_tmp = vlan_tag;
						tag_reg_val_tmp = vlan_tag;
						vlan_tag++;

						snprintf(tag_reg_val, sizeof(tag_reg_val), "0x%x", tag_reg_val_tmp);
						_dprintf("%s: tag register value=%s\n", __FUNCTION__, tag_reg_val);

						if (set_flag) {
							/* Set vlan entry for port 0 ~ port 4 */
							//vlan_entry_tmp = convert_vlan_entry(atoi(enable), atoi(portset), tag_reg_val);
							vlan_entry_tmp = convert_vlan_entry(0, atoi(portset), tag_reg_val);

							/* Set vlan entry for cpu port */
							vlan_entry_tmp |= (1 << cpu_port);
						}

						/* Set vlan table entry register */
						snprintf(vlan_id, sizeof(vlan_id), "0x%x", vlan_id_tmp);
						_dprintf("%s: vlan id=%s\n", __FUNCTION__, vlan_id);
						snprintf(vlan_entry, sizeof(vlan_entry), "0x%x", vlan_entry_tmp);
						_dprintf("%s: vlan entry=%s\n", __FUNCTION__, vlan_entry);
						if (set_flag) {
#ifndef HND_ROUTER
							eval("et", "robowr", "0x05", "0x83", vlan_entry);
							eval("et", "robowr", "0x05", "0x81", vlan_id);
#if !defined(RTAC87U)
							eval("et", "robowr", "0x05", "0x80", "0x0000");
#endif
							eval("et", "robowr", "0x05", "0x80", "0x0080");
#endif

							/* Create the VLAN interface */
							snprintf(vlan_id, sizeof(vlan_id), "%d", vlan_id_tmp);
							eval("vconfig", "add", interface, vlan_id);

							/* Setup ingress map (vlan->priority => skb->priority) */
							snprintf(vlan_id, sizeof(vlan_id), "vlan%d", vlan_id_tmp);
							for (i = 0; i < VLAN_NUMPRIS; i++) {
								char prio[8];

								snprintf(prio, sizeof(prio), "%d", i);
								eval("vconfig", "set_ingress_map", vlan_id, prio, prio);
							}
						}

						snprintf(vlan_id, sizeof(vlan_id), "vlan%d", vlan_id_tmp);
						set_vlan_ifnames(br_index, atoi(wlmap), subnet_name, vlan_id);
					}
					else	/* portset is 0, only for wireless */
					{
						set_vlan_ifnames(br_index, atoi(wlmap), subnet_name, NULL);
					}
				}
				/* Port mapping of the switch for MODEL_APN12HP, MODEL_RTN53, MODEL_RTN12
				   MODEL_RTN12B1, MODEL_RTN12C1, MODEL_RTN12D1, MODEL_RTN12VP, MODEL_RTN12HP
				   MODEL_RTN12HP_B1, MODEL_RTN10P, MODEL_RTN10D1 and MODEL_RTN10PV2 */
				/* P0 P1 P2 P3 P4  P5 */
				/* L4 L3 L2 L1 WAN CPU */
				/* Port mapping of the switch for MODEL_RTN14UHP and MODEL_RTN10U */
				/* P0 P1 P2 P3 P4  P5 */
				/* L1 L2 L3 L4 WAN CPU */
				/* Port mapping of the switch for MODEL_RTAC53U */
				/* P0  P1 P2 P3 P4 P5 */
				/* WAN L1 L2 L3 L4 CPU */
				else
				{
					/*char tag_reg_val[7], vlan_id[9], vlan_entry[12];
					unsigned int vlan_entry_tmp = 0, tag_reg_val_tmp = 0;
					int i = 0, vlan_id_tmp = 0;*/
					cpu_port = 5;

					/* Enable high bits check */
#ifndef HND_ROUTER
					eval("et", "robowr", "0x34", "0x3", "0x0080");
#endif

					if (atoi(portset) != 0) {
						vlan_id_tmp = vlan_tag;
						tag_reg_val_tmp = vlan_tag;
						vlan_tag++;

						snprintf(tag_reg_val, sizeof(tag_reg_val), "0x%x", tag_reg_val_tmp);
						_dprintf("%s: tag register value=%s\n", __FUNCTION__, tag_reg_val);

						if (set_flag) {
							/* Set vlan entry for port 0 ~ port 4 */
							//vlan_entry_tmp = convert_vlan_entry_bcm5325(atoi(enable), atoi(portset), tag_reg_val);
							vlan_entry_tmp = convert_vlan_entry_bcm5325(0, atoi(portset), tag_reg_val);

							/* Set vlan entry for cpu port */
							vlan_entry_tmp |= (1 << cpu_port);
						}

						/* Set vlan table entry register */
						snprintf(vlan_id, sizeof(vlan_id), "0x%x", ((1 << 13) |	(1 << 12) | vlan_id_tmp));
						_dprintf("%s: vlan id=%s\n", __FUNCTION__, vlan_id);
						snprintf(vlan_entry, sizeof(vlan_entry), "0x%x", ((1 << 24) | (vlan_id_tmp << 12) | vlan_entry_tmp));
						_dprintf("%s: vlan entry=%s\n", __FUNCTION__, vlan_entry);

						if (set_flag) {
							eval("et", "robowr", "0x34", "0x8", vlan_entry);
							eval("et", "robowr", "0x34", "0x6", vlan_id);

							/* Create the VLAN interface */
							snprintf(vlan_id, sizeof(vlan_id), "%d", vlan_id_tmp);
							eval("vconfig", "add", interface, vlan_id);

							/* Setup ingress map (vlan->priority => skb->priority) */
							snprintf(vlan_id, sizeof(vlan_id), "vlan%d", vlan_id_tmp);
							for (i = 0; i < VLAN_NUMPRIS; i++) {
								char prio[8];

								snprintf(prio, sizeof(prio), "%d", i);
								eval("vconfig", "set_ingress_map", vlan_id, prio, prio);
							}
						}

						snprintf(vlan_id, sizeof(vlan_id), "vlan%d", vlan_id_tmp);
						set_vlan_ifnames(br_index, atoi(wlmap), subnet_name, vlan_id);
					}
					else	/* portset is 0, only for wireless */
					{
						set_vlan_ifnames(br_index, atoi(wlmap), subnet_name, NULL);
					}
				}

				br_index++;
			}
			free(nv);
		}
	}
	return;
}
#endif

#ifdef HND_ROUTER
void fc_init()
{
	int fc_enable = !nvram_get_int("fc_disable");
#if 0
	int unit;

	for (unit = WAN_UNIT_FIRST; unit < WAN_UNIT_MAX; unit++) {
		if (dualwan_unit__usbif (unit)) {
			fc_enable = 0;
			break;
		}
	}
#endif

	eval("fc", fc_enable ? "enable" : "disable");
}

void fc_fini()
{
	eval("fc", "disable");
	eval("fc", "flush");
}

void hnd_nat_ac_init(int bootup)
{
	int routing_mode = is_routing_enabled();

	// A.QOS : not to disable fc
	nvram_set_int("fc_disable", nvram_get_int("fc_disable_force") || (routing_mode && IS_NON_AQOS() && !IS_RB_QOS()) ? 1 : 0);

	// A.QOS : no need to disable runner
	nvram_set_int("runner_disable", nvram_get_int("runner_disable_force") || (routing_mode && IS_NON_AQOS() && !IS_RB_QOS()) ? 1 : 0);

#ifdef RTCONFIG_HND_ROUTER_BE_4916
	/*
		For SDN models, adjust logic
		4916           : can't work with fc due to lost iqos patch and must disable fc in RE
	*/
	if (nvram_get_int("re_mode") == 1) {
		// bw limiter : disable fc
		nvram_set_int("fc_disable", nvram_get_int("fc_disable_force") || (IS_BW_QOS()) ? 1 : 0);
	}
#endif

	if (nvram_match("fc_disable", "1"))
		fc_fini();
	else if (!bootup)
		fc_init();

#ifdef RTCONFIG_HND_ROUTER_AX
	eval("fc", "config", "--tcp-ack-mflows", nvram_get_int("fc_tcp_ack_mflows_disable_force") ? "0" : "1");
#endif

	if (nvram_match("runner_disable", "1"))
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
		/* apply archer instead of runner */
		eval("fc", "config", "--hw-accel", "0");
#else
		eval("runner", "disable");
#endif
	else if (!bootup)
#if defined(RTCONFIG_HND_ROUTER_AX_675X)
		/* apply archer instead of runner */
		eval("fc", "config", "--hw-accel", "1");
		eval("archerctl", "sysport_tm", "disable");
#else
		eval("runner", "enable");
#endif
}
#endif

#if defined(HND_ROUTER) || defined(RTCONFIG_BCM_7114) || defined(RTCONFIG_BCM4708)
/* This function updates the nvram radio_dmode_X to NIC/DGL depending on driver mode */
void wl_driver_mode_update(void)
{
	int unit = -1, maxunit = -1;
	int i = 0;
	char ifname[16] = {0};

	/* Search for existing wl devices with eth prefix and the max unit number used */
	for (i = 0; i <= DEV_NUMIFS; i++) {
		snprintf(ifname, sizeof(ifname), WL_IF_PREFIX, i);
		if (!wl_probe(ifname)) {
			int unit = -1;
			char mode_str[128];
			char *mode = "NIC";

#ifdef __CONFIG_DHDAP__
			mode = dhd_probe(ifname) ? "NIC" : "DGL";
#endif // endif

			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit))) {
				maxunit = (unit > maxunit) ? unit : maxunit + 1;
				sprintf(mode_str, "wlradio_dmode_%d", maxunit);
				if (strcmp(nvram_safe_get(mode_str), mode) != 0) {
					_dprintf("%s: Setting %s = %s\n", __FUNCTION__, mode_str, mode);
					nvram_set(mode_str, mode);
				}
			}
		}
	}

	/* Search for existing wl devices with wl prefix and the max unit number used */
	for (i = 0; i <= DEV_NUMIFS; i++) {
		snprintf(ifname, sizeof(ifname), "wl%d", i);
		if (!wl_probe(ifname)) {
			char mode_str[128];
			char *mode = "NIC";

#ifdef __CONFIG_DHDAP__
			mode = dhd_probe(ifname) ? "NIC" : "DGL";
#endif // endif

			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit))) {
				sprintf(mode_str, "wlradio_dmode_%d", i);
				if (strcmp(nvram_get(mode_str), mode) != 0) {
					_dprintf("%s: Setting %s = %s\n", __FUNCTION__, mode_str, mode);
					nvram_set(mode_str, mode);
				}
			}
		}
	}
}
#endif

#ifdef BCM_BSD
void smart_connect_realign_ifnames() {

	int wlif_count = num_of_wl_if();
	int i = 0, j = 0, bandtype;
	char tmp[32], prefix[]="wlXXXXXXX_";
	char tmp2[32], prefix2[]="wlXXXXXXX_";
	char tmp3[32];
	char bsd_ifnames[32];
	char bsd_if_sel[32];
#if defined(RTCONFIG_BCMBSD_V2)
	char ifname[32];
	int idx_5g = 0;
	int idx_6g = 0;
	int smartconnect_selif = nvram_get_int("smart_connect_selif");
	int smartconnect_selif_x = nvram_get_int("smart_connect_selif_x");
	char target_bss[16];
#endif
#if defined(RTCONFIG_MULTILAN_MWL)
	char word[16], *next;
	char sdn_selif_names[64];
	char sdn_selif[MAX_RADIO_NUM][8];
	int sdn_selif_count;
#endif

#if !defined(RTCONFIG_MULTILAN_MWL)
	if (nvram_match("re_mode", "1"))
		return;
#endif

#if defined(RTCONFIG_BCMBSD_V2)
	if(!nvram_get_int("smart_connect_x"))
		return;
#if defined(RTCONFIG_MULTILAN_MWL)
	memset(sdn_selif_names, 0, sizeof(sdn_selif_names));
	sdn_selif_count = 0;
	get_fh_sc_ifnames(&smartconnect_selif_x, &sdn_selif_names, sizeof(sdn_selif_names));
	if(smartconnect_selif_x != nvram_get_int("smart_connect_selif_x"))
		nvram_set_int("smart_connect_selif_x", smartconnect_selif_x);

	if(strlen(sdn_selif_names) > 0) {
		dbg("fh_sc_ifnames[%s], bitmap[%x]\n", sdn_selif_names, smartconnect_selif_x);
	}
#endif

	_dprintf("smart_connect_x=[%d]\n", nvram_get_int("smart_connect_x"));
	_dprintf("smart_connect_selif_x=[%d], smart_connect_selif=[%d] ", smartconnect_selif_x, smartconnect_selif);

	if(smartconnect_selif > 0  && smartconnect_selif_x > 0 && smartconnect_selif != smartconnect_selif_x) {
		snprintf(bsd_ifnames, sizeof(bsd_ifnames), "%s", nvram_safe_get("bsd_ifnames"));
		foreach(word, bsd_ifnames, next) {
			if(strlen(word)) {
				nvram_unset(strcat_r(word, "_bsd_if_select_policy", tmp));
				nvram_unset(strcat_r(word, "_bsd_sta_select_policy", tmp));
				nvram_unset(strcat_r(word, "_bsd_sta_select_policy_def", tmp));
			}
                }
        }

	memset(bsd_ifnames, 0, sizeof(bsd_ifnames));
#if defined(RTCONFIG_MULTILAN_MWL)
	foreach(word, sdn_selif_names, next) {
		snprintf(prefix, sizeof(prefix), "%s_", word);
		if(sdn_selif_count < MAX_RADIO_NUM) {
			snprintf(sdn_selif[sdn_selif_count], sizeof(sdn_selif[MAX_RADIO_NUM]), "%s", word);
			sdn_selif_count++;
		}
#else
	for (i = 0; i < wlif_count; i++) {
		snprintf(prefix, sizeof(prefix), "wl%d_", i);
#endif
		bandtype = nvram_get_int(strcat_r(prefix, "nband", tmp));
		if((bandtype == WLC_BAND_2G && smartconnect_selif_x & SMRTCONN_SEL_2G))
			add_to_list(nvram_safe_get(strcat_r(prefix, "ifname", tmp)) , bsd_ifnames, sizeof(bsd_ifnames));
		if(bandtype == WLC_BAND_5G) {
			if( idx_5g == 0 && smartconnect_selif_x & SMRTCONN_SEL_5G 
#if defined(RTCONFIG_HAS_5G_2)
			 || (idx_5g == 0 || idx_5g == 1) && smartconnect_selif_x & SMRTCONN_SEL_5G2
#endif
			) {
				add_to_list(nvram_safe_get(strcat_r(prefix, "ifname", tmp)) , bsd_ifnames, sizeof(bsd_ifnames));
				idx_5g++;
			}
		}
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
		if(bandtype == WLC_BAND_6G) { 
			if (idx_6g == 0 && smartconnect_selif_x & SMRTCONN_SEL_6G
#if defined(RTCONFIG_HAS_6G_2)
			 || (idx_6g == 0 || idx_6g == 1) && smartconnect_selif_x & SMRTCONN_SEL_6G2
#endif
			) {
				add_to_list(nvram_safe_get(strcat_r(prefix, "ifname", tmp)) , bsd_ifnames, sizeof(bsd_ifnames));
				idx_6g++;
			}
		}
#endif
	}
	if(strlen(bsd_ifnames)) {
		if((smartconnect_selif_x & SMRTCONN_SEL_2G) == 0) {
			// smart_connect_x: 2
			_dprintf("bsd_ifnames_x=[%s]\n",bsd_ifnames);
			nvram_set("bsd_ifnames_x", bsd_ifnames);
		}

		// smart_connect_x: 1
		_dprintf("bsd_ifnames=[%s]\n",bsd_ifnames);
		nvram_set("bsd_ifnames", bsd_ifnames);
	}

#if defined(RTCONFIG_MULTILAN_MWL)
        foreach(word, sdn_selif_names, next) {
                snprintf(prefix, sizeof(prefix), "%s_", word);
#else
	for (i = 0; i < wlif_count; i++) {
		snprintf(prefix, sizeof(prefix), "wl%d_", i);
#endif
		snprintf(ifname, sizeof(ifname), "%s", nvram_safe_get(strcat_r(prefix, "ifname", tmp)));
		if(!find_in_list(bsd_ifnames, ifname))
			continue;
		bandtype = nvram_get_int(strcat_r(prefix, "nband", tmp));
		memset(bsd_if_sel, 0, sizeof(bsd_if_sel));
		memset(tmp2, 0, sizeof(tmp2));
		memset(tmp3, 0, sizeof(tmp3));

		/* 2.4G */
		if(bandtype == WLC_BAND_2G && smartconnect_selif_x & SMRTCONN_SEL_2G) { // 2.4G is selected (smart_connect_x: 1)

			if(!strlen(nvram_safe_get(strcat_r(prefix, "bsd_if_select_policy", tmp)))) {
//WL_2G_BAND, WL_5G_BAND, WL_6G_BAND, WL_6G_2_BAND
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
				// 6G/6G-1 is the first priority
				if(smartconnect_selif_x & SMRTCONN_SEL_6G) {
					snprintf(prefix2, sizeof(prefix2), "%s_", smart_connect_get_active_prefix_by_bandid(target_bss, sizeof(target_bss), WL_6G_BAND));
					if(find_in_list(bsd_ifnames, (nvram_safe_get(strcat_r(prefix2, "ifname", tmp2))))) {
						_dprintf("%s: add [%s] into target ifname\n", ifname, nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)));
						add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
					}
				}
#endif
#if defined(RTCONFIG_HAS_6G_2)
				if(smartconnect_selif_x & SMRTCONN_SEL_6G2) {
                                        snprintf(prefix2, sizeof(prefix2), "%s_", smart_connect_get_active_prefix_by_bandid(target_bss, sizeof(target_bss), WL_6G_2_BAND));
					if(find_in_list(bsd_ifnames, (nvram_safe_get(strcat_r(prefix2, "ifname", tmp2))))) {
						_dprintf("%s: add [%s] into target ifname\n", ifname, nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)));
						add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
					}
				}
#endif
#if defined(RTCONFIG_HAS_5G_2)
				if(smartconnect_selif_x & SMRTCONN_SEL_5G2) {
                                        snprintf(prefix2, sizeof(prefix2), "%s_", smart_connect_get_active_prefix_by_bandid(target_bss, sizeof(target_bss), WL_5G_2_BAND));
					if(find_in_list(bsd_ifnames, (nvram_safe_get(strcat_r(prefix2, "ifname", tmp2))))) {
						_dprintf("%s: add [%s] into target ifname\n", ifname, nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)));
						add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
					}
				}
#endif
				if(smartconnect_selif_x & SMRTCONN_SEL_5G) {
                                        snprintf(prefix2, sizeof(prefix2), "%s_", smart_connect_get_active_prefix_by_bandid(target_bss, sizeof(target_bss), WL_5G_BAND));
					if(find_in_list(bsd_ifnames, (nvram_safe_get(strcat_r(prefix2, "ifname", tmp2))))) {
						_dprintf("%s: add [%s] into target ifname\n", ifname, nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)));
						add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
					}
				}

				if(strlen(bsd_if_sel)) {
					_dprintf("%s: %s=[%s]\n", ifname, strcat_r(prefix, "bsd_if_select_policy", tmp), bsd_if_sel);
					nvram_set(strcat_r(prefix, "bsd_if_select_policy", tmp), bsd_if_sel);
				}

			}
		}

		/* 5G/6G/6G2 */
		if ((bandtype == WLC_BAND_5G && ((smartconnect_selif_x & SMRTCONN_SEL_5G) || (smartconnect_selif_x & SMRTCONN_SEL_5G2)))  // 5G or 5G-2 is selected
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_HAS_6G)
		|| (bandtype == WLC_BAND_6G && ((smartconnect_selif_x & SMRTCONN_SEL_6G) || (smartconnect_selif_x & SMRTCONN_SEL_6G2))) // 6G or 6G-2 is selected
#endif
		) {
			if(!strlen(nvram_safe_get(strcat_r(prefix, "bsd_if_select_policy", tmp)))) {
#if defined(RTCONFIG_MULTILAN_MWL)
				for (j = sdn_selif_count - 1; j >= 0; j--) {
					if(!strncmp(word, sdn_selif[j], strlen(word)))
						continue;
					snprintf(prefix2, sizeof(prefix2), "%s_", sdn_selif[j]);
#else
				for (j = wlif_count - 1; j >= 0; j--) {
					if(j == i)
						continue;
					snprintf(prefix2, sizeof(prefix2), "wl%d_", j);
#endif
					if (find_in_list(bsd_ifnames, (nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)))))
					{
						_dprintf("%s: add [%s] into target ifname\n", ifname, nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)));
						if( nvram_get_int(strcat_r(prefix2, "nband", tmp2)) == WLC_BAND_2G && strlen(bsd_if_sel) > 0) {
							strncpy(tmp3, bsd_if_sel, strlen(bsd_if_sel));
							snprintf(bsd_if_sel, sizeof(bsd_if_sel), "%s %s", nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)), tmp3);
						}
						else
							add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
					}

				}
				if(strlen(bsd_if_sel)) {
					_dprintf("%s: %s=[%s]\n", ifname, strcat_r(prefix, "bsd_if_select_policy", tmp), bsd_if_sel);
					nvram_set(strcat_r(prefix, "bsd_if_select_policy", tmp), bsd_if_sel);
				}
			}
		}
	}

	if(
#if !defined(RTCONFIG_MULTILAN_MWL)
		smartconnect_selif > 0  && smartconnect_selif_x > 0 && 
#endif
		smartconnect_selif != smartconnect_selif_x) {
		_dprintf("%s, change selif (%d->%d)\n", __func__, smartconnect_selif, smartconnect_selif_x);
		nvram_set_int("smart_connect_selif", smartconnect_selif_x);
		nvram_commit();
		gen_bcmbsd_def_policy(smartconnect_selif_x);
	}

	return;
#else
	if(nvram_get_int("smart_connect_x") == SMRTCONN_2G_AND_5G) {
		memset(bsd_ifnames, 0, sizeof(bsd_ifnames));
		for (i = 0; i < wlif_count; i++) {
			memset(bsd_if_sel, 0, sizeof(bsd_if_sel));
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			bandtype = nvram_get_int(strcat_r(prefix, "nband", tmp));
			if(bandtype == 2 || bandtype == 1)
				add_to_list(nvram_safe_get(strcat_r(prefix, "ifname", tmp)) , bsd_ifnames, sizeof(bsd_ifnames));
			else
				continue;

			for (j = wlif_count - 1; j >= 0; j--) {
				snprintf(prefix2, sizeof(prefix2), "wl%d_", j);
				if (j == i)
				    continue;
				bandtype = nvram_get_int(strcat_r(prefix2, "nband", tmp2));
				if(bandtype == 2 || bandtype == 1)
					add_to_list(nvram_safe_get(strcat_r(prefix2, "ifname", tmp2)) , bsd_if_sel, sizeof(bsd_if_sel));
			}

			if(strlen(bsd_if_sel))
				nvram_set(strcat_r(prefix, "bsd_if_select_policy", tmp), bsd_if_sel);
		}

		if(strlen(bsd_ifnames))
			nvram_set("bsd_ifnames", bsd_ifnames);

	}
#endif
}

void smart_connect_sync_config(int unit) {

	char tmp[100], prefix[]="wlXXXXXXX_";
	char tmp2[100], prefix2[]="wlXXXXXXX_";
	int wlif_count = num_of_wl_if();
	int i = 0;
#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_PRELINK) && !defined(RTCONFIG_MSSID_PRELINK)
	int result = 0;
#endif

	if(!nvram_get_int("smart_connect_x"))
		return;

#if defined(RTCONFIG_BCMBSD_V2)
	// wifi configuration sync will be done through GUI setting directly
	return;
#endif

	if (((unit == WL_2G_BAND) && nvram_get_int("smart_connect_x") == SMRTCONN_FULL_BANDS) ||// Full bands smart connect
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
	   ((unit == WL_2G_BAND) && nvram_get_int("smart_connect_x") == SMRTCONN_2G_AND_5G) ||	// 2.4GHz + 5GHz smart connect
#endif
	   ((unit == WL_5G_BAND) && nvram_get_int("smart_connect_x") == SMRTCONN_5G_ONLY)	// 5GHz only smart connect
	) {
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		for (i = unit + 1; i < wlif_count; i++) {
#ifdef RTCONFIG_DWB
			if((nvram_get_int("dwb_mode") == 1 || nvram_get_int("dwb_mode") == 3) && nvram_get_int("dwb_band") > 0 && i == nvram_get_int("dwb_band"))
			{
				dbg("Don't apply wl0 to wl%d in smart_connect function, if enabled DWB mode.\n", i);
				continue;
			}
#endif
#if defined(RTCONFIG_AMAS) && defined(RTCONFIG_PRELINK) && !defined(RTCONFIG_MSSID_PRELINK)
			result = 0;
			if (amas_prelink_band_sync_bypass(i, &result) == AMAS_RESULT_SUCCESS) {
				if (result == 1) {
					_dprintf("Do not apply wl0 to wl%d, it meets prelink setting.\n", i);
					continue;
				}
			}
#endif
			snprintf(prefix2, sizeof(prefix2), "wl%d_", i);
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
			if (nvram_get_int("smart_connect_x") == SMRTCONN_2G_AND_5G && nvram_get_int(strcat_r(prefix2, "nband", tmp2)) != 1) { // only sync 5G with 2.4G
				continue;
			}

			if (nvram_get_int(strcat_r(prefix2, "nband", tmp2)) == 4) { // 6G band, convert to supported auth mode
				if (!strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "open") ||
				    !strcmp(nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)), "openowe")) {
					nvram_set(strcat_r(prefix2, "auth_mode_x", tmp2), "owe");
					nvram_set(strcat_r(prefix2, "crypto", tmp2), "");
				}
				else {
					nvram_set(strcat_r(prefix2, "auth_mode_x", tmp2), "sae");
					nvram_set(strcat_r(prefix2, "crypto", tmp2), "aes");
				}
			}
			else
#endif
			{
				nvram_set(strcat_r(prefix2, "auth_mode_x", tmp2), nvram_safe_get(strcat_r(prefix, "auth_mode_x", tmp)));
				nvram_set(strcat_r(prefix2, "crypto", tmp2), nvram_safe_get(strcat_r(prefix, "crypto", tmp)));
			}
			nvram_set(strcat_r(prefix2, "ssid", tmp2), nvram_safe_get(strcat_r(prefix, "ssid", tmp)));
			nvram_set(strcat_r(prefix2, "wep_x", tmp2), nvram_safe_get(strcat_r(prefix, "wep_x", tmp)));
			nvram_set(strcat_r(prefix2, "key", tmp2), nvram_safe_get(strcat_r(prefix, "key", tmp)));
			nvram_set(strcat_r(prefix2, "key1", tmp2), nvram_safe_get(strcat_r(prefix, "key1", tmp)));
			nvram_set(strcat_r(prefix2, "key2", tmp2), nvram_safe_get(strcat_r(prefix, "key2", tmp)));
			nvram_set(strcat_r(prefix2, "key3", tmp2), nvram_safe_get(strcat_r(prefix, "key3", tmp)));
			nvram_set(strcat_r(prefix2, "key4", tmp2), nvram_safe_get(strcat_r(prefix, "key4", tmp)));
			nvram_set(strcat_r(prefix2, "phrase_x", tmp2), nvram_safe_get(strcat_r(prefix, "phrase_x", tmp)));
			nvram_set(strcat_r(prefix2, "wpa_psk", tmp2), nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp)));
			nvram_set(strcat_r(prefix2, "radius_ipaddr", tmp2), nvram_safe_get(strcat_r(prefix, "radius_ipaddr", tmp)));
			nvram_set(strcat_r(prefix2, "radius_key", tmp2), nvram_safe_get(strcat_r(prefix, "radius_key", tmp)));
			nvram_set(strcat_r(prefix2, "radius_port", tmp2), nvram_safe_get(strcat_r(prefix, "radius_port", tmp)));
#if defined(RTCONFIG_MULTILAN_CFG)
			nvram_set(strcat_r(prefix2, "radius_acct_ipaddr", tmp2), nvram_safe_get(strcat_r(prefix, "radius_acct_ipaddr", tmp)));
			nvram_set(strcat_r(prefix2, "radius_acct_key", tmp2), nvram_safe_get(strcat_r(prefix, "radius_acct_key", tmp)));
			nvram_set(strcat_r(prefix2, "radius_acct_port", tmp2), nvram_safe_get(strcat_r(prefix, "radius_acct_port", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_ipaddr", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_ipaddr", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_key", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_key", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_port", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_port", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_acct_ipaddr", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_acct_ipaddr", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_acct_key", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_acct_key", tmp)));
			nvram_set(strcat_r(prefix2, "radius2_acct_port", tmp2), nvram_safe_get(strcat_r(prefix, "radius2_acct_port", tmp)));
#endif
			nvram_set(strcat_r(prefix2, "closed", tmp2), nvram_safe_get(strcat_r(prefix, "closed", tmp)));
#if defined(RTCONFIG_HND_ROUTER_AX)
			nvram_set(strcat_r(prefix2, "11ax", tmp2), nvram_safe_get(strcat_r(prefix, "11ax", tmp)));
			nvram_set(strcat_r(prefix2, "mbo_enable", tmp2), nvram_safe_get(strcat_r(prefix, "mbo_enable", tmp)));
			nvram_set(strcat_r(prefix2, "mfp", tmp2), nvram_safe_get(strcat_r(prefix, "mfp", tmp)));
#endif
		}
#if defined(RTCONFIG_WIFI6E) || defined(RTCONFIG_WIFI7)
		smart_connect_realign_ifnames();
#endif
	}

}
#endif
