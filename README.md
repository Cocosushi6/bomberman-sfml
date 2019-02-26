# bomberman
Multiplayer over network port of Bomberman.

Currently working, but no config file (will come soon)

Uses only SFML modules (networking, rendering, etc., no sound though)

To compile, you can download the whole repo using `git clone`, and then use the following command : 
`g++ -std=c++14 -g -O0 render/*.cpp game/*.cpp utils/*.cpp net/*.cpp main.cpp -lsfml-system -lsfml-graphics -lsfml-window -lsfml-network -pedantic`

Note: you must have the SFML libs already installed, and if possible the latest version

Here's a screenshot of the latest working version : 

![screenshot of game](https://raw.githubusercontent.com/Cocosushi6/bomberman-sfml/master/screenshot.png)
