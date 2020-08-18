#include <conovox/core.h>
#include <conovox/shader.h>
#include <conovox/camera.h>
#include <conovox/voxel.h>
#include <iostream>

#define WIDTH 800
#define HEIGHT 800
#define PIXELSCALE 1
#define MOUSE_SENSE 0.001;
float cameraYaw;
float cameraPitch;
int windowWidth = WIDTH;
int windowHeight = HEIGHT;

void framebufferResizeCall(GLFWwindow* window, int width, int height);
void keyboardCall(GLFWwindow* window, int key, int scancode, int action, int mods);
void mousePosCall(GLFWwindow* window, double mouseX, double mouseY);

const glm::vec3 XNORM(1.0f, 0.0f, 0.0f);
const glm::vec3 YNORM(0.0f, 1.0f, 0.0f);
const glm::vec3 ZNORM(0.0f, 0.0f, 1.0f);
const glm::mat4 IDENT(1.0f);

int main(void)
{
    // Initialize GLFW and set window restrictions
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Create new window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "gltest", NULL, NULL);
    if (!window)
    {
        std::cerr << "ERROR: Could not create GLFW window! Exiting...\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR: Could not initialize glad! Exiting...\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwSetFramebufferSizeCallback(window, framebufferResizeCall);

    // Configure inputs
    glfwSetKeyCallback(window, keyboardCall);
    glfwSetCursorPosCallback(window, mousePosCall);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Quad coordinates
    float quadVert[] = {
        -1.0,  1.0,  0.0,   0.0, 1.0,
         1.0,  1.0,  0.0,   1.0, 1.0,
        -1.0, -1.0,  0.0,   0.0, 0.0,
         1.0, -1.0,  0.0,   1.0, 0.0
    };
    unsigned int quadIndeces[] = {
        2, 1, 0,
        1, 2, 3
    };

    const float widget[] {
        0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, //x
        0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 0.0f, 1.0f, 0.0f, //y
        0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f  //z
    };

    const float cubeWidget[] {
        -0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f, -0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
        -0.51f,  0.51f,  0.51f, 0.0f, 0.0f, 0.0f,
         0.51f, -0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
         0.51f,  0.51f, -0.51f, 0.0f, 0.0f, 0.0f,
    };

    // Create custom framebuffer w/ texture
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // Create color texture and depth renderbuffer
    unsigned int cTEX;
    unsigned int dRBO;
    glGenTextures(1, &cTEX);
    glGenRenderbuffers(1, &dRBO);
    // Configure color texture
    glBindTexture(GL_TEXTURE_2D, cTEX);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth/PIXELSCALE, windowHeight/PIXELSCALE, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cTEX, 0);
    // Configure depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, dRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, windowWidth/PIXELSCALE, windowHeight/PIXELSCALE);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dRBO);
    // Check buffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR: Framebuffer is not complete! Status " << std::hex << glCheckFramebufferStatus(GL_FRAMEBUFFER) <<" Exiting...\n";
        return EXIT_FAILURE;
    }
    // Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Allocate VBOs for drawing data
    unsigned int qVBO;
    unsigned int cubeWidgetVBO;
    unsigned int widgetVBO;
    glGenBuffers(1, &qVBO);
    glGenBuffers(1, &cubeWidgetVBO);
    glGenBuffers(1, &widgetVBO);
    unsigned int qEBO;
    glGenBuffers(1, &qEBO);

    // Allocate vertex array objects to store the vertex attribute pointer configs
    unsigned int qVAO;
    unsigned int cubeWidgetVAO;
    unsigned int widgetVAO;
    glGenVertexArrays(1, &qVAO);
    glGenVertexArrays(1, &cubeWidgetVAO);
    glGenVertexArrays(1, &widgetVAO);

    // Initialize shaders
    Conovox::Shader voxShader("shaders/voxel.vert", "shaders/voxel.frag");
    Conovox::Shader widgetShader("shaders/widget.vert", "shaders/widget.frag");
    Conovox::Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
    if (voxShader.getID() == 0 || screenShader.getID() == 0 || widgetShader.getID() == 0)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    // Create camera
    Conovox::Camera camera(0.0f, 0.0f, -5.0f, 0.0f, M_PIf32, M_PIf32 / 3, 100.0f, (float)windowHeight/(float)windowWidth);

    // Create voxel environment
    const int CHUNK_WIDTH = 128;
    const int CHUNK_DEPTH = 128;
    const int CHUNK_HEIGHT = 128;
    std::vector<char> terrainTest;
    for (int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for (int z = 0; z < CHUNK_DEPTH; z++)
        {
            for (int x = 0; x < CHUNK_WIDTH; x++)
            {
                if (y < CHUNK_HEIGHT / 2)
                {
                    if ((x + z + y) % 2 == 1)
                    {
                        terrainTest.push_back(1);
                    }
                    else
                    {
                        terrainTest.push_back(0);
                    }
                }
                else
                {
                    terrainTest.push_back(-1);
                }
            }
        }
    }

    std::vector<Conovox::Material> testMaterials = {
        {{0.3647f, 0.4314f, 0.3922f}, 1.0f, 1.0f, 1.0f}, // Grey
        {{0.5f, 0.1f, 0.4f}, 1.0f, 1.0f, 1.0f}, // Purple
        {{0.1529f, 0.5686f, 0.3255f}, 1.0f, 1.0f, 1.0f}  // Green
    };
    const float VOX_SCALE = 0.0625f;
    float offX = VOX_SCALE * CHUNK_WIDTH;
    float offY = VOX_SCALE * CHUNK_HEIGHT;
    float offZ = VOX_SCALE * CHUNK_DEPTH;

    std::vector<Conovox::VoxChunk> chunks;
    int chunkNum = 0;
    for (int x = 0; x < 3; x++)
    {
        for (int z = 0; z < 3; z++)
        {
            chunks.push_back(Conovox::VoxChunk(terrainTest, testMaterials, CHUNK_WIDTH, CHUNK_DEPTH, CHUNK_HEIGHT, VOX_SCALE, glm::vec3(x*offX, 0.0f, z*offZ)));
        }
    }


    // Initialize & configure the VAOs
    // Bind quad VAO
    glBindVertexArray(qVAO);
    // Bind VBO & EBO
    glBindBuffer(GL_ARRAY_BUFFER, qVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVert), quadVert, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, qEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndeces), quadIndeces, GL_STATIC_DRAW);
    // Set attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configure widget VAOs
    glBindVertexArray(widgetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, widgetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(widget), widget, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(cubeWidgetVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeWidgetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeWidget), cubeWidget, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Colors
    const glm::vec4 purple(0.3f, 0.1f, 0.3f, 1.0f);
    const glm::vec4 skyBlue(0.8627f, 0.8667f, 0.9608f, 1.0f);

    // Configure lighting (temporary -_-)
    glm::vec3 sunDir(-1.0f, -1.0f, -1.0f);
    glm::vec3 sunColor(1.0f, 1.0f, 1.0f);
    voxShader.use();
    voxShader.setInt("numSuns", 1);
    voxShader.setVec3("suns[0].dir", glm::value_ptr(sunDir));
    voxShader.setVec3("suns[0].color", glm::value_ptr(sunColor));
    voxShader.setFloat("suns[0].ambFact", 0.25f);
    voxShader.setFloat("suns[0].diffFact", 1.0f);
    voxShader.setFloat("suns[0].specFact", 0.8f);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Update loop
    float dt = 0.0f;
    cameraPitch = 0.0f;
    cameraYaw = M_PIf32;
    while (!glfwWindowShouldClose(window))
    {
        // Update
        float runtime = (float)glfwGetTime();
        // Enable quadbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, windowWidth / PIXELSCALE, windowHeight / PIXELSCALE);
        // Clear buffers & enable depth
        glClearColor(skyBlue.r, skyBlue.g, skyBlue.b, skyBlue.a);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Process transformation matrices
        glm::mat4 model(1.0f);
        glm::mat4 norm(1.0f);
        glm::mat4 view;
        glm::mat4 proj;

        // Set camera angle
        camera.setAngles(cameraPitch, cameraYaw);
        glm::vec3 cameraDir = camera.getDir();

        // Handle walking
        glm::vec3 cameraMove(0.0f);
        bool fKey = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool bKey = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool leftKey = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool rightKey = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        bool upKey = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool downKey = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;
        bool slowKey = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        bool leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
        bool rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;
        if (fKey)
        {
            cameraMove += glm::vec3(cameraDir.x, 0.0f, cameraDir.z);
        }
        if (bKey)
        {
            cameraMove -= glm::vec3(cameraDir.x, 0.0f, cameraDir.z);
        }
        if (rightKey)
        {
            cameraMove += glm::vec3(-cameraDir.z, 0.0f, cameraDir.x);
        }
        if (leftKey)
        {
            cameraMove -= glm::vec3(-camera.getDir().z, 0.0f, camera.getDir().x);
        }
        if (upKey)
        {
            cameraMove += glm::vec3(0.0f, 1.0f, 0.0f);
        }
        if (downKey)
        {
            cameraMove -= glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        if (glm::length(cameraMove) > 0.0f)
        {
            cameraMove = 2.0f * glm::normalize(cameraMove) * dt;
            if (slowKey)
            {
                cameraMove *= 0.1f;
            }
            camera.setPos(camera.getPos() + cameraMove);
        }
        // Get camera view
        view = camera.getViewMat();
        glm::vec3 cameraPos = camera.getPos();
        // Compute transform matrices
        proj  = camera.getProjMat();

        // Jank raystep to voxel
        const int MAX_DIST = 5.0f;
        const float STEP_SIZE = 0.0001f;
        float rayLen = 0.0f;
        glm::vec3 rayDir = camera.getDir();
        glm::vec3 rayPos = cameraPos;
        glm::vec3 voxSpacePos;
        bool hasCollided = false;
        int chunkCollNum = 0;
        int coordX;
        int coordY;
        int coordZ;
        while (rayLen < MAX_DIST && hasCollided == false)
        {
            for (int v = 0; v < 9 && !hasCollided; v++)
            {
                voxSpacePos = chunks[v].getWorldVoxelCoords(rayPos);
                coordX = (int)floorf(voxSpacePos.x);
                coordY = (int)floorf(voxSpacePos.y);
                coordZ = (int)floorf(voxSpacePos.z);
                if (coordX >= 0 && coordX < CHUNK_WIDTH && 
                    coordY >= 0 && coordY < CHUNK_HEIGHT &&
                    coordZ >= 0 && coordZ < CHUNK_DEPTH && 
                    chunks[v].getVoxel(coordX, coordY, coordZ) >= 0)
                {
                    hasCollided = true;
                    chunkCollNum = v;
                    if (leftClick)
                    {
                        chunks[v].setSphere(coordX, coordY, coordZ, 20, -1);
                    }
                    else if (rightClick)
                    {
                        chunks[v].setSphere(coordX, coordY, coordZ, 10, 2);
                    }
                }
            }
            rayLen += STEP_SIZE;
            rayPos += rayDir * STEP_SIZE;
        }

        // Render voxel object
        voxShader.use();
        voxShader.setMatrix("view", glm::value_ptr(view));
        voxShader.setMatrix("proj", glm::value_ptr(proj));
        voxShader.setVec3("cameraPos", glm::value_ptr(cameraPos));
        for (int v = 0; v < 9; v++)
        {
            chunks[v].draw(voxShader);
        }

        // Render cube widget
        widgetShader.use();
        if (hasCollided)
        {
            glBindVertexArray(cubeWidgetVAO);
            model = glm::translate(glm::mat4(1.0f), chunks[chunkCollNum].getVoxelWorldCoords(glm::vec3((float)coordX, (float)coordY, (float)coordZ)));
            model = glm::scale(model, glm::vec3(VOX_SCALE));
            widgetShader.setMatrix("model", glm::value_ptr(model));
            widgetShader.setMatrix("view", glm::value_ptr(view));
            widgetShader.setMatrix("proj", glm::value_ptr(proj));
            glDrawArrays(GL_LINES, 0, 28);
        }

        // Render widget
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(widgetVAO);
        model = glm::rotate(glm::mat4(1.0f), camera.getPitch(), XNORM);
        model = glm::rotate(model, -camera.getYaw(), YNORM);
        model = glm::scale(model, glm::vec3(0.2f));
        widgetShader.setMatrix("model", glm::value_ptr(model));
        widgetShader.setMatrix("view", (float*)glm::value_ptr(IDENT));
        widgetShader.setMatrix("proj", (float*)glm::value_ptr(IDENT));
        glDrawArrays(GL_LINES, 0, 6);
        

        // Render to screen quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        screenShader.use();
        glBindVertexArray(qVAO);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cTEX);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
        // Event & buffer loop
        glfwSwapBuffers(window);
        glfwPollEvents();
        dt = (float)glfwGetTime() - runtime;
    }
    glDeleteFramebuffers(1, &FBO);
    glfwTerminate();
    return 0;
}

// OpenGL call to resize buffer alongside window
void framebufferResizeCall(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}

// Keyboard callback to handle keyboard events
void keyboardCall(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, 1);
    }
    else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
    }
}

// Mouse callback to handle movement
void mousePosCall(GLFWwindow* window, double mouseX, double mouseY)
{
    static double oldX = WIDTH / 2.0;
    static double oldY = HEIGHT / 2.0;
    // Get deltas
    double dMouseX = mouseX - oldX;
    double dMouseY = mouseY - oldY;
    oldX = mouseX;
    oldY = mouseY;
    double mouseOffX = dMouseX * MOUSE_SENSE;
    double mouseOffY = dMouseY * MOUSE_SENSE;
    // Set global camera angles
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        cameraPitch -= (float)mouseOffY;
        if (cameraPitch > M_PI_2f32 - 0.01)
        {
            cameraPitch = M_PI_2f32 - 0.01;
        }
        else if (cameraPitch < -M_PI_2f32 + 0.01)
        {
            cameraPitch = -M_PI_2f32 + 0.01;
        }
        cameraYaw += (float)mouseOffX;
    }
}