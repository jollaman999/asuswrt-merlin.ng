﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scale=0">
	<meta http-equiv="X-UA-Compatible" content="ie=edge">
	<link rel="shortcut icon" href="images/favicon.png">
	<title><#Web_Title#> - <#menu5_1_6#></title>
	<link rel="stylesheet" href="index_style.css"> 
	<link rel="stylesheet" href="form_style.css">
	<link rel="stylesheet" href="css/confirm_block.css" />
	<script src="/js/jquery.js"></script>
	<script src="/js/httpApi.js"></script>
	<script src="/calendar/jquery-ui.js"></script>
	<script src="/state.js"></script>
	<script src="/general.js"></script>
	<script src="/help.js"></script>
	<script src="/popup.js"></script>
	<script src="/validator.js"></script>
	<script language="JavaScript" type="text/javascript" src="/js/weekSchedule/weekSchedule.js"></script>
	<script language="JavaScript" type="text/javascript" src="/form.js"></script>
<style>
.ui-slider {
	position: relative;
	text-align: left;
}
.ui-slider .ui-slider-handle {
	position: absolute;
	width: 12px;
	height: 12px;
}
.ui-slider .ui-slider-range {
	position: absolute;
}
.ui-slider-horizontal {
	height: 6px;
}

.ui-widget-content {
	border: 2px solid #000;
	background-color:#000;
}
.ui-state-default,
.ui-widget-content .ui-state-default,
.ui-widget-header .ui-state-default {
	border: 1px solid ;
	background: #e6e6e6;
	margin-top:-4px;
	margin-left:-6px;
}

/* Corner radius */
.ui-corner-all,
.ui-corner-top,
.ui-corner-left,
.ui-corner-tl {
	border-top-left-radius: 4px;
}
.ui-corner-all,
.ui-corner-top,
.ui-corner-right,
.ui-corner-tr {
	border-top-right-radius: 4px;
}
.ui-corner-all,
.ui-corner-bottom,
.ui-corner-left,
.ui-corner-bl {
	border-bottom-left-radius: 4px;
}
.ui-corner-all,
.ui-corner-bottom,
.ui-corner-right,
.ui-corner-br {
	border-bottom-right-radius: 4px;
}

.ui-slider-horizontal .ui-slider-range {
	top: 0;
	height: 100%;
}

#slider .ui-slider-range {
	background: #93E7FF; 
	border-top-left-radius: 3px;
	border-top-right-radius: 1px;
	border-bottom-left-radius: 3px;
	border-bottom-right-radius: 1px;
}
#slider .ui-slider-handle { border-color: #93E7FF; }
.parental_th{
	color:white;
	background:#2F3A3E;
	cursor: pointer;
	width:160px;
	height:22px;
	border-bottom:solid 1px black;
	border-right:solid 1px black;
}
.parental_th:hover{
	background:rgb(94, 116, 124);
	cursor: pointer;
}

.checked{
	background-color:#9CB2BA;
	width:82px;
	border-bottom:solid 1px black;
	border-right:solid 1px black;
}

.disabled{
	width:82px;
	border-bottom:solid 1px black;
	border-right:solid 1px black;
	background-color:#475A5F;
}

  #selectable .ui-selecting { background: #FECA40; }
  #selectable .ui-selected { background: #F39814; color: white; }
  #selectable .ui-unselected { background: gray; color: green; }
  #selectable .ui-unselecting { background: green; color: black; }
  #selectable { border-spacing:0px; margin-left:0px;margin-top:0px; padding: 0px; width:100%;}
  #selectable td { height: 22px; }

</style>
<script>
$(function () {
	if(amesh_support && ((isSwMode("RT") || isSwMode("WISP")) || isSwMode("ap")) && ameshRouter_support) {
		addNewScript('/require/modules/amesh.js');
	}
});
//Get boot loader version and convert type form string to Integer
var bl_version = '<% nvram_get("bl_version"); %>';
var bl_version_array = bl_version.split(".");
var bootLoader_ver = "";
for(i=0;i<bl_version_array.length;i++){
	bootLoader_ver +=bl_version_array[i];
}

<% wl_get_parameter(); %>
var mcast_rates = [
	["HTMIX 6.5/15", "14", 0, 1, 1],	// MTK HTMIX
	["HTMIX 13/30",	 "15", 0, 1, 1],
	["HTMIX 19.5/45","16", 0, 1, 1],
	["HTMIX 26/60",	 "18", 0, 1, 2],
	["HTMIX 130/144","13", 0, 1, 2],
	["HTMIX 6.5",    "14", 0, 2, 1],	// QCA HTMIX
	["HTMIX 13",	 "15", 0, 2, 1],
	["HTMIX 19.5",   "16", 0, 2, 1],
	["HTMIX 26",	 "18", 0, 2, 2],
	["HTMIX 130",    "13", 0, 2, 2],
	["OFDM 6",	 "4",  0, 0, 1],
	["OFDM 9",	 "5",  0, 0, 1],
	["OFDM 12",	 "7",  0, 0, 1],
	["OFDM 18",	 "8",  0, 0, 1],
	["OFDM 24",	 "9",  0, 0, 1],
	["OFDM 36",	 "10", 0, 0, 1],
	["OFDM 48",	 "11", 0, 0, 1],
	["OFDM 54",	 "12", 0, 0, 1],
	["CCK 1",	 "1",  1, 0, 1],
	["CCK 2",	 "2",  1, 0, 1],
	["CCK 5.5",	 "3",  1, 0, 1],
	["CCK 11",	 "6",  1, 0, 1]
];

var tcode = ttc.substring(0,2);
var sdk_6 = sdk_version_array[0] == "6" ? true:false
var sdk_7 = sdk_version_array[0] == "7" ? true:false
var wl_user_rssi_onload = '<% nvram_get("wl_user_rssi"); %>';
var reboot_needed_time = eval("<% get_default_reboot_time(); %>");
var orig_region = '<% nvram_get("ui_location_code"); %>';
var wl_txpower_orig = '<% nvram_get("wl_txpower"); %>';
var machine_name = '<% get_machine_name(); %>';
var machine_arm = (machine_name.search("arm") == -1 && machine_name.search("aarch64") == -1) ? false : true;
var array;
var clock_type = "";
var wifi_schedule_value = '<% nvram_get("wl_sched"); %>'.replace(/&#62/g, ">").replace(/&#60/g, "<");
var wl_unit_value = '<% nvram_get("wl_unit"); %>';
var wl_bw_160_value = '<% nvram_get("wl_bw_160"); %>';
var country_array = [<% get_support_region_list(); %>][0];
var EG_mode = ('<% nvram_get("EG_mode"); %>' == 1);

var current_page = window.location.pathname.split("/").pop();
var faq_index_tmp = get_faq_index(FAQ_List, current_page, 1);

if(country_array == undefined)
	country_array = [];

var country_selection_list = [["AA", "<#country_AA#>"], ["CN", "<#country_CN#>"], ["SG", "<#country_SG#>"], ["EU", "<#country_EU#>"], ["KR", "<#country_KR#>"], ["RU", "<#country_RU#>"], ["US", "<#country_US#>"], ["AU", "<#country_AU#>"], ["XX", "<#country_AU#>"]];

var U5_country_code = false;
if(
	tcode == "CN" ||
	tcode == "CT" ||
	tcode == "TC" ||
	tcode == "GD" ||
	tcode == "AA" ||
	tcode == "AP" ||
	tcode == "AQ" ||
	tcode == "HK" ||
	tcode == "SG"
){
	country_selection_list[6][0] = "U5";
	country_array = country_array.join().replace("US", "U5").split(",")
	U5_country_code = true;
}

var country_selection_array = new Array();
var _AU1_support = false;
var _AU2_support = false;

if(tcode == "GD"){
	country_array = country_array.join("-").replace("-CN", "").split("-")
	country_selection_list.push(["GD", "<#country_CN#>"]);
}

if(EG_mode){
	location_list_support = true;
	country_array = ["EU"];
	country_selection_list[3][1] = "Egypt";
}

// for RT-AC58U CX/01
if(country_array.indexOf("CX") != -1 && (ttc == "CX/01" || ttc == "CX/05")){
	country_selection_list[7][0] = "CX";
	country_selection_list[7][1] = "New Zealand";
	country_selection_list.splice(8,1);
}
// for RT-AC58U SP/01
if(country_array.indexOf("SP") != -1){
	country_selection_list[7][0] = "SP";
	country_selection_list.splice(8,1);
}

if(country_array.indexOf("NZ") != -1){
	country_selection_list[7][1] = "New Zealand";
	country_selection_list.splice(8,1);

	if(in_territory_code("CX")){
		country_array[country_array.indexOf("NZ")] = "CX";
		country_selection_list[7][0] = "CX";
	}
}

if(country_array.indexOf("AU") != -1){
	_AU1_index = country_array.indexOf("AU");
	_AU1_support = true;
}

if(country_array.indexOf("XX") != -1){
	_AU2_support = true;
}

if(_AU1_support && _AU2_support){
	country_array.splice(_AU1_index, 1);
}

for(i=0;i<country_selection_list.length;i++){
	var index = country_selection_list[i][0];
	country_selection_array.push(index);
	country_selection_array[index] = {
		code: index,
		name: country_selection_list[i][1]
	}
}
var cfg_ui_region_disable = 0;
if(location_list_support && amesh_support && ameshRouter_support) {
	cfg_ui_region_disable = parseInt('<% nvram_get("cfg_ui_region_disable"); %>');
	if(isNaN(cfg_ui_region_disable))
		cfg_ui_region_disable = 0;
}

var QAM256_support = false;
var QAM1024_support = false;
var QAM256_2G_support = isSupport('qam256_2g');	// RTCONFIG_QAM256_2G
var QAM1024_5G_support = isSupport('qam1024_5g'); // RTCONFIG_QAM1024_5G
var MUMIMO2G_support = isSupport('mumimo_2g'); // RTCONFIG_MUMIMO_2G
var MUMIMO5G_support = isSupport('mumimo_5g'); // RTCONFIG_MUMIMO_5G
var wifi7_ofdma_support = wifi7_support;
var wifi7_mumimo_support = isSupport('hide_mumimo') ? false : wifi7_support;
(function(){
	if(Bcmwifi_support){
		var _cap = '';
		var _chipset = '';
		if(is_unit_24g(wl_unit_value)){
			_cap = '<% wl_cap_2g(); %>';
			_chipset = '<% wl_chipnum_2g(); %>';
		}
		else if(is_unit_5g(wl_unit_value)){
			_cap = '<% wl_cap_5g(); %>';
			_chipset = '<% wl_chipnum_5g(); %>';
		}
		else if(is_unit_5g_2(wl_unit_value)){
			_cap = '<% wl_cap_5g_2(); %>';
			_chipset = '<% wl_chipnum_5g_2(); %>';
		}

		QAM1024_support = (function(){
			if(isNaN(_chipset)){	// check the value is hexdecimal and then convert it to decimal
				_chipset = parseInt(_chipset, 16);
			}

			if(_cap.indexOf('11ax') != -1 || _chipset == '4366' || _chipset == '4365' || _chipset == '43664' || _chipset == '43666'){
				return true
			}

			return false;
		})();

		QAM256_support = (_cap.indexOf('vht-prop-rates') != -1) ? true : false;
		wifi7_ofdma_support = (_cap.indexOf('11be') != -1) ? true : false;
		wifi7_mumimo_support = (_cap.indexOf('11be') != -1) ? true : false;
	}
	else{
		if(based_modelid == "GT-AXY16000"
		|| based_modelid == "RT-AX89U" || based_modelid == "PL-AX56_XP4"
		|| based_modelid == "RT-AC95U" || based_modelid == "RT-AC88N" || based_modelid == "RT-AC88Q" || based_modelid == "RT-AC82U" || based_modelid == "RT-AC58U"
		|| based_modelid == "MAP-AC1300" || based_modelid == "MAP-AC2200" || based_modelid == "VZW-AC1300" || based_modelid == "MAP-AC1750" || based_modelid == "MAP-AC3000" 
		|| based_modelid == "RT-AD7200"
		|| based_modelid == "BRT-AC828"
		|| based_modelid == "RT-ACRH18"
		|| based_modelid == "4G-AX56" || based_modelid == "RT-AX53U" || based_modelid == "RT-AX54" || based_modelid == "XD4S"){
			QAM256_support = true;
		}
	}
})();

function backForwardCompatibility(){
	var locationCode = httpApi.nvramGet(["ui_location_code", "location_code"]);
	if(locationCode.ui_location_code == "" && locationCode.location_code != ""){
		httpApi.nvramSet({
			"ui_location_code": (U5_country_code && locationCode.location_code == "US") ? "U5" : locationCode.location_code,
            "action_mode": "apply",
			"rc_service": "saveNvram"
		}, function(){
			var result = httpApi.nvramGet(["ui_location_code"], true);
			if(result.ui_location_code !== "") refreshpage()
		})
	}
}

function initial(){
	show_menu();
	register_event();
	array = new Array(7);
	init_array(array);
	init_cookie();
	count_time();
	backForwardCompatibility();

	if(lantiq_support){
		checkWLReady();
	}

	if("<% get_parameter("af"); %>" == "wl_timesched"){
		autoFocus("wl_timesched");
	}
	if("<% get_parameter("af"); %>" == "wl_radio"){
		autoFocus("wl_radio");
	}

	if(userRSSI_support)
		changeRSSI(wl_user_rssi_onload);
	else{
		document.getElementById("rssiTr").style.display = "none";
		$("#wl_unit_field").toggle(!(isSwMode("RP") || isSwMode("ew")))
	}

	if(!band5g_support)
		document.getElementById("wl_unit_field").style.display = "none";

	regen_band(document.form.wl_unit);
	if(Rawifi_support){
		inputCtrl(document.form.wl_noisemitigation, 0);
	}
	else if(Qcawifi_support){
		// FIXME
		document.getElementById("DLSCapable").style.display = "none";	
		document.getElementById("PktAggregate").style.display = "none";
		inputCtrl(document.form.wl_noisemitigation, 0);
	}else{
		// BRCM
		document.getElementById("DLSCapable").style.display = "none";	
		document.getElementById("PktAggregate").style.display = "none";
		
		if(is_unit_5g(wl_unit_value) || machine_arm){	// MODELDEP: for Broadcom SDK 6.x model
			inputCtrl(document.form.wl_noisemitigation, 0);
		}
	}

	if(wifi_hw_sw_support){ //For N55U
		if(document.form.wl_HW_switch.value == "1"){
			document.form.wl_radio[0].disabled = true;
		}
	}
	
	// MODELDEP: for AC ser
	if(Rawifi_support){
		inputCtrl(document.form.wl_ampdu_mpdu, 0);
		inputCtrl(document.form.wl_ack_ratio, 0);
	}else if(Qcawifi_support){
		inputCtrl(document.form.wl_ampdu_mpdu, 0);
		document.getElementById("ampdu_rts_tr").style.display = "none";
		inputCtrl(document.form.wl_ack_ratio, 0);
	}else{
		if (sdk_6){
			// for BRCM new SDK 6.x
			inputCtrl(document.form.wl_ampdu_mpdu, 1);
			inputCtrl(document.form.wl_ack_ratio, 1);
		}else if(sdk_7 || sdk_9 || sdk_version_array[0] == '17'){ // for BRCM new SDK 7.x/9.x/17.x
			inputCtrl(document.form.wl_ampdu_mpdu, 1);
			inputCtrl(document.form.wl_ack_ratio, 0);
		}else{
			inputCtrl(document.form.wl_ampdu_mpdu, 0);
			inputCtrl(document.form.wl_ack_ratio, 0);
		}
	}
	
	inputCtrl(document.form.wl_turbo_qam, 0);
	inputCtrl(document.form.wl_turbo_qam_brcm_intop, 0);
	inputCtrl(document.form.traffic_5g, 0);

	if(is_unit_5g(wl_unit_value) || is_unit_5g_2(wl_unit_value) || is_unit_6g(wl_unit_value)){ // 5GHz up
		if(no_vht_support){
			inputCtrl(document.form.wl_txbf, 0);
			inputCtrl(document.form.wl_itxbf, 0);
		}
		else
		{
			if(band5g_11ax_support){
				if(is_unit_5g(wl_unit_value) && based_modelid == 'RT-AX92U'){
					document.getElementById('wl_txbf_desc').innerHTML = "<#WLANConfig11b_x_acBeam#>";
				}
				else{
					document.getElementById('wl_txbf_desc').innerHTML = "<#WLANConfig11b_x_axBeam#>";
				}
			}
			else
			{
				document.getElementById('wl_txbf_desc').innerHTML = "<#WLANConfig11b_x_acBeam#>";
			}

			inputCtrl(document.form.wl_txbf, 1);
			inputCtrl(document.form.wl_itxbf, 1);
		}

		if(based_modelid == "RT-AC88N" || based_modelid == "RT-AC88Q" 
		|| based_modelid == "BRT-AC828" || based_modelid == "RT-AD7200" 
		|| based_modelid == "RT-AC58U" || based_modelid.substring(0,7) == "RT-AC59" || based_modelid == "RT-AC82U" 
		|| based_modelid == "MAP-AC1300" || based_modelid == "MAP-AC2200" 
		|| based_modelid == "VZW-AC1300"
		|| based_modelid == "RT-AC95U" || based_modelid == "GT-AXY16000" || based_modelid == "RT-AX89U" || based_modelid == "PL-AX56_XP4")
		{
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}		
		if( based_modelid == "RT-AC55U" || based_modelid == "RT-AC55UHP")
			inputCtrl(document.form.traffic_5g, 1);

		if(QAM1024_support || (Bcmwifi_support && QAM256_support)){
			if(document.form.wl_nmode_x.value == "0" || document.form.wl_nmode_x.value == "8"){		// wireless mode: Auto , N/AC mixed
				inputCtrl(document.form.wl_turbo_qam, 1);
			}
			else{		// wireless mode: N only, Legacy
				inputCtrl(document.form.wl_turbo_qam, 0);
			}

			if(no_vht_support){	//Hide 11AC/80MHz from GUI
				inputCtrl(document.form.wl_turbo_qam, 0);
			}
			
			$("#turbo_qam_title").html("<#WLANConfig11b_x_ModulationScheme#> (WiFi 5)");
			if(QAM1024_support){
				var desc = ["Up to MCS 9 (802.11ac)", "Up to MCS 11 (NitroQAM/1024-QAM)"];
				var value = ["1", "2"];
			}
			else if(QAM256_support){
				var desc = ["Up to MCS 9 (802.11ac)"];
				var value = ["1"];
			}
			
			add_options_x2(document.form.wl_turbo_qam, desc, value, '<% nvram_get("wl_turbo_qam"); %>');
			$('#turbo_qam_hint').click(function(){openHint(3,33);});
		}

		if(based_modelid == "RT-AX53U" || based_modelid == "RT-AX54" || based_modelid == "XD4S"){
			inputCtrl(document.form.wl_turbo_qam, 1);
		}
		if ((Qcawifi_support || Rawifi_support) && QAM1024_5G_support) {
			inputCtrl(document.form.wl_turbo_qam, 1);
			$("#turbo_qam_title").html("1024-QAM");
		}

		if((!Qcawifi_support && !Rawifi_support) || based_modelid == "RT-AC87U"
		    || based_modelid == "MAP-AC1300" || based_modelid == "MAP-AC2200" || based_modelid == "VZW-AC1300" || based_modelid == "RT-AC95U"
		    || based_modelid == "RT-AC82U" || based_modelid == "RT-AC58U" || based_modelid == "4G-AC53U" || based_modelid == "4G-AC56" || (based_modelid == "RP-AC87" && is_unit_5g(wl_unit_value)) ){		// hide on Broadcom platform
			document.getElementById("wl_plcphdr_field").style.display = "none";
		}

		if (MUMIMO5G_support) {
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}
	}
	else if(is_unit_60g(wl_unit_value)){ // 60GHz up
		inputCtrl(document.form.wl_user_rssi_option, 0);
		inputCtrl(document.form.wl_igs, 0);
		inputCtrl(document.form.wl_mrate_x, 0);
		inputCtrl(document.form.wl_plcphdr, 0);
		inputCtrl(document.form.wl_rts, 0);
		inputCtrl(document.form.wl_dtim, 0);
		inputCtrl(document.form.wl_frameburst, 0);
		inputCtrl(document.form.wl_wme_apsd, 0);
		inputCtrl(document.form.wl_atf, 0);
		inputCtrl(document.form.wl_txbf, 0);
		document.getElementById("wl_itxbf_field").style.display = "none";
		document.getElementById("wl_txPower_field").style.display = "none";
	}
	else{ // 2.4GHz
		if((Qcawifi_support || Rawifi_support) && (QAM256_support || QAM256_2G_support)){
			inputCtrl(document.form.wl_turbo_qam, 1);
			$("#turbo_qam_title").html("256-QAM");
		}
		else if(QAM1024_support || (Bcmwifi_support && QAM256_support)){
			if(based_modelid == "RT-N18U" && bootLoader_ver < 2000)
				inputCtrl(document.form.wl_turbo_qam, 0);
			else{
				if(document.form.wl_nmode_x.value == "0" || document.form.wl_nmode_x.value == "1"){		// wireless mode: Auto, N only
					inputCtrl(document.form.wl_turbo_qam, 1);
				}
				else{		// wireless mode: Legacy
					inputCtrl(document.form.wl_turbo_qam, 0);
				}
				
				$("#turbo_qam_title").html("<#WLANConfig11b_x_ModulationScheme#> (WiFi 5)");
				if(QAM1024_support){
					var desc = ["Up to MCS 7 (802.11n)", "Up to MCS 9 (TurboQAM/256-QAM)", "Up to MCS 11 (NitroQAM/1024-QAM)"];
					var value = ["0", "1", "2"];
				}
				else if(QAM256_support){
					var desc = ["Up to MCS 7 (802.11n)", "Up to MCS 9 (TurboQAM/256-QAM)"];
					var value = ["0", "1"];
				}
				
				add_options_x2(document.form.wl_turbo_qam, desc, value, '<% nvram_get("wl_turbo_qam"); %>');
				$('#turbo_qam_hint').click(function(){openHint(3,33);});				
			}
				
			if(no_vht_support){	//Hide 11AC/80MHz from GUI
					inputCtrl(document.form.wl_turbo_qam, 0);
					inputCtrl(document.form.wl_txbf, 0);
					inputCtrl(document.form.wl_itxbf, 0);
			}
			else{	
				document.getElementById('wl_txbf_desc').innerHTML = "<#WLANConfig11b_x_ExpBeam#>";
				inputCtrl(document.form.wl_txbf, 1);
				inputCtrl(document.form.wl_itxbf, 1);
			}	
		}

		if(Qcawifi_support && (based_modelid == "GT-AXY16000" || based_modelid == "RT-AX89U" || based_modelid == "PL-AX56_XP4" ) && document.form.wl_nmode_x.value == "0" && document.form.wl0_11ax.value == "1"){
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}
		if(Qcawifi_support && (based_modelid == "RT-AC95U")){
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}

		if(MUMIMO2G_support){
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}
	}

	var mcast_rate = '<% nvram_get("wl_mrate_x"); %>';
	var mcast_unit = wl_unit_value;
	var HtTxStream;
	if (mcast_unit == 1)
		HtTxStream = '<% nvram_get("wl1_HT_TxStream"); %>';
	else
		HtTxStream = '<% nvram_get("wl0_HT_TxStream"); %>';
	for (var i = 0; i < mcast_rates.length; i++) {
		if ((mcast_unit == '1' || mcast_unit == '2') && mcast_rates[i][2]) // 5Ghz && CCK
			continue;
		if (!Rawifi_support && !Qcawifi_support && mcast_rates[i][3]) // BCM && HTMIX
			continue;
		if (Qcawifi_support){
			if (mcast_rates[i][3] == 1) // QCA, skip MTK's HTMIX
				continue;
		}else{
			if (mcast_rates[i][3] == 2) // another platform, skip QCA's HTMIX
				continue;
		}
		if ((Rawifi_support || Qcawifi_support) && document.form.wl_nmode_x.value == "2" && mcast_rates[i][3])
			continue;
		if (Rawifi_support && HtTxStream < mcast_rates[i][4]) // ralink && HtTxStream
			continue;
		add_option(document.form.wl_mrate_x, mcast_rates[i][0], mcast_rates[i][1], (mcast_rate == mcast_rates[i][1]) ? 1 : 0);
	}

	if(repeater_support || psta_support){		//with RE mode
		document.getElementById("DLSCapable").style.display = "none";	
	}	


	if(document.form.wl_nmode_x.value == "2"){    //Legacy
		var wme_array = ["<#Auto#>", "<#WLANConfig11b_WirelessCtrl_button1name#>", "<#WLANConfig11b_WirelessCtrl_buttonname#>"];
		var wme_value = ["auto", "on", "off"];
		inputCtrl(document.form.wl_frag, 1);
		add_options_x2(document.form.wl_wme, wme_array, wme_value, document.form.wl_wme.value);
		if(document.form.wl_wme.value == "off"){
			inputCtrl(document.form.wl_wme_no_ack, 0);
			inputCtrl(document.form.wl_wme_apsd, 0);
		}

		document.form.wl_txbf.value = 0;
		document.form.wl_mumimo.value = 0;
		document.getElementById("wl_MU_MIMO_field").style.display = "none";
		inputCtrl(document.form.wl_txbf, 0);
	}
	else{  
		var wme_array = ["<#WLANConfig11b_WirelessCtrl_button1name#>"];
		var wme_value = ["on"];  
		add_options_x2(document.form.wl_wme, wme_array, wme_value, "on");
		inputCtrl(document.form.wl_frag, 0);
	}
		
	adjust_tx_power();	
	if(svc_ready == "0")
		document.getElementById('svc_hint_div').style.display = "";	
	
	corrected_timezone();		
	if(based_modelid == "RT-AC87U" && is_unit_5g(wl_unit_value)){	//for RT-AC87U 5 GHz Advanced setting
		document.getElementById("wl_mrate_select").style.display = "none";
		document.getElementById("ampdu_rts_tr").style.display = "none";
		document.getElementById("rts_threshold").style.display = "none";
		document.getElementById("wl_frameburst_field").style.display = "none";
		document.getElementById("wl_wme_apsd_field").style.display = "none";
		document.getElementById("wl_ampdu_mpdu_field").style.display = "none";
		document.getElementById("wl_ack_ratio_field").style.display = "none";
		document.getElementById("wl_MU_MIMO_field").style.display = "";
		document.form.wl_mumimo.disabled = false;
	}
	
	if(non_frameburst_support){
		document.getElementById("wl_frameburst_field").style.display = "none";
	}

	if(bcm_mumimo_support){
		document.getElementById("wl_MU_MIMO_field").style.display = "";
		document.form.wl_mumimo.disabled = false;

		/* MODELDEP */
		if (based_modelid == "GT-AC2900") {	//MODELDEP: AC2900(RT-AC86U)
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
			document.form.wl_mumimo.disabled = true;
		}
		else if((based_modelid == "RT-AC85U" || based_modelid == "RT-AC85P" || based_modelid == "RT-ACRH26" || based_modelid == "RT-AC65U" || based_modelid == "RT-ACRH18" || based_modelid == "4G-AC86U" || based_modelid == "4G-AX56") && is_unit_24g(wl_unit_value)){
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
		}
		else if(based_modelid == "RT-AX92U" && (is_unit_24g(wl_unit_value) || is_unit_5g(wl_unit_value))){
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
			document.form.wl_mumimo.disabled = true;
		}
		else if (based_modelid == 'RT-AX88U' && is_unit_24g(wl_unit_value)) {
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
			document.form.wl_mumimo.disabled = true;
		}
		else if ((based_modelid == 'RT-AX92U' || based_modelid == 'RT-AX95Q' || based_modelid == 'XT8PRO' || based_modelid == 'BT12' || based_modelid == 'BT10' || based_modelid == 'BQ16' || based_modelid == 'BQ16_PRO' || based_modelid == 'BM68' || based_modelid == 'XT8_V2' || based_modelid == 'RT-AXE95Q' || based_modelid == 'ET8PRO' || based_modelid == 'ET8_V2' || based_modelid == 'RT-AX56_XD4' || based_modelid == 'XD4PRO' || based_modelid == 'CT-AX56_XD4' || based_modelid == 'RT-AX56U' || based_modelid == "RP-AX56" || based_modelid == "RP-AX58" || based_modelid == "RP-BE58" || based_modelid == 'XC5' || based_modelid == "EBA63") && (is_unit_24g(wl_unit_value) || is_unit_5g(wl_unit_value))){
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
			document.form.wl_mumimo.disabled = true;
		}
		else if(Rawifi_support && band5g_11ax_support){
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
			document.form.wl_mumimo.disabled = true;
		}
	}

	if (lantiq_support) {
		if (is_unit_5g(wl_unit_value)) {
			$('#wl_MU_MIMO_field').show();
			document.form.wl_mumimo.disabled = false;
			document.form.wl1_mumimo.disabled = false;
			document.form.wl_mumimo.value = document.form.wl1_mumimo.value;
		}
	}
	
	/*Airtime fairness, only for Broadcom ARM platform, except RT-AC87U 5 GHz*/
	if(Bcmwifi_support && machine_arm){
		inputCtrl(document.form.wl_atf, 1);
		if(based_modelid == "RT-AC87U" && is_unit_5g(wl_unit_value)){
			inputCtrl(document.form.wl_atf, 0);
		}		
	}
	else if(atf_support && wl_unit_value != '3'){
		inputCtrl(document.form.wl_atf, 1); /* QCA Airtime fairness. */
	}
	else{
		inputCtrl(document.form.wl_atf, 0);
	}
	

	/* Extended NSS, 5G 160MHz only */
	if((is_unit_5g(wl_unit_value) || is_unit_5g_2(wl_unit_value)) && document.form.wl_nmode_x.value != 2 && wl_bw_160_value == '1' && (based_modelid == "GT-AXY16000" || based_modelid == "RT-AX89U")){
		inputCtrl(document.form.wl_ext_nss, 1);
	}
	else{
		inputCtrl(document.form.wl_ext_nss, 0);
	}

	/* Hardware WiFi offloading */
	if(wl_unit_value != '3' && (based_modelid == "RT-AC88Q" || based_modelid == "BRT-AC828" || based_modelid == "RT-AD7200")){
		inputCtrl(document.form.wl_hwol, 1);
	}
	else{
		inputCtrl(document.form.wl_hwol, 0);
	}

	/* Agile DFS, EU sku, HE2.0 only */
	if((is_unit_5g(wl_unit_value) || is_unit_5g_2(wl_unit_value)) && "<% nvram_get("wl0_country_code"); %>" == 'GB' && "<% soc_version_major(); %>" == "2" && (based_modelid == "RT-AX89U" || based_modelid == "GT-AXY16000")){
		inputCtrl(document.form.wl_precacen, 1);
	}
	else{
		inputCtrl(document.form.wl_precacen, 0);
	}

	if(based_modelid != "RT-AC87U"){
		check_ampdu_rts();
	}

	if(Rawifi_support || Qcawifi_support)	//brcm : 3 ; else : 1
		document.getElementById("wl_dtim_th").onClick = function (){openHint(3, 4);}
	else
		document.getElementById("wl_dtim_th").onClick = function (){openHint(3, 11);}

	/* 2.4GHz Bluetooth Coexisistence mode, only for Broadcom platform */
	if (!Qcawifi_support && !Rawifi_support && !Rtkwifi_support && !lantiq_support && is_unit_24g(wl_unit_value))
		inputCtrl(document.form.wl_btc_mode, 1);
	else
		inputCtrl(document.form.wl_btc_mode, 0);
	
	/*ui_location_code Setting*/		
	if(location_list_support && !cfg_ui_region_disable){
		generate_country_selection();
		document.getElementById('region_tr').style.display = "";
	}

	control_TimeField();

	if(isSwMode("RP")){
		var _rows = document.getElementById("WAdvTable").rows;
		for(var i=0; i<_rows.length; i++){
			if(_rows[i].className.search("rept") == -1){
				_rows[i].style.display = "none";
				_rows[i].disabled = true;
			}
		}

		if(!Rawifi_support){
			document.getElementById("DLSCapable").style.display = "none";
		}
	}
	else if(isSwMode("ew")){
		var _rows = document.getElementById("WAdvTable").rows;
		for(var i=0; i<_rows.length; i++){
			if(_rows[i].className.search("ew") == -1){
				_rows[i].style.display = "none";
				_rows[i].disabled = true;
			}
		}

		if(wlc_express == "1"){
			document.form.wl_unit.innerHTML = '<option class="content_input_fd" value="1" selected="">5 GHz</option>';
			if(wl_unit_value != 1) change_wl_unit();
		}
		else if(wlc_express == "2"){
			document.form.wl_unit.innerHTML = '<option class="content_input_fd" value="0" selected="">2.4 GHz</option>';
			if(wl_unit_value != 0) change_wl_unit();
		}
	}

	if(based_modelid == "RP-AC55"){
		if(is_unit_5g(wl_unit_value)){
			document.getElementById("wl_MU_MIMO_field").style.display = "";
			document.form.wl_mumimo.disabled = false;
		}
	}

	if(ofdma_support && sw_mode != 2){
		var wl_11ax = '<% nvram_get("wl_11ax"); %>';
		if(document.form.wl_nmode_x.value == '0' || document.form.wl_nmode_x.value == '8'){
			if (based_modelid != 'RT-AX92U' || (wl_unit_value != '0' && wl_unit_value != '1')) {
				$('#ofdma_field').show();
				if(wl_11ax == '0'){
					document.form.wl_ofdma.value = 0;
					document.form.wl_ofdma.disabled = true;
					$('#ofdma_hint').show();
				}
			}
		}
		else if(document.form.wl_nmode_x.value == '9'){
			$('#ofdma_field').show();
			if(wl_11ax == '0'){
				document.form.wl_ofdma.value = 0;
				document.form.wl_ofdma.disabled = true;
				$('#ofdma_hint').show();
			}
		}

		if(ofdma_onlyDL_support){
			var value = ['0', '1', '3'];
			var desc = ['<#WLANConfig11b_WirelessCtrl_buttonname#>', 'DL OFDMA only', 'DL OFDMA + MU-MIMO'];
			add_options_x2(document.form.wl_ofdma, desc, value, document.form.wl_ofdma.value);
		}
		else if(Rawifi_support){
			var value = ['0', '1', '4', '2','3'];
			var desc = ['<#WLANConfig11b_WirelessCtrl_buttonname#>', 'DL OFDMA only', 'DL OFDMA + MU-MIMO', 'DL/UL OFDMA', 'DL/UL OFDMA + MU-MIMO'];
            var _ori_value = "<% nvram_get("wl_ofdma"); %>";
			add_options_x2(document.form.wl_ofdma, desc, value, _ori_value);
		}
	}

	if(maxassoc_support){
		$("#wl_bss_maxassoc_field").show();
	}
	else{
		$("#wl_bss_maxassoc_field").hide();
	}

	if(wifi7_support){
		document.getElementById('wifi7_mode_field').style.display = '';
		document.getElementById('he_mode_field').style.display = 'none';
		inputCtrl(document.form.wl_btc_mode, 0);
		document.getElementById("wl_mrate_select").style.display = "none";
		document.getElementById("wl_ampdu_mpdu_field").style.display = "none";
		document.getElementById("wl_module_scheme_field").style.display = "none";
        document.getElementById("wl_txbf_field").style.display = "none";
		document.getElementById("wl_itxbf_field").style.display = "none";
		if(!wifi7_mumimo_support){
			document.getElementById("wl_MU_MIMO_field").style.display = "none";
		}
	}
	
	if(isSupport("sdn_mainfh")){
		document.getElementById('wifi7_mode_field').style.display = 'none';
		document.getElementById('wl_sched_enable').style.display = 'none';
	}	
}

function generate_country_selection(){
	var code = '';
	var matched = false;
	
	code += '<select class="input_option" name="ui_location_code">';
	for(i=0; i<country_array.length; i++){
		var index = country_array[i];
		if(index == "NZ")
			index = "AU";
		var country = country_selection_array[index];
		var name = country ? country.name : '<#WLANConfig11b_x_Region#> ' + index;

		if(tcode == index){
			matched = true;
			name += ' (<#Setting_factorydefault_value#>)';
		}
		code += '<option value='+ index +'>'+ name +'</option>';
	}

	if(!matched){
		code += '<option value='+ tcode +' ><#Setting_factorydefault_value#></option>';
	}

	code += '</select>';

	document.getElementById('region_div').innerHTML = code;

	if(EG_mode){
		document.form.ui_location_code.disabled = true;
		document.getElementById('tx_power_desc_EG').style.display = "";
		if(is_unit_24g(wl_unit_value)){	//2.4GHz
			document.getElementById('wl_txPower_field_title').innerHTML = "<#WLANConfig11b_TxPower_itemname#>&nbsp;&nbsp;&nbsp;&nbsp;<20dBm";
			document.getElementById('tx_power_desc_EG').innerHTML = "<20dBm";
		}
		else{
			document.getElementById('wl_txPower_field_title').innerHTML = "<#WLANConfig11b_TxPower_itemname#>&nbsp;&nbsp;&nbsp;<23dBm";
			document.getElementById('tx_power_desc_EG').innerHTML = "<23dBm";
		}
	}

	if(orig_region == ""){
		document.form.ui_location_code.value = tcode;
	}
	else{
		document.form.ui_location_code.value = orig_region;
	}

}

function adjust_tx_power(){
	var power_value_old = document.form.wl_TxPower.value;	//old nvram not exist now (value)
	var power_value_new = document.form.wl_txpower.value;	//current nvram now (percentage)
	var translated_value = 0;
	
	if(!power_support){
		document.getElementById("wl_txPower_field").style.display = "none";
	}
	else{
		if(power_value_old != ""){
			translated_value = parseInt(power_value_old/80*100);
			if(translated_value >=100){
				translated_value = 100;
			}
			else if(translated_value <=1){
				translated_value = 1;			
			}

			document.getElementById('slider').children[0].style.width = translated_value + "%";
			document.getElementById('slider').children[1].style.left = translated_value + "%";
			document.form.wl_txpower.value = translated_value;
		}
		else{
			document.getElementById('slider').children[0].style.width = power_value_new + "%";
			document.getElementById('slider').children[1].style.left = power_value_new + "%";
			document.form.wl_txpower.value = power_value_new;
		}

		if(document.form.wl_txpower.value < 25){
			document.getElementById('slider').children[0].style.width = "0%";
			document.getElementById('slider').children[1].style.left =  "0%";
			document.form.wl_txpower.value = 0;
			document.getElementById("tx_power_desc").innerHTML = power_table_desc[0];
		}
		else if(document.form.wl_txpower.value < 50){
			document.getElementById('slider').children[0].style.width = "25%";
			document.getElementById('slider').children[1].style.left =  "25%";
			document.form.wl_txpower.value = 25;				
			document.getElementById("tx_power_desc").innerHTML = power_table_desc[1];
		}
		else if(document.form.wl_txpower.value < 88){
			document.getElementById('slider').children[0].style.width = "50%";
			document.getElementById('slider').children[1].style.left =  "50%";
			document.form.wl_txpower.value = 50;				
			document.getElementById("tx_power_desc").innerHTML = power_table_desc[2];
		}
		else if(document.form.wl_txpower.value < 100){
			document.getElementById('slider').children[0].style.width = "75%";
			document.getElementById('slider').children[1].style.left =  "75%";
			document.form.wl_txpower.value = 88;
			document.getElementById("tx_power_desc").innerHTML = power_table_desc[3];
		}
		else{
			document.getElementById('slider').children[0].style.width = "100%";
			document.getElementById('slider').children[1].style.left =  "100%";
			document.form.wl_txpower.value = 100;
			document.getElementById("tx_power_desc").innerHTML = power_table_desc[4];
		}	
	}
}

function changeRSSI(_switch){
	if(_switch == 0){
		document.getElementById("rssiDbm").style.display = "none";
		document.form.wl_user_rssi.value = 0;
	}
	else{
		document.getElementById("rssiDbm").style.display = "";
		var default_value = "-70";
		if(amesh_support && is_unit_24g(wl_unit_value) && ameshRouter_support)
			default_value = "-70";
		if(wl_user_rssi_onload == 0)
			document.form.wl_user_rssi.value = default_value;
		else
			document.form.wl_user_rssi.value = wl_user_rssi_onload;
	}
}

var reboot_confirm=0;
function applyRule(){
	if(lantiq_support && wave_ready != 1){
		alert(`<#Wireless_ready#>`);
		return false;
	}
	
	if(validForm()){
		if(wifi_hw_sw_support && !Qcawifi_support) { //For N55U
			document.form.wl_HW_switch.value = "0";
			document.form.wl_HW_switch.disabled = false;
		}
		
		if(power_support){
			document.form.wl_TxPower.value = "";	
		}
		
		if(	based_modelid == "RT-AC88U" || based_modelid == "RT-AX88U" || based_modelid == "RT-AC86U" || based_modelid == "GT-AC2900" || based_modelid == "RT-AC3100" ||
			based_modelid == "RT-AC5300" || based_modelid == "GT-AC5300" || based_modelid == "GT-AX11000" || based_modelid == "RT-AX92U" || based_modelid == "RT-AX95Q" || based_modelid == "XT8PRO" || based_modelid == "BT12" || based_modelid == "BT10" || based_modelid == "BQ16" || based_modelid == "BQ16_PRO" || based_modelid == "BM68" || based_modelid == "XT8_V2" || based_modelid == "RT-AXE95Q" || based_modelid == "ET8PRO" || based_modelid == "ET8_V2" || based_modelid == "RT-AX56_XD4" || based_modelid == "XD4PRO" || based_modelid == "CT-AX56_XD4" || based_modelid == "RT-AX58U" || based_modelid == "RT-AX58U_V2" || based_modelid == "BR63" || based_modelid == "RT-AX3000N" || based_modelid == "TUF-AX3000" || based_modelid == "TUF-AX3000_V2" || based_modelid == "TUF-AX5400" || based_modelid == "TUF-AX5400_V2" || based_modelid == "RT-AX5400" || based_modelid == "DSL-AX82U" || based_modelid == "RT-AX82U" || based_modelid == "RT-AX82U_V2" || based_modelid == "RT-AX56U" || based_modelid == "RP-AX56" || based_modelid == "RP-AX58" || based_modelid == "RP-BE58" || based_modelid == "RT-AX86U" || based_modelid == "RT-AX5700" || based_modelid == "RT-AX86S" || based_modelid == "RT-AX68U" || based_modelid == "RT-AC68U_V4" || based_modelid == "RT-AXE7800" || based_modelid == "GT10" || based_modelid == "RT-AX9000" || based_modelid == "GT-AXE11000" || based_modelid == "GS-AX3000" || based_modelid == "GS-AX5400" || based_modelid == "RT-BE96U" || based_modelid == "XC5" || based_modelid == "EBA63" || based_modelid == "RT-AX86U_PRO" || based_modelid == "RT-AX88U_PRO" || based_modelid == "RT-BE88U" || based_modelid == "RT-BE86U" || based_modelid == "RT-BE58U" || based_modelid == "TUF-BE3600" || based_modelid == "RT-BE92U" || based_modelid == "RT-BE95U" || based_modelid == "RT-BE82U" || based_modelid == "TUF-BE82" || based_modelid == "RT-BE82M" || based_modelid == "RT-BE58U_PRO" || based_modelid == "GS-BE18000" || based_modelid == "GS-BE12000" || based_modelid == "GS7_PRO"){
			document.form.action_wait.value = "10";
		}
		// bcm4912 models
		else if ( based_modelid == "GT-AX6000" || based_modelid == "GT-AX11000_PRO" || based_modelid == "ET12" || based_modelid == "XT12" || based_modelid == "GT-AXE16000" || based_modelid == "GT-BE98" || based_modelid == "GT-BE98_PRO" || based_modelid == "GT-BE96" || based_modelid == "GT-BE19000" ||
			  based_modelid == "RT-AX88U_PRO" || based_modelid == "GT-BE19000AI" || based_modelid == "GT-BE96_AI") {
			document.form.action_wait.value = "15";
		}
		else if(sdk_7){
			document.form.action_wait.value = "5";
		}

		if (Bcmwifi_support && wl_txpower_orig != document.form.wl_txpower.value) {
			if(!wifi7_support){
				reboot_confirm=1;
			}
		}

		if(is_unit_5g(wl_unit_value) && "<% nvram_get("wl1_country_code"); %>" == "EU" && based_modelid == "RT-AC87U"){	//for EU RT-AC87U 5G Advanced setting
			if(document.form.wl1_80211h[0].selected && "<% nvram_get("wl1_chanspec"); %>" == "0")	//Interlocking set acs_dfs="0" while disabled 802.11h and wl1_chanspec="0"(Auto)
				document.form.acs_dfs.value = "0";
		}
		
		if(location_list_support && !cfg_ui_region_disable){
			if((orig_region.length > 0 && orig_region != document.form.ui_location_code.value)
			|| (orig_region == "" && document.form.ui_location_code.value != tcode)){
				if(amesh_support && ((isSwMode("RT") || isSwMode("WISP")) || isSwMode("ap")) && ameshRouter_support) {
					if(!AiMesh_confirm_msg("Wireless_CountryCode"))
						return;
				}
				if(lantiq_support){
					document.form.action_script.value = "restart_wireless";
					document.form.group_id.value = "location"
				}
				else{
					reboot_confirm=1;
				}
			}				
		}

		if((based_modelid == "RT-AC87U") && is_unit_5g(wl_unit_value)){
			if(document.form.wl_mumimo.value != "<% nvram_get("wl_mumimo"); %>"){
				reboot_confirm=1;	
			}
		}

		if(lantiq_support && is_unit_5g(wl_unit_value)){
			document.form.wl1_mumimo.value = document.form.wl_mumimo.value;
			document.form.group_id.value = "mumimo"
		}
		
		document.form.wl_sched.value = wifi_schedule_value;
		if(amesh_support && ((isSwMode("RT") || isSwMode("WISP")) || isSwMode("ap")) && ameshRouter_support) {
			var radio_value = (document.form.wl_radio[0].checked) ? 1 : 0;
			if(!AiMesh_confirm_msg("Wireless_Radio",radio_value))
				return;
		}

		if(location_list_support && !cfg_ui_region_disable){
			var uiLocationCode = document.form.ui_location_code.value;
			var locationCode = (U5_country_code && uiLocationCode == "U5") ? "AA" : uiLocationCode;
		}

		if(reboot_confirm==1){
			if(confirm("<#AiMesh_Node_Reboot#>")){
				FormActions("start_apply.htm", "apply", "reboot", reboot_needed_time);

				if(location_list_support && !cfg_ui_region_disable){
					httpApi.nvramSet({
						"location_code": locationCode,
						"action_mode": "apply",
						"rc_service": "saveNvram"
					})
				}

				showLoading();
				document.form.submit();
			}
		}
		else{
			if(location_list_support && !cfg_ui_region_disable){
				httpApi.nvramSet({
					"location_code": locationCode,
					"action_mode": "apply",
					"rc_service": "saveNvram"
				})
			}

			showLoading();
			setTimeout(function(){document.form.submit()}, 500);
		}		
	}
}

function validForm(){
	if(sw_mode != "2"){
		if(!validator.range(document.form.wl_frag, 256, 2346)
				|| !validator.range(document.form.wl_rts, 0, 2347)
				|| !validator.range(document.form.wl_dtim, 1, 255)
				|| !validator.range(document.form.wl_bcn, 20, 1000)
				){
			return false;
		}	
		if(Qcawifi_support){
			min_bcn=40;
			if(based_modelid == "BRT-AC828" || based_modelid == "GT-AXY16000" || based_modelid == "RT-AX89U" || based_modelid == "RT-AD7200" || based_modelid == "PL-AX56_XP4")
				min_bcn=100;
				
			if(!validator.range(document.form.wl_bcn, min_bcn, 1000))
				return false;
		}
	}
		
	if(power_support && !Rawifi_support && !Qcawifi_support){
		// MODELDEP
		if(hw_ver.search("RTN12HP") != -1){
		  FormActions("start_apply.htm", "apply", "set_wltxpower;reboot", "<% get_default_reboot_time(); %>");
		}
		else if(based_modelid == "RT-AC66U" || based_modelid == "RT-N66U" || based_modelid == "RT-N18U"){
			FormActions("start_apply.htm", "apply", "set_wltxpower;restart_wireless", "15");
		}	
	}

	if(maxassoc_support){
		if(!validator.range(document.form.wl_bss_maxassoc, 1, 128)){
			document.form.wl_bss_maxassoc.focus();
			return false;
		}
	}

	if(userRSSI_support){
		if(document.form.wl_user_rssi.value != 0){
			if(!validator.range(document.form.wl_user_rssi, -90, -40)){
				document.form.wl_user_rssi.focus();
				return false;			
			}
		}
	}

	return true;
}

function done_validating(action){
	refreshpage();
}

function disableAdvFn(row){
	for(var i=row; i>=3; i--){
		document.getElementById("WAdvTable").deleteRow(i);
	}
}

function enable_wme_check(obj){
	if(obj.value == "off"){    //Disable
		inputCtrl(document.form.wl_wme_no_ack, 0);
		inputCtrl(document.form.wl_wme_apsd, 0);
		if(!Rawifi_support && !Qcawifi_support){
			inputCtrl(document.form.wl_igs, 0);
		}
	}
	else{    //Auto, Enable
		inputCtrl(document.form.wl_wme_no_ack, 1);
		inputCtrl(document.form.wl_wme_apsd, 1);	
		if(!Rawifi_support && !Qcawifi_support){
			inputCtrl(document.form.wl_igs, 1);
		}		
	}
}

/* AMPDU RTS for AC model, Jieming added at 2013.08.26 */
function check_ampdu_rts(){
	if(document.form.wl_nmode_x.value != 2 && band5g_11ac_support && !Qcawifi_support && !Rawifi_support){
		document.getElementById('ampdu_rts_tr').style.display = "";
		if(document.form.wl_ampdu_rts.value == 1){
			document.form.wl_rts.disabled = false;
			document.getElementById('rts_threshold').style.display = "";
		}	
		else{
			document.form.wl_rts.disabled = true;
			document.getElementById('rts_threshold').style.display = "none";
		}

		if(no_vht_support){		//Hide 11AC/80MHz from GUI
			document.form.wl_ampdu_rts.disabled = true;
			document.getElementById('ampdu_rts_tr').style.display = "none";
		}
	}
	else{
		document.form.wl_ampdu_rts.disabled = true;
		document.getElementById('ampdu_rts_tr').style.display = "none";
	}
}
power_table_desc = ["<#WLANConfig11b_TxPower1#>", "<#WLANConfig11b_TxPower2#>", "<#WLANConfig11b_TxPower3#>", "<#WLANConfig11b_TxPower4#>", "<#WLANConfig11b_TxPower5#>"];
//power_table_desc = ["Power Saving", "Fair", "Balance", "Good", "Performance"];
//power_table_desc = ["省電", "弱", "平衡", "強", "效能"];
function register_event(){
	
	$(function() {
		$( "#slider" ).slider({
			orientation: "horizontal",
			range: "min",
			min:1,
			max: 5,
			value:5,
			slide:function(event, ui){
				document.getElementById('tx_power_desc').innerHTML = power_table_desc[ui.value-1];
			},
			stop:function(event, ui){
				set_power(ui.value);	  
			}
		}); 
	});

	var array_temp = new Array(7);
	var checked = 0
	var unchecked = 0;
	init_array(array_temp);

  $(function() {
    $( "#selectable" ).selectable({
		filter:'td',
		selecting: function(event, ui){

		},
		unselecting: function(event, ui){

		},
		selected: function(event, ui){
			id = ui.selected.getAttribute('id');
			column = parseInt(id.substring(0,1), 10);
			row = parseInt(id.substring(1,3), 10);

			array_temp[column][row] = 1;
			if(array[column][row] == 1){
				checked = 1;
			}
			else if(array[column][row] == 0){
				unchecked = 1;
			}
		},
		unselected: function(event, ui){

		},
		stop: function(event, ui){
			if((checked == 1 && unchecked == 1) || (checked == 0 && unchecked == 1)){
				for(i=0;i<7;i++){
					for(j=0;j<24;j++){
						if(array_temp[i][j] == 1){
						array[i][j] = array_temp[i][j];
						array_temp[i][j] = 0;		//initialize
						if(j < 10){
							j = "0" + j;
						}
							id = i.toString() + j.toString();
							document.getElementById(id).className = "checked";
						}
					}
				}
			}
			else if(checked == 1 && unchecked == 0){
				for(i=0;i<7;i++){
					for(j=0;j<24;j++){
						if(array_temp[i][j] == 1){
						array[i][j] = 0;
						array_temp[i][j] = 0;

						if(j < 10){
							j = "0" + j;
						}
							id = i.toString() + j.toString();
							document.getElementById(id).className = "disabled";
						}
					}
				}
			}

			checked = 0;
			unchecked = 0;
		}
	});
  });
}

function set_power(power_value){	
	var power_table = [0, 25, 50, 88, 100];	
	document.form.wl_txpower.value = power_table[power_value-1];
}

function init_array(arr){
	for(i=0;i<7;i++){
		arr[i] = new Array(24);

		for(j=0;j<24;j++){
			arr[i][j] = 0;
		}
	}
}

function init_cookie(){
	if(document.cookie.indexOf('clock_type') == -1)		//initialize
		document.cookie = "clock_type=1";

	x = document.cookie.split(';');
	for(i=0;i<x.length;i++){
		if(x[i].indexOf('clock_type') != -1){
			clock_type = x[i].substring(x[i].length-1, x[i].length);
		}
	}
}

//draw time slot at first time
function redraw_selected_time(obj){
	var start_day = 0;
	var end_day = 0;
	var start_time = "";
	var end_time = "";
	var time_temp = "";
	var duration = "";
	var id = "";

	for(i=0;i<obj.length;i++){
		time_temp = obj[i];
		start_day = parseInt(time_temp.substring(0,1), 10);
		end_day =  parseInt(time_temp.substring(1,2), 10);
		start_time =  parseInt(time_temp.substring(2,4), 10);
		end_time =  parseInt(time_temp.substring(4,6), 10);
		if((start_day == end_day) && (end_time - start_time) < 0)	//for Sat 23 cross to Sun 00
			end_day = 7;

		if(start_day == end_day){			// non cross day
			duration = end_time - start_time;
			if(duration == 0)	//for whole selected
				duration = 7*24;

			while(duration >0){
				array[start_day][start_time] = 1;
				if(start_time < 10)
					start_time = "0" + start_time;

				id = start_day.toString() + start_time.toString();
				document.getElementById(id).className = "checked";
				start_time++;
				if(start_time == 24){
					start_time = 0;
					start_day++;
					if(start_day == 7)
						start_day = 0;
				}

				duration--;
				id = "";
			}
		}else{			// cross day
			var duration_day = 0;
			if(end_day - start_day < 0)
				duration_day = 7 - start_day;
			else
				duration_day = end_day - start_day;

			duration = (24 - start_time) + (duration_day - 1)*24 + end_time;
			while(duration > 0){
				array[start_day][start_time] = 1;
				if(start_time < 10)
					start_time = "0" + start_time;

				id = start_day.toString() + start_time.toString();
				document.getElementById(id).className = "checked";
				start_time++;
				if(start_time == 24){
					start_time = 0;
					start_day++;
					if(start_day == 7)
						start_day = 0;
				}

				duration--;
				id = "";
			}
		}
	}
}

function select_all(){
	var full_flag = 1;
	for(i=0;i<7;i++){
		for(j=0;j<24;j++){
			if(array[i][j] ==0){
				full_flag = 0;
				break;
			}
		}

		if(full_flag == 0){
			break;
		}
	}

	if(full_flag == 1){
		for(i=0;i<7;i++){
			for(j=0;j<24;j++){
				array[i][j] = 0;
				if(j<10){
					j = "0"+j;
				}

				id = i.toString() + j.toString();
				document.getElementById(id).className = "disabled";
			}
		}
	}
	else{
		for(i=0;i<7;i++){
			for(j=0;j<24;j++){
				if(array[i][j] == 1)
					continue;
				else{
					array[i][j] = 1;
					if(j<10){
						j = "0"+j;
					}

					id = i.toString() + j.toString();
					document.getElementById(id).className = "checked";
				}
			}
		}
	}
}

function select_all_day(day){
	var check_flag = 0
	day = day.substring(4,5);
	for(i=0;i<24;i++){
		if(array[day][i] == 0){
			check_flag = 1;
		}
	}

	if(check_flag == 1){
		for(j=0;j<24;j++){
			array[day][j] = 1;
			if(j<10){
				j = "0"+j;
			}

			id = day + j;
			document.getElementById(id).className = "checked";
		}
	}
	else{
		for(j=0;j<24;j++){
			array[day][j] = 0;
			if(j<10){
				j = "0"+j;
			}

			id = day + j;
			document.getElementById(id).className = "disabled";
		}
	}
}

function select_all_time(time){
	var check_flag = 0;
	time_int = parseInt(time, 10);
	for(i=0;i<7;i++){
		if(array[i][time] == 0){
			check_flag = 1;
		}
	}

	if(time<10){
		time = "0"+time;
	}

	if(check_flag == 1){
		for(i=0;i<7;i++){
			array[i][time_int] = 1;

		id = i + time;
		document.getElementById(id).className = "checked";
		}
	}
	else{
		for(i=0;i<7;i++){
			array[i][time_int] = 0;

		id = i + time;
		document.getElementById(id).className = "disabled";
		}
	}
}

function change_clock_type(type){
	document.cookie = "clock_type="+type;
	if(type == 1)
		var array_time = ["00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"];
	else
		var array_time = ["12", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"];

	for(i=0;i<24;i++){
		if(type == 1)
			document.getElementById(i).innerHTML = array_time[i] +" ~ "+ array_time[i+1];
		else{
			if(i<11 || i == 23)
				document.getElementById(i).innerHTML = array_time[i] +" ~ "+ array_time[i+1] + " AM";
			else
				document.getElementById(i).innerHTML = array_time[i] +" ~ "+ array_time[i+1] + " PM";
		}
	}
}

function save_wifi_schedule(){
	var flag = 0;
	var start_day = 0;
	var end_day = 0;
	var start_time = 0;
	var end_time = 0;
	var time_temp = "";

	for(i=0;i<7;i++){
		for(j=0;j<24;j++){
			if(array[i][j] == 1){
				if(flag == 0){
					flag =1;
					start_day = i;
					if(j<10)
						j = "0" + j;

					start_time = j;
				}
			}
			else{
				if(flag == 1){
					flag =0;
					end_day = i;
					if(j<10)
						j = "0" + j;

					end_time = j;
					if(time_temp != "")
						time_temp += "<";

					time_temp += start_day.toString() + end_day.toString() + start_time.toString() + end_time.toString();
				}
			}
		}
	}

	if(flag == 1){
		if(time_temp != "")
			time_temp += "<";

		time_temp += start_day.toString() + "0" + start_time.toString() + "00";
	}

	if(time_temp == ""){
		alert("If you want to deny WiFi radio all time, you should check the '<#WLANConfig11b_x_RadioEnable_itemname#>' to No");
		return false;
	}

	wifi_schedule_value = time_temp;
	document.getElementById("schedule_block").style.display = "none";
	document.getElementById("titl_desc").style.display = "";
	document.getElementById("WAdvTable").style.display = "";
	document.getElementById("apply_btn").style.display = "";
}

function cancel_wifi_schedule(client){
	init_array(array);
	document.getElementById("schedule_block").style.display = "none";
	document.getElementById("titl_desc").style.display = "";
	document.getElementById("WAdvTable").style.display = "";
	document.getElementById("apply_btn").style.display = "";
}

function count_time(){		// To count system time
	systime_millsec += 1000;
	setTimeout("count_time()", 1000);
}

function showclock(){
	JS_timeObj.setTime(systime_millsec);
	JS_timeObj2 = JS_timeObj.toString();
	JS_timeObj2 = JS_timeObj2.substring(0,3) + ", " +
	              JS_timeObj2.substring(4,10) + "  " +
				  checkTime(JS_timeObj.getHours()) + ":" +
				  checkTime(JS_timeObj.getMinutes()) + ":" +
				  checkTime(JS_timeObj.getSeconds()) + "  " +
				  JS_timeObj.getFullYear();
	document.getElementById("system_time").value = JS_timeObj2;
	setTimeout("showclock()", 1000);

	if(svc_ready == "0")
		document.getElementById('svc_hint_div').style.display = "";
	corrected_timezone();
}

function show_wifi_schedule(){
	document.getElementById("titl_desc").style.display = "none";
	document.getElementById("WAdvTable").style.display = "none";
	document.getElementById("apply_btn").style.display = "none";
	document.getElementById("schedule_block").style.display = "";

	var array_date = ["Select All", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
	var array_time_id = ["00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"];
	if(clock_type == "1")
		var array_time = ["00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"];
	else
		var array_time = ["12am", "1am", "2am", "3am", "4am", "5am", "6am", "7am", "8am", "9am", "10am", "11am", "12pm", "1pm", "2pm", "3pm", "4pm", "5pm", "6pm", "7pm", "8pm", "9pm", "10pm", "11pm", "12am"];

	var code = "";

	wifi_schedule_row = wifi_schedule_value.split('<');

	code +='<div style="margin-bottom:10px;color: #003399;font-family: Verdana;" align="left">';
	code +='<table width="100%" border="1" cellspacing="0" cellpadding="4" align="center" class="FormTable">';
	code +='<thead><tr><td colspan="6" id="LWFilterList"><#ParentalCtrl_Act_schedule#></td></tr></thead>';
	code +='<tr><th style="width:40%;height:20px;" align="right"><#General_x_SystemTime_itemname#></th>';
	code +='<td align="left" style="color:#FFF"><input type="text" id="system_time" name="system_time" class="devicepin" value="" readonly="1" style="font-size:12px;width:200px;"></td></tr>';
	code +='</table><table id="main_select_table">';
	code +='<table  id="selectable" class="table_form" >';
	code += "<tr>";
	for(i=0;i<8;i++){
		if(i == 0)
			code +="<th class='parental_th' onclick='select_all();'>"+array_date[i]+"</th>";
		else
			code +="<th id=col_"+(i-1)+" class='parental_th' onclick='select_all_day(this.id);'>"+array_date[i]+"</th>";
	}

	code += "</tr>";
	for(i=0;i<24;i++){
		code += "<tr>";
		code +="<th id="+i+" class='parental_th' onclick='select_all_time(this.id)'>"+ array_time[i] + " ~ " + array_time[i+1] +"</th>";
		for(j=0;j<7;j++){
			code += "<td id="+ j + array_time_id[i] +" class='disabled' ></td>";
		}

		code += "</tr>";
	}

	code +='</table></table></div>';
	document.getElementById("mainTable").innerHTML = code;

	register_event();
	redraw_selected_time(wifi_schedule_row);

	var code_temp = "";
	code_temp = '<table style="width:350px;margin-left:20px;"><tr>';
	code_temp += "<td><div style=\"width:95px;font-family:Arial,sans-serif,Helvetica;font-size:18px;\"><#Clock_Format#></div></td>";
	code_temp += '<td><div>';
	code_temp += '<select id="clock_type_select" class="input_option" onchange="change_clock_type(this.value);">';
	code_temp += '<option value="0" >12-hour</option>';
	code_temp += '<option value="1" >24-hour</option>';
	code_temp += '</select>';
	code_temp += '</div></td>';
	code_temp += '<td><div align="left" style="font-family:Arial,sans-serif,Helvetica;font-size:18px;margin:0px 5px 0px 30px;"><#ParentalCtrl_allow#></div></td>';
	code_temp += '<td><div style="width:90px;height:20px;background:#9CB2BA;"></div></td>';
	code_temp += '<td><div align="left" style="font-family:Arial,sans-serif,Helvetica;font-size:18px;margin:0px 5px 0px 30px;"><#ParentalCtrl_deny#></div></td>';
	code_temp += '<td><div style="width:90px;height:20px;border:1px solid #000;background:#475A5F;"></div></td>';
	code_temp += '</tr></table>';
	document.getElementById('hintBlock').innerHTML = code_temp;
	document.getElementById('hintBlock').style.marginTop = "10px";
	document.getElementById('hintBlock').style.display = "";
	document.getElementById("ctrlBtn").innerHTML = '<input class="button_gen" type="button" onClick="cancel_wifi_schedule();" value="<#CTL_Cancel#>" style="margin:0 10px;">';
	document.getElementById("ctrlBtn").innerHTML += '<input class="button_gen" type="button" onClick="save_wifi_schedule();" value="<#CTL_ok#>" style="margin:0 10px;">';
	document.getElementById('clock_type_select')[clock_type].selected = true;		// set clock type by cookie

	document.getElementById("mainTable").style.display = "";
	$("#mainTable").fadeIn();
	showclock();
}

function control_TimeField(){
	if(document.form.wl_radio[0].checked){
		document.getElementById("wl_sched_enable").style.display = "";
		document.form.wl_timesched.disabled = false;
		if(document.form.wl_timesched[0].checked){
			document.getElementById("time_setting").style.display = "";
			document.form.wl_sched.disabled = false;
		}
		else{
			document.getElementById("time_setting").style.display = "none";
			document.form.wl_sched.disabled = true;
		}
	}
	else{
		document.getElementById("wl_sched_enable").style.display = "none";
		document.form.wl_timesched.disabled = true;
	}
}

function handle_mimo(value){
	if(value == 1 && document.form.wl_txbf.value == 0){
		document.form.wl_txbf.value = 1;
	}
}

function handle_beamforming(value){
	var confirm_txt = "<#WLANConfig11b_MUMIMO_disabled_confirm#>";

	if(is_unit_5g(wl_unit_value) || is_unit_5g_2(wl_unit_value)){ // 5GHz up
		if(band5g_11ax_support){
			if(based_modelid == 'RT-AX92U'){
				confirm_txt = confirm_txt.replace("$Beamforming$", "<#WLANConfig11b_x_acBeam#>");
			}
			else{
				confirm_txt = confirm_txt.replace("$Beamforming$", "<#WLANConfig11b_x_axBeam#>");
			}
		}
		else{
			confirm_txt = confirm_txt.replace("$Beamforming$", "<#WLANConfig11b_x_acBeam#>");
		}
	}
	else{	// 2.4GHz
		confirm_txt = confirm_txt.replace("$Beamforming$", "<#WLANConfig11b_x_ExpBeam#>");
	}

	if (value == 0 && document.form.wl_mumimo.value == 1){
		if (confirm(confirm_txt)) {
			document.form.wl_mumimo.value = 0;
		}
		else {
			document.form.wl_txbf.value = 1;
			return false;
		}
	}
}

function checkWLReady(){
	$.ajax({
	    url: '/ajax_wl_ready.asp',
	    dataType: 'script',	
	    error: function(xhr) {
			setTimeout("checkWLReady();", 1000);
	    },
	    success: function(response){
	    	if(wave_ready != 1){
	    		$("#lantiq_ready").show();
	    		setTimeout("checkWLReady();", 1000);
	    	}
	    	else{
	    		$("#lantiq_ready").hide();
	    	}
			
	    }
  	});
}

function wifi7_mode(obj){
	var wifi7ModeEnable = obj.value;
	if(wifi7ModeEnable == '1'){
		let hintStr = '';
		if(document.form.wl_auth_mode_x.value == 'psk'
		|| document.form.wl_auth_mode_x.value == 'psk2'
		|| document.form.wl_auth_mode_x.value == 'pskpsk2'){
			hintStr = 'WPA3-Personal';
		}
		else if(document.form.wl_auth_mode_x.value == 'wpa'
			 || document.form.wl_auth_mode_x.value == 'wpa2'
			 || document.form.wl_auth_mode_x.value == 'wpawpa2'){
				if(document.form.wl_radius_ipaddr.value != '' && document.form.wl_radius_key != '' && document.form.wl_radius_port.value != ''){
					hintStr = 'WPA2/WPA3-Enterprise';
				}
				// else{
				// 	hintStr = `<#WiFi7_enable_hint#>`;
				// }

		}
		else if(document.form.wl_auth_mode_x.value == 'open'
			 || document.form.wl_auth_mode_x.value == 'openowe'){
				hintStr = 'Enhanced Open';
		}
		if(document.form.wl_auth_mode_x.value == 'psk'
		|| document.form.wl_auth_mode_x.value == 'psk2'
		|| document.form.wl_auth_mode_x.value == 'pskpsk2'
		|| document.form.wl_auth_mode_x.value == 'wpa'
		|| document.form.wl_auth_mode_x.value == 'wpa2'
		|| document.form.wl_auth_mode_x.value == 'wpawpa2'
		|| document.form.wl_auth_mode_x.value == 'open'
		|| document.form.wl_auth_mode_x.value == 'openowe'){
			confirm_asus({
                title: "",
                contentA: `<#WiFi7_enable_hint#>`,
                contentC: "",
                left_button: "<#checkbox_No#>",
                left_button_callback: function () {
                    confirm_cancel();
					be_confirm_flag = 0;
		 			document.form.wl_11be.value = wifi7ModeEnable === '1' ? '0': '1';
                    	return false;
                },
                left_button_args: {},
                right_button: "<#checkbox_Yes#>",
                right_button_callback: function () {
					be_confirm_flag = 1;
                    confirm_cancel();
                },
                right_button_args: {},
                iframe: "",
                margin: (() => {
                    return `${document.documentElement.scrollTop}px 0 0 25px`;
                })(),
                note_display_flag: 0,
            });
		}
	}
	else{
		if(mloEnable == '1'){
			confirm_asus({
                title: "MLO Hint",
                contentA:
                    `<b><#WiFi7_mlo_adjust_hint#></b>`,
                contentC: "",
                left_button: "<#CTL_Cancel#>",
                left_button_callback: function () {
                    confirm_cancel();
					document.form.wl_11be.value = '0';
                    return false;
                },
                left_button_args: {},
                right_button: "<#btn_go#>",
                right_button_callback: function () {
                    confirm_cancel();
                    location.href = "/MLO.asp";
                },
                right_button_args: {},
                iframe: "",
                margin: (() => {
                    return `${document.documentElement.scrollTop}px 0 0 25px`;
                })(),
                note_display_flag: 0,
            });
		}
	}
}
</script>
</head>

<body onload="initial();" onunLoad="return unload_body();" class="bg">
<div id="TopBanner"></div>

<div id="Loading" class="popup_bg"></div>


<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="wl_nmode_x" value="<% nvram_get("wl_nmode_x"); %>">
<input type="hidden" name="wl_gmode_protection_x" value="<% nvram_get("wl_gmode_protection_x"); %>">
<input type="hidden" name="wl0_11ax" value="<% nvram_get("wl0_11ax"); %>">
<input type="hidden" name="current_page" value="Advanced_WAdvanced_Content.asp">
<input type="hidden" name="next_page" value="Advanced_WAdvanced_Content.asp">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_mode" value="apply_new">
<input type="hidden" name="action_script" value="restart_wireless">
<input type="hidden" name="action_wait" value="3">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="wl_subunit" value="-1">
<input type="hidden" name="wl_amsdu" value="<% nvram_get("wl_amsdu"); %>">
<input type="hidden" name="wl0_country_code" value="<% nvram_get("wl0_country_code"); %>" disabled>
<input type="hidden" name="wl_HW_switch" value="<% nvram_get("wl_HW_switch"); %>" disabled>
<input type="hidden" name="wl_TxPower" value="<% nvram_get("wl_TxPower"); %>" >
<input type="hidden" name="wl1_80211h_orig" value="<% nvram_get("wl1_80211h"); %>" >
<input type="hidden" name="acs_dfs" value="<% nvram_get("acs_dfs"); %>">
<input type="hidden" name="w_Setting" value="1">
<input type="hidden" name="wl_sched" value="<% nvram_get("wl_sched"); %>">
<input type="hidden" name="wl_txpower" value="<% nvram_get("wl_txpower"); %>">
<input type="hidden" name="wl1_mumimo" value="<% nvram_get("wl1_mumimo"); %>" disabled>
<table class="content" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td width="17">&nbsp;</td>
		
		<td valign="top" width="202">				
		<div  id="mainMenu"></div>	
		<div  id="subMenu"></div>		
		</td>				
		
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div>

		<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
	<tr>
		<td valign="top" >

			<table width="760px" border="0" cellpadding="4" cellspacing="0" class="FormTitle" id="FormTitle">
			<tbody>
			<tr>
		  		<td bgcolor="#4D595D" valign="top">
		  			<div class="container">

		  			<div>&nbsp;</div>
		  			<div class="formfonttitle"><#menu5_1#> - <#menu5_1_6#></div>
		  			<div class="formfonttitle_help"><i onclick="show_feature_desc(`Professional options - Introduction`)" class="icon_help"></i></div>
		  			<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
		 				<div id="titl_desc" class="formfontdesc"><#WLANConfig11b_display5_sectiondesc#></div>
		 				<div id="lantiq_ready" style="display:none;color:#FC0;margin-left:5px;font-size:13px;">Wireless is setting...</div>
		 				<div id="svc_hint_div" style="display:none;margin-left:5px;"><span onClick="location.href='Advanced_System_Content.asp?af=ntp_server0'" style="color:#FFCC00;text-decoration:underline;cursor:pointer;"><#General_x_SystemTime_syncNTP#></span></div>
		  			<div id="timezone_hint_div" style="margin-left:5px;display:none;"><span id="timezone_hint" onclick="location.href='Advanced_System_Content.asp?af=time_zone_select'" style="color:#FFCC00;text-decoration:underline;cursor:pointer;"></span></div>


					<div id="schedule_block" style="display:none">
						<div id="hintBlock" style="width: 650px; margin-top: 10px;"></div>
						<div id="mainTable" style="padding:0 20px 20px 20px;"></div>
						<div id="ctrlBtn" style="text-align:center;"></div>
					</div>

					
					<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" class="FormTable" id="WAdvTable">	

					<tr id="wl_unit_field" class="rept ew">
						<th><#Interface#></th>
						<td>
							<select name="wl_unit" class="input_option" onChange="change_wl_unit();">
								<option class="content_input_fd" value="0" <% nvram_match("wl_unit", "0","selected"); %>>2.4GHz</option>
								<option class="content_input_fd" value="1"<% nvram_match("wl_unit", "1","selected"); %>>5GHz</option>
							</select>			
						</td>
					</tr>
					<tr id="wl_rf_enable">
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 1);"><#WLANConfig11b_x_RadioEnable_itemname#></a></th>
			  			<td>
			  				<input type="radio" value="1" name="wl_radio" class="input" onClick="control_TimeField();" <% nvram_match("wl_radio", "1", "checked"); %>><#checkbox_Yes#>
			    			<input type="radio" value="0" name="wl_radio" class="input" onClick="control_TimeField();" <% nvram_match("wl_radio", "0", "checked"); %>><#checkbox_No#>
			  			</td>
					</tr>

					<tr id="wl_sched_enable">
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 23);"><#WLANConfig11b_x_SchedEnable_itemname#></a></th>
			  			<td>
			  				<input type="radio" value="1" name="wl_timesched" class="input" onClick="control_TimeField();return change_common_radio(this, 'WLANConfig11b', 'wl_timesched', '1');" <% nvram_match("wl_timesched", "1", "checked"); %>><#checkbox_Yes#>
			    			<input type="radio" value="0" name="wl_timesched" class="input" onClick="control_TimeField();return change_common_radio(this, 'WLANConfig11b', 'wl_timesched', '0')" <% nvram_match("wl_timesched", "0", "checked"); %>><#checkbox_No#>
							<span id="time_setting" style="padding-left:20px;cursor:pointer;text-decoration:underline" onclick="show_wifi_schedule();"><#Time_Scheduling#></span>
						</td>
					</tr>

					<tr id="wl_ap_isolate_field">
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 5);"><#WLANConfig11b_x_IsolateAP_itemname#></a></th>
			  			<td>
							<input type="radio" value="1" name="wl_ap_isolate" class="input" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_ap_isolate', '1')" <% nvram_match("wl_ap_isolate", "1", "checked"); %>><#checkbox_Yes#>
							<input type="radio" value="0" name="wl_ap_isolate" class="input" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_ap_isolate', '0')" <% nvram_match("wl_ap_isolate", "0", "checked"); %>><#checkbox_No#>
			  			</td>
					</tr>

					<tr id="wl_bss_maxassoc_field" style="display:none;">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 35);"><#WLANConfig11b_x_client_limit_max#></a></th>
						<td>
							<input type="text" maxlength="3" name="wl_bss_maxassoc" id="wl_bss_maxassoc" class="input_3_table" value="<% nvram_get("wl_bss_maxassoc"); %>" onKeyPress="return validator.isNumber(this,event)" autocorrect="off" autocapitalize="off">
						</td>
					</tr>

					<tr id="rssiTr" class="rept ew">
		  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 31);"><#Roaming_assistant#></a></th>
						<td>
							<div style="display:table-cell;vertical-align:middle">
								<select id="wl_user_rssi_option" class="input_option" onchange="changeRSSI(this.value);">
									<option value="1"><#WLANConfig11b_WirelessCtrl_button1name#></option>
									<option value="0" <% nvram_match("wl_user_rssi", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								</select><br>
								<div id="rssiDbm" style="color:#FFF">
								<#Roaming_assistant_depart#>
			  					<input type="text" maxlength="3" name="wl_user_rssi" class="short_input input_3_table" value="<% nvram_get("wl_user_rssi"); %>" autocorrect="off" autocapitalize="off"> dBm
								</div>
							</div>
						</td>
					</tr><tr id="he_mode_field">
						<th>
								<a id="he_mode_text" class="hintstyle"><#WLANConfig11b_HE_Frame_Mode_itemname#></a>
						</th>
						<td>
							<div style="width:465px;display:flex;align-items: center;">
								<select name="wl_11ax" class="input_option" onChange="he_frame_mode(this);">
										<option value="1" <% nvram_match("wl_11ax", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
										<option value="0" <% nvram_match("wl_11ax", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								</select>
								<span id="he_mode_faq" style="padding: 0 10px"><#WLANConfig11b_HE_Frame_Mode_faq#></span>
							</div>
						</td>
					</tr>


					<tr id="wifi7_mode_field" style="display:none">
						<th>
							<a id="wifi7_mode_text" class="hintstyle"><#WiFi7_Mode#></a>
						</th>
						<td>
							<div style="width:465px;display:flex;align-items: center;">
								<select name="wl_11be" class="input_option" onChange="wifi7_mode(this);">
									<option value="1" <% nvram_match("wl_11be", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
									<option value="0" <% nvram_match("wl_11be", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								</select>
							</div>
						</td>
					</tr>
					<!-- 2.4GHz Bluetooth Coexisistence mode, only for Broadcom platform -->
					<tr>
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,34);"><#WLANConfig11b_x_BTCoexistence_itemname#></a></th>
						<td>
							<select name="wl_btc_mode" class="input_option">
									<option value="0" <% nvram_match("wl_btc_mode", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_btc_mode", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
									<option value="2" <% nvram_match("wl_btc_mode", "2","selected"); %> ><#WLANConfig11b_x_BTCoexistence_pre-emptive#></option>
							</select>
						</td>
					</tr>

					<tr>
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 22);"><#WLANConfig11b_x_IgmpSnEnable_itemname#></a></th>
						<td>
							<select name="wl_igs" class="input_option">
								<option value="1" <% nvram_match("wl_igs", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
								<option value="0" <% nvram_match("wl_igs", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
							</select>
						</td>
					</tr>
					<tr id="wl_mrate_select">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 7);"><#WLANConfig11b_MultiRateAll_itemname#></a></th>
						<td>
							<select name="wl_mrate_x" class="input_option">
								<option value="0" <% nvram_match("wl_mrate_x", "0", "selected"); %>><#Auto#></option>
							</select>
						</td>
					</tr>
					<tr id="wl_plcphdr_field">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,20);"><#WLANConfig11n_PremblesType_itemname#></a></th>
						<td>
						<select name="wl_plcphdr" class="input_option">
							<option value="long" <% nvram_match("wl_plcphdr", "long", "selected"); %>><#WLANConfig11n_PremblesType_long#></option>
							<option value="short" <% nvram_match("wl_plcphdr", "short", "selected"); %>><#WLANConfig11n_PremblesType_short#></option>
<!-- auto mode applicable for STA only
							<option value="auto" <% nvram_match("wl_plcphdr", "auto", "selected"); %>><#Auto#></option>
-->
						</select>
						</td>
					</tr>
					<tr>
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 9);"><#WLANConfig11b_x_Frag_itemname#></a></th>
			  			<td>
			  				<input type="text" maxlength="4" name="wl_frag" id="wl_frag" class="input_6_table" value="<% nvram_get("wl_frag"); %>" onKeyPress="return validator.isNumber(this,event)" autocorrect="off" autocapitalize="off">
						</td>
					</tr>
					<tr id='ampdu_rts_tr'>
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,30);">AMPDU RTS</a></th>	<!-- untranslated -->
						<td>
							<select name="wl_ampdu_rts" class="input_option" onchange="check_ampdu_rts();">
								<option value="1" <% nvram_match("wl_ampdu_rts", "1", "selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
								<option value="0" <% nvram_match("wl_ampdu_rts", "0", "selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
							</select>
						</td>
					</tr>
					<tr id="rts_threshold">
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 10);"><#WLANConfig11b_x_RTS_itemname#></a></th>
			  			<td>
			  				<input type="text" maxlength="4" name="wl_rts" class="input_6_table" value="<% nvram_get("wl_rts"); %>" onKeyPress="return validator.isNumber(this,event)" autocorrect="off" autocapitalize="off">
			  			</td>
					</tr>
					<tr id="wl_dtim_field">
			  			<th><a class="hintstyle" id="wl_dtim_th"><#WLANConfig11b_x_DTIM_itemname#></a></th>
						<td>
			  				<input type="text" maxlength="3" name="wl_dtim" class="input_6_table" value="<% nvram_get("wl_dtim"); %>" onKeyPress="return validator.isNumber(this,event)" autocorrect="off" autocapitalize="off">
						</td>			  
					</tr>
					<tr id="wl_bcn_field">
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 12);"><#WLANConfig11b_x_Beacon_itemname#></a></th>
						<td>
							<input type="text" maxlength="4" name="wl_bcn" class="input_6_table" value="<% nvram_get("wl_bcn"); %>" onKeyPress="return validator.isNumber(this,event)" autocorrect="off" autocapitalize="off">
						</td>
					</tr>
					<tr id="wl_frameburst_field">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 13);"><#WLANConfig11b_x_TxBurst_itemname#></a></th>
						<td>
							<select name="wl_frameburst" class="input_option">
								<option value="off" <% nvram_match("wl_frameburst", "off","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="on" <% nvram_match("wl_frameburst", "on","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<tr id="PktAggregate"><!-- RaLink Only -->
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3, 16);"><#WLANConfig11b_x_PktAggregate_itemname#></a></th>
						<td>
							<select name="wl_PktAggregate" class="input_option">
								<option value="0" <% nvram_match("wl_PktAggregate", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="1" <% nvram_match("wl_PktAggregate", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>

					<!-- WMM setting start  -->
					<tr>
			  			<th><a class="hintstyle"  href="javascript:void(0);" onClick="openHint(3, 14);"><#WLANConfig11b_x_WMM_itemname#></a></th>
			  			<td>
							<select name="wl_wme" id="wl_wme" class="input_option" onChange="enable_wme_check(this);">			  	  				
			  	  				<option value="auto" <% nvram_match("wl_wme", "auto", "selected"); %>><#Auto#></option>
			  	  				<option value="on" <% nvram_match("wl_wme", "on", "selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
			  	  				<option value="off" <% nvram_match("wl_wme", "off", "selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>			  	  			
							</select>
			  			</td>
					</tr>
					<tr>
			  			<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,15);"><#WLANConfig11b_x_NOACK_itemname#></a></th>
			  			<td>
							<select name="wl_wme_no_ack" id="wl_wme_no_ack" class="input_option">
			  	  				<option value="off" <% nvram_match("wl_wme_no_ack", "off","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
			  	  				<option value="on" <% nvram_match("wl_wme_no_ack", "on","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
			  			</td>
					</tr>
					<tr id="wl_wme_apsd_field">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,17);"><#WLANConfig11b_x_APSD_itemname#></a></th>
						<td>
                  				<select name="wl_wme_apsd" class="input_option">
                    					<option value="off" <% nvram_match("wl_wme_apsd", "off","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
                    					<option value="on" <% nvram_match("wl_wme_apsd", "on","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
                  				</select>
						</td>
					</tr>					
					<!-- WMM setting end  -->

					<tr id="DLSCapable" class="rept"> <!-- RaLink Only  -->
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,18);"><#WLANConfig11b_x_DLS_itemname#></a></th>
						<td>
							<select name="wl_DLSCapable" class="input_option">
								<option value="0" <% nvram_match("wl_DLSCapable", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="1" <% nvram_match("wl_DLSCapable", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>

					<tr> <!-- BRCM MIPS Only  -->
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,21);"><#WLANConfig11b_x_EnhanInter_itemname#></a></th>
						<td>
							<select name="wl_noisemitigation" class="input_option" onChange="">
								<option value="0" <% nvram_match("wl_noisemitigation", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="1" <% nvram_match("wl_noisemitigation", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					
					<tr> <!-- MODELDEP: RT-AC55U -->
						<th><a class="hintstyle" href="javascript:void(0);"><#WLANConfig11b_x_traffic_counter#></a></th>
						<td>
							<select name="traffic_5g" class="input_option">
								<option value="1" <% nvram_match("traffic_5g", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
								<option value="0" <% nvram_match("traffic_5g", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
							</select>
						</td>
					</tr>
					

					<!-- [MODELDEP] for Broadcom SDK 6.x -->
					<tr id="wl_ampdu_mpdu_field">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,26);"><#WLANConfig11b_x_AMPDU#></a></th>
						<td>
							<select name="wl_ampdu_mpdu" class="input_option">
									<option value="0" <% nvram_match("wl_ampdu_mpdu", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_ampdu_mpdu", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>					
					<tr id="wl_ack_ratio_field">
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,27);"><#WLANConfig11b_x_ACK#></a></th>
						<td>
							<select name="wl_ack_ratio" class="input_option">
									<option value="0" <% nvram_match("wl_ack_ratio", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_ack_ratio", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<tr>
						<th id="turbo_qam_title"><a id="turbo_qam_hint" class="hintstyle" href="javascript:void(0);" onClick="openHint(3,28);"><#WLANConfig11b_x_TurboQAM#></a></th>
						<td>
							<select name="wl_turbo_qam" class="input_option">
									<option value="0" <% nvram_match("wl_turbo_qam", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_turbo_qam", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<tr>
						<th><a class="hintstyle">256-QAM Broadcom interoperability</a></th>
						<td>
							<select name="wl_turbo_qam_brcm_intop" class="input_option">
									<option value="0" <% nvram_match("wl_turbo_qam_brcm_intop", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_turbo_qam_brcm_intop", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<!-- [MODELDEP] end -->
					<!--For 5GHz of RT-AC87U  -->
					<tr id="wl_80211h_tr" style="display:none;">
						<th><a class="hintstyle"><#WLANConfig11b_x_80211H#></a></th>
						<td>
							<select name="wl1_80211h" class="input_option">
									<option value="0" <% nvram_match("wl1_80211h", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl1_80211h", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					
					
					<!--Broadcom ARM platform only, except RT-AC87U(5G) -->
					<tr>
						<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(3,32);"><#WLANConfig11b_x_Airtime_Fairness#></a></th>
						<td>
							<select name="wl_atf" class="input_option">
									<option value="0" <% nvram_match("wl_atf", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_atf", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					
					<!--Multi-User MU-MIMO for RT-AC88U, RT-AX88U, RT-AC86U, GT-AC2900, RT-AC3100, RT-AC5300, GT-AX11000, RT-AX92U and RT-AC87U 5 GHz only-->
					<tr id="wl_MU_MIMO_field" style="display:none">
						<th><a class="hintstyle"><#WLANConfig11b_MUMIMO_itemdesc#></a></th>
						<td>
							<div style="display:table-cell;vertical-align:middle">
								<select name="wl_mumimo" class="input_option" onchange="handle_mimo(this.value)" disabled>
									<option value="0" <% nvram_match("wl_mumimo", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_mumimo", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
								</select>
							</div>
						</td>
					</tr>
					<tr id="ofdma_field" style="display:none">
						<th><a class="hintstyle"><#OFDMA_title#></a></th>
						<td>
							<div style="display:table-cell;vertical-align:middle">
								<select name="wl_ofdma" class="input_option">
									<option value="0" <% nvram_match("wl_ofdma", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_ofdma", "1","selected"); %>>DL OFDMA only</option>
									<option value="2" <% nvram_match("wl_ofdma", "2","selected"); %>>DL/UL OFDMA</option>
									<option value="3" <% nvram_match("wl_ofdma", "3","selected"); %>>DL/UL OFDMA + MU-MIMO</option>
								</select>
								<span id="ofdma_hint" style="margin-left:4px;display:none">*Need to enable <a href="Advanced_Wireless_Content.asp" style="color:#FC0;text-decoration:underline;">802.11ax / WiFi 6 mode</a></span>
							</div>
						</td>
					</tr>
					<tr id="wl_txbf_field">
						<th><a id="wl_txbf_desc" class="hintstyle" href="javascript:void(0);" onClick="openHint(3,24);"><#WLANConfig11b_x_ExpBeam#></a></th>
						<td>
							<select name="wl_txbf" class="input_option" onchange="handle_beamforming(this.value)">
									<option value="0" <% nvram_match("wl_txbf", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_txbf", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>					
					<tr id="wl_itxbf_field">
						<th><a id="wl_itxbf_desc" class="hintstyle" href="javascript:void(0);" onClick="openHint(3,25);"><#WLANConfig11b_x_uniBeam#></a></th>
						<td>
							<select name="wl_itxbf" class="input_option" disabled>
								<option value="0" <% nvram_match("wl_itxbf", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="1" <% nvram_match("wl_itxbf", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>					
					<!-- RT-AC82U & RT-AC58U & 4G-AC53U & MAP-AC1300 & MAP-AC2200 & VZW-AC1300 & RT-AC95U -->
					<tr id="ext_nss_field" style="display:none">
						<th><a class="hintstyle">Extended NSS</a></th>
						<td>
							<select name="wl_ext_nss" class="input_option">
								<option value="0" <% nvram_match("wl_ext_nss", "0","selected"); %>><#WLANConfig11b_WirelessCtrl_buttonname#></option>
								<option value="2" <% nvram_match("wl_ext_nss", "2","selected"); %>>4x4@80MHz</option>
								<option value="1" <% nvram_match("wl_ext_nss", "1","selected"); %>><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<tr id="wl_txPower_field">
						<th><a id="wl_txPower_field_title" class="hintstyle" href="javascript:void(0);" onClick="openHint(0, 16);"><#WLANConfig11b_TxPower_itemname#></a></th>
						<td>
							<div>
								<table>
									<tr>
										<td style="border:0px;padding-left:0px;">
											<div id="slider" style="width:80px;"></div>
										</td>									
										<td style="border:0px;width:60px;">
											<div id="tx_power_desc" style="width:150px;font-size:14px;"></div>
											<div id="tx_power_desc_EG" style="margin-top:-20px;margin-left:140px;display:none;"></div>
										</td>					

									</tr>
								</table>
							</div>
						</td>
					</tr>

					<!--QCA9984 platform only, e.g. BRT-AC828 -->
					<tr id="agiledfs_tr" style="display:none">
						<th>Agile DFS</th>
						<td>
							<select name="wl_precacen" class="input_option">
									<option value="0" <% nvram_match("wl_precacen", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_precacen", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>
					<tr>
						<th><#WLANConfig11b_x_Hardware_Offloading#></th>
						<td>
							<select name="wl_hwol" class="input_option">
									<option value="0" <% nvram_match("wl_hwol", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
									<option value="1" <% nvram_match("wl_hwol", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
							</select>
						</td>
					</tr>

					<tr id="region_tr" style="display:none" class="rept ew">
						<th><a class="hintstyle"><#WLANConfig11b_x_Region#></a></th>
						<td><div id="region_div"></div></td>
					</tr>
				</table>				
						<div class="apply_gen" id="apply_btn">
							<input class="button_gen" onclick="applyRule();" type="button" value="<#CTL_apply#>"/>
						</div>
					
				</div>	<!-- for .container  -->
				<div class="popup_container popup_element_second"></div>
		</td>
	</tr>
</tbody>
</table>		
</td>
</form>
        </tr>
      </table>
		<!--===================================Ending of Main Content===========================================-->		
	</td>
		
    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>
<div id="footer"></div>
</body>
</html>
