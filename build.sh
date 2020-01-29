# On defini les includes non propre au projet pour eviter les erreurs de compilation
# Wall signifier montrer les warnings
g++ -Wall -s -I include -I "./include" -c classes/*.h
g++ -Wall -s -I include -I "./include" -c classes/*.cpp
g++ -Wall -s -I include -I "./include" -c *.cpp

# On definie le resultat du build + les fichiers obj a lier + les librairies dans dans /lib
g++ -L "./lib" -o "./bin/puzzle" *.o -s -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
# Lance l'application
./bin/puzzle