// Include external development libraries for use of specific assets, shapes, textures, models, shaders, audio and functions
#include <cstdio>    // cstdio library
#include "raylib.h"  // Raylib library
#include "raymath.h" // Raymath library
#include <iostream>  // iostream library
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define Types
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum Animations
{
    Idle,
    //   Walk,
    Run,
    Jump,
    //   Slide,
    //   Die
};

enum JumpDirection
{
    Up,
    Down
};

Animations animations{Animations::Idle};

float screenWidth = 1280.f; // Set the screen width of the window to 1280px,
float screenHeight = 720.f; // Set the screen height of the window to 720px,

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Program main entry point of game
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Naughty List"); // Initialization of the main game window
    InitAudioDevice();

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Fonts
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Font SnowforSanta = LoadFont("resources/font/SnowforSanta.ttf");
    Font coolvetica_rg = LoadFont("resources/font/coolvetica_rg.otf");

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Music and intitialise volume settings
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Music main_sound = LoadMusicStream("resources/audio/drama-tension.wav"); // Music by <a href="https://pixabay.com/users/musictown-25873992/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=music&amp;utm_content=113757">Musictown</a></a>
    // Music menu_voice = LoadMusicStream("resources/audio/SANTA_CLAUS-You're_on_the_naughty_list.mp3");
    // menu_voice.looping = false;
    // main_sound.looping = true;
    // SetMusicVolume(menu_voice, 0.3f);
    // SetMusicVolume(main_sound, 0.05f);

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Booleans
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    bool pause = true;

    Texture2D xmasPresent[2]; //{LoadTexture("resources/image_tiles/xmas_present.png"), LoadTexture("resources/image_tiles/xmas_present2.png")};
    Rectangle xPSourceRect[2];
    Rectangle xPDestRect[2];

    xmasPresent[0] = LoadTexture("resources/image_tiles/xmas_present1.png");
    xmasPresent[1] = LoadTexture("resources/image_tiles/xmas_present2.png");
    Vector2 xPOrigin[2];

    float const xpScale = 0.015; // 10%

    for (int i = 0; i < 2; i++)
    {
        xPSourceRect[i].x = 0;
        xPSourceRect[i].y = 0;
        xPSourceRect[i].height = xmasPresent[i].height;
        xPSourceRect[i].width = xmasPresent[i].width;

        xPDestRect[i].height = xmasPresent[i].height * xpScale;
        xPDestRect[i].width = xmasPresent[i].width * xpScale;
        xPDestRect[i].x = 0;
        xPDestRect[i].y = 0;

        xPOrigin[i].x = xmasPresent[i].width / 2 * xpScale;
        xPOrigin[i].y = xmasPresent[i].height / 2 * xpScale;
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Textures
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Texture2D bg_image = LoadTexture("resources/image_tiles/bg_image1.png");

    Rectangle bg_SourceRec = {};
    Rectangle bg_DestRec = {};

    bg_SourceRec.x = 0;
    bg_SourceRec.y = 0;
    bg_SourceRec.width = bg_image.width;
    bg_SourceRec.height = bg_image.height;

    bg_DestRec.x = 0;
    bg_DestRec.y = 0;
    bg_DestRec.width = screenWidth;
    bg_DestRec.height = screenHeight;

    Texture2D santa_Idle = LoadTexture("resources/idle_spritesheet.png");
    Texture2D santa_Run = LoadTexture("resources/run_spritesheet.png");
    Texture2D santa_Jump = LoadTexture("resources/jump_spritesheet.png");

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Variables
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    int numFramesIdle = 16;
    int numFramesRun = 11;
    int numFramesJump = 16;

    float idle_FrameWidth = (float)(santa_Idle.width / numFramesIdle);
    float run_FrameWidth = (float)(santa_Run.width / numFramesRun);
    float jump_FrameWidth = (float)(santa_Jump.width / numFramesJump);

    float idle_FrameHeight = santa_Idle.height;
    float run_FrameHeight = santa_Run.height;
    float jump_FrameHeight = santa_Jump.height;

    int currentFrame = 0;
    float frameCounter = 0.f;
    float frameTime = 1.f / 15.f;
    float deltaTime = 0.f; // initialise as 0 here, get current time at beginning of loop
    // float direction = 1.f;
    float const ground = 560.f;
    float const jumpSpeedUp = 500.f;
    float const jumpSpeedDown = 250.f;
    float maxJumpHeight = 350.f;
    float scrollSpeed = 700.f;
    JumpDirection jumpdirection;

    // float gravity = 9.81.f;
    // float velocity = 10.f;

    Rectangle idle_DestRec = {screenWidth / 2.f, screenHeight / 2.f, idle_FrameWidth, idle_FrameHeight};
    Rectangle run_DestRec = {screenWidth / 2.f, screenHeight / 2.f, run_FrameWidth, run_FrameHeight};
    Rectangle jump_DestRec = {screenWidth / 2.f, screenHeight / 2.f, jump_FrameWidth, jump_FrameHeight};

    Rectangle objectRectIdle{};
    Rectangle objectRectRun{};
    Rectangle objectRectJump{};

    objectRectIdle.width = idle_FrameWidth;
    objectRectIdle.height = idle_FrameHeight;

    objectRectRun.width = run_FrameWidth;
    objectRectRun.height = run_FrameHeight;

    objectRectJump.width = jump_FrameWidth;
    objectRectJump.height = jump_FrameHeight;

    Vector2 santa_IdleCentre = {(idle_FrameWidth / 2), (idle_FrameHeight / 2)};
    Vector2 santa_RunCentre = {(run_FrameWidth / 2), (run_FrameHeight / 2)};
    Vector2 santa_JumpCentre = {(jump_FrameWidth / 2), (jump_FrameHeight / 2)};
    SetTargetFPS(144); // Set the target frames per second to 144FPS
    //-----------------------------------------------------------------------------------------------------------------------

    // Main Game loop to keep the program active/repeating until all conditions have been met as defined within {}
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //-------------------------------------------------------------------------------------------------------------------
        // Update Variables
        //-------------------------------------------------------------------------------------------------------------------
        deltaTime = GetFrameTime();
        if (animations != Jump && !pause)
        {
            animations = Run;
        }
        else if (pause)
            animations = Idle;

        frameCounter += deltaTime;

        //-------------------------------------------------------------------------------------------------------------------
        // Sounds
        //-------------------------------------------------------------------------------------------------------------------
        // UpdateMusicStream(main_sound);
        // UpdateMusicStream(menu_voice);
        // PlayMusicStream(menu_voice);

        //-------------------------------------------------------------------------------------------------------------------
        // Logic (detect user interaction)
        //-------------------------------------------------------------------------------------------------------------------

        if (IsKeyPressed(KEY_P))
        {
            pause = !pause;
            if (pause)
            {
                // PauseMusicStream(main_sound);
                animations = Idle;
            }
            // else
            //  ResumeMusicStream(main_sound);
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            if (!pause)
            {

                if (animations != Jump)
                {
                    animations = Jump;
                    jumpdirection = Up;
                    jump_DestRec.y = run_DestRec.y;
                }
            }
        }

        if (IsKeyDown(KEY_D))
        {
            if (!pause)
            {
                animations = Run;

                // playAudio = !playAudio;

                // if (playAudio)
                //  PlayMusicStream(main_sound);
            }
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Logic (compute movement)
        //-------------------------------------------------------------------------------------------------------------------

        // background image scroll
        if (!pause)
            bg_SourceRec.x += scrollSpeed * deltaTime;

        if (bg_SourceRec.x >= bg_image.width)
            bg_SourceRec.x = 0;

        // scrolling of the xmas presents
        for (int i = 0; i < 2; i++)
        {
            if (xPDestRect[i].x < GetRandomValue(-500, -200)) // min / max
            {
                xPDestRect[i].x = GetRandomValue(200, 500) + screenWidth;
                xPDestRect[i].y = ground;
            }
            else
                xPDestRect[i].x -= scrollSpeed * deltaTime;

            if (CheckCollisionCircles(santa_RunCentre, run_DestRec.width / 4, xPOrigin[i], xPDestRect[i].width / 2))
            {
                xPDestRect[i].x = GetRandomValue(200, 500) + screenWidth;
            }
        }

        // jump
        if (animations == Jump)
        {
            if (jumpdirection == Up)
            {
                if (jump_DestRec.y <= maxJumpHeight)
                    jumpdirection = Down;
                else
                {
                    jump_DestRec.y = jump_DestRec.y - jumpSpeedUp * deltaTime;
                }
            }
            else
            {
                if (jump_DestRec.y >= ground)
                    animations = Run;
                else
                {
                    jump_DestRec.y = jump_DestRec.y + jumpSpeedDown * deltaTime;
                }
            }
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Switch Animations (Logic to update animation)
        //-------------------------------------------------------------------------------------------------------------------
        switch (animations)
        {
        case Animations::Idle:
            if (frameCounter >= frameTime) //  ensure that Idle animation is executed at the correct FPS
            {
                frameCounter = 0.f; // frame presented to the player
                currentFrame++;     // increment frames

                if (currentFrame >= numFramesIdle) // reset frames
                {
                    currentFrame = 1; // reset to this value
                }
            }

            objectRectIdle.x = idle_FrameWidth * currentFrame;
            objectRectIdle.y = 0;
            objectRectIdle.width = idle_FrameWidth;
            idle_DestRec.y = ground;
            break;

        case Animations::Run:
            if (frameCounter >= frameTime)
            {
                frameCounter = 0.f;
                currentFrame++;

                if (currentFrame >= numFramesRun)
                {
                    currentFrame = 1;
                }
            }

            objectRectRun.x = run_FrameWidth * currentFrame;
            objectRectRun.y = 0;
            objectRectRun.width = run_FrameWidth;
            run_DestRec.y = ground;
            break;

        case Animations::Jump:
            if (frameCounter >= frameTime)
            {
                frameCounter = 0.f;
                currentFrame++;

                if (currentFrame >= numFramesJump)
                {
                    currentFrame = 1;
                }
            }

            objectRectJump.x = jump_FrameWidth * currentFrame;
            objectRectJump.y = 0; // initial ground position Y of the player
            objectRectJump.width = jump_FrameWidth;
            break;
        }

        //----------------------------------------------------------------------------------------------------------------------------------------------
        // Start Drawing
        //----------------------------------------------------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw background image
        DrawTexturePro(bg_image, bg_SourceRec, bg_DestRec, Vector2Zero(), 0, WHITE);

        // Draw Xmas presents
        DrawTexturePro(xmasPresent[0], xPSourceRect[0], xPDestRect[0], Vector2Zero(), 0, WHITE);

        const char *msg = {"Naughty List"};
        const char *msg2 = {"by Rob N"};

        const int textWidth_SnowforSanta = MeasureTextEx(SnowforSanta, msg, 38, 5).x;    //(font type, message, font size, font spacing)
        const int textWidth_coolvetica_rg = MeasureTextEx(coolvetica_rg, msg2, 22, 2).x; //(font type, message, font size, font spacing)

        // Draw lines in centre of screen for reference
        // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
        // DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, YELLOW);

        // Draw line on top of screen for reference
        // DrawLine(0, screenHeight - screenHeight, screenWidth, screenHeight - screenHeight, BLACK);

        // DrawTexturePro(Texture, objectRect, destination, objectCenter, rotation, WHITE);

        switch (animations)
        {
        case Animations::Idle:
            DrawTexturePro(santa_Idle, objectRectIdle, idle_DestRec, santa_IdleCentre, 0, WHITE);
            break;

        case Animations::Run:
            DrawTexturePro(santa_Run, objectRectRun, run_DestRec, santa_RunCentre, 0, WHITE);
            break;

        case Animations::Jump:
            DrawTexturePro(santa_Jump, objectRectJump, jump_DestRec, santa_JumpCentre, 0, WHITE);
            break;
        }

        DrawTextEx(SnowforSanta, msg, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta / (float)2), screenHeight - screenHeight}, 38, 5, GetColor(0x052c46ff));
        DrawTextEx(coolvetica_rg, msg2, (Vector2){screenWidth / (float)2 - (textWidth_coolvetica_rg / (float)2), screenHeight - screenHeight + 30}, 22, 2, WHITE);

        EndDrawing();
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    // De-Initilisation
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    UnloadTexture(santa_Run);
    UnloadTexture(santa_Jump);
    UnloadTexture(bg_image);
    UnloadTexture(xmasPresent[0]);
    UnloadTexture(xmasPresent[1]);
    UnloadFont(SnowforSanta);
    UnloadFont(coolvetica_rg);
    // UnloadMusicStream(main_sound);
    // UnloadMusicStream(menu_voice);

    CloseWindow();
    //-------------------------------------------------------------------------------------------------------------------------------------------------

    return 0;
}