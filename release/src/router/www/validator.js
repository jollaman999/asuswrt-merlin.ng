﻿
var validator = {

	ipv4cidr: function(obj){
		var rangere_cidr=new RegExp("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])(\/([0-9]|[1-2][0-9]|3[0-2]))$", "gi");
		if(rangere_cidr.test(obj.value) || validator.ipAddr4(obj)) {
			return true;
		}else{
			alert(obj.value+" is not valid.  Please enter a valid IP, which can optionally be in CIDR format (1.2.3.4/24).");
			obj.focus();
			obj.select();
			return false;
		}
	},

	account: function(string_obj, flag){
		var invalid_char = "";

		if(string_obj.value.charAt(0) == ' '){
			if(flag != "noalert")
				alert('<#JS_validstr1#> [  ]');

			string_obj.value = "";
			string_obj.focus();

			if(flag != "noalert")
				return false;
			else
				return '<#JS_validstr1#> [&nbsp;&nbsp;&nbsp;]';
		}
		else if(string_obj.value.charAt(0) == '-'){
			if(flag != "noalert")
				alert('<#JS_validstr1#> [-]');

			string_obj.value = "";
			string_obj.focus();

			if(flag != "noalert")
				return false;
			else
				return '<#JS_validstr1#> [-]';
		}

		for(var i = 0; i < string_obj.value.length; ++i){
			if(this.ssidChar(string_obj.value.charCodeAt(i))){
				invalid_char = invalid_char+string_obj.value.charAt(i);
			}	
			
			if(string_obj.value.charAt(i) == '"'
					||  string_obj.value.charAt(i) == '/'
					||  string_obj.value.charAt(i) == '\\'
					||  string_obj.value.charAt(i) == '['
					||  string_obj.value.charAt(i) == ']'
					||  string_obj.value.charAt(i) == ':'
					||  string_obj.value.charAt(i) == ';'
					||  string_obj.value.charAt(i) == '|'
					||  string_obj.value.charAt(i) == '='
					||  string_obj.value.charAt(i) == ','
					||  string_obj.value.charAt(i) == '+'
					||  string_obj.value.charAt(i) == '*'
					||  string_obj.value.charAt(i) == '?'
					||  string_obj.value.charAt(i) == '<'
					||  string_obj.value.charAt(i) == '>'
					||  string_obj.value.charAt(i) == '@'
					||  string_obj.value.charAt(i) == ' '
					){
				invalid_char = invalid_char+string_obj.value.charAt(i);
			}
		}

		if(invalid_char != ""){
			if(flag != "noalert")
				alert("<#JS_validstr2#> ' "+invalid_char+" ' !");

			string_obj.value = "";
			string_obj.focus();

			if(flag != "noalert")
				return false;
			else
				return "<#JS_validstr2#> ' "+invalid_char+" ' !";
		}

		if(flag != "noalert")
			return true;
		else
			return "";
	},

	bandwidth_code: function(o,event) {
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		var target = o.value.split(".");
		
		if (validator.isFunctionButton(event))
			return true;
			
		if((keyPressed == 46) && (target.length > 1))
			return false;

		if((target.length > 1) && (target[1].length > 0))
			return false;
			
		if ((keyPressed == 46) || (keyPressed > 47 && keyPressed < 58))
			return true;
		else
			return false;
	},

	checkIP: function(o,e){
		var nextInputBlock = o.nextSibling.nextSibling; //find the next input (sibling include ".")
		var nc = window.event ? e.keyCode : e.which;
		var s = o.value;
		if((nc>=48 && nc<=57) || (nc>=97 && nc<=105) || nc==8 || nc==0)
		{
			return true;
		}
		else if(nc==190 || nc==32 || nc==110 || nc==46){
			//Nothing
		}else if((nc==37 || nc==8) && s==""){
			//Nothing
		}	
		else{
			nc=0;
			return false;
		}
		//Range handler
	},

	checkIPAddrInput: function(obj, emp){	
		if(document.getElementById("check_ip_input"))
			obj.parentNode.removeChild(obj.parentNode.childNodes[2]);	
		if(!this.ipAddr4(obj) || (emp == 1 && obj.value == "")){
			var childsel=document.createElement("div");
			childsel.setAttribute("id","check_ip_input");
			childsel.style.color="#FFCC00";
			obj.parentNode.appendChild(childsel);
			document.getElementById("check_ip_input").innerHTML="<#JS_validip#>";		
			document.getElementById("check_ip_input").style.display = "";
			obj.value = obj.parentNode.childNodes[0].innerHTML;
			obj.focus();
			obj.select();
			return false;	
		}else
			return true;
	},

	checkWord: function(o,e){
		var moveLeft_key = 0;
		var moveRight_key = 0;

		var getCaretPos =  function(obj){
		 	if(obj.selectionStart >= 0){
		 		return obj.selectionStart; //Gecko
		 	}
		 	else{                        //For IE
		 		var currentRange=document.selection.createRange();   
		 		var workRange=currentRange.duplicate();
		 		obj.select();
		 		var allRange=document.selection.createRange();   
		 		var len=0;   
		 		while(workRange.compareEndPoints("StartToStart",allRange)>0){   
		 			workRange.moveStart("character",-1);   
		 			len++;   
		 		}
		 		currentRange.select();   
		 		return	len;
		 	}
		}

		var validateRange = function(v){
			if(v.value < 0 || v.value >= 256){
				alert(v.value+" <#BM_alert_IP2#>. \n<#BM_alert_port1#> 0<#BM_alert_to#>255.");
				v.focus();
				v.select();
				return false;
			}else{
				return true;
			}
		};
		
		var nextInputBlock = o.nextSibling.nextSibling; //find the next input (sibling include ".")
		var prevInputBlock = o.previousSibling;
		prevInputBlock = (prevInputBlock != null)?prevInputBlock.previousSibling:o;
		
		var sk = window.event ? e.keyCode : e.which;
		var s = o.value;
		
		if((sk>=48 && sk<=57) || (sk>=97 && sk<=105) || sk==0) // 0->other key
		{
			if(s.length == 3){
				if(getCaretPos(o) == 3 && nextInputBlock){
					if(validateRange(o)){
						nextInputBlock.focus();
						nextInputBlock.select();
					}
				}
				else{
					validateRange(o);
				}
			}
			else if(s.length == getCaretPos(o)){
				moveRight_key = 1;
			}
			moveLeft_key = 0;
		}
		else if(sk==190 || sk==110 || sk==32){ //190 & 110-> "dot", 32->"space"
			if(o.value == '.' || o.value == '..' || o.value == '...'
					|| o.value == ' ' || o.value == '  ' || o.value == '   '){
				o.value = "";
				o.focus();
			}
			else{
				this.validateIP(o);
				if(nextInputBlock){
					nextInputBlock.focus();
					nextInputBlock.select();
				}
			}
		}
		else if(sk==8){ //8->backspace
			if(getCaretPos(o) == 0 && moveLeft_key == 0){
				moveLeft_key = 1;
			}
			else if(getCaretPos(o) == 0){
				if(prevInputBlock){
					prevInputBlock.focus();
					prevInputBlock.select();
					moveLeft_key = 0;
					moveRight_key = 1;
				}
			}
		}
		else if(sk==37){ // 37-> 鍵盤向左鍵
			if(getCaretPos(o) == 0 && moveLeft_key == 0){
				moveLeft_key = 1;
			}
			else if(prevInputBlock && getCaretPos(o) == 0 && moveLeft_key == 1){
				prevInputBlock.focus();
				prevInputBlock.select();
				moveLeft_key = 0;
			}
			else{
				moveLeft_key = 0;
			}
			moveRight_key = 0;
		}
		else if(sk==39){ //39 ->鍵盤向右鍵
			if(getCaretPos(o) == 0 && s.length == 0 && nextInputBlock && validateRange(o)){
				nextInputBlock.focus();
				moveRight_key = 0;
				moveLeft_key = 1;
			}
			else if(getCaretPos(o) == s.length && moveRight_key == 0){
				moveRight_key = 1;
				moveLeft_key = 0;
			}
			else if(moveRight_key == 1 && nextInputBlock && validateRange(o)){
				
				nextInputBlock.focus();
				moveRight_key = 0;
				moveLeft_key = 1;
			}
			else{
				moveLeft_key = 0;
			}
		}
		else{
			if(isNaN(s) && s.length >= 1 && sk != 13){
				alert("<#LANHostConfig_x_DDNS_alarm_9#>");
				o.focus();
				o.select();	
				return false;
			}
			else{
				nc=0;
				return false;
			}
		}	
	},

	eachPort: function(o, num, min, max) {	
		if(num<min || num>max) {
			alert(num + " <#JS_validport#>");
			return false;
		}
		else {
			//o.value = str2val(o.value);
			if(o.value=="")
				o.value="0";
			return true;
		}
	},

	integer: function(_val){
		var obj_value = _val;
		var re = new RegExp("[^0-9]+","gi");
		
		if(re.test(obj_value))
			return false;
		else
			return true;
	},

	hex: function(obj){
		var obj_value = obj.value
		var re = new RegExp("[^a-fA-F0-9]+","gi");
		
		if(re.test(obj_value))
			return false;
		else
			return true;
	},

	haveFullWidthChar: function(obj) {
		var re = /[^\x00-\xff]/g;
		if (obj.value.match(re)) {
			return false;
		}
		else {
			return true;
		}
	},

	hostNameChar: function(ch){
		if (ch>=48&&ch<=57) return true;	//0-9
		if (ch>=97&&ch<=122) return true;	//little EN
		if (ch>=65&&ch<=90) return true; 	//Large EN
		if (ch==45) return true;	//-
		if (ch==46) return true;	//.
		
		return false;
	},

	domainName: function (obj) { //support a-z, 0-9, "-", "_" , "."", The first character cannot be dash "-" or under line "_"
		var re = new RegExp(/^(?:[a-z0-9](?:[a-z0-9-_]{0,61}[a-z0-9])?\.)*[a-z0-9][a-z0-9-_]{0,61}[a-z0-9]$/i);
		if(re.test(obj.value)){
			return "";
		}
		else{
			return "<#JS_valid_FQDN#>";
		}
	},

	host_name: function(obj){
		var re = new RegExp(/^[a-z0-9][a-z0-9-_]+$/i);
		if(re.test(obj.value))
			return "";
		else
			return "<#Login_Name_Rule#>";
	},

	samba_name: function(obj, flag){
		let re = (flag == "computer_name") ? new RegExp(/^[a-z0-9][a-z0-9-]*$/i) : new RegExp(/^[a-z0-9][a-z0-9-_]*$/i);
		let hint = (flag == "computer_name") ? `<#JS_valid_host_name_1#> <#JS_valid_host_name_first_char_1#>` : `<#JS_valid_host_name#> <#JS_valid_host_name_first_char#>`;
		if(re.test(obj.value))
			return "";
		else
			return hint;
	},

	friendly_name: function(obj){
		var invalid_char = "";
		for(var i = 0; i < obj.value.length; ++i){
			if(obj.value.charAt(i) < ' ' || obj.value.charAt(i) > '~')
				invalid_char = invalid_char+obj.value.charAt(i);
		}

		if(invalid_char != "")
			return "<#JS_validstr2#> '"+invalid_char+"' !";
		else
			return "";
	},

	account_name: function(obj){
		var re = new RegExp(/^[a-z][a-z0-9-]*$/i);
		if(re.test(obj.value))
			return "";
		else
			return "<#JS_valid_account_name#> <#JS_valid_account_name_first_char#>";
	},

	requireWANIP: function(v){
		if(v == 'wan_ipaddr_x' || v == 'wan_netmask_x' ||
				v == 'lan_ipaddr' || v == 'lan_netmask' ||
				v == 'lan1_ipaddr' || v == 'lan1_netmask'){
			// 2008.03 James. patch for Oleg's patch. {
			/*if(document.form.wan_proto.value == "static" || document.form.wan_proto.value == "pptp")
				return 1;
			else
				return 0;*/
			if(document.form.wan_proto.value == "static")
				return 1;
			else if(document.form.wan_proto.value == "pppoe" && this.inet_network(document.form.wan_ipaddr_x.value))
				return 1;
			else if((document.form.wan_proto.value=="pptp" || document.form.wan_proto.value == "l2tp")
					&& document.form.wan_ipaddr_x.value != '0.0.0.0')
				return 1;
			else
				return 0;
			// 2008.03 James. patch for Oleg's patch. }
		}
		
		else return 0;
	},

	isEmpty: function(obj) {
		if(obj.value.trim() == "") {
			alert("<#JS_fieldblank#>");
			obj.focus();
			return false;
		}
		else 
			return true;
	},

	isFunctionButton: function(e){
	//function keycode for Firefox/Opera
		var keyCode = e.keyCode;
		if(e.which == 0) {
			if (keyCode == 0
				|| keyCode == 27 //Esc
				|| keyCode == 35 //end
				|| keyCode == 36 //home
				|| keyCode == 37 //<-
				|| keyCode == 39 //->
				|| keyCode == 45 //Insert
				|| keyCode == 46 //Del
				){
				return true;
			}
		}
		if (keyCode == 8 	//backspace
			|| keyCode == 9 	//tab
			){
			return true;
		}
		return false;
	},

	isHWAddr: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		var i, j;
		if (this.isFunctionButton(event)){
			return true;
		}

		if((keyPressed>47 && keyPressed<58)||(keyPressed>64 && keyPressed<71)||(keyPressed>96 && keyPressed<103)){	//Hex
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == ':'){
					j++;
				}
			}
			
			if(j < 5 && i >= 2){
				if(o.value.charAt(i-2) != ':' && o.value.charAt(i-1) != ':'){
					o.value = o.value+':';
				}
			}
			
			return true;
		}	
		else if(keyPressed == 58 || keyPressed == 13){	//symbol ':' & 'ENTER'
			return true;
		}
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}
		else{
			return false;
		}
	},

	isNumberFloat: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;

		if (this.isFunctionButton(event)){
			return true;
		}

		if ((keyPressed == 46) || (keyPressed>47 && keyPressed<58))
			return true;
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}
		else
			return false;
	},

	isNegativeNumber: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;

		if (this.isFunctionButton(event)){
			return true;
		}

		if ((keyPressed == 45) || (keyPressed>47 && keyPressed<58))
			return true;
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}
		else
			return false;
	},

	isNumber: function(o,event){	
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		
		if (this.isFunctionButton(event)){
			return true;
		}

		if (keyPressed>47 && keyPressed<58){
			/*if (keyPressed==48 && o.value.length==0){	//single 0
				return false;	
			}*/
			return true;
		}
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}		
		else{
			return false;
		}
	},

	isContainblanksStr: function(obj) {
		var obj_value = obj.value;
		if(obj_value.replace(/^\s+|\s+$/g,"").match(/\x20/i)) {
			obj.focus();
			obj.select();
			alert("The Name can not contain blanks");
			return false;
		}
		else {
			return true;
		}
	},

	isIPAddr: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		var i, j;
		if (this.isFunctionButton(event)){
			return true;
		}

		if((keyPressed > 47 && keyPressed < 58)){
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == '.'){
					j++;
				}
			}
			
			if(j < 3 && i >= 3){
				if(o.value.charAt(i-3) != '.' && o.value.charAt(i-2) != '.' && o.value.charAt(i-1) != '.'){
					o.value = o.value+'.';
				}
			}
			
			return true;
		}
		else if(keyPressed == 46){
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == '.'){
					j++;
				}
			}
			
			if(o.value.charAt(i-1) == '.' || j == 3){
				return false;
			}
			
			return true;
		}else if(keyPressed == 13){	// 'ENTER'
			return true;
		}
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}

		return false;
	},

	isIPAddrPlusNetmask: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		var i,j;
		if (this.isFunctionButton(event)){
			return true;
		}

		if((keyPressed > 47 && keyPressed < 58)){
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == '.'){
					j++;
				}
			}
			
			if(j < 3 && i >= 3){
				if(o.value.charAt(i-3) != '.' && o.value.charAt(i-2) != '.' && o.value.charAt(i-1) != '.'){
					o.value = o.value+'.';
				}
			}
			
			return true;
		}
		else if(keyPressed == 46){
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == '.'){
					j++;
				}
			}
			
			if(o.value.charAt(i-1) == '.' || j == 3){
				return false;
			}
			
			return true;
		}
		else if(keyPressed == 47){
			j = 0;
			
			for(i = 0; i < o.value.length; i++){
				if(o.value.charAt(i) == '.'){
					j++;
				}
			}
			
			if( j < 3){
				return false;
			}
			
			return true;
		}
		return false;
	},

	isIPRange: function(o, event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;
		var i, j;
		//function keycode for Firefox/Opera
		if (this.isFunctionButton(event)){
			return true;
		}

		if ((keyPressed > 47 && keyPressed < 58)){	// 0~9
			j = 0;
			for(i=0; i<o.value.length; i++){
				if (o.value.charAt(i)=='.'){
					j++;
				}
			}
			if (j<3 && i>=3){
				if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
				o.value = o.value + '.';
			}
			return true;
		}
		else if (keyPressed == 46){	// .
			j = 0;
			for(i=0; i<o.value.length; i++){
				if (o.value.charAt(i)=='.'){
					j++;
				}
			}
			if (o.value.charAt(i-1)=='.' || j==3){
				return false;
			}
			return true;
		}
		else if (keyPressed == 42){ // *
			return true;
		}

		return false;
	},

	isLegalIP: function(obj_name, obj_flag) {
		// A : 1.0.0.0~126.255.255.255
		// B : 127.0.0.0~127.255.255.255 (forbidden)
		// C : 128.0.0.0~255.255.255.254
		var A_class_start = inet_network("1.0.0.0");
		var A_class_end = inet_network("126.255.255.255");
		var B_class_start = inet_network("127.0.0.0");
		var B_class_end = inet_network("127.255.255.255");
		var C_class_start = inet_network("128.0.0.0");
		var C_class_end = inet_network("255.255.255.255");		
		var ip_obj = obj_name;
		var ip_num = inet_network(ip_obj.value);

		if(obj_flag == "DNS" && ip_num == -1){ //DNS allows to input nothing
			return true;
		}
		
		if(obj_flag == "GW" && ip_num == -1){ //GW allows to input nothing
			return true;
		}

		if(ip_num > A_class_start && ip_num < A_class_end){
		   obj_name.value = ipFilterZero(ip_obj.value);
			return true;
		}
		else if(ip_num > B_class_start && ip_num < B_class_end){
			alert(ip_obj.value+" <#JS_validip#>");
			ip_obj.focus();
			ip_obj.select();
			return false;
		}
		else if(ip_num > C_class_start && ip_num < C_class_end){
			obj_name.value = ipFilterZero(ip_obj.value);
			return true;
		}
		else{
			alert(ip_obj.value+" <#JS_validip#>");
			ip_obj.focus();
			ip_obj.select();
			return false;
		}
	},

	isLegalMask: function(obj_name) {
		var wrong_netmask = 0;
		var netmask_obj = obj_name;
		var netmask_num = inet_network(netmask_obj.value);

		var netmask_reverse_num = 0;
		var test_num = 0;
		if(netmask_num != -1) { 
			if(netmask_num == 0) {
				netmask_reverse_num = 0; //Viz 2011.07 : Let netmask 0.0.0.0 pass
			}
			else {
				netmask_reverse_num = ~netmask_num;
			}
			
			if(netmask_num < 0) {
				wrong_netmask = 1;
			}

			test_num = netmask_reverse_num;
			while(test_num != 0){
				if((test_num + 1) % 2 == 0) {
					test_num = (test_num + 1) / 2 - 1;
				}
				else{
					wrong_netmask = 1;
					break;
				}
			}
			if(wrong_netmask == 1){
				alert(netmask_obj.value + " is not a valid Mask address!");
				netmask_obj.focus();
				netmask_obj.select();
				return false;
			}
			else {
				return true;
			}
		}
		else { //null
			alert("This is not a valid Mask address!");
			netmask_obj.focus();
			netmask_obj.select();
			return false;
		}
	},

	isLegalIPAndMask: function(obj_name) {
		// A : 1.0.0.0~126.255.255.255
		// B : 127.0.0.0~127.255.255.255 (forbidden)
		// C : 128.0.0.0~255.255.255.254
		var A_class_start = inet_network("1.0.0.0");
		var A_class_end = inet_network("126.255.255.255");
		var B_class_start = inet_network("127.0.0.0");
		var B_class_end = inet_network("127.255.255.255");
		var C_class_start = inet_network("128.0.0.0");
		var C_class_end = inet_network("255.255.255.255");		
		var ip_obj = obj_name;
		var ip_mask_array = ip_obj.value.split("/");

		var vaildMaskRange = function() {
			var mask = parseInt(ip_mask_array[1]);

			if(parseInt(ip_mask_array[1]) == "" || isNaN(mask)) {
				alert("This is not a valid IP/Mask address!");
				ip_obj.focus();
				ip_obj.select();
				return false;
			}
			else if(parseInt(ip_mask_array[1]) < 0 || parseInt(ip_mask_array[1]) > 32) {
				alert(ip_mask_array[1] + " is not a valid Mask address!");
				ip_obj.focus();
				ip_obj.select();
				return false;
			}
			else {
				return true;
			}
		};

		if(ip_mask_array.length != 2) {
			alert("This is not a valid IP/Mask address!");
			ip_obj.focus();
			ip_obj.select();
			return false;
		}
		var ip_num = inet_network(ip_mask_array[0]);

		if(ip_num > A_class_start && ip_num < A_class_end){
			if(vaildMaskRange()) {
				obj_name.value = ipFilterZero(ip_mask_array[0]) + "/" + ip_mask_array[1];
				return true;
			}
		}
		else if(ip_num > B_class_start && ip_num < B_class_end){
			alert(ip_mask_array[0]+" <#JS_validip#>");
			ip_obj.focus();
			ip_obj.select();
			return false;
		}
		else if(ip_num > C_class_start && ip_num < C_class_end){
			if(vaildMaskRange()) {
				obj_name.value = ipFilterZero(ip_mask_array[0]) + "/" + ip_mask_array[1];
				return true;
			}
		}
		else{
			alert(ip_mask_array[0]+" <#JS_validip#>");
			ip_obj.focus();
			ip_obj.select();
			return false;
		}	
	},

	isLegal_ipv6: function(obj, flag) {
		
		var rangere=new RegExp("^((([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}:([0-9A-Fa-f]{1,4}:)?[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){4}:([0-9A-Fa-f]{1,4}:){0,2}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){3}:([0-9A-Fa-f]{1,4}:){0,3}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){2}:([0-9A-Fa-f]{1,4}:){0,4}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|(([0-9A-Fa-f]{1,4}:){0,5}:((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|(::([0-9A-Fa-f]{1,4}:){0,5}((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|([0-9A-Fa-f]{1,4}::([0-9A-Fa-f]{1,4}:){0,5}[0-9A-Fa-f]{1,4})|(::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:))$", "gi");

		if(rangere.test(obj.value)){
			return true;
		}
		else{
			if(flag != 1){	//1: mute
				alert(obj.value+" <#JS_validip#>");
				obj.focus();
				obj.select();
			}
			return false;
		}
 	},

	isPortRange: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;

		if (this.isFunctionButton(event)){
			return true;
		}

		if ((keyPressed > 47 && keyPressed < 58)){	//0~9
			return true;
		}
		else if (keyPressed == 58 && o.value.length>0){
			for(var i=0; i<o.value.length; i++){
				var c=o.value.charAt(i);
				if (c==':' || c=='>' || c=='<' || c=='=')
				return false;
			}
			return true;
		}
		else if (keyPressed==44){  //"�? can be type in first charAt  ::: 0220 Lock add"
			if (o.value.length==0)
				return false;		
			else
				return true;
		}
		else if (keyPressed==60 || keyPressed==62){  //">" and "<" only can be type in first charAt ::: 0220 Lock add
			if (o.value.length==0)
				return true;		
			else
				return false;
		}
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}		

		return false;
	},

	isPortlist: function(o,event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;

		//function keycode for Firefox/Opera
		if (this.isFunctionButton(event)){
			return true;
		}
			
		if ((keyPressed>47 && keyPressed<58) || keyPressed == 32){
			return true;
		}
		else if(event.metaKey && (keyPressed == 65 || keyPressed == 67 || keyPressed == 86 || keyPressed == 88 
			                   || keyPressed == 97 || keyPressed == 99 || keyPressed == 118 || keyPressed == 120)){		//for Mac + Safari, let 'Command + A'(C, V, X) can work
			return true
		}
		else{
			return false;
		}
	},

	isPrivateIP: function(_val){
		var A_class_start = this.inet_network("10.0.0.0");
		var A_class_end = this.inet_network("10.255.255.255");
		var B_class_start = this.inet_network("172.16.0.0");
		var B_class_end = this.inet_network("172.31.255.255");
		var C_class_start = this.inet_network("192.168.0.0");
		var C_class_end = this.inet_network("192.168.255.255");
		var ip_num = this.inet_network(_val);

		if(ip_num > A_class_start && ip_num < A_class_end)
			return true;
		else if(ip_num > B_class_start && ip_num < B_class_end)
			return true;
		else if(ip_num > C_class_start && ip_num < C_class_end)
			return true;
		else 
			return false;
	},

	isString: function(o, event){
		var keyPressed = event.keyCode ? event.keyCode : event.which;

		if(keyPressed >= 0 && keyPressed <= 126)
			return true;
		else{
			alert('<#JS_validchar#>');
			return false;
		}	
	},

	isValidURL: function(value) {
		var urlregex = new RegExp("^(http|https|ftp)\://([a-zA-Z0-9\.\-]+(\:[a-zA-Z0-9\.&amp;%\$\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|([a-zA-Z0-9\-]+\.)*[a-zA-Z0-9\-]+\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))(\:[0-9]+)*(/($|[a-zA-Z0-9\.\,\?\'\\\+&amp;%\$#\=~_\-]+))*$");
		if (urlregex.test(value)) {
			return true;
		}
		alert("It is invalid URL."); /*untranslated*/
		return false;
	},

	isValidHost: function(value) {	//<#JS_validip#>
		var err_count=0;
		var alert_content = "<#JS_validip#>\n<#JS_valid_FQDN#>";
		var urlregex = new RegExp("^([a-zA-Z0-9\.\-]+(\:[a-zA-Z0-9\.&amp;%\$\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|([a-zA-Z0-9\-]+\.)*[a-zA-Z0-9\-]+\.(com|edu|gov|int|mil|net|org|biz|arpa|info|name|pro|aero|coop|museum|[a-zA-Z]{2}))$");
		if(!urlregex.test(value)){
			err_count++;
		}

		var rangere=new RegExp("^((([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}:[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){5}:([0-9A-Fa-f]{1,4}:)?[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){4}:([0-9A-Fa-f]{1,4}:){0,2}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){3}:([0-9A-Fa-f]{1,4}:){0,3}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){2}:([0-9A-Fa-f]{1,4}:){0,4}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){6}((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|(([0-9A-Fa-f]{1,4}:){0,5}:((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|(::([0-9A-Fa-f]{1,4}:){0,5}((\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b)\\.){3}(\\b((25[0-5])|(1\\d{2})|(2[0-4]\\d)|(\\d{1,2}))\\b))|([0-9A-Fa-f]{1,4}::([0-9A-Fa-f]{1,4}:){0,5}[0-9A-Fa-f]{1,4})|(::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})|(([0-9A-Fa-f]{1,4}:){1,7}:))$", "gi");
		if(!rangere.test(value)){
			err_count++;
		}

		if(err_count==2){
			alert(value +" \n"+ alert_content);
			return false;
		}
		else
			return true;
	},	

	// 2010.07 James. {
	inet_network: function(ip_str){
		if(!ip_str)
			return -1; //null
		
		var re = /^(\d+)\.(\d+)\.(\d+)\.(\d+)$/;
		if(re.test(ip_str)){
			var v1 = parseInt(RegExp.$1);
			var v2 = parseInt(RegExp.$2);
			var v3 = parseInt(RegExp.$3);
			var v4 = parseInt(RegExp.$4);
			
			if(v1 < 256 && v2 < 256 && v3 < 256 && v4 < 256)
				return v1*256*256*256+v2*256*256+v3*256+v4; //valid
		}
		
		return -2; //not valid
	},

	ipAddr4: function(obj){
		var num = -1;	
		var pos = 0;
		var v1, v2, v3, v4;
		if(obj.value == "")
				return true;
		else{			
			for(var i = 0; i < obj.value.length; ++i){
				var c = obj.value.charAt(i);
			
				if(c >= '0' && c <= '9'){
					if(num == -1 ){
						num = (c-'0');
					}
					else{
						num = num*10+(c-'0');
					}
				}
				else{
					if(num < 0 || num > 255 || c != '.'){
						return false;
					}			
					if(pos == 0)
						v1 = num;
					else if(pos == 1)
						v2 = num;
					else if(pos == 2)
						v3 = num;
				
					num = -1;
					++pos;
				}
			}
		
			if(pos!=3 || num<0 || num>255){		
				return false;
			}
			else
				v4 = num;
			
			return true;
		}
	},

	ipAddrFinal: function(o, v, noAlert){
		var num = -1;
		var pos = 0;
		var v1, v2, v3, v4;
		if(o.value.length == 0){
			if(v == 'dhcp_start' || v == 'dhcp_end' ||
					v == 'wan_ipaddr_x' ||
					v == 'dhcp1_start' || v=='dhcp1_end' ||
					v == 'lan_ipaddr' || v=='lan_netmask' ||
					v=='lan1_ipaddr' || v=='lan1_netmask' ||
					v == 'wl_radius_ipaddr' || v == 'hs_radius_ipaddr') {
				alert("<#JS_fieldblank#>");
				
				if(v == 'wan_ipaddr_x'){
					document.form.wan_ipaddr_x.value = "10.1.1.1";
					document.form.wan_netmask_x.value = "255.0.0.0";
				}
				else if(v == 'lan_ipaddr'){
					document.form.lan_ipaddr.value = "192.168.1.1";
					document.form.lan_netmask.value = "255.255.255.0";
				}
				else if(v == 'lan1_ipaddr'){
					document.form.lan1_ipaddr.value = "192.168.2.1";
					document.form.lan1_netmask.value = "255.255.255.0";
				}
				else if(v == 'lan_netmask')
					document.form.lan_netmask.value = "255.255.255.0";
				else if(v == 'lan1_netmask')
					document.form.lan1_netmask.value = "255.255.255.0";
				
				o.focus();
				o.select();
				
				return false;
			}
			else
				return true;
		}
		
		if(v == 'wan_ipaddr_x' && document.form.wan_netmask_x.value == "")
			document.form.wan_netmask_x.value="255.255.255.0";
		
		for(var i = 0; i < o.value.length; ++i){
			var c = o.value.charAt(i);
			
			if(c >= '0' && c <= '9'){
				if(num == -1 ){
					num = (c-'0');
				}
				else{
					num = num*10+(c-'0');
				}
			}
			else{
				if(num < 0 || num > 255 || c != '.'){
					if(v == 'wl_radius_ipaddr' && typeof(noAlert) != 'undefined' && noAlert == 1){
						return false;
					}
					else{
						if(noAlert != 1){
							alert(o.value+" <#JS_validip#>");
							o.value = "";
							o.focus();
							o.select();
						}
						return false;
					}
				}
				
				if(pos == 0)
					v1 = num;
				else if(pos == 1)
					v2 = num;
				else if(pos == 2)
					v3 = num;
				
				num = -1;
				++pos;
			}
		}
		
		if(pos!=3 || num<0 || num>255){
			if(v == 'wl_radius_ipaddr' && typeof(noAlert) != 'undefined' && noAlert == 1){
				return false;
			}
			else{
				if(noAlert != 1){
					alert(o.value + " <#JS_validip#>");
					o.value = "";
					o.focus();
					o.select();
				}
				return false;
			}
		}
		else
			v4 = num;
		
		if(v == 'dhcp_start' || v == 'dhcp_end' ||
				v == 'wan_ipaddr_x' ||
				v == 'dhcp1_start' || v == 'dhcp1_end' ||
				v == 'lan_ipaddr' || v == 'lan1_ipaddr' ||
				v == 'staticip' || v == 'wl_radius_ipaddr' ||
				v == 'dhcp_dns1_x' || v == 'dhcp_gateway_x' || v == 'dhcp_wins_x' ||
				v == 'sip_server'){
			if((v!='wan_ipaddr_x')&& (v1==255||v4==255||v1==0||v4==0||v1==127||v1==224)){
				if(v == 'wl_radius_ipaddr' && typeof(noAlert) != 'undefined' && noAlert == 1){
					return false;
				}
				else{
					alert(o.value + " <#JS_validip#>");
					
					o.value = "";
					o.focus();
					o.select();
					return false;
				}
			}
			
			if(sw_mode == "2" || sw_mode == "3")	// variables are defined in state.js
				;	// there is no WAN in AP mode, so it wouldn't be compared with the wan ip..., etc.
			else if(this.requireWANIP(v) && (
					(v=='wan_ipaddr_x' &&  this.matchSubnet2(o.value, document.form.wan_netmask_x, document.form.lan_ipaddr.value, document.form.lan_netmask)) ||
					(v=='lan_ipaddr' &&  this.matchSubnet2(o.value, document.form.lan_netmask, document.form.wan_ipaddr_x.value, document.form.wan_netmask_x))
					)){
				alert(o.value + " <#JS_validip#>");
				if(v == 'wan_ipaddr_x'){
					document.form.wan_ipaddr_x.value = "10.1.1.1";
					document.form.wan_netmask_x.value = "255.0.0.0";
				}
				else if(v == 'lan_ipaddr'){
					document.form.lan_ipaddr.value = "192.168.1.1";
					document.form.lan_netmask.value = "255.255.255.0";
				}
				else if(v == 'lan1_ipaddr'){
					document.form.lan1_ipaddr.value = "192.168.2.1";
					document.form.lan1_netmask.value = "255.255.255.0";
				}
				
				o.focus();
				o.select();
				
				return false;
			}
		}
		else if(v=='lan_netmask' || v=='lan1_netmask'){
			if(v1==255&&v2==255&&v3==255&&v4==255){
				alert(o.value + " <#JS_validip#>");
				o.value = "";
				o.focus();
				o.select();
				return false;
			}
		}
		
		if(sw_mode=="2" || sw_mode=="3")	// variables are defined in state.js
				;	// there is no WAN in AP mode, so it wouldn't be compared with the wan ip..., etc.
		else if(this.requireWANIP(v) && (
				(v=='wan_netmask_x' &&  this.matchSubnet2(document.form.wan_ipaddr_x.value, o, document.form.lan_ipaddr.value, document.form.lan_netmask)) ||
				(v=='lan_netmask' &&  this.matchSubnet2(document.form.lan_ipaddr.value, o, document.form.wan_ipaddr_x.value, document.form.wan_netmask_x))
				)){
			alert(o.value + " <#JS_validip#>");
			
			if (v=='wan_netmask_x'){
				document.form.wan_ipaddr_x.value = "10.1.1.1";
				document.form.wan_netmask_x.value = "255.0.0.0";
			}
			else if(v=='lan_netmask'){
				document.form.lan_ipaddr.value = "192.168.1.1";
				document.form.lan_netmask.value = "255.255.255.0";
			}
			else if(v=='lan1_netmask'){
				document.form.lan1_ipaddr.value = "192.168.2.1";
				document.form.lan1_netmask.value = "255.255.255.0";
			}
			
			o.focus();
			o.select();
			return false;
		}
			
			
		o.value = v1 + "." + v2 + "." + v3 + "." + v4;
		
		if((v1 > 0) && (v1 < 127)) mask = "255.0.0.0";
		else if ((v1 > 127) && (v1 < 192)) mask = "255.255.0.0";
		else if ((v1 > 191) && (v1 < 224)) mask = "255.255.255.0";
		else mask = "0.0.0.0";
		
		if(v=='wan_ipaddr_x' && document.form.wan_netmask_x.value==""){
			document.form.wan_netmask_x.value = mask;
		}
		else if (v=='lan_ipaddr' && document.form.lan_netmask.value=="" ){
			document.form.lan_netmask.value = mask;
		}else if (v=='dhcp_start'){
			if (!this.matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_start.value, 3)){
				alert(o.value + " <#JS_validip#>");
				o.focus();
				o.select();
				return false;
			}
		}
		else if (v=='dhcp_end'){
			if (!this.matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_end.value, 3)){
				alert(o.value + " <#JS_validip#>");
				o.focus();
				o.select();
				return false;
			}
		}
		else if (v=='lan1_ipaddr'){
			if(document.form.lan1_netmask.value=="") document.form.lan1_netmask.value = mask;
		}
		else if (v=='dhcp1_start'){
			if (!this.matchSubnet(document.form.lan1_ipaddr.value, document.form.dhcp1_start.value, 3)){
				alert(o.value + " <#JS_validip#>");
				o.focus();
				o.select();
				return false;
			}
		}
		else if (v=='dhcp1_end'){
			if (!this.matchSubnet(document.form.lan1_ipaddr.value, document.form.dhcp1_end.value, 3)){
				alert(o.value + " <#JS_validip#>");
				o.focus();
				o.select();
				return false;
			}
		}
		
		return true;
	},

	ipAddrFinalQIS: function(o,v){
		var IP_Validate = function(o){
			var ip_reg=/^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/;
			
			if(ip_reg.test(o.value)){ //區分是否為IP
				return true;
			}
			else{
				return false;
			}
		};

		var fulfillIP = function(obj){
			var src_ip = obj.value.split(".");
			var IP_List = document.getElementById(obj.name+"_div").childNodes;
			
			for(var i=0,j=0; i < IP_List.length, j<src_ip.length; i++){
				if(IP_List[i].type == "text"){
					IP_List[i].value = src_ip[j];
					j++;
				}
			}
		};
		
		if(v == 'wan_ipaddr_x'){
			if(o.value.length == 0){    /*Blank.*/
				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				alert(o.title+"<#JS_fieldblank#>");
				document.form.wan_ipaddr_x1.focus();
				return false;
			}
			else if(o.value.indexOf("0") == 0){ /*首字不能為0*/
				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				alert(document.form.wan_ipaddr_x.value + " <#JS_validip#>");
				document.form.wan_ipaddr_x1.focus();
				return false;
			}		
			else if(!(IP_Validate(o))){ /*IP格式錯誤*/
				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				alert(document.form.wan_ipaddr_x.value + " <#JS_validip#>");
				document.form.wan_ipaddr_x4.focus();
				return false;
			}
			else if(IP_Validate(o)){    
				if(document.form.wan_ipaddr_x1.value >= 224){
					alert("<#JS_field_wanip_rule2#>");
					document.form.wan_ipaddr_x1.focus();
					document.form.wan_ipaddr_x1.select();
					return false;
				}
				else if(document.form.wan_ipaddr_x1.value == 127){
					alert(document.form.wan_ipaddr_x1.value + "<#JS_field_wanip_rule1#>");
					document.form.wan_ipaddr_x1.focus();
					document.form.wan_ipaddr_x1.select();
					return false;
				}
				else{
					return true;
				}
			}
		}
		else if(v == 'wan_netmask_x'){
			var wan_ipaddr_x1 = document.form.wan_ipaddr_x1.value;
			if(o.value.length == 0){    /*Blank.*/

				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				
				if(confirm(o.title+"<#JS_fieldblank#>\n<#JS_field_fulfillSubmask#>")){
					if((wan_ipaddr_x1 > 0) && (wan_ipaddr_x1 < 127)) o.value = "255.0.0.0";
					else if ((wan_ipaddr_x1 > 127) && (wan_ipaddr_x1 < 192)) o.value = "255.255.0.0";
					else if ((wan_ipaddr_x1 > 191) && (wan_ipaddr_x1 < 224)) o.value = "255.255.255.0";
					else o.value = "0.0.0.0";
					fulfillIP(o);
				}
				document.form.wan_netmask_x1.focus();
				return false;
			}
			else if(!(IP_Validate(o))){ /*IP格式錯誤*/
				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				alert(o.value + " <#JS_validip#>");
				return false;
			}
			else if(IP_Validate(o)){    
				if(this.requireWANIP(v) && (
				(this.matchSubnet2(document.form.wan_ipaddr_x.value, o, document.form.lan_ipaddr.value, document.form.lan_netmask))
				)){
					if(document.getElementById(o.name+"_div")){
						document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
					}
					alert(o.value + " <#JS_validip#>");
					return false;
				}
				else{
					return true;
				}
			}
		}
		else if(v == 'wan_gateway_x'){
			if(o.value.length > 0){
				if(!(IP_Validate(o))){ /* IP格式錯誤*/
					if(document.getElementById(o.name+"_div")){
						document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
					}
					alert(o.value + " <#JS_validip#>");
					return false;
				}
				else if(o.value == document.form.wan_ipaddr_x.value){
					if(document.getElementById(o.name+"_div")){
						document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
					}
					alert("<#IPConnection_warning_WANIPEQUALGatewayIP#>");
					return false;			
				}
			}
			
			return true;	
		}
		else if(v == 'wan_dns1_x' || v == 'wan_dns2_x'){
			
			var split_IP = o.value.split(".");
			
			if(!(IP_Validate(o))){
				if(document.getElementById(o.name+"_div")){
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
				}
				alert(o.value + " <#JS_validip#>");
				return false;
			}
			return true;
			/*
			else if(IP_Validate(o)){ 
				if(split_IP[0]==255||split_IP[1]==255||split_IP[2]==255||split_IP[3]==255||split_IP[0]==0||split_IP[3]==0||split_IP[0]==127||split_IP[0]==224){
					alert(o.value +" <#JS_validip#>");
					document.getElementById(o.name+"_div").style.border = "2px solid #CE1E1E";
					return false;
				}			
				else{
					return true;
				}
			}
			*/
		}
	},

	ipList: function(o, event) {
		var keyPressed;
		if (event.which == null)
			keyPressed = event.keyCode;     // IE
		else if (event.which != 0 && event.charCode != 0)
			keyPressed = event.which        // All others
		else
			return true;                    // Special key

		if ((keyPressed>=48&&keyPressed<=57) || //0-9
			(keyPressed==46) ||                  //.
			(keyPressed==44)) return true;	//,

		return false;
	},

	ipRange: function(o, v){
		var num = -1;
		var pos = 0;
		if (o.value.length==0)
			return true;
		for(var i=0; i<o.value.length; i++)
		{
			var c = o.value.charAt(i);
			if(c>='0'&&c<='9')
			{
				if ( num==-1 ){
					num = (c-'0');
				}
				else{
					num = num*10 + (c-'0');
				}
			}
			else if (c=='*'&&num==-1){
				num = 0;
			}
			else{
				if ( num<0 || num>255 || (c!='.')){
					alert(o.value + " <#JS_validip#>");
					o.value = "";
					o.focus();
					o.select();
					return false;
				}
				num = -1;
				pos++;
			}
		}
		if (pos!=3 || num<0 || num>255){
			alert(o.value + " <#JS_validip#>");
			o.value = "";
			o.focus();
			o.select();
			return false;
		}
		if (v=='ExternalIPAddress' && document.form.wan_netmask_x.value == ''){
			document.form.wan_netmask_x.value = "255.255.255.0";
		}
		else if (v=='IPRouters' && document.form.lan_netmask.value == ''){
			document.form.lan_netmask.value = "255.255.255.0";
		}
		return true;
	},

	ipSubnet: function(obj){
		var ipPattern1 = new RegExp("(^([0-9]{1,3})\\.([0-9]{1,3})\\.([0-9]{1,3})\\.(\\*)$)", "gi");
		var ipPattern2 = new RegExp("(^([0-9]{1,3})\\.([0-9]{1,3})\\.(\\*)\\.(\\*)$)", "gi");
		var ipPattern3 = new RegExp("(^([0-9]{1,3})\\.(\\*)\\.(\\*)\\.(\\*)$)", "gi");
		var ipPattern4 = new RegExp("(^(\\*)\\.(\\*)\\.(\\*)\\.(\\*)$)", "gi");
		var parts = obj.value.split(".");
		if(!ipPattern1.test(obj.value) && !ipPattern2.test(obj.value) && !ipPattern3.test(obj.value) && !ipPattern4.test(obj.value)){
			alert(obj.value + " <#JS_validip#>");
			obj.focus();
			obj.select();
			return false;
		}else if(parts[0] == 0 || parts[0] > 255 || parts[1] > 255 || parts[2] > 255){			
			alert(obj.value + " <#JS_validip#>");
			obj.focus();
			obj.select();
			return false;
		}else
			return true;
	},

	maskRange: function (min, max, mask) {
		var maskMinimum = inet_network(min);
		var maskMaximum = inet_network(max);
		var maskNum = inet_network(mask);
		if(maskMinimum > maskNum || maskMaximum < maskNum) {
			return false;
		}
		else {
			return true;
		}

	},

	matchSubnet: function(ip1, ip2, count){
		var c = 0;
		var v1 = 0;
		var v2 = 0;
		for(i=0;i<ip1.length;i++){
			if (ip1.charAt(i) == '.'){
				if (ip2.charAt(i) != '.')
					return false;
					
				c++;
				if (v1!=v2) 
					return false;
					
				v1 = 0;
				v2 = 0;
			}
			else{
				if (ip2.charAt(i)=='.') 
					return false;
					
				v1 = v1*10 + (ip1.charAt(i) - '0');
				v2 = v2*10 + (ip2.charAt(i) - '0');
			}
			if (c==count) 
				return true;
		}
		return false;
	},

	matchSubnet2: function(wan_ip1, wan_sb1,lan_ip2,lan_sb2){
		var nsb;
		var nsb1 = this.inet_network(wan_sb1.value);
		var nsb2 = this.inet_network(lan_sb2.value);
		var nip1 = this.inet_network(wan_ip1);

		if(nip1 == 0 || nip1 == -1) // check if WAN IP = 0.0.0.0 or NULL (factory default value)
			return 1;

		if(nsb1 < nsb2 && nsb1 !=0)
			nsb = nsb1;
		else
			nsb = nsb2;
		
		if((this.inet_network(wan_ip1)&nsb) == (this.inet_network(lan_ip2)&nsb))
			return 1;
		else
			return 0;
	},

	numberRange: function(obj, mini, maxi){
		var PortRange = obj.value;
		var rangere=new RegExp("^([0-9]{1,5})\:([0-9]{1,5})$", "gi");

		if(rangere.test(PortRange)){
			if(parseInt(RegExp.$1) >= parseInt(RegExp.$2)){
				alert("<#JS_validport#>");	
				obj.focus();
				obj.select();		
				return false;												
			}
			else{
				if(!this.eachPort(obj, RegExp.$1, mini, maxi) || !this.eachPort(obj, RegExp.$2, mini, maxi)){
					obj.focus();
					obj.select();
					return false;											
				}
				return true;								
			}
		}
		else{
			if(!this.range(obj, mini, maxi)){		
				obj.focus();
				obj.select();
				return false;
			}
			return true;	
		}	
	},

	portList: function(o, v){
		if (o.value.length==0)
			return true;
		var num = 0;
		for(var i=0; i<o.value.length; i++){
			var c = o.value.charAt(i);
			if (c>='0'&&c<='9'){
				num = num*10 + (c-'0');
			}
			else{
				if (num>255){
					alert(num + " <#JS_validport#>");
					
				o.focus();
				o.select();
				return false;
				}
				num = 0;
			}
		}
			
		if (num>255){
			alert(num + " <#JS_validport#>");
			o.focus();
			o.select();
			return false;
		}
		
		return true;
	},

	psk: function(psk_obj, wl_unit){
		var psk_length = psk_obj.value.length;
		var psk_length_trim = psk_obj.value.trim().length;
		
		if(psk_length < 8){
			alert("<#JS_PSK64Hex#>");
			psk_obj.focus();
			psk_obj.select();
			
			return false;
		}
				
		if(psk_length > 64){
			alert("<#JS_PSK64Hex#>");
			psk_obj.focus();
			psk_obj.select();
			return false;
		}

		if(psk_length != psk_length_trim){
			alert(stringSafeGet("<#JS_PSK64Hex_whiteSpace#>"));
			psk_obj.focus();
			psk_obj.select();				
			return false;
		}
				
		if(psk_length >= 8 && psk_length <= 63 && !this.string(psk_obj)){
			alert("<#JS_PSK64Hex#>");
			psk_obj.focus();
			psk_obj.select();
			return false;
		}
				
		if(psk_length == 64 && !this.hex(psk_obj)){
			alert("<#JS_PSK64Hex#>");
			psk_obj.focus();
			psk_obj.select();				
			return false;
		}

		return true;
	},

	psk_KR: function(psk_obj, flag){
		
		var psk_length = psk_obj.value.length;
		var psk_length_trim = psk_obj.value.trim().length;
		if(!/[A-Za-z]/.test(psk_obj.value) || !/[0-9]/.test(psk_obj.value) || psk_length < 8 || psk_length > 63 
				|| !/[\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]/.test(psk_obj.value)){
			alert("<#JS_PSK64Hex_kr#> <#JS_validPWD#>");
			psk_obj.value = "";
			psk_obj.focus();
			return false;
		}

		if(psk_length != psk_length_trim){
			alert(stringSafeGet("<#JS_PSK64Hex_whiteSpace#>"));
			psk_obj.focus();
			psk_obj.select();
			return false;
		}

		if(!this.string_KR(psk_obj)){
			alert("<#JS_PSK64Hex#>");
			psk_obj.focus();
			psk_obj.select();
			return false;
		}

		var invalid_char = "";
		for(var i = 0; i < psk_length; ++i){
			if(psk_obj.value.charAt(i) <= ' ' || psk_obj.value.charAt(i) > '~'){
				invalid_char = invalid_char+psk_obj.value.charAt(i);
			}
		}

		if(invalid_char != ""){
			if(flag != "noalert")
				alert("<#JS_validstr2#> '"+invalid_char+"' !");
			psk_obj.value = "";
			psk_obj.focus();	
			return false;
		}

		return true;
	},

	range: function(o, _min, _max) {
		var str2val = function(v){
			for(i=0; i<v.length; i++){
				if (v.charAt(i) !='0') 
					break;
			}
			return v.substring(i);
		};

		if(_min > _max){
			var tmpNum = "";
		
			tmpNum = _min;
			_min = _max;
			_max = tmpNum;
		}

		if(isNaN(o.value) || o.value < _min || o.value > _max) {
			alert('<#JS_validrange#> ' + _min + ' <#JS_validrange_to#> ' + _max);
			o.focus();
			o.select();
			return false;
		}
		else {
			o.value = str2val(o.value);
			if(o.value=="")
				o.value="0";
				
			return true;
		}
	},

	range_s46_ports: function(obj, port) {
		const wan0_proto = httpApi.nvramGet(["wan0_proto"]).wan0_proto;
		const get_ipv6_s46_ports = (isSupport("s46") && (wan0_proto == "v6plus" || wan0_proto == "ocnvc" || wan0_proto == "v6opt")) ? httpApi.nvramGet(["ipv6_s46_ports"]).ipv6_s46_ports : '0';
		let array_ipv6_s46_ports = new Array("");
		if (get_ipv6_s46_ports != "0" && get_ipv6_s46_ports != "") {
			array_ipv6_s46_ports = get_ipv6_s46_ports.split(" ");
		}
		//e.g. ipv6_s46_ports="6448-6463 10544-10559 14640-14655 18736-18751 22832-22847 26928-26943 31024-31039 35120-35135 39216-39231 43312-43327 47408-47423 51504-51519 55600-55615 59696-59711 63792-63807"
		var inAvailable=false;
		var array_each_s46_ports = new Array("");
		value_comp = parseInt(obj.value); 
		port_comp = parseInt(port);
		for(var x=0;x<array_ipv6_s46_ports.length;x++){
			array_each_s46_ports=array_ipv6_s46_ports[x].split("-");
			if(port != "none"){
				if(port_comp >= parseInt(array_each_s46_ports[0]) && port_comp <= parseInt(array_each_s46_ports[1])) {
					//console.log("[port]"+port_comp+" : "+array_each_s46_ports[0]+"-"+array_each_s46_ports[1]);
					inAvailable=true;
				}
			}
			else{
				if(value_comp >= parseInt(array_each_s46_ports[0]) && value_comp <= parseInt(array_each_s46_ports[1])) {
					//console.log("[obj.value]"+value_comp+" : "+array_each_s46_ports[0]+"-"+array_each_s46_ports[1]);
					inAvailable=true;
				}
			}
		}

		if(!inAvailable)
			return false;
		else
			return true;
	},

	rangeNull: function(o, min, max, def) {		//Viz add 2013.03 allow to set null
		if (o.value=="") return true;
		
		if(isNaN(o.value) || o.value < min || o.value > max) {
			alert('<#JS_validrange#> ' + min + ' <#JS_validrange_to#> ' + max + '.');
			o.value = def;
			o.focus();
			o.select();
			return false;
		}
		
		return true;
	},

	rangeAllowZero: function(o, min, max, def) {		//allow to set "0"
		var str2val = function(v){
			for(i=0; i<v.length; i++){
				if (v.charAt(i) !='0') 
					break;
			}
			return v.substring(i);
		};

		if (o.value==0) return true;

		if(isNaN(o.value) || o.value < min || o.value > max) {
			alert('<#JS_validrange#> ' + min + ' <#JS_validrange_to#> ' + max + '.');
			o.value = def;
			o.focus();
			o.select();
			return false;
		}
		else {
			o.value = str2val(o.value);
			if(o.value=="") 
				o.value="0";
			return true;
		}
	},

	rangeFloat: function(o, _min, _max, def){
        if(isNaN(o.value) || o.value < _min || o.value > _max) {
            alert('<#JS_validrange#> ' + _min + ' <#JS_validrange_to#> ' + _max + '.');
			o.value = def;
			o.focus();
			o.select();
			return false;
		}

        return true;
	},

	lengthInUtf8: function(str) {
		var asciiLength = str.match(/[\u0000-\u007f]/g) ? str.match(/[\u0000-\u007f]/g).length : 0;
		var multiByteLength = encodeURI(str.replace(/[\u0000-\u007f]/g)).match(/%/g) ? encodeURI(str.replace(/[\u0000-\u007f]/g, '')).match(/%/g).length : 0;
		return asciiLength + multiByteLength;
	},

	ssidChar: function(ch){
		if(ch >= 32 && ch <= 126)
			return false;
		
		return true;
	},

	string: function(string_obj, flag){
		if(string_obj.value.charAt(0) == '"'){
			if(flag != "noalert")
				alert('<#JS_validstr1#> ["]');

			string_obj.focus();

			return false;
		}
		else if(string_obj.value.charAt(string_obj.value.length - 1) == '"'){
			if(flag != "noalert"){
				alert('<#JS_validstr3#> ["]');
			}
			
			string_obj.focus();

			return false;
		}
		else{
			var invalid_char = "";
			for(var i = 0; i < string_obj.value.length; ++i){
				if(string_obj.value.charAt(i) < ' ' || string_obj.value.charAt(i) > '~'){
					invalid_char = invalid_char+string_obj.value.charAt(i);
				}
			}

			if(invalid_char != ""){
				if(flag != "noalert")
					alert("<#JS_validstr2#> '"+invalid_char+"' !");

				string_obj.focus();

				return false;
			}
		}

		return true;
	},
	
	string_KR: function(string_obj, flag){		//KR: Alphabets, numbers, specialcharacters mixed. 8 chars at least.
							//S2: Mixed 2 out of Alphabets(Upper/Lower case), numbers, specialcharacters.
							//	  10 chars at least. Not have consecutive identical characters.
		var string_length = string_obj.value.length;
		if(!/[A-Za-z]/.test(string_obj.value) || !/[0-9]/.test(string_obj.value) || string_length < 10
				|| !/[\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~]/.test(string_obj.value)
				|| /([A-Za-z0-9\!\"\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\\\]\^\_\`\{\|\}\~])\1/.test(string_obj.value) 
		){
				
				alert("<#JS_validLoginPWD#>");
				string_obj.focus();
				return false;
		}

		if(string_obj.value.charAt(0) == '"'){
			if(flag != "noalert")
				alert('<#JS_validstr1#> ["]');

			string_obj.focus();

			return false;
		}
		else if(string_obj.value.charAt(string_obj.value.length - 1) == '"'){
			if(flag != "noalert"){
				alert('<#JS_validstr3#> ["]');
			}

			string_obj.focus();

			return false;
		}
		
		var invalid_char = "";
		for(var i = 0; i < string_obj.value.length; ++i){
			if(string_obj.value.charAt(i) <= ' ' || string_obj.value.charAt(i) > '~'){
				invalid_char = invalid_char+string_obj.value.charAt(i);
			}
		}

		if(invalid_char != ""){
			if(flag != "noalert"){
				alert("<#JS_validstr2#> '"+invalid_char+"' !");
			}

			string_obj.focus();
			return false;
		}

		return true;
	},

	phone_CN: function(string_obj, flag){
		var mobile = /^1[0-9]{10}$/;
		var phone = /(^0\d{2,3}?-?\d{7,8}$)/;
		var result = "";

		switch(flag){
			case "both":
						result = mobile.test(string_obj.value) || phone.test(string_obj.value);
						break;

			case "mobile":
						result = mobile.test(string_obj.value);
						break;

			case "phone":
						result = phone.test(string_obj.value);
						break;

			default:
						result = mobile.test(string_obj.value) || phone.test(string_obj.value);
						break;
		}

		return result;
	},

	qq: function(string_obj){
		var id_number = /^[0-9]{5,32}$/;
		var result = id_number.test(string_obj.value);

		return result;
	},

	//validate SSID string
	stringGroup: function(o){
		var groupChar = function(ch){
		if(ch == 60 || ch == 62) /*ch == 39 || */ //60 is <, 62 is > 
			return false;
		
		return true;
		};

		var c;	// character code	
		for(var i = 0; i < o.value.length; ++i){
			c = o.value.charCodeAt(i);
			if(!groupChar(c)){
				alert('<#JS_validSSID1#> '+o.value.charAt(i)+' <#JS_validSSID2#>');
				o.focus();
				o.select();
				return false;
			}
		}

		return true;
	},

	stringSSID: function(o){
		var rc_support = '<% nvram_get("rc_support"); %>';
		var utf8_ssid_support = (rc_support.split(" ").indexOf("utf8_ssid") == -1) ? false : true;
		var c;	// character code
		var flag=0; // notify valid characters of SSID except space
		
		if(o.value==""){	// to limit null SSID
			alert('<#JS_fieldblank#>');
			o.focus();
			return false;
		}	

		len = this.lengthInUtf8(o.value);

		if(len > 32){
			alert("<#JS_max_ssid#>");
			o.value = "";
			o.focus();
			o.select();
			return false;
		}

		for(var i = 0; i < len; ++i){

			c = o.value.charCodeAt(i);
			if(!utf8_ssid_support){
				if(this.ssidChar(c)){
					alert('<#JS_validSSID1#> '+o.value.charAt(i)+' <#JS_validSSID2#>');
					o.value = "";
					o.focus();
					o.select();
					return false;
				}
			}
			
			if(c != 32)
				flag ++;
		}

		if(flag ==0){	// to limit SSID only include space
			alert('<#JS_fieldblank#>');
			return false;
		}
		
		return true;
	},

	ssidCheck: function(ssid_obj){
		var rc_support = '<% nvram_get("rc_support"); %>';
		var utf8_ssid_support = (rc_support.split(" ").indexOf("utf8_ssid") == -1) ? false : true;
		var c;
		var ssid = ssid_obj.val();
		var showHint = 0;
		var hintStr = "";

		ssid_obj.parent().children().remove(".hint");
		len = this.lengthInUtf8(ssid);
		if(len > 32){
			hintStr = "<#JS_max_ssid#>";
			showHint = 1;
		}

		for(var i = 0; i < len; ++i){
			c = ssid.charCodeAt(i);
			if(!utf8_ssid_support){
				if(this.ssidChar(c)){
					hintStr = '<#JS_validSSID1#> ' + ssid.charAt(i) + ' <#JS_validSSID2#>';
					showHint = 1;
				}
			}
		}

		if(showHint){
			$("<div>")
				.html(hintStr)
				.addClass("hint")
				.css("color", "#FC0")
				.css("margin", "2px 0 0 5px")
				.appendTo(ssid_obj.parent());

			ssid_obj.focus();
			return false;
		}
		else
			return true;
	},

	subnetAndMaskCombination: function (subnet, mask) { //ex. 10.66.77.6/255.255.255.248 is invalid, 10.66.77.8/255.255.255.248 is valid
		var subnetNum = inet_network(subnet);
		var networkAddrNum = 0;
		var subnetArray = subnet.split(".");
		var maskArray = mask.split(".");
		var networkAddr = "";
		networkAddr = (subnetArray[0] & maskArray[0]) + "." + (subnetArray[1] & maskArray[1]) + "." + 
					(subnetArray[2] & maskArray[2]) + "." + (subnetArray[3] & maskArray[3]);

		networkAddrNum = inet_network(networkAddr);
		if((networkAddrNum - subnetNum) != 0) {
			return false
		}
		else {
			return true
		}
	},

	timeRange: function(o, p){
		if (o.value.length==0) 
			o.value = "00";
		else if (o.value.length==1) 
			o.value = "0" + o.value;
			
		if (o.value.charAt(0)<'0' || o.value.charAt(0)>'9') 
			o.value = "00";
		else if (o.value.charAt(1)<'0' || o.value.charAt(1)>'9') 
			o.value = "00";
		else if (p==0 || p==2)
		{
			if(o.value>23){
				alert('<#JS_validrange#> 00 <#JS_validrange_to#> 23');
				o.value = "00";
				o.focus();
				o.select();
				return false;			
			}	
			return true;
		}
		else
		{
			if(o.value>59){
				alert('<#JS_validrange#> 00 <#JS_validrange_to#> 59');
				o.value = "00";
				o.focus();
				o.select();
				return false;			
			}	
			return true;
		}
		return true;
	},

	validateIP: function(o){
		var s = o.value;	

		if(s.indexOf(".") == 0 || s.indexOf(" ") == 0){
			o.value = s.substring(1,2);
		}
		else if(s.indexOf(".") == 1 || s.indexOf(" ") == 1){
			o.value = s.substring(0,1);
		}
		else if(s.indexOf(".") == 2 || s.indexOf(" ") == 2){
			o.value = s.substring(0,2);
		}	
	},

	validIPForm: function(obj, flag){
		if(obj.value == ""){
			return true;
		}else if(flag==0){	//0:without netMask
			if(!this.ipAddrFinal(obj, obj.name)){
				obj.focus();
				obj.select();
				return false;
			}else
				return true;
		}else if(flag==3){	//3:mute
			if(!this.ipAddrFinal(obj, obj.name, 1)){
				return false;
			}else
				return true;
		}else if(flag==1){	//with netMask and generate netmask
			var strIP = obj.value;
			var re = new RegExp("^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$", "gi");			
				
			if(!this.ipAddrFinal(obj, obj.name)){
				obj.focus();
				obj.select();		
				return false;	
			}			
				
			if(obj.name=="sr_ipaddr_x_0" && re.test(strIP)){
				if((RegExp.$1 > 0) && (RegExp.$1 < 127)) document.form.sr_netmask_x_0.value = "255.0.0.0";
				else if ((RegExp.$1 > 127) && (RegExp.$1 < 192)) document.form.sr_netmask_x_0.value = "255.255.0.0";
				else if ((RegExp.$1 > 191) && (RegExp.$1 < 224)) document.form.sr_netmask_x_0.value = "255.255.255.0";
				else document.form.sr_netmask_x_0.value = "0.0.0.0";												
			}else if(obj.name=="wan_ipaddr_x" && re.test(strIP)){
				if((RegExp.$1 > 0) && (RegExp.$1 < 127)) document.form.wan_netmask_x.value = "255.0.0.0";
				else if ((RegExp.$1 > 127) && (RegExp.$1 < 192)) document.form.wan_netmask_x.value = "255.255.0.0";
				else if ((RegExp.$1 > 191) && (RegExp.$1 < 224)) document.form.wan_netmask_x.value = "255.255.255.0";
				else document.form.wan_netmask_x.value = "0.0.0.0";												
			}else if(obj.name=="lan_ipaddr" && re.test(strIP)){
				if((RegExp.$1 > 0) && (RegExp.$1 < 127)) document.form.lan_netmask.value = "255.0.0.0";
				else if ((RegExp.$1 > 127) && (RegExp.$1 < 192)) document.form.lan_netmask.value = "255.255.0.0";
				else if ((RegExp.$1 > 191) && (RegExp.$1 < 224)) document.form.lan_netmask.value = "255.255.255.0";
				else document.form.lan_netmask.value = "0.0.0.0";				
			}
				
			return true;
		}else if(flag==2){ 	//ip plus netmask
					
			if(obj.value.search("/") == -1){		// only IP
				if(!this.ipAddrFinal(obj, obj.name)){
					obj.focus();
					obj.select();		
					return false;	
				}else
					return true;
			}else{															// IP plus netmask
				if(obj.value.split("/").length > 2){
					alert(obj.value + " <#JS_validip#>");
					obj.value = "";
					obj.focus();
					obj.select();
					return false;
				}else{
					if(obj.value.split("/")[1] == "" || obj.value.split("/")[1] == 0 || obj.value.split("/")[1] > 32){
						alert(obj.value + " <#JS_validip#>");
						obj.value = "";
						obj.focus();
						obj.select();
						return false;								
					}else{
						var IP_tmp = obj.value;
						obj.value = obj.value.split("/")[0];
						if(!this.ipAddrFinal(obj, obj.name)){
							obj.focus();
							obj.select();		
							return false;	
						}else{
							obj.value = IP_tmp;
							return true;
						}		
					}
				}
			}		
		}else
			return false;
	},

	wlKey: function(key_obj){
		var wep_type = "";
		if (document.form === undefined) {
			var wep_type_id = key_obj.id.slice(0, 8) + "x";
			wep_type = document.getElementById(wep_type_id).value;
		} else {
			wep_type = document.form.wl_wep_x.value;
		}

		var iscurrect = true;
		var str = "<#JS_wepkey#>";
		var wl_key_type = '<% nvram_get("wl_key_type"); %>';

		if(wep_type == "0")
			iscurrect = true;	// do nothing
		else if(wep_type == "1"){
			if(key_obj.value.length == 5 && this.string(key_obj)){
				if (wl_key_type !== "") {
					document.form.wl_key_type.value = 1; /*Lock Add 11.25 for ralink platform*/
				}

				iscurrect = true;
			}
			else if(key_obj.value.length == 10 && this.hex(key_obj)){
				if (wl_key_type !== "") {
					document.form.wl_key_type.value = 0; /*Lock Add 11.25 for ralink platform*/
				}

				iscurrect = true;
			}
			else{
				str += "(<#WLANConfig11b_WEPKey_itemtype1#>)";
				
				iscurrect = false;
			}
		}
		else if(wep_type == "2"){
			if(key_obj.value.length == 13 && this.string(key_obj)){
				if (wl_key_type !== "") {
  					document.form.wl_key_type.value = 1; /*Lock Add 11.25 for ralink platform*/
				}

				iscurrect = true;
			}
			else if(key_obj.value.length == 26 && this.hex(key_obj)){
				if (wl_key_type !== "") {
					document.form.wl_key_type.value = 0; /*Lock Add 11.25 for ralink platform*/
				}

				iscurrect = true;
			}
			else{
				str += "(<#WLANConfig11b_WEPKey_itemtype2#>)";
				
				iscurrect = false;
			}
		}
		else{
			alert("System error!");
			iscurrect = false;
		}
		
		if(iscurrect == false){
			alert(str);
			key_obj.focus();
			key_obj.select();
		}
		
		return iscurrect;
	},

	WPAPSK: function(o){
		if(o.value.length >= 64){
			o.value = o.value.substring(0, 63);
			alert("<#JS_wpapass#>");
			return false;
		}
		
		return true;
	},

	mac_addr: function(_value) {
		var hwaddr = new RegExp("(([a-fA-F0-9]{2}(\:|$)){6})", "gi");
		if(hwaddr.test(_value))
			return true;
		else
			return false;
	},

	ipv4_addr: function(_value) {
		//ip address accept is 0.0.0.0~255.255.255.255
		var ipformat  = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
		if((ipformat.test(_value)))
			return true;
		else
			return false;
	},

	ipv4_addr_range: function(_value) {
		//ip address accept is 0.0.0.0~255.255.255.255
		//format: 192.168.1.*, 192.168.1.2-100, 192.168.1.0/24
		var ipformatSubnet = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(\*)$/;
		if((ipformatSubnet.test(_value)))	//192.168.1.*
			return true;
		else {
			var ipformatRange = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)-(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?))$/;
			if((ipformatRange.test(_value))) {	//192.168.1.0-255
				var part = _value.split(".");
				var range = part[3].split("-");
				if(parseInt(range[0]) < parseInt(range[1]))
					return true;
				else
					return false;
			}
			else {
				var ipformatMask = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\/(3[0-2]|2[0-9]|1[0-9]|[1-9])$/;
				if((ipformatMask.test(_value)))	//192.168.1.0/24
					return true;
				else
					return false;
			}
		}
	},

	safeName: function(obj, flag){
		if (obj.value.length == 0) return true;

		var re = new RegExp(/^[a-zA-Z0-9:\-_. ]+$/gi);
		if(re.test(obj.value)){
			return true;
		}else{
			if (flag != "noalert"){
				alert("Only letters, numbers, spaces, underscores, periods and dashes are accepted.");
				obj.focus();
				obj.select();
			}
			return false;
		}
	},

        domainName_flag: function(_value) {
                //domin name
		var domainNameFormat = /^((?:(?:(?:\w[\.\-\+]?)*)\w)+)((?:(?:(?:\w[\.\-\+]?){0,62})\w)+)\.(\w{2,})$/;
                if(domainNameFormat.test(_value))
                        return true;
                else
                        return false;
	},

	dwb_check_wl_setting: function(_jsonPara) {
		var status = true;
		var edit_wl_unit = _jsonPara["edit_wl_unit"];
		var edit_wl_ssid = _jsonPara["edit_wl_ssid"];
		var dwb_unit = _jsonPara["dwb_unit"];
		var smart_connect = _jsonPara["smart_connect"];
		var current_ssid = _jsonPara["current_ssid"];
		if(edit_wl_unit != dwb_unit) {
			if(edit_wl_ssid == current_ssid[dwb_unit])//compare dwb ssid
				status = false;
		}
		else {
			if(smart_connect == "1") {
				if(edit_wl_ssid == current_ssid[0])//compare wl0 ssid
					status = false;
			}
			else {
				current_ssid.splice(dwb_unit, 1);//filter dwb ssid
				for (var idx in current_ssid) {
					if (current_ssid.hasOwnProperty(idx)) {
						if(edit_wl_ssid == current_ssid[idx]) {//compare all ssid
							status = false;
							break;
						}
					}
				}
			}
		}
		return status;
	},
	isMobileNumber: function(_value){
		var re = /^(\+?[1-9]{1}[0-9]{7,14}|[0-9]{7,14})$/g;
		if(re.test(_value))
			return true;
		else{
			return false;
		}
        }
};
