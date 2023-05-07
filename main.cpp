#include "Headers/Shader.h"
#include <stdlib.h>
// #include <glm/ext/matrix_clip_space.hpp> // previously didn't work without them, now it does, idk why
// #include <glm/ext/matrix_transform.hpp> //required for matrix transformation, or at least I thought so, code works without them
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Headers/Collision.h"
#include "Headers/Classes.h"
// #include <unistd.h> // used for usleep()

int WINDOW_HEIGHT = 600, WINDOW_WIDTH = 600;
std::string map = "Maps/New.lvl";
int timeout = 0;
int ZYcount = 0;
// bool pmovement = false; // movement on a previous iteration, not yet implemented
bool NewMap = false;
sprite Pacman;
wall Wall;
wall MapWall;
button OriginalMapButton;
button TestButton;
button TempWall;
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);
void LogMovement(float x, float y);
void processGameInput(GLFWwindow *window);
void processMenuInput(GLFWwindow *window);
void processMakerInput(GLFWwindow *window);
int main()
{
    // Filling the classes with information:
        // Pacman:
    Pacman.size = 0.05f;
    Pacman.speed = 0.015f;
    Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    Pacman.findpos();
        // Walls:
    Wall.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    MapWall.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    TempWall.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        // Buttons:
    OriginalMapButton.width = 0.5f;
    OriginalMapButton.height = 0.1f;
    OriginalMapButton.coords[0] = 0.5f;
    OriginalMapButton.coords[1] = 0.0f;
    OriginalMapButton.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    OriginalMapButton.findpos();
    OriginalMapButton.isPressed = false;

    TestButton.width = 0.5f;
    TestButton.height = 0.1f;
    TestButton.coords[0] = -0.5f;
    TestButton.coords[1] = 0.0f;
    TestButton.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    TestButton.findpos();
    TestButton.isPressed = false;

    // initializing glfw:
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Menu screen:
    GLFWwindow* menu = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Menu screen", NULL, NULL);
    if(menu == NULL) // error checking with glfw
    {
        std::cout << "Error: GLFW window creation failed <type: menu>\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(menu);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error: GLAD initiation failed <window type: menu>\n";
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Buttons config:
    Shader OriginalMapButtonShader("Shaders/Buttons/OriginalMapBtn.vs", "Shaders/Buttons/OriginalMapBtn.fs");
    glGenVertexArrays(1, &OriginalMapButton.VAO);
    glGenBuffers(1, &OriginalMapButton.VBO);
    glBindVertexArray(OriginalMapButton.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, OriginalMapButton.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(OriginalMapButton.pos), OriginalMapButton.pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    OriginalMapButton.view = translate(OriginalMapButton.view, glm::vec3(OriginalMapButton.coords[0], OriginalMapButton.coords[1], -2.0f));
    OriginalMapButton.projection = projection;
    
    Shader TestMapButtonShader("Shaders/Buttons/TestMapBtn.vs", "Shaders/Buttons/TestMapBtn.fs");
    glGenVertexArrays(1, &TestButton.VAO);
    glGenBuffers(1, &TestButton.VBO);
    glBindVertexArray(TestButton.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, TestButton.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TestButton.pos), TestButton.pos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    TestButton.view = translate(TestButton.view, glm::vec3(TestButton.coords[0], TestButton.coords[1], -2.0f));
    TestButton.projection = projection;
    
    // Declaring uniform values:
    unsigned int ButtonModelLoc;
    unsigned int ButtonViewLoc;
    unsigned int ButtonColorLoc;
    unsigned int ButtonProjectionLoc;

    while(!glfwWindowShouldClose(menu))
    {
        processMenuInput(menu);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render the triangle
        OriginalMapButtonShader.use();
        ButtonModelLoc = glGetUniformLocation(OriginalMapButtonShader.ID, "Bmodel");
        ButtonViewLoc = glGetUniformLocation(OriginalMapButtonShader.ID, "Bview");
        ButtonColorLoc = glGetUniformLocation(OriginalMapButtonShader.ID, "BColor");
        ButtonProjectionLoc = glGetUniformLocation(OriginalMapButtonShader.ID, "Bprojection");
        glBindVertexArray(OriginalMapButton.VAO);
        // uniform values for the buttons:
        glUniformMatrix4fv(ButtonModelLoc, 1, GL_FALSE, glm::value_ptr(OriginalMapButton.model));
        glUniformMatrix4fv(ButtonViewLoc, 1, GL_FALSE, glm::value_ptr(OriginalMapButton.view));
        glUniform4fv(ButtonColorLoc, 1, glm::value_ptr(OriginalMapButton.color));
        glUniformMatrix4fv(ButtonProjectionLoc, 1, GL_FALSE, glm::value_ptr(OriginalMapButton.projection));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        TestMapButtonShader.use();
        ButtonModelLoc = glGetUniformLocation(TestMapButtonShader.ID, "Bmodel");
        ButtonViewLoc = glGetUniformLocation(TestMapButtonShader.ID, "Bview");
        ButtonColorLoc = glGetUniformLocation(TestMapButtonShader.ID, "BColor");
        ButtonProjectionLoc = glGetUniformLocation(TestMapButtonShader.ID, "Bprojection");
        glUniformMatrix4fv(ButtonModelLoc, 1, GL_FALSE, glm::value_ptr(TestButton.model));
        glUniformMatrix4fv(ButtonViewLoc, 1, GL_FALSE, glm::value_ptr(TestButton.view));
        glUniform4fv(ButtonColorLoc, 1, glm::value_ptr(TestButton.color));
        glUniformMatrix4fv(ButtonProjectionLoc, 1, GL_FALSE, glm::value_ptr(TestButton.projection));
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(menu);
        glfwPollEvents();
        if(OriginalMapButton.isPressed)
        {
            map = "Maps/Original.lvl";
            glfwDestroyWindow(menu);
            break;
        }
        if(TestButton.isPressed)
        {
            map = "Maps/TestPlayground.lvl";
            glfwDestroyWindow(menu);
            break;
        }
        if(NewMap)
        {
            glfwDestroyWindow(menu);
            break;
        }
    }
    if(NewMap)
    {
        WINDOW_WIDTH = 600;
        WINDOW_HEIGHT = 600;
        GLFWwindow* mapcreator = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Map creator", NULL, NULL);
        if(mapcreator == NULL)
        {
            std::cout << "Error: GLFW window creation failed <type: Map Creator>\n";
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(mapcreator);
        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Error: GLAD initiation failed <window type: Map Creator>\n";
            glfwTerminate();
            return -1;
        }
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        Shader MapWallShader("Shaders/MapWall/MapWall.vs", "Shaders/MapWall/MapWall.fs");
        glGenVertexArrays(1, &MapWall.VAO);
        glGenBuffers(1, &MapWall.VBO);
        glBindVertexArray(MapWall.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, MapWall.VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // mathematics for the wall:
        MapWall.projection = projection;
        // TempWall:
        Shader TempWallShader("Shaders/TempWall/TempWall.vs", "Shaders/TempWall/TempWall.fs");
        glGenVertexArrays(1, &TempWall.VAO);
        glGenBuffers(1, &TempWall.VBO);
        glBindVertexArray(TempWall.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, TempWall.VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        TempWall.projection = projection;
        // uniform values:
        unsigned int MapWallModelLoc = glGetUniformLocation(MapWallShader.ID, "Wmodel");
        unsigned int MapWallViewLoc = glGetUniformLocation(MapWallShader.ID, "Wview");
        unsigned int MapWallColorLoc = glGetUniformLocation(MapWallShader.ID, "WColor");
        unsigned int MapWallProjectionLoc = glGetUniformLocation(MapWallShader.ID, "Wprojection");

        unsigned int TempWallModelLoc = glGetUniformLocation(TempWallShader.ID, "Bmodel");
        unsigned int TempWallViewLoc = glGetUniformLocation(TempWallShader.ID, "Bview");
        unsigned int TempWallColorLoc = glGetUniformLocation(TempWallShader.ID, "BColor");
        unsigned int TempWallProjectionLoc = glGetUniformLocation(TempWallShader.ID, "Bprojection");

        while(!glfwWindowShouldClose(mapcreator))
        {
            processMakerInput(mapcreator);
            if(timeout > 0)
            {
                timeout--;
            }
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            // TempWall values:
            TempWall.findpos();
            TempWall.model = glm::translate(TempWall.model, glm::vec3(TempWall.coords[0], TempWall.coords[1], -2.0f));
            glBufferData(GL_ARRAY_BUFFER, sizeof(TempWall.pos), TempWall.pos, GL_DYNAMIC_DRAW);
            glUniformMatrix4fv(TempWallModelLoc, 1, GL_FALSE, glm::value_ptr(TempWall.model));
            glUniformMatrix4fv(TempWallViewLoc, 1, GL_FALSE, glm::value_ptr(TempWall.view));
            glUniform4fv(TempWallColorLoc, 1, glm::value_ptr(TempWall.color));
            glUniformMatrix4fv(TempWallProjectionLoc, 1, GL_FALSE, glm::value_ptr(TempWall.projection));
            TempWall.model = glm::mat4(1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            if(MapWall.index == 0) {}
            else
            {
                for(int i = 0, n = MapWall.index; i < n; i++)
                {
                    MapWall.pos[0]  =  MapWall.width[i]; MapWall.pos[1]  =  MapWall.height[i]; MapWall.pos[2]  = 0.0f;
                    MapWall.pos[3]  =  MapWall.width[i]; MapWall.pos[4]  = -MapWall.height[i]; MapWall.pos[5]  = 0.0f;
                    MapWall.pos[6]  = -MapWall.width[i]; MapWall.pos[7]  =  MapWall.height[i]; MapWall.pos[8]  = 0.0f;
                    MapWall.pos[9]  = -MapWall.width[i]; MapWall.pos[10] =  MapWall.height[i]; MapWall.pos[11] = 0.0f;
                    MapWall.pos[12] = -MapWall.width[i]; MapWall.pos[13] = -MapWall.height[i]; MapWall.pos[14] = 0.0f;
                    MapWall.pos[15] =  MapWall.width[i]; MapWall.pos[16] = -MapWall.height[i]; MapWall.pos[17] = 0.0f;
                    MapWall.model = glm::translate(MapWall.model, glm::vec3(MapWall.x[i], MapWall.y[i], -2.0f));
                    MapWallShader.use();
                    glBufferData(GL_ARRAY_BUFFER, sizeof(MapWall.pos), MapWall.pos, GL_STATIC_DRAW);
                    glUniformMatrix4fv(MapWallModelLoc, 1, GL_FALSE, glm::value_ptr(MapWall.model));
                    glUniformMatrix4fv(MapWallViewLoc, 1, GL_FALSE, glm::value_ptr(MapWall.view));
                    glUniform4fv(MapWallColorLoc, 1, glm::value_ptr(MapWall.color));
                    glUniformMatrix4fv(MapWallProjectionLoc, 1, GL_FALSE, glm::value_ptr(MapWall.projection));
                    MapWall.model = glm::mat4(1.0f);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
            if(glfwWindowShouldClose(mapcreator))
            {
                glfwDestroyWindow(mapcreator);
                break;
            }
            glfwSwapBuffers(mapcreator);
            glfwPollEvents();
        }
    }
    // Configuring the main game:
    WINDOW_HEIGHT = 600;
    WINDOW_WIDTH = 600;
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Error: GLFW window creation failed <type: main window>\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glad initiation:
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error: GLAD initiation failed <window type: main>\n";
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    Shader PacmanShader("Shaders/Pacman/Pacman.vs", "Shaders/Pacman/Pacman.fs");
    glGenVertexArrays(1, &Pacman.VAO);
    glGenBuffers(1, &Pacman.VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(Pacman.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Pacman.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pacman.pos), Pacman.pos, GL_DYNAMIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // mathematics:
    Pacman.view = translate(Pacman.view, glm::vec3(0.0f, 0.0f, -2.0f));
    Pacman.projection = projection; 
    
    // Wall config:
    Shader WallShader("Shaders/Wall/Wall.vs", "Shaders/Wall/Wall.fs");    
    glGenVertexArrays(1, &Wall.VAO);
    glGenBuffers(1, &Wall.VBO);
    glBindVertexArray(Wall.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Wall.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // mathematics for the wall
    Wall.model = glm::translate(Wall.model, glm::vec3(0.0f, 0.0f, -2.0f));
    Wall.projection = projection;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        Pacman.movement = true;
        processGameInput(window);
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render the triangle
        PacmanShader.use();
        glBindVertexArray(Pacman.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // uniform values for pacman
        unsigned int PacmanModelLoc = glGetUniformLocation(PacmanShader.ID, "Pmodel");
        glUniformMatrix4fv(PacmanModelLoc, 1, GL_FALSE, glm::value_ptr(Pacman.model));
        unsigned int PacmanviewLoc = glGetUniformLocation(PacmanShader.ID, "Pview");
        glUniformMatrix4fv(PacmanviewLoc, 1, GL_FALSE, glm::value_ptr(Pacman.view));
        unsigned int PacmanColorLoc = glGetUniformLocation(PacmanShader.ID, "InputColor");
        glUniform4fv(PacmanColorLoc, 1, glm::value_ptr(Pacman.color));
        unsigned int PacmanProjectionLoc = glGetUniformLocation(PacmanShader.ID, "Pprojection");
        glUniformMatrix4fv(PacmanProjectionLoc, 1, GL_FALSE, glm::value_ptr(Pacman.projection));
        // uniform values for the walls:
        unsigned int WallModelLoc = glGetUniformLocation(WallShader.ID, "Wmodel");
        unsigned int WallViewLoc = glGetUniformLocation(WallShader.ID, "Wview");
        unsigned int WallProjectionLoc = glGetUniformLocation(WallShader.ID, "Wprojection");
        unsigned int WallColorLoc = glGetUniformLocation(WallShader.ID, "WColor");
        glBindVertexArray(Wall.VAO);
        // loading data from the .lvl file:
        std::ifstream level;
        level.open(map.c_str());
        if(level)
        {       
            int j = 0;
            for(std::string line; std::getline(level, line);)   //read stream line by line
            {
                std::istringstream in(line);
                std::string type;
                in >> Wall.x[j] >> Wall.y[j] >> Wall.width[j] >> Wall.height[j];
                Wall.pos[0]  =  Wall.width[j]; Wall.pos[1]  =  Wall.height[j]; Wall.pos[2]  = 0.0f;
                Wall.pos[3]  =  Wall.width[j]; Wall.pos[4]  = -Wall.height[j]; Wall.pos[5]  = 0.0f;
                Wall.pos[6]  = -Wall.width[j]; Wall.pos[7]  =  Wall.height[j]; Wall.pos[8]  = 0.0f;
                Wall.pos[9]  = -Wall.width[j]; Wall.pos[10] =  Wall.height[j]; Wall.pos[11] = 0.0f;
                Wall.pos[12] = -Wall.width[j]; Wall.pos[13] = -Wall.height[j]; Wall.pos[14] = 0.0f;
                Wall.pos[15] =  Wall.width[j]; Wall.pos[16] = -Wall.height[j]; Wall.pos[17] = 0.0f;
                glBufferData(GL_ARRAY_BUFFER, sizeof(Wall.pos), Wall.pos, GL_STATIC_DRAW);
                Wall.model = glm::translate(Wall.model, glm::vec3(Wall.x[j], Wall.y[j], -2.0f));
                WallShader.use();
                glUniformMatrix4fv(WallModelLoc, 1, GL_FALSE, glm::value_ptr(Wall.model));
                glUniformMatrix4fv(WallViewLoc, 1, GL_FALSE, glm::value_ptr(Wall.view));
                glUniformMatrix4fv(WallProjectionLoc, 1, GL_FALSE, glm::value_ptr(Wall.projection));
                glUniform4fv(WallColorLoc, 1, glm::value_ptr(Wall.color));
                glDrawArrays(GL_TRIANGLES, 0, 6);
                Wall.model = glm::mat4(1.0f);
                j++;
            }
            level.close();
        }
        else
        {
            std::cout << "Failed to load level\n";
            level.close();
            return -1;
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &Pacman.VAO);
    glDeleteBuffers(1, &Pacman.VBO);
    glDeleteVertexArrays(1, &Wall.VAO);
    glDeleteVertexArrays(1, &Wall.VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
int collision;
void processGameInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(Wall.x)/sizeof(float); i < n; i++)
        {
            collision = RCollisionDetection(Pacman.view[3][0]+Pacman.speed, Pacman.view[3][1], Pacman.size, Pacman.size, Wall.x[i], Wall.y[i], Wall.width[i], Wall.height[i]);
            if(collision != 0)
            {
                // Pacman.view = translate(Pacman.view, glm::vec3(-Pacman.speed, 0.0f, 0.0f));
                Pacman.movement = false;
                Pacman.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(Pacman.movement)
        {
            if(Pacman.view[3][0] >= 0.9f)
            {
                Pacman.view[3][0] = -0.9f;
            }
            else
            {
                Pacman.view = translate(Pacman.view, glm::vec3(Pacman.speed, 0.0f, 0.0f));
            }
            Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(Pacman.view[3][0], Pacman.view[3][1]);
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(Wall.x)/sizeof(float); i < n; i++)
        {
            collision = TCollisionDetection(Pacman.view[3][0], Pacman.view[3][1]+Pacman.speed, Pacman.size, Pacman.size, Wall.x[i], Wall.y[i], Wall.width[i], Wall.height[i]);
            if(collision != 0)
            {
                // Pacman.view = translate(Pacman.view, glm::vec3(0.0f, -Pacman.speed, 0.0f));
                Pacman.movement = false;
                Pacman.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(Pacman.movement)
        {
            if(Pacman.view[3][1] >= 0.9f)
            {
                Pacman.view[3][1] = -0.9f;
            }
            else
            {
                Pacman.view = glm::translate(Pacman.view, glm::vec3(0.0f, Pacman.speed, 0.0f));
            }
            Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(Pacman.view[3][0], Pacman.view[3][1]);
        }  
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(Wall.x)/sizeof(float); i < n; i++)
        {
            collision = LCollisionDetection(Pacman.view[3][0]-Pacman.speed, Pacman.view[3][1], Pacman.size, Pacman.size, Wall.x[i], Wall.y[i], Wall.width[i], Wall.height[i]);
            if(collision != 0)
            {
                // Pacman.view = glm::translate(Pacman.view, glm::vec3(Pacman.speed, 0.0f, 0.0f));
                Pacman.movement = false;
                Pacman.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(Pacman.movement)
        {
            if(Pacman.view[3][0] <= -0.9f)
            {
                Pacman.view[3][0] = 0.9f;
            }
            else
            {
                Pacman.view = glm::translate(Pacman.view, glm::vec3(-Pacman.speed, 0.0f, 0.0f));
            }
            Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(Pacman.view[3][0], Pacman.view[3][1]);
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(Wall.x)/sizeof(float); i < n; i++)
        {
            collision = BCollisionDetection(Pacman.view[3][0], Pacman.view[3][1]-Pacman.speed, Pacman.size, Pacman.size, Wall.x[i], Wall.y[i], Wall.width[i], Wall.height[i]);
            if(collision != 0)
            {
                // Pacman.view = glm::translate(Pacman.view, glm::vec3(0.0f, Pacman.speed, 0.0f));
                Pacman.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                Pacman.movement = false;
            }
        }
        if(Pacman.movement)
        {
            if(Pacman.view[3][1] <= -0.9f)
            {
                Pacman.view[3][1] = 0.9f;
            }
            else
            {
                Pacman.view = glm::translate(Pacman.view, glm::vec3(0.0f, -Pacman.speed, 0.0f));
            }
            Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(Pacman.view[3][0], Pacman.view[3][1]);
        }
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Pacman.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
}
void LogMovement(float x, float y)
{
    system("clear");
    std::cout << "Position(x, y): " << x << ", " << y << "\n";
}
void processMenuInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if(glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        NewMap = true;
    }
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        OriginalMapButton.isPressed = MouseDetection((float)mx/((float)WINDOW_WIDTH/2)-1.0f, (float)my/((float)WINDOW_HEIGHT/2)-1.0f, OriginalMapButton.coords[0], OriginalMapButton.coords[1], OriginalMapButton.width, OriginalMapButton.height);
        TestButton.isPressed = MouseDetection((float)mx/((float)WINDOW_WIDTH/2)-1.0f, (float)my/((float)WINDOW_HEIGHT/2)-1.0f, TestButton.coords[0], TestButton.coords[1], TestButton.width, TestButton.height);
    }
}
void processMakerInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        if(timeout == 0)
        {
            if(ZYcount > 0)
            {
                MapWall.index --;
                timeout = 25;
                ZYcount ++;
            }
        }
        else if(timeout > 0)
        {
            timeout--;
        }
    }
    else if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        if(timeout == 0)
        {
            if(ZYcount > 0)
            {
                MapWall.index++;
                timeout = 25;
                ZYcount --;
            }
        }
        else if(timeout > 0)
        {
            timeout--;
        }
    }
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if(timeout == 0)
        {
            timeout = 25;
            std::ofstream output("Maps/New.lvl");
            if(output)
            {
                for(int i = 0; i < MapWall.index; i++)
                {
                    output << MapWall.x[i] << " " << MapWall.y[i] << " " << MapWall.width[i] << " " << MapWall.height[i] << "\n";
                }
                std::cout << "Map Saved successfully\n";
            }
            else
            {
                std::cout << "Failed to create file for creation\n";
                glfwTerminate();
                output.close();
            }
            output.close();
        }
        else if(timeout > 0)
        {
            timeout--;
        }
    }
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    LogMovement(mx, my);
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        if(!MapWall.MousePressed)
        {
            MapWall.MousePressed = true;
            MapWall.StartingX = mx;
            MapWall.StartingY = my;
        }
        else
        {
            TempWall.width = abs((((float)MapWall.StartingX-(float)mx)/2))/((float)WINDOW_WIDTH/2);
            TempWall.height = abs((((float)MapWall.StartingY-(float)my)/2))/((float)WINDOW_HEIGHT/2);
            
            TempWall.coords[0] = (((float)MapWall.StartingX+(float)mx)/2)/((float)WINDOW_WIDTH/2) - 1.0f;
            TempWall.coords[1] = -((((float)MapWall.StartingY+(float)my)/2)/((float)WINDOW_HEIGHT/2) - 1.0f);
        }
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        if(MapWall.MousePressed)
        {
            MapWall.MousePressed = false;
            
            MapWall.width[MapWall.index] = abs((((float)MapWall.StartingX-(float)mx)/2))/((float)WINDOW_WIDTH/2);
            MapWall.height[MapWall.index] = abs((((float)MapWall.StartingY-(float)my)/2))/((float)WINDOW_HEIGHT/2);
            
            MapWall.x[MapWall.index] = (((float)MapWall.StartingX+(float)mx)/2)/((float)WINDOW_WIDTH/2) - 1.0f;
            MapWall.y[MapWall.index] = -((((float)MapWall.StartingY+(float)my)/2)/((float)WINDOW_HEIGHT/2) - 1.0f);

            // MapWall.x[MapWall.index] = ((float)(abs(MapWall.StartingX-mx))/2 + (float)MapWall.StartingX)/((float)WINDOW_HEIGHT/2) -1.0f;
            // MapWall.y[MapWall.index] = ((float)(abs(MapWall.StartingY-my))/2 + (float)MapWall.StartingY)/((float)WINDOW_HEIGHT/2) -1.0f;
            
            // MapWall.x[MapWall.index] = abs(MapWall.width[MapWall.index]) + (float)mx/((float)WINDOW_WIDTH/2) - 1.0f;
            // MapWall.y[MapWall.index] = abs(MapWall.height[MapWall.index]) + (float)my/((float)WINDOW_HEIGHT/2) - 1.0f;

            ZYcount ++;
            MapWall.index ++;
        }
    }
}