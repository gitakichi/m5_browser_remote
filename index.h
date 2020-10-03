const char index_str[] = "<!DOCTYPE html>\
<html lang=\"ja\">\
<html>\
<head>\
	<meta charset=\"utf-8\">\
	<title>RC </title>\
</head>\
<body>\
<table>\
    <tr>\
      <th><input type=\"button\" value=\"↖\" onmousedown=\"fwd_left();\" onmouseup=\"mouseup();\"/></th>\
      <th><input type=\"button\" value=\"↑\" onmousedown=\"fwd();\" onmouseup=\"mouseup();\"/></th>\
      <th><input type=\"button\" value=\"↗\" onmousedown=\"fwd_right();\" onmouseup=\"mouseup();\"/></th>\
    </tr>\
    <tr>\
      <th><!--<input type=\"button\" value=\"←\" onmousedown=\"mousedown();\" onmouseup=\"mouseup();\"/>--></th>\
      <th>★<!--none--></th>\
      <th><!--<input type=\"button\" value=\"→\" onmousedown=\"mousedown();\" onmouseup=\"mouseup();\"/>--></th>\
    </tr>\
    <tr>\
      <th><input type=\"button\" value=\"↙\" onmousedown=\"rv_left();\" onmouseup=\"mouseup();\"/></th>\
      <th><input type=\"button\" value=\"↓\" onmousedown=\"rv();\" onmouseup=\"mouseup();\"/></th>\
      <th><input type=\"button\" value=\"↘\" onmousedown=\"rv_right();\" onmouseup=\"mouseup();\"/></th>\
    </tr>\
  </table>\
<div id=\"area1\">\
	ここでマウスボタンを押してください。\
</div>\
</body>\
<script>\
function mousedown(){\
  document.getElementById(\"area1\").innerText = \"マウスボタンが押し下げられています。\";\
}\
function mouseup(){\
  document.getElementById(\"area1\").innerText = \"ここにマウスボタンが離されました。\";\
}\
function fwd_left(){\
  document.getElementById(\"area1\").innerText = \"calling fwd_left\";\
}\
function fwd(){\
  document.getElementById(\"area1\").innerText = \"calling fwd\";\
}\
function fwd_right(){\
  document.getElementById(\"area1\").innerText = \"calling fwd_right\";\
}\
function rv_left(){\
  document.getElementById(\"area1\").innerText = \"calling rv_left\";\
}\
function rv(){\
  document.getElementById(\"area1\").innerText = \"calling rv\";\
}\
function rv_right(){\
  document.getElementById(\"area1\").innerText = \"calling rv_right\";\
}\
</script>\
</html>";