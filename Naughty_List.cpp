// Include external development libraries for use of specific assets, shapes, textures, models, shaders, audio and functions
#include <stdio.h>   // Standard input and output library
#include "raylib.h"  // Raylib library
#include "raymath.h" // Raymath library

// Declare Global Variables - accessible throughout the program, dynamically allocated (not known ahead of time) - Stored in memory for entire lifespan of the program

static const int screenWidth = 1280; // Set the screen width of the window to 1280px, static (single instance of the variable and can be shared with all objects in a class), constant (remains the same, read only and unchangeable), integer (whole number)
static const int screenHeight = 720; // Set the screen height of the window to 720px, static (single instance of the variable and can be shared with all objects in a class), constant (remains the same, read only and unchangeable), integer (whole number)

// Declare Local Program Functions

// Program main entry point of game
int main(void)
{
    InitWindow(screenWidth, screenHeight, "Naughty List"); // Initialization of the main game window

    Font christmabet = LoadFont("resources/font/christmabet.ttf");

    const char *msg = {"Naughty List"};
    const char *msg2 = {"by Rob N"};
    const int spacing = 2;

    Texture2D badSanta = LoadTexture("resources/run_spritesheet6.png");

    const int badSantaSpeed = 5;

    unsigned numFrames = 6;
    int frameWidth = badSanta.width / numFrames;
    int frameHeight = badSanta.height;
    Rectangle frameRec = {0.f, 0.f, (float)frameWidth, (float)frameHeight};
    Vector2 badSantaPosition = {((screenWidth / 2.f) - frameWidth), ((screenHeight / 2.f) - (frameHeight / 2))};
    Vector2 badSantaVelocity = {0.f, 0.f};

    unsigned frameDelay = 5;
    unsigned frameDelayCounter = 0;
    unsigned frameIndex = 0;

    SetWindowState(FLAG_VSYNC_HINT); // Set game to update only as fast as users monitor refresh rate
    SetTargetFPS(60);                // Set the target frames per second to 60FPS

    // Main Game loop to keep the program active/repeating until all conditions have been met as defined within {}
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            badSantaVelocity.x = badSantaSpeed;
            if (frameRec.width < 0)
            {
                frameRec.width = -frameRec.width;
            }
        }
        else if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            badSantaVelocity.x = -badSantaSpeed;
            if (frameRec.width > 0)
            {
                frameRec.width = -frameRec.width;
            }
        }
        else
        {
            badSantaVelocity.x = 0;
        }
        bool badSantaMoving = badSantaVelocity.x != 0.0f || badSantaVelocity.y != 0.0f;

        badSantaPosition = Vector2Add(badSantaPosition, badSantaVelocity);

        ++frameDelayCounter;
        if (frameDelayCounter > frameDelay)
        {
            frameDelayCounter = 0;

            if (badSantaMoving)
            {
                ++frameIndex;
                frameIndex %= numFrames;
                frameRec.x = (float)frameWidth * frameIndex;
            }
        }

        // Set the parameters/conditions within the while loop
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTextureRec(badSanta, frameRec, badSantaPosition, WHITE);

        int width = GetScreenWidth();
        int height = GetScreenHeight();

        DrawTextEx(christmabet, msg, (Vector2){(float)width / 2 - 100, (float)height - 60}, (float)christmabet.baseSize, spacing, DARKGREEN);
        DrawText(msg2, (float)width / 2 - 25, (float)height - 30, 20, DARKGRAY);
        // DrawTextEx(christmabet, msg2, (Vector2){(float)width / 2 - 25, (float)height - 30}, (float)christmabet.baseSize / 1.5, spacing, DARKGRAY);

        EndDrawing();
    }

    UnloadTexture(badSanta);
    UnloadFont(christmabet);

    CloseWindow();

    return 0;
}