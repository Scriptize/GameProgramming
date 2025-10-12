/**
* Author: Darren Blaylock
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

enum Gamemode { SINGLE, MULTI };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60,
              SPEED         = 200,
              SHRINK_RATE   = 100;

constexpr float PADDLE_HEIGHT = 90.0f,
                PADDLE_WIDTH  = 15.0f;
    

constexpr Vector2   ORIGIN               = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                    PADDLE_BASE_SIZE     = { PADDLE_WIDTH, PADDLE_HEIGHT },
                    P1_INIT_POS          = { 10.0f, (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT - 2) },
                    P2_INIT_POS          = { SCREEN_WIDTH - PADDLE_WIDTH - 10, (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT - 2) };


constexpr char PADDLE_FP[]  = "assets/paddle.jpg";


// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gPreviousTicks = 0.0f;

Vector2 gP1Position       = P1_INIT_POS,
        gP1Movement       = { 0.0f, 0.0f },
        gPaddleScale      = PADDLE_BASE_SIZE,

        gP2Position       = P2_INIT_POS,
        gP2Movement       = { 0.0f, 0.0f },
        gPaddleScale      = PADDLE_BASE_SIZE,

Texture2D gPaddleTexture;
Gamemode gGamemode = MULTI;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();


void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale, float angle)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        angle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
        "User Input / Collision Detection");

    gPaddleTexture  = LoadTexture(KYUREM_FP);
    // gReshiramTexture = LoadTexture(RESHIRAM_FP);
    // gZekromTexture   = LoadTexture(ZEKROM_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{   
    
    gP1Movement = { 0.0f, 0.0f };
    gP2Movement = { 0.0f, 0.0f };
    
    if (IsKeyDown(KEY_W)) gP1Movement.y =   1;
    if (IsKeyDown(KEY_S)) gP1Movement.y =  -1;

    if (gGamemode == MULTI){
        if (IsKeyDown(KEY_O)) gP2Movement.y =   1;
        if (IsKeyDown(KEY_L)) gP2Movement.y =  -1;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}
