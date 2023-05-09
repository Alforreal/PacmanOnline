#include <glm/glm.hpp>
class sprite // used for the Pacman and the ghosts(hopefully)
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
        glm::mat4 projection; // projection matrix for the perspective
        void findpos(void)
        {
            pos[0]  =  size; pos[1]  =  size; pos[2]  = 0.0f;
            pos[3]  =  size; pos[4]  = -size; pos[5]  = 0.0f;
            pos[6]  = -size; pos[7]  =  size; pos[8]  = 0.0f;
            pos[9]  = -size; pos[10] =  size; pos[11] = 0.0f;
            pos[12] = -size; pos[13] = -size; pos[14] = 0.0f;
            pos[15] =  size; pos[16] = -size; pos[17] = 0.0f;
        }
};
class wall
{
    public:
        float width[128];
        float height[128];
        float pos[18];
        glm::vec4 color; // Color of the walls
        float x[128];
        float y[128];
        unsigned int VBO;
        unsigned int VAO;
        glm::mat4 view = glm::mat4(1.0f); // view matrix
        glm::mat4 model = glm::mat4(1.0f); // model matrix
        glm::mat4 projection;
        // Stuff for the wall creator:
        int index = 0;
        double StartingX;
        double StartingY;
        bool MousePressed = false;
        float MapSpeed;
};
class button {
    public:
        float width;
        float height;
        float pos[18];
        float coords[2];
        bool isPressed;
        bool ShouldRender;
        glm::vec4 color;
        unsigned int VBO;
        unsigned int VAO;
        glm::mat4 view = glm::mat4(1.0f); // view matrix
        glm::mat4 model = glm::mat4(1.0f); // model matrix
        glm::mat4 projection;
        void findpos(void)
        {
            pos[0]  =  width; pos[1]  =  height; pos[2]  = 0.0f;
            pos[3]  =  width; pos[4]  = -height; pos[5]  = 0.0f;
            pos[6]  = -width; pos[7]  =  height; pos[8]  = 0.0f;
            pos[9]  = -width; pos[10] =  height; pos[11] = 0.0f;
            pos[12] = -width; pos[13] = -height; pos[14] = 0.0f;
            pos[15] =  width; pos[16] = -height; pos[17] = 0.0f;
        }
};