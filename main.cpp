#include "Headers/Shader.h"
// #include <glm/ext/matrix_clip_space.hpp>
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
// #include <unistd.h> // used for usleep()

const int WINDOW_HEIGHT = 600, WINDOW_WIDTH = 600;
bool pmovement = false; // movement on a previous iteration, not yet implemented
class Sprite // used for the Pacman and the ghosts(hopefully)
{
    public:
        float size;
        float speed;
        bool movement = false;
        glm::vec4 color; // Color of the Pacman     
        float pos[18]; // used for the structure of the sprite for opengl(in the form of triangles)
        unsigned int VBO;
        unsigned int VAO;
        glm::mat4 view = glm::mat4(1.0f); // view matrix for manipulating the movement
        glm::mat4 model = glm::mat4(1.0f); // model matrix for opengl
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f); // projection matrix for the perspective
};
Sprite Pacman;
class wall
{
    public:
        float width[128];
        float height[128];
        float pos[18];
        glm::vec4 color; // Color of the walls
        float coords[256];
        unsigned int VBO;
        unsigned int VAO;
        glm::mat4 view = glm::mat4(1.0f); // view matrix
        glm::mat4 model = glm::mat4(1.0f); // model matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);
};
wall Wall;

// Button class(WIP):
struct Button {
    float height;
    float width;
    bool isPressed;
};
int timeout = 0; // used for a timeout for collision, not yet implemented

void LogMovement(float x, float y);
void processInput(GLFWwindow *window);
int main()
{
    // Filling the classes with information:
        // Pacman:
    Pacman.size = 0.05f;
    Pacman.speed = 0.015f;
    Pacman.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    Pacman.pos[0]  =  Pacman.size; Pacman.pos[1]  =  Pacman.size; Pacman.pos[2]  = 0.0f;
    Pacman.pos[3]  =  Pacman.size; Pacman.pos[4]  = -Pacman.size; Pacman.pos[5]  = 0.0f;
    Pacman.pos[6]  = -Pacman.size; Pacman.pos[7]  =  Pacman.size; Pacman.pos[8]  = 0.0f;
    Pacman.pos[9]  = -Pacman.size; Pacman.pos[10] =  Pacman.size; Pacman.pos[11] = 0.0f;
    Pacman.pos[12] = -Pacman.size; Pacman.pos[13] = -Pacman.size; Pacman.pos[14] = 0.0f;
    Pacman.pos[15] =  Pacman.size; Pacman.pos[16] = -Pacman.size; Pacman.pos[17] = 0.0f;
        // Walls:
    Wall.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    // initializing glfw:
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Menu screen:
    /*
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
    */

    // Configuring the main game:
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
    Shader PacmanShader("Shaders/Pacman.vs", "Shaders/Pacman.fs");
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
    
    // Wall config:
    Shader WallShader("Shaders/Wall.vs", "Shaders/Wall.fs");    
    glGenVertexArrays(1, &Wall.VAO);
    glGenBuffers(1, &Wall.VBO);
    glBindVertexArray(Wall.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Wall.VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // mathematics for the wall
    Wall.model = glm::translate(Wall.model, glm::vec3(0.0f, 0.0f, -2.0f));

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        Pacman.movement = true;
        processInput(window);
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
        level.open("Maps/Square.lvl");
        if(level)
        {       
            int i = 0, j = 0;
            for(std::string line; std::getline(level, line);)   //read stream line by line
            {
                std::istringstream in(line);
                std::string type;
                in >> Wall.coords[i] >> Wall.coords[i+1] >> Wall.width[j] >> Wall.height[j];
                Wall.pos[0]  =  Wall.width[j]; Wall.pos[1]  =  Wall.height[j]; Wall.pos[2]  = 0.0f;
                Wall.pos[3]  =  Wall.width[j]; Wall.pos[4]  = -Wall.height[j]; Wall.pos[5]  = 0.0f;
                Wall.pos[6]  = -Wall.width[j]; Wall.pos[7]  =  Wall.height[j]; Wall.pos[8]  = 0.0f;
                Wall.pos[9]  = -Wall.width[j]; Wall.pos[10] =  Wall.height[j]; Wall.pos[11] = 0.0f;
                Wall.pos[12] = -Wall.width[j]; Wall.pos[13] = -Wall.height[j]; Wall.pos[14] = 0.0f;
                Wall.pos[15] =  Wall.width[j]; Wall.pos[16] = -Wall.height[j]; Wall.pos[17] = 0.0f;
                glBufferData(GL_ARRAY_BUFFER, sizeof(Wall.pos), Wall.pos, GL_STATIC_DRAW);
                Wall.model = glm::translate(Wall.model, glm::vec3(Wall.coords[i], Wall.coords[i+1], -2.0f));
                WallShader.use();
                glUniformMatrix4fv(WallModelLoc, 1, GL_FALSE, glm::value_ptr(Wall.model));
                glUniformMatrix4fv(WallViewLoc, 1, GL_FALSE, glm::value_ptr(Wall.view));
                glUniformMatrix4fv(WallProjectionLoc, 1, GL_FALSE, glm::value_ptr(Wall.projection));
                glUniform4fv(WallColorLoc, 1, glm::value_ptr(Wall.color));
                glDrawArrays(GL_TRIANGLES, 0, 6);
                Wall.model = glm::mat4(1.0f);
                i+= 2;
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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(Wall.coords)/sizeof(float), j = 0; i < n; i+= 2, j++)
        {
            collision = RCollisionDetection(Pacman.view[3][0], Pacman.view[3][1], Pacman.size, Pacman.size, Wall.coords[i], Wall.coords[i+1], Wall.width[j], Wall.height[j]);
            if(collision != 0)
            {
                Pacman.movement = false;
                Pacman.view = translate(Pacman.view, glm::vec3(-Pacman.speed, 0.0f, 0.0f));
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
        // PacmanMovement(0, 1, 'u');
        for(int i = 0, n = sizeof(Wall.coords)/sizeof(float), j = 0; i < n; i+=2, j++)
        {
            collision = TCollisionDetection(Pacman.view[3][0], Pacman.view[3][1], Pacman.size, Pacman.size, Wall.coords[i], Wall.coords[i+1], Wall.width[j], Wall.height[j]);
            if(collision != 0)
            {
                Pacman.view = translate(Pacman.view, glm::vec3(0.0f, -Pacman.speed, 0.0f));
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
        for(int i = 0, n = sizeof(Wall.coords)/sizeof(float), j = 0; i < n; i+=2, j++)
        {
            collision = LCollisionDetection(Pacman.view[3][0], Pacman.view[3][1], Pacman.size, Pacman.size, Wall.coords[i], Wall.coords[i+1], Wall.width[j], Wall.height[j]);
            if(collision != 0)
            {
                Pacman.view = glm::translate(Pacman.view, glm::vec3(Pacman.speed, 0.0f, 0.0f));
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
        for(int i = 0, n = sizeof(Wall.coords)/sizeof(float), j = 0; i < n; i+=2, j++)
        {
            collision = BCollisionDetection(Pacman.view[3][0], Pacman.view[3][1], Pacman.size, Pacman.size, Wall.coords[i], Wall.coords[i+1], Wall.width[j], Wall.height[j]);
            if(collision != 0)
            {
                Pacman.view = glm::translate(Pacman.view, glm::vec3(0.0f, Pacman.speed, 0.0f));
                Pacman.movement = false;
                Pacman.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
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