//改行は\(バックスラッシュ)﻿
//"(ダブルクォーテーション)は"(バックスラッシュ+ダブルクォーテーション)

const char index_str[] = R"=====(
<!DOCTYPE html>
<html lang="ja">
<html>
<head>
	<meta charset="utf-8">
	<title>RC </title>
</head>
<body>
<table>
    <tr>
      <th><input type="button" value="↖" onmousedown="fwd_left();" onmouseup="mouseup();"/></th>
      <th><input type="button" value="↑" onmousedown="fwd();" onmouseup="mouseup();"/></th>
      <th><input type="button" value="↗" onmousedown="fwd_right();" onmouseup="mouseup();"/></th>
    </tr>
    <tr>
      <th><!--<input type="button" value="←" onmousedown="mousedown();" onmouseup="mouseup();"/>--></th>
      <th>★<!--none--></th>
      <th><!--<input type="button" value="→" onmousedown="mousedown();" onmouseup="mouseup();"/>--></th>
    </tr>
    <tr>
      <th><input type="button" value="↙" onmousedown="rv_left();" onmouseup="mouseup();"/></th>
      <th><input type="button" value="↓" onmousedown="rv();" onmouseup="mouseup();"/></th>
      <th><input type="button" value="↘" onmousedown="rv_right();" onmouseup="mouseup();"/></th>
    </tr>
  </table>
<div id="area1">
	ここでマウスボタンを押してください。
</div>
</body>
<script>
var ws = new WebSocket('ws://' + window.location.hostname + ':81/');
ws.onopen = function (evt){
  console.log(/"Websocket open!/");
}
ws.onclose = function(evt){\
  console.log(/"Websocket closed!/");
  ws.close();
}
function mousedown(){
  document.getElementById("area1").innerText = "マウスボタンが押し下げられています。";
}
function mouseup(){
  document.getElementById("area1").innerText = "ここにマウスボタンが離されました。";
  ws.send("ws_polling");
}


function fwd_left(){
  document.getElementById("area1").innerText = "calling fwd_left";
  ws.send("ws_polling");
}
function fwd(){
  document.getElementById("area1").innerText = "calling fwd";
  ws.send("ws_polling");
}
function fwd_right(){
  document.getElementById("area1").innerText = "calling fwd_right";
  ws.send("ws_polling");
}


function rv_left(){
  document.getElementById("area1").innerText = "calling rv_left";
  ws.send("ws_polling");
}
function rv(){
  document.getElementById("area1").innerText = "calling rv";
  ws.send("ws_polling");
}
function rv_right(){
  document.getElementById("area1").innerText = "calling rv_right";
  ws.send("ws_polling");
}
</script>
</html>
)=====";
