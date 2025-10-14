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

enum Gamemode { ONE, MULTI };

// Global Constants

constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60,
              SPEED         = 200,
              SHRINK_RATE   = 100;

constexpr float PADDLE_HEIGHT = 90.0f,
                PADDLE_WIDTH  = 15.0f,
                BALL_HEIGHT   = 30.0f,
                BALL_WIDTH    = 30.0f;
    
                
constexpr Vector2   ORIGIN               = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                    PADDLE_BASE_SIZE     = { PADDLE_WIDTH, PADDLE_HEIGHT },
                    P1_INIT_POS          = { 10.0f, (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT - 2) },
                    P2_INIT_POS          = { SCREEN_WIDTH - PADDLE_WIDTH - 10, (SCREEN_HEIGHT/2) - (PADDLE_HEIGHT - 2) },
                    BALL_BASE_SIZE       = { BALL_HEIGHT, BALL_WIDTH },
                    BALL_INIT_POS        = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };


constexpr char PADDLE_FP[]  = "assets/paddle.jpg";
constexpr char BALL_FP[]    = "assets/ball.jpg";
constexpr int MAX_BALLS = 3;


Vector2 gBallPositions[MAX_BALLS] = { BALL_INIT_POS, BALL_INIT_POS, BALL_INIT_POS };
Vector2 gBallMovements[MAX_BALLS] = { {0.75f, 0.75f}, {0.75f, -0.75f}, {-0.75f, 0.75f} };
Vector2 gBallScales[MAX_BALLS] = { BALL_BASE_SIZE, BALL_BASE_SIZE, BALL_BASE_SIZE };
// Global Variables
AppStatus gAppStatus     = RUNNING;
int gNumBalls = 1;
float     gPreviousTicks = 0.0f;

Vector2 gP1Position       = P1_INIT_POS,
        gP1Movement       = { 0.0f, 0.0f },
        gP2Position       = P2_INIT_POS,
        gP2Movement       = { 0.0f, 0.0f },
        gPaddleScale      = PADDLE_BASE_SIZE,
        gBallPosition     = BALL_INIT_POS,
        gBallMovement     = { 0.75f, 0.75f },
        gBallScale        = BALL_BASE_SIZE;

Texture2D gPaddleTexture;
Texture2D gBallTexture;
Gamemode gGamemode = MULTI;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(postionA->x - positionB->x) - 
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(postionA->y - positionB->y) - 
                      ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

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

    gPaddleTexture  = LoadTexture(PADDLE_FP);
    gBallTexture    = LoadTexture(BALL_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{   
    
    if (IsKeyPressed(KEY_ONE)) {
        gNumBalls = 1;
    }
    if (IsKeyPressed(KEY_TWO)) {
        gNumBalls = 2;
    }
    if (IsKeyPressed(KEY_THREE)) {
        gNumBalls = 3;
    }
    gP1Movement = { 0.0f, 0.0f };
    gP2Movement = { 0.0f, 0.0f };
    if (IsKeyPressed(KEY_T)){
       gGamemode = gGamemode == MULTI ? ONE : MULTI;
    }
    if (IsKeyDown(KEY_W)) {
        if (gP1Position.y > 45) gP1Movement.y = -1;
    }
    if (IsKeyDown(KEY_S)) {
        if (gP1Position.y < SCREEN_HEIGHT - 45) gP1Movement.y = 1;
    }

    if (gGamemode == MULTI) {
        if (IsKeyDown(KEY_O)) {
            if (gP2Position.y > 45) gP2Movement.y = -1;
        }
        if (IsKeyDown(KEY_L)) {
            if (gP2Position.y < SCREEN_HEIGHT - 45) gP2Movement.y = 1;
        }
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gP1Position = {
        gP1Position.x,
        gP1Position.y + SPEED * gP1Movement.y * deltaTime
    };
    if (gGamemode == MULTI) {
        gP2Position = {
            gP2Position.x,
            gP2Position.y + SPEED * gP2Movement.y * deltaTime
        };
    } else {
       
        static int direction = 1;
        if (gP2Position.y <= 45) {
            direction = 1;
        } else if (gP2Position.y >= SCREEN_HEIGHT - 45) {
            direction = -1;
        }
        gP2Movement.y = direction;
        gP2Position = {
            gP2Position.x,
            gP2Position.y + SPEED * gP2Movement.y * deltaTime
        };
    }

   
    static int prevNumBalls = 1;
    if (gNumBalls != prevNumBalls) {
        for (int i = 0; i < gNumBalls; ++i) {
            gBallPositions[i] = BALL_INIT_POS;
            gBallMovements[i] = gBallMovements[i]; 
            gBallScales[i] = BALL_BASE_SIZE;
        }
        prevNumBalls = gNumBalls;
    }

    
    for (int i = 0; i < gNumBalls; ++i) {
        
        if (gBallPositions[i].y <= 15 || gBallPositions[i].y >= SCREEN_HEIGHT - 15) {
            gBallMovements[i].y *= -1;
        }
       
        if (isColliding(&gBallPositions[i], &gBallScales[i], &gP1Position, &gPaddleScale)) {
            gBallMovements[i].x *= -1;
        }
        if (isColliding(&gBallPositions[i], &gBallScales[i], &gP2Position, &gPaddleScale)) {
            gBallMovements[i].x *= -1;
        }
      
        gBallPositions[i] = {
            gBallPositions[i].x + SPEED * gBallMovements[i].x * deltaTime,
            gBallPositions[i].y + SPEED * gBallMovements[i].y * deltaTime
        };
    }

    for (int i = 0; i < gNumBalls; ++i) {
        if (gBallPositions[i].x <= 0 || gBallPositions[i].x >= SCREEN_WIDTH) {
            gAppStatus = TERMINATED;
            break;
        }
    }
    gBallPosition = gBallPositions[0];
    gBallMovement = gBallMovements[0];
    gBallScale = gBallScales[0];
}

void render()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    renderObject(&gPaddleTexture, &gP1Position, &gPaddleScale, 0.0f);
    renderObject(&gPaddleTexture, &gP2Position, &gPaddleScale, 0.0f);

    // Render all active balls
    for (int i = 0; i < gNumBalls; ++i) {
        renderObject(&gBallTexture, &gBallPositions[i], &gBallScales[i], 0.0f);
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
