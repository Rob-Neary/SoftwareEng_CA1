// Include external development libraries for use of specific assets, shapes, textures, models, shaders, audio and functions
//#define RAYGUI_IMPLEMENTATION // required for Raygui library
//#include <raygui.h>  // Raygui library
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
    Run,
    Jump,
    Dead,
    Hide
};

enum JumpDirection
{
    Up,
    Down
};

Animations animations{Animations::Idle};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Variables Declaration
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

float screenWidth = 1280.f; // Set the screen width of the window to 1280px,
float screenHeight = 720.f; // Set the screen height of the window to 720px,

static bool gameOver = false;
static bool retry = true;
static int playerScore = 0;
// static int hiScore = 0;

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
    Music main_sound = LoadMusicStream("resources/audio/drama-tension.wav"); // Music by <a href="https://pixabay.com/users/musictown-25873992/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=music&amp;utm_content=113757">Musictown</a></a>
    Music menu_voice = LoadMusicStream("resources/audio/SANTA_CLAUS-You're_on_the_naughty_list.mp3");
    Music footsteps_sound = LoadMusicStream("resources/audio/effects/snow_footsteps.wav");
    Music xmas_ambience = LoadMusicStream("resources/audio/ChristmasAmbienceSoundEffects.wav");

    SetMusicVolume(menu_voice, 0.3f);
    SetMusicVolume(main_sound, 0.1f);
    SetMusicVolume(footsteps_sound, 0.8f);
    SetMusicVolume(xmas_ambience, 0.2f);
    float timePlayed = 0.0f;

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Sounds
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Sound fall_sound = LoadSound("resources/audio/effects/fall_soundeffect.wav");
    Sound die_sound = LoadSound("resources/audio/effects/death_soundeffect.wav");
    SetSoundVolume(die_sound, 0.1f);
    SetSoundVolume(fall_sound, 0.3f);

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Booleans
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    bool pause = true;

    Texture2D xmasPresent[2]{}; //{LoadTexture("resources/sprites/xmas_present.png"), LoadTexture("resources/sprites/xmas_present2.png")};
    Rectangle xPSourceRect[2]{};
    Rectangle xPDestRect[2]{};

    xmasPresent[0] = LoadTexture("resources/sprites/xmas_present1.png");
    xmasPresent[1] = LoadTexture("resources/sprites/xmas_present2.png");

    float const xpScale = 0.015; // initialise presents scale
    float const ground = 560.f;
    float const maxJumpHeight = 330.f;

    for (int i = 0; i < 2; i++) // initialise variable i to 0 (initialised only once); boolean(test) expression, if i is less than 2 execute code within for loop; update i by 1 and re-execute test expression until condition is met
    {
        xPSourceRect[i].x = 0;
        xPSourceRect[i].y = 0;
        xPSourceRect[i].height = xmasPresent[i].height;
        xPSourceRect[i].width = xmasPresent[i].width;

        xPDestRect[i].height = xmasPresent[i].height * xpScale; // set presents scale
        xPDestRect[i].width = xmasPresent[i].width * xpScale;   // set presents scale
        xPDestRect[i].x = 0;
        xPDestRect[i].y = 0;

        std::cout << "testing value of i - for i < 2, i should return twice\n"; // debugging test expression
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Textures
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Texture2D bg_image = LoadTexture("resources/images/bg_image1.png");
    Texture2D pressEnterToPlay = LoadTexture("resources/images/UI/pressEnterToPlay.png");
    Texture2D score = LoadTexture("resources/images/UI/score.png");

    float const scoreScale = 0.28;
    float const enterScale = 0.7;

    pressEnterToPlay.width = pressEnterToPlay.width * enterScale;
    pressEnterToPlay.height = pressEnterToPlay.height * enterScale;

    score.width = score.width * scoreScale;
    score.height = score.height * scoreScale;

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

    Texture2D santa_Idle = LoadTexture("resources/sprites/idle_spritesheet_single.png");
    Texture2D santa_Run = LoadTexture("resources/sprites/run_spritesheet.png");
    Texture2D santa_Jump = LoadTexture("resources/sprites/jump_spritesheet_single.png");
    Texture2D santa_Dead = LoadTexture("resources/sprites/dead_spritesheet.png");

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Variables
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    int numFramesIdle = 1;
    int numFramesRun = 11;
    int numFramesJump = 1;
    int numFramesDead = 10;

    float idle_FrameWidth = (float)(santa_Idle.width / numFramesIdle);
    float run_FrameWidth = (float)(santa_Run.width / numFramesRun);
    float jump_FrameWidth = (float)(santa_Jump.width / numFramesJump);
    float dead_FrameWidth = (float)(santa_Dead.width / numFramesDead);

    float idle_FrameHeight = santa_Idle.height;
    float run_FrameHeight = santa_Run.height;
    float jump_FrameHeight = santa_Jump.height;
    float dead_FrameHeight = santa_Dead.height;

    int currentFrame = 0;
    float frameCounter = 0.f;
    float frameTime = 1.f / 15.f;
    float deltaTime = 0.f; // initialise as 0 here, get current time at beginning of loop
    int blinkFrames = 0;   // pause blink text

    float const jumpSpeedUp = 800.f;
    float const jumpSpeedDown = 500.f;
    float scrollSpeed = 700.f;

    JumpDirection jumpdirection;
    Rectangle tmpRect;

    Rectangle idle_DestRec = {screenWidth / 2.f, screenHeight / 2.f, idle_FrameWidth, idle_FrameHeight};
    Rectangle run_DestRec = {screenWidth / 2.f, screenHeight / 2.f, run_FrameWidth, run_FrameHeight};
    Rectangle jump_DestRec = {screenWidth / 2.f, screenHeight / 2.f, jump_FrameWidth, jump_FrameHeight};
    Rectangle dead_DestRec = {screenWidth / 2.f, screenHeight / 2.f, dead_FrameWidth, dead_FrameHeight};

    Rectangle objectRectIdle{};
    Rectangle objectRectRun{};
    Rectangle objectRectJump{};
    Rectangle objectRectDead{};

    objectRectIdle.width = idle_FrameWidth;
    objectRectIdle.height = idle_FrameHeight;

    objectRectRun.width = run_FrameWidth;
    objectRectRun.height = run_FrameHeight;

    objectRectJump.width = jump_FrameWidth;
    objectRectJump.height = jump_FrameHeight;

    objectRectDead.width = dead_FrameWidth;
    objectRectDead.height = dead_FrameHeight;

    Vector2 santa_IdleCentre = {(idle_FrameWidth / 2), (idle_FrameHeight / 2)};
    Vector2 santa_RunCentre = {(run_FrameWidth / 2), (run_FrameHeight / 2)};
    Vector2 santa_JumpCentre = {(jump_FrameWidth / 2), (jump_FrameHeight / 2)};
    Vector2 santa_DeadCentre = {(dead_FrameWidth / 2), (dead_FrameHeight / 2)};

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
            PlayMusicStream(footsteps_sound);
        }
        else if (pause)
            animations = Idle;

        if (gameOver)
        {
            animations = Dead;
        }
        frameCounter += deltaTime;

        //-------------------------------------------------------------------------------------------------------------------
        // Sounds
        //-------------------------------------------------------------------------------------------------------------------
        UpdateMusicStream(main_sound);
        UpdateMusicStream(menu_voice);
        UpdateMusicStream(xmas_ambience);
        UpdateMusicStream(footsteps_sound);

        if (!pause && !gameOver)
        {
            PlayMusicStream(main_sound);
            PlayMusicStream(xmas_ambience);
            timePlayed = GetMusicTimePlayed(main_sound) / GetMusicTimeLength(main_sound);
            if (timePlayed >= 1.0f)
                SeekMusicStream(main_sound, 0);
            StopMusicStream(menu_voice);
        }

        if (pause)
        {
            PlayMusicStream(menu_voice);
            PauseMusicStream(main_sound);
            PauseMusicStream(footsteps_sound);
            PauseSound(fall_sound);
        }

        if (gameOver)
        {
            StopMusicStream(main_sound);
            StopMusicStream(footsteps_sound);
            StopMusicStream(xmas_ambience);
            StopSound(fall_sound);
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Logic (detect user interaction)
        //-------------------------------------------------------------------------------------------------------------------

        if (IsKeyPressed(KEY_P))
        {
            if (animations != Dead)
            {
                if (!gameOver)
                {
                    pause = !pause;
                    if (pause)

                        PauseMusicStream(main_sound);
                    PauseMusicStream(footsteps_sound);
                    PauseSound(fall_sound);
                    animations = Idle;
                }
            }
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            if (animations != Dead)
            {
                if (!pause)
                {

                    if (animations != Jump)
                    {
                        animations = Jump;
                        StopMusicStream(footsteps_sound);
                        jumpdirection = Up;
                        jump_DestRec.y = run_DestRec.y;
                    }
                }
            }
        }

        if (IsKeyDown(KEY_D))
        {
            if (animations != Dead)
            {
                if (!pause)
                {
                    animations = Run;
                }
            }
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            if (retry)
            {
                gameOver = false;
                pause = false;
                animations = Run;
                PlayMusicStream(footsteps_sound);
                StopMusicStream(menu_voice);
                PlayMusicStream(main_sound);
                PlayMusicStream(xmas_ambience);
            }
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Game Logic (compute movement)
        //-------------------------------------------------------------------------------------------------------------------

        // background image scroll
        if (!pause)
            bg_SourceRec.x += scrollSpeed * deltaTime;
        else
            blinkFrames++;

        if (gameOver)
            bg_SourceRec.x = 0 * deltaTime;

        if (bg_SourceRec.x >= bg_image.width)
            bg_SourceRec.x = 0;

        // scrolling of the xmas presents
        if (!gameOver)
        {
            if (!pause)

            {
                for (int i = 0; i < 2; i++)
                {
                    xPDestRect[i].y = ground;
                    if (xPDestRect[i].x < GetRandomValue(-500, -200)) // min / max
                    {
                        xPDestRect[i].x = GetRandomValue(200, 500) + screenWidth;
                    }
                    else
                        xPDestRect[i].x -= scrollSpeed * deltaTime;

                    if (animations != Jump)
                        tmpRect = run_DestRec;
                    else
                        tmpRect = jump_DestRec;
                    if (CheckCollisionCircles(Vector2{tmpRect.x, tmpRect.y}, tmpRect.width / 4,
                                              Vector2{xPDestRect[i].x + xPDestRect[i].width / 2, xPDestRect[i].y + xPDestRect[i].height / 2}, xPDestRect[i].width / 2))
                    {
                        xPDestRect[i].x = GetRandomValue(200, 500) + screenWidth;

                        PlaySoundMulti(fall_sound);
                        PlaySoundMulti(die_sound);
                        StopMusicStream(footsteps_sound);
                        animations = Dead;
                        gameOver = true;
                        pause = true;
                        playerScore = 0;
                        std::cout << "Game Over\n";
                    }

                    else if ((santa_JumpCentre.x > xPDestRect[i].x) && !gameOver)
                    {
                        playerScore += 1;

                        // if (playerScore > hiScore)
                        //  hiScore = playerScore;
                    }
                }
            }
        }

        // animations
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

        case Animations::Dead:
            if (frameCounter >= frameTime)
            {
                frameCounter = 0.f;
                currentFrame++;

                if (currentFrame >= numFramesDead)
                {
                    currentFrame--; // do animation once and stop
                }
            }

            objectRectDead.x = dead_FrameWidth * currentFrame;
            objectRectDead.y = 0; // initial ground position Y of the player
            objectRectDead.width = dead_FrameWidth;
            dead_DestRec.y = ground;
            break;

        case Animations::Hide:
            break;
        }

        //----------------------------------------------------------------------------------------------------------------------------------------------
        // Start Drawing
        //----------------------------------------------------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw background image
        DrawTexturePro(bg_image, bg_SourceRec, bg_DestRec, Vector2Zero(), 0, WHITE);

        DrawFPS(1150, 10);

        const char *msg1 = {"Naughty List"};
        const char *msg2 = {"by Rob N"};
        const char *msg3 = {"You Died! Press [ENTER] to Try Again"};
        const char *msg4 = {"Game Paused"};
        const char *msg5 = {"Press [P] or [ENTER] to Continue"};

        const int textWidth_SnowforSanta = MeasureTextEx(SnowforSanta, msg1, 50, 15).x;  //(font type, message, font size, font spacing)
        const int textWidth_coolvetica_rg = MeasureTextEx(coolvetica_rg, msg2, 22, 2).x; //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta1 = MeasureTextEx(SnowforSanta, msg3, 40, 5).x;  //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta2 = MeasureTextEx(SnowforSanta, msg4, 46, 5).x;  //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta3 = MeasureTextEx(SnowforSanta, msg5, 40, 5).x;  //(font type, message, font size, font spacing)

        // Draw Xmas presents
        if (!gameOver)
        {
            if (!pause)
            {
                for (int i = 0; i < 2; i++)
                    DrawTexturePro(xmasPresent[i], xPSourceRect[i], xPDestRect[i], Vector2Zero(), 0, WHITE);
            }
        }

        if (!gameOver && !pause)
        {
            DrawTexture(score, 160 - score.width / 2, 120 - score.height / 2, RAYWHITE);
            DrawText(TextFormat("%4i", playerScore), 127, 130, 30, DARKBLUE);
        }

        if (gameOver)
        {
            DrawTextEx(SnowforSanta, msg3, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta1 / (float)2), screenHeight - screenHeight / 2}, 40, 5, RED);
        }

        if (pause && !gameOver && !bg_SourceRec.x == 0)
        {
            DrawTextEx(SnowforSanta, msg5, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta3 / (float)2), screenHeight - screenHeight / 2}, 40, 5, DARKGREEN);

            if ((blinkFrames / 30) % 3)
            {
                DrawTextEx(SnowforSanta, msg4, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta2 / (float)2), screenHeight - screenHeight / 2 - 100}, 46, 5, ColorFromHSV(0, 100, 53));
            }
        }

        if (pause && bg_SourceRec.x == 0 && !gameOver)
        {
            DrawTexture(pressEnterToPlay, screenWidth / 2 - pressEnterToPlay.width / 2, screenHeight / 2 - pressEnterToPlay.height / 2 + 30, WHITE);
            animations = Hide;
        }

        // Draw lines in centre of screen for reference
        // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
        // DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, YELLOW);

        // Draw line on top of screen for reference
        // DrawLine(0, screenHeight - screenHeight, screenWidth, screenHeight - screenHeight, BLACK);

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

        case Animations::Dead:
            DrawTexturePro(santa_Dead, objectRectDead, dead_DestRec, santa_DeadCentre, 0, WHITE);
            break;

        case Animations::Hide:
            break;
        }

        DrawTextEx(SnowforSanta, msg1, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta / (float)2), screenHeight - screenHeight + 20}, 50, 15, GetColor(0x052c46ff));
        DrawTextEx(coolvetica_rg, msg2, (Vector2){screenWidth / (float)2 - (textWidth_coolvetica_rg / (float)2), screenHeight - screenHeight + 70}, 22, 2, WHITE);

        EndDrawing();
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    // De-Initilisation
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    UnloadTexture(santa_Idle);
    UnloadTexture(santa_Run);
    UnloadTexture(santa_Jump);
    UnloadTexture(santa_Dead);
    UnloadTexture(bg_image);
    UnloadTexture(xmasPresent[0]);
    UnloadTexture(xmasPresent[1]);
    UnloadTexture(pressEnterToPlay);
    UnloadTexture(score);
    UnloadFont(SnowforSanta);
    UnloadFont(coolvetica_rg);
    UnloadMusicStream(main_sound);
    UnloadMusicStream(menu_voice);
    UnloadMusicStream(footsteps_sound);
    UnloadMusicStream(xmas_ambience);
    UnloadSound(fall_sound);
    UnloadSound(die_sound);

    CloseWindow();
    //-------------------------------------------------------------------------------------------------------------------------------------------------

    return 0;
}