
EX_OBJ = obj/evolution.o
OBJS = obj/Field.o obj/Dude.o obj/Reader.o obj/fastRandom.o obj/stream_utils.o obj/direction.o \
obj/NeuralNetwork.o obj/InputNode.o obj/HiddenNode.o obj/OutputNode.o obj/NeuralNode.o obj/NeuralLink.o \
obj/mutation.o obj/hashMap.o

LIBS = -lcrypto

CC = g++ -Wall -g -O3 
INCFLG = -Iinc $(LIBS)

run : bin/evolution.exe
	./bin/evolution.exe

clean : 
	rm $(OBJS) $(EX_OBJ)
	rm bin/evolution.exe
	
only : bin/evolution.exe

reset : bin/evolution.exe
	./bin/evolution.exe reset
	
#####################################################################
# executable files
	
bin/evolution.exe : $(OBJS) $(EX_OBJ)
	$(CC) -o bin/evolution.exe $(OBJS) $(EX_OBJ) $(INCFLG)
	
#####################################################################
# executable objects files

obj/evolution.o : src/evolution.cpp
	$(CC) -c -o obj/evolution.o src/evolution.cpp $(INCFLG)
	
#####################################################################
# object files

obj/Field.o : src/Field.cpp
	$(CC) -c -o obj/Field.o src/Field.cpp $(INCFLG)
	
obj/Dude.o : src/Dude.cpp
	$(CC) -c -o obj/Dude.o src/Dude.cpp $(INCFLG)
	
obj/Reader.o : src/Reader.cpp
	$(CC) -c -o obj/Reader.o src/Reader.cpp $(INCFLG)
	
obj/NeuralNetwork.o : src/NeuralNetwork.cpp
	$(CC) -c -o obj/NeuralNetwork.o src/NeuralNetwork.cpp $(INCFLG)
	
obj/NeuralNode.o : src/NeuralNode.cpp
	$(CC) -c -o obj/NeuralNode.o src/NeuralNode.cpp $(INCFLG)

obj/InputNode.o : src/InputNode.cpp
	$(CC) -c -o obj/InputNode.o src/InputNode.cpp $(INCFLG)

obj/HiddenNode.o : src/HiddenNode.cpp
	$(CC) -c -o obj/HiddenNode.o src/HiddenNode.cpp $(INCFLG)

obj/OutputNode.o : src/OutputNode.cpp
	$(CC) -c -o obj/OutputNode.o src/OutputNode.cpp $(INCFLG)
	
obj/NeuralLink.o : src/NeuralLink.cpp
	$(CC) -c -o obj/NeuralLink.o src/NeuralLink.cpp $(INCFLG)
	
obj/fastRandom.o : src/fastRandom.c
	$(CC) -c -o obj/fastRandom.o src/fastRandom.c $(INCFLG)
	
obj/stream_utils.o : src/stream_utils.c
	$(CC) -c -o obj/stream_utils.o src/stream_utils.c $(INCFLG)
	
obj/direction.o : src/direction.c
	$(CC) -c -o obj/direction.o src/direction.c $(INCFLG)

obj/mutation.o : src/mutation.c
	$(CC) -c -o obj/mutation.o src/mutation.c $(INCFLG)
	
obj/hashMap.o : src/hashMap.c
	$(CC) -c -o obj/hashMap.o src/hashMap.c $(INCFLG)
