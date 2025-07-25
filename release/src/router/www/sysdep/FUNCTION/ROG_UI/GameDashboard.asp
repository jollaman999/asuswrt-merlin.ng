﻿<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=Edge"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<title><#Web_Title#> - Game Dashboard</title>
<link rel="icon" href="images/favicon.png">
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="css/basic.css">
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="tmmenu.css">
<link rel="stylesheet" type="text/css" href="menu_style.css">
<link rel="stylesheet" type="text/css" href="device-map/device-map.css">
<link rel="stylesheet" type="text/css" href="css/rog_cod.css">
<script type="text/javascript" src="js/loader.js"></script>
<script type="text/javascript" src="/js/jquery.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/httpApi.js"></script>
<script type="text/javascript" src="/form.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/client_function.js"></script>
<script language="JavaScript" type="text/javascript" src="/js/html5kellycolorpicker.min.js"></script>
<script type="text/javascript" src="/switcherplugin/jquery.iphone-switch.js"></script>
<script type="text/javascript" src="/md5.js"></script>
<script type="text/javascript" src="/js/asus_clientlist.js"></script>
<style>
.traffic_bar{
	width: 0%;
	height: 5px;
	border-radius:5px;
}
.traffic_bar_download{
	background-color: #32ADB2;
}
.traffic_bar_upload{
	background-color: #BCBD4D;
}
.transition_style{
	-webkit-transition: all 2s ease-in-out;
	-moz-transition: all 2s ease-in-out;
	-o-transition: all 2s ease-in-out;
	transition: all 2s ease-in-out;
}
.icon-wl2g {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJ3aWZpXzJnIj48ZyBpZD0iVmVjdG9yX19fX18wXzBfVUtEVUZZU0ZNVCI+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik03LjUwMDIgNC45MDAzVjYuMDk5NTFIMy41OTk4VjIyLjQwMDNIMjAuNDAwNlY2LjA5OTUxSDE3LjUwMDJWNC45MDAzSDIxLjU5OThWMjMuNTk5NUgyLjQwMDU5VjQuOTAwM0g3LjUwMDJaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xNS45NjMxIDExLjU2MjVDMTUuOTYzMSA3LjgyNjkzIDEzLjA0NzUgNC43OTUzIDkuMzU5NTcgNC42MDkzOEw5LjAwMDIgNC41OTk2MUg4LjQwMDU5VjMuNDAwMzlIOS4wMDAyTDkuNDIxMDkgMy40MTAxNkMxMy43NDMyIDMuNjI4MDkgMTcuMTYyMyA3LjE4NDggMTcuMTYyMyAxMS41NjI1VjEyLjE2MjFIMTUuOTYzMVYxMS41NjI1WiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTIuNDYzMSAxMS4xODc1QzEyLjQ2MzEgOS41MTA0MiAxMS4xMzEgOC4xMjY3NyA5LjQ3MzgzIDguMDQyMDFMOS4zMTI3IDguMDM3MTJIOC43MTMwOVY2LjgzNzkxSDkuMzEyN0w5LjUzNTM1IDYuODQyNzlDMTEuODI2NyA2Ljk1OTU2IDEzLjY2MjMgOC44NjgzIDEzLjY2MjMgMTEuMTg3NVYxMS43ODcxSDEyLjQ2MzFWMTEuMTg3NVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEzLjA3NDMgMTguODM3NVYxOC4yTDE3LjEyNDMgMTUuMDVIMTguNjk5M1YyMEgxNy41NzQzVjE4LjgzNzVIMTMuMDc0M1pNMTQuNTM2OCAxOC4ySDE3LjU3NDNWMTUuODM3NUwxNC41MzY4IDE4LjJaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xMC45MzgyIDIwVjE5LjFIMTIuMDYzMlYyMEgxMC45MzgyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNC4yOTk4NSAyMFYxNy45Mzc1QzQuMjk5ODUgMTYuOTI1IDUuMTk5ODUgMTYuOTI1IDYuMDk5ODUgMTYuOTI1SDguMzQ5ODVDOC41NzQ4NSAxNi45MjUgOC43OTk4NSAxNi45MjUgOC43OTk4NSAxNi42NjI1VjE1Ljk1QzguNzk5ODUgMTUuNjg3NSA4LjU3NDg1IDE1LjY4NzUgOC4zNDk4NSAxNS42ODc1SDQuNjc0ODVWMTUuMDVIOC4xMjQ4NUM5LjAyNDg1IDE1LjA1IDkuOTI0ODUgMTUuMDUgOS45MjQ4NSAxNi4wNjI1VjE2LjU1QzkuOTI0ODUgMTcuNTYyNSA5LjAyNDg1IDE3LjU2MjUgOC4xMjQ4NSAxNy41NjI1SDUuODc0ODVDNS42NDk4NSAxNy41NjI1IDUuNDI0ODUgMTcuNTYyNSA1LjQyNDg1IDE3LjgyNVYxOS4zNjI1SDkuOTI0ODVWMjBINC4yOTk4NVoiLz48L2c+PC9nPjwvZz48L3N2Zz4=");
}
.icon-wl5g {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV81ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX1NJT1hJREZNRk0iPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5OFYyMi40MDAzSDIwLjQwMDZWNi4wOTk1MUgxNy41MDAyVjQuOTAwM0gyMS41OTk4VjIzLjU5OTVIMi40MDA1OVY0LjkwMDNINy41MDAyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTUuOTYzMSAxMS41NjI1QzE1Ljk2MzEgNy44MjY5MyAxMy4wNDc1IDQuNzk1MyA5LjM1OTU3IDQuNjA5MzhMOS4wMDAyIDQuNTk5NjFIOC40MDA1OVYzLjQwMDM5SDkuMDAwMkw5LjQyMTA5IDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik04LjgxMiAyMFYxOS4yNzc1SDEzLjQwMkMxMy42NTcgMTkuMjc3NSAxMy45MTIgMTkuMjc3NSAxMy45MTIgMTguOThWMTcuNTM1QzEzLjkxMiAxNy4yMzc1IDEzLjY1NyAxNy4yMzc1IDEzLjQwMiAxNy4yMzc1SDguODEyVjE0LjM5SDE0Ljc2MlYxNS4xMTI1SDEwLjA4N1YxNi41MTVIMTMuMTQ3QzE0LjE2NyAxNi41MTUgMTUuMTg3IDE2LjUxNSAxNS4xODcgMTcuNjYyNVYxOC44NTI1QzE1LjE4NyAyMCAxNC4xNjcgMjAgMTMuMTQ3IDIwSDguODEyWiIvPjwvZz48L2c+PC9nPjwvc3ZnPg==");
}
.icon-wl5g1 {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV81ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX0lNSVJOVklQRkkiPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5OFYyMi40MDAzSDIwLjQwMDZWNi4wOTk1MUgxNy41MDAyVjQuOTAwM0gyMS41OTk4VjIzLjU5OTVIMi40MDA1OVY0LjkwMDNINy41MDAyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTUuOTYzMSAxMS41NjI1QzE1Ljk2MzEgNy44MjY5MyAxMy4wNDc1IDQuNzk1MyA5LjM1OTU3IDQuNjA5MzhMOS4wMDAyIDQuNTk5NjFIOC40MDA1OVYzLjQwMDM5SDkuMDAwMkw5LjQyMTA5IDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xOC44MjMyIDE1LjA1VjIwSDE3LjY5ODJWMTUuNjg3NUgxNi4wNDgyVjE1LjA1SDE4LjgyMzJaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xMC41MDE4IDE4LjU3NVYxNy44MjVIMTMuNTAxOFYxOC41NzVIMTAuNTAxOFoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTMuOTc1ODggMjBWMTkuMzYyNUg4LjAyNTg4QzguMjUwODggMTkuMzYyNSA4LjQ3NTg4IDE5LjM2MjUgOC40NzU4OCAxOS4xVjE3LjgyNUM4LjQ3NTg4IDE3LjU2MjUgOC4yNTA4OCAxNy41NjI1IDguMDI1ODggMTcuNTYyNUgzLjk3NTg4VjE1LjA1SDkuMjI1ODhWMTUuNjg3NUg1LjEwMDg4VjE2LjkyNUg3LjgwMDg4QzguNzAwODggMTYuOTI1IDkuNjAwODggMTYuOTI1IDkuNjAwODggMTcuOTM3NVYxOC45ODc1QzkuNjAwODggMjAgOC43MDA4OCAyMCA3LjgwMDg4IDIwSDMuOTc1ODhaIi8+PC9nPjwvZz48L2c+PC9zdmc+");
}
.icon-wl5g2 {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV81ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX09SUElaSFdYUkUiPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5ODFWMjIuNDAwM0gyMC40MDA2VjYuMDk5NTFIMTcuNTAwMlY0LjkwMDNIMjEuNTk5OFYyMy41OTk1SDIuNDAwNTlWNC45MDAzSDcuNTAwMloiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTE1Ljk2MzEgMTEuNTYyNUMxNS45NjMxIDcuODI2OTMgMTMuMDQ3NSA0Ljc5NTMgOS4zNTk1NyA0LjYwOTM4TDkuMDAwMiA0LjU5OTYxSDguNDAwNTlWMy40MDAzOUg5LjAwMDJMOS40MjExIDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xNC4zOTgyIDIwVjE3LjkzNzVDMTQuMzk4MiAxNi45MjUgMTUuMjk4MiAxNi45MjUgMTYuMTk4MiAxNi45MjVIMTguNDQ4MkMxOC42NzMyIDE2LjkyNSAxOC44OTgyIDE2LjkyNSAxOC44OTgyIDE2LjY2MjVWMTUuOTVDMTguODk4MiAxNS42ODc1IDE4LjY3MzIgMTUuNjg3NSAxOC40NDgyIDE1LjY4NzVIMTQuNzczMlYxNS4wNUgxOC4yMjMyQzE5LjEyMzIgMTUuMDUgMjAuMDIzMiAxNS4wNSAyMC4wMjMyIDE2LjA2MjVWMTYuNTVDMjAuMDIzMiAxNy41NjI1IDE5LjEyMzIgMTcuNTYyNSAxOC4yMjMyIDE3LjU2MjVIMTUuOTczMkMxNS43NDgyIDE3LjU2MjUgMTUuNTIzMiAxNy41NjI1IDE1LjUyMzIgMTcuODI1VjE5LjM2MjVIMjAuMDIzMlYyMEgxNC4zOTgyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTAuNTAxOCAxOC41NzVWMTcuODI1SDEzLjUwMThWMTguNTc1SDEwLjUwMThaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0zLjk3NTg4IDIwVjE5LjM2MjVIOC4wMjU4OEM4LjI1MDg4IDE5LjM2MjUgOC40NzU4OCAxOS4zNjI1IDguNDc1ODggMTkuMVYxNy44MjVDOC40NzU4OCAxNy41NjI1IDguMjUwODggMTcuNTYyNSA4LjAyNTg4IDE3LjU2MjVIMy45NzU4OFYxNS4wNUg5LjIyNTg4VjE1LjY4NzVINS4xMDA4OFYxNi45MjVINy44MDA4OEM4LjcwMDg4IDE2LjkyNSA5LjYwMDg4IDE2LjkyNSA5LjYwMDg4IDE3LjkzNzVWMTguOTg3NUM5LjYwMDg4IDIwIDguNzAwODggMjAgNy44MDA4OCAyMEgzLjk3NTg4WiIvPjwvZz48L2c+PC9nPjwvc3ZnPg==");
}
.icon-wl6g {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV82ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX1lST1RYSFBGTVoiPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5OFYyMi40MDAzSDIwLjQwMDZWNi4wOTk1MUgxNy41MDAyVjQuOTAwM0gyMS41OTk4VjIzLjU5OTVIMi40MDA1OVY0LjkwMDNINy41MDAyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTUuOTYzMSAxMS41NjI1QzE1Ljk2MzEgNy44MjY5MyAxMy4wNDc1IDQuNzk1MyA5LjM1OTU3IDQuNjA5MzhMOS4wMDAyIDQuNTk5NjFIOC40MDA1OVYzLjQwMDM5SDkuMDAwMkw5LjQyMTA5IDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik04LjgxMjIgMTguODUyNVYxNS43NUM4LjgxMjIgMTQuMzkgMTAuMDAyMiAxNC4zOSAxMS4xOTIyIDE0LjM5SDE0Ljc2MjJWMTUuMTEyNUgxMS4yNzcyQzEwLjY4MjIgMTUuMTEyNSAxMC4wODcyIDE1LjExMjUgMTAuMDg3MiAxNS43OTI1VjE2LjUxNUgxMy4xNDcyQzE0LjE2NzIgMTYuNTE1IDE1LjE4NzIgMTYuNTE1IDE1LjE4NzIgMTcuNjYyNVYxOC44NTI1QzE1LjE4NzIgMjAgMTQuMTY3MiAyMCAxMy4xNDcyIDIwSDEwLjg1MjJDOS44MzIyIDIwIDguODEyMiAyMCA4LjgxMjIgMTguODUyNVpNMTAuMDg3MiAxOC45OEMxMC4wODcyIDE5LjI3NzUgMTAuMzQyMiAxOS4yNzc1IDEwLjU5NzIgMTkuMjc3NUgxMy40MDIyQzEzLjY1NzIgMTkuMjc3NSAxMy45MTIyIDE5LjI3NzUgMTMuOTEyMiAxOC45OFYxNy41MzVDMTMuOTEyMiAxNy4yMzc1IDEzLjY1NzIgMTcuMjM3NSAxMy40MDIyIDE3LjIzNzVIMTAuNTk3MkMxMC4zNDIyIDE3LjIzNzUgMTAuMDg3MiAxNy4yMzc1IDEwLjA4NzIgMTcuNTM1VjE4Ljk4WiIvPjwvZz48L2c+PC9nPjwvc3ZnPg==");
}
.icon-wl6g1 {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV82ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX05OQVBWT0hETUYiPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5OFYyMi40MDAzSDIwLjQwMDZWNi4wOTk1MUgxNy41MDAyVjQuOTAwM0gyMS41OTk4VjIzLjU5OTVIMi40MDA1OVY0LjkwMDNINy41MDAyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTUuOTYzMSAxMS41NjI1QzE1Ljk2MzEgNy44MjY5MyAxMy4wNDc1IDQuNzk1MyA5LjM1OTU3IDQuNjA5MzhMOS4wMDAyIDQuNTk5NjFIOC40MDA1OVYzLjQwMDM5SDkuMDAwMkw5LjQyMTA5IDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xOC44MjMyIDE1LjA1VjIwSDE3LjY5ODJWMTUuNjg3NUgxNi4wNDgyVjE1LjA1SDE4LjgyMzJaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xMC41MDE4IDE4LjU3NVYxNy44MjVIMTMuNTAxOFYxOC41NzVIMTAuNTAxOFoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTMuOTc1ODggMTguOTg3NVYxNi4yNUMzLjk3NTg4IDE1LjA1IDUuMDI1ODggMTUuMDUgNi4wNzU4OCAxNS4wNUg5LjIyNTg4VjE1LjY4NzVINi4xNTA4OEM1LjYyNTg4IDE1LjY4NzUgNS4xMDA4OCAxNS42ODc1IDUuMTAwODggMTYuMjg3NVYxNi45MjVINy44MDA4OEM4LjcwMDg4IDE2LjkyNSA5LjYwMDg4IDE2LjkyNSA5LjYwMDg4IDE3LjkzNzVWMTguOTg3NUM5LjYwMDg4IDIwIDguNzAwODggMjAgNy44MDA4OCAyMEg1Ljc3NTg4QzQuODc1ODggMjAgMy45NzU4OCAyMCAzLjk3NTg4IDE4Ljk4NzVaTTUuMTAwODggMTkuMUM1LjEwMDg4IDE5LjM2MjUgNS4zMjU4OCAxOS4zNjI1IDUuNTUwODggMTkuMzYyNUg4LjAyNTg4QzguMjUwODggMTkuMzYyNSA4LjQ3NTg4IDE5LjM2MjUgOC40NzU4OCAxOS4xVjE3LjgyNUM4LjQ3NTg4IDE3LjU2MjUgOC4yNTA4OCAxNy41NjI1IDguMDI1ODggMTcuNTYyNUg1LjU1MDg4QzUuMzI1ODggMTcuNTYyNSA1LjEwMDg4IDE3LjU2MjUgNS4xMDA4OCAxNy44MjVWMTkuMVoiLz48L2c+PC9nPjwvZz48L3N2Zz4=");
}
.icon-wl6g2 {
	--svg: url("data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIyNCIgaGVpZ2h0PSIyNCIgZmlsbD0ibm9uZSIgdmlld0JveD0iMCAwIDI0IDI0Ij48ZyBpZD0iR3JvdXAiPjxnIGlkPSJEZXZpY2VzIC8gd2lmaV82ZyI+PGcgaWQ9IlZlY3Rvcl9fX19fMF8wX0tESUZNQU5RT1QiPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNNy41MDAyIDQuOTAwM1Y2LjA5OTUxSDMuNTk5ODFWMjIuNDAwM0gyMC40MDA2VjYuMDk5NTFIMTcuNTAwMlY0LjkwMDNIMjEuNTk5OFYyMy41OTk1SDIuNDAwNTlWNC45MDAzSDcuNTAwMloiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTE1Ljk2MzEgMTEuNTYyNUMxNS45NjMxIDcuODI2OTMgMTMuMDQ3NSA0Ljc5NTMgOS4zNTk1NyA0LjYwOTM4TDkuMDAwMiA0LjU5OTYxSDguNDAwNTlWMy40MDAzOUg5LjAwMDJMOS40MjExIDMuNDEwMTZDMTMuNzQzMiAzLjYyODA5IDE3LjE2MjMgNy4xODQ4IDE3LjE2MjMgMTEuNTYyNVYxMi4xNjIxSDE1Ljk2MzFWMTEuNTYyNVoiLz48cGF0aCBmaWxsPSJibGFjayIgZD0iTTEyLjQ2MzEgMTEuMTg3NUMxMi40NjMxIDkuNTEwNDIgMTEuMTMxIDguMTI2NzcgOS40NzM4MyA4LjA0MjAxTDkuMzEyNyA4LjAzNzEySDguNzEzMDlWNi44Mzc5MUg5LjMxMjdMOS41MzUzNSA2Ljg0Mjc5QzExLjgyNjcgNi45NTk1NiAxMy42NjIzIDguODY4MyAxMy42NjIzIDExLjE4NzVWMTEuNzg3MUgxMi40NjMxVjExLjE4NzVaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0xNC4zOTgyIDIwVjE3LjkzNzVDMTQuMzk4MiAxNi45MjUgMTUuMjk4MiAxNi45MjUgMTYuMTk4MiAxNi45MjVIMTguNDQ4MkMxOC42NzMyIDE2LjkyNSAxOC44OTgyIDE2LjkyNSAxOC44OTgyIDE2LjY2MjVWMTUuOTVDMTguODk4MiAxNS42ODc1IDE4LjY3MzIgMTUuNjg3NSAxOC40NDgyIDE1LjY4NzVIMTQuNzczMlYxNS4wNUgxOC4yMjMyQzE5LjEyMzIgMTUuMDUgMjAuMDIzMiAxNS4wNSAyMC4wMjMyIDE2LjA2MjVWMTYuNTVDMjAuMDIzMiAxNy41NjI1IDE5LjEyMzIgMTcuNTYyNSAxOC4yMjMyIDE3LjU2MjVIMTUuOTczMkMxNS43NDgyIDE3LjU2MjUgMTUuNTIzMiAxNy41NjI1IDE1LjUyMzIgMTcuODI1VjE5LjM2MjVIMjAuMDIzMlYyMEgxNC4zOTgyWiIvPjxwYXRoIGZpbGw9ImJsYWNrIiBkPSJNMTAuNTAxOCAxOC41NzVWMTcuODI1SDEzLjUwMThWMTguNTc1SDEwLjUwMThaIi8+PHBhdGggZmlsbD0iYmxhY2siIGQ9Ik0zLjk3NTg4IDE4Ljk4NzVWMTYuMjVDMy45NzU4OCAxNS4wNSA1LjAyNTg4IDE1LjA1IDYuMDc1ODggMTUuMDVIOS4yMjU4OFYxNS42ODc1SDYuMTUwODhDNS42MjU4OCAxNS42ODc1IDUuMTAwODggMTUuNjg3NSA1LjEwMDg4IDE2LjI4NzVWMTYuOTI1SDcuODAwODhDOC43MDA4OCAxNi45MjUgOS42MDA4OCAxNi45MjUgOS42MDA4OCAxNy45Mzc1VjE4Ljk4NzVDOS42MDA4OCAyMCA4LjcwMDg4IDIwIDcuODAwODggMjBINS43NzU4OEM0Ljg3NTg4IDIwIDMuOTc1ODggMjAgMy45NzU4OCAxOC45ODc1Wk01LjEwMDg4IDE5LjFDNS4xMDA4OCAxOS4zNjI1IDUuMzI1ODggMTkuMzYyNSA1LjU1MDg4IDE5LjM2MjVIOC4wMjU4OEM4LjI1MDg4IDE5LjM2MjUgOC40NzU4OCAxOS4zNjI1IDguNDc1ODggMTkuMVYxNy44MjVDOC40NzU4OCAxNy41NjI1IDguMjUwODggMTcuNTYyNSA4LjAyNTg4IDE3LjU2MjVINS41NTA4OEM1LjMyNTg4IDE3LjU2MjUgNS4xMDA4OCAxNy41NjI1IDUuMTAwODggMTcuODI1VjE5LjFaIi8+PC9nPjwvZz48L2c+PC9zdmc+");
}
.ic-signal{
	-webkit-mask-image: var(--svg);
	mask-image: var(--svg);
	-webkit-mask-repeat: no-repeat;
	-webkit-mask-position: center;
	-webkit-mask-size: contain;
	display:inline-block;
	width:48px;
	height:48px;
}
.ic-signal.active{
	background-color: #53CECB
}
.ic-signal.resting{
	background-color: #53514f;
}
.wan_state_icon{
	width: 136px;
	height: 136px;
	margin: 5px 0 0 75px;
}
.wan_icon_connect{
	background: url('images/New_ui/wan-connect.png') no-repeat;
}
.wan_icon_disconnect{
	background: url('images/New_ui/wan-disconnect.png') no-repeat;
}

.aura-scheme-container{
	display: flex;
	overflow-x: auto;
	overflow-y: hidden;
	word-break: break-all;
}
.aura-event-container{
	position: absolute;
	z-index: 20;
	width: 345px;
	background: rgba(0,0,0,.9);
	font-family: Roboto;
	font-size: 14px;
	padding: 0 5px;
	overflow-y: auto;
	transition-duration: 0.3s;
	transition-timing-function: ease-out;
}
.aura-event-desc-show{
	margin-left: 0;
	opacity: 1;
	height: 425px;
}

.aura-event-desc-hide{
	margin-left: 0px;
	opacity: 0;
	height: 0;
}
.aura-scheme-container::-webkit-scrollbar-track,
.aura-event-container::-webkit-scrollbar-track{
    border: 6px solid #000;
	padding: 2px 0;
	background-color: #404040;
}
.aura-scheme-container::-webkit-scrollbar,
.aura-event-container::-webkit-scrollbar{
    width: 10px;
}
.aura-scheme-container::-webkit-scrollbar-thumb,
.aura-event-container::-webkit-scrollbar-thumb{
    border: 4px solid #000;
    border-radius: 10px;
    background-color: #737272;
}
.aura-scheme{
	margin: 8px;
	text-align: center;
}
.aura-icon{
	width: 36px;
	height: 36px;
	margin: 0 6px;
	background-repeat: no-repeat;
	cursor:pointer;
	background-size: 100%;
}
.aura-desc{
	margin-top: 8px;
	line-height: 14px;
	word-break: keep-all;
}
.aura-icon-static{
	background-image: url('images/aura/static.svg');
}
.aura-icon-static:hover, .aura-icon-static-active{
	background-image: url('images/aura/static_active.svg');
}
.aura-icon-breathing{
	background-image: url('images/aura/breathing.svg');
}
.aura-icon-breathing:hover, .aura-icon-breathing-active{
	background-image: url('images/aura/breathing_active.svg');
}
.aura-icon-rainbow{
	background-image: url('images/aura/rainbow.svg');
}
.aura-icon-rainbow:hover, .aura-icon-rainbow-active{
	background-image: url('images/aura/rainbow_active.svg');
}
.aura-icon-comet{
	background-image: url('images/aura/comet.svg');
}
.aura-icon-comet:hover, .aura-icon-comet-active{
	background-image: url('images/aura/comet_active.svg');
}
.aura-icon-flash{
	background-image: url('images/aura/flash.svg');
}
.aura-icon-flash:hover, .aura-icon-flash-active{
	background-image: url('images/aura/flash_active.svg');
}
.aura-icon-yoyo{
	background-image: url('images/aura/glowing_yoyo.svg');
}
.aura-icon-yoyo:hover, .aura-icon-yoyo-active{
	background-image: url('images/aura/glowing_yoyo_active.svg');
}
.aura-icon-ccycle{
	background-image: url('images/aura/color_cycle.svg');
}
.aura-icon-ccycle:hover, .aura-icon-ccycle-active{
	background-image: url('images/aura/color_cycle_active.svg');
}
.aura-icon-snight{
	background-image: url('images/aura/starry_night.svg');
}
.aura-icon-snight:hover, .aura-icon-snight-active{
	background-image: url('images/aura/starry_night_active.svg');
}
.aura-icon-strobing{
	background-image: url('images/aura/strobing.svg');
}
.aura-icon-strobing:hover, .aura-icon-strobing-active{
	background-image: url('images/aura/strobing_active.svg');
}
.aura-icon-wave{
	background-image: url('images/aura/wave.svg');
}
.aura-icon-wave:hover, .aura-icon-wave-active{
	background-image: url('images/aura/wave_active.svg');
}
.aura-icon-event{
	background-image: url('images/aura/event.svg');
}
.aura-icon-event:hover, .aura-icon-event-active{
	background-image: url('images/aura/event_active.svg');
}
.boost-function{
	width: 150px;
	font-size: 16px;
	font-weight: bolder;
	padding-left: 10px;
	background: #111;
	display: flex;
	align-items: center;
	transform: skew(-30deg);
}
.boost-border-odd{
	border-top: 1px solid rgb(145,7,31);
	border-bottom: 1px solid rgb(145,7,31);
}
.boost-border-even{
	border: 1px solid rgb(145,7,31);
}
.boost-key-checked, .boost-function:hover{
	background: rgba(145,7,31, .7);
}
.boost-text{
	font-family: Xolonium;
	transform: skew(30deg);
}
.rog-title{
	font-family: Xolonium;
	font-size: 20px;
	color:#BFBFBF;
}
.event-cancel{
	width: 20px;
	height: 20px;
	background: url('images/gameprofile/cancel.svg') no-repeat center;
	border: 1px solid #842500;
	background-color:#262626;
	border-radius: 50%;
	margin-right: 5px;
	cursor: pointer;
}
.event-cancel:hover{
	border: 1px solid #E64000;
}
.light_effect_iframe {
	width: 100%;
	height: 100%;
}
.light_effect_iframe .light_effect_bg {
	height: 160px;
}
#wan_state, #sw_mode_desc {
	font-family: Xolonium;
}
.titledown, #ssidTitle{
	visibility: hidden !important;
}
</style>
<script>
// disable auto log out
AUTOLOGOUT_MAX_MINUTE = 0;
var isDemoMode = ('<% nvram_get("demoMode"); %>' == 1) ? true : false;

google.charts.load('current', {'packages':['corechart']});
<% wanlink(); %>
var ddns_enable = '<% nvram_get("ddns_enable_x"); %>';
var ddns_server_x = '<% nvram_get("ddns_server_x");%>';
var ddnsName = decodeURIComponent('<% nvram_char_to_ascii("", "ddns_hostname_x"); %>');

var dataArray = new Array();
var time = new Date();
var timestamp = time.getTime();
var t_year = time.getFullYear();
var t_mon = time.getMonth();
var t_date = time.getDate();
var t_hour = time.getHours();
var t_min = time.getMinutes();
var t_sec = time.getSeconds();

var label_mac = <% get_label_mac(); %>;
var CNSku = in_territory_code("CN");

for(i=0;i<30;i++){
	var temp = [];
	temp = [new Date(t_year, t_mon, t_date, t_hour, t_min, t_sec),  0, '', 0, ''];
	dataArray.push(temp);
	timestamp -= 3000;
	time = new Date(timestamp);
	getTime();
}

google.charts.setOnLoadCallback(function(){
	drawChart(dataArray);
});  

function getTime(){
	t_year = time.getFullYear();
	t_mon = time.getMonth();
	t_date = time.getDate();
	t_hour = time.getHours();
	t_min = time.getMinutes();
	t_sec = time.getSeconds();
}

var aurargb = "";
var aura_settings = new Array();
var boost_id = "";
function initial(){
	if(odm_support){
		$(".banner").attr("class", "banner_COD");
		$("#wan_state_icon").attr("class", "cod_connect");
	}

	if(isGundam || isEva){
		$(".banner").attr("class", "banner_GD");
		$("#wan_state_icon").attr("class", "gd_connect");
	}
	
	if(rog_support){
		$("#pingMap").show();
		if (aura_support) {
			if(based_modelid == 'GT-AC2900'){
				$("#aura_event").show();
			}

			$("#aura_field").show();
		}
		if (boostKey_support) {
			$("#boostKey_field").show();
		}
	}
	else{
		$("#wan_state_icon").attr("class", "wan_state_icon");
	}

	if(isSupport("ledg")){
		$("#aura_field").show();
		$("#light_effect_container").show();
		$("#aura_rgb_bg").hide();
	}

	if($("#aura_field").css("display") == "none")
		$('#pingMap').height('430px');

	if(uu_support && based_modelid == 'GT-AC5300'){
		$('#uu_field').show();
	}

	show_menu();
	update_tarffic();
	check_sw_mode();
	check_wireless();
	updateClientsCount();

	if((isSwMode("RT") || isSwMode("WISP"))){
		if (ddns_enable == '0' || ddnsName == '' || ddnsName == isMD5DDNSName()) {
			$('#wan_ip_title').html('WAN IP');
			updateWANIP();
		}
		else {
			$('#wan_ip_title').html('DDNS');
			$('#wan_ip_field').html(ddnsName);
		}
	}
	else{	// AP/Repeater/Media Bridge
		$('#wan_ip_title').html("<#LAN_IP#>");
		$('#wan_ip_field').html('<% nvram_get("lan_ipaddr"); %>');
	}

	aura_settings = document.form.aurargb_val.value.split(',');
	aurargb = "#" + rgbToHex(aura_settings[0]) + rgbToHex(aura_settings[1]) + rgbToHex(aura_settings[2]);
	setColor(aurargb);

	if(httpApi.nvramGet(["aurargb_enable"]).aurargb_enable == "1"){
		$('#aura_switch').prop('checked', true);
	}

	for(i=0; i < auraEffectArray.length; i++){
		if(aura_settings[3] == auraEffectArray[i][2]){
			$("#" + auraEffectArray[i][0]).removeClass(auraEffectArray[i][1]).addClass(auraEffectArray[i][1] + "-active");
		}
	}

	if((isSwMode("RT") || isSwMode("WISP")) || isSwMode("ap")){
		if(Qcawifi_support){
			var ch = eval('<% channel_list_5g(); %>').toString().split(",");
			if(isSupport("triband"))
			ch += eval('<% channel_list_5g_2(); %>').toString().split(",");
			if(ch.indexOf("52") != -1 || ch.indexOf("56") != -1 || ch.indexOf("60") != -1 || ch.indexOf("64") != -1 || ch.indexOf("100") != -1 || ch.indexOf("104") != -1 || ch.indexOf("108") != -1 || ch.indexOf("112") != -1 || ch.indexOf("116") != -1 || ch.indexOf("120") != -1 || ch.indexOf("124") != -1 || ch.indexOf("128") != -1 || ch.indexOf("132") != -1 || ch.indexOf("136") != -1 || ch.indexOf("140") != -1 || ch.indexOf("144") != -1){
				document.getElementById("boost_dfs").style.display = "";
			}
		}else{
			var wl1_reg_mode = '<% nvram_get("wl1_reg_mode"); %>';
			if(isSupport("triband")){
				var wl2_reg_mode = '<% nvram_get("wl2_reg_mode"); %>';
			}

			if(wl1_reg_mode == 'h' || wl2_reg_mode == 'h'){
				document.getElementById("boost_dfs").style.display = "";
			}
		}
	}

	if ((isSwMode("RT") || isSwMode("WISP")))
		document.getElementById("boost_qos").style.display = "";

	
		
	var boost_key = '<% nvram_get("turbo_mode"); %>';
	$("#" + _array[boost_key]).addClass("boost-key-checked");
	boost_id = _array[boost_key];

	if(based_modelid == "GT-AC2900"){
		$('#boost_led').hide();
		$('#boost_shuffle').show();
		$('#boost_aura').hide();

		$('#boost_shuffle').mouseenter(function(e){
			boostKeyDesc(e.target.id);
		}).mouseleave(function(){
			boostKeyDesc(boost_id);
		});
		$('#boost_qos').mouseenter(function(e){
			boostKeyDesc(e.target.id);
		}).mouseleave(function(){
			boostKeyDesc(boost_id);
		});
		$('#boost_dfs').mouseenter(function(e){
			boostKeyDesc(e.target.id);
		}).mouseleave(function(){
			boostKeyDesc(boost_id);
		});

		// GT-AC2900, handle Geforce Now
		$('#boost_geforce').show();
		$('#boost_geforce').mouseenter(function(e){
			boostKeyDesc(e.target.id);
		}).mouseleave(function(){
			boostKeyDesc(boost_id);
		});

		boostKeyDesc(boost_id);
	}
	else{
		$('#boost_led').show();
		$('#boost_shuffle').hide();
		$('#boost_aura').show();
	}

	var boostKey = httpApi.boostKey_support();
	$(".boost-function").hover(function(){
		$("#boostKey_desc").html(boostKey[this.id].desc)
	})

	httpApi.nvramGetAsync({
		data: ["ping_target"],
		success: function(nvram){
			netoolApiDashBoard.start({
				"type": 1, 
				"target": (nvram.ping_target == "") ? "www.google.com" : nvram.ping_target
			});
		}
	})
}

function updateWANIP(){
	$.ajax({
		url: '/ajax_wanlink.asp',
		dataType: 'script', 
		error: function(xhr) {
			setTimeout("updateWANIP();", 1000);
		},
		success: function(){
			$('#wan_ip_field').html(wanlink_ipaddr());
			setTimeout("updateWANIP();", 3000);
		}
	});
}

function check_sw_mode(){
	var mode = "";
	if(sw_mode == "1")
		mode = "<#wireless_router#>";
	else if(sw_mode == "2"){
		if(wlc_express == 1)
			mode = "<#OP_RE2G_item#>";
		else if(wlc_express == 2)
			mode = "<#OP_RE5G_item#>";
		else
			mode = "<#OP_RE_item#>";
	}
	else if(sw_mode == "3")
		mode = "<#OP_AP_item#>";
	else if(sw_mode == "4")
		mode = "<#OP_MB_item#>";
	else
		mode = "Unknown";	

	$("#sw_mode_desc").html(mode);
}

function check_wireless(){
	const options = [
		{ "band": "2g", "dut_list_bit": "1", "unit": "", "radio": "0" },
		{ "band": "5g", "dut_list_bit": "2", "unit": "", "radio": "0" },
		{ "band": "5g1", "dut_list_bit": "4", "unit": "", "radio": "0" },
		{ "band": "5g2", "dut_list_bit": "8", "unit": "", "radio": "0" },
		{ "band": "6g", "dut_list_bit": "16", "unit": "", "radio": "0" },
		{ "band": "6g1", "dut_list_bit": "32", "unit": "", "radio": "0" },
		{ "band": "6g2", "dut_list_bit": "64", "unit": "", "radio": "0" },
	];

	const wifi_band_info = (() => {
		if (isSupport("noWiFi")) {
			return [];
		}
		let result = [];
		if (get_wl_unit_by_band("2G") !== "") {
			const option = options.find(el => el.dut_list_bit === "1");
			if (option) result = result.concat(option);
		}
		if (get_wl_unit_by_band("5G2") !== "") {
			result = result.concat(options.filter(el => ["4", "8"].includes(el.dut_list_bit)));
		}
		else if (get_wl_unit_by_band("5G") !== "") {
			const option = options.find(el => el.dut_list_bit === "2");
			if (option) result = result.concat(option);
		}
		if (get_wl_unit_by_band("6G2") !== "") {
			result = result.concat(options.filter(el => ["32", "64"].includes(el.dut_list_bit)));
		}
		else if (get_wl_unit_by_band("6G") !== "") {
			const option = options.find(el => el.dut_list_bit === "16");
			if (option) result = result.concat(option);
		}
		return result;
	})();
	for (const item of wifi_band_info) {
		item.unit = get_wl_unit_by_band(item.band) || "";
	}

	if (isSupport('sdn_mainfh')) {
		let mainFHNum = 0;
		const mainFH = decodeURIComponent(httpApi.nvramCharToAscii(["sdn_rl"]).sdn_rl).split("<").filter(item => item.includes("MAINFH"));
		mainFH.forEach(item => {
			if (item !== "") {
				const apmIdx = item.split(">")[5];
				const apm_dut_list = decodeURIComponent(httpApi.nvramCharToAscii([`apm${apmIdx}_dut_list`])[`apm${apmIdx}_dut_list`]);
				const first_dut_list = apm_dut_list.split("<").find(dut_info => dut_info !== "");
				const wifi_band = first_dut_list ? parseInt(first_dut_list.split(">")[1], 10) || 0 : 0;
				mainFHNum += wifi_band;
			}
		});
		wifi_band_info.forEach(item => {
			item.radio = (parseInt(item.dut_list_bit, 10) & mainFHNum) ? "1" : "0";
		});
	}
	else {
		const wl_radio = httpApi.nvramGet(['wl0_radio', 'wl1_radio', 'wl2_radio', 'wl3_radio']);
		wifi_band_info.forEach(item => {
			item.radio = wl_radio[`wl${item.unit}_radio`] || "0";
		});
	}
	wifi_band_info.forEach((item, idx) => {
		let wifiClass = `icon-wl${item.band} resting`;
		if (isSwMode('mb')) {
			wifiClass = `icon-wl${item.band} resting`;
		} else if (item.radio === '1') {
			wifiClass = `icon-wl${item.band} active`;
		}
		$(`#wl${idx}_icon`).addClass(wifiClass).show();
	});
}

function drawChart(data){
	var dataTable = new google.visualization.DataTable();
	dataTable.addColumn('datetime', 'Date');
	dataTable.addColumn('number', '<#tm_transmission#>');
	dataTable.addColumn({type: 'string', role: 'tooltip'});
	dataTable.addColumn('number', '<#tm_reception#>');
	dataTable.addColumn({type: 'string', role: 'tooltip'});
	dataTable.addRows(data);

	var options = {
		backgroundColor: "transparent",
			title: 'Internet Traffic',
			legend: { position: 'top' },
			legendTextStyle: { color: '#BFBFBF' },
			colors: ['#BCBD4D', '#32ADB2'],
			vAxis: {
				format: '#,### KB/s',
				gridlines: {count: 4},
				textStyle: {color: '#BFBFBF'},
			},
			hAxis: {
				textStyle: {color: '#BFBFBF'}
			}
	}

	var chart = new google.visualization.AreaChart(document.getElementById('area_chart'));   
    chart.draw(dataTable, options);
}
var last_rx = 0
var last_tx = 0
var current_rx = 0;
var current_tx = 0;
var traffic_array = new Array();
function update_tarffic() {
	$.ajax({
    	url: '/update.cgi',
    	dataType: 'script',	
    	data: {'output': 'netdev'},
    	error: function(xhr) {
			setTimeout("update_tarffic();", 1000);
    	},
    	success: function(response){
    		var diff_rx = 0;
    		if(last_rx != 0){
    			if((current_rx - last_rx) < 0){
    				diff_rx = 1;
    			}
    			else{
    				diff_rx = (current_rx - last_rx)/2;
    			}
    		}

    		last_rx = current_rx;
    		if(netdev.INTERNET){
    			current_rx = netdev.INTERNET.rx;
    		}
    		else{
    			current_rx = netdev.WIRED.rx + netdev.WIRELESS0.rx + netdev.WIRELESS1.rx;
    			if( netdev.WIRELESS2){
    				current_rx += netdev.WIRELESS2.rx;
    			}
    		}

    		var diff_tx = 0;
    		if(last_tx != 0){
    			if((current_tx - last_tx) < 0){
    				diff_tx = 1;
    			}
    			else{
    				diff_tx = (current_tx - last_tx)/2;
	    		}
    		}

    		last_tx = current_tx;
    		if(netdev.INTERNET){
    			current_tx = netdev.INTERNET.tx;
    		}
    		else{
    			current_tx = netdev.WIRED.tx + netdev.WIRELESS0.tx + netdev.WIRELESS1.tx;
    			if( netdev.WIRELESS2){
    				current_tx += netdev.WIRELESS2.tx;
    			}
    		}
    		
    		traffic_bar(diff_tx, diff_rx);
    		refineData(diff_tx, diff_rx);
			setTimeout("update_tarffic();drawChart(dataArray);", 2000);
    	}
	});
}

function traffic_bar(tx, rx){
	/*basic unit: Bytes*/
	var temp = "";
	temp = translate_traffic(tx);
	$("#upload_traffic").html(temp[0] + " " + temp[1]);
	$("#upload_traffic_bar").css("width", temp[2]);
	temp = translate_traffic(rx);
	$("#download_traffic").html(temp[0] + " " + temp[1]);
	$("#download_traffic_bar").css("width", temp[2]);
}

function translate_traffic(traffic){
	var value = 0;
	var unit = "Bytes";
	var bar_width = "";
	var bar_value = 0;

	if(traffic != 0)
		bar_value = 1;

	if(traffic > 1024){
		traffic = traffic/1024;
		unit = "KB";
		if(traffic > 1024){
			traffic = traffic/1024;
			unit = "MB";
			bar_value = parseInt(traffic);
			if(bar_value > 100)
				bar_value = 100;
		}
	}

	bar_width = bar_value + "%";
	return [traffic.toFixed(2), unit, bar_width];
}

function refineData(tx, rx){
	time = new Date();
	getTime();
	var tooltip_tx = "";
	var tooltip_rx = "";
	var label_tx = tx + "Bytes/s";
	var label_rx = rx + "Bytes/s";
	rx_temp = rx;
	tx_temp = tx;
	if(tx > 1024){
		label_tx = (tx/1024).toFixed(2) + "KB/s";
		tx = tx/1024;

		if(tx > 1024){
			label_tx = (tx/1024).toFixed(2) + "MB/s";
		}
	}

	if(rx > 1024){
		label_rx = (rx/1024).toFixed(2) + "KB/s";
		rx = rx/1024;

		if(rx > 1024){
			label_rx = (rx/1024).toFixed(2) + "MB/s";
		}
	}	

	tooltip_tx = new Date(t_year, t_mon, t_date, t_hour, t_min, t_sec) + "\n TX: " + label_tx;
	tooltip_rx = new Date(t_year, t_mon, t_date, t_hour, t_min, t_sec) + "\n RX: " + label_rx;
    var temp = [new Date(t_year, t_mon, t_date, t_hour, t_min, t_sec),  tx_temp/1024, tooltip_tx , rx_temp/1024, tooltip_rx];	

    dataArray.push(temp);
    dataArray.shift();
}

var targetData = {}

function initTargetData(){
	var retObj = {};
	retObj["points"] = [];
	retObj["sum"] = 0;
	retObj["avg"] = 0;
	retObj["pingMax"] = 0;
	retObj["pingMin"] = 9999;
	retObj["jitter"] = 0;
	retObj["loss"] = 0;
	retObj["max"] = 0;
	retObj["min"] = 9999;
	retObj["isDoing"] = true;

	retObj["jitters"] = [];
	retObj["jitterSum"] = 0;
	retObj["jitterAvg"] = 0;

	return retObj;
}

var netoolApiDashBoard = {
	start: function(obj){
		if(!targetData[obj.target]){
			targetData[obj.target] = new initTargetData();
		}

		$.getJSON("/netool.cgi", obj)
			.done(function(data){
				if(data.successful != "0"){
					setTimeout(function(){
						netoolApiDashBoard.check(obj, data.successful)
					}, 2000)
				} 
			})
	},

	check: function(obj, fileName){
		$.getJSON("/netool.cgi", {"type":0,"target":fileName})
			.done(function(data){
				if(data.result.length == 0) return false;
				var thisTarget = targetData[obj.target];
				var pingVal = (data.result[0].ping !== "") ? parseFloat(data.result[0].ping) : 0;
				var jitterVal = (thisTarget.points.length === 0) ? 0 : Math.abs(pingVal - thisTarget.points[thisTarget.points.length-1]).toFixed(1);

				thisTarget.isDoing = (thisTarget.points.length > 110) ? false : thisTarget.isDoing;
				// ping status
				thisTarget.points.push(pingVal);
				thisTarget.pingMax = (thisTarget.pingMax > pingVal) ? thisTarget.pingMax : pingVal;
				thisTarget.pingMin = (thisTarget.pingMin < pingVal) ? thisTarget.pingMin : pingVal;
				thisTarget.sum += pingVal;
				thisTarget.avg = (thisTarget.sum/thisTarget.points.length).toFixed(1);
				thisTarget.jitter = Math.abs(thisTarget.pingMax - thisTarget.pingMin).toFixed(1);
				thisTarget.loss += (parseInt(data.result[0].loss) / 100);

				// jitter status
				thisTarget.jitters.push(jitterVal)
				thisTarget.jitterMax = (thisTarget.jitterMax > jitterVal) ? thisTarget.jitterMax : jitterVal;
				thisTarget.jitterMin = (thisTarget.jitterMin < jitterVal) ? thisTarget.jitterMin : jitterVal;
				thisTarget.jitterSum += parseFloat(jitterVal);
				thisTarget.jitterAvg = (thisTarget.jitterSum/thisTarget.jitters.length).toFixed(1);

				var gap = parseInt(thisTarget.jitter/4) + 2;
				thisTarget.min = parseInt(thisTarget.pingMin/gap)*gap;
				thisTarget.max = thisTarget.min + gap*4;

				if(thisTarget.isDoing){
					netoolApiDashBoard.render(obj.target);
					netoolApiDashBoard.start(obj);
				}
			})
			.fail(function(data){
				setTimeout(function(){
					netoolApiDashBoard.check(obj, fileName);
				}, 500);
			});
	},

	render: function(target){
		var thisTarget = targetData[target];

		// Ping Graph
		var toPosition = function(point){
			return (170-((point-thisTarget.min)/(thisTarget.max-thisTarget.min))*170);
		}

		$(".yAxis")
			.each(function(id){
				$(this).html(thisTarget.min + (thisTarget.max-thisTarget.min)*id/4 + " ms")
			})

		$("#ping_graph")
			.attr("points", function(){
				return thisTarget.points
					.map(function(el, id){return ((id*3) + "," + toPosition(el));})
					.join(" ");
			});

		$("#ping_avg_graph")
			.attr("points", "0," + toPosition(thisTarget.avg) + " 340," + toPosition(thisTarget.avg));

		$("#pingAvg").html(thisTarget.avg + " ms")

		// Jitter Graph
		var toJitterPosition = function(point){
			var graphHeight = (thisTarget.jitter == 0) ? "999" : thisTarget.jitter;
			return (170-(point/graphHeight)*170);
		}

		$(".yAxisJitter")
			.each(function(id){
				$(this).html(Math.abs(thisTarget.jitter*id/4).toFixed(1) + " ms")
			})

		$("#jitter_graph")
			.attr("points", function(){
				return thisTarget.jitters
					.map(function(el, id){return ((id*3) + "," + toJitterPosition(el));})
					.join(" ");
			});

		$("#jitter_avg_graph")
			.attr("points", "0," + toJitterPosition(thisTarget.jitterAvg) + " 340," + toJitterPosition(thisTarget.jitterAvg));

		$("#jitterAvg").html(thisTarget.jitterAvg + " ms")
	},

	reset: function(obj){
		netoolApiDashBoard.stopAll();
		targetData[obj.target] = new initTargetData();
		$("#ping_graph").attr("points", "0,170");
		$("#ping_avg_graph").attr("points", "0,170");
		$("#jitter_graph").attr("points", "0,170");
	},

	stopAll: function(){
		for(var i in targetData){
			targetData[i].isDoing = false;
		}
	}
}
function updateClientsCount() {
	originData.fromNetworkmapd[0] = httpApi.hookGet("get_clientlist", true);
	genClientList();
	$("#client_count").html(totalClientNum.online);
	setTimeout("updateClientsCount();", 5000);
}

function rgbToHex(c){
	c = parseInt(c);
	var hex = c.toString(16);
    return hex.length === 1 ? "0" + hex : hex;
}
function setColor(color){
	colorPicker.setColorByHex(color);
	$("#color_pad").css({"background": color});
	$("#color").val(color.toUpperCase());
}

function hexToR(h) {return parseInt((cutHex(h)).substring(0,2),16)}
function hexToG(h) {return parseInt((cutHex(h)).substring(2,4),16)}
function hexToB(h) {return parseInt((cutHex(h)).substring(4,6),16)}
function cutHex(h) {return (h.charAt(0)=="#") ? h.substring(1,7):h}

function inputColor(color){
	setColor(color);
	var r = hexToR(color)
	var g = hexToG(color)
	var b = hexToB(color)
	aura_settings[0] = r;
	aura_settings[1] = g;
	aura_settings[2] = b;
	submitAura();
}

function aura_enable(obj){
	if(obj.checked){
		httpApi.nvramSet({
			"aurargb_enable": "1",
		   	"action_mode": "apply",
		   	"rc_service": "start_aurargb"
		});
	}
	else{
		httpApi.nvramSet({
			"aurargb_enable": "0",
		   	"action_mode": "apply",
		   	"rc_service": "start_aurargb"
		});	
	}
}

function submitAura(){
	httpApi.nvramSet({
    	"aurargb_val": aura_settings.join(","),
    	"action_mode": "apply",
    	"rc_service": "start_aurargb"
	});
}
var auraEffectArray = [["_static", "aura-icon-static", "1", "0"], 
				  ["_breathing", "aura-icon-breathing", "2", "0"], 
				  ["_rainbow", "aura-icon-rainbow", "5", "0"],
				  ["_comet", "aura-icon-comet", "8", "0"],
				  ["_flash", "aura-icon-flash", "3", "0"],
				  ["_yoyo", "aura-icon-yoyo", "12", "0"],
				  ["_ccycle", "aura-icon-ccycle", "4", "2"],
				  ["_snight", "aura-icon-snight", "13", "0"],
				  ["_strobing", "aura-icon-strobing", "6", "0"],
				  ["_wave", "aura-icon-wave", "11", "0"],
				  ["_event", "aura-icon-event", "0", "0"]];
function changeRgbMode(obj){
	for(i=0;i<auraEffectArray.length;i++){
		var prefixID = auraEffectArray[i][0].split("_");
		$("#" + auraEffectArray[i][0]).removeClass(auraEffectArray[i][1] + "-active").addClass(auraEffectArray[i][1]);
	}

	var _id = obj.id;
	for(i=0;i<auraEffectArray.length;i++){
		if(auraEffectArray[i][0] == _id){
			var prefixID = auraEffectArray[i][0].split("_");
			$("#" + _id).removeClass(auraEffectArray[i][1]).addClass(auraEffectArray[i][1] + "-active");
			aura_settings[3] = auraEffectArray[i][2];
			aura_settings[4] = auraEffectArray[i][3];
		}
	}

	submitAura();
}

var _array = ["boost_led", "boost_dfs", "boost_aura", "boost_qos", "boost_shuffle", "boost_geforce"];
function handleBoostKey(obj){
	var _id = obj.id;
	var _nvram = httpApi.nvramGet(["turbo_mode"], true);
	var _rcService = 'saveNvram';
	
	if(_array[_nvram.turbo_mode] == _id){
		return false;
	}

	if(_id == 'boost_qos' && (policy_status.TM == "0" || policy_status.TM_time == "")){
		const policyModal = new PolicyModalComponent({
            policy: "TM"
        });
        policyModal.show();
		return false;
	}

	for(i=0;i<_array.length;i++){
		$("#" + _array[i]).removeClass("boost-key-checked")
	}

	$("#" + _id).addClass("boost-key-checked");

	if(obj.id == "boost_dfs"){
		_boost_key = "1";
	}
	else if(obj.id == "boost_aura"){
		_boost_key = "2";
	}
	else if(obj.id == "boost_qos"){
		_boost_key = "3";
		_rcService += ';restart_wrs;restart_firewall';
	}
	else if(obj.id == "boost_shuffle"){
		_boost_key = "4";
	}
	else if(obj.id == "boost_geforce"){
		_boost_key = "5";
	}
	else{		//default, LED (boost_led)
		_boost_key = "0";
	}

	boost_id = obj.id;
	httpApi.nvramSet({
    	"turbo_mode": _boost_key,
    	"action_mode": "apply",
    	"rc_service": _rcService
	});
}

var isMD5DDNSName = function(){
	var macAddr = '<% nvram_get("lan_hwaddr"); %>'.toUpperCase().replace(/:/g, "");
	return "A"+hexMD5(macAddr).toUpperCase()+".asuscomm.com";
}
function boostKeyDesc(id){
	if(id == ""){
		return true;
	}
	
	var desc = {
		"boost_shuffle": {
			content: "<#BoostKey_AURA_Shuffle_desc#>"
		},
		"boost_qos": {
			content: "<#BoostKey_Boost_desc#>"
		},
		"boost_dfs": {
			content: "<#BoostKey_DFS_desc#>"
		},
		"boost_geforce": {
			content: "<#BoostKey_GeForce_desc#>"
		}
	}

	if(desc[id] == undefined){
		return false;
	}

	$('#boostKey_desc').html(desc[id].content);
}
function tm_agree(){
	httpApi.nvramSet({
		"turbo_mode": '3',
		"TM_EULA": '1',
    	"action_mode": "apply",
    	"rc_service": "restart_wrs;restart_firewall"
	});

	$('#Loading').css('visibility', 'hidden');
	$('#alert_tm_EULA').hide();
	for(i=0;i<_array.length;i++){
		$("#" + _array[i]).removeClass("boost-key-checked")
	}

	$("#boost_qos").addClass("boost-key-checked");
}
function tm_disagree(){
	$('#Loading').css('visibility', 'hidden');
	$('#alert_tm_EULA').hide();
}

function showEventTriggerDesc(){
	$('#aura_event_trigger').removeClass('aura-event-desc-hide')
							.addClass('aura-event-desc-show');
}
function hideEventTriggerDesc(){
	$('#aura_event_trigger').removeClass('aura-event-desc-show')
							.addClass('aura-event-desc-hide');
}
function uuRegister(mac){
	var _mac = mac.toLowerCase();
	window.open('https://router.uu.163.com/asus/pc.html#/acce?gwSn=' + _mac + '&type=asuswrt', '_blank');
}

function showClientlistModal() {
	const clientlistModal = new ClientlistModel();
	clientlistModal.show();
}
</script>
</head>

<body onload="initial();" onunload="unload_body();" class="bg">
<div id="TopBanner"></div>
<div id="Loading" class="popup_bg" style="height:300%;"></div>
<iframe name="hidden_frame" id="hidden_frame" width="0" height="0" frameborder="0"></iframe>
<form method="post" name="form" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="GameDashBoard.asp">
<input type="hidden" name="next_page" value="GameDashBoard.asp">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_wait" value="4">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="restart_wrs;restart_firewall">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>" disabled>
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">
<input type="hidden" name="aurargb_val" value="<% nvram_get("aurargb_val"); %>">
<input type="hidden" name="TM_EULA" value="<% nvram_get("TM_EULA"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0" >

	<tr>
		<td width="17">&nbsp;</td>		
		<td valign="top" width="202">				
			<div  id="mainMenu"></div>	
			<div  id="subMenu"></div>		
		</td>					
		<td valign="top">
			<div id="tabMenu" class="submenuBlock"></div>	
		<!--===================================Beginning of Main Content===========================================-->		
			<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0" >
				<tr>
					<td valign="top" >		
						<table width="760px" border="0" cellpadding="4" cellspacing="0" class="FormTitle" id="FormTitle">
							<tbody>
							<tr>
								<td class="bg_rog" valign="top">
								<div>								
									<div style="width:100%;height:260px;position: relative;">
										<div class="banner"></div>
									</div>
									
									<div style="width:99%;height:200px;margin-top:20px;">
										<div style="display: inline-block;width:240px;vertical-align: top;text-align: center;">			
											<div style="margin: 0 0 0 30px;">
												<div class="rog-title"><#ROG_WIRELESS_STATE#></div>
												<div style="text-align: right;margin:15px 0 0 0;">
													<div id="wl0_icon" class="ic-signal" style="display:none"></div>
													<div id="wl1_icon" class="ic-signal" style="display:none"></div>
													<div id="wl2_icon" class="ic-signal" style="display:none"></div>
													<div id="wl3_icon" class="ic-signal" style="display:none"></div>
												</div>
											</div>
											<div style="margin: 15px 0 0 10px;text-align: center;">
												<div id="wan_ip_title" class="rog-title">WAN IP</div>
												<div id="wan_ip_field" style="font-size: 18px;margin-top:15px;color:#57BDBA;word-break: break-all"></div>
											</div>
										</div>
										<div style="display: inline-block;width:270px;">
											<div id="internet_title" class="rog-title" style="text-align: center;text-transform: uppercase;"><#statusTitle_Internet#></div>
											<div id="wan_state_icon" class="wan_state_icon"></div>
											<div id="wan_state" style="font-size: 18px;color:#57BDBA;text-transform: uppercase;text-align: center;margin: 0 0 0 15px;"></div>
										</div>	
										<div style="display: inline-block;width:180px;vertical-align: top;text-align: center;">
											
											<div>
												<div class="rog-title" style="text-transform: uppercase;"><#DSL_Mode#></div>
												<div id="sw_mode_desc" style="font-size: 18px;margin-top:20px;color:#57BDBA"></div>
											</div>
											<div style="margin-top: 40px;text-align:center;color:#BFBFBF;">
												<div class="rog-title" style="text-transform: uppercase;"><#Full_Clients#></div>
												<div style="cursor:pointer;margin-top:15px;font-family: Xolonium;" onclick="showClientlistModal()">
													<span id="client_count" style="font-size: 20px;padding:0 10px;color:#57BDBA"></span>
													<span style="font-size: 14px;color:#57BDBA;text-transform: uppercase;"><#Clientlist_Online#></span>
												</div>	
											</div>
										</div>
									</div>

									<div style="width:99%;height:300px;margin: 10px auto;">
										<div>
											<div class="rog-title" style="margin: 60px 20px 0 100px;"><#ROG_Network_Traffic#></div>
										</div>
										<div style="width:100%;">
											<div id="area_chart" style="width: 600px; height: 310px;margin: 0 auto;display:inline-block;"></div>
											<div style="display:inline-block;vertical-align: top;margin:130px 0 0 -50px;width:170px;">
												<div >
													<div>
														<div style="height:5px;padding:2px;background-color:#2A2523;border-radius:10px;width:100px;">
															<div id="download_traffic_bar" class="traffic_bar traffic_bar_download transition_style traffic_bar_boost"></div>
														</div>
													</div>
													<div style="font-size:18px;color:#BFBFBF;margin: 5px 0"><#option_download#></div>
													<div id="download_traffic" style="text-align: right;color:#32ADB2;font-size:22px;margin-bottom:5px;"></div>
												</div>
												<div >
													<div>
														<div style="height:5px;padding:2px;background-color:#2A2523;border-radius:10px;width:100px;">
															<div id="upload_traffic_bar" class="traffic_bar traffic_bar_upload transition_style traffic_bar_boost"></div>
														</div>
													</div>
													<div style="font-size:18px;color:#BFBFBF;margin: 5px 0"><#option_upload#></div>
													<div id="upload_traffic" style="text-align: right;color:#BCBD4D;font-size:22px;margin-bottom:5px;"></div>
												</div>
											</div>
										</div>
									</div>

									<div style="width:99%;height:300px;margin: 10px auto;">
										<div class="rog-title" style="margin:45px 120px;position:absolute;width:200px;"><#ROG_Network_PING#></div>
										<div id="svgPingContainer" style="margin:85px 0px 0px 20px;position:absolute;background-color:#221712;">
											<svg width="340px" height="170px">
												<g>
													<line stroke-width="1" stroke-opacity="1"   stroke="#221712" x1="0" y1="0%"   x2="100%" y2="0%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="25%"  x2="100%" y2="25%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="50%"  x2="100%" y2="50%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="75%"  x2="100%" y2="75%" />
													<line stroke-width="1" stroke-opacity="1"   stroke="#221712" x1="0" y1="100%" x2="100%" y2="100%" />
												</g>							
												<g>
													<text class="yAxis" font-family="Verdana" fill="#999" font-size="8" x="0" y="98%">0 ms</text>
													<text class="yAxis" font-family="Verdana" fill="#999" font-size="8" x="0" y="78%">25 ms</text>
													<text class="yAxis" font-family="Verdana" fill="#999" font-size="8" x="0" y="55%">50 ms</text>
													<text class="yAxis" font-family="Verdana" fill="#999" font-size="8" x="0" y="28%">75 ms</text>
													<text class="yAxis" font-family="Verdana" fill="#999" font-size="8" x="0" y="5%">100 ms</text>
												</g>							

												<polyline id="ping_avg_graph" style="fill:none;stroke:#BCCCDC;stroke-width:1;" points="0,250"></polyline>
												<polyline id="ping_graph" style="fill:none;stroke:#57BDBA;stroke-width:2;z-index:9999" points="0,250"></polyline>
											</svg>
										</div>
										<div style="text-align: center;font-family: Xolonium;font-size:16px;margin:255px 40px;position:absolute;width:300px;color:#BFBFBF"><#Average_value#> : <span id="pingAvg" style="font-size:26px">0 ms</span></div>

										<div class="rog-title" style="margin:45px 500px;position:absolute;width:200px;"><#ROG_PING_DEVIATION#></div>
										<div id="svgJitterContainer" style="margin:85px 0px 0px 400px;position:absolute;background-color:#221712;"> 
											<svg width="340px" height="170px">
												<g>
													<line stroke-width="1" stroke-opacity="1"   stroke="#221712" x1="0" y1="0%"   x2="100%" y2="0%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="25%"  x2="100%" y2="25%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="50%"  x2="100%" y2="50%" />
													<line stroke-width="1" stroke-opacity="0.2" stroke="rgb(255,255,255)" x1="0" y1="75%"  x2="100%" y2="75%" />
													<line stroke-width="1" stroke-opacity="1"   stroke="#221712" x1="0" y1="100%" x2="100%" y2="100%" />
												</g>							
												<g>
													<text class="yAxisJitter" font-family="Verdana" fill="#999" font-size="8" x="0" y="98%">0 ms</text>
													<text class="yAxisJitter" font-family="Verdana" fill="#999" font-size="8" x="0" y="78%">0 ms</text>
													<text class="yAxisJitter" font-family="Verdana" fill="#999" font-size="8" x="0" y="55%">0 ms</text>
													<text class="yAxisJitter" font-family="Verdana" fill="#999" font-size="8" x="0" y="28%">0 ms</text>
													<text class="yAxisJitter" font-family="Verdana" fill="#999" font-size="8" x="0" y="5%">0 ms</text>
												</g>							

												<polyline id="jitter_avg_graph" style="fill:none;stroke:#BCCCDC;stroke-width:1;" points="0,250"></polyline>
												<polyline id="jitter_graph" style="fill:none;stroke:#BCBD4D;stroke-width:2;z-index:9999" points="0,250"></polyline>
											</svg>
										</div>
										<div style="text-align: center;font-family: Xolonium;font-size:16px;margin:255px 420px;position:absolute;width:300px;color:#BFBFBF"><#Average_value#> : <span id="jitterAvg" style="font-size:26px">0 ms</span></div>

									</div>									
									</div>

									<div id="pingMap" style="width:380px;height:350px;margin:15px;display:none"></div>
									<script>
										$("#pingMap").load("/cards/pingMap.html");
									</script>
									<div id="aura_field" style="width:345px;height:425px;margin:-360px 0 0 390px;display:none;position: relative">
										<div id="light_effect_container" style="width:100%;height:100%;">
											<iframe id="light_effect_iframe" class="light_effect_iframe" frameborder="0"></iframe>
											<script>
												if(isSupport("ledg")){
													setTimeout(function(){
														$("#light_effect_iframe").attr("src", "/light_effect/light_effect.html");
														$("#light_effect_iframe").load(function(){
															let $ledg_html = $(this).contents();
															$ledg_html.find(".light_effect_bg").css("height", "160px");
															$ledg_html.find(".logo_container").css({"width":"18vw"});
															if(isSupport("antled")){
																$ledg_html.find(".switch_mode_list_bg")
																	.css({"width":"160px", "height":"35px", "background-size":"190px 35px", "background-position":"100%"});
																$ledg_html.find(".switch_mode_list_bg .mode_list_bg").css("width", "130px");
																$ledg_html.find(".logo_container")
																	.css({"width":"18vw", "background-position-y":"50%"});
															}
															const support_night_mode = (()=>{
																return ((based_modelid == "GT-BE98" || based_modelid == "GT-BE98_PRO" || based_modelid == "GT-BE96" || based_modelid == "GT-BE19000" || based_modelid == "GT-BE19000AI" || based_modelid == "GT-BE96_AI") ? true : false);
															})();
															if(support_night_mode){
																$ledg_html.find(".light_effect_mask").css({
																	"width":"102%",
																	"margin-left":"-1%",
																	"height":"102%"
																});
																$(this).css({
																	"transform":"scale(0.96)",
																	"width":"106%",
																	"height":"106%",
																	"transform-origin":"top left"
																});
															}
														});
													}, 1000);
												}
											</script>
										</div>
										<div id="aura_rgb_bg">
											<div id="aura_event_trigger" class="aura-event-container aura-event-desc-hide">
												<div>
													<div style="display: flex;justify-content: space-between">
														<div style="font-size: 16px;font-weight: bold;"><#AURA_Event#></div>
														<div class="event-cancel" onclick="hideEventTriggerDesc();"></div>
													</div>
													<div style="margin: 3px 0 6px 0;color:#BFBFBF;"><#AURA_Event_desc#></div>
												</div>
												<div style="margin-left:12px;">
													<div>
														<li style="font-size: 14px;font-weight: bold;"><#Game_Boost#></li>
														<div style="margin: 0 0 6px 0;color:#BFBFBF"><#AURA_Event_Boost_desc#></div>
													</div>
													<div>
														<li style="font-size: 14px;font-weight: bold;"><#AURA_Event_Traffic#></li>
														<div style="margin: 0 0 6px 0;color:#BFBFBF;"><#AURA_Event_Traffic_desc#></div>
													</div>
													<div>
														<li style="font-size: 14px;font-weight: bold;"><#AURA_Event_Login#></li>
														<div style="margin: 0 0 6px 0;color:#BFBFBF;"><#AURA_Event_Login_desc#></div>
													</div>

													<div>
														<li style="font-size: 14px;font-weight: bold;"><#AURA_Event_Attack#></li>
														<div style="margin: 0 0 6px 0;color:#BFBFBF;"><#AURA_Event_Attack_desc#></div>
													</div>
													<div style="margin: 12px 0 0 -12px;"><#AURA_Event_Note#></div>
												</div>
											</div>

											<div style="display:flex;align-items: center;justify-content: space-around;">
												<div class="rog-title" style="height:65px;"><#BoostKey_Aura_RGB#></div>
												<div style="width: 68px;height:68px;margin-top:10px;background: url('./images/New_ui/img-aurasync-logo.png')"></div>
											</div>
											<div style="display:flex;margin-top:-20px;">
												<div style="position: relative;">
													<canvas id="picker"></canvas><br>
												</div>

												<div style="margin:30px 0 0 30px;width: 101px;height: 160px;">
													<div id="color_pad" style="width: 115px;height:115px;margin: 5px auto 0 auto;border-radius: 50%;position: relative;">
														 <div style="width: 120px;height:120px;background:url('images/New_ui/img-aurasync-rog-logo-mask.png') no-repeat;background-size:100%;position: absolute;top:-1px;left:-1px;"></div>
													</div>

													<input id="color" value="" style="width:90px;height: 30px;margin:10px 0 0 10px;border: 1px solid rgb(145,7,31);background: rgb(62,3,13);color: #FFF;font-size:16px;padding: 0 5px;" onchange="inputColor(this.value);">
												</div>
												<script>
													var colorPicker = new KellyColorPicker({
														place : 'picker',
														size : 200,
														method : 'quad',
														input: 'color',
														input_color : false,
														userEvents : {mouseupsv: function(event, handler, dot){
															$("#color_pad").css({"background": handler.getCurColorHex()});
															$("#color").val(handler.getCurColorHex().toUpperCase());
															var _rgb = handler.getCurColorRgb();
															aura_settings[0] = _rgb.r.toString();
															aura_settings[1] = _rgb.g.toString();
															aura_settings[2] = _rgb.b.toString();
															submitAura();
														},
														mouseuph: function(event, handler, dot){
															$("#color_pad").css({"background": handler.getCurColorHex()});
															$("#color").val(handler.getCurColorHex().toUpperCase());
															var _rgb = handler.getCurColorRgb();
															aura_settings[0] = _rgb.r.toString();
															aura_settings[1] = _rgb.g.toString();
															aura_settings[2] = _rgb.b.toString();
															submitAura();
														}}
													});
												</script>
											</div>

											<div style="display:flex;align-items:center;margin: 12px;">
												<div style="font-size:14px;font-family: Xolonium;"><#ROG_LED_Enable#></div>
												<div class="switch-button-container" >
													<label for="aura_switch" style="cursor:pointer;">
														<input type="checkbox" id="aura_switch" class="switch-button" style="display:none;" onchange="aura_enable(this);">
														<div class="switch-button-bg"></div>
														<div class="switch-button-circle"></div>
													</label>
												</div>
											</div>

											<div class="aura-scheme-container">
												<div id="aura_event" class="aura-scheme" style="display:none">
													<div id="_event" class="aura-icon aura-icon-event" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#AURA_Event#></div>
													<div style="width:16px;height:16px;background: url('images/New_ui/helpicon.png');margin: 5px auto 0 auto;cursor:pointer;" onclick="showEventTriggerDesc();"></div>
												</div>
												<div class="aura-scheme">
													<div id="_static" class="aura-icon aura-icon-static" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_static#></div>
												</div>
												<div class="aura-scheme">
													<div id="_breathing" class="aura-icon aura-icon-breathing" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_breathing#></div>
												</div>
												<div class="aura-scheme">
													<div id="_rainbow" class="aura-icon aura-icon-rainbow" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_rainbow#></div>
												</div>
												<div class="aura-scheme">
													<div id="_comet" class="aura-icon aura-icon-comet" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_comet#></div>
												</div>
												<div class="aura-scheme">
													<div id="_flash" class="aura-icon aura-icon-flash" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_flash#></div>
												</div>
												<div class="aura-scheme">
													<div id="_yoyo" class="aura-icon aura-icon-yoyo" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_glowing#></div>
												</div>
												<div class="aura-scheme">
													<div id="_ccycle" class="aura-icon aura-icon-ccycle" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_cycle#></div>
												</div>
												<div class="aura-scheme">
													<div id="_snight" class="aura-icon aura-icon-snight" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_star#></div>
												</div>
												<div class="aura-scheme">
													<div id="_strobing" class="aura-icon aura-icon-strobing" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_strobing#></div>
												</div>
												<div class="aura-scheme">
													<div id="_wave" class="aura-icon aura-icon-wave" onclick="changeRgbMode(this);"></div>
													<div class="aura-desc"><#BoostKey_AURA_state_wave#></div>
												</div>
											</div>
										</div>
									</div>
										<div id="uu_field" style="width:345px;height:425px;margin:-428px 0 0 390px;position: relative;display:none;">
										<div style="font-size: 26px;color:#BFBFBF;margin-left:12px;">网易UU加速器</div>
										<div style="margin: 24px 0 36px 18px;">
											<img src="/images/uu_accelerator.png" alt="">
										</div>
										<div style="font-size:16px;margin: 0 6px;">UU路由器插件为三大主机PS4、Switch、Xbox One提供加速。可实现多台主机同时加速，NAT类型All Open。畅享全球联机超快感！</div>
										<div style="margin:6px;">
											<a href="https://uu.163.com/router/" target="_blank" style="color:#4A90E2;text-decoration: underline">FAQ</a>
										</div>
										<div class="content-action-container" onclick="uuRegister(label_mac);" style="margin-top:36px;">
											<div class="button-container button-container-sm" style="margin: 0 auto;">
												<div class="button-icon icon-go"></div>
												<div class="button-text"><#btn_go#></div>
											</div>
										</div>													
									</div>
									
									<div id="boostKey_field" style="width:720px;height:340px;margin: 33px 0 20px 15px;display:none">
										<div style="display:flex;align-items: center;justify-content: space-around;margin-bottom:40px;">
											<div>
												<div class="rog-title" style="margin: 0px 10px 40px 10px;"><#BoostKey#></div>			
												<div style="width:240px;height: 150px;background: url('./images/New_ui/Img-subProd-base.png') no-repeat;background-size: 100%;"></div>
											</div>
											<div style="width: 350px;height:170px;">
												<div style="font-size: 16px;font-family: Roboto;margin-bottom: 24px;"><#BoostKey_desc#></div>
												<div id="boostKey_desc" style="font-size: 16px;font-family: Roboto;color: #BFBFBF;"></div>
											</div>	
										</div>
										<div style="display:flex;width:720px;height: 76px;margin: 20px 0 10px 30px;">
											<div style="width:30px;height:76px;background: rgb(145,7,31);transform: skew(-30deg);"></div>
											<div id="boost_led" class="boost-function boost-border-odd" onclick="handleBoostKey(this)">
												<div class="boost-text"><#BoostKey_LED#></div>
											</div>
											<div id="boost_shuffle" style="display:none;" class="boost-function boost-border-even" onclick="handleBoostKey(this)">
												<div class="boost-text"><#BoostKey_AURA_Shuffle#></div>
											</div>
											<div id="boost_dfs" style="display:none;" class="boost-function boost-border-even" onclick="handleBoostKey(this)">
												<div class="boost-text"><#BoostKey_DFS#></div>
											</div>
											<div id="boost_aura" class="boost-function boost-border-odd" onclick="handleBoostKey(this)">
												<div class="boost-text"><#BoostKey_Aura_RGB#></div>
											</div>
											<div id="boost_qos" style="display:none;" class="boost-function boost-border-even" onclick="handleBoostKey(this)">
												<div class="boost-text"><#Game_Boost#></div>
											</div>
											<div id="boost_geforce" style="display:none;" class="boost-function boost-border-even" onclick="handleBoostKey(this)">
												<div class="boost-text"><#BoostKey_GeForce#></div>
											</div>
										</div>
									</div>
								</div>
								</td>
							</tr>
							</tbody>	
						</table>
					</td>         
				</tr>
			</table>				
		<!--===================================Ending of Main Content===========================================-->		
		</td>		
		<td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>
<div id="footer"></div>
</form>
</body>
</html>
