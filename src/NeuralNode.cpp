#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "NeuralNode.h"
#include "InputNode.h"
#include "HiddenNode.h"
#include "OutputNode.h"
#include "Dude.h"

NeuralNode * create_node(node_type type, Dude * dude) {
	switch(type) {
		case INPUT_NODE:
			return new InputNode(dude);
		case HIDDEN_NODE :
			return new HiddenNode();
		case OUTPUT_NODE:
			return new OutputNode();
		default:
			return 0;
		
	}
}
