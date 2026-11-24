#include "libs/glad_loader_docs.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include "stb_image.h"
#include "tools/Layout.h"
#include "tools/Texture.h"
#include "tools/Table.h"
#include "tools/Card.h"
#include "tools/Room.h"
#include "tools/Decor.h"

using namespace std;
using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ---------------------------------------------------------------------------
// The two fixed camera states
//   state 1 : straight above the table, cards lie flat
//   state 2 : the player's point of view, cards stand up
// `camBlend` goes 0 -> 1 (or 1 -> 0) over CAMERA_SECONDS (it grows with deltaTime,
// like the camera movement in the lecture), so the switch is animated.
// ---------------------------------------------------------------------------
const vec3 EYE_TOP(0.0f, 6.5f, 0.0f), TARGET_TOP(0.0f, 0.0f, 0.0f), UP_TOP(0.0f, 0.0f, -1.0f);
const vec3 EYE_PLAYER(0.0f, 2.6f, 6.0f), TARGET_PLAYER(0.0f, 0.8f, 0.0f), UP_PLAYER(0.0f, 1.0f, 0.0f);
const float CAMERA_SECONDS = 0.9f;

int cameraState = 1;
float camBlend = 0.0f;

bool playCardRequested = false; // set by processInput when G is pressed

Camera camera(EYE_TOP);

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (width <= 0 || height <= 0)
        return;
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

int main()
{
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Card Game - Table", NULL, NULL);
    if (!window) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
}

    Shader shader("./shaders/vs/Card.vs", "./shaders/fs/Card.fs");
    Shader modelShader("./shaders/vs/Model.vs", "./shaders/fs/Model.fs"); // for the 3D model (the globe)

    glEnable(GL_DEPTH_TEST);
    stbi_set_flip_vertically_on_load(true);

    // ---- textures ----
    GLuint woodTexture = loadTexture("./textures/wood.jpg");
    GLuint plainTexture = loadTexture("./textures/white.jpg");
    GLuint wallpaperTexture = loadTexture("./textures/wallpaper.jpg");
    GLuint floorTexture = loadTexture("./textures/floor.jpg");
    GLuint posterTexture = loadTexture("./textures/poster.jpg");
    GLuint globeTexture = loadTexture("./textures/globe.jpg");
    vector<string> cardPaths = {"./textures/card1.jpg", "./textures/card2.jpg", "./textures/card3.jpg",
                                "./textures/card4.jpg", "./textures/card5.jpg"};
    vector<GLuint> cardTextures;
    for (string path : cardPaths)
        cardTextures.push_back(loadTexture(path));

    // ---- scene objects ----
    Table table(woodTexture, plainTexture);
    Room room(wallpaperTexture, floorTexture, woodTexture, plainTexture, posterTexture);
    Decor decor(plainTexture, globeTexture);

    // the five cards of the player's stock, one in each slot of the STOCK zone
    vector<Card> stock;
    for (int i = 0; i < Layout::SLOTS; i++)
    {
        vec3 pivot(Layout::slotX(i), Layout::Y_CARD, Layout::zoneCenterZ(Layout::STOCK) + Layout::CARD_HEIGHT / 2);
        stock.push_back(Card(cardTextures[i], pivot));
    }

    srand(static_cast<unsigned int>(time(NULL))); // for the random card of key G

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // key G: pick a random card that is still in the stock and send it to the play area
        if (playCardRequested)
        {
            playCardRequested = false;

            vector<int> inStock;
            for (int i = 0; i < stock.size(); i++)
                if (stock[i].isInStock())
                    inStock.push_back(i);

            if (inStock.size() > 0) // nothing happens when the stock is empty (6th press)
            {
                int chosen = inStock[rand() % inStock.size()];
                // the card keeps its column and moves to the same column of the PLAYED zone
                vec3 target(Layout::slotX(chosen), Layout::Y_CARD, Layout::zoneCenterZ(Layout::PLAYED) + Layout::CARD_HEIGHT / 2);
                stock[chosen].moveTo(target);
            }
        }

        // move camBlend towards the wanted state
        float step = deltaTime / CAMERA_SECONDS;
        if (cameraState == 2)
        {
            camBlend = camBlend + step;
            if (camBlend > 1.0f)
                camBlend = 1.0f;
        }
        else
        {
            camBlend = camBlend - step;
            if (camBlend < 0.0f)
                camBlend = 0.0f;
        }

        glClearColor(0.05f, 0.04f, 0.04f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setInt("tex", 0);

        // the lecture's Camera class: move it between the two states (a + (b - a) * t)
        camera.Position = EYE_TOP + (EYE_PLAYER - EYE_TOP) * camBlend;
        vec3 center = TARGET_TOP + (TARGET_PLAYER - TARGET_TOP) * camBlend;
        camera.Front = normalize(center - camera.Position);
        camera.Up = normalize(UP_TOP + (UP_PLAYER - UP_TOP) * camBlend);

       float aspect = (SCR_HEIGHT > 0)
    ? (static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT))
    : (1280.0f / 720.0f);
if (aspect <= 0.0001f)
    aspect = 1280.0f / 720.0f;

mat4 projection = perspective(radians(45.0f), aspect, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);


        room.draw(shader);
        decor.draw(shader);
        table.draw(shader);
        for (int i = 0; i < stock.size(); i++)
        {
            stock[i].update(deltaTime);
            stock[i].draw(shader, camBlend);
        }

        // the 3D model uses its own shader, so it gets the same camera matrices again
        modelShader.use();
        modelShader.setInt("tex", 0);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        decor.drawModels(modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        cameraState = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        cameraState = 2;

    // G: react once per key press (not every frame while the key is held down)
    static bool gWasDown = false;
    bool gIsDown = (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS);
    if (gIsDown && !gWasDown)
        playCardRequested = true;
    gWasDown = gIsDown;
}

