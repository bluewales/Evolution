
EX_OBJ = obj/evolution.o
OBJS = obj/Field.o obj/Dude.o obj/Reader.o obj/fastRandom.o obj/stream_utils.o obj/direction.o \
obj/NeuralNetwork.o obj/NeuralNode.o obj/InputNode.o obj/HiddenNode.o obj/OutputNode.o obj/NeuralLink.o \
obj/mutation.o obj/hashMap.o

LIBS = -lcrypto

CC = g++ -Wall -g -O3 
INCFLG = -Iinc $(LIBS)

run : bin/evolution
	./bin/evolution

clean : 
	rm $(OBJS) $(EX_OBJ)
	rm bin/evolution
	
only : bin/evolution

reset : bin/evolution
	./bin/evolution reset
	
#####################################################################
# executable files
	
bin/evolution : $(OBJS) $(EX_OBJ)
	$(CC) -o bin/evolution $(INCFLG) $(OBJS) $(EX_OBJ)
	
#####################################################################
# executable objects files

obj/evolution.o : src/evolution.cpp
	$(CC) -c -o obj/evolution.o $(INCFLG) src/evolution.cpp
	
#####################################################################
# object files

obj/Field.o : src/Field.cpp
	$(CC) -c -o obj/Field.o $(INCFLG) src/Field.cpp
	
obj/Dude.o : src/Dude.cpp
	$(CC) -c -o obj/Dude.o $(INCFLG) src/Dude.cpp
	
obj/Reader.o : src/Reader.cpp
	$(CC) -c -o obj/Reader.o $(INCFLG) src/Reader.cpp
	
obj/NeuralNetwork.o : src/NeuralNetwork.cpp
	$(CC) -c -o obj/NeuralNetwork.o $(INCFLG) src/NeuralNetwork.cpp
	
obj/NeuralNode.o : src/NeuralNode.cpp
	$(CC) -c -o obj/NeuralNode.o $(INCFLG) src/NeuralNode.cpp

obj/InputNode.o : src/InputNode.cpp
	$(CC) -c -o obj/InputNode.o $(INCFLG) src/InputNode.cpp

obj/HiddenNode.o : src/HiddenNode.cpp
	$(CC) -c -o obj/HiddenNode.o $(INCFLG) src/HiddenNode.cpp

obj/OutputNode.o : src/OutputNode.cpp
	$(CC) -c -o obj/OutputNode.o $(INCFLG) src/OutputNode.cpp
	
obj/NeuralLink.o : src/NeuralLink.cpp
	$(CC) -c -o obj/NeuralLink.o $(INCFLG) src/NeuralLink.cpp
	
obj/fastRandom.o : src/fastRandom.c
	$(CC) -c -o obj/fastRandom.o $(INCFLG) src/fastRandom.c
	
obj/stream_utils.o : src/stream_utils.c
	$(CC) -c -o obj/stream_utils.o $(INCFLG) src/stream_utils.c
	
obj/direction.o : src/direction.c
	$(CC) -c -o obj/direction.o $(INCFLG) src/direction.c

obj/mutation.o : src/mutation.c
	$(CC) -c -o obj/mutation.o $(INCFLG) src/mutation.c
	
obj/hashMap.o : src/hashMap.c
	$(CC) -c -o obj/hashMap.o $(INCFLG) src/hashMap.c
