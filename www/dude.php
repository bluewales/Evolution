<!DOCTYPE html>
<html>
<head>
<title>Dude</title>
</head>
<body>
<h1>Dude <?php echo $_GET['g'];?></h1>
<p id="parent"></p>
<canvas id="canvas">
</body>
<script>
var data = [<?php

$genome = $_GET['g'];

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$field_file = "../dudes/$genome.dude";
$data = file_get_contents($field_file);
$data = str_split($data);

$datalen = count($data);
for( $i = 0; $i < $datalen; $i++ ) {
	echo "" . ord($data[$i]) . ",";
}
?>];

var nodes = [];
var links = [];


window.onload = function() {
	
	
	var genome_length = data[3] * 16777216 + data[2] * 65536 + data[1] * 256 + data[0];
	
	var parent_genome_name = "";
	for(var j = 4; j < 56; j++) {
		parent_genome_name += String.fromCharCode(data[j]);
		console.log(String.fromCharCode(data[j]));
	}
	parent_genome_name = parent_genome_name.replace( /(.dude)/g, '' );
	parent_genome_name = parent_genome_name.replace( /(dudes\/)/g, '' );
	
	
	
	if(parent_genome_name.indexOf(' ') > -1) {
		document.getElementById('parent').innerHTML = parent_genome_name;
	}else {
		document.getElementById('parent').innerHTML = "Parent: ";
		var aTag = document.createElement('a');
		aTag.setAttribute('href',"dude.php?g=" + parent_genome_name);
		aTag.innerHTML = parent_genome_name;
		document.getElementById("parent").appendChild(aTag);
	}
	
	

	var INTERNALLY_TRADED_ATTRIBUTES = 5;
	var attribute_stream_length = (INTERNALLY_TRADED_ATTRIBUTES * (INTERNALLY_TRADED_ATTRIBUTES-1)) / 2;

	var start_of_neural_network = 52 + 8 + attribute_stream_length + 4 * 6;

	var number_of_nodes = 	data[start_of_neural_network + 3] * 16777216 + 
							data[start_of_neural_network + 2] * 65536 + 
							data[start_of_neural_network + 1] * 256 + 
							data[start_of_neural_network + 0];
							
	console.log(genome_length);
	console.log(number_of_nodes + " nodes");
							
	var index = start_of_neural_network + 4;
	for(var i = 0; i < number_of_nodes; i++) {
		var type_of_node = 	data[index + 3] * 16777216 + 
							data[index + 2] * 65536 + 
							data[index + 1] * 256 + 
							data[index + 0];
		index+=4;
		if(type_of_node == 0) {
			// input node
			var historic_mark = data[index + 3] * 16777216 + 
								data[index + 2] * 65536 + 
								data[index + 1] * 256 + 
								data[index + 0];
			index+=4;
			var sense = data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
			index+=4;
			var facing =	data[index + 3] * 16777216 + 
							data[index + 2] * 65536 + 
							data[index + 1] * 256 + 
							data[index + 0];
			index+=4;

			console.log(index + " Input " + historic_mark + ":" + sense + ":" + facing);
			
			var node = {"type":type_of_node, "id":historic_mark, "sense":sense, "facing":facing};
			nodes.push(node);
			
		} else if(type_of_node == 1) {
			// hidden node
			var historic_mark = data[index + 3] * 16777216 + 
								data[index + 2] * 65536 + 
								data[index + 1] * 256 + 
								data[index + 0];
			index+=4;
			var start_value = 	data[index + 3] * 16777216 + 
								data[index + 2] * 65536 + 
								data[index + 1] * 256 + 
								data[index + 0];
			index+=8;
			var bias = 	data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
			index+=8;
			
			console.log(index + " Hidden " + historic_mark + ":" + start_value + ":" + bias);
			
			var node = {"type":type_of_node, "id":historic_mark, "start_value":start_value, "bias":bias};
			nodes.push(node);
		} else if(type_of_node == 2) {
			// output node
			var historic_mark = data[index + 3] * 16777216 + 
								data[index + 2] * 65536 + 
								data[index + 1] * 256 + 
								data[index + 0];
			index+=4;
			var output =	data[index + 3] * 16777216 + 
							data[index + 2] * 65536 + 
							data[index + 1] * 256 + 
							data[index + 0];
			index+=4;
			var start_value = 	data[index + 3] * 16777216 + 
								data[index + 2] * 65536 + 
								data[index + 1] * 256 + 
								data[index + 0];
			index+=8;
			var bias = 	data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
			index+=8;
			 
			console.log(index + " Output " + historic_mark + ":" + output + ":" + start_value + ":" + bias);
			
			var node = {"type":type_of_node, "id":historic_mark, "output":output, "start_value":start_value, "bias":bias};
			nodes.push(node);
		}
	}
	var number_of_links = 	data[index + 3] * 16777216 + 
							data[index + 2] * 65536 + 
							data[index + 1] * 256 + 
							data[index + 0];
	index+=4;
	for(var i = 0; i < number_of_links; i++) {
		var historic_mark =	data[index + 3] * 16777216 + 
							data[index + 2] * 65536 + 
							data[index + 1] * 256 + 
							data[index + 0];
		index+=4;
		var input_id =	data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
		index+=4;
		var output_id =	data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
		index+=4;
		var wieght =	data[index + 3] * 16777216 + 
						data[index + 2] * 65536 + 
						data[index + 1] * 256 + 
						data[index + 0];
		index+=8;
		
		console.log(index + " Link " + historic_mark + " " + input_id + "->" + output_id);
		
		var link = {"id":historic_mark, "input_id":input_id, "output_id":output_id, "wieght":wieght};
		links.push(link);
	}
	
	var rows = [];
	rows[0] = [];
	var all_mapped = false;
	
	nodes.forEach(function(node, index) {
		if(node['type'] == 2) {
			rows[0].push(node);
		}
		if(node['type'] == 1) {
			node['mapped'] = false;
			all_mapped = false;
		}
	});
	
	var current_row = 0;
	while(!all_mapped) {
		all_mapped = true;
		rows[current_row+1] = [];
		rows[current_row].forEach(function(node, index) {
			links.forEach(function(link, link_index) {
				if(link['output_id'] == node['id']) {
					nodes.forEach(function(new_node, new_node_index) {
						if(link['input_id'] == new_node['id'] && new_node['mapped'] == false && new_node['type'] == 1) {
							rows[current_row+1].push(new_node);
							new_node['mapped'] = true;
							all_mapped = false;
						}
					});
				}
			});
		});
		current_row += 1;
	}
	
	nodes.forEach(function(node, index) {
		if(node['type'] == 0) {
			rows[current_row].push(node);
		}
	});
	
	console.log(JSON.stringify(rows));
	
	var net_depth = rows.length;
	var net_width = 0;
	rows.forEach(function(row, index) {
		if(row.length > net_width) {
			net_width = row.length;
		}
	});
	
	console.log(net_width + ", " + net_depth);
	
	var grid_width = Math.max(net_width, net_depth);
	
	
	var canvas = document.getElementById('canvas');
	canvas.width = 500;
	canvas.height = 500;
	var context = canvas.getContext('2d');
	
	var grid_size = 500 / grid_width;
	var radius = grid_size * 0.3;
	
	var vertical_spacing = 500 / rows.length;
	
	rows.forEach(function(row, j) {
		var horizontal_spacing = 500 / row.length;
		
		row.forEach(function(node, i) {
			var centerX = i * horizontal_spacing + horizontal_spacing / 2;
			var centerY = j * vertical_spacing + vertical_spacing / 2;

			context.beginPath();
			context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
			context.fillStyle = (node['type']==2)?'green':(node['type']==1)?'blue':'yellow';
			context.fill();
			context.lineWidth = radius / 5;
			context.strokeStyle = (node['type']==2)?'#003300':(node['type']==1)?'#000033':'#333300';
			context.stroke();

			
			node['centerX'] = centerX;
			node['centerY'] = centerY;
		});
	});
	
	links.forEach(function(link, link_index) {
		var input_node;
		var output_node;
		nodes.forEach(function(node, node_index) {
			if(node['id'] == link['output_id']) {
				output_node = node;
			}
			if(node['id'] == link['input_id']) {
				input_node = node;
			}
		});
		
		console.log(link['input_id'] + " to " + link['output_id']);
		
		console.log("("+input_node['centerX']+","+input_node['centerY']+") to ("+output_node['centerX']+","+output_node['centerY']+")" );
		
		
		var line_start = {'x':input_node['centerX'],'y':input_node['centerY']};
		var line_stop = {'x':output_node['centerX'],'y':output_node['centerY']};
		var dx = line_stop['x'] - line_start['x'];
		var dy = line_stop['y'] - line_start['y'];
		
		if(dy < 0) {
			var raw_len = Math.sqrt(dx*dx + dy*dy);
			line_start['x'] = line_start['x'] + dx * radius/raw_len;
			line_start['y'] = line_start['y'] + dy * radius/raw_len;
			
			line_stop['x'] = line_stop['x'] - dx * radius/raw_len;
			line_stop['y'] = line_stop['y'] - dy * radius/raw_len;
			
		
			var arrow_angle = Math.PI / 8;

			var arrow_corner_1 = {'x':line_stop['x'],'y':line_stop['y']};
			var arrow_corner_2 = {'x':line_stop['x'],'y':line_stop['y']};
			arrow_corner_1['x'] -= (dx*Math.cos(arrow_angle) - dy*Math.sin(arrow_angle)) * radius/raw_len;
			arrow_corner_1['y'] -= (dx*Math.sin(arrow_angle) + dy*Math.cos(arrow_angle)) * radius/raw_len;
			arrow_corner_2['x'] -= (dx*Math.cos(0-arrow_angle) - dy*Math.sin(0-arrow_angle)) * radius/raw_len;
			arrow_corner_2['y'] -= (dx*Math.sin(0-arrow_angle) + dy*Math.cos(0-arrow_angle)) * radius/raw_len;
			
			context.strokeStyle = '#000033';
			
			context.beginPath();
			context.moveTo(line_start['x'],line_start['y']);
			context.lineTo(line_stop['x'],line_stop['y']);
			
			context.moveTo(arrow_corner_1['x'],arrow_corner_1['y']);
			context.lineTo(line_stop['x'],line_stop['y']);
			context.lineTo(arrow_corner_2['x'],arrow_corner_2['y']);
			context.stroke();
			
			console.log(JSON.stringify(arrow_corner_1) + " " + JSON.stringify(line_stop) + " " + JSON.stringify(arrow_corner_2));
		} else {
			// recursive arrow
			var raw_len = Math.sqrt(dx*dx + dy*dy);
			
			var control_point_1 = {'x':line_start['x']+dx/2,'y':line_start['y']+dy/2};
			var control_point_2 = {'x':line_start['x']+dx/2,'y':line_start['y']+dy/2};
			
			line_start['x'] = line_start['x'] + dx * radius/raw_len;
			line_start['y'] = line_start['y'] + dy * radius/raw_len;
			
			line_stop['x'] = line_stop['x'] - dx * radius/raw_len;
			line_stop['y'] = line_stop['y'] - dy * radius/raw_len;
			
			var flatness = 200;
			
			control_point_1['x'] += dy * (radius) / flatness;
			control_point_1['y'] -= dx * (radius) / flatness;
			control_point_2['x'] += dy * (radius) / flatness;
			control_point_2['y'] -= dx * (radius) / flatness;
			
			var arrow_angle = Math.PI / 8;

			var arrow_corner_1 = {'x':line_stop['x'],'y':line_stop['y']};
			var arrow_corner_2 = {'x':line_stop['x'],'y':line_stop['y']};
			arrow_corner_1['x'] -= (dx*Math.cos(arrow_angle) - dy*Math.sin(arrow_angle)) * radius/raw_len;
			arrow_corner_1['y'] -= (dx*Math.sin(arrow_angle) + dy*Math.cos(arrow_angle)) * radius/raw_len;
			arrow_corner_2['x'] -= (dx*Math.cos(0-arrow_angle) - dy*Math.sin(0-arrow_angle)) * radius/raw_len;
			arrow_corner_2['y'] -= (dx*Math.sin(0-arrow_angle) + dy*Math.cos(0-arrow_angle)) * radius/raw_len;
			
			context.strokeStyle = '#330000';
			
			context.beginPath();
			context.moveTo(line_start['x'],line_start['y']);
			//context.lineTo(line_stop['x'],line_stop['y']);
			context.bezierCurveTo(control_point_1['x'],control_point_1['y'],control_point_2['x'],control_point_2['y'],line_stop['x'],line_stop['y']);
			
			context.moveTo(arrow_corner_1['x'],arrow_corner_1['y']);
			context.lineTo(line_stop['x'],line_stop['y']);
			context.lineTo(arrow_corner_2['x'],arrow_corner_2['y']);
			context.stroke();
			
			console.log(JSON.stringify(arrow_corner_1) + " " + JSON.stringify(line_stop) + " " + JSON.stringify(arrow_corner_2));
		}
	});
}

</script>
</html>