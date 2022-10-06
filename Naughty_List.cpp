// Include external development libraries for use of specific assets, shapes, textures, models, shaders, audio and functions
#include <stdio.h>   // Stdio library
#include "raylib.h"  // Raylib library
#include "raymath.h" // Raymath library
#include <iostream>  // iostream library

enum Animations
{
    Idle,
    //   Walk,
    Run,
    Jump,
    //   Slide,
    //   Die
};

// Declare Global Variables - accessible throughout the program, dynamically allocated (not known ahead of time) - Stored in memory for entire lifespan of the program

Animations animations{Animations::Idle};

float screenWidth = 1280; // Set the screen width of the window to 1280px,
float screenHeight = 720; // Set the screen height of the window to 720px,

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Program main entry point of game
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "Naughty List"); // Initialization of the main game window
    InitAudioDevice();

    Font SnowforSanta = LoadFont("resources/font/SnowforSanta.ttf");
    Font coolvetica_rg = LoadFont("resources/font/coolvetica_rg.otf");

    Music main_sound = LoadMusicStream("resources/audio/drama-tension.wav"); // Music by <a href="https://pixabay.com/users/musictown-25873992/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=music&amp;utm_content=113757">Musictown</a></a>
    Music menu_voice = LoadMusicStream("resources/audio/SANTA_CLAUS-You're_on_the_naughty_list.mp3");
    menu_voice.looping = false;
    main_sound.looping = true;
    SetMusicVolume(menu_voice, 0.2f);
    SetMusicVolume(main_sound, 0.05f);

    bool pause = false;
    bool playAudio = false;
    bool isPlaying = false;
    bool isRunning = false;
    // bool isInAir = false;

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

    int numFramesIdle = 16;
    int numFramesRun = 11;
    int numFramesJump = 16;

    float idle_FrameWidth = (float)(santa_Idle.width / numFramesIdle);
    float run_FrameWidth = (float)(santa_Run.width / numFramesRun);
    float jump_FrameWidth = (float)(santa_Jump.width / numFramesJump);

    float idle_FrameHeight = santa_Idle.height;
    float run_FrameHeight = santa_Run.height;
    float jump_FrameHeight = santa_Jump.height;

    float timer = 0.f;
    int frame = 0;
    int currentFrame = 0;
    float frameCounter = 0.f;
    float frameTime = 1.f / 15.f;
    float deltaTime = 0.f; // initialise as 0 here, get current time at beginning of loop
    float direction;
    // float gravity = 9.81;
    // float velocity = 10;

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
    //-------------------------------------------------------------------------------------------------------------------------------------------------

    // Main Game loop to keep the program active/repeating until all conditions have been met as defined within {}
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        //---------------------------------------------------------------------------------------------------------------------------------------------

        // Update Variables
        deltaTime = GetFrameTime();
        animations = Run;
        // animations = Idle;

        // Sounds
        UpdateMusicStream(main_sound);
        UpdateMusicStream(menu_voice);

        if (IsKeyDown(KEY_ENTER))
        {
            playAudio = !playAudio;

            if (playAudio)
                PlayMusicStream(menu_voice);
        }

        if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_M))
        {
            pause = !pause;

            if (pause)
                PauseMusicStream(main_sound);
            else
                ResumeMusicStream(main_sound);
        }

        if (IsKeyDown(KEY_SPACE))
        {
            animations = Jump;
            direction = 1;
            // velocity = 10;
        }

        timer += GetFrameTime();
        if (timer >= 0.5f)
        {
            timer = 0.f;
            frame += 1;
        }

        //////////////////////////////////////////////////////////////////////////////////////////

        frameCounter += deltaTime;

        switch (animations)
        {
        case Animations::Idle:
            if (frameCounter >= frameTime) //  ensure that Idle animation is executed at the correct FPS
            {
                frameCounter = 0.f; // frame presented to the player
                currentFrame++;     // increment frames

                if (currentFrame >= numFramesIdle)
                {
                    currentFrame = 1;
                }
            }

            objectRectIdle.x = idle_FrameWidth * currentFrame;
            objectRectIdle.y = 0;
            objectRectIdle.width = idle_FrameWidth * direction;
            idle_DestRec.y = 560;
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
            objectRectRun.width = run_FrameWidth * direction;
            run_DestRec.y = 560;
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
            objectRectJump.width = jump_FrameWidth * direction;
            jump_DestRec.y = 450; // jump height Y max position
            break;
        }

        /////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////

        if (IsKeyDown(KEY_D))
        {
            isPlaying = true;
            isRunning = true;
            animations = Run;
            direction = 1;

            playAudio = !playAudio;

            if (playAudio)
                PlayMusicStream(main_sound);
        }

        if ((isRunning) && (isPlaying))
            bg_SourceRec.x += 700 * deltaTime;
        else
            bg_SourceRec.x = 0 * deltaTime;

        if (bg_SourceRec.x >= bg_image.width)
            bg_SourceRec.x = 0;

        //----------------------------------------------------------------------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------------------------------------------------------------------

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw background image
        DrawTexturePro(bg_image, bg_SourceRec, bg_DestRec, Vector2Zero(), 0, WHITE);

        // Draw lines in centre of screen for reference
        // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
        // DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, YELLOW);

        // Draw line on top of screen for reference
        // DrawLine(0, screenHeight - screenHeight, screenWidth, screenHeight - screenHeight, BLACK);

        // DrawTexturePro(Texture, objectRect, destination, objectCenter, rotation, WHITE)

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

        const char *msg = {"Naughty List"};
        const char *msg2 = {"by Rob N"};

        const int textWidth_SnowforSanta = MeasureTextEx(SnowforSanta, msg, 38, 5).x;    //(font type, message, font size, font spacing)
        const int textWidth_coolvetica_rg = MeasureTextEx(coolvetica_rg, msg2, 22, 2).x; //(font type, message, font size, font spacing)

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
    UnloadFont(SnowforSanta);
    UnloadFont(coolvetica_rg);
    UnloadMusicStream(main_sound);

    CloseWindow();
    //-------------------------------------------------------------------------------------------------------------------------------------------------

    return 0;
}