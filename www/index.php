<!HTML>
<html>
<head>
	<title>Evolution</title>
	<style>
	table {
		margin:0;
		padding:0;
		
		border-collapse:collapse;
		border:1px solid grey;
	}
	td {
		background-color: rgba(0, 255, 0, 0.5);
		border:0px solid grey;
		width:3px;
		height:3px;
		margin:0;
		padding:0;
	}
	.dude {background-color: rgba(255, 0, 255, 1);}
	div {
		margin:10px;
		font-family: "Courier New", Courier, monospace;
		float:left;
	}
	<?php
	for($i = 0; $i < 256; $i++) {
		echo ".veg".$i." {background-color: rgba(0, 255, 0, " . ($i / 256) . ");}";
	}
	?>
	</style>
</head>
<body>
	<h1>Evolution</h1>
	<div id="field"></div>
	<div id="info">
		<p id="num_dudes"></p>
		<p id="genomes"></p>
	</div>
</body>
<script>
var data = [<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$field_file = "../field/one.field";
$data = file_get_contents($field_file);
$data = str_split($data);

$datalen = count($data);
for( $i = 0; $i < $datalen; $i++ ) {
	echo "" . ord($data[$i]) . ",";
}
?>];
window.onload = function() {
	var boxes = "";
	var i, j;
	
	var width = data[3] *16777216 + data[2] * 65536 + data[1] * 256 + data[0];
	var height = data[7] *16777216 + data[6] * 65536 + data[5] * 256 + data[4];
	var historic_count = data[11] *16777216 + data[10] * 65536 + data[9] * 256 + data[8];
	var day = data[15] *16777216 + data[14] * 65536 + data[13] * 256 + data[12];
	
	var start_of_dudes = 16 + width*height;
	var num_dudes = data[start_of_dudes+3] *16777216 + data[start_of_dudes+2] * 65536 + data[start_of_dudes+1] * 256 + data[start_of_dudes+0];
	start_of_dudes += 4;
	
	var dudes = [];
	for(i = 0; i < width; i++) {
		dudes[i] = [];
		for(j = 0; j < height; j++) {
			dudes[i][j] = false;
		}
	}
	
	var genomes = {};
	
	console.log("Width: " + width);
	console.log("Height: " + height);
	console.log("historic_count: " + historic_count);
	console.log("num_dudes: " + num_dudes);
	
	var index = start_of_dudes;
	
	for(i = 0; i < num_dudes; i++) {
		
		var dude_size = 25+52;
		
		var len = 
		data[index + 3] * 16777216 + 
		data[index + 2] * 65536 + 
		data[index + 1] * 256 + 
		data[index + 0];
		var genome_name = "";
		for(j = 4; j < 56; j++) {
			genome_name += String.fromCharCode(data[index + j]);
		}
		genome_name = genome_name.replace( /([a-z\/.])/g, '' );
		if(genome_name in genomes) {
			genomes[genome_name]++;
		} else {
			genomes[genome_name] = 1;
		}
		var x = 
		data[index + j + 3] * 16777216 + 
		data[index + j + 2] * 65536 + 
		data[index + j + 1] * 256 + 
		data[index + j + 0]; 
		var y = 
		data[index + j + 7] * 16777216 + 
		data[index + j + 6] * 65536 + 
		data[index + j + 5] * 256 + 
		data[index + j + 4];
		
		console.log(i + " " + x + "," + y);
		
		dudes[x][y] = true;
		
		index += len + 4;
	}
	
	var index = 16;
	
	boxes += "<table>";
	
	for(i = 0; i < width; i++) {
		boxes += "<tr class='row'"+j+"' >";
		for(j = 0; j < height; j++) {
			var dude_class = "";
			if(dudes[i][j]) {
				dude_class = "dude";
			} else {
				dude_class = "veg"+data[index];
			}
			boxes += "<td class='column"+i + " " + dude_class + "' >";
			index += 1;
		}
		boxes += "</tr>";
	}
	boxes += "</table>";
	
	document.getElementById("field").innerHTML = boxes;
	
	
	document.getElementById("num_dudes").innerHTML = "" + num_dudes + " Dudes";
	
	for(genome in genomes) {
		
		var aTag = document.createElement('a');
		aTag.setAttribute('href',"dude.php?g=" + genome);
		aTag.innerHTML = genome;
		document.getElementById("genomes").appendChild(aTag);
		
		var spanTag = document.createElement('span');
		spanTag.innerHTML = " " + genomes[genome];
		document.getElementById("genomes").appendChild(spanTag);
		
		var brTag = document.createElement('br');
		document.getElementById("genomes").appendChild(brTag);
		
	}
}
</script>
</html>
