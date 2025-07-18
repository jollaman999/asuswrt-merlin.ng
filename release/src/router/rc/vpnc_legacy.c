/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2004, ASUSTeK Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */
#include "rc.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>															
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>

/* TODO: remove */
#undef VPNC_LEGACY

#define L2TP_VPNC_PID	"/var/run/l2tpd-vpnc.pid"
#define L2TP_VPNC_CTRL	"/var/run/l2tpctrl-vpnc"
#define L2TP_VPNC_CONF	"/tmp/l2tp-vpnc.conf"

int vpnc_unit = 5;

int legacy_vpnc_pppstatus(void)
{
	char statusfile[sizeof("/var/run/ppp-vpnXXXXXXXXXX.status")];

	snprintf(statusfile, sizeof(statusfile), "/var/run/ppp-vpn%d.status", vpnc_unit);
	return _pppstatus(statusfile);
}

int
legacy_start_vpnc(void)
{
	FILE *fp;
	char options[80];
	char *pppd_argv[] = { "/usr/sbin/pppd", "file", options, NULL};
	char tmp[100], prefix[] = "vpnc_", wan_prefix[] = "wanXXXXXXXXXX_";
	char wan_proto[16];
	char buf[256];	/* although maximum length of pppoe_username/pppoe_passwd is 64. pppd accepts up to 256 characters. */
	mode_t mask;
	int ret = 0;

	snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());
	snprintf(wan_proto, sizeof(wan_proto), "%s", nvram_safe_get(strcat_r(wan_prefix, "proto", tmp)));

/* TODO: remove */
#if 0
	if (nvram_match(strcat_r(wan_prefix, "proto", tmp), "pptp") || nvram_match(strcat_r(wan_prefix, "proto", tmp), "l2tp"))
		return 0;
#endif
	if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp"))
		sprintf(options, "/tmp/ppp/vpnc_options.pptp");
	else if (nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
		sprintf(options, "/tmp/ppp/vpnc_options.l2tp");
	else
		return 0;

	/* shut down previous instance if any */
	legacy_stop_vpnc();

#if !defined(DSL_AX82U) ///TODO: !defined(RTCONFIG_HND_ROUTER_AX_675X)
#ifdef HND_ROUTER
	/* workaround for ppp packets are dropped by fc GRE learning when pptp server / client enabled */
	if (nvram_match("fc_disable", "0") &&
		(!strcmp(wan_proto, "pppoe") ||
		 !strcmp(wan_proto, "pptp") ||
		 !strcmp(wan_proto, "l2tp"))) {
		dbg("[%s, %d] Flow Cache Learning of GRE flows Tunnel: DISABLED, PassThru: ENABLED\n", __FUNCTION__, __LINE__);
		eval("fc", "config", "--gre", "0");
	}
#endif
#endif

	/* unset vpnc_dut_disc */
	nvram_unset(strcat_r(prefix, "dut_disc", tmp));

	legacy_update_vpnc_state(prefix, WAN_STATE_INITIALIZING, 0);

	mask = umask(0000);

	/* Generate options file */
	if (!(fp = fopen(options, "w"))) {
		perror(options);
		umask(mask);
		return -1;
	}

	umask(mask);

#ifdef VPNC_LEGACY
	/* route for pptp/l2tp's server */
	if (nvram_match(strcat_r(wan_prefix, "proto", tmp), "pptp") || nvram_match(strcat_r(wan_prefix, "proto", tmp), "l2tp")) {
		char *wan_ifname = nvram_safe_get(strcat_r(wan_prefix, "pppoe_ifname", tmp));
		route_add(wan_ifname, 0, nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0", "255.255.255.255");
	}
#endif /* VPNC_LEGACY */

	/* do not authenticate peer and do not use eap */
	fprintf(fp, "noauth\n");
	fprintf(fp, "refuse-eap\n");
	fprintf(fp, "user '%s'\n",
		ppp_safe_escape(nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)), buf, sizeof(buf)));
	fprintf(fp, "password '%s'\n",
		ppp_safe_escape(nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)), buf, sizeof(buf)));

	if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp")) {
		fprintf(fp, "plugin pptp.so\n");
		fprintf(fp, "pptp_server '%s'\n",
			nvram_invmatch(strcat_r(prefix, "heartbeat_x", tmp), "") ?
			nvram_safe_get(strcat_r(prefix, "heartbeat_x", tmp)) :
			nvram_safe_get(strcat_r(prefix, "gateway_x", tmp)));
		/* see KB Q189595 -- historyless & mtu */
		if (nvram_match(strcat_r(wan_prefix, "proto", tmp), "pptp") || nvram_match(strcat_r(wan_prefix, "proto", tmp), "l2tp"))
			fprintf(fp, "nomppe-stateful mtu 1300\n");
		else
			fprintf(fp, "nomppe-stateful mtu 1400\n");

		if (nvram_match(strcat_r(prefix, "pptp_options_x", tmp), "-mppc")) {
			fprintf(fp, "nomppe nomppc\n");
		} else
		if (nvram_match(strcat_r(prefix, "pptp_options_x", tmp), "+mppe-40")) {
			fprintf(fp, "require-mppe\n"
				    "require-mppe-40\n");
		} else
		if (nvram_match(strcat_r(prefix, "pptp_options_x", tmp), "+mppe-56")) {
			fprintf(fp, "nomppe-40\n"
				    "require-mppe\n"
				    "require-mppe-56\n");
		} else
		if (nvram_match(strcat_r(prefix, "pptp_options_x", tmp), "+mppe-128")) {
			fprintf(fp, "nomppe-40\n"
				    "nomppe-56\n"
				    "require-mppe\n"
				    "require-mppe-128\n");
		} else
		if (nvram_match(strcat_r(prefix, "pptp_options_x", tmp), "")) {
			fprintf(fp, "require-mppe-40\n"
				    "require-mppe-56\n"
				    "require-mppe-128\n");
		}
	} else {
		fprintf(fp, "nomppe nomppc\n");

		if (nvram_match(strcat_r(wan_prefix, "proto", tmp), "pptp") || nvram_match(strcat_r(wan_prefix, "proto", tmp), "l2tp"))
			fprintf(fp, "mtu 1300\n");
		else
			fprintf(fp, "mtu 1400\n");			
	}

	if (nvram_invmatch(strcat_r(prefix, "proto", tmp), "l2tp")) {
		ret = nvram_get_int(strcat_r(prefix, "pppoe_idletime", tmp));
		if (ret && nvram_get_int(strcat_r(prefix, "pppoe_demand", tmp))) {
			fprintf(fp, "idle %d ", ret);
			if (nvram_invmatch(strcat_r(prefix, "pppoe_txonly_x", tmp), "0"))
				fprintf(fp, "tx_only ");
			fprintf(fp, "demand\n");
		}
		fprintf(fp, "persist\n");
	}

	fprintf(fp, "holdoff %d\n", nvram_get_int(strcat_r(prefix, "pppoe_holdoff", tmp)) ? : 10);
	fprintf(fp, "maxfail %d\n", nvram_get_int(strcat_r(prefix, "pppoe_maxfail", tmp)));

	if (nvram_invmatch(strcat_r(prefix, "dnsenable_x", tmp), "0"))
		fprintf(fp, "usepeerdns\n");

	fprintf(fp, "ipcp-accept-remote ipcp-accept-local noipdefault\n");
	fprintf(fp, "ktune\n");

	/* pppoe set these options automatically */
	/* looks like pptp also likes them */
	fprintf(fp, "default-asyncmap nopcomp noaccomp\n");

	/* pppoe disables "vj bsdcomp deflate" automagically */
	/* ccp should still be enabled - mppe/mppc requires this */
	fprintf(fp, "novj nobsdcomp nodeflate\n");

	/* echo failures */
	fprintf(fp, "lcp-echo-interval 6\n");
	fprintf(fp, "lcp-echo-failure 10\n");

	/* pptp has Echo Request/Reply, l2tp has Hello packets */
	if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp") ||
	    nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
		fprintf(fp, "lcp-echo-adaptive\n");

	fprintf(fp, "unit %d\n", vpnc_unit);
	fprintf(fp, "linkname vpn%d\n", vpnc_unit);
	fprintf(fp, "ip-up-script %s\n", "/tmp/ppp/vpnc-ip-up");
	fprintf(fp, "ip-down-script %s\n", "/tmp/ppp/vpnc-ip-down");
	fprintf(fp, "ip-pre-up-script %s\n", "/tmp/ppp/vpnc-ip-pre-up");
	fprintf(fp, "auth-fail-script %s\n", "/tmp/ppp/vpnc-auth-fail");

	/* user specific options */
	fprintf(fp, "%s\n",
		nvram_safe_get(strcat_r(prefix, "pppoe_options_x", tmp)));

	fclose(fp);

#if 0
	/* shut down previous instance if any */
	legacy_stop_vpnc();

	nvram_unset(strcat_r(prefix, "dut_disc", tmp));
#endif

	if (nvram_match(strcat_r(prefix, "proto", tmp), "l2tp"))
	{
		if (!(fp = fopen(L2TP_VPNC_CONF, "w"))) {
			perror(options);
			return -1;
		}

		fprintf(fp,
			"global\n\n"
			"load-handler \"sync-pppd.so\"\n"
			"load-handler \"cmd.so\"\n\n"
			"section sync-pppd\n\n"
			"lac-pppd-opts \"file %s\"\n\n"
			"section peer\n"
			"port 1701\n"
			"peername %s\n"
			"hostname %s\n"
			"lac-handler sync-pppd\n"
			"persist yes\n"
			"maxfail %d\n"
			"holdoff %d\n"
			"hide-avps no\n"
			"section cmd\n"
			"socket-path " L2TP_VPNC_CTRL "\n\n",
			options,
                        nvram_invmatch(strcat_r(prefix, "heartbeat_x", tmp), "") ?
                                nvram_safe_get(strcat_r(prefix, "heartbeat_x", tmp)) :
                                nvram_safe_get(strcat_r(prefix, "gateway_x", tmp)),
			nvram_invmatch(strcat_r(prefix, "hostname", tmp), "") ?
				nvram_safe_get(strcat_r(prefix, "hostname", tmp)) : "localhost",
			nvram_get_int(strcat_r(prefix, "pppoe_maxfail", tmp))  ? : 32767,
			nvram_get_int(strcat_r(prefix, "pppoe_holdoff", tmp)) ? : 10);

		fclose(fp);

		/* launch l2tp */
		eval("/usr/sbin/l2tpd", "-c", L2TP_VPNC_CONF, "-p", L2TP_VPNC_PID);

		ret = 3;
		do {
			_dprintf("%s: wait l2tpd up at %d seconds...\n", __FUNCTION__, ret);
			usleep(1000*1000);
		} while (!pids("l2tpd") && ret--);

		/* start-session */
		ret = eval("/usr/sbin/l2tp-control", "-s", L2TP_VPNC_CTRL, "start-session 0.0.0.0");

		/* pppd sync nodetach noaccomp nobsdcomp nodeflate */
		/* nopcomp novj novjccomp file /tmp/ppp/options.l2tp */

	} else
		ret = _eval(pppd_argv, NULL, 0, NULL);

	legacy_update_vpnc_state(prefix, WAN_STATE_CONNECTING, 0);

	return ret;
}

void
legacy_stop_vpnc(void)
{
	char pidfile[sizeof("/var/run/ppp-vpnXXXXXXXXXX.pid")];
	char tmp[100], prefix[] = "vpnc_";
	
	snprintf(pidfile, sizeof(pidfile), "/var/run/ppp-vpn%d.pid", vpnc_unit);

	/* Reset the state of vpnc_dut_disc */
	nvram_set_int(strcat_r(prefix, "dut_disc", tmp), 1);

	/* Stop l2tp */
	if(check_if_file_exist(L2TP_VPNC_PID))
	{
		kill_pidfile_tk(L2TP_VPNC_PID);
		usleep(1000*10000);
	}

	/* Stop pppd */
	if (kill_pidfile_s(pidfile, SIGHUP) == 0 &&
	    kill_pidfile_s(pidfile, SIGTERM) == 0) {
		usleep(3000*1000);
		kill_pidfile_tk(pidfile);
	}

#if !defined(DSL_AX82U) ///TODO: !defined(RTCONFIG_HND_ROUTER_AX_675X)
#ifdef HND_ROUTER
	/* workaround for ppp packets are dropped by fc GRE learning when pptp server / client enabled */
	if (nvram_match("fc_disable", "0")) eval("fc", "config", "--gre", "1");
#endif
#endif
}

int
legacy_vpnc_ppp_linkunit(char *linkname)
{
	if (strncmp(linkname, "vpn", 3))
		return -1;
	if (!isdigit(linkname[3]))
		return -1;
	return atoi(&linkname[3]);
}

void legacy_update_vpnc_state(char *prefix, int state, int reason)
{
	char tmp[100];

	_dprintf("%s(%s, %d, %d)\n", __FUNCTION__, prefix, state, reason);

	nvram_set_int(strcat_r(prefix, "state_t", tmp), state);
	nvram_set_int(strcat_r(prefix, "sbstate_t", tmp), 0);

	// 20110610, reset auxstate each time state is changed
	//nvram_set_int(strcat_r(prefix, "auxstate_t", tmp), 0);

	if (state == WAN_STATE_STOPPED) {
		// Save Stopped Reason
		// keep ip info if it is stopped from connected
		nvram_set_int(strcat_r(prefix, "sbstate_t", tmp), reason);
	}
	else if(state == WAN_STATE_STOPPING) {
		snprintf(tmp, sizeof(tmp), "/var/run/ppp-vpn%d.status", vpnc_unit);
		unlink(tmp);
	}
}

int legacy_vpnc_update_resolvconf(void)
{
	FILE *fp, *fp_servers;
	char tmp[100], prefix[] = "vpnc_";
	char *wan_dns, *next;
	int lock;
#ifdef RTCONFIG_YANDEXDNS
	int yadns_mode = nvram_get_int("yadns_enable_x") ? nvram_get_int("yadns_mode") : YADNS_DISABLED;
#endif
#ifdef RTCONFIG_DNSPRIVACY
	int dnspriv_enable = nvram_get_int("dnspriv_enable");
#endif

	lock = file_lock("resolv");

	if (!(fp = fopen("/tmp/resolv.conf", "w+"))) {
		perror("/tmp/resolv.conf");
		goto error;
	}
#ifdef RTCONFIG_YANDEXDNS
	if (yadns_mode != YADNS_DISABLED) {
		/* keep yandex.dns servers */
		fp_servers = NULL;
	} else
#endif
#ifdef RTCONFIG_DNSPRIVACY
	if (dnspriv_enable) {
		/* keep dns privacy servers */
		fp_servers = NULL;
	} else
#endif
	if (!(fp_servers = fopen("/tmp/resolv.dnsmasq", "w+"))) {
		perror("/tmp/resolv.dnsmasq");
		fclose(fp);
		goto error;
	}

	wan_dns = nvram_safe_get(strcat_r(prefix, "dns", tmp));
	foreach(tmp, wan_dns, next) {
		fprintf(fp, "nameserver %s\n", tmp);
		if (fp_servers)
			fprintf(fp_servers, "server=%s\n", tmp);
	}

	fclose(fp);
	if (fp_servers)
		fclose(fp_servers);
	file_unlock(lock);

#ifdef RTCONFIG_MULTILAN_CFG
	reload_dnsmasq(NULL);
#else
	reload_dnsmasq();
#endif
	return 0;

error:
	file_unlock(lock);
	return -1;
}

void legacy_vpnc_add_firewall_rule()
{
	char tmp[100], prefix[] = "vpnc_", wan_prefix[] = "wanXXXXXXXXXX_";
	char *vpnc_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
	char *wan_proto = NULL;
	char lan_if[IFNAMSIZ+1];

	strcpy(lan_if, nvram_safe_get("lan_ifname"));

	if (check_if_file_exist(strcat_r("/tmp/ppp/link.", vpnc_ifname, tmp)))
	{
		snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());
		wan_proto = nvram_safe_get(strcat_r(wan_prefix, "proto", tmp));
		if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static"))
			//eval("iptables", "-I", "FORWARD", "-p", "tcp", "--syn", "-j", "TCPMSS", "--clamp-mss-to-pmtu");
			eval("iptables", "-I", "FORWARD", "-p", "tcp", "--tcp-flags", "SYN,RST", "SYN", "-j", "TCPMSS", "--clamp-mss-to-pmtu");
#ifdef RTCONFIG_BCMARM
		else	/* mark tcp connection to bypass CTF */
#ifdef HND_ROUTER
			if (nvram_match("fc_disable", "0") && nvram_match("fc_pt_war", "1"))
#else
			if (nvram_match("ctf_disable", "0"))
#endif
			eval("iptables", "-t", "mangle", "-A", "FORWARD", "-p", "tcp", 
				"-m", "state", "--state", "NEW","-j", "MARK", "--set-mark", "0x01/0x7");
#endif

		eval("iptables", "-A", "VPNCF", "-o", vpnc_ifname, "!", "-i", lan_if, "-j", "DROP");
		eval("iptables", "-A", "VPNCF", "-i", vpnc_ifname, "-j", "ACCEPT");
		eval("iptables", "-t", "nat", "-I", "PREROUTING", "-d", 
			nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), "-j", "VSERVER");
		eval("iptables", "-t", "nat", "-I", "POSTROUTING", "-o", 
			vpnc_ifname, "!", "-s", nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), "-j", "MASQUERADE");
	}
}

void
legacy_vpnc_up(char *vpnc_ifname)
{
#ifdef VPNC_LEGACY
	int ret = 0;
	char tmp[100], prefix[] = "vpnc_", wan_prefix[] = "wanXXXXXXXXXX_";
	char *wan_ifname = NULL, *wan_proto = NULL;

	snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());
	wan_proto = nvram_safe_get(strcat_r(wan_prefix, "proto", tmp));

	if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static"))
		wan_ifname = nvram_safe_get(strcat_r(wan_prefix, "ifname", tmp));
	else
		wan_ifname = nvram_safe_get(strcat_r(wan_prefix, "pppoe_ifname", tmp));

	/* Reset default gateway route via PPPoE interface */
	if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static")) {
		route_del(wan_ifname, 0, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
		route_add(wan_ifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
	}
	else if (!strcmp(wan_proto, "pppoe") || !strcmp(wan_proto, "pptp") || !strcmp(wan_proto,  "l2tp"))
	{
		char *wan_xgateway = nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp));

		route_del(wan_ifname, 0, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
		route_add(wan_ifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");

		if (strlen(wan_xgateway) > 0 && strcmp(wan_xgateway, "0.0.0.0")) {
			char *wan_xifname =  nvram_safe_get(strcat_r(wan_prefix, "ifname", tmp));

			route_del(wan_xifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp)), "0.0.0.0");
			route_add(wan_xifname, 3, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp)), "0.0.0.0");
		}
	}
	
	/* Add the default gateway of VPN client */
	ret = route_add(vpnc_ifname, 0, "0.0.0.0", nvram_safe_get(strcat_r(prefix, "gateway", tmp)), "0.0.0.0");
	if(ret != 0) {
		_dprintf("%s: fail to add route table\n", __FUNCTION__);
		legacy_update_vpnc_state(prefix, WAN_STATE_STOPPED, WAN_STOPPED_REASON_IPGATEWAY_CONFLICT);
		return;
	}
	/* Remove route to the gateway - no longer needed */
	route_del(vpnc_ifname, 0, nvram_safe_get(strcat_r(prefix, "gateway", tmp)), NULL, "255.255.255.255");
#else /* VPNC_LEGACY */
	char tmp[100], prefix[] = "vpnc_";
	char *gateway = nvram_safe_get(strcat_r(prefix, "gateway", tmp));

	/* Add default routes via VPN interface */
	if (nvram_get_int(strcat_r(prefix, "defroute_x", tmp))) {
		if (route_add(vpnc_ifname, 0, "0.0.0.0", gateway, "128.0.0.0") != 0)
			goto error;
		if (route_add(vpnc_ifname, 0, "128.0.0.0", gateway, "128.0.0.0") != 0) {
			route_del(vpnc_ifname, 0, "0.0.0.0", gateway, "128.0.0.0");
		error:
			_dprintf("%s: fail to add route table\n", __FUNCTION__);
			legacy_update_vpnc_state(prefix, WAN_STATE_STOPPED, WAN_STOPPED_REASON_IPGATEWAY_CONFLICT);
			return;
		}
	}

	/* Add interface dependent static routes */
	add_routes(prefix, "route", vpnc_ifname);

	/* Remove obsolete default route via VPN interface */
	route_del(vpnc_ifname, 0, "0.0.0.0", NULL, "0.0.0.0");

	/* Remove gateway route - no longer needed, avoid routing loops */
	route_del(vpnc_ifname, 0, gateway, NULL, "255.255.255.255");
#endif

	/* Add dns servers to resolv.conf */
	if (nvram_invmatch(strcat_r(prefix, "dns", tmp), ""))
		legacy_vpnc_update_resolvconf();

	/* Add firewall rules for VPN client */
	legacy_vpnc_add_firewall_rule();

	legacy_update_vpnc_state(prefix, WAN_STATE_CONNECTED, 0);	
}

int
legacy_vpnc_ipup_main(int argc, char **argv)
{
	FILE *fp;
	char *vpnc_ifname = safe_getenv("IFNAME");
	char *vpnc_linkname = safe_getenv("LINKNAME");
	char tmp[100], prefix[] = "vpnc_";
	char buf[256], *value;
	int unit;

	_dprintf("%s():: %s\n", __FUNCTION__, argv[0]);

	/* Get unit from LINKNAME: vpn[UNIT] */
	if ((unit = legacy_vpnc_ppp_linkunit(vpnc_linkname)) < 0)
		return 0;

	_dprintf("%s: unit=%d ifname=%s\n", __FUNCTION__, unit, vpnc_ifname);

	/* Touch connection file */
	if (!(fp = fopen(strcat_r("/tmp/ppp/link.", vpnc_ifname, tmp), "a"))) {
		perror(tmp);
		return errno;
	}
	fclose(fp);

	if ((value = getenv("IPLOCAL"))) {
		if (nvram_invmatch(strcat_r(prefix, "ipaddr", tmp), value))
			ifconfig(vpnc_ifname, IFUP, "0.0.0.0", NULL);
		_ifconfig(vpnc_ifname, IFUP, value, "255.255.255.255", getenv("IPREMOTE"), 0);
		nvram_set(strcat_r(prefix, "ipaddr", tmp), value);
		nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");
	}

	if ((value = getenv("IPREMOTE")))
		nvram_set(strcat_r(prefix, "gateway", tmp), value);

	strcpy(buf, "");
	if ((value = getenv("DNS1")))
		snprintf(buf, sizeof(buf), "%s", value);
	if ((value = getenv("DNS2")))
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%s%s", strlen(buf) ? " " : "", value);
	nvram_set(strcat_r(prefix, "dns", tmp), buf);

	legacy_vpnc_up(vpnc_ifname);

	_dprintf("%s:: done\n", __FUNCTION__);
	return 0;
}

void legacy_vpnc_del_firewall_rule()
{
	char tmp[100], prefix[] = "vpnc_", wan_prefix[] = "wanXXXXXXXXXX_";
	char *vpnc_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
	char *wan_proto = NULL;
	char lan_if[IFNAMSIZ+1];

	strcpy(lan_if, nvram_safe_get("lan_ifname"));

	snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());
	wan_proto = nvram_safe_get(strcat_r(wan_prefix, "proto", tmp));
	if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static"))
		//eval("iptables", "-D", "FORWARD", "-p", "tcp", "--syn", "-j", "TCPMSS", "--clamp-mss-to-pmtu");
		eval("iptables", "-D", "FORWARD", "-p", "tcp", "--tcp-flags", "SYN,RST", "SYN", "-j", "TCPMSS", "--clamp-mss-to-pmtu");
#ifdef RTCONFIG_BCMARM
	else
#ifdef HND_ROUTER
		if (nvram_match("fc_disable", "0") && nvram_match("fc_pt_war", "1"))
#else
		if (nvram_match("ctf_disable", "0"))
#endif
		eval("iptables", "-t", "mangle", "-D", "FORWARD", "-p", "tcp", 
			"-m", "state", "--state", "NEW","-j", "MARK", "--set-mark", "0x01/0x7");
#endif

	eval("iptables", "-D", "FORWARD", "-o", vpnc_ifname, "!", "-i", lan_if, "-j", "DROP");
	eval("iptables", "-t", "nat", "-D", "PREROUTING", "-d", 
		nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), "-j", "VSERVER");
	eval("iptables", "-t", "nat", "-D", "POSTROUTING", "-o", 
		vpnc_ifname, "!", "-s", nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), "-j", "MASQUERADE");
}

void
legacy_vpnc_down(char *vpnc_ifname)
{
#ifdef VPNC_LEGACY
	char tmp[100], prefix[] = "vpnc_", wan_prefix[] = "wanXXXXXXXXXX_";
	char *wan_ifname = NULL, *wan_proto = NULL;

	snprintf(wan_prefix, sizeof(wan_prefix), "wan%d_", wan_primary_ifunit());
	wan_proto = nvram_safe_get(strcat_r(wan_prefix, "proto", tmp));

	if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static"))
		wan_ifname = nvram_safe_get(strcat_r(wan_prefix, "ifname", tmp));
	else
		wan_ifname = nvram_safe_get(strcat_r(wan_prefix, "pppoe_ifname", tmp));

#if !defined(CONFIG_BCMWL5) && defined(RTCONFIG_DUALWAN)
	if (get_nr_wan_unit() > 1 && nvram_match("wans_mode", "lb")) {
		/* Reset default gateway route */
		if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static")) {
			route_del(wan_ifname, 2, "0.0.0.0", nvram_pf_safe_get(wan_prefix, "gateway"), "0.0.0.0");
		}
		else if (!strcmp(wan_proto, "pppoe") || !strcmp(wan_proto, "pptp") || !strcmp(wan_proto, "l2tp"))
		{
			char *wan_xgateway = nvram_pf_safe_get(wan_prefix, "xgateway");

			route_del(wan_ifname, 2, "0.0.0.0", nvram_pf_safe_get(wan_prefix, "gateway"), "0.0.0.0");
			if (strlen(wan_xgateway) > 0 && strcmp(wan_xgateway, "0.0.0.0")) {
				char *wan_xifname = nvram_pf_safe_get(wan_prefix, "ifname");

				route_del(wan_xifname, 3, "0.0.0.0", nvram_pf_safe_get(wan_prefix, "xgateway"), "0.0.0.0");
			}

			/* Delete route to pptp/l2tp's server */
			if (nvram_pf_get_int(prefix, "dut_disc") && strcmp(wan_proto, "pppoe"))
				route_del(wan_ifname, 0, nvram_pf_safe_get(wan_prefix, "gateway"), "0.0.0.0", "255.255.255.255");
		}

		/* default route via default gateway */
		add_multi_routes();
	} else {
#endif
		/* Reset default gateway route */
		if (!strcmp(wan_proto, "dhcp") || !strcmp(wan_proto, "static")) {
			route_del(wan_ifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
			route_add(wan_ifname, 0, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
		}
		else if (!strcmp(wan_proto, "pppoe") || !strcmp(wan_proto, "pptp") || !strcmp(wan_proto, "l2tp"))
		{
			char *wan_xgateway = nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp));

			route_del(wan_ifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");
			route_add(wan_ifname, 0, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0");

			if (strlen(wan_xgateway) > 0 && strcmp(wan_xgateway, "0.0.0.0")) {
				char *wan_xifname = nvram_safe_get(strcat_r(wan_prefix, "ifname", tmp));

				route_del(wan_xifname, 3, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp)), "0.0.0.0");
				route_add(wan_xifname, 2, "0.0.0.0", nvram_safe_get(strcat_r(wan_prefix, "xgateway", tmp)), "0.0.0.0");
			}

			/* Delete route to pptp/l2tp's server */
			if (nvram_get_int(strcat_r(prefix, "dut_disc", tmp)) && strcmp(wan_proto, "pppoe"))
				route_del(wan_ifname, 0, nvram_safe_get(strcat_r(wan_prefix, "gateway", tmp)), "0.0.0.0", "255.255.255.255");
		}
#if !defined(CONFIG_BCMWL5) && defined(RTCONFIG_DUALWAN)
	}
#endif
#endif /* VPNC_LEGACY */

	/* Delete firewall rules for VPN client */
	legacy_vpnc_del_firewall_rule();
}

/*
 * Called when link goes down
 */
int
legacy_vpnc_ipdown_main(int argc, char **argv)
{
	char *vpnc_ifname = safe_getenv("IFNAME");
	char *vpnc_linkname = safe_getenv("LINKNAME");
	char tmp[100], prefix[] = "vpnc_";
	int unit;

	_dprintf("%s():: %s\n", __FUNCTION__, argv[0]);

	/* Get unit from LINKNAME: vpn[UNIT] */
	if ((unit = legacy_vpnc_ppp_linkunit(vpnc_linkname)) < 0)
		return 0;

	_dprintf("%s: unit=%d ifname=%s\n", __FUNCTION__, unit, vpnc_ifname);

	/* override wan_state to get real reason */
	legacy_update_vpnc_state(prefix, WAN_STATE_STOPPED, legacy_vpnc_pppstatus());

	legacy_vpnc_down(vpnc_ifname);

	/* Add dns servers to resolv.conf */
	update_resolvconf();

	unlink(strcat_r("/tmp/ppp/link.", vpnc_ifname, tmp));

	_dprintf("%s:: done\n", __FUNCTION__);
	return 0;
}

/*
 * Called when interface comes up
 */
int
legacy_vpnc_ippreup_main(int argc, char **argv)
{
	char *vpnc_ifname = safe_getenv("IFNAME");
	char *vpnc_linkname = safe_getenv("LINKNAME");
	char tmp[100], prefix[] = "vpnc_";
	int unit;

	_dprintf("%s():: %s\n", __FUNCTION__, argv[0]);

	/* Get unit from LINKNAME: vpn[UNIT] */
	if ((unit = legacy_vpnc_ppp_linkunit(vpnc_linkname)) < 0)
		return 0;

	_dprintf("%s: unit=%d ifname=%s\n", __FUNCTION__, unit, vpnc_ifname);

	/* Set vpnc_pppoe_ifname to real interface name */
	nvram_set(strcat_r(prefix, "pppoe_ifname", tmp), vpnc_ifname);

	_dprintf("%s:: done\n", __FUNCTION__);
	return 0;
}

/*
 * Called when link closing with auth fail
 */
int
legacy_vpnc_authfail_main(int argc, char **argv)
{
	char *vpnc_ifname = safe_getenv("IFNAME");
	char *vpnc_linkname = safe_getenv("LINKNAME");
	char prefix[] = "vpnc_";
	int unit;

	_dprintf("%s():: %s\n", __FUNCTION__, argv[0]);

	/* Get unit from LINKNAME: ppp[UNIT] */
	if ((unit = legacy_vpnc_ppp_linkunit(vpnc_linkname)) < 0)
		return 0;

	_dprintf("%s: unit=%d ifname=%s\n", __FUNCTION__, unit, vpnc_ifname);

	/* override vpnc_state */
	legacy_update_vpnc_state(prefix, WAN_STATE_STOPPED, WAN_STOPPED_REASON_PPP_AUTH_FAIL);

	_dprintf("%s:: done\n", __FUNCTION__);
	return 0;
}

int legacy_is_vpnc_dns_active()
{
	if (nvram_get_int("vpnc_state_t") == WAN_STATE_CONNECTED &&
	    nvram_invmatch("vpnc_dns", ""))
		return 1;

	return 0;
}
