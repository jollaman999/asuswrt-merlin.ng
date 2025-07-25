/* For iOS Push Notification Title */

struct pnsInfo
{
	int      event;
	char    *title;
	char    *iftttMsg;
};
	
struct pnsInfo mapPushInfo[] =
{
	/* RESERVATION EVENT */
	/* ------------------------------
	   ### System ###
	---------------------------------*/
	{SYS_WAN_DISCONN_EVENT                       ,"Unable to connect to the Internet"                       ,"" },
	{SYS_WAN_BLOCK_EVENT                         ,"WAN Connection is paused"                                ,"" },
	{SYS_NEW_DEVICE_WIFI_CONNECTED_EVENT         ,"New Device Connected(wifi)"                              ,"" },
	{SYS_NEW_DEVICE_ETH_CONNECTED_EVENT          ,"New Device Connected(eth)"                               ,"" },
	{SYS_EXISTED_DEVICE_WIFI_CONNECTED_EVENT     ,"Manage Your Wireless Client"                             ,"" },
	{SYS_WAN_CABLE_UNPLUGGED_EVENT               ,"The network cable is unplugged"                          ,"" },
	{SYS_WAN_PPPOE_AUTH_FAILURE_EVENT            ,"PPPoE Authentication Failure"                            ,"" },
	{SYS_WAN_USB_MODEM_UNREADY_EVENT             ,"USB modem is not ready."                                 ,"" },
	{SYS_WAN_IP_CONFLICT_EVENT                   ,"IP conflict detected"                                    ,"" },
	{SYS_WAN_UNABLE_CONNECT_PARENT_AP_EVENT      ,"Unable to connect to Parent AP"                          ,"" },
	{SYS_WAN_MODEM_OFFLINE_EVENT                 ,"The modem appears to be offline."                        ,"" },
	{SYS_WAN_GOT_PROBLEMS_FROM_ISP_EVENT         ,"Your ISP appears to have problems."                      ,"" },
	{SYS_WAN_UNPUBLIC_IP_EVENT                   ,"External IP service is unavailable."                     ,"" },
	{SYS_ALL_WIFI_TURN_OFF_EVENT                 ,"All your WiFi is turned off"                             ,"" },
	{SYS_SCY_UNCONN_NOTICE_EVENT                 ,"Security Notice"                                         ,"" },
	{SYS_SCY_CONN_NOTICE_EVENT                   ,"Security Notice"                                         ,"" },
	{SYS_FW_UPGRADE_OK_EVENT                     ,"Firmware upgrade successed"                              ,"" },
	{SYS_FW_UPGRADE_NOK_EVENT                    ,"Firmware upgrade failed"                                 ,"" },
	{SYS_REBOOT_OK_EVENT                         ,"Device reboot successed"                                 ,"" },
	{SYS_REBOOT_NOK_EVENT                        ,"Device reboot failed"                                    ,"" },
	{SYS_LAN_PORT_IN_EVENT                       ,"LAN port plug-in"                                        ,"" },
	{SYS_LAN_PORT_OUT_EVENT                      ,"LAN port plug-out"                                       ,"" },
	{SYS_AFC_COLD_REBOOT_SILENT_EVENT            ,"AFC Cold reboot in silent"                               ,"" },
	{SYS_AFC_COLD_REBOOT_EVENT                   ,"AFC Cold reboot"                                         ,"" },
	{SYS_FW_NEW_VERSION_AVAILABLE_EVENT          ,"New Firmware Available"                                  ,"" },
	/* ------------------------------
	   ### Administration ###
	---------------------------------*/
	{ADMIN_REMOTE_LOGIN_EVENT                    ,"Login successed"                                         ,"" },
	{ADMIN_BOUND_DEV_EVENT                       ,"Device Bound"                                            ,"" },
	{ADMIN_UNBOUND_DEV_EVENT                     ,"Device Unbound"                                          ,"" },
	{ADMIN_LOGIN_FAIL_SSH_EVENT                  ,"Unusual Router Sign-In"                                  ,"" },
	{ADMIN_LOGIN_FAIL_TELNET_EVENT               ,"Unusual Router Sign-In"                                  ,"" },
	{ADMIN_LOGIN_FAIL_LAN_WEB_EVENT              ,"Unusual Router Sign-In"                                  ,"" },
	/* ------------------------------
	   ### Security ###
	---------------------------------*/
	{PROTECTION_VULNERABILITY_EVENT              ,"Intrusion Prevention System Alert"                       ,"" },
	{PROTECTION_CC_EVENT                         ,"Infected Device Detected and Blocked"                    ,"" },
	{PROTECTION_MALICIOUS_SITE_EVENT             ,"Malicious Site Access Blocked"                           ,"" },
	{PROTECTION_INFECTED_DEVICE_EVENT            ,"Infected Device Detected and Blocked"                    ,"" },
	/* ------------------------------
	   ### USB Function ###
	---------------------------------*/
	{USB_TETHERING_EVENT                         ,"Auto USB Backup WAN"                                     ,"" },
	/* ------------------------------
	    ### General Event  ###
	---------------------------------*/
	{GENERAL_WIFI_DEV_ONLINE                     ,"Manage Your Wireless Client"                             ,"" },
	{GENERAL_WIFI_DEV_OFFLINE                    ,"Wireless Device Disconnectd"                             ,"" },
	{GENERAL_ETH_DEV_ONLINE                      ,"Manage Your Ethernet Client"                             ,"" },
	{GENERAL_ETH_DEV_OFFLINE                     ,"Ethernet Device Disconnected"                            ,"" },
	{GENERAL_ETH_DEV_REFUSED                     ,""                                                        ,"" },
	{GENERAL_SYS_STATES                          ,""                                                        ,"" },
	{GENERAL_DEV_UPDATE                          ,"DeviceAddOrUpdate"                                       ,"" },
	{GENERAL_DEV_DELETED                         ,"DeviceDelete"                                            ,"" },
	{GENERAL_DEV_ACCESS_CHANGE                   ,"DeviceNetworkAccessChange"                               ,"" },
	{GENERAL_QOS_UPDATE                          ,"QosModeUpdate"                                           ,"" },
	{GENERAL_TOGGLE_STATES_UPDATE                ,"ToggleStateUpdate"                                       ,"" },
	/* ------------------------------
	    ### AiMesh Event  ###
	---------------------------------*/
	{AIMESH_ETH_OB_EVENT                         ,"AiMesh Node Ethenet Auto Setup"                          ,"" },
	/* The End */
	{0,0,0,0}
};
