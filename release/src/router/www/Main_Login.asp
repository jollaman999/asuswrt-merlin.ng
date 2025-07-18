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
<script type="text/javascript" src="/js/jquery.js"></script>
<script type="text/javascript" src="/js/https_redirect/https_redirect.js"></script>
<title>ASUS Login</title>
<style>
html::-webkit-scrollbar{
    display: block;
    width: 4px;
    height: 4px;
    padding: 2px;
}
html::-webkit-scrollbar-thumb{
    background-color: #248DFF !important;   
    border-radius: 50px;
}
html::-webkit-scrollbar-track{
    background-color: #192229 !important;
}
body{
	font-family: Arial, MS UI Gothic, MS P Gothic, Microsoft Yahei UI, sans-serif;
}
.wrapper{
	background:url(images/New_ui/login_bg.png) #283437 no-repeat;
	background-size: 1280px 1076px;
	background-position: center 0%;
	margin: 0px; 
	background:#283437\9;
}
.title_name {
	font-size: 30pt;
	color:#93d2d9;
}
.prod_madelName{
	font-size: 20pt;
	color:#fff;
	margin-left:78px;
	margin-top: 10px;
}
.login_img{
	width:43px;
	height:43px;
	background-image: url('images/New_ui/icon_titleName.png');
	background-repeat: no-repeat;
}
.p1{
	font-size: 12pt;
	color:#fff;
	width:480px;
}
.button{
	background-color:#279FD9;
	border-radius: 4px ;
	transition: visibility 0s linear 0.218s,opacity 0.218s,background-color 0.218s;
	height: 48px;
	width: 200px;
	font-size: 14pt;
	color:#fff;
	text-align: center;
	float:right; 
	margin:25px 79px 0px 39px;
	line-height:48px;
}
.button:hover{
	cursor:pointer;
}
.button.disabled{
	filter: grayscale(100%) opacity(0.5);
    pointer-events: none;
}
.form_input{
	background-color:rgba(255,255,255,0.2);
	background-color:#576D73\9;
	border-radius: 4px;
	padding:13px 11px;
	width: 380px;
	border: 0;
	height:25px;
	color:#fff;
	font-size:18px;
}
.nologin{
	margin:10px 0px 0px 78px;
	background-color:rgba(255,255,255,0.2);
	padding:20px;
	line-height:18px;
	border-radius: 5px;
	width: 380px;
	border: 0;
	color:#FFF;
	color:#FFF\9; /* IE6 IE7 IE8 */
	font-size:16px;
}
.div_table{
	display:table;
}
.div_tr{
	display:table-row;
}
.div_td{
	display:table-cell;
}
.title_gap{
	margin:20px 0px 0px 78px;
}
.img_gap{
	padding-right:30px;
	vertical-align:middle;
}
.password_gap{
	margin:30px 0px 0px 78px;
}
.error_hint{
	color: rgb(255, 204, 0);
	margin:10px 0px -10px 78px; 
	font-size: 12px;
}

.error_hint1{
	margin:40px 0px -10px 78px; 
	font-size: 24px;
	line-height:32px;
	width: 580px;
}

.main_field_gap{
	margin:100px auto 0;
}
.warming_desc{
	font-size: 12px;
	margin:10px 0px -10px 78px;
	color:#FC0;
	width: 600px;
}

#captcha_img_div{
	margin: 30px 0px 0px 30px;
	width: 160px;
	height: 60px;
	border-radius: 4px;
	background-color:#FFF;
	float: left;
}

#captcha_pic{
	width: 90%;
	height:90%;
	margin: 3px 0px 0px 0px;
}

#captcha_input_div{
	margin: 30px 0px 0px 78px;
	float: left;
}

#captcha_text{
	width: 245px;
	background-color: rgba(255,255,255,0.2);
	background-color: #576D73\9;
	border-radius: 4px;
	padding: 15px 22px;
	border: 0;
	height: 30px;
	color: #fff;
	font-size: 28px;
}

#reCaptcha{
	margin: 45px 0px 0px 10px;
	width: 30px;
	height: 30px;
	float: left;
	background-image: url("data:image/svg+xml;charset=US-ASCII,%3C%3Fxml%20version%3D%221.0%22%20encoding%3D%22iso-8859-1%22%3F%3E%0A%3C!DOCTYPE%20svg%20PUBLIC%20%22-%2F%2FW3C%2F%2FDTD%20SVG%201.1%2F%2FEN%22%20%22http%3A%2F%2Fwww.w3.org%2FGraphics%2FSVG%2F1.1%2FDTD%2Fsvg11.dtd%22%3E%0A%3Csvg%20version%3D%221.1%22%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20xmlns%3Axlink%3D%22http%3A%2F%2Fwww.w3.org%2F1999%2Fxlink%22%20width%3D%2232%22%20height%3D%2232%22%20viewBox%3D%220%200%2032%2032%22%3E%0A%3Cpath%20fill%3D%22%23ABB2BB%22%20d%3D%22M31.638%2022.702c-0.181-0.664-0.906-0.604-1.509-0.785l-1.208-0.423c0.966-1.992%201.087-3.321%201.087-5.555%200-8.211-6.642-14.853-14.853-14.853s-14.853%206.702-14.853%2014.913%206.641%2014.853%2014.853%2014.853c0.664%200%201.208-0.543%201.208-1.208s-0.543-1.208-1.208-1.208c-6.823%200-12.438-5.555-12.438-12.438s5.615-12.438%2012.438-12.438%2012.438%205.555%2012.438%2012.438c0%201.872-0.060%202.838-0.845%204.528l-0.906-0.785c-0.604-0.423-0.906-0.966-1.509-0.785-0.664%200.181-0.966%200.906-0.785%201.509l1.449%204.408c0.181%200.543%200.664%200.845%201.147%200.845%200.121%200%200.241%200%200.362-0.060l4.408-1.449c0.604-0.181%200.966-0.845%200.725-1.509v0z%22%3E%3C%2Fpath%3E%0A%3C%2Fsvg%3E%0A");
	background-repeat: no-repeat;
}

/*for mobile device*/
@media screen and (max-width: 1000px){
	.title_name {
		font-size: 20pt;
		color:#93d2d9;
		margin-left:15px;
	}
	.prod_madelName{
		font-size: 13pt;
		margin-left: 15px;
	}
	.p1{
		font-size: 12pt;
		width:100%;
	}
	.login_img{
		background-size: 75%;
	}
	.form_input{	
		padding:10px 11px;
		width: 100%;
		height:30px;
		font-size:16px
	}
	.button{
		height: 50px;
		width: 100%;
		font-size: 14pt;
		text-align: center;
		float:right; 
		margin: 25px -22px 40px 15px;
		line-height:50px;
		padding-left: 7px;
	}
	.nologin{
		margin-left:10px; 
		padding:10px;
		line-height:18px;
		width: 100%;
		font-size:14px;
	}
	.error_hint{
		margin-left:10px; 
	}
	.error_hint1{
		width: 100%;
		font-size:14px;
	}
	.main_field_gap{
		width:80%;
		margin:30px 0 0 15px;
		/*margin:30px auto 0;*/
	}
	.title_gap{
		margin-left:15px; 
	}
	.password_gap{
		margin-left:15px; 
	}
	.img_gap{
		padding-right:0;
		vertical-align:middle;
	}
	.warming_desc{
		margin: 10px 15px;
		width: 100%; 
	}

	#captcha_img_div{
		margin-left: 10px;
		width: 30%;
		height: 40px;
		border-radius: 4px;
		background-color:#FFF;
		float: left;
	}

	#captcha_pic{
		width: 90%;
		height: 90%;
		margin: 2px 0px 0px 3px;
	}

	#captcha_input_div{
		margin-left: 15px;
		width: 47%;
		height: 40px;
	}

	#captcha_text{
		padding: 0px 11px;
		width: 80%;
		height: 40px;
		font-size: 16px
	}

	#reCaptcha{
		margin-top: 35px;
	}
}

@media screen and (max-width: 320px){
	#captcha_input_div{
		width: 44%;
	}
}
</style>
<script>
if('<% nvram_get("productid"); %>' == 'PRT-AX57_GO')
	$('link').last().after('<link rel="stylesheet" type="text/css" href="/main_login.css">');

/* add Array.prototype.forEach() in IE8 */
if(typeof Array.prototype.forEach != 'function'){
	Array.prototype.forEach = function(callback){
		for(var i = 0; i < this.length; i++){
			callback.apply(this, [this[i], i, this]);
		}
	};
}

function tryParseJSON (jsonString){
    try {
        var o = JSON.parse(jsonString);

        if (o && typeof o === "object") {
            return o;
        }
    }
    catch (e) {
        // do something
    }

    return false;
}

var htmlEnDeCode = (function() {
	var charToEntityRegex,
		entityToCharRegex,
		charToEntity,
		entityToChar;

	function resetCharacterEntities() {
		charToEntity = {};
		entityToChar = {};
		// add the default set
		addCharacterEntities({
			'&amp;'	 :   '&',
			'&gt;'	  :   '>',
			'&lt;'	  :   '<',
			'&quot;'	:   '"',
			'&#39;'	 :   "'"
		});
	}

	function addCharacterEntities(newEntities) {
		var charKeys = [],
			entityKeys = [],
			key, echar;
		for (key in newEntities) {
			echar = newEntities[key];
			entityToChar[key] = echar;
			charToEntity[echar] = key;
			charKeys.push(echar);
			entityKeys.push(key);
		}
		charToEntityRegex = new RegExp('(' + charKeys.join('|') + ')', 'g');
		entityToCharRegex = new RegExp('(' + entityKeys.join('|') + '|&#[0-9]{1,5};' + ')', 'g');
	}

	function htmlEncode(value){
		var htmlEncodeReplaceFn = function(match, capture) {
			return charToEntity[capture];
		};

		return (!value) ? value : String(value).replace(charToEntityRegex, htmlEncodeReplaceFn);
	}

	function htmlDecode(value) {
		var htmlDecodeReplaceFn = function(match, capture) {
			return (capture in entityToChar) ? entityToChar[capture] : String.fromCharCode(parseInt(capture.substr(2), 10));
		};

		return (!value) ? value : String(value).replace(entityToCharRegex, htmlDecodeReplaceFn);
	}

	resetCharacterEntities();

	return {
		htmlEncode: htmlEncode,
		htmlDecode: htmlDecode
	};
})();

var login_info =  tryParseJSON('<% login_error_info(); %>');
var isIE8 = navigator.userAgent.search("MSIE 8") > -1; 
var isIE9 = navigator.userAgent.search("MSIE 9") > -1; 
var remaining_time = login_info.lock_time;
var remaining_time_min;
var remaining_time_sec;
var remaining_time_show;
var countdownid, rtime_obj;
var redirect_page = login_info.page;
var isRouterMode = (htmlEnDeCode.htmlEncode(decodeURIComponent('<% nvram_char_to_ascii("","sw_mode"); %>')) == '1') ? true : false;

const getQueryString = function(name){
	var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
	var r = window.location.search.substr(1).match(reg);
	if (r != null) return unescape(r[2]); return null;
};
function loadScript(src, timeout = 2000) {
    return new Promise((resolve, reject) => {
        const script = document.createElement('script');
        script.src = src;
        script.onload = () => resolve(script);
        script.onerror = () => reject(new Error(`Failed to load script ${src}`));
        document.head.appendChild(script);
        setTimeout(() => {
			loadScriptTimeout = true;
            reject(new Error(`Loading script ${src} timed out`));
        }, timeout);
    });
}
var loadScriptTimeout = false;
var header_info = [<% get_header_info(); %>][0];
var ROUTERHOSTNAME = '<#Web_DOMAIN_NAME#>';
var domainNameUrl = `${header_info.protocol}://${ROUTERHOSTNAME}:${header_info.port}`;
var chdom = function(){if(getQueryString("redirct")!=="false" && !loadScriptTimeout)window.location.href=domainNameUrl};
if ('<% nvram_get("http_dut_redir"); %>' == '1') {
	if(ROUTERHOSTNAME !== header_info.host && ROUTERHOSTNAME != "" && isRouterMode){
		setTimeout(() => {
			loadScript(`${domainNameUrl}/chdom.json?hostname=${header_info.host}`).catch(error => {console.error(error.message);});
		}, 100);
	}
}

function isSupport(_ptn){
	var ui_support = [<% get_ui_support(); %>][0];
	return (ui_support[_ptn]) ? ui_support[_ptn] : 0;
}
var captcha_support = isSupport("captcha");
var captcha_enable = htmlEnDeCode.htmlEncode(decodeURIComponent('<% nvram_char_to_ascii("", "captcha_enable"); %>'));
if(captcha_support && captcha_enable != "0")
	var captcha_on = (login_info.error_num >= 2 && login_info.error_status != "7" && login_info.error_status != "11")? true : false;
else
	var captcha_on = false;

var faq_href = "https://nw-dlcdnet.asus.com/support/forward.html?model=&type=SG_TeleStand&lang=&kw=&num=";
var ATEMODE = '<% nvram_get("ATEMODE"); %>';

function initial(){
	top.name = "";/* reset cache of state.js win.name */

	if(ATEMODE == "1"){
		$(".div_td.signin_hint").text("<#CTL_signin#>"+" (ATE MODE)");
	}

	if(isSupport("ROG_UI") || isSupport("TS_UI") || isSupport("TUF_UI")){
		$(".wrapper").css({"background-size":"cover"});
	}
	var flag = login_info.error_status;
	if(isIE8 || isIE9){
		document.getElementById("name_title_ie").style.display ="";
		document.getElementById("password_title_ie").style.display ="";
	}

	if(flag != 11 && login_info.last_time_lock_warning){
		document.getElementById("last_time_lock_warning").style.display ="";
		document.getElementById("last_time_lock_warning").innerHTML ="You have entered an incorrect username or password 9 times. If there's one more failed account or password attempt, your router will be blocked from accessing, and need to be reset to factory setting.";
	}

	if(flag != ""){
		document.getElementById("error_status_field").style.display ="";

		if(flag == 3){
			document.getElementById("error_status_field").innerHTML ="* <#JS_validLogin#>";
		}
		else if(flag == 7){
			document.getElementById("error_status_field").innerHTML ="You have entered an incorrect username or password 5 times. Please try again after "+"<span id='rtime'></span>"+" seconds.";
			document.getElementById("error_status_field").className = "error_hint error_hint1";
			disable_input(1);
			disable_button(1);
			rtime_obj=document.getElementById("rtime");
			countdownfunc();
			countdownid = window.setInterval(countdownfunc,1000);
		}
		else if(flag == 8){
			document.getElementById("login_filed").style.display ="none";
			document.getElementById("logout_field").style.display ="";
		}
		else if(flag == 9){
			document.getElementById("login_filed").style.display ="none";
			document.getElementById("nologin_field").style.display ="";
		}
		else if(flag == 10){
			document.getElementById("error_captcha_field").style.display ="";
		}
		else if(flag == 11){
			document.getElementById("error_status_field").innerHTML ="For security reasons, this router has been locked out because of 10 times of incorrect username and password attempts.<br>To unlock, please manually reset your router to factory setting by pressing the reset button on the back.<br>Click <a id=\"faq_SG\" href=\"\" target=\"_blank\" style=\"color:#FC0;text-decoration:underline;\">here</a> for more details.";
			document.getElementById("faq_SG").href = faq_href;
			document.getElementById("error_status_field").className = "error_hint error_hint1";
			disable_input(1);
			disable_button(1);
		}
		else{
			document.getElementById("error_status_field").style.display ="none";
		}
	}

	document.form.login_username.focus();

	/*register keyboard event*/
	document.form.login_username.onkeyup = function(e){
		e=e||event;
		if(e.keyCode == 13){
			document.form.login_passwd.focus();
			return false;
		}
	};
	document.form.login_username.onkeypress = function(e){
		e=e||event;
		if(e.keyCode == 13){
			return false;
		}
	};

	document.form.login_passwd.onkeyup = function(e){
		e=e||event;
		if(e.keyCode == 13){
			if(captcha_on)
				document.form.captcha_text.focus();
			else
				preLogin();
			return false;
		}
	};
	document.form.login_passwd.onkeypress = function(e){
		e=e||event;
		if(e.keyCode == 13){
			return false;
		}
	};

	if(captcha_on){
		var timestamp = new Date().getTime();
		var captcha_pic = document.getElementById("captcha_pic");
		captcha_pic.src = "captcha.gif?t=" + timestamp;

		document.form.captcha_text.onkeyup = function(e){
			e=e||event;
			if(e.keyCode == 13){
				preLogin();
				return false;
			}
		};

		document.form.captcha_text.onkeypress = function(e){
			e=e||event;
			if(e.keyCode == 13){
				return false;
			}
		};

		document.getElementById("captcha_field").style.display = "";
	}
	else
		document.getElementById("captcha_field").style.display = "none";

	if(history.pushState != undefined) history.pushState("", document.title, window.location.pathname + window.location.search);
}

function countdownfunc(){
	remaining_time_min = checkTime(Math.floor(remaining_time/60));
	remaining_time_sec = checkTime(Math.floor(remaining_time%60));
	remaining_time_show = remaining_time_min +":"+ remaining_time_sec;
	rtime_obj.innerHTML = remaining_time_show;
	if (remaining_time==0){
		clearInterval(countdownid);
		setTimeout("top.location.href='/Main_Login.asp';", 2000);
	}
	remaining_time--;
}

function preLogin(){
    if(document.querySelector('#button')?.classList.contains('disabled') || document.querySelector('.button')?.classList.contains('disabled')) return;
    document.querySelector('#button')?.classList.add('disabled');
    document.querySelector('.button')?.classList.add('disabled');
    let id = randomString(10);
    fetch('get_Nonce.cgi', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({id: id})
    })
    .then(response => response.json())
    .then(data => {
        const { nonce } = data;
        login(id, nonce);
    })
    .catch(error => top.location.href='/Main_Login.asp');
}

function login(id, nonce){

    const cnonce = randomString(32);

	var trim = function(val){
		val = val+'';
		for (var startIndex=0;startIndex<val.length && val.substring(startIndex,startIndex+1) == ' ';startIndex++);
		for (var endIndex=val.length-1; endIndex>startIndex && val.substring(endIndex,endIndex+1) == ' ';endIndex--);
		return val.substring(startIndex,endIndex+1);
	}

	if(!window.btoa){
		window.btoa = function(input){
			var keyStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
			var output = "";
			var chr1, chr2, chr3, enc1, enc2, enc3, enc4;
			var i = 0;
			var utf8_encode = function(string) {
				string = string.replace(/\r\n/g,"\n");
				var utftext = "";
				for (var n = 0; n < string.length; n++) {
					var c = string.charCodeAt(n);
					if (c < 128) {
						utftext += String.fromCharCode(c);
					}
					else if((c > 127) && (c < 2048)) {
						utftext += String.fromCharCode((c >> 6) | 192);
						utftext += String.fromCharCode((c & 63) | 128);
					}
					else {
						utftext += String.fromCharCode((c >> 12) | 224);
						utftext += String.fromCharCode(((c >> 6) & 63) | 128);
						utftext += String.fromCharCode((c & 63) | 128);
					}
				}
				return utftext;
			};
			input = utf8_encode(input);
			while (i < input.length) {
				chr1 = input.charCodeAt(i++);
				chr2 = input.charCodeAt(i++);
				chr3 = input.charCodeAt(i++);
				enc1 = chr1 >> 2;
				enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
				enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
				enc4 = chr3 & 63;
				if (isNaN(chr2)) {
					enc3 = enc4 = 64;
				}
				else if (isNaN(chr3)) {
					enc4 = 64;
				}
				output = output + 
				keyStr.charAt(enc1) + keyStr.charAt(enc2) + 
				keyStr.charAt(enc3) + keyStr.charAt(enc4);
			}
			return output;
		};
	}

	document.form.id.value = id;
    document.form.cnonce.value = cnonce;
	document.form.login_authorization.value = sha256(`${document.form.login_username.value}:${nonce}:${document.form.login_passwd.value}:${cnonce}`);
	document.form.login_username.disabled = true;
	document.form.login_passwd.disabled = true;
	document.form.login_captcha.value = btoa(document.form.captcha_text.value);
	document.form.captcha_text.disabled = true;

	try{
		if(redirect_page == "" 
			|| redirect_page == "Logout.asp" 
			|| redirect_page == "Main_Login.asp" 
			|| redirect_page.indexOf(" ") != -1 
			|| redirect_page.indexOf("//") != -1 
			|| redirect_page.indexOf("http") != -1
			|| (redirect_page.indexOf(".asp") == -1 && redirect_page.indexOf(".htm") == -1 && redirect_page != "send_IFTTTPincode.cgi" && redirect_page != "cfg_onboarding.cgi" && redirect_page != "ig_s2s_link.cgi")
		){
			document.form.next_page.value = "";
		}
		else{
			document.form.next_page.value = redirect_page;
		}
	}
	catch(e){
		document.form.next_page.value = "";
	}

	document.form.submit();
}

function disable_input(val){
	var disable_input_x = document.getElementsByClassName('form_input');
	for(i=0;i<disable_input_x.length;i++){
		if(val == 0)
			disable_input_x[i].disabled = true;
		else
			disable_input_x[i].style.display = "none";
	}
}

function disable_button(val){
	if(val == 0)
		document.getElementsByClassName('button')[0].disabled = true;
	else
		document.getElementsByClassName('button')[0].style.display = "none";
}

function checkTime(i){
	if (i<10){
		i="0" + i
	}
	return i
}

function regen_captcha(){
	var timestamp = new Date().getTime();
	var captcha_pic = document.getElementById("captcha_pic");
	var queryString = "?t=" + timestamp;
	captcha_pic.src = "captcha.gif" + queryString;
}

function randomString(length) {
    let chars = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    let result = '';
    for (var i = length; i > 0; --i) result += chars[Math.floor(Math.random() * chars.length)];
    return result;
}

const sha256 = function a(b){function c(a,b){return a>>>b|a<<32-b}for(var d,e,f=Math.pow,g=f(2,32),h="length",i="",j=[],k=8*b[h],l=a.h=a.h||[],m=a.k=a.k||[],n=m[h],o={},p=2;64>n;p++)if(!o[p]){for(d=0;313>d;d+=p)o[d]=p;l[n]=f(p,.5)*g|0,m[n++]=f(p,1/3)*g|0}for(b+="\x80";b[h]%64-56;)b+="\x00";for(d=0;d<b[h];d++){if(e=b.charCodeAt(d),e>>8)return;j[d>>2]|=e<<(3-d)%4*8}for(j[j[h]]=k/g|0,j[j[h]]=k,e=0;e<j[h];){var q=j.slice(e,e+=16),r=l;for(l=l.slice(0,8),d=0;64>d;d++){var s=q[d-15],t=q[d-2],u=l[0],v=l[4],w=l[7]+(c(v,6)^c(v,11)^c(v,25))+(v&l[5]^~v&l[6])+m[d]+(q[d]=16>d?q[d]:q[d-16]+(c(s,7)^c(s,18)^s>>>3)+q[d-7]+(c(t,17)^c(t,19)^t>>>10)|0),x=(c(u,2)^c(u,13)^c(u,22))+(u&l[1]^u&l[2]^l[1]&l[2]);l=[w+x|0].concat(l),l[4]=l[4]+w|0}for(d=0;8>d;d++)l[d]=l[d]+r[d]|0}for(d=0;8>d;d++)for(e=3;e+1;e--){var y=l[d]>>8*e&255;i+=(16>y?0:"")+y.toString(16)}return i};

</script>
</head>
<body class="wrapper" onload="initial();">
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>
<iframe id="dmRedirection" width="0" height="0" frameborder="0" scrolling="no" src=""></iframe>

<form method="post" name="form" action="login_v2.cgi" target="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="action_wait" value="5">
<input type="hidden" name="current_page" value="Main_Login.asp">
<input type="hidden" name="next_page" value="Main_Login.asp">
<input type="hidden" name="login_authorization" value="">
<input type="hidden" name="id" value="">
<input type="hidden" name="cnonce" value="">
<input type="hidden" name="login_captcha" value="">
<div class="div_table main_field_gap">
	<div class="div_tr">
		<div class="title_name">
			<div class="div_td img_gap">
				<div class="login_img"></div>
			</div>
			<div class="div_td signin_hint"><#CTL_signin#></div>
		</div>	
		<div class="prod_madelName"><#Web_Title2#></div>

		<!-- Login field -->
		<div id="login_filed">
			<div class="p1 title_gap"><#Sign_in_title#></div>

			<div id="name_title_ie" style="display:none;margin:20px 0 -10px 78px;" class="p1 title_gap"><#Username#></div>
			<div class="title_gap">
				<input type="text" id="login_username" name="login_username" tabindex="1" class="form_input" maxlength="128" autocapitalize="off" autocomplete="off" placeholder="<#Username#>">
			</div>
			<div id="password_title_ie" style="display:none;margin:20px 0 -20px 78px;" class="p1 title_gap"><#HSDPAConfig_Password_itemname#></div>
			<div class="password_gap">
				<input type="password" name="login_passwd" tabindex="2" class="form_input" maxlength="128" placeholder="<#HSDPAConfig_Password_itemname#>" autocapitalize="off" autocomplete="off">
			</div>
			<div class="error_hint" style="display:none;" id="error_status_field"></div>
			<div class="warming_desc" style="display:none;" id="last_time_lock_warning"></div>
			<div id="captcha_field" style="display: none;">
				<div id="captcha_input_div"><input id ="captcha_text" name="captcha_text" tabindex="3" maxlength="5" autocapitalize="off" autocomplete="off"></div>
				<div id="captcha_img_div"><img id="captcha_pic"></div>
				<div id="reCaptcha" onclick="regen_captcha();"></div>
				<div class="error_hint" style="display:none; clear:left;" id="error_captcha_field">Captcha is wrong. Please input again.</div>
			</div>
			<input type="button" value="<#CTL_signin#>" class="button" style="border-style: hidden;" onclick="preLogin();">
		</div>

		<!-- No Login field -->
		<div id="nologin_field" style="display:none;">
			<div class="p1 title_gap"></div>
			<div class="nologin">
				<#login_hint2#>
				<div id="logined_ip_str"></div>
			</div>
		</div>

		<!-- Logout field -->
		<div id="logout_field" style="display:none;">
			<div class="p1 title_gap"></div>
			<div class="nologin"><#logoutmessage#>
				<br><br>Click <a style="color: #FFFFFF; font-weight: bolder;text-decoration:underline;" class="hyperlink" href="Main_Login.asp">here</a> to log back in.
			</div>
		</div>
	</div>
</div>
</form>
</body>
</html>
