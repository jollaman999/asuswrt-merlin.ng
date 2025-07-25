<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<link rel="icon" href="images/favicon.png">
<link rel="stylesheet" type="text/css" href="css/main_login.css">
<title><#Web_Title#></title>
<script language="JavaScript" type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/js/https_redirect/https_redirect.js"></script>
<script type="text/javascript" src="/js/httpApi.js"></script>
<style>
body, .p1, .form-input{
	color: #FFF;
}
body{
	font-family: Roboto, STHeiti, Microsoft JhengHei, sans-serif, Arial;
	background-color: rgb(31, 31, 31) !important;
}
.main-field-bg{
	margin:20px auto 0;
	width: 887px;
	height: 849px;
}
.main-field-padding{
	width: 100%;
	margin: 0 auto;	
}
.logo-container{
	display: flex;
	align-items: center;
	margin-left: 90px;
}
.login-title{
	font-size: 48px;
	color: #93d2d9;
}
.login-title-desc{
	margin: 15px 30px 36px 120px;
	text-align: left;
}
.desc{
	margin: 12px 0;
}
.input-container{
	display: flex;
	justify-content: center;
	margin: 0 0 20px 0;
}
.loading-icon{
	margin: 50px 290px 0px 0px;
	float: right;
}

/*for mobile device*/
@media screen and (max-width: 1000px){
	.loading-icon{
		margin: 0;
		float: none;
		text-align: center;
	}
	.login-title-desc{
		font-size: 14pt;
		margin: auto 15px;
	}
}
</style>
<script>
/* String splice function */
String.prototype.splice = function( idx, rem, s ) {
    return (this.slice(0,idx) + s + this.slice(idx + Math.abs(rem)));
};

/* String repeat function */
String.prototype.repeat = function(times) {
   return (new Array(times + 1)).join(this);
};

String.prototype.strReverse = function() {
	var newstring = "";
	for (var s=0; s < this.length; s++) {
		newstring = this.charAt(s) + newstring;
	}
	return newstring;
	//strOrig = ' texttotrim ';
	//strReversed = strOrig.revstring();
};

function isSupport(_ptn){
	var ui_support = [<% get_ui_support(); %>][0];
	return (ui_support[_ptn]) ? ui_support[_ptn] : 0;
}
var CoBrand = '<% nvram_get("CoBrand"); %>';

var is_AA_sku = (function(){
	var ttc = '<% nvram_get("territory_code"); %>';
	return (ttc.search("AA") == -1) ? false : true;
})();
var is_KR_sku = (function(){
	var ttc = '<% nvram_get("territory_code"); %>';
	return (ttc.search("KR") == -1) ? false : true;
})();
var is_SG_sku = (function(){
	var ttc = '<% nvram_get("territory_code"); %>';
	return (ttc.search("SG") == -1) ? false : true;
})();
var isIE8 = navigator.userAgent.search("MSIE 8") > -1; 
var isIE9 = navigator.userAgent.search("MSIE 9") > -1; 

var secure_default = (function(){
	var rc_support = '<% nvram_get("rc_support"); %>';
	return (rc_support.search("secure_default") == -1) ? false : true;
})();

if(isSupport("TS_UI")){
	$('link').last().after('<link rel="stylesheet" type="text/css" href="css/difference.css">');
	$(".form-input").css("")
}

function initial(){
	top.name = "";/* reset cache of state.js win.name */

	if(is_KR_sku || is_SG_sku || is_AA_sku || secure_default)
		document.getElementById("KRHint").style.display = "";

	if(isSupport("TS_UI") && CoBrand == "12"){
		document.getElementsByClassName("logo-rog")[0].className = "logo-tx_miku";
		$(".main-field-bg").css("width", "980px");
	}

	if(isIE8 || isIE9){
		document.getElementById("router_name_tr").style.display = "";
		document.getElementById("router_password_tr").style.display = "";
		document.getElementById("router_password_confirm_tr").style.display = "";
	}

	document.form.http_username_x.focus();
	if(navigator.userAgent.search("MSIE 8") === -1){
		document.form.http_username_x.onkeyup = function(e){
			if(e.keyCode == 13){
				document.form.http_passwd_x.focus();
			}
		};

		document.form.http_passwd_x.onkeyup = function(e){
			if(e.keyCode == 13){
				document.form.http_passwd_2_x.focus();
			}
		};

		document.form.http_passwd_2_x.onkeyup = function(e){
			if(e.keyCode == 13){
				submitForm();
			}
		};
	}
}

// ---------- Viz add common string check for password 2015.09 start--------
function check_common_string(pwd, flag){
	//Sequential
	var termAlphas = "abcdefghijklmnopqrstuvwxyz";
	var termNumerics = "01234567890";
	var termSymbols = "~!@#$%^&*()_+";
	var termKeyboards1 = "qwertyuiop";
	var termKeyboards2 = "asdfghjkl";
	var termKeyboards3 = "zxcvbnm";
	var termCommon5 = ["123123","abc123","letmein","master","qazwsx","admin"];
	var termCommon8 = ["adminpassword","loginpassword","passw0rd","password","useradmin","userpassword"];
	var nSeqString = 0;
	if(flag == "httpd_password"){	//at lease length 5		
		if(termAlphas.toLowerCase().indexOf(pwd) != -1 || termAlphas.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termNumerics.toLowerCase().indexOf(pwd) != -1 || termNumerics.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termSymbols.toLowerCase().indexOf(pwd) != -1 || termSymbols.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termKeyboards1.toLowerCase().indexOf(pwd) != -1 || termKeyboards1.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termKeyboards2.toLowerCase().indexOf(pwd) != -1 || termKeyboards2.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termKeyboards3.toLowerCase().indexOf(pwd) != -1 || termKeyboards3.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		for(var s=0;s<termCommon5.length;s++){
			if(pwd == termCommon5[s])	{ nSeqString++; }	
		}
		for(var t=0;t<termCommon8.length;t++){
			if(pwd == termCommon8[t])	{ nSeqString++; }	
		}		
	}
	else if(flag == "wpa_key"){	//at lease length 8
		if(termAlphas.toLowerCase().indexOf(pwd) != -1 || termAlphas.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termNumerics.toLowerCase().indexOf(pwd) != -1 || termNumerics.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termSymbols.toLowerCase().indexOf(pwd) != -1 || termSymbols.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termKeyboards1.toLowerCase().indexOf(pwd) != -1 || termKeyboards1.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		if(termKeyboards2.toLowerCase().indexOf(pwd) != -1 || termKeyboards2.strReverse().toLowerCase().indexOf(pwd) != -1) { nSeqString++; }
		for(var s=0;s<termCommon8.length;s++){
			if(pwd == termCommon8[s])	{ nSeqString++; }	
		}		
	}
	
	//pure repeat character string
	if(pwd == pwd.charAt(0).repeat(pwd.length)) { nSeqString++; }
	
	if(nSeqString > 0)
		return true;
	else		
		return false;
}
// ---------- Viz add common string check for password 2015.09 end--------

function validForm(){
	if($("#defpassCheckbox").prop('checked')) return true;

	if(!validator.chkLoginId(document.form.http_username_x)){
		return false;
	}
	
	if(document.form.http_passwd_x.value == ""){
			showError("<#File_Pop_content_alert_desc6#>");
			document.form.http_passwd_x.value = "";
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;
		}
		
	if(document.form.http_passwd_x.value != document.form.http_passwd_2_x.value){
			showError("<#File_Pop_content_alert_desc7#>");
			document.form.http_passwd_x.value = "";
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;                   
	}

	if(document.form.http_passwd_x.value == document.form.http_username_x.value){
			showError("<#JS_validLoginPWD_same#>");
			document.form.http_passwd_x.value = "";
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;                   
	}

	if(is_KR_sku || is_SG_sku || is_AA_sku || secure_default){		/* MODELDEP by Territory Code */
		if(!validator.chkLoginPw_KR(document.form.http_passwd_x)){
			return false;
		}
		if(document.form.http_passwd_x.value == document.form.http_username_x.value){
			alert("<#JS_validLoginPWD#>");
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;
		}
	}
	else{
		if(!validator.chkLoginPw(document.form.http_passwd_x)){
			return false;
		}
	}
	
	if(document.form.http_passwd_x.value == '<% nvram_default_get("http_passwd"); %>'){
			showError("<#QIS_adminpass_confirm0#>");
			document.form.http_passwd_x.value = "";
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;
	}
	
	//confirm common string combination	#JS_common_passwd#
	var is_common_string = check_common_string(document.form.http_passwd_x.value, "httpd_password");
	if(document.form.http_passwd_x.value.length > 0 && is_common_string){
		if(!confirm("<#JS_common_passwd#>")){
			document.form.http_passwd_x.focus();
			document.form.http_passwd_x.select();
			return false;	
		}	
	}

	return true;	
}

function submitForm(){
	var postData = {"restart_httpd": "0", "new_username":document.form.http_username_x.value, "new_passwd":document.form.http_passwd_x.value};
	var sw_mode = '<% nvram_get("sw_mode"); %>';

	if(sw_mode == 3 && '<% nvram_get("wlc_psta"); %>' == 2)
		sw_mode = 2;

	if(validForm()){
		document.getElementById("error_status_field").style.display = "none";
		document.getElementById('btn_modify').style.display = "none";
		document.getElementById('loadingIcon').style.display = '';

		setTimeout(function(){
			httpApi.chpass(postData);
		}, 100);

		setTimeout(function(){
			if('<% nvram_get("w_Setting"); %>' == '0' && sw_mode != 2)
				location.href = '/QIS_wizard.htm?flag=wireless';
			else
				location.href = "/";
		}, 3000);
	}
	else
		return;
}	


var validator = {
	chkLoginId: function(obj){
		var re = new RegExp("^[a-zA-Z0-9][a-zA-Z0-9\-\_]+$","gi");

		if(obj.value == ""){
			showError("<#File_Pop_content_alert_desc1#>");
			obj.value = "";
			obj.focus();
			obj.select();
			return false;
		}
		else if(re.test(obj.value)){
			if(obj.value == "root" || obj.value == "guest" || obj.value == "anonymous"){
				showError("<#USB_Application_account_alert#>");
				obj.value = "";
				obj.focus();
				obj.select();
				return false;
			}

			return true;
		}
		else{
			if(obj.value.length < 2)
				showError("<#JS_short_username#>");
			else
				showError("<#JS_validhostname#>");
			obj.value = "";
			obj.focus();
			obj.select();
			return false;
		}
	},

	chkLoginPw: function(obj){
		
		if(obj.value.length > 0 && obj.value.length < 5){
			showError("<#JS_short_password#>");
			obj.focus();
			obj.select();
			return false;
		}		

		var str_valid_max_password = `<#JS_max_password_var#>`;
		str_valid_max_password = str_valid_max_password.replace("%1$@", "5");
		if(obj.value.length > 32){
			showError(str_valid_max_password);
			obj.focus();
			obj.select();
			return false;
		}

		if(obj.value.charAt(0) == '"'){
			showError('<#JS_validstr1#> ["]');
			obj.focus();
			obj.select();
			return false;
		}
		else{
			var invalid_char = ""; 
			for(var i = 0; i < obj.value.length; ++i){
				if(obj.value.charAt(i) < ' ' || obj.value.charAt(i) > '~'){
					invalid_char = invalid_char+obj.value.charAt(i);
				}
			}

			if(invalid_char != ""){
				showError("<#JS_validstr2#> '"+invalid_char+"' !");
				obj.focus();
				obj.select();
				return false;
			}
		}		

		return true;
	},
	
	chkLoginPw_KR: function(obj){		//KR: Alphabets, numbers, specialcharacters mixed. 8 chars at least.
						//S2: Mixed 2 out of Alphabets(Upper/Lower case), numbers, specialcharacters.
						//	  10 chars at least. Not have consecutive identical characters.
		var string_length = obj.value.length;		
		
		if(!/[A-Za-z]/.test(obj.value) || !/[0-9]/.test(obj.value) || string_length < 10
				|| !/[\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]/.test(obj.value)
				|| /([A-Za-z0-9\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~])\1/.test(obj.value)
		){
				
				showError("<#JS_validLoginPWD#>");
				obj.focus();
				obj.select();
				return false;	
		}

		var str_valid_max_password = `<#JS_max_password_var#>`;
		str_valid_max_password = str_valid_max_password.replace("%1$@", "10");
		if(obj.value.length > 32){
            showError(str_valid_max_password);
            obj.focus();
            obj.select();
            return false;
        }	
		
		var invalid_char = "";
		for(var i = 0; i < obj.value.length; ++i){
			if(obj.value.charAt(i) <= ' ' || obj.value.charAt(i) > '~'){
				invalid_char = invalid_char+obj.value.charAt(i);
			}
		}

		if(invalid_char != ""){
			showError("<#JS_validstr2#> '"+invalid_char+"' !");
			obj.focus();
			obj.select();
			return false;
		}
		
		return true;
	}
}

function showError(str){
	document.getElementById("error_status_field").style.display = "";
	document.getElementById("error_status_field").innerHTML = '<div class="error-text">'+ str + '</div>';
}
</script>
</head>
<body class="bg_mainLogin" onload="initial();">
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="saveNvram">
<input type="hidden" name="action_wait" value="0">
<input type="hidden" name="current_page" value="Main_Login.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="flag" value="">
<input type="hidden" name="login_authorization" value="">
<input name="foilautofill" style="display: none;" type="password">
<input type="hidden" name="http_username" value="">
<input type="hidden" name="http_passwd" value="">
<input type="hidden" name="cfg_pause" value="0">
<div class="main-field-bg">
	<div class="main-field-padding">
		<div class="logo-container">
			<div class="logo-rog"></div>
			<div class="login-title"><#PASS_changepasswd#></div>
		</div>
		<div class="login-title-desc">
			<div class="desc"><#Web_Title2#> is currently not protected and uses an unsafe default username and password.</div>
			<div class="desc"><#QIS_pass_desc1#></div>
			<div id="KRHint" class="desc" style="display: none;"><#JS_validLoginPWD#></div>
		</div>
		<div>
			<div id="router_name_tr" style="display:none" class="p1"><#Router_Login_Name#></div>
			<div class="input-container">
				<input type="text" id="http_username_x" name="http_username_x" tabindex="1" class="form-input" maxlength="32" autocapitalize="off" autocomplete="off" placeholder="<#Router_Login_Name#>">
			</div>
			<div id="router_password_tr" style="display:none" class="p1"><#PASS_new#></div>
			<div class="input-container">
				<input type="password" id="http_passwd_x" name="http_passwd_x" tabindex="2" class="form-input" maxlength="33" autocapitalize="off" autocomplete="off" placeholder="<#PASS_new#>">
			</div>
			<div id="router_password_confirm_tr" style="display:none" class="p1"><#Confirmpassword#></div>
			<div class="input-container">
				<input type="password" id="http_passwd_2_x" name="http_passwd_2_x" tabindex="3" class="form-input" maxlength="33" autocapitalize="off" autocomplete="off" placeholder="<#Confirmpassword#>">
			</div>
			<div style="font-size: 16pt; display:none">
				<input id="defpassCheckbox" type="checkbox" style="height:30px;width:30px;vertical-align: middle;">Use the default settings
			</div>
			<script>
				$("#defpassCheckbox").change(function(){
					var status = $(this).is(':checked');
					if(status){
						$("[name='http_username_x']")
							.val("")
							.prop('disabled', true)
							.css({opacity: "0.3"})

						$("[name='http_passwd_x']")
							.val("")
							.prop('disabled', true)
							.css({opacity: "0.3"})

						$("[name='http_passwd_2_x']")
							.val("")
							.prop('disabled', true)
							.css({opacity: "0.3"})
					}
					else{
						$("[name='http_username_x']")
							.prop('disabled', false)
							.css({opacity: "1"})

						$("[name='http_passwd_x']")
							.prop('disabled', false)
							.css({opacity: "1"})

						$("[name='http_passwd_2_x']")
							.prop('disabled', false)
							.css({opacity: "1"})						
					}
				})

				if(isSupport("defpass")){
					$("#defpassCheckbox").parent().show();
					$("#defpassCheckbox").prop('checked', true).change()
				}
			</script>
			<div id="error_status_field" class="error-hint-bg" style="display: none;" ></div>
			<div id="btn_modify" class="login-btn-bg" onclick="submitForm();"><#CTL_modify#></div>
			<div id="loadingIcon" class="loading-icon" style="display:none;">
				<img style="width:35px;height:35px;" src="/images/InternetScan.gif">
			</div>
		</div>
	</div>
</div>
</form>
</body>
</html>

