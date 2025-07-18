﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title><#Web_Title#> - <#menu5_6_1_title#></title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="usp_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<script type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/state.js"></script>
<script language="JavaScript" type="text/javascript" src="/general.js"></script>
<script language="JavaScript" type="text/javascript" src="/popup.js"></script>
<script language="JavaScript" type="text/javascript" src="/help.js"></script>
<script language="JavaScript" type="text/javascript" src="/validator.js"></script>
<script language="JavaScript" type="text/javascript" src="/form.js"></script>
<style type="text/css">
.contentM_qis{ 
	width:650px;
	height:475px;
	margin-top:-160px;
	margin-left:250px;
	position:absolute;
	-webkit-border-radius: 5px;
	-moz-border-radius: 5px;
	border-radius: 5px;
	z-index:200;
	background-color:#2B373B;
	box-shadow: 3px 3px 10px #000;
	display:none;
	behavior: url(/PIE.htc);
}
.QISmain{
	width:730px;
	/*font-family:Verdana, Arial, Helvetica, sans-serif;*/
	font-size:14px;
	z-index:200;
	position:relative;
	background-color:black:
}	
.QISform_wireless{
	width:600px;
	font-size:12px;
	color:#FFFFFF;
	margin-top:10px;
	*margin-left:10px;
}

.QISform_wireless thead{
	font-size:15px;
	line-height:20px;
	color:#FFFFFF;	
}

.QISform_wireless th{
	padding-left:10px;
	*padding-left:30px;
	font-size:12px;
	font-weight:bolder;
	color: #FFFFFF;
	text-align:left; 
}
               
.QISform_wireless li{	
	margin-top:10px;
}
.QISGeneralFont{
	font-family:Segoe UI, Arial, sans-serif;
	margin-top:10px;
	margin-left:70px;
	*margin-left:50px;
	margin-right:30px;
	color:white;
	LINE-HEIGHT:18px;
}	
.description_down{
	margin-top:10px;
	margin-left:10px;
	padding-left:5px;
	font-weight:bold;
	line-height:140%;
	color:#ffffff;	
}
</style>
<script>
$(function () {
	if(amesh_support && ameshRouter_support) {
		addNewScript('/require/modules/amesh.js');
	}
});

window.onresize = function() {
	if(document.getElementById("routerSSID") != null){
		if(document.getElementById("routerSSID").style.display == "block") {
			cal_panel_block("routerSSID", 0.25);
		}
	}
} 

var sw_mode_orig = '<% nvram_get("sw_mode"); %>';
var wlc_express_orig = '<% nvram_get("wlc_express"); %>';
var tcode = '<% nvram_get("territory_code"); %>';

if(isSwMode("MB"))
	sw_mode_orig = 4;
else if(isSwMode("AP"))
	sw_mode_orig = 3;
else if(isSwMode("RP"))
	sw_mode_orig = 2;
else if(isSwMode("WISP"))
	sw_mode_orig = 6;

var current_page = window.location.pathname.split("/").pop();
var faq_index_tmp = get_faq_index(FAQ_List, current_page, 1);

function initial(){
	show_menu();
	var gen_operation_mode = function(_jsonArray, _sw_mode) {
		var $spanHtml = $('<span>');
		$spanHtml.attr({"id" : _jsonArray["span"]["id"]});
		if(_jsonArray.css_list != undefined)
			$spanHtml.css(_jsonArray.css_list);
		var $inputHtml = $('<input>');
		$inputHtml.attr({ "type" : "radio", "id" : _jsonArray["input"]["id"], "name" :  _jsonArray["input"]["name"], "value" : _jsonArray["input"]["value"] });
		$inputHtml.addClass("input");
		$inputHtml.click(
			function() {
				setScenerion(_jsonArray["mode"], _jsonArray["express"]);
			}
		);
		if(_jsonArray["mode"] == _sw_mode)
			$inputHtml.prop('checked', true);
		var $labelHtml = $('<label>');
		$labelHtml.attr({ "for" : _jsonArray["input"]["id"] });
		$labelHtml.html(_jsonArray["label"]["text"]);
		$spanHtml.append($inputHtml);
		$spanHtml.append($labelHtml);
		return $spanHtml;
	};
	var operation_array = {
		"routerMode" : {
			"span" : {
				"id" : "routerMode"
			},
			"input" : {
				"id" : "sw_mode1_radio",
				"name" : "sw_mode_radio",
				"value" : "1"
			},
			"label" : {
				"text" : (amesh_support && ameshRouter_support) ? "<#AiMesh_GW_item#>" : "<#OP_GW_item#>"
			},
			"mode" : "1",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"block"}
		},
		"wispMode" : {
			"span" : {
				"id" : "wispMode"
			},
			"input" : {
				"id" : "sw_mode6_radio",
				"name" : "sw_mode_radio",
				"value" : "6"
			},
			"label" : {
				"text" : "<#OP_WISP_item#>"
			},
			"mode" : "6",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"block"}
		},
		"apMode" : {
			"span" : {
				"id" : "apMode"
			},
			"input" : {
				"id" : "sw_mode3_radio",
				"name" : "sw_mode_radio",
				"value" : "3"
			},
			"label" : {
				"text" : (amesh_support && ameshRouter_support) ? "<#AiMesh_AP_item#>" : "<#OP_AP_item#>"
			},
			"mode" : "3",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"block"}
		},
		"repeaterMode" : {
			"span" : {
				"id" : "repeaterMode"
			},
			"input" : {
				"id" : "sw_mode2_radio",
				"name" : "sw_mode_radio",
				"value" : "2"
			},
			"label" : {
				"text" : "<#OP_RE_item#>"
			},
			"mode" : "2",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"inline-block"}
		},
		"rp_express_2g" : {
			"span" : {
				"id" : "rp_express_2g"
			},
			"input" : {
				"id" : "sw_mode2_0_radio",
				"name" : "sw_mode_radio",
				"value" : "2"
			},
			"label" : {
				"text" : "<#OP_RE2G_item#>"
			},
			"mode" : "2",
			"express" : "1",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"inline-block"}
		},
		"rp_express_5g" : {
			"span" : {
				"id" : "rp_express_5g"
			},
			"input" : {
				"id" : "sw_mode2_1_radio",
				"name" : "sw_mode_radio",
				"value" : "2"
			},
			"label" : {
				"text" : "<#OP_RE5G_item#>"
			},
			"mode" : "2",
			"express" : "2",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"inline-block"}
		},
		"mbMode" : {
			"span" : {
				"id" : "mbMode"
			},
			"input" : {
				"id" : "sw_mode4_radio",
				"name" : "sw_mode_radio",
				"value" : "4"
			},
			"label" : {
				"text" : "<#OP_MB_item#>"
			},
			"mode" : "4",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"block"}
		},
		"AiMeshMode" : {
			"span" : {
				"id" : "AiMeshMode"
			},
			"input" : {
				"id" : "sw_mode5_radio",
				"name" : "sw_mode_radio",
				"value" : "5"
			},
			"label" : {
				"text" : "<#AiMesh_Node#>"
			},
			"mode" : "5",
			"express" : "0",
			"css_list" : {"margin":"0px 10px 5px 0px", "display":"block"}
		}
	}

	$("#operation_mode_bg").append(gen_operation_mode(operation_array["routerMode"], sw_mode_orig));
	$("#operation_mode_bg").append(gen_operation_mode(operation_array["apMode"], sw_mode_orig));
	$("#operation_mode_bg").append(gen_operation_mode(operation_array["repeaterMode"], sw_mode_orig));
	$("#operation_mode_bg").append(gen_operation_mode(operation_array["rp_express_2g"], sw_mode_orig));
	$("#operation_mode_bg").append(gen_operation_mode(operation_array["rp_express_5g"], sw_mode_orig));
	$("#operation_mode_bg").append(gen_operation_mode(operation_array["mbMode"], sw_mode_orig));

	if(amesh_support && ameshNode_support)
		$("#operation_mode_bg").append(gen_operation_mode(operation_array["AiMeshMode"], sw_mode_orig));

	if(isSupport("wisp"))
		$("#operation_mode_bg").append(gen_operation_mode(operation_array["wispMode"], sw_mode_orig));

	setScenerion(sw_mode_orig, document.form.wlc_express.value);

	if(downsize_4m_support || downsize_8m_support)
		document.getElementById("Senario").style.display = "none";

	document.getElementById("rp_express_2g").style.display = "none";
	document.getElementById("rp_express_5g").style.display = "none";
	if(isSupport("noRouter")){
		document.getElementById("routerMode").style.display = "none";
		document.getElementById("sw_mode1_radio").disabled = true;
	}

	if(isSupport("rp_express_2g"))
		document.getElementById("rp_express_2g").style.display = "";
	if(isSupport("rp_express_5g"))
		document.getElementById("rp_express_5g").style.display = "";

	if(!repeater_support){
		document.getElementById("repeaterMode").style.display = "none";
		document.getElementById("sw_mode2_radio").disabled = true;
	}

	if(!psta_support){
		document.getElementById("mbMode").style.display = "none";
		document.getElementById("sw_mode4_radio").disabled = true;
	}

	if(isSupport("noAP")){
		$("#apMode").hide();
		$("#sw_mode3_radio").attr("disabled", true);
	}

	if(!ameshNode_support && !repeater_support && !psta_support && isSupport("noAP")){
		$("#op_title_desc").hide();
	}
}

function restore_wl_config(prefix){
	var wl_defaults = {
		ssid: {_name: prefix+"ssid", _value: "ASUS"},
		wep: {_name: prefix+"wep_x", _value: "0"},
		key: {_name: prefix+"key", _value: "1"},
		key1: {_name: prefix+"key1", _value: ""},
		key2: {_name: prefix+"key2", _value: ""},
		key3: {_name: prefix+"key3", _value: ""},
		key4: {_name: prefix+"key4", _value: ""},
		auth_mode: {_name: prefix+"auth_mode_x", _value: "open"},
		crypto: {_name: prefix+"crypto", _value: "tkip+aes"},
		wpa_psk: {_name: prefix+"wpa_psk", _value: ""},
		nbw_cap: {_name: prefix+"nbw_cap", _value: "1"},
		bw_cap: {_name: prefix+"bw", _value: "1"},
		hwaddr: {_name: prefix+"hwaddr", _value: ""}
	}
	
	if(prefix.length == 6)
		wl_defaults.ssid._value = "ASUS_Guest" + prefix[4];

	if(prefix[2] == 1)
		wl_defaults.ssid._value = wl_defaults.ssid._value.replace("ASUS", "ASUS_5G");

	for(var i in wl_defaults) 
		set_variable(wl_defaults[i]._name, wl_defaults[i]._value);
}
function restore_wl_config_wep(prefix){
	var wl_defaults = {
		wep: {_name: prefix+"wep_x", _value: "0"},
		key: {_name: prefix+"key", _value: "1"},
		key1: {_name: prefix+"key1", _value: ""},
		key2: {_name: prefix+"key2", _value: ""},
		key3: {_name: prefix+"key3", _value: ""},
		key4: {_name: prefix+"key4", _value: ""},
		nbw_cap: {_name: prefix+"nbw_cap", _value: "1"},
		bw_cap: {_name: prefix+"bw", _value: "1"}
	}

	for(var i in wl_defaults) 
		set_variable(wl_defaults[i]._name, wl_defaults[i]._value);
}
function close_guest_unit(_unit, _subunit){
	var NewInput = document.createElement("input");
	NewInput.type = "hidden";
	NewInput.name = "wl"+ _unit + "." + _subunit +"_bss_enabled";
	NewInput.value = "0";
	document.form.appendChild(NewInput);
}

function saveMode(){
	const OP_sdn_change_mode_desc1 = `<#OP_sdn_change_mode_desc1#>`.replace(`#FUNCTIONS`, `<#AiProtection_title#>, <#vpnc_title#>, <#BOP_isp_heart_item#>, <#EzQoS_type_QoS#>`);
	const OP_sdn_change_mode_desc2 = `<#OP_sdn_change_mode_desc2#>`.replace(`#GUESTNAMING`, Guest_Network_naming);
	const OP_sdn_change_mode_desc3 = `<#OP_sdn_change_mode_desc3#>`;
	let sdn_change_mode_hint = `${OP_sdn_change_mode_desc1}\n${OP_sdn_change_mode_desc2}\n${OP_sdn_change_mode_desc3}\n<#Setting_factorydefault_hint2#>`;
	if(sw_mode_orig == document.form.sw_mode.value){
		if(document.form.sw_mode.value != 2){				
			alert("<#Web_Title2#> <#op_already_configured#>");
			return false;
		}
		else{		//Repeater, Express Way 2.4 GHz, Express Way 5 GHz
			if(wlc_express_orig == document.form.wlc_express.value){
				alert("<#Web_Title2#> <#op_already_configured#>");
				return false;
			}
		}
	}

	if(amesh_support && ameshRouter_support) {
		if(!AiMesh_confirm_msg("Operation_Mode", document.form.sw_mode.value))
			return false;
	}

	if(document.form.sw_mode.value == 2){
		if(document.form.wlc_express.value == 1)
			parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_exp2';
		else if(document.form.wlc_express.value == 2)	
			parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_exp5';
		else	
			parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_rep';
		return false;
	}
	else if(document.form.sw_mode.value == 3){
		var confirmFlag = true;
		if(isSupport("mtlancfg") && sw_mode_orig == "1"){
			confirmFlag = confirm(sdn_change_mode_hint.replace("#OPMODE", $("#apMode").find("label").html()));
		}
		if(confirmFlag){
			parent.location.href = '/QIS_wizard.htm?flag=lanip';
		}
		return false;
	}
	else if(document.form.sw_mode.value == 4){
		parent.location.href = '/QIS_wizard.htm?flag=sitesurvey_mb';
		return false;
	}
	else if(document.form.sw_mode.value == 5){
		parent.location.href = '/QIS_wizard.htm?flag=amasnode_page';
		return false;
	}
	else if(document.form.sw_mode.value == 6){
		parent.location.href = '/QIS_wizard.htm?flag=wispMode';
		return false;
	}
	else{ // default router
		let confirmFlag = true;
		if(isSupport("mtlancfg") && sw_mode_orig == "3"){
			confirmFlag = confirm(sdn_change_mode_hint.replace("#OPMODE", $("#routerMode").find("label").html()));
		}
		if(confirmFlag){
			parent.location.href = '/QIS_wizard.htm?flag=rtMode';
		}
		return false;		
	}

	applyRule();
}

function applyRule(){
	if(document.form.sw_mode.value == 1 && (sw_mode_orig == '2' || sw_mode_orig == '4')){
		if(!validator.stringSSID(document.form.wl0_ssid)){ //validate 2.4G SSID
			document.form.wl0_ssid.focus();
			return false;
		}	

		if(document.form.wl0_wpa_psk.value != ""){
			if(is_KR_sku){
				if(!validator.psk_KR(document.form.wl0_wpa_psk))           //validate 2.4G WPA2 key
                                        return false;
			}
			else{
				if(!validator.psk(document.form.wl0_wpa_psk))		//validate 2.4G WPA2 key
					return false;
			}		
		
			document.form.wl0_auth_mode_x.value = "psk2";		//
			document.form.wl0_crypto.value = "aes";
		}
		else
			document.form.wl0_auth_mode_x.value = "open";

		if(document.form.smart_connect_x.value != "1"){
			if(band5g_support){
				inputCtrl(document.form.wl1_ssid,1);
				inputCtrl(document.form.wl1_crypto,1);
				inputCtrl(document.form.wl1_wpa_psk,1);
				inputCtrl(document.form.wl1_auth_mode_x,1);
				if(!validator.stringSSID(document.form.wl1_ssid)){ //validate 5G SSID
					document.form.wl1_ssid.focus();
					return false;
				}

				if(document.form.wl1_wpa_psk.value != ""){
					if(is_KR_sku){
						if(!validator.psk_KR(document.form.wl1_wpa_psk)) //validate 2.4G WPA2 key
							return false;
					}
					else{
						if(!validator.psk(document.form.wl1_wpa_psk)) //validate 5G WPA2 key
							return false;
					}

					document.form.wl1_auth_mode_x.value = "psk2";
					document.form.wl1_crypto.value = "aes";
				}
				else
					document.form.wl1_auth_mode_x.value = "open";
			}

			if(wl_info.band5g_2_support || wl_info.band6g_support){
				inputCtrl(document.form.wl2_ssid,1);
				inputCtrl(document.form.wl2_crypto,1);
				inputCtrl(document.form.wl2_wpa_psk,1);
				inputCtrl(document.form.wl2_auth_mode_x,1);
				if(!validator.stringSSID(document.form.wl2_ssid)){ //validate 5G-2 SSID
					document.form.wl2_ssid.focus();
					return false;
				}

				if(document.form.wl2_wpa_psk.value != ""){
					if(is_KR_sku){
						if(!validator.psk_KR(document.form.wl2_wpa_psk)) //validate 2.4G WPA2 key
							return false;
					}
					else{
						if(!validator.psk(document.form.wl2_wpa_psk)) //validate 5G-2 WPA2 key
							return false;
					}

					document.form.wl1_auth_mode_x.value = "psk2";
					document.form.wl1_crypto.value = "aes";
				}
				else
					document.form.wl1_auth_mode_x.value = "open";
			}
		}
	}

	showLoading();	
	document.form.target="hidden_frame";
	document.getElementById("forSSID_bg").style.visibility = "hidden";
	document.form.submit();	
}

function done_validating(action){
	refreshpage();
}

var id_WANunplungHint;
/*
Router:	             sw_mode: 1, wlc_express: 0, wlc_psta: 0
Repeater:            sw_mode: 2, wlc_express: 0, wlc_psta: 0
Express Way 2.4 GHz: sw_mode: 2, wlc_express: 1, wlc_psta: 0
Express Way 5 GHz:   sw_mode: 2, wlc_express: 2, wlc_psta: 0
AP mode:             sw_mode: 3, wlc_express: 0, wlc_psta: 0
Media Bridge:        sw_mode: 3, wlc_express: 0, wlc_psta: 1
*/

function setScenerion(mode, express){
	if(mode == '2'){
		document.form.sw_mode.value = 2;
		var url = "/images/New_ui/re.jpg";
		var height = "";
		if(odmpid == "RT-AC66U_B1" || odmpid == "RT-AC1750_B1" || odmpid == "RT-N66U_C1" || odmpid == "RT-AC1900U" || odmpid == "RT-AC67U")
			url = "/images/RT-AC66U_V2/re.jpg";
		else if(odmpid == "RP-AC1900") {
			url = "/images/RP-AC1900/re.jpg";
			height = "180px";
		}
		else if(based_modelid == "RP-AC53" && tcode == "UK/01")
			url = "/images/New_ui/re_UK.jpg";
		$("#Senario").css({"height": height, "background": "url(" + url + ") center no-repeat", "margin": "auto", "margin-bottom": "30px"});

		clearTimeout(id_WANunplungHint);
		$("#Unplug-hint").css("display", "none");
		if(express == 1){	//Express Way 2.4 GHz
			$("#sw_mode2_0_radio").prop('checked', true);
			document.form.wlc_express.value = 1;
			$("#mode_desc").html("<#OP_RE2G_desc#><br/><span style=\"color:#FC0\"><#deviceDiscorvy2#></span>");
		}
		else if(express == 2){	//Express Way 5 GHz
			$("#sw_mode2_1_radio").prop('checked', true);
			document.form.wlc_express.value = 2;
			$("#mode_desc").html("<#OP_RE5G_desc#><br/><span style=\"color:#FC0\"><#deviceDiscorvy2#></span>");
		}	
		else{		// Repeater
			$("#sw_mode2_radio").prop('checked', true);
			document.form.wlc_express.value = 0;
			$("#mode_desc").html("<#OP_RE_desc#><br/><span style=\"color:#FC0\"><#deviceDiscorvy2#></span>");
		}
	}
	else if(mode == '3'){		// AP mode
		document.form.sw_mode.value = 3;
		document.form.wlc_express.value = 0;
		var url = "/images/New_ui/ap.jpg";
		var height = "";
		if(odmpid == "RT-AC66U_B1" || odmpid == "RT-AC1750_B1" || odmpid == "RT-N66U_C1" || odmpid == "RT-AC1900U" || odmpid == "RT-AC67U")
			url = "/images/RT-AC66U_V2/ap.jpg";
		else if(odmpid == "RP-AC1900") {
			url = "/images/RP-AC1900/ap.jpg";
			height = "180px";
		}
		else if(based_modelid == "RP-AC53" && tcode == "UK/01")
			url = "/images/New_ui/ap_UK.jpg";

		$("#Senario").css({"height": height, "background": "url(" + url + ") center no-repeat", "margin": "auto", "margin-bottom": "30px"});
		/*if(findasus_support){
			$("#mode_desc").html("<#OP_AP_desc#><br/><span style=\"color:#FC0\"><#OP_AP_hint#></span>");
		}else{*/
			var desc = "";
			if(amesh_support && ameshRouter_support) {
				desc += "<#AiMesh_AP_desc#>";
				desc += "<br>";
				desc += "<#AiMesh_Node_Add#>";
				desc += "<br>";
				desc += "<span style=\"color:#FC0\"><#AiMesh_deviceDiscorvy3#></span>";
			}
			else
				desc = "<#OP_AP_desc#><br/><span style=\"color:#FC0\"><#deviceDiscorvy3#></span>";
			$("#mode_desc").html(desc);
		//}
		clearTimeout(id_WANunplungHint);
		$("#Unplug-hint").css("display", "none");
		$("input[name=sw_mode_radio][value=3]").prop('checked', true);
	}
	else if(mode == '4'){		// Media Bridge
		document.form.sw_mode.value = 4;
		document.form.wlc_express.value = 0;
		var pstaDesc =  "<#OP_MB_desc1#>";
			pstaDesc += "<#OP_MB_desc2#>";
			pstaDesc += "<#OP_MB_desc3#>";
			pstaDesc += "<#OP_MB_desc4#>";
			pstaDesc += "<#OP_MB_desc5#>";
			pstaDesc += "<br><#OP_MB_desc6#>";
			pstaDesc += "<br/><span style=\"color:#FC0\"><#deviceDiscorvy4#></span>";

		var url = "/images/New_ui/mb.jpg";
		var height = "305px";
		if(odmpid == "RT-AC66U_B1" || odmpid == "RT-AC1750_B1" || odmpid == "RT-N66U_C1" || odmpid == "RT-AC1900U" || odmpid == "RT-AC67U")
			url = "/images/RT-AC66U_V2/mb.jpg";
		else if(odmpid == "RP-AC1900")
			url = "/images/RP-AC1900/mb.jpg";
		else if(based_modelid == "RP-AC53" && tcode == "UK/01")
			url = "/images/New_ui/mb_UK.jpg";
		$("#Senario").css({"height": height, "background": "url(" + url + ") center no-repeat", "margin": "auto", "margin-bottom": "0px"});
		if(!band5g_11ac_support || no_vht_support){			
			pstaDesc = replaceAll(pstaDesc, " 802\.11ac","");
			pstaDesc = replaceAll(pstaDesc, " 802\.11AC","");
		}
		$("#mode_desc").html(pstaDesc);
		clearTimeout(id_WANunplungHint);
		$("#Unplug-hint").css("display", "none");
		$("input[name=sw_mode_radio][value=4]").prop('checked', true);
	}
	else if(mode == '5') {
		document.form.sw_mode.value = 5;
		$("#Senario").css({"height": "400px", "background": "url(/images/New_ui/amesh/house_final_dea.png) center no-repeat", "margin": "auto", "margin-bottom": "30px"});
		var desc = "<#AiMesh_Node_desc#>";
		desc += "<br>";
		desc += "1. <#AiMesh_Node_desc1#>";
		desc += "<br>";
		desc += "2. <#AiMesh_Node_desc2#>";
		$("#mode_desc").html(desc);
		$("input[name=sw_mode_radio][value=5]").prop('checked', true);
	}
	else if(mode == '6') {
		document.form.sw_mode.value = 6;
		
		$("#Senario").css({
			"height": "305px", 
			"background": "url(/images/New_ui/re.jpg) center no-repeat", 
			"margin": "auto", 
			"margin-bottom": "0px"
		});

		var desc = "<#OP_WISP_desc1#>";
		desc += "<br>";
		desc += "<#OP_WISP_desc2#>";

		$("#mode_desc").html(desc);
		$("input[name=sw_mode_radio][value=6]").prop('checked', true);
	}
	else{ // Default: Router
		document.form.sw_mode.value = 1;
		document.form.wlc_express.value = 0;
		
		if(odmpid == "RT-AC66U_B1" || odmpid == "RT-AC1750_B1" || odmpid == "RT-N66U_C1" || odmpid == "RT-AC1900U" || odmpid == "RT-AC67U")
			$("#Senario").css({"height": "", "background": "url(/images/RT-AC66U_V2/rt.jpg) center no-repeat", "margin": "auto", "margin-bottom": "30px"});
		else
			$("#Senario").css({"height": "", "background": "url(/images/New_ui/rt.jpg) center no-repeat", "margin": "auto", "margin-bottom": "30px"});
		var desc = "";
		if(amesh_support && ameshRouter_support) {
			desc += "<#AiMesh_GW_desc#>";
			desc += "<br>";
			desc += "<#AiMesh_Node_Add#>";
		}
		else
			desc = "<#OP_GW_desc#>";
		$("#mode_desc").html(desc);
		$("input[name=sw_mode_radio][value=1]").prop('checked', true);
	}
}

function Sync_2ghz(band){
	if(band == 2){
		if(document.form.sync_with_2ghz.checked == true){
			document.form.wl1_wpa_psk.value = document.form.wl0_wpa_psk.value;
			document.form.wl1_ssid.value = document.form.wl0_ssid.value + "_5G";
		}
	}
	else
		document.form.sync_with_2ghz.checked = false;
}

function Sync_5ghz(band){
	if(band == 2){
		if(document.form.sync_with_5ghz.checked == true){
			document.form.wl2_wpa_psk.value = document.form.wl1_wpa_psk.value;
			document.form.wl2_ssid.value = document.form.wl1_ssid.value + "-2";
		}
	}
	else if (band == 'smartcon'){
			document.form.wl2_ssid.value = document.form.wl1_ssid.value; 
			document.form.wl2_wpa_psk.value = document.form.wl1_wpa_psk.value; 				
	}
	else
		document.form.sync_with_5ghz.checked = false;
}

function cancel_SSID_Block(){
	$("#routerSSID").fadeOut(300);
	$("#forSSID_bg").fadeOut(300);
	//document.getElementById("hiddenMask").style.visibility = "hidden";
	//document.getElementById("forSSID_bg").style.visibility = "hidden";
}

function change_smart_con(v){
	if(v == '1'){
		document.getElementById("wl_unit_field_1").style.display = "none";
		document.getElementById("wl_unit_field_2").style.display = "none";
		document.getElementById("wl_unit_field_3").style.display = "none";
		document.getElementById("wl_unit_field_4").style.display = "none";
		document.getElementById("wl_unit_field_5").style.display = "none";
		document.getElementById("wl_unit_field_6").style.display = "none";
		document.getElementById("wl0_desc_name").innerHTML = "Tri-band Smart Connect Security";
		document.getElementById("wl0_desc_name").style = "padding-bottom:10px";
		document.getElementById("wl0_ssid").onkeyup = function(){Sync_2ghz('Tri-band');};
		document.getElementById("wl0_wpa_psk").onkeyup = function(){Sync_2ghz('Tri-band');};
		document.getElementById('routerSSID').style.height="450px";
		document.form.smart_connect_x.value = 1;
	}else if (v == '0'){
		document.getElementById("wl_unit_field_1").style.display = "";
		document.getElementById("wl_unit_field_2").style.display = "";
		document.getElementById("wl_unit_field_3").style.display = "";
		document.getElementById("wl_unit_field_4").style.display = "";
		document.getElementById("wl_unit_field_5").style.display = "";
		document.getElementById("wl_unit_field_6").style.display = "";
		document.getElementById("wl0_desc_name").innerHTML = "2.4 GHz - <#Security#>";
		document.getElementById("wl0_ssid").onkeyup = function(){Sync_2ghz(2);};
		document.getElementById("wl0_wpa_psk").onkeyup = function(){Sync_2ghz(2);};
		document.getElementById('routerSSID').style.height="620px";
		document.form.smart_connect_x.value = 0;
	}
}
</script>
</head>
<body onload="initial();" onunLoad="return unload_body();" class="bg">
<div id="TopBanner"></div>
<div id="hiddenMask" class="popup_bg">
<table cellpadding="4" cellspacing="0" id="dr_sweet_advise" class="dr_sweet_advise" align="center">
		<tr>
		<td>
			<div class="drword" id="drword"><#Main_alert_proceeding_desc4#> <#Main_alert_proceeding_desc1#>...
				<br/>
				<br/>
		    </div>
		  <div class="drImg"><img src="images/alertImg.png"></div>
			<div style="height:70px; "></div>
		</td>
		</tr>
	</table>
<!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]-->
</div>
<div id="forSSID_bg" class="popup_bg" style="z-index: 99;"></div>
<div id="Loading" class="popup_bg"></div>
<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" id="ruleForm" action="/start_apply.htm">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="restart_all">
<input type="hidden" name="action_wait" value="<% get_default_reboot_time(); %>">
<input type="hidden" name="prev_page" value="Advanced_OperationMode_Content.asp">
<input type="hidden" name="current_page" value="Advanced_OperationMode_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="flag" value="">
<input type="hidden" name="sw_mode" value="<% nvram_get("sw_mode"); %>">
<input type="hidden" name="lan_proto" value="<% nvram_get("lan_proto"); %>">
<input type="hidden" name="lan_ipaddr" value="<% nvram_get("lan_ipaddr"); %>">
<input type="hidden" name="lan_netmask" value="<% nvram_get("lan_netmask"); %>">
<input type="hidden" name="lan_gateway" value="<% nvram_get("lan_gateway"); %>">
<input type="hidden" name="lan_ipaddr_rt" value="<% nvram_get("lan_ipaddr_rt"); %>">
<input type="hidden" name="lan_netmask_rt" value="<% nvram_get("lan_netmask_rt"); %>">
<input type="hidden" name="wl0_auth_mode_x" value="<% nvram_get("wl0_auth_mode_x"); %>" disabled="disabled">
<input type="hidden" name="wl0_crypto" value="<% nvram_get("wl0_crypto"); %>" disabled="disabled">
<input type="hidden" name="wl1_auth_mode_x" value="<% nvram_get("wl1_auth_mode_x"); %>" disabled="disabled">
<input type="hidden" name="wl1_crypto" value="<% nvram_get("wl1_crypto"); %>" disabled="disabled">
<input type="hidden" name="wl2_auth_mode_x" value="<% nvram_get("wl2_auth_mode_x"); %>" disabled="disabled">
<input type="hidden" name="wl2_crypto" value="<% nvram_get("wl2_crypto"); %>" disabled="disabled">
<input type="hidden" name="smart_connect_x" value="<% nvram_get("smart_connect_x"); %>">
<input type="hidden" name="w_Setting" value="1">
<!-- AC66U's repeater mode -->
<input type="hidden" name="wlc_psta" value="<% nvram_get("wlc_psta"); %>" disabled>
<input type="hidden" name="wlc_express" value="<% nvram_get("wlc_express"); %>" disabled>
<!-- AiMesh -->
<input type="hidden" name="cfg_master" value="<% nvram_get("cfg_master"); %>" disabled>

<!-- Input SSID and Password block for switching Repeater to Router mode -->
<div id="routerSSID" class="contentM_qis">
	<table id="smart_connect_table" style="display:none;" class="QISSmartCon_table">
		<tr>
			<td width="200px">

			<div id="smart_connect_image" style="background: url(/images/New_ui/smart_connect.png); width: 130px; height: 87px; margin-left:115px; margin-top:20px; no-repeat;"></div>
			</td>
			<td>			
				<table style="margin-left:30px; margin-top:20px;">
						<td style="font-style:normal;font-size:13px;font-weight:bold;" >
							<input type="radio" value="1" id="smart_connect_t" name="smart_connect_t" class="input" onclick="return change_smart_con(this.value)" <% nvram_match("smart_connect_x", "1", "checked"); %>><#triband_smart_connect#>
						</td>
					</tr>
					<tr>
						<td style="font-style:normal;font-size:13px;font-weight:bold;">
							<input type="radio" value="0" name="smart_connect_t" class="input" onclick="return change_smart_con(this.value)" <% nvram_match("smart_connect_x", "0", "checked"); %>>Standard Setup
						</td>
					</tr>									
				</table>
			</td>
		</tr>
	</table>
	<table class="QISform_wireless" width="400" border=0 align="center" cellpadding="5" cellspacing="0">
		<tr>
			<div class="description_down"><#QKSet_wireless_webtitle#></div>
		</tr>
		<tr>
			<div class="QISGeneralFont" align="left"><#qis_wireless_setting#></div>
		</tr>
		<tr>
			<div style="width: 640px; *width: 640px; height: 2px;margin:5px;*margin-left:-5px;" class="splitLine"></div>
		</tr>
		<tr>
			<th width="180" id="wl0_desc_name">2.4GHz - <#Security#></th>
			<td class="QISformtd"></td>
		</tr>
		<tr>
			<th width="180">
				<span onmouseout="return nd();" onclick="openHint(0, 22);" style="cursor:help;"><#QIS_finish_wireless_item1#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span>
			</th>
			<td class="QISformtd">
				<input type="text" id="wl0_ssid" name="wl0_ssid" onkeypress="return validator.isString(this, event);" onkeyup="Sync_2ghz(2);" style="height:25px;" class="input_32_table" maxlength="32" value="" disabled="disabled" autocorrect="off" autocapitalize="off">
			</td>
		</tr>
		<tr id="wl_unit_field_0">
			<th width="180">
				<span onmouseout="return nd();" onclick="openHint(0, 23);" style="cursor:help;"><#Network_key#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span>		
			</th>
			<td class="QISformtd">
				<input id="wl0_wpa_psk" name="wl0_wpa_psk" type="password" onBlur="switchType(this,false);" onFocus="switchType(this,true);" value="" onkeyup="Sync_2ghz(2);" style="height:25px;" class="input_32_table" maxlength="65" disabled="disabled" autocorrect="off" autocapitalize="off">
			</td>
		</tr>
		<tr id="wl_unit_field_1">
			<th id="wl_unit_field_1_1" width="180">5GHz - <#Security#> </th>
			<td class="QISformtd">
				<input type="checkbox" id="sync_with_2ghz" name="sync_with_2ghz" class="input" onclick="setTimeout('Sync_2ghz(2);',0);" checked="checked"><span id="syncCheckbox"><#qis_ssid_desc#></span>
			</td>
		</tr>
		<tr id="wl_unit_field_2">
			<th width="180">
				<span onmouseout="return nd();" onclick="openHint(0, 22);" style="cursor:help;"><#QIS_finish_wireless_item1#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span>
			</th>
			<td class="QISformtd">
				<input type="text" id="wl1_ssid" name="wl1_ssid" onkeypress="return validator.isString(this, event);" onkeyup="Sync_2ghz(5);" style="height:25px;" class="input_32_table" maxlength="32" value="" disabled="disabled" autocorrect="off" autocapitalize="off">
			</td>
		</tr>
		<tr id="wl_unit_field_3">
			<th width="180">
				<span onmouseout="return nd();" onclick="openHint(0, 23);" style="cursor:help;"><#Network_key#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span>
			</th>
			<td class="QISformtd">
				<input id="wl1_wpa_psk" name="wl1_wpa_psk" type="password" onBlur="switchType(this,false);" onFocus="switchType(this,true);" value="" onkeyup="Sync_2ghz(5);" style="height:25px;" class="input_32_table" maxlength="65" disabled="disabled" autocorrect="off" autocapitalize="off">
			</td>
		</tr>
		<tr id="wl_unit_field_4" style="display:none;">
			<th id="5g2_title" width="180">5 GHz-2 - <#Security#> </th>
			<td class="QISformtd" id="wl_unit_field_4_2">
				<input type="checkbox" id="sync_with_5ghz" name="sync_with_5ghz" tabindex="8" class="input" onclick="setTimeout('Sync_5ghz(2);',0);" checked="checked"><span id="syncCheckbox_5_2"><#qis_ssid_desc#></span>
			</td>
		</tr>
		<tr id="wl_unit_field_5" style="display:none;">
			<th width="180"><span onmouseout="return nd();" onclick="openHint(0, 22);" style="cursor:help;"><#QIS_finish_wireless_item1#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span></th>
			<td class="QISformtd">
				<input type="text" id="wl2_ssid" name="wl2_ssid" tabindex="9" onkeypress="return validator.isString(this, event);" onkeyup="Sync_5ghz(5);" style="height:25px;" class="input_32_table" maxlength="32" value="" disabled="disabled" autocorrect="off" autocapitalize="off"/>
			</td>
		</tr>
		<tr id="wl_unit_field_6" style="display:none;">
			<th width="180"><span onmouseout="return nd();" onclick="openHint(0, 23);" style="cursor:help;"><#Network_key#><img align="right" style="cursor:pointer;margin-top:-14px\9;" src="/images/New_ui/helpicon.png"></span></th>
			<td class="QISformtd">
				<input id="wl2_wpa_psk" name="wl2_wpa_psk" tabindex="10" type="password" onBlur="switchType(this, false);" onFocus="switchType(this, true);" value="" disabled="disabled" onkeyup="Sync_5ghz(5);" style="height:25px;" class="input_32_table" maxlength="64" autocorrect="off" autocapitalize="off">
			</td>
		</tr>	
		<tr>
			<td colspan="2">
				<div class="QISGeneralFont" ><#qis_wireless_setting_skdesc#></div>
			</td>
		</tr>
	</table>
	<div style="margin-top:20px;text-align:center;background-color:#2B373B">
		<input type="button"  value="<#CTL_Cancel#>" class="button_gen" onclick="cancel_SSID_Block();">
		<input type="button" id="btn_next_step" value="<#CTL_apply#>" class="button_gen" onclick="applyRule();">
	</div>
</div>
<!-- End input SSID and Password block for switching Repeater to Router mode -->
<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="17">&nbsp;</td>
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div>
		<!--===================================Beginning of Main Content===========================================-->
		<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
			<tr>
				<td valign="top">

					<table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
						<tr bgcolor="#4D595D" valign="top">
							<td>
							<div class="container">

								<div>&nbsp;</div>
								<div class="formfonttitle"><#menu5_6#> - <#menu5_6_1_title#></div>
								<div class="formfonttitle_help"><i onclick="show_feature_desc(`Introduction of Operation Mode`)" class="icon_help"></i></div>
								<div style="margin:10px 0 10px 5px;" class="splitLine"></div>
								<div class="formfontdesc" id="op_title_desc"><#OP_desc1#></div>
							</div>	<!-- for .container  -->
							</td>
						</tr>
						<tr bgcolor="#4D595D" valign="top" style="height:15%">
							<td>
								<div class="container">
								<div style="width:95%; margin:0 auto; padding-bottom:3px;">
									<span style="font-size:16px; font-weight:bold;color:white;">
										<div id="operation_mode_bg"></div>
									</span>
									<br/>
									<span style="word-wrap:break-word;word-break:break-all"><div class="formfontdesc" id="mode_desc"></div></span>
								</div>
								</div>	<!-- for .container  -->
							</td>
						</tr>
						<tr bgcolor="#4D595D" valign="top" style="height:70%">
                 			<td>
                 				<div class="container">
							    <div id="Senario" >
								<div id="Unplug-hint" style="border:2px solid red; background-color:#FFF; padding:3px;margin:0px 0px 0px 150px;width:250px; position:absolute; display:block; display:none;"><#web_redirect_suggestion1#></div>
						         	</div>	
								<div class="apply_gen">
									<input name="button" type="button" class="button_gen" onClick="saveMode();" value="<#CTL_onlysave#>">
								</div>

								</div>	<!-- for .container  -->
								<div class="popup_container popup_element_second"></div>
							</td>
						</tr>
					</table>

				</td>
			</tr>
		</table>
			<!--===================================Ending of Main Content===========================================-->
		</td>
		<td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

</form>
<div id="footer"></div>
</body>
</html>
