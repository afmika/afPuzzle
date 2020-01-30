# no need to include sfml files on linux
g++ -Wall -O2 -Iclasses -c classes/*.h
g++ -Wall -O2 -Iclasses -c classes/*.cpp
g++ -Wall -O2 -Iclasses -c *.cpp

g++  -o ./bin/puzzle *.o -s -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

./bin/puzzle
