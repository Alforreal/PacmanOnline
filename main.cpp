#include "shader.h"
#include <stdlib.h>
// #include <glm/ext/matrix_clip_space.hpp>
// #include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

bool movement = false;
float WallCoords [] {
    0.5f, 0.5f,
    -0.5f, -0.5f,
};
glm::mat4 PacmanView = glm::mat4(1.0f);
void processInput(GLFWwindow *window);
int main()
{
    const int WINDOW_HEIGHT = 800, WINDOW_WIDTH = 800;
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
        0.05f, 0.05f, 0.0f,
        0.05f, -0.05f, 0.0f,
        -0.05f, 0.05f, 0.0f,

        -0.05f, 0.05f, 0.0f,
        -0.05f, -0.05f, 0.0f,
        0.05f, -0.05f, 0.0f
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
    
    
    Shader Wall("Shaders/wall.vs", "Shaders/wall.fs");
    float HorizontalWallPos[] = {
        0.05f, 0.02f, 0.0f,
        0.05f, -0.02f, 0.0f,
        -0.05f, 0.02f, 0.0f,

        -0.05f, 0.02f, 0.0f,
        -0.05f, -0.02f, 0.0f,
        0.05f, -0.02f, 0.0f
    };
    unsigned int WallVBO, WallVAO;
    glGenVertexArrays(1, &WallVAO);
    glGenBuffers(1, &WallVBO);

    glBindVertexArray(WallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, WallVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(HorizontalWallPos), HorizontalWallPos, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // mathematics for the wall

    glm::mat4 HorizontalWallModel = glm::mat4(1.0f);
    HorizontalWallModel = glm::translate(HorizontalWallModel, glm::vec3(0.0f, 0.0f, -2.0f));
    glm::mat4 HorizontalWallView = glm::mat4(1.0f);
    glm::mat4 HorizontalWallProjection;
    HorizontalWallProjection = glm::perspective(glm::radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);

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

        unsigned int PacmanProjectionLoc = glGetUniformLocation(PacMan.ID, "Pprojection");
        glUniformMatrix4fv(PacmanProjectionLoc, 1, GL_FALSE, glm::value_ptr(PacmanProjection));
        unsigned int WallModelLoc = glGetUniformLocation(Wall.ID, "Wmodel");
        unsigned int WallViewLoc = glGetUniformLocation(Wall.ID, "Wview");
        unsigned int WallProjectionLoc = glGetUniformLocation(Wall.ID, "Wprojection");
         
        glBindVertexArray(WallVAO);
        for(int i = 0, n = sizeof(WallCoords)/sizeof(float); i < n; i+=2)
        {
            HorizontalWallModel = glm::translate(HorizontalWallModel, glm::vec3(WallCoords[i], WallCoords[i+1], -2.0f));
            Wall.use();
            glUniformMatrix4fv(WallModelLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallModel));
            glUniformMatrix4fv(WallViewLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallView));
            glUniformMatrix4fv(WallProjectionLoc, 1, GL_FALSE, glm::value_ptr(HorizontalWallProjection));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            HorizontalWallModel = glm::mat4(1.0f);
        }

        // uniform values for the wall
        // ---------------------------
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &PacmanVAO);
    glDeleteBuffers(1, &PacmanVBO);
    glDeleteVertexArrays(1, &WallVAO);
    glDeleteVertexArrays(1, &WallVBO);

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
        movement = true;
        while(movement)
        {
            // for debugging
            system("clear");
            std::cout << "Posidion(x, y): ";
            for(int j = 0; j < 2; j++)
            {
                std::cout << PacmanView[3][j] << " ";
            }
            std::cout << "\n";
            PacmanView = glm::translate(PacmanView, glm::vec3(0.015f, 0.0f, 0.0f));
            movement = false;
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        movement = true;
        while(movement)
        {
            // for debugging
            system("clear");
            std::cout << "Posidion(x, y): ";
            for(int j = 0; j < 2; j++)
            {
                std::cout << PacmanView[3][j] << " ";
            }
            std::cout << "\n";
            PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, 0.015f, 0.0f));
            movement = false;
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        movement = true;
        while(movement)
        {
            // for debugging
            system("clear");
            std::cout << "Posidion(x, y): ";
            for(int j = 0; j < 2; j++)
            {
                std::cout << PacmanView[3][j] << " ";
            }
            std::cout << "\n";
            // check for collision
                // X:
            
            PacmanView = glm::translate(PacmanView, glm::vec3(-0.015f, 0.0f, 0.0f));
            movement = false;
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        movement = true;
        while(movement)
        {
            // for debugging  
            system("clear");
            std::cout << "Posidion(x, y): ";
            for(int j = 0; j < 2; j++)
            {
                std::cout << PacmanView[3][j] << " ";
            }
            std::cout << "\n";
            PacmanView = glm::translate(PacmanView, glm::vec3(0.0f, -0.015f, 0.0f));
            movement = false;
        }
        
    }
}