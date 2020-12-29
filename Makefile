STD = -std=c++11
CLASSES = classes

# SFML variables
SFML_INCLUDES = include
SFML_LIB = lib
SFML_ARGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SET_RELEASE_VER =  -mwindows
DEST = bin
DEST_NAME = puzzle.exe

all: build_classes build_main run

# We define the includes which are not specific to the project to avoid compilation errors
# Wall means show warnings
build_classes:
	g++ $(SET_RELEASE_VER) $(STD) -Wall -s -I $(SFML_INCLUDES) -c ${CLASSES}/*.h
	g++ $(SET_RELEASE_VER) $(STD) -Wall -s -I $(SFML_INCLUDES) -c ${CLASSES}/*.cpp
	g++ $(SET_RELEASE_VER) $(STD) -Wall -s -I $(SFML_INCLUDES) -c *.cpp


# We define the result of the build + obj files to link + the libraries included in / lib
build_main:
	g++ -std=c++11 -L $(SFML_LIB) -o $(DEST)/$(DEST_NAME) *.o -s $(SFML_ARGS)

run:
	$(DEST)/$(DEST_NAME)

clean_win:
	echo Cleaning...
	del *.o
	del *.gch
	del $(CLASSES)\*.o
	del $(CLASSES)\*.gch	

clean_linux:
	echo Cleaning...
	rm *.o
	rm classes/*.h.gch