# PacmanOnline
This is a game which mimics an old arcade game Pacman, added with more maps and multiplayer.

The app is in its early stage of development, so it is incredibly raw - if it was a steak, it would still breathe and walk, but if you want to compile the project(for some god forsaken reason) here's the way to compile the project:
1. Dependencies

    Beside the default libraries this project depends on glm, glad, glfw, so if you have problems with compiling - check if they are installed correctly. It is also best to have something like g++ or gcc to compile the code.
2. Compiling the project

    Before runing the program you also need to get glad.c for your computer, here's the explanation of how to do it:
    >  Go to the GLAD [web service](http://glad.dav1d.de/), make sure the language is set to C++, and in the API section select an OpenGL version of at least 3.3 (which is what we'll be using; higher versions are fine as well). Also make sure the profile is set to Core and that the Generate a loader option is ticked. Ignore the extensions (for now) and click Generate to produce the resulting library files.

    > GLAD by now should have provided you a zip file containing two include folders, and a single glad.c file. Copy both include folders (glad and KHR) into your include(s) directoy (or add an extra item pointing to these folders), and add the glad.c file to your project. 
    
    You can now compile the glad.c file and the project using these commands:
    ```
    g++ glad.c -c
    g++ main.cpp -o main glad.o -ldl -lglfw
    ```
    
    And finally, to run the code, simply run:
    `./main`
    ### This code is meant for Linux and MacOS users, the mileage on Windows will vary
