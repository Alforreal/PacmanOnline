#include "shader.h"
#include <stdlib.h>
// #include <glm/ext/matrix_clip_space.hpp> // previously didn't work without them, now it does, idk why
// #include <glm/ext/matrix_transform.hpp> //required for matrix transformation, or at least I thought so, code works without them
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <unistd.h> // used for usleep()

bool movement = false;
bool pmovement = false; // movement on a previous iteration
float PacmanSize = 0.05f;
float WallWidth = 0.05f;
float WallHeight = 0.02f;
float PacmanSpeed = 0.015f;
float HorizontalWallCoords [] {
    0.5f, 0.5f,
    0.6f, 0.4f,
    0.4f, 0.6f,
    -0.5f, 0.5f,
    -0.6f, 0.4f,
    -0.4f, 0.6f,
    0.5f, -0.5f,
    0.6f, -0.4f,
    0.4f, -0.6f,
    -0.5f, -0.5f,
    -0.6f, -0.4f,
    -0.4f, -0.6f
};
float VerticalWallCoords[] {
    0.3f, 0.3f
};
glm::vec4 InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
int collision;
glm::mat4 PacmanView = glm::mat4(1.0f);
const int WINDOW_HEIGHT = 600, WINDOW_WIDTH = 600;
// int PacmanMovement(float x, float y, char direction);
int CollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1);
void LogMovement(float x, float y);
void processInput(GLFWwindow *window);
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Error: GLFW window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glad initiation:
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error: GLAD initiation failed\n";
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    Shader PacMan("Shaders/pacman.vs", "Shaders/pacman.fs");
    float PacmanPos[] = {
        // positions         
        PacmanSize, PacmanSize, 0.0f,
        PacmanSize, -PacmanSize, 0.0f,
        -PacmanSize, PacmanSize, 0.0f,

        -PacmanSize, PacmanSize, 0.0f,
        -PacmanSize, -PacmanSize, 0.0f,
        PacmanSize, -PacmanSize, 0.0f
    };

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
    
    
    Shader HorizontalWall("Shaders/HorizontalWall.vs", "Shaders/HorizontalWall.fs");
    float HorizontalWallPos[] = {
        WallWidth, WallHeight, 0.0f,
        WallWidth, -WallHeight, 0.0f,
        -WallWidth, WallHeight, 0.0f,

        -WallWidth, WallHeight, 0.0f,
        -WallWidth, -WallHeight, 0.0f,
        WallWidth, -WallHeight, 0.0f
    };
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

    //vertical wall:
    Shader VerticalWall("Shaders/VerticalWall.vs", "Shaders/VerticalWall.fs");
    float VerticalWallPos[] = {
        WallHeight, WallWidth, 0.0f,
        WallHeight, -WallWidth, 0.0f,
        -WallHeight, WallWidth, 0.0f,

        -WallHeight, WallWidth, 0.0f,
        -WallHeight, -WallWidth, 0.0f,
        WallHeight, -WallWidth, 0.0f
    };
    unsigned int VerticalWallVBO, VerticalWallVAO;
    glGenVertexArrays(1, &VerticalWallVAO);
    glGenBuffers(1, &VerticalWallVBO);
    glBindVertexArray(VerticalWallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VerticalWallVAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticalWallPos), VerticalWallPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // mathematics for the wall:
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

        // detecting collision:

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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        // PacmanMovement(1, 0, 'r');

        // for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        // {
        //     if(CollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight))
        //     {
        //         std::cout << "COLLISON\n";
        //         LogMovement(PacmanView[3][0], PacmanView[3][1]);
        //         movement = false;
        //         if(pmovement)
        //         {
        //             PacmanView = glm::translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
        //             pmovement = false;
        //         }
        //     }
        //     else
        //     {
        //         movement = true;
        //         pmovement = true;
        //     }
        // }
        // if(movement)
        // {
        //     PacmanView = glm::translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
        //     LogMovement(PacmanView[3][0], PacmanView[3][1]);
        // }
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+= 2)
        {
            collision = CollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight);
            if(collision == 1)
            {
                // std::cout << "Collision from the top at coordinates: (" << PacmanView[3][0] << ", " << PacmanView[3][1] << ")\n";
                PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            else if(collision == 2)
            {
                // std::cout << "Collisiton from the right at coordinates: (" << PacmanView[3][0] << ", " << PacmanView[3][1] << ")\n";
                PacmanView = glm::translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
                InputColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
            else
            {
                movement = true;
            }
        }
        if(movement)
        {
            PacmanView = translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
            InputColor = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        // PacmanMovement(0, 1, 'u');
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            if(CollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight))
            {
                std::cout << "COLLISON\n";
                LogMovement(PacmanView[3][0], PacmanView[3][1]);
                movement = false;
                if(pmovement)
                {
                    PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));
                    pmovement = false;
                }
            }
            else
            {
                movement = true;
                pmovement = true;
            }
        }
        if(movement)
        {
            PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, PacmanSpeed, 0.0f));
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }  
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        // PacmanMovement(-1, 0, 'l');
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            if(CollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight))
            {
                std::cout << "COLLISON\n";
                LogMovement(PacmanView[3][0], PacmanView[3][1]);
                movement = false;
                if(pmovement)
                {
                    PacmanView = glm::translate(PacmanView, glm::vec3(PacmanSpeed, 0.0f, 0.0f));
                    pmovement = false;
                }
            }
            else
            {
                movement = true;
                pmovement = true;
            }
        }
        if(movement)
        {
            PacmanView = glm::translate(PacmanView, glm::vec3(-PacmanSpeed, 0.0f, 0.0f));
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        // PacmanMovement(0, -1, 'd');
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+=2)
        {
            if(CollisionDetection(PacmanView[3][0], PacmanView[3][1], PacmanSize, PacmanSize, HorizontalWallCoords[i], HorizontalWallCoords[i+1], WallWidth, WallHeight))
            {
                std::cout << "COLLISON\n";
                LogMovement(PacmanView[3][0], PacmanView[3][1]);
                movement = false;
                if(pmovement)
                {
                    PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, PacmanSpeed, 0.0f)); 
                    pmovement = false;
                }
            }
            else
            {
                movement = true;
                pmovement = true;
            }
        }
        if(movement)
        {
            PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, -PacmanSpeed, 0.0f));  
            LogMovement(PacmanView[3][0], PacmanView[3][1]);
        }
    }
}
// COLLISION SYSTEM 
// The function returns a bit cryptic numbers, so here's explanation of outputs:
// This collision system is designed for the PacMan, and he's one of the only characters that move, so I focus on PacMan and check if it collides
// In the code I use x0, y0, width0 and height0 as inputs for the PacMan, and x1, y1, width1 and height1 as the wall, or anything else that collides with the PacMan really
/* 
         1
         ↓
     ----------
     |        |
 4 → | PacMan | ← 2
     |        |
     ----------
         ↑
         3

*/
// If there's no collision, the funciton will return 0 
// This code should be improved for efficiency, for now it is what it is
int CollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1) // width and height are counted from the center of the rectangle
{
    // collision from the top:
    if(y0+height0 >= y1-height1)
    {
        if(y0+height0 <= y1+height1)
        {
            if(x0-width0 <= x1+width1)
            {
                if(x0+width0 >= x1-width1)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    // collision from the bottom:
    if(y0-height0 <= y1+height1)
    {
        if(y0-height0 >= y1-height1)
        {
            if(x0-width0 <= x1+width1)
            {
                if(x0+width0 >= x1-width1)
                {
                    return 3;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    // collision from the left:
    if(x0-width0 <= x1+width1)
    {
        if(x0-width0 >= x1-width1)
        {
            if(y0+height0 >= y1-height1)
            {
                if(y0-height0 <= y1+height1)
                {
                    return 4;
                }
                else
                {
                    return 0;
                }
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
    // collision from the right:
    if(x0+width0 >= x1-width1)
    {
        if(x0+width0 <= x1+width1)
        {
            if(y0+height0 >= y1-height1)
            {
                if(y0-height0 <= y1+height1)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
void LogMovement(float x, float y)
{
    system("clear");
    std::cout << "Position(x, y): " << x << ", " << y << "\n";
}
/*
// Old variation of the collision system
int PacmanMovement(float x, float y, char direction)
{
    // movement = true; // For blocky movement, just as in the original PacMan
    // while(movement)
    // {
        // for debugging
        system("clear");
        std::cout << "Position(x, y): ";
        for(int j = 0; j < 2; j++)
        {
            std::cout << PacmanView[3][j] << " ";
        }
        std::cout << "\n";
        // check for collision
        for(int i = 0, n = sizeof(HorizontalWallCoords)/sizeof(float); i < n; i+= 2)
        {
            if(direction == 'l') // collision from the left
            {
                if(PacmanView[3][0]-PacmanSize <= HorizontalWallCoords[i]+WallWidth && PacmanView[3][0]+PacmanSize >= HorizontalWallCoords[i]+WallWidth && HorizontalWallCoords[i+1]+WallHeight >= PacmanView[3][1]-PacmanSize && HorizontalWallCoords[i+1]-WallHeight <= PacmanView[3][1]+PacmanSize)
                {
                    std::cout << "Collision from the left side on x: " << PacmanView[3][0]-PacmanSize << ", y(upper): " << PacmanView[3][1]+PacmanSize << ", y(lower): " << PacmanView[3][1]-PacmanSize << "\n";
                    PacmanView = glm::translate(PacmanView, glm::vec3(-x*(PacmanSpeed), -y*(PacmanSpeed), 0.0f));
                    return 0;
                }
            }
            else if(direction == 'r') // collision from the right
            {
                if(PacmanView[3][0]+PacmanSize >= HorizontalWallCoords[i]-WallWidth && PacmanView[3][0]+PacmanSize <= HorizontalWallCoords[i]+WallWidth && PacmanView[3][1]+PacmanSize >= HorizontalWallCoords[i+1]-WallHeight && PacmanView[3][1]-PacmanSize <= HorizontalWallCoords[i+1]+WallHeight)
                {
                    std::cout << "Collision from the right side on x: " << PacmanView[3][0]+PacmanSize << ", y(upper): " << PacmanView[3][1]+PacmanSize << ", y(lower): " << PacmanView[3][1]-PacmanSize << "\n";
                    PacmanView = glm::translate(PacmanView, glm::vec3(-x*(PacmanSpeed), -y*(PacmanSpeed), 0.0f));
                    return 0;
                }
            }
            else if(direction == 'u') // collision form the top
            {
                if(PacmanView[3][1]+PacmanSize >= HorizontalWallCoords[i+1]-WallHeight && PacmanView[3][1]+PacmanSize <= HorizontalWallCoords[i+1]+WallHeight && PacmanView[3][0]-PacmanSize <= HorizontalWallCoords[i]+WallWidth && PacmanView[3][0]+PacmanSize >= HorizontalWallCoords[i]-WallWidth)
                {
                    std::cout << "Collision from the top on y: " << PacmanView[3][1]+PacmanSize << ", x(right): " << PacmanView[3][0]+PacmanSize << ", x(left): " << PacmanView[3][0]-PacmanSize << "\n";
                    PacmanView = glm::translate(PacmanView, glm::vec3(-x*(PacmanSpeed), -y*(PacmanSpeed), 0.0f));
                    return 0;
                }
            }
            else if(direction == 'd') // collision from the bottom
            {
                if(PacmanView[3][1]-PacmanSize <= HorizontalWallCoords[i+1]+WallHeight && PacmanView[3][1]-PacmanSize >= HorizontalWallCoords[i+1]-WallHeight && PacmanView[3][0]-PacmanSize <= HorizontalWallCoords[i]+WallWidth && PacmanView[3][0]+PacmanSize >= HorizontalWallCoords[i]-WallWidth)
                {
                    std::cout << "Collision from the bottom on y: " << PacmanView[3][1]-PacmanSize << ", x(right): " << PacmanView[3][0]+PacmanSize << ", x(left): " << PacmanView[3][0]-PacmanSize << "\n";
                    return 0;
                }
            }
        }
    // } for the while(movement) loop
        PacmanView = glm::translate(PacmanView, glm::vec3(x*(PacmanSpeed), y*(PacmanSpeed), 0.0f));
    return 0;
}
*/