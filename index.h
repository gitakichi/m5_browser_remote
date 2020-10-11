//改行は\(バックスラッシュ)﻿
//"(ダブルクォーテーション)は"(バックスラッシュ+ダブルクォーテーション)

const char index_str[] = R"=====(
<!DOCTYPE html>
<html lang="ja">
<html>
<head>
	<meta charset="utf-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width,initial-scale=1">
	<title>m5_browser_remote</title>
	<style>
		.button2 {
		    width: 90%;
		    height: 90%;
		    font-size: 1.4em;
		}
		.table1 {
			text-align: left;
			}
		.table2 {
			    border-collapse:  collapse; /* セルの線を重ねる */
			    width: 200px;
			    height: 200px;
			    table-layout: fixed;        /* セルの幅計算指定 */
			}
		@media screen and (max-width:480px) {
			.button2 {
			    width: 90%;
			    height: 90%;
			    font-size: 1.4em;
			}
			.table1 {
				text-align: left;
			}
			.table2 {
			    border-collapse:  collapse; /* セルの線を重ねる */
			    width:  100%;               /* 幅指定 */
			    height: 100vw;
			    table-layout: fixed;        /* セルの幅計算指定 */
			}
		}
	</style>
</head>
	
<body>
<h3>m5_browser_remote</h3>
<div id="ws_status_msg"></div>

<table class="table1">
<tr>
  <th>Speed</th>
  <th>
	<!--<select id="speed" onChange="speed_change()">
	<option value="1">1</option>
	<option value="2" selected>2</option>
	<option value="3">3</option>
	<option value="4">4</option>
	</select>-->
	
	<input type="range" id="speed" min="0" max="15" value="7" onChange="speed_change()"> 
	
  </th>
</tr>
<tr>
  <th>Touchscreen</th>
  <th><input type="checkbox" id="check" onchange="change()"></th>
</tr>
  </table>
  
  
<table class="table2">
    <tr>
      <th><input type="button" class = "button2" value="Q" onmousedown="cmmd_Q();" onmouseup="cmmd_N(false);"/></th>
      <th><input type="button" class = "button2" value="W" onmousedown="cmmd_W();" onmouseup="cmmd_N(false);"/></th>
      <th><input type="button" class = "button2" value="E" onmousedown="cmmd_E();" onmouseup="cmmd_N(false);"/></th>
    </tr>
    <tr>
      <th></th>
      <th><input type="button" class = "button2" value="N" onmouseup="cmmd_N(true);"/></th>
      <th></th>
    </tr>
    <tr>
      <th><input type="button" class = "button2" value="A" onmousedown="cmmd_A();" onmouseup="cmmd_N(false);"/></th>
      <th><input type="button" class = "button2" value="S" onmousedown="cmmd_S();" onmouseup="cmmd_N(false);"/></th>
      <th><input type="button" class = "button2" value="D" onmousedown="cmmd_D();" onmouseup="cmmd_N(false);"/></th>
    </tr>
  </table>
</body>

<script>
	var touchscreen = false;
	
	if(document.documentElement.clientWidth <= 480){
		var checkbox = document.getElementById("check");
		checkbox.checked = true;
		
		change();
	}
	
	var speed = Number(document.getElementById("speed").value).toString(16);
	
	var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

	ws.onclose = function(evt){
	  console.log("Websocket closed!");
	  ws.close();
	}
	
	function parent_wssend(message){
    if(ws.readyState == 1){
      ws.send(message);
    }
    else{
      var ws_status_msg = document.getElementById("ws_status_msg");
      ws_status_msg.innerHTML = "サーバとの通信が切断されました。再読み込みしてください。";
    }
  }
	
	function speed_change(){
	    speed = Number(document.getElementById("speed").value).toString(16);
	    //console.log(speed);
	}
	
	function change(){
	    touchscreen = document.getElementById('check').checked;
	    //console.log("changed");
	}

	function cmmd_N(force){
	  if(touchscreen == false || force == true){
	    parent_wssend("n"+","+speed);
	  }
	}
  
	function cmmd_Q(){
	  //console.log("q"+","+speed);
	  parent_wssend("q"+","+speed);
	}
	
	function cmmd_W(){
	  parent_wssend("w"+","+speed);
	}
	
	function cmmd_E(){
	  parent_wssend("e"+","+speed);
	}
	
	function cmmd_A(){
	  parent_wssend("a"+","+speed);
	}
	
	function cmmd_S(){
	 	parent_wssend("s"+","+speed);
	}
	
	function cmmd_D(){
	  parent_wssend("d"+","+speed);
	}
	
	var key_det = 0;
	
	document.addEventListener('keydown', event => {
	    var keyName = event.key;
	    
	    if(key_det === 0){
	  		key_det = 1;
		    if (keyName === 'q')		    cmmd_Q();
		    else if (keyName === 'w')	  cmmd_W();
		    else if (keyName === 'e') 	cmmd_E();
		    else if (keyName === 'a') 	cmmd_A();
		    else if (keyName === 's') 	cmmd_S();
		    else if (keyName === 'd') 	cmmd_D();
		    else if (keyName === 'n')   cmmd_N(true);
		  }
    });
    
    document.addEventListener('keyup', event => {
	    var keyName = event.key;
	    
	    key_det = 0;
	    if (event.key === 'q')		  cmmd_N(false);
	    else if (keyName === 'w')	  cmmd_N(false);
	    else if (keyName === 'e') 	cmmd_N(false);
	    else if (keyName === 'a') 	cmmd_N(false);
	    else if (keyName === 's') 	cmmd_N(false);
	    else if (keyName === 'd') 	cmmd_N(false);
    }); 
</script>
</html>
)=====";
