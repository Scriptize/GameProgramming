/**
* Author: Darren Blaylock
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"

enum KyuremStatus { NORMAL, KWHITE, KBLACK };

// Global Constants
constexpr int SCREEN_WIDTH  = 800,
              SCREEN_HEIGHT = 450,
              FPS           = 60,
              SPEED         = 200,
              SHRINK_RATE   = 100;

constexpr Vector2 ORIGIN               = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  KYUREM_BASE_SIZE     = { 150.0f, 150.0f },
                  ZEKROM_BASE_SIZE     = { 150.0f, 150.0f },
                  RESHIRAM_BASE_SIZE   = { 150.0f, 150.0f },
                  KYUREM_INIT_POS      = { ORIGIN.x, ORIGIN.y },
                  ZEKROM_INIT_POS      = { ORIGIN.x - 300.0f, ORIGIN.y },
                  RESHIRAM_INIT_POS    = { ORIGIN.x + 300.0f, ORIGIN.y };

// @see https://idol.st/idols/ for source
constexpr char KYUREM_FP[]   = "assets/kyurem.png";
constexpr char ZEKROM_FP[]   = "assets/zekrom.png";
constexpr char RESHIRAM_FP[] = "assets/reshiram.png";
constexpr char WKYUREM_FP[]  = "assets/wkyurem.png";
constexpr char BKYUREM_FP[]  = "assets/bkyurem.png";

// Global Variables
bool gKyuremTransformed = false;
AppStatus gAppStatus     = RUNNING;
float     gPreviousTicks = 0.0f;

float gReshiramAngle = 0.0f;
float gZekromAngle   = 0.0f;

Vector2 gKyuremPosition   = KYUREM_INIT_POS,
        gKyuremMovement   = { 0.0f, 0.0f },
        gKyuremScale      = KYUREM_BASE_SIZE,

        gReshiramPosition = RESHIRAM_INIT_POS,
        gResiramMovement  = { 0.0f, 0.0f },
        gReshiramScale    = RESHIRAM_BASE_SIZE,

        gZekromPosition   = ZEKROM_INIT_POS,
        gZekromMovement   = { 0.0f, 0.0f },
        gZekromScale      = ZEKROM_BASE_SIZE,

        gMouseScale       = { 1.0f, 1.0f },
        gMousePosition    = GetMousePosition();

Texture2D gKyuremTexture;
Texture2D gZekromTexture;
Texture2D gReshiramTexture;

KyuremStatus gKyuremStatus = NORMAL;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

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

    gKyuremTexture   = LoadTexture(KYUREM_FP);
    gReshiramTexture = LoadTexture(RESHIRAM_FP);
    gZekromTexture   = LoadTexture(ZEKROM_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gKyuremMovement = { 0.0f, 0.0f };

    if      (IsKeyDown(KEY_A)) gKyuremMovement.x = -1;
    else if (IsKeyDown(KEY_D)) gKyuremMovement.x =  1;
    if      (IsKeyDown(KEY_W)) gKyuremMovement.y = -1;
    else if (IsKeyDown(KEY_S)) gKyuremMovement.y =  1;

    if (GetLength(&gKyuremMovement) > 1.0f) Normalise(&gKyuremMovement);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    // delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gKyuremPosition = {
        gKyuremPosition.x + SPEED * gKyuremMovement.x * deltaTime,
        gKyuremPosition.y + SPEED * gKyuremMovement.y * deltaTime
    };

    bool zekromCollision = isColliding(
        &gKyuremPosition, &gKyuremScale,
        &gZekromPosition, &gZekromScale
    );

    bool reshiramCollision = isColliding(
        &gKyuremPosition, &gKyuremScale,
        &gReshiramPosition, &gReshiramScale
    );

    if (!gKyuremTransformed) {
        if (zekromCollision) {
            gKyuremStatus = KBLACK;
            gKyuremTransformed = true;
        }
        if (reshiramCollision) {
            gKyuremStatus = KWHITE;
            gKyuremTransformed = true;
        }
    }
    
    if (gKyuremStatus == KWHITE) {
        gReshiramScale.x -= SHRINK_RATE * deltaTime;
        gReshiramScale.y -= SHRINK_RATE * deltaTime;

        // rotate clockwise (right)
        gReshiramAngle += 90.0f * deltaTime;

        if (gKyuremScale.x < 300.0f || gKyuremScale.y < 300.f){
            gKyuremScale.x += SHRINK_RATE * deltaTime;
            gKyuremScale.y += SHRINK_RATE * deltaTime;
        }

        if (gReshiramScale.x < 0.0f) gReshiramScale.x = 0.0f;
        if (gReshiramScale.y < 0.0f) gReshiramScale.y = 0.0f;

        if (gReshiramScale.x == 0.0f && gReshiramScale.y == 0.0f){
            gKyuremTexture = LoadTexture(WKYUREM_FP);
        }
    } 
    if (gKyuremStatus == KBLACK){
        gZekromScale.x -= SHRINK_RATE * deltaTime;
        gZekromScale.y -= SHRINK_RATE * deltaTime;

        // rotate counter-clockwise (left)
        gZekromAngle -= 90.0f * deltaTime;

        if (gKyuremScale.x < 300.0f || gKyuremScale.y < 300.f){
            gKyuremScale.x += SHRINK_RATE * deltaTime;
            gKyuremScale.y += SHRINK_RATE * deltaTime;
        }
        
        if (gZekromScale.x < 0.0f) gZekromScale.x = 0.0f;
        if (gZekromScale.y < 0.0f) gZekromScale.y = 0.0f;

        if (gZekromScale.x == 0.0f && gZekromScale.y == 0.0f){
            gKyuremTexture = LoadTexture(BKYUREM_FP);
        }
    }
}

void render()
{
    BeginDrawing();
    if (gKyuremStatus == KWHITE) {
        ClearBackground(ORANGE);
    }
    else if (gKyuremStatus == KBLACK) {
        ClearBackground(SKYBLUE);     
    } 
    else {
        ClearBackground(RAYWHITE); 
    }

    renderObject(&gKyuremTexture, &gKyuremPosition, &gKyuremScale, 0.0f);
    renderObject(&gReshiramTexture, &gReshiramPosition, &gReshiramScale, gReshiramAngle);
    renderObject(&gZekromTexture, &gZekromPosition, &gZekromScale, gZekromAngle);

    EndDrawing();
}

void shutdown() { CloseWindow(); }

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
