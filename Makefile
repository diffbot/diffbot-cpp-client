all:		diffbot

CPP_FILES	+= $(wildcard src/*.cpp)
OBJ_FILES	+= $(addprefix lib/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS	+= -Iinclude 
LD_FLAGS	+= -lstdc++ -lcurl -ljsoncpp

diffbot:	$(OBJ_FILES)
		g++ $^ $(LD_FLAGS) -o $@

lib/%.o:	src/%.cpp include/*.h
		g++ $(CC_FLAGS) -c -o $@ $<

clean:
		rm -f diffbot lib/*

test:		diffbot
		echo "Running test"
		./diffbot http://www.diffbot.com/

pack:		clean
		cd .. && rm -f diffbot.zip 2>/dev/null && zip -r diffbot.zip diffbot

#CC_FLAGS	+= -MMD
#-include	$(OBJFILES:.o=.d)
