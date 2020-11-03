# no need to include sfml files on linux
g++ -std=c++11 -Wall -O2 -Iclasses -c classes/*.h
g++ -std=c++11 -Wall -O2 -Iclasses -c classes/*.cpp
g++ -std=c++11 -Wall -O2 -Iclasses -c *.cpp

g++ -std=c++11  -o ./bin/puzzle *.o -s -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

./bin/puzzle