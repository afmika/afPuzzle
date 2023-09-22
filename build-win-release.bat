make config=release
xcopy .\ressources\* .\bin\Release\afPuzzle\ressources\* /c /i /e /h /y
xcopy .\src\shaders\* .\bin\Release\afPuzzle\shaders\* /c /i /e /h /y
xcopy .\vendor\sfml\bin\* .\bin\Release\afPuzzle\* /c /i /e /h /y
.\bin\Release\afPuzzle\afPuzzle.exe