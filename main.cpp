#include "Headers/Shader.h"
#include <stdlib.h>
// #include <glm/ext/matrix_clip_space.hpp> // previously didn't work without them, now it does, idk why
// #include <glm/ext/matrix_transform.hpp> //required for matrix transformation, or at least I thought so, code works without them
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Headers/Collision.h"
#include "Maps/TestPlayground.h"
// #include <unistd.h> // used for usleep()

const int WINDOW_HEIGHT = 600, WINDOW_WIDTH = 600;
bool movement = false;
bool pmovement = false; // movement on a previous iteration, not yet implemented
const float PacmanSize = 0.05f;
const float PacmanSpeed = 0.015f;
float PacmanPos[] = {        
    PacmanSize, PacmanSize, 0.0f,
    PacmanSize, -PacmanSize, 0.0f,
    -PacmanSize, PacmanSize, 0.0f,

    -PacmanSize, PacmanSize, 0.0f,
    -PacmanSize, -PacmanSize, 0.0f,
    PacmanSize, -PacmanSize, 0.0f
};
const float WallWidth = 0.05f;
const float WallHeight = 0.02f;
float HorizontalWallPos[] = {
    WallWidth, WallHeight, 0.0f,
    WallWidth, -WallHeight, 0.0f,
    -WallWidth, WallHeight, 0.0f,

    -WallWidth, WallHeight, 0.0f,
    -WallWidth, -WallHeight, 0.0f,
    WallWidth, -WallHeight, 0.0f
};
float VerticalWallPos[] = {
    WallHeight, WallWidth, 0.0f,
    WallHeight, -WallWidth, 0.0f,
    -WallHeight, WallWidth, 0.0f,

    -WallHeight, WallWidth, 0.0f,
    -WallHeight, -WallWidth, 0.0f,
    WallHeight, -WallWidth, 0.0f
};
float HorizontalSquare [(int) (2/WallWidth*2*2)]; // for 
float VerticalSquare [(int) (2/WallWidth*2*2)];
float HorizontalWallCoords[sizeof(HorizontalSquare)/sizeof(float)];
float VerticalWallCoords[sizeof(VerticalSquare)/sizeof(float)];
glm::vec4 InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Color of the Pacman
glm::mat4 PacmanView = glm::mat4(1.0f); // Pacman View matrix
int timeout = 0; // used for a timeout for collision, not yet implemented
void MakeSquare(float width, float height);
void LogMovement(float x, float y);
void processInput(GLFWwindow *window);
int main()
{
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

    // Configuring main game:
    MakeSquare(WallWidth, WallWidth);
    for(int i = 0, n = sizeof(HorizontalSquare)/sizeof(float); i < n; i++)
    {
        HorizontalWallCoords[i] = HorizontalSquare[i];
    }
    for(int i = 0, n = sizeof(VerticalSquare)/sizeof(float); i < n; i++)
    {
        VerticalWallCoords[i] = VerticalSquare[i];
    }

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
    Shader PacMan("Shaders/pacman.vs", "Shaders/pacman.fs");
    unsigned int PacmanVBO, PacmanVAO;
    glGenVertexArrays(1, &PacmanVAO);
    glGenBuffers(1, &PacmanVBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(PacmanVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PacmanVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PacmanPos), PacmanPos, GL_DYNAMIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // mathematics:
        // model matrix
    glm::mat4 PacmanModel = glm::mat4(1.0f);
        // view matrix
    PacmanView = translate(PacmanView, glm::vec3(0.0f, 0.0f, -2.0f));
        // projection matrix
    glm::mat4 PacmanProjection;
    PacmanProjection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);
    
    // Horizontal Wall config:
    Shader HorizontalWall("Shaders/HorizontalWall.vs", "Shaders/HorizontalWall.fs");    
    unsigned int HorizontalWallVBO, HorizontalWallVAO;
    glGenVertexArrays(1, &HorizontalWallVAO);
    glGenBuffers(1, &HorizontalWallVBO);
    glBindVertexArray(HorizontalWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, HorizontalWallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HorizontalWallPos), HorizontalWallPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // mathematics for the wall
    glm::mat4 HorizontalWallModel = glm::mat4(1.0f);
    HorizontalWallModel = glm::translate(HorizontalWallModel, glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 HorizontalWallView = glm::mat4(1.0f);
    glm::mat4 HorizontalWallProjection;
    HorizontalWallProjection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);

    // Vertical Wall config:
    Shader VerticalWall("Shaders/VerticalWall.vs", "Shaders/VerticalWall.fs");
    unsigned int VerticalWallVBO, VerticalWallVAO;
    glGenVertexArrays(1, &VerticalWallVAO);
    glGenBuffers(1, &VerticalWallVBO);
    glBindVertexArray(VerticalWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VerticalWallVAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticalWallPos), VerticalWallPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // mathematics for the vertical wall:
    glm::mat4 VerticalWallModel = glm::mat4(1.0f);
    VerticalWallModel = glm::translate(VerticalWallModel, glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 VerticalWallView = glm::mat4(1.0f);
    glm::mat4 VerticalWallProjection;
    VerticalWallProjection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        movement = true;
        processInput(window);
        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // render the triangle
        PacMan.use();
        glBindVertexArray(PacmanVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // uniform values for pacman
        // -------------------------
        unsigned int PacmanModelLoc = glGetUniformLocation(PacMan.ID, "Pmodel");
        glUniformMatrix4fv(PacmanModelLoc, 1, GL_FALSE, glm::value_ptr(PacmanModel));

        unsigned int PacmanViewLoc = glGetUniformLocation(PacMan.ID, "Pview");
        glUniformMatrix4fv(PacmanViewLoc, 1, GL_FALSE, glm::value_ptr(PacmanView));
        
        unsigned int PacmanColorLoc = glGetUniformLocation(PacMan.ID, "InputColor");
        glUniform4fv(PacmanColorLoc, 1, glm::value_ptr(InputColor));
        
        unsigned int PacmanProjectionLoc = glGetUniformLocation(PacMan.ID, "Pprojection");
        glUniformMatrix4fv(PacmanProjectionLoc, 1, GL_FALSE, glm::value_ptr(PacmanProjection));
        unsigned int HorizontalWallModelLoc = glGetUniformLocation(HorizontalWall.ID, "Wmodel");
        unsigned int HorizontalWallViewLoc = glGetUniformLocation(HorizontalWall.ID, "Wview");
        unsigned int HorizontalWallProjectionLoc = glGetUniformLocation(HorizontalWall.ID, "Wprojection");
        glBindVertexArray(HorizontalWallVAO);
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            HorizontalWallModel = glm::translate(HorizontalWallModel, glm::vec3(HorizontalWallCoords[i], HorizontalWallCoords[i+1], -2.0f));
            HorizontalWall.use();
            glUniformMatrix4fv(HorizontalWallModelLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallModel));
            glUniformMatrix4fv(HorizontalWallViewLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallView));
            glUniformMatrix4fv(HorizontalWallProjectionLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallProjection));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            HorizontalWallModel = glm::mat4(1.0f);
        }
        unsigned int VerticalWallModelLoc = glGetUniformLocation(VerticalWall.ID, "Wmodel");
        unsigned int VerticalWallViewLoc = glGetUniformLocation(VerticalWall.ID, "Wview");
        unsigned int VerticalWallProjectionLoc = glGetUniformLocation(VerticalWall.ID, "Wprojection");
        glBindVertexArray(VerticalWallVAO);
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {    
            VerticalWallModel = glm::translate(VerticalWallModel, glm::vec3(VerticalWallCoords[i], VerticalWallCoords[i+1], -2.0f));
            VerticalWall.use();
            glUniformMatrix4fv(VerticalWallModelLoc, 1, GL_FALSE, glm::value_ptr(VerticalWallModel));
            glUniformMatrix4fv(VerticalWallViewLoc, 1, GL_FALSE, glm::value_ptr(VerticalWallView));
            glUniformMatrix4fv(VerticalWallProjectionLoc, 1, GL_FALSE, glm::value_ptr(VerticalWallProjection));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            VerticalWallModel = glm::mat4(1.0f);
        }


        // uniform values for the wall
        // ---------------------------
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        // LogMovement(PacmanView[3][0], PacmanView[3][1]);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &PacmanVAO);
    glDeleteBuffers(1, &PacmanVBO);
    glDeleteVertexArrays(1, &HorizontalWallVAO);
    glDeleteVertexArrays(1, &HorizontalWallVBO);
    glDeleteVertexArrays(1, &VerticalWallVAO);
    glDeleteVertexArrays(1, &VerticalWallVBO);

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
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+= 2)
        {
            collision = RCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight);
            if(collision != 0)
            {
                movement = false;
                PacmanView = translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        for(int i = 0, n = sizeof(VerticalWallCoords)/sizeof(float); i < n; i+= 2)
        {
            collision = RCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, VerticalWallCoords[i], VerticalWallCoords[i+1], WallHeight, WallWidth);
            if(collision != 0)
            {
                PacmanView = translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(movement)
        {
            if(PacmanView[3][0] >= 0.9f)
            {
                PacmanView[3][0] = -0.9f;
            }
            else
            {
                PacmanView = translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
            }
            InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        // PacmanMovement(0, 1, 'u');
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = TCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight);
            if(collision != 0)
            {
                PacmanView = translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        for(int i = 0, n = sizeof(VerticalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = TCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, VerticalWallCoords[i], VerticalWallCoords[i+1], WallHeight, WallWidth);
            if(collision != 0)
            {
                PacmanView = translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(movement)
        {
            if(PacmanView[3][1] >= 0.9f)
            {
                PacmanView[3][1] = -0.9f;
            }
            else
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, PacmanSpeed, 0.0f));
            }
            InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }  
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = LCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight);
            if(collision != 0)
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        for(int i = 0, n = sizeof(VerticalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = LCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, VerticalWallCoords[i], VerticalWallCoords[i+1], WallHeight, WallWidth);
            if(collision != 0)
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(movement)
        {
            if(PacmanView[3][0] <= -0.9f)
            {
                PacmanView[3][0] = 0.9f;
            }
            else
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
            }
            InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = BCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight);
            if(collision != 0)
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, PacmanSpeed, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        for(int i = 0, n = sizeof(VerticalWallCoords)/sizeof(float); i < n; i+=2)
        {
            collision = BCollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, VerticalWallCoords[i], VerticalWallCoords[i+1], WallHeight, WallWidth);
            if(collision != 0)
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, PacmanSpeed, 0.0f));
                movement = false;
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        if(movement)
        {
            if(PacmanView[3][1] <= -0.9f)
            {
                PacmanView[3][1] = 0.9f;
            }
            else
            {
                PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));
            }
            InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }
    }
    else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        InputColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
}
void LogMovement(float x, float y)
{
    system("clear");
    std::cout << "Position(x, y): " << x << ", " << y << "\n";
}
void MakeSquare(float width, float height) // WIP
{
    for(int i = 0, n = 2/width*2; i < n; i+=2)
    {
        HorizontalSquare[i] = -1.0f+i*width;
        HorizontalSquare[i+1] = 0.8f;
    }
    for(int i = 2/width*2, n = 2/width*2*2; i < n; i+=2)
    {
        HorizontalSquare[i] = -1.0f+i*width;
        HorizontalSquare[i+1] = -0.8f;
    }
    for(int i = 0, n = 2/height*2; i < n; i+=2)
    {
        VerticalSquare[i] = 0.8f;
        VerticalSquare[i+1] = -1.0f+i*height;
    }
    for(int i = 2/height*2, n = 2/height*2*2; i < n; i+=2)
    {
        VerticalSquare[i] = -0.8f;
        VerticalSquare[i+1] = -1.0f+i*height;
    }
}