//改行は\(バックスラッシュ)﻿
//"(ダブルクォーテーション)は"(バックスラッシュ+ダブルクォーテーション)

const char index_str[] = R"=====(
<!DOCTYPE html>
<html lang="ja">
<html>
	
<head>
	<meta charset="utf-8">
	<title>m5_browser_remote</title>
</head>
	
<body>
<h3>m5_browser_remote</h3>
<table>
    <tr>
      <th><input type="button" value="Q" onmousedown="cmmd_Q();" onmouseup="cmmd_N();"/></th>
      <th><input type="button" value="W" onmousedown="cmmd_W();" onmouseup="cmmd_N();"/></th>
      <th><input type="button" value="E" onmousedown="cmmd_E();" onmouseup="cmmd_N();"/></th>
    </tr>
    <tr>
      <th></th>
      <th><input type="button" value="N" onmouseup="cmmd_N();"/></th>
      <th></th>
    </tr>
    <tr>
      <th><input type="button" value="A" onmousedown="cmmd_A();" onmouseup="cmmd_N();"/></th>
      <th><input type="button" value="S" onmousedown="cmmd_S();" onmouseup="cmmd_N();"/></th>
      <th><input type="button" value="D" onmousedown="cmmd_D();" onmouseup="cmmd_N();"/></th>
    </tr>
  </table>
</body>

<script>
	var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

	ws.onclose = function(evt){
	  console.log("Websocket closed!");
	  ws.close();
	}

	function cmmd_N(){
	  ws.send("n");
	}
	function cmmd_Q(){
	  ws.send("q");
	}
	function cmmd_W(){
	  ws.send("w");
	}
	function cmmd_E(){
	  ws.send("e");
	}
	function cmmd_A(){
	  ws.send("a");
	}
	function cmmd_S(){
	  ws.send("s");
	}
	function cmmd_D(){
	  ws.send("d");
	}
	
	var key_det = 0;
	
	document.addEventListener('keydown', event => {
	    var keyName = event.key;
	    
	    if(key_det === 0){
	  		key_det = 1;
		    if (keyName === 'q')		cmmd_Q();
		    else if (keyName === 'w')	cmmd_W();
		    else if (keyName === 'e') 	cmmd_E();
		    else if (keyName === 'a') 	cmmd_A();
		    else if (keyName === 's') 	cmmd_S();
		    else if (keyName === 'd') 	cmmd_D();
		    else if (keyName === 'n')   cmmd_N();
		}
    });
    
    document.addEventListener('keyup', event => {
	    var keyName = event.key;
	    
	    key_det = 0;
	    if (event.key === 'q')		cmmd_N();
	    else if (keyName === 'w')	cmmd_N();
	    else if (keyName === 'e') 	cmmd_N();
	    else if (keyName === 'a') 	cmmd_N();
	    else if (keyName === 's') 	cmmd_N();
	    else if (keyName === 'd') 	cmmd_N();
    }); 
</script>
</html>
)=====";
