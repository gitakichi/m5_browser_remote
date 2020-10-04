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
<label><input type="checkbox" id="check" onchange="change()">Touchscreen</label>
<div id="result1"></div>
<table>
    <tr>
      <th><input type="button" value="Q" onmouseover="cmmd_Q(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_Q(true);" onmouseup="cmmd_N(true);"/></th>
      <th><input type="button" value="W" onmouseover="cmmd_W(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_W(true);" onmouseup="cmmd_N(true);"/></th>
      <th><input type="button" value="E" onmouseover="cmmd_E(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_E(true);" onmouseup="cmmd_N(true);"/></th>
    </tr>
    <tr>
      <th></th>
      <th><input type="button" value="N" onmouseover="cmmd_N(false);" onmouseup="cmmd_N(true);"/></th>
      <th></th>
    </tr>
    <tr>
      <th><input type="button" value="A" onmouseover="cmmd_A(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_A(true);" onmouseup="cmmd_N(true);"/></th>
      <th><input type="button" value="S" onmouseover="cmmd_S(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_S(true);" onmouseup="cmmd_N(true);"/></th>
      <th><input type="button" value="D" onmouseover="cmmd_D(false);" onmouseout="cmmd_N(false);" onmousedown="cmmd_D(true);" onmouseup="cmmd_N(true);"/></th>
    </tr>
  </table>
</body>

<script>
	var ws = new WebSocket('ws://' + window.location.hostname + ':81/');

	ws.onclose = function(evt){
	  console.log("Websocket closed!");
	  ws.close();
	}
	
	var isChecked = false;
	
	function change(){
      isChecked = document.getElementById('check').checked;
	  //console.log(isChecked)
  	}

	function cmmd_N(click_en){
	  if(click_en || isChecked){
	  	ws.send("n");
	  }
	}
	
	function cmmd_Q(click_en){
	  if(click_en || isChecked){
	  	ws.send("q");
	  }
	}
	
	function cmmd_W(click_en){
	  if(click_en || isChecked){
	  	ws.send("w");
	  }
	}
	
	function cmmd_E(click_en){
	  if(click_en || isChecked){
	  	ws.send("e");
	  }
	}
	
	function cmmd_A(click_en){
		if(click_en || isChecked){
	  		ws.send("a");
		}
	}
	
	function cmmd_S(click_en){
	  if(click_en || isChecked){
	  	ws.send("s");
	  }
	}
	
	function cmmd_D(click_en){
	  if(click_en || isChecked){
	  	ws.send("d");
	  }
	}
	
	var key_det = 0;
	
	document.addEventListener('keydown', event => {
	    var keyName = event.key;
	    
	    if(key_det === 0){
	  		key_det = 1;
		    if (keyName === 'q')		cmmd_Q(true);
		    else if (keyName === 'w')	cmmd_W(true);
		    else if (keyName === 'e') 	cmmd_E(true);
		    else if (keyName === 'a') 	cmmd_A(true);
		    else if (keyName === 's') 	cmmd_S(true);
		    else if (keyName === 'd') 	cmmd_D(true);
		    else if (keyName === 'n')   cmmd_N(true);
		}
    });
    
    document.addEventListener('keyup', event => {
	    var keyName = event.key;
	    
	    key_det = 0;
	    if (event.key === 'q')		cmmd_N(true);
	    else if (keyName === 'w')	cmmd_N(true);
	    else if (keyName === 'e') 	cmmd_N(true);
	    else if (keyName === 'a') 	cmmd_N(true);
	    else if (keyName === 's') 	cmmd_N(true);
	    else if (keyName === 'd') 	cmmd_N(true);
    }); 
</script>
</html>
)=====";
