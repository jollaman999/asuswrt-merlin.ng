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
<title><#Web_Title#> - <#Bandwidth_monitor_WANLAN#></title>
<link rel="stylesheet" type="text/css" href="index_style.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="usp_style.css">
<link rel="stylesheet" type="text/css" href="/device-map/device-map.css" />
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/client_function.js"></script>
<script type="text/javascript" src="/calendar/jquery-ui.js"></script>
<script type="text/javascript" src="/switcherplugin/jquery.iphone-switch.js"></script>
<script type="text/javascript" src="/form.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/asus_policy.js?v=4"></script>
<style type="text/css">
.appIcons{
	width:36px;
	height:36px;
	background-image:url('images/New_ui/app_icons/3-144-0.png');
	background-repeat:no-repeat;
	background-position:50% 45%;
	background-size:100%;
	border: 2px solid #282E30;
	border-radius:7px;
	margin-left:45px;
}
#sortable div table tr:hover{
	cursor: pointer;
	color: #000;
	background-color: #66777D;
	font-weight: bolder;
}
#sortable div table{
	font-family:Verdana;
	width: 100%;
	border-spacing: 0px;
}
.qosLevel, .qosLevel3{
	box-shadow: 0px 0px 0px 2px #7A797A;
}
.qosLevel0{
	box-shadow: 0px 0px 0px 2px #F01F09;
}
.qosLevel1{
	box-shadow: 0px 0px 0px 2px #F08C09;
}
.qosLevel2{
	box-shadow: 0px 0px 0px 2px #F3DD09;
}
.qosLevel4{
	box-shadow: 0px 0px 0px 2px #58CCED;
}

#indicator_upload, #indicator_download{
	-webkit-transform:rotate(-123deg);
    -moz-transform:rotate(-123deg);
    -o-transform:rotate(-123deg);
    msTransform:rotate(-123deg);
    transform:rotate(-123deg);
}
.divUserIcon{
	cursor: pointer;
	margin: 0 auto;
	width: 50px;
	height: 50px;
	-webkit-border-radius: 10px;
	-moz-border-radius: 10px;
	border-radius: 10px;
}
.traffic_bar{
	width: 0%;
	background-color: #08FFF0;
	height: 8px;
	border-radius:5px;
}
.traffic_bar_boost{
	background-color: #B71010;
}

.transition_style{
	-webkit-transition: all 2s ease-in-out;
	-moz-transition: all 2s ease-in-out;
	-o-transition: all 2s ease-in-out;
	transition: all 2s ease-in-out;
}
.boost_tag_BM {
	font-weight: normal;
	text-align: center;
	width: 55px;
	height: 25px;
	line-height: 25px;
	border-radius: 15%;
	margin-top: -21px;
	margin-left: 25px;
	color: #FFFFFF;
	font-size: 9px;
	-webkit-transform: scale(0.75);
	background: #FF4848; /* Old browsers */
	background: -moz-linear-gradient(top, #A21717 0%, #B71010 50%, #FF4848 100%); /* FF3.6+ */
	background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#A21717), color-stop(50%,#B71010), color-stop(100%,#FF4848)); /* Chrome,Safari4+ */
	background: -webkit-linear-gradient(top, #A21717 0%,#B71010 50%,#FF4848 100%); /* Chrome10+,Safari5.1+ */
	background: -o-linear-gradient(top, #A21717 0%,#B71010 50%,#FF4848 100%); /* Opera 11.10+ */
	background: -ms-linear-gradient(top, #A21717 0%,#B71010 50%,#FF4848 100%); /* IE10+ */
	background: linear-gradient(to bottom, #A21717 0%,#B71010 70%,#FF4848 100%); /* W3C */
	filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#A21717', endColorstr='#FF4848',GradientType=0 ); /* IE6-9 */
}
</style>
<script>
// disable auto log out
AUTOLOGOUT_MAX_MINUTE = 0;
var detect_interval = 2;	// get information per second
var qos_rulelist = "<% nvram_get("qos_rulelist"); %>".replace(/&#62/g, ">").replace(/&#60/g, "<");
var curState = '<% nvram_get("apps_analysis"); %>';
if(dns_dpi_support)
{
  curState = '<% nvram_get("dns_dpi_apps_analysis"); %>';
}
var ctf_disable = '<% nvram_get("ctf_disable"); %>';
var ctf_fa_mode = '<% nvram_get("ctf_fa_mode"); %>';
var bwdpi_app_rulelist = "<% nvram_get("bwdpi_app_rulelist"); %>".replace(/&#60/g, "<");
var qos_enable = "<% nvram_get("qos_enable"); %>";
var qos_type = "<% nvram_get("qos_type"); %>";
var enable_GameBoost = false;
if((qos_enable == '1') && (qos_type == '1') && (bwdpi_app_rulelist.indexOf('game') != -1)){
	enable_GameBoost = true;
}
else{
	enable_GameBoost = false;
}


function register_event(){
	var color_array = ["#F01F09", "#F08C09", "#F3DD09", "#7A797A", "#58CCED", "inherit"];
	$(function() {
		$("#sortable").sortable();
		$("#sortable").disableSelection();
		$("#0,#1,#2,#3,#4,#5").draggable({helper:"clone",revert:true,revertDuration:10});
		$("[id^='icon_tr_']").droppable({
			out: function(){
				this.style.color = "";
				this.style.backgroundColor = "";
				this.style.fontWeight = "";
			},
			over: function(){
				this.style.color = "#000";
				this.style.backgroundColor = "#66777D";
				this.style.fontWeight = "bolder";
			},
			drop: function(event,ui){
				this.style.color = "";
				this.style.backgroundColor = "";
				this.style.fontWeight = "";
				this.children[0].children[0].style.boxShadow = "0px 0px 0px 2px " + color_array[ui.draggable[0].id] + "";
				regen_qos_rule(this.children[0].children[0], ui.draggable[0].id);
			}
		});
	});
}

function initial(){
	show_menu();
	var faq_href = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=Faq&lang="+ui_lang+"&kw=&num=102";
	document.getElementById("faq").href = faq_href;
	show_clients();
}

var download_maximum = 100 * 1024;
var upload_maximum = 100 * 1024;
function redraw_unit(){
	var upload_node = document.getElementById('upload_unit').children;
	var download_node = document.getElementById('download_unit').children;
	var upload = upload_maximum/1024;
	var download = download_maximum/1024;

	for(i=0;i<5;i++){   // length could be document.getElementById('upload_unit').children.length -2
		if(i == 4){
			if(upload < 5){
				if(upload > parseInt(upload)){
					if(upload > (parseInt(upload) + 0.5))
						upload_node[i].innerHTML =  parseInt(upload) + 1;
					else
						upload_node[i].innerHTML = parseInt(upload) + 0.5;
				}
				else
					upload_node[i].innerHTML = parseInt(upload);
			}
			else{
				upload_node[i].innerHTML = Math.ceil(upload);
			}

			if(download < 5){
				if(download < parseInt(download)){
					if(download > (parseInt(download + 0.5)))
						download_node[i].innerHTML = parseInt(download) + 1;
					else
						download_node[i].innerHTML = parseInt(download) + 0.5;
				}
				else
					download_node[i].innerHTML =  parseInt(download);
			}
			else{
				download_node[i].innerHTML = Math.ceil(download);
			}
		}
		else{
			if(upload < 5){
				upload_node[i].innerHTML =(upload*(i+1)/5).toFixed(1);
			}
			else{
				upload_node[i].innerHTML = Math.round(upload*(i+1)/5);
			}

			if(download < 5){
				download_node[i].innerHTML = (download*(i+1)/5).toFixed(1);
			}
			else{
				download_node[i].innerHTML = Math.round(download*(i+1)/5);
			}
		}
	}
}

// for speedmeter
router_traffic_old = new Array();
function calculate_router_traffic(traffic){
	router_traffic_new = new Array();
	router_traffic_new = traffic;
	var tx = 0;
	var rx = 0;

	if(!router_traffic_old){
		router_traffic_old = [0, 0];
	}

	if((router_traffic_new[0] - router_traffic_old[0]) < 0){		// to control overflow issue
		//tx = (parseInt(router_traffic_new[0]) + Math.pow(2,32)) - router_traffic_old[0];
	}
	else{
		tx = router_traffic_new[0] - router_traffic_old[0];
	}

	if((router_traffic_new[1] - router_traffic_old[1]) < 0){
		//rx = (parseInt(router_traffic_new[1]) + Math.pow(2,32)) - router_traffic_old[1];
	}
	else{
		rx = router_traffic_new[1] - router_traffic_old[1];
	}

	tx = tx*8/detect_interval;		// translate to bits
	rx = rx*8/detect_interval;
	var tx_kb = tx/1024;
	var rx_kb = rx/1024;
	var tx_mb = tx/1024/1024;
	var rx_mb = rx/1024/1024;
	var angle = 0;
	var rotate = "";
/* angle mapping table: [0M: -123deg, 1M: -90deg, 5M: -58deg, 10M: -33deg, 20M: -1deg, 30M: 30deg, 50M: 58deg, 75M: 88deg, 100M: 122deg]*/

	if(router_traffic_old.length != 0){
		//angle = (tx_mb - lower unit)/(upper unit - lower unit)*(degree in the range) + (degree of previous scale) + (degree of 0M)
		document.getElementById('upload_speed').innerHTML = tx_mb.toFixed(2);
		if(tx_mb <= 1){
			angle = (tx_mb*33) + (-123);
		}
		else if(tx_mb > 1 && tx_mb <= 5){
			angle = ((tx_mb-1)/4)*32 + 33 +(-123);

		}
		else if(tx_mb > 5 && tx_mb <= 10){
			angle = ((tx_mb - 5)/5)*25 + (33 + 32) + (-123);
		}
		else if(tx_mb > 10 && tx_mb <= 20){
			angle = ((tx_mb - 10)/10)*32 + (33 + 32 + 25) + (-123)
		}
		else if(tx_mb > 20 && tx_mb <= 30){
			angle = ((tx_mb - 20)/10)*31 + (33 + 32 + 25 + 32) + (-123);
		}
		else if(tx_mb > 30 && tx_mb <= 50){
			angle = ((tx_mb - 30)/20)*28 + (33 + 32 + 25 + 32 + 31) + (-123);
		}
		else if(tx_mb > 50 && tx_mb <= 75){
			angle = ((tx_mb - 50)/25)*30 + (33 + 32 + 25 + 32 + 31 + 28) + (-123);
		}
		else if(tx_mb >75 && tx_mb <= 100){
			angle = ((tx_mb - 75)/25)*34 + 	(33 + 32 + 25 + 32 + 31 + 28 + 30) + (-123);
		}
		else{		// exception case temporally, upload traffic exceed 100Mb.
			angle = 123;
		}

		rotate = "rotate("+angle.toFixed(1)+"deg)";
		$('#indicator_upload').css({
			"-webkit-transform": rotate,
			"-moz-transform": rotate,
			"-o-transform": rotate,
			"msTransform": rotate,
			"transform": rotate
		});

		//angle = (rx_mb - lower unit)/(upper unit - lower unit)*(degree in the range) + (degree of previous scale) + (degree of 0M)
		document.getElementById('download_speed').innerHTML = rx_mb.toFixed(2);
		if(rx_mb <= 1){
			angle = (rx_mb*33) + (-123);
		}
		else if(rx_mb > 1 && rx_mb <= 5){
			angle = ((rx_mb-1)/4)*32 + 33 +(-123);

		}
		else if(rx_mb > 5 && rx_mb <= 10){
			angle = ((rx_mb - 5)/5)*25 + (33 + 32) + (-123);
		}
		else if(rx_mb > 10 && rx_mb <= 20){
			angle = ((rx_mb - 10)/10)*32 + (33 + 32 + 25) + (-123)
		}
		else if(rx_mb > 20 && rx_mb <= 30){
			angle = ((rx_mb - 20)/10)*31 + (33 + 32 + 25 + 32) + (-123);
		}
		else if(rx_mb > 30 && rx_mb <= 50){
			angle = ((rx_mb - 30)/20)*28 + (33 + 32 + 25 + 32 + 31) + (-123);
		}
		else if(rx_mb > 50 && rx_mb <= 75){
			angle = ((rx_mb - 50)/25)*30 + (33 + 32 + 25 + 32 + 31 + 28) + (-123);
		}
		else if(rx_mb >75 && rx_mb <= 100){
			angle = ((rx_mb - 75)/25)*34 + 	(33 + 32 + 25 + 32 + 31 + 28 + 30) + (-123);
		}
		else{		// exception case temporally, download traffic exceed 100Mb.
			angle = 123;
		}

		rotate = "rotate("+angle.toFixed(1)+"deg)";
		$('#indicator_download').css({
		"-webkit-transform": rotate,
        "-moz-transform": rotate,
        "-o-transform": rotate,
        "msTransform": rotate,
        "transform": rotate
		});
	}


	router_traffic_old = [];
	router_traffic_old = router_traffic_new;
}

function show_clients(priority_type){
	var code = "";
	var short_name = "";
	//user icon
	var userIconBase64 = "NoIcon";

	if(clientList.length == 0){
		setTimeout("show_clients();", 500);
		return false;
	}

	if(typeof(priority_type) != "undefined"){
		document.getElementById('block_all').style.visibility = "visible";
	}
	else{
		document.getElementById('block_all').style.visibility = "hidden";
	}

	clearTimeout(device_time_flag);
	clientList.sort(function(a,b){		//sort client by priority level
		if(clientList[a].qosLevel == "" && clientList[b].qosLevel == ""){
			return 0;
		}
		else if(clientList[a].qosLevel == "" && clientList[b].qosLevel != ""){
			return 1;
		}
		else if(clientList[a].qosLevel != "" && clientList[b].qosLevel == ""){
			return -1;
		}
		else{
			return clientList[a].qosLevel - clientList[b].qosLevel;
		}
	});

	for(i=0; i<clientList.length; i++){
		var clientObj = clientList[clientList[i]];

		if(clientObj.isGateway || !clientObj.isOnline)
			continue;

		var client_qosLevel = (clientObj.qosLevel == "") ? "3" : clientObj.qosLevel;
		if(typeof(priority_type) != "undefined"  && client_qosLevel != priority_type){
			code += '<div style="display:none">';
		}
		else{		// initial or click priority block, show all
			code += '<div>';
		}

		code += '<table><tr id="icon_tr_'+i+'">';
		code += '<td style="width:70px;height:60px;">';

		if(usericon_support) {
			var clientMac = clientObj.mac.replace(/\:/g, "");
			userIconBase64 = getUploadIcon(clientMac);
		}
		if(userIconBase64 != "NoIcon") {
            if(top.isIE8){
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed qosLevel' + clientObj.qosLevel + ' clientIconIE8HACK" ';
            }
            else{
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed qosLevel' + clientObj.qosLevel + ' clientIcon" ';
            }
            code += '>';
            if(clientObj.isUserUplaodImg){
                code += '<img class="clientIcon" src="' + userIconBase64 + '">';
            }else{
                code += '<i class="type" style="--svg:url(' + userIconBase64 + ')"></i>';
            }
            code += '</div>';
        }
        else if(clientObj.type != "0" || clientObj.vendor == "") {
            if(top.isIE8){
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed clientIconIE8HACK' + ' qosLevel' + clientObj.qosLevel + '"></div>';
            }
            else{
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed clientIcon qosLevel' + clientObj.qosLevel + '"><i class="type' + clientObj.type + '"></i></div>';
            }
        }
        else if(clientObj.vendor != "") {
            var clientListCSS = "";
            var vendorIconClassName = getVendorIconClassName(clientObj.vendor.toLowerCase());
            if(vendorIconClassName != "" && !downsize_4m_support) {
                clientListCSS = "vendor-icon " + vendorIconClassName;
            }
            else {
                clientListCSS = "type" + clientObj.type;
            }

            if(top.isIE8){
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed clientIconIE8HACK qosLevel' + clientObj.qosLevel + '"></div>';
            }
            else{
                code += '<div id="icon_' + i + '" onclick="show_apps(this);" class="closed qosLevel clientIcon' + clientObj.qosLevel + '"><i class="' + clientListCSS + '"></i></div>';
            }
        }

		if(clientObj.wtfast && wtfast_support) {
			code += '<div class="boost_tag_BM">BOOST</div>';/*untranslated*/
		}

		code += '</td>';
		code += '<td style="width:180px;">';

		var clientName = (clientObj.nickName == "") ? clientObj.name : clientObj.nickName;
		if(clientName.length > 23){
			short_name = clientName.substr(0,20) + "...";
			code += '<div style="font-family:monospace, Courier New, Courier;" title="' + clientObj.mac + '">'+ short_name +'</div>';
		}
		else{
			code += '<div style="font-family:monospace, Courier New, Courier;" title="' + clientObj.mac + '">'+ clientName +'</div>';
		}

		code += '</td>';
		code += '<td><div><table>';
		code += '<tr>';
		code += '<td style="width:385px">';
		code += '<div style="height:8px;padding:3px;background-color:#000;border-radius:10px;">';
		if(clientObj.wtfast && wtfast_support)
			code += '<div id="'+clientObj.mac+'_upload_bar" class="traffic_bar transition_style traffic_bar_boost"></div>';
		else
			code += '<div id="'+clientObj.mac+'_upload_bar" class="traffic_bar transition_style"></div>';
		code += '</div>';
		code += '</td>';
		code += '<td style="text-align:right;">';
		code += '<div id="'+clientObj.mac+'_upload" style="width:45px;">0.0</div>';
		code += '</td>';
		code += '<td style="width:20px;">';
		code += '<div id="'+clientObj.mac+'_upload_unit">Kb</div>';
		code += '</td>';
		code += '<td style="width:20px;">';
		code += '<div style="width:0;height:0;border-width:0 7px 10px 7px;border-style:solid;border-color:transparent transparent #08FFF0 transparent;"></div>';
		code += '</td>';
		code += '</tr>';
		code += '<tr>';
		code += '<td>';
		code +=	'<div style="height:8px;padding:3px;background-color:#000;border-radius:10px;">';
		if(clientObj.wtfast && wtfast_support)
			code += '<div id="'+clientObj.mac+'_download_bar" class="traffic_bar transition_style traffic_bar_boost"></div>';
		else
			code += '<div id="'+clientObj.mac+'_download_bar" class="traffic_bar transition_style"></div>';
		code +=	'</div>';
		code += '</td>';
		code += '<td style="text-align:right;">';
		code += '<div id="'+clientObj.mac+'_download" style="width:45px;">0.0</div>';
		code += '</td>';
		code += '<td style="width:20px;">';
		code += '<div id="'+clientObj.mac+'_download_unit" >Kb</div>';
		code += '</td>';
		code += '<td style="width:20px;">';
		code += '<div style="width:0;height:0;border-width:10px 7px 0 7px;border-style:solid;border-color:#08FFF0 transparent transparent transparent;"></div>';
		code += '</td>';
		code += '</tr>';
		code += '</table></div></td>';
		code += '</tr></table>';
		code += '<div class="splitLine"></div>';
		code += '</div>';
	}

	document.getElementById('sortable').innerHTML = code;
  if(dns_dpi_support) {
		update_device_tarffic_Dns();
	} else {
    update_device_tarffic();
  }
	setTimeout("register_event();",500);
}

var previous_click_device = new Object();
function show_apps(obj){
	var parent_obj = obj.parentNode.parentNode.parentNode.parentNode.parentNode;
	var parent_obj_temp = obj.parentNode.parentNode.parentNode.parentNode.parentNode;
	var client_mac = obj.parentNode.parentNode.childNodes[1].firstChild.title;
	var children_obj = obj.childNodes[0];
	var clientObj = clientList[client_mac];
	var code = "";

  if(dns_dpi_support) {
		if(document.form.dns_dpi_apps_analysis.value == 0)
		return false;
	}
	else {
    if(document.form.apps_analysis.value == 0)
		return false;
  }

	if(obj.className.indexOf("closed") == -1){			//close device's APPs
		clearTimeout(apps_time_flag);
		apps_time_flag = "";
		apps_traffic_old = [];
		var first_element = parent_obj_temp.firstChild;
		if(first_element.nodeName == "#text"){
			parent_obj_temp.removeChild(first_element);
			first_element = parent_obj_temp.firstChild;
		}

		var last_element = parent_obj.lastChild;
		if(last_element.nodeName == "#text"){
			parent_obj.removeChild(last_element);
			last_element = parent_obj.lastChild;
		}

		$(parent_obj_temp).empty();
		parent_obj_temp.appendChild(first_element);
		parent_obj_temp.appendChild(last_element);
		register_event();
		if(children_obj != undefined) {
			obj.setAttribute("class", "closed qosLevel" + clientObj.qosLevel + " divUserIcon");
		}
		else {
			if(clientObj.type != "0" || clientObj.vendor == "") {
				obj.setAttribute("class", "closed clientIcon type" + clientObj.type + " qosLevel" + clientObj.qosLevel + " divUserIcon");
			}
			else if(clientObj.vendor != "") {
				var clientListCSS = "";
				var vendorIconClassName = getVendorIconClassName(clientObj.vendor.toLowerCase());
				if(vendorIconClassName != "" && !downsize_4m_support) {
					clientListCSS = "vendorIcon " + vendorIconClassName;
				}
				else {
					clientListCSS = "clientIcon type" + clientObj.type;
				}
				obj.setAttribute("class", "closed " + clientListCSS + " qosLevel" + clientObj.qosLevel + " divUserIcon");
			}
		}
	}
	else{
		clearTimeout(device_time_flag);
		if(apps_time_flag != ""){
			cancel_previous_device_apps(previous_click_device);
			previous_click_device = obj;
		}
		else{
			previous_click_device = obj;
		}

		var last_element = parent_obj.lastChild;
		if(last_element.nodeName == "#text"){
			parent_obj.removeChild(last_element);
			last_element = parent_obj.lastChild;
		}

		var new_element = document.createElement("table");
		parent_obj.removeChild(last_element);
		parent_obj.appendChild(new_element);
		parent_obj.appendChild(last_element);
		if(children_obj != undefined) {
			obj.setAttribute("class", "opened clicked qosLevel" + clientObj.qosLevel + " divUserIcon");
		}
		else {
			if(clientObj.type != "0" || clientObj.vendor == "") {
				obj.setAttribute("class", "opened clientIcon_clicked type" + clientObj.type + " clicked qosLevel" + clientObj.qosLevel + " divUserIcon");
			}
			else if(clientObj.vendor != "") {
				var clientListCSS = "";
				var vendorIconClassName = getVendorIconClassName(clientObj.vendor.toLowerCase());
				if(vendorIconClassName != "" && !downsize_4m_support) {
					clientListCSS = "vendorIcon_clicked " + vendorIconClassName;
				}
				else {
					clientListCSS = "clientIcon_clicked type" + clientObj.type;
				}
				obj.setAttribute("class", "opened " + clientListCSS + " clicked qosLevel" + clientObj.qosLevel + " divUserIcon");
			}
		}
    if(dns_dpi_support) {
			update_device_tarffic_Dns();
			update_apps_tarffic_Dns(client_mac, obj, new_element);
		} else {
      update_device_tarffic();
		  update_apps_tarffic(client_mac, obj, new_element);
    }
	}
}

function cancel_previous_device_apps(obj){
	var parent_obj = obj.parentNode.parentNode.parentNode.parentNode.parentNode;
	var parent_obj_temp = obj.parentNode.parentNode.parentNode.parentNode.parentNode;
	var client_mac = obj.parentNode.parentNode.childNodes[1].firstChild.title;
	var children_obj = obj.childNodes[0];
	var clientObj = clientList[client_mac];

	clearTimeout(apps_time_flag);
	apps_time_flag = "";
	apps_traffic_old = [];
	var first_element = parent_obj_temp.firstChild;
	if(first_element.nodeName == "#text"){
		parent_obj_temp.removeChild(first_element);
		first_element = parent_obj_temp.firstChild;
	}
	var last_element = parent_obj.lastChild;
	if(last_element.nodeName == "#text"){
		parent_obj.removeChild(last_element);
		last_element = parent_obj.lastChild;
	}

	$(parent_obj_temp).empty();
	parent_obj_temp.appendChild(first_element);
	parent_obj_temp.appendChild(last_element);
	if(children_obj != undefined) {
		obj.setAttribute("class", "closed qosLevel" + clientObj.qosLevel + " divUserIcon");
	}
	else {
		if(clientObj.type != "0" || clientObj.vendor == "") {
			obj.setAttribute("class", "closed clientIcon type" + clientObj.type + " qosLevel" + clientObj.qosLevel + " divUserIcon");
		}
		else if(clientObj.vendor != "") {
			var clientListCSS = "";
			var vendorIconClassName = getVendorIconClassName(clientObj.vendor.toLowerCase());
			if(vendorIconClassName != "" && !downsize_4m_support) {
				clientListCSS = "vendorIcon " + vendorIconClassName;
			}
			else {
				clientListCSS = "clientIcon type" + clientObj.type;
			}
			obj.setAttribute("class", "closed " + clientListCSS + " qosLevel" + clientObj.qosLevel + " divUserIcon");
		}
	}
}

function render_apps(apps_array, obj_icon, apps_field){
	var code = "";
	var img = "";

	apps_array.sort();	//sort apps' name
	for(i=0;i<apps_array.length;i++){
		code +='<tr>';
		code +='<td style="width:70px;">';
		img = new Image();
		img.src = 'https://nw-dlcdnet.asus.com/plugin/app_icons/'+ apps_array[i][3] +'-'+ apps_array[i][4] +'-0.png';	// to check image file exist
		if(img.height == 0){	//default image, if image file doesn't exist
			code +='<div class="appIcons"></div>';
		}
		else{
			code +='<div class="appIcons" style="background-image:url(\'https://nw-dlcdnet.asus.com/plugin/app_icons/'+ apps_array[i][3] +'-'+ apps_array[i][4] +'-0.png\')"></div>';
		}

		code +='</td>';
		code +='<td style="width:230px;border-top:1px dotted #333;">';
		code +='<div id="'+ apps_array[i][0] +'" style="font-family:monospace, Courier New, Courier">'+apps_array[i][0]+'</div>';
		code +='</td>';

		code +='<td style="border-top:1px dotted #333;">';
		code +='<div style="margin-left:15px;">';
		code +='<table>';
		code +='<tr>';
		code +='<td style="width:305px">';
		code +='<div style="height:6px;padding:3px;background-color:#000;border-radius:10px;">';
		code +='<div id="'+apps_array[i][0]+'_upload_bar" style="width:0%;background-color:#08FFF0;height:6px;black;border-radius:5px;"></div>';
		code +='</div>';
		code +='</td>';
		code +='<td style="text-align:right;">';
		code +='<div id="'+apps_array[i][0]+'_upload">0.0</div>';
		code +=	'</td>';
		code +='<td style="width:30px;">';
		code +='<div id="'+apps_array[i][0]+'_upload_unit">Kb</div>';
		code +='</td>';
		code +='</tr>';

		code +='<tr>';
		code +='<td>';
		code +='<div style="height:6px;padding:3px;background-color:#000;border-radius:10px;">';
		code +='<div id="'+apps_array[i][0]+'_download_bar" style="width:0%;background-color:#08FFF0;height:6px;black;border-radius:5px;"></div>';
		code +='</div>';
		code +='</td>';
		code +='<td style="text-align:right;">';
		code +='<div id="'+apps_array[i][0]+'_download">0.0</div>';
		code +='</td>';
		code +='<td style="width:30px;">';
		code +='<div id="'+apps_array[i][0]+'_download_unit">Kb</div>';
		code +='</td>';
		code +='</tr>';
		code +='</table>';
		code +='</div>';
		code +='</td>';
		code +='</tr>';
	}

	if(code == ""){
		code = "<tr><td colspan='3' style='text-align:center;color:#FFCC00'><div style='padding:5px 0px;border-top:solid 1px #333;'><#Bandwidth_monitor_noList#></div></td></tr>";
	}

	$(apps_field).empty();
	$(apps_field).append(code);
	calculate_apps_traffic(apps_array);
}

client_traffic_old = new Array();
function calculate_traffic(array_traffic){
	var client_traffic_new = new Array();

	for(i=0;i< array_traffic.length;i++){
		if(typeof(clientList[array_traffic[i][0]]) != "undefined" && clientList[array_traffic[i][0]].isOnline){
			client_traffic_new.push(array_traffic[i][0]);
			client_traffic_new[array_traffic[i][0]] = {"tx":array_traffic[i][1], "rx":array_traffic[i][2]};
		}
	}

	for(i=0;i< client_traffic_new.length;i++){
		if(client_traffic_old.length != 0){
			var diff_tx = 0;
			var diff_rx = 0;
			var diff_tx_kb = 0;
			var diff_rx_kb = 0;
			var diff_tx_mb = 0;
			var diff_rx_mb = 0;
			var tx_width = 0;
			var rx_width = 0;

			if(client_traffic_old[client_traffic_new[i]]){
				if((client_traffic_new[client_traffic_new[i]].tx - client_traffic_old[client_traffic_new[i]].tx) < 0){
					//diff_tx = (parseInt(client_traffic_new[client_traffic_new[i]].tx) + Math.pow(2,32)) - client_traffic_old[client_traffic_new[i]].tx;
				}
				else{
					diff_tx = client_traffic_new[client_traffic_new[i]].tx - client_traffic_old[client_traffic_new[i]].tx;
				}
			}
			else{
				diff_tx = 0;
			}

			if(client_traffic_old[client_traffic_new[i]]){
				if((client_traffic_new[client_traffic_new[i]].rx - client_traffic_old[client_traffic_new[i]].rx) < 0){
					//diff_rx = (parseInt(client_traffic_new[client_traffic_new[i]].rx) + Math.pow(2,32)) - client_traffic_old[client_traffic_new[i]].rx;
				}
				else{
					diff_rx = client_traffic_new[client_traffic_new[i]].rx - client_traffic_old[client_traffic_new[i]].rx;
				}
			}
			else{
				diff_rx = 0;
			}

			diff_tx = diff_tx*8/detect_interval;
			diff_rx = diff_rx*8/detect_interval;
			diff_tx_kb = diff_tx/1024;
			diff_rx_kb = diff_rx/1024;
			diff_tx_mb = diff_tx/1024/1024;
			diff_rx_mb = diff_rx/1024/1024;

			//upload traffic
			if((diff_tx/1024) < upload_maximum/5){
				if(diff_tx == 0){
					try{
						document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = "0%";
					}
					catch(e){
						console.log("[" + i + "] " + client_traffic_new[i]);
						continue;
					}
				}
				else{
					tx_width = parseInt(diff_tx_kb/(upload_maximum/5)*30);
					if(diff_tx_kb.toFixed(1) >= 0.1 && tx_width < 1)
						tx_width = 1;

					if(document.getElementById(client_traffic_new[i]+'_upload_bar') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload_bar') != null){
						document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					}
				}

				if(diff_tx_kb < 1024){
					if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
						document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_kb.toFixed(1);
						document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Kb";
					}
				}
				else{
					if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
						document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_mb.toFixed(1) ;
						document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Mb";
					}
				}
			}
			else if((diff_tx_kb >= upload_maximum/5) && (diff_tx_kb < upload_maximum*2/5)){
				tx_width = parseInt((diff_tx_kb - (upload_maximum/5))/(upload_maximum/5)*25);
				tx_width += 30;
				if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
					document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Mb";
				}
			}
			else if((diff_tx_kb >= upload_maximum*2/5) && (diff_tx_kb < upload_maximum*3/5)){
				tx_width = parseInt((diff_tx_kb - (upload_maximum*2/5))/(upload_maximum/5)*20);
				tx_width += 55;
				if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
					document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Mb";
				}
			}
			else if((diff_tx_kb >= upload_maximum*3/5) && (diff_tx_kb < upload_maximum*4/5)){
				tx_width = parseInt((diff_tx_kb - (upload_maximum*3/5))/(upload_maximum/5)*15);
				tx_width += 75;
				if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
					document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Mb";
				}
			}
			else{
				tx_width = parseInt((diff_tx_kb - (upload_maximum*4/5))/(upload_maximum/5)*15);
				tx_width += 90;
				if(tx_width > 100)
					tx_width = 100;
				if(document.getElementById(client_traffic_new[i]+'_upload') != "undefined" && document.getElementById(client_traffic_new[i]+'_upload') != null){
					document.getElementById(client_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					document.getElementById(client_traffic_new[i]+'_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(client_traffic_new[i]+'_upload_unit').innerHTML = "Mb";
				}
			}

			// download traffic
			if(diff_rx_kb < download_maximum/5){		//30%
				if(diff_rx == 0){
					try{
						document.getElementById(client_traffic_new[i]+'_download_bar').style.width = "0%";
					}
					catch(e){
						console.log("[" + i + "] " + client_traffic_new[i]);
						continue;
					}
				}
				else{
					rx_width = parseInt(diff_rx_kb/(download_maximum/5)*30);
					if( diff_rx_kb.toFixed(1) >= 0.1 &&  rx_width < 1)
						rx_width = 1;
					if(document.getElementById(client_traffic_new[i]+'_download_bar') != "undefined" && document.getElementById(client_traffic_new[i]+'_download_bar') != null){
						document.getElementById(client_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					}
				}

				if(diff_rx_kb < 1024){
					if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
						document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_kb.toFixed(1);
						document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Kb";
					}
				}
				else{
					if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
						document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
						document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Mb";
					}
				}
			}
			else if((diff_rx_kb >= download_maximum/5) && (diff_rx_kb < download_maximum*2/5)){		//	25%
				rx_width = parseInt((diff_rx_kb - (download_maximum/5))/(download_maximum/5)*25);
				rx_width += 30;
				if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
					document.getElementById(client_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
			}
			else if((diff_rx_kb >= download_maximum*2/5) && (diff_rx_kb < download_maximum*3/5)){		// 20%
				rx_width = parseInt((diff_rx_kb - (download_maximum*2/5))/(download_maximum/5)*20);
				rx_width += 55;
				if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
					document.getElementById(client_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
			}
			else if((diff_rx_kb >= download_maximum*3/5) && (diff_rx_kb <download_maximum*4/5)){		//	15%
				rx_width = parseInt((diff_rx_kb - (download_maximum*3/5))/(download_maximum/5)*15);
				rx_width += 75;
				if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
					document.getElementById(client_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
			}
			else{		//10%
				rx_width = parseInt((diff_rx_kb - (download_maximum*4/5))/(download_maximum/5)*10);
				rx_width += 90;
				if(rx_width > 100)
					rx_width = 100;

				if(document.getElementById(client_traffic_new[i]+'_download') != "undefined" && document.getElementById(client_traffic_new[i]+'_download') != null){
					document.getElementById(client_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(client_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(client_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
			}
		}
	}

	client_traffic_old = [];
	client_traffic_old = client_traffic_new;
}

apps_traffic_old = new Array();

function calculate_apps_traffic(apps_traffic){
	apps_traffic_new = new Array();
	var traffic_flag = 0;
	var empty_count = 0;
	for(i=0;i< apps_traffic.length;i++){
		apps_traffic_new[i] = apps_traffic[i][0];
		apps_traffic_new[apps_traffic[i][0]] = {"tx":apps_traffic[i][1], "rx":apps_traffic[i][2], "account":0};
	}

	for(i=0;i< apps_traffic_new.length;i++){
		if(apps_traffic_old.length != 0){
			var diff_tx = 0;
			var diff_rx = 0;
			var diff_tx_kb = 0;
			var diff_rx_kb = 0;
			var diff_tx_mb = 0;
			var diff_rx_mb = 0;
			var tx_width = 0;
			var rx_width = 0;

			diff_tx = (apps_traffic_old[apps_traffic_new[i]]) ? apps_traffic_new[apps_traffic_new[i]].tx - apps_traffic_old[apps_traffic_new[i]].tx : 0;
			diff_rx = (apps_traffic_old[apps_traffic_new[i]]) ? apps_traffic_new[apps_traffic_new[i]].rx - apps_traffic_old[apps_traffic_new[i]].rx : 0;
      
      /* dns dpi report current active conntrack and its accumulated bytes for simplicity
			   However, a few conntrack might timeout and won't report next query then have a smaller app bytes compared to last query
			   To fix in fw, need to have a baseline conntrack and add calculated diff , minus tracks which there is no same app_id active
			   ct. Nice to have since fw already give correct acculmuated info
			*/
			if(dns_dpi_support) {
				if(diff_tx < 0)
					diff_tx = 0;
				if(diff_rx < 0)
					diff_rx = 0;
			}
			
			diff_tx = diff_tx*8/detect_interval;
			diff_rx = diff_rx*8/detect_interval;

			diff_tx_kb = diff_tx/1024;
			diff_rx_kb = diff_rx/1024;
			diff_tx_mb = diff_tx/1024/1024;
			diff_rx_mb = diff_rx/1024/1024;

			if(apps_traffic_old[apps_traffic_new[i]])
				empty_count = apps_traffic_old[apps_traffic_new[i]].account + apps_traffic_new[apps_traffic_new[i]].account;

			if(diff_tx ==0 && diff_rx == 0){
				apps_traffic_new[apps_traffic_new[i]].account = empty_count + 1;
				if(apps_traffic_new[apps_traffic_new[i]].account >= 10){
					document.getElementById(apps_traffic_new[i]).parentNode.parentNode.style.display = "none";
				}
			}
			else{
				apps_traffic_new[apps_traffic_new[i]].account = 0;
				traffic_flag = 1;
				if((diff_tx/1024) < upload_maximum/5){
					if(diff_tx == 0){
						try{
							document.getElementById(apps_traffic_new[i]+'_upload_bar').style.width = "0%";
						}
						catch(e){
							console.log("[" + i + "] " + apps_traffic_new[i]);
						}
					}
					else{
						tx_width = parseInt(diff_tx_kb/(upload_maximum/5)*30);
						if(diff_tx_kb.toFixed(1) >= 0.1 && tx_width < 1)
							tx_width = 1;

						document.getElementById(apps_traffic_new[i]+'_upload_bar').style.width = tx_width + "%";
					}

					if(diff_tx_kb < 1024){
						document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_kb.toFixed(1);
						document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Kb";
					}
					else{
						document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_mb.toFixed(1) ;
						document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Mb";
					}
				}
				else if((diff_tx_kb >= upload_maximum/5) && (diff_tx_kb < upload_maximum*2/5)){
					tx_width = parseInt((diff_tx_kb - (upload_maximum/5))/(upload_maximum/5)*25);
					tx_width += 30;
					document.getElementById(apps_traffic_new[i] + '_upload_bar').style.width = tx_width + "%";
					document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Mb";
				}
				else if((diff_tx_kb >= upload_maximum*2/5) && (diff_tx_kb < upload_maximum*3/5)){
					tx_width = parseInt((diff_tx_kb - (upload_maximum*2/5))/(upload_maximum/5)*20);
					tx_width += 55;
					document.getElementById(apps_traffic_new[i] + '_upload_bar').style.width = tx_width + "%";
					document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Mb";
				}
				else if((diff_tx_kb >= upload_maximum*3/5) && (diff_tx_kb < upload_maximum*4/5)){
					tx_width = parseInt((diff_tx_kb - (upload_maximum*3/5))/(upload_maximum/5)*15);
					tx_width += 75;
					document.getElementById(apps_traffic_new[i] + '_upload_bar').style.width = tx_width + "%";
					document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Mb";
				}
				else{
					tx_width = parseInt((diff_tx_kb - (upload_maximum*4/5))/(upload_maximum/5)*15);
					tx_width += 90;
					if(tx_width > 100)
						tx_width = 100;

					document.getElementById(apps_traffic_new[i] + '_upload_bar').style.width = tx_width + "%";
					document.getElementById(apps_traffic_new[i] + '_upload').innerHTML = diff_tx_mb.toFixed(1)
					document.getElementById(apps_traffic_new[i] + '_upload_unit').innerHTML = "Mb";
				}

				if(diff_rx_kb < download_maximum/5){		//30%
					if(diff_rx == 0){
						try{
							document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = "0%";
						}
						catch(e){
							console.log("[" + i + "] " + apps_traffic_new[i]);
						}
					}
					else{
						rx_width = parseInt(diff_rx_kb/(download_maximum/5)*30);
						if(diff_rx_kb.toFixed(1) >= 0.1 && rx_width < 1)
							rx_width = 1;

						document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					}

					if(diff_rx_kb < 1024){
						document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_kb.toFixed(1);
						document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Kb";

					}
					else{
						document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
						document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Mb";
					}
				}
				else if((diff_rx_kb >= download_maximum/5) && (diff_rx_kb < download_maximum*2/5)){		//	25%
					rx_width = parseInt((diff_rx_kb - (download_maximum/5))/(download_maximum/5)*25);
					rx_width += 30;
					document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
				else if((diff_rx_kb >= download_maximum*2/5) && (diff_rx_kb < download_maximum*3/5)){		// 20%
					rx_width = parseInt((diff_rx_kb - (download_maximum*2/5))/(download_maximum/5)*20);
					rx_width += 55;
					document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
				else if((diff_rx_kb >= download_maximum*3/5) && (diff_rx_kb <download_maximum*4/5)){		//	15%
					rx_width = parseInt((diff_rx_kb - (download_maximum*3/5))/(download_maximum/5)*15);
					rx_width += 75;
					document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
				else{		//10%
					rx_width = parseInt((diff_rx_kb - (download_maximum*4/5))/(download_maximum/5)*10);
					rx_width += 90;
					if(rx_width > 100)
						rx_width = 100;

					document.getElementById(apps_traffic_new[i]+'_download_bar').style.width = rx_width + "%";
					document.getElementById(apps_traffic_new[i]+'_download').innerHTML = diff_rx_mb.toFixed(1);
					document.getElementById(apps_traffic_new[i]+'_download_unit').innerHTML = "Mb";
				}
			}
		}
	}

	apps_traffic_old = [];
	apps_traffic_old = apps_traffic_new;
}

var device_time_flag = "";
function update_device_tarffic() {
  $.ajax({
    url: '/getTraffic.asp',
    dataType: 'script',
    error: function(xhr) {
		setTimeout("update_device_tarffic();", detect_interval*1000);
    },
    success: function(response){
		calculate_router_traffic(router_traffic);
		calculate_traffic(array_traffic);
		device_time_flag = setTimeout("update_device_tarffic();", detect_interval*1000);
    }
  });
}
function update_device_tarffic_Dns() {
  $.ajax({
    url: '/getTraffic_Dns.asp',
    dataType: 'script',
    error: function(xhr) {
		setTimeout("update_device_tarffic_Dns();", detect_interval*1000);
    },
    success: function(response){
		calculate_router_traffic(router_traffic);
		calculate_traffic(array_traffic);
		device_time_flag = setTimeout("update_device_tarffic_Dns();", detect_interval*1000);
    }
  });
}

var apps_time_flag = "";
function update_apps_tarffic(mac, obj, new_element) {
  $.ajax({
    url: '/getTraffic.asp?client='+mac,
    dataType: 'script',
    error: function(xhr) {
		setTimeout("update_apps_tarffic('"+mac+"');", detect_interval*1000);
    },
    success: function(response){
		render_apps(array_traffic, obj, new_element);

		apps_time_flag = setTimeout(function(){
			update_apps_tarffic(mac,obj,new_element)
		}, detect_interval*1000);
    }
  });
}
function update_apps_tarffic_Dns(mac, obj, new_element) {
  $.ajax({
    url: '/getTraffic_Dns.asp?client='+mac,
    dataType: 'script',
    error: function(xhr) {
		setTimeout("update_apps_tarffic_Dns('"+mac+"');", detect_interval*1000);
    },
    success: function(response){
		render_apps(array_traffic, obj, new_element);

		apps_time_flag = setTimeout(function(){
			update_apps_tarffic_Dns(mac,obj,new_element)
		}, detect_interval*1000);
    }
  });
}


function regen_qos_rule(obj, priority){
	var qos_rulelist_row =  qos_rulelist.split("<");
	var target_name = $(obj.parentNode).siblings()[0].children[0].innerHTML;
	var target_mac = $(obj.parentNode).siblings()[0].children[0].title;
	var rule_temp = "";
	var match_flag = 0;

	for(i=1;i<qos_rulelist_row.length;i++){
		var qos_rulelist_col = qos_rulelist_row[i].split(">");
		if( (target_mac == qos_rulelist_col[1])){
			continue;
		}

		rule_temp += "<";
		for(j=0;j<qos_rulelist_col.length;j++){
			if(target_mac == qos_rulelist_col[1]){		//for device already in the rule list
				if(j == 0){
					rule_temp += target_name + ">";
				}
				else if(j == 1){
					rule_temp += target_mac + ">";
				}
				else if(j==3){
					rule_temp += "any>";
				}
				else if(j == qos_rulelist_col.length-1){
					rule_temp += priority;
				}
				else{
					rule_temp += ">";
				}

				match_flag = 1;
			}
			else{
				rule_temp += qos_rulelist_col[j];
				if(j != qos_rulelist_col.length-1)
					rule_temp += ">";
			}
		}
	}

	if(match_flag != 1){		//new rule
		rule_temp += "<" + target_name + ">" + target_mac + ">>any>>" + priority;
	}

	qos_rulelist = rule_temp;
}

function applyRule(){
  if(dns_dpi_support)
		document.form.action_script.value = "restart_dnsqd;restart_qos;restart_firewall";
	document.form.qos_rulelist.value = qos_rulelist;
	document.form.submit();
}

function eula_confirm(){
	var _flag = document.form._flag.value;
	document.form.TM_EULA.value = 1;
	if(_flag == "apps_analysis" || _flag == "dns_dpi_apps_analysis"){
		document.form.apps_analysis.value = 1;
    if(dns_dpi_support)
		  document.form.dns_dpi_apps_analysis.value = 1;
    applyRule();
	}
	else if(_flag == "game"){
		document.form.qos_enable.value = '1';
		document.form.qos_type.value = '1';
		document.form.bwdpi_app_rulelist.disabled = false;
		document.form.bwdpi_app_rulelist.value = "9,20<8<4<0,5,6,15,17<4,13<13,24<1,3,14<7,10,11,21,23<game";
		if(ctf_disable == 1){
			document.form.action_script.value = "restart_qos;restart_firewall";
      if(dns_dpi_support)
		    document.form.action_script.value = "restart_dnsqd;restart_qos;restart_firewall";
		}
		else{
			if(ctf_fa_mode == "2"){
				FormActions("start_apply.htm", "apply", "reboot", "<% get_default_reboot_time(); %>");
			}
			else{
				if(document.form.qos_type.value == 0)
					FormActions("start_apply.htm", "apply", "reboot", "<% get_default_reboot_time(); %>");
				else{
					document.form.action_script.value = "restart_qos;restart_firewall";
          if(dns_dpi_support)
		        document.form.action_script.value = "restart_dnsqd;restart_qos;restart_firewall";
				}
			}
		}

		document.form.submit();
	}
}

function cancel(){
	if(document.form._flag.value == "game"){
		refreshpage();
	}
	else{
		curState = 0;
		$('#iphone_switch').animate({backgroundPosition: -37}, "slow", function() {});
	}
}

function switch_control(_status){
	if(_status) {
        if(!dns_dpi_support){
            if(reset_wan_to_fo.check_status()) {
                if(policy_status.TM == 0 || policy_status.TM_time == ''){
                    const policyModal = new PolicyModalComponent({
                        policy: "TM",
                        agreeCallback: eula_confirm,
                        disagreeCallback: cancel
                    });
                    policyModal.show();
                }else{
                    eula_confirm();
                }
            }else
                cancel();
        }else {
            document.form.apps_analysis.value = 1;
            document.form.dns_dpi_apps_analysis.value = 1;
            applyRule();
        }
	}
	else {
		document.form.apps_analysis.value = 0;
		if(dns_dpi_support)
			document.form.dns_dpi_apps_analysis.value = 0;
		applyRule();
	}
}
</script>
</head>

<body onload="initial();" onunload="unload_body();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg"></div>
<div id="hiddenMask" class="popup_bg" style="z-index:999;">
	<table cellpadding="5" cellspacing="0" id="dr_sweet_advise" class="dr_sweet_advise" align="center">
	</table>
	<!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]-->
</div>
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="current_page" value="/GameBoost_ROG.asp">
<input type="hidden" name="next_page" value="/GameBoost_ROG.asp">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="restart_qos;restart_firewall">
<input type="hidden" name="action_wait" value="5">
<input type="hidden" name="flag" value="">
<input type="hidden" name="qos_rulelist" value="">
<input type="hidden" name="TM_EULA" value="<% nvram_get("TM_EULA"); %>">
<input type="hidden" name="apps_analysis" value="<% nvram_get("apps_analysis"); %>">
<input type="hidden" name="dns_dpi_apps_analysis" value="<% nvram_get("dns_dpi_apps_analysis"); %>">
<input type="hidden" name="qos_enable" value="<% nvram_get("qos_enable"); %>">
<input type="hidden" name="qos_type" value="<% nvram_get("qos_type"); %>">
<input type="hidden" name="qos_type_ori" value="<% nvram_get("qos_type"); %>">
<input type="hidden" name="bwdpi_app_rulelist" value="<% nvram_get("bwdpi_app_rulelist"); %>" disabled>
<input type="hidden" name="_flag" value="" disabled>

<table class="content" align="center" cellpadding="0" cellspacing="0">
	<tr>
		<td width="17">&nbsp;</td>
		<!--=====Beginning of Main Menu=====-->
		<td valign="top" width="202">
		  <div id="mainMenu"></div>
		  <div id="subMenu"></div>
		</td>
		<td valign="top">
			<div id="tabMenu" class="submenuBlock"></div>
			<!--===================================Beginning of Main Content===========================================-->
			<table width="95%" border="0" align="left" cellpadding="0" cellspacing="0" class="FormTitle" id="FormTitle">
			<tr>
				<td bgcolor="#4D595D" valign="top">
					<table width="760px" border="0" cellpadding="4" cellspacing="0">
						<tr>
							<td bgcolor="#4D595D" valign="top">
								<table width="100%">
									<tr>
										<td class="formfonttitle" align="left">
											<div><#menu5_3_2#> - <#Bandwidth_monitor_WANLAN#></div>
										</td>
										<td>
											<div>
												<table align="right">
													<tr>
														<td>
															<div class="formfonttitle" style="margin-bottom:0px;margin-left:50px;" title="<#Bandwidth_monit_analy_desc#>"><#Bandwidth_monitor_analysis#></div>
														</td>
														<td >
															<div align="center" class="left" style="width:94px; float:left; cursor:pointer;" id="apps_analysis_enable"></div>
															<script type="text/javascript">
                              if(dns_dpi_support)
                              {  
																$('#apps_analysis_enable').iphoneSwitch('<% nvram_get("dns_dpi_apps_analysis"); %>',
																	function(){
                                                                        switch_control(1);
                                                                    },
                                                                    function(){
                                                                        switch_control(0);
                                                                    }
																);
                              } else 
                              {
                                                                $('#apps_analysis_enable').iphoneSwitch('<% nvram_get("apps_analysis"); %>',
																	function(){
                                                                        switch_control(1);
                                                                    },
                                                                    function(){
                                                                        switch_control(0);
                                                                    }
																);
                              }
															</script>
														</td>
													</tr>
												</table>
											</div>
										</td>
									</tr>
								</table>
							</td>
						</tr>
						<tr>
							<td height="5" bgcolor="#4D595D" valign="top"><div class="splitLine"></div></td>
						</tr>
						<tr>
							<td>
								<div>
									<table style="width:99%;">
										<tr>
											<td id="upload_unit" style="width:50%;">
												<div style="margin:-10px 0 5px 70px;font-size:16px;text-align:center;"><#upload_bandwidth#></div>
												<div style="position:absolute;margin:12px 0px 0px 112px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:-8px 0px 0px 222px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:50px 0px 0px 300px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:121px 0px 0px 296px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:150px 0px 0px 275px;font-size:16px;display:none;"></div>
												<div id="upload_speed" style="position:absolute;margin:147px 0px 0px 187px;font-size:24px;width:60px;text-align:center;">0.00</div>
												<div style="background-image:url('images/New_ui/speedmeter.png');height:188px;width:270px;background-repeat:no-repeat;margin:-10px 0px 0px 70px"></div>
												<div id="indicator_upload" class="transition_style" style="background-image:url('images/New_ui/indicator.png');position:absolute;height:100px;width:50px;background-repeat:no-repeat;margin:-110px 0px 0px 194px;"></div>
											</td>
											<td id="download_unit">
												<div style="margin:-10px 0 5px -55px;font-size:16px;text-align:center;"><#download_bandwidth#></div>
												<div style="position:absolute;margin:12px 0px 0px 88px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:-6px 0px 0px 203px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:50px 0px 0px 275px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:120px 0px 0px 270px;font-size:16px;display:none;"></div>
												<div style="position:absolute;margin:150px 0px 0px 250px;font-size:16px;display:none;"></div>
												<div id="download_speed" style="position:absolute;margin:147px 0px 0px 130px;font-size:24px;text-align:center;width:60px;">0.00</div>
												<div style="background-image:url('images/New_ui/speedmeter.png');height:188px;width:270px;background-repeat:no-repeat;margin:-10px 0px 0px 10px"></div>
												<div id="indicator_download" class="transition_style" style="background-image:url('images/New_ui/indicator.png');position:absolute;height:100px;width:50px;background-repeat:no-repeat;margin:-110px 0px 0px 133px;"></div>
											</td>
										</tr>
									</table>
								</div>
							</td>
						</tr>
						<tr>
							<td>
								<div style="margin:10px;">
									<table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">
										<tr>
											<th style="border-bottom:1px solid #FFF"><#CTL_Enabled#> Game Boost</th>
											<td style="border-bottom:1px solid #FFF">
												<div align="center" class="left" style="width:94px; float:left; cursor:pointer;" id="radio_gameBoost_enable"></div>
												<div class="iphone_switch_container" style="height:32px; width:74px; position: relative; overflow: hidden">
													<script type="text/javascript">
														$('#radio_gameBoost_enable').iphoneSwitch(enable_GameBoost,
															function(){
																document.form._flag.value = "game";
                                                                if(policy_status.TM == 0 || policy_status.TM_time == ''){
                                                                    const policyModal = new PolicyModalComponent({
                                                                        policy: "TM",
                                                                        agreeCallback: eula_confirm,
                                                                    });
                                                                    policyModal.show();
                                                                }else{
                                                                    eula_confirm();
                                                                }
															},
															function(){
																document.form.qos_enable.value = '0';
																document.form.bwdpi_app_rulelist.disabled = true;
																document.form.submit();
															}
														);
													</script>
												</div>
											</td>
										</tr>
									</table>
								</div>
							</td>
						</tr>

						<tr>
							<td>
								<div style="margin:-5px 0px;">
									<table>
										<tr>
											<td id="block_all" style="width:50%;font-family: Arial, Helvetica, sans-serif;text-align:left;padding-left:15px;visibility:hidden;">
												<div style="cursor:pointer;background-color:rgb(124,6,26);width:113px;border-radius:10px;text-align:center;box-shadow:0px 2px black;" onclick="show_clients()">
													<table>
														<tr>
															<td><div style="width:110px;"><#show_all#></div></td>
														</tr>
													</table>
												</div>
											</td>
											<td>
												<div id="priority_block">
													<table>
														<tr>
															<td>
																<div id="0" style="cursor:pointer;background-color:rgb(124,6,26);width:100px;border-radius:10px;text-align:center;box-shadow:0px 2px black;z-index:100;" onclick="show_clients(this.id)">
																	<table>
																		<tr>
																			<td style="width:25px;"><div style="width:12px;height:12px;border-radius:10px;background-color:#F01F09;margin-left:5px;"></div></td>
																			<td><#Highest#></td>
																		</tr>
																	</table>
																</div>
															</td>
															<td>
																<div id="1" style="cursor:pointer;background-color:rgb(124,6,26);width:90px;border-radius:10px;text-align:center;box-shadow:0px 2px black;z-index:100;" onclick="show_clients(this.id)">
																	<table>
																		<tr>
																			<td style="width:25px;"><div style="width:12px;height:12px;border-radius:10px;background-color:#F08C09;margin-left:5px;"></div></td>
																			<td><#High#></td>
																		</tr>
																	</table>
																</div>
															</td>
															<td>
																<div id="2" style="cursor:pointer;background-color:rgb(124,6,26);width:90px;border-radius:10px;text-align:center;box-shadow:0px 2px black;z-index:100;" onclick="show_clients(this.id)">
																	<table>
																		<tr>
																			<td style="width:25px;"><div style="width:12px;height:12px;border-radius:10px;background-color:#F3DD09;margin-left:5px;"></div></td>
																			<td><#Medium#></td>
																		</tr>
																	</table>
																</div>
															</td>
															<td>
																<div id="3" style="cursor:pointer;background-color:rgb(124,6,26);width:90px;border-radius:10px;text-align:center;box-shadow:0px 2px black;z-index:100;" onclick="show_clients(this.id)">
																	<table>
																		<tr>
																			<td style="width:25px;"><div style="width:12px;height:12px;border-radius:10px;background-color:#7A797A;margin-left:5px;"></div></td>
																			<td><#Setting_factorydefault_value#></td>
																		</tr>
																	</table>
																</div>
															</td>
															<td>
																<div id="4" style="cursor:pointer;background-color:rgb(124,6,26);width:100px;border-radius:10px;text-align:center;box-shadow:0px 2px black;z-index:100;" onclick="show_clients(this.id)">
																	<table>
																		<tr>
																			<td style="width:25px;"><div style="width:12px;height:12px;border-radius:10px;background-color:#58CCED;margin-left:5px;"></div></td>
																			<td><#Lowest#></td>
																		</tr>
																	</table>
																</div>
															</td>
														</tr>
													</table>
												</div>
											<td>
										</tr>
									</table>
								</div>
							</td>
						</tr>

						<tr>
							<td>
								<div colspan="2" style="width:99%;height:510px;background-color:rgb(62,3,13);border-radius:10px;margin-left:4px;overflow:auto;">
									<div id="sortable" style="padding-top:5px;">
										<div style="width: 100%;text-align: center;margin-top: 50px;">
											<img src="/images/InternetScan.gif" style="width: 50px;">
										</div>
    								</div>
								</div>
							</td>
						</tr>
						<tr>
							<td>
								<div style=" *width:136px;margin:5px 0px 0px 300px;" class="titlebtn" align="center" onClick="applyRule();">
									<span><#CTL_apply#></span>
									<div style="margin:-30px 0 0px -480px;"><a id="faq" style="text-decoration:underline;" href="" target="_blank"><#Bandwidth_monitor_WANLAN#> FAQ</a></div>
								</div>
							</td>
						</tr>
					</table>
				</td>
			</tr>
			</table>
			<!--===================================End of Main Content===========================================-->
		</td>
	</tr>
</table>
<div id="footer"></div>
