//
// Author: Robert Neary
// Date: 13 November 2022
// Course: MSc in Games and Extended Reality (Dundalk IT)
// Module: Software Engineering
// Continuous Assesment 1 - Raylib Application
// Game Title: Naughty List
//
// Include external development libraries for use of specific assets, shapes, textures, models, shaders, audio and functions
//#define RAYGUI_IMPLEMENTATION // required for Raygui library
//#include <raygui.h>  // Raygui library
#include <cstdio>    // cstdio library
#include "raylib.h"  // Raylib library
#include "raymath.h" // Raymath library
#include <iostream>  // iostream library, to utilise the C++ print function (std::cout << "String";) i.e. character output

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define Types
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum Animations // Define an enumeration (each with a unique int value for the 5 animation states i.e. 0, 1, 2, 3, 4 consequtive to the declaration) to include all animation states in the game, this will be utilised during the main game loop and within a state machine to update each animation
{
    Idle, // declare state Idle
    Run,  // declare state Run
    Jump, // declare state Jump
    Dead, // declare state Dead
    Hide  // declare state Hide
};

enum JumpDirection // Declare an enumeration (each jump direction with a unique int value i.e. 0 and 1) to define the jump direction of the character in the game, this will be utilised during the main game loop to determine if the character is moving up and when the max height is reached, we will flip the direction to moving down
{
    Up,  // declare jump direction Up
    Down // declare jump direction Down
};

Animations animations{Animations::Idle}; // Initialise the starting animation state as Idle

float myTimer = 0.f; // declare a timer variable as a floating point number. initialised at frame 0

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Variables Declaration
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

float screenWidth = 1280.f; // Set the screen width of the window to 1280px,
float screenHeight = 720.f; // Set the screen height of the window to 720px,

static bool gameOver = false; // create a boolean variable for game over, we can use this within the main game loop to verify conditions or state changes. This will be used to check if the game is over
static bool retry = true;     // create a boolean variable for retry, we can use this within the main game loop to verify conditions or state changes. This will check if the user can retry the level
static int playerScore = 0;   // initialise variable of playerScore to 0. This is being declared as a static int as the score should only increase using whole numbers and static because the score remains in memory while the game is running. This is the same reason for above variables

Rectangle xPDestRect[2]{};  // define the destination rectangle for the presents within an array of 2
float deltaTime = 0.f;      // create a vaiable for deltaTime and initialise as 0. This will be used as the variable for GetFrameTime() at the beginning of the main game loop. Declaring the variable as deltaTime allows us to re-use the function more easily
float scrollSpeed = 700.f;  // create a variable for scrollSpeed and initialise to 700, this will be used to determine the speed the background texture will move
float const ground = 560.f; // intialise variable ground to 560px, this will be used to determine the ground position in relation to predefined screen height of 720px

void initPresents(int i) // initialise variable initPresents with a parameter expected of (int i). (this is known as a method in C++ or a function in C)
{
    xPDestRect[i].y = ground;                         // set the Y position of the presents destination rectangle on ground where i is both presents
    if (xPDestRect[i].x < GetRandomValue(-500, -200)) // if the present on X axis is less than a random value between -500 and -200 px (min and max) then execute the following
    {
        xPDestRect[i].x = GetRandomValue(200, 1000) + screenWidth; // spawn a new present between 200 and 1000 plus the screenwidth i.e. a new present will spawn off screen on the right and come into view during presents scroll
        if (i == 1)                                                // if i is equal to 1 execute condition below
        {
            do // do the following - execute the condition below only while the "while" condition is running
            {
                xPDestRect[i].x = GetRandomValue(200, 1000) + screenWidth; // spawn a new present between 200 and 1000 (min and max) plus the screenwidth i.e. a new present will spawn off screen on the right and come into view during presents scroll
            } while (abs(xPDestRect[0].x - xPDestRect[i].x) < 400);        // do the above while this condition is active. To ensure the absolute value is greater than 400px as the presents spawn. This avoids the user colliding with a present on game start or retry. Allows some distance in px before the presents begin moving
        }
    }
    else
        xPDestRect[i].x -= scrollSpeed * deltaTime; // if above condition is met then begin scrolling (moving) the presents on screen from right to left. (this is multiplied by deltatime to ensure the frames presented to the player are the same everyone)
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Program main entry point of game
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main() // main entry point of the game
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);                     // set configuration flags at main entry point of game, in this case, multi sample anti aliasing is increased 4 times
    InitWindow(screenWidth, screenHeight, "Naughty List"); // Initialization of the main game window
    InitAudioDevice();                                     // Initialization of the main audio device
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Fonts
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Font SnowforSanta = LoadFont("resources/font/SnowforSanta.ttf");   // load font so it can be used within the game
    Font coolvetica_rg = LoadFont("resources/font/coolvetica_rg.otf"); // load font so it can be used within the game

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Music and intitialise volume settings
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Music main_sound = LoadMusicStream("resources/audio/drama-tension.wav"); // Load music for use within game. Music by <a href="https://pixabay.com/users/musictown-25873992/?utm_source=link-attribution&amp;utm_medium=referral&amp;utm_campaign=music&amp;utm_content=113757">Musictown</a></a>
    Music menu_voice = LoadMusicStream("resources/audio/SANTA_CLAUS-You're_on_the_naughty_list.mp3");
    Music footsteps_sound = LoadMusicStream("resources/audio/effects/snow_footsteps.wav");
    Music xmas_ambience = LoadMusicStream("resources/audio/ChristmasAmbienceSoundEffects.wav");
    // set music volumes
    SetMusicVolume(menu_voice, 0.3f);
    SetMusicVolume(main_sound, 0.1f);
    SetMusicVolume(footsteps_sound, 0.8f);
    SetMusicVolume(xmas_ambience, 0.2f);
    float timePlayed = 0.0f; // create a variable for timePlayed to be used within the main game loop

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Sounds
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Sound fall_sound = LoadSound("resources/audio/effects/fall_soundeffect.wav");
    Sound die_sound = LoadSound("resources/audio/effects/death_soundeffect.wav");
    Sound points = LoadSound("resources/audio/effects/points.wav");

    // set sound volumes
    SetSoundVolume(die_sound, 0.1f);
    SetSoundVolume(fall_sound, 0.3f);
    SetSoundVolume(points, 0.4f);

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Booleans
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    bool pause = true; // initialise pause variable to True. This is a boolean expression

    Texture2D xmasPresent[2]{};  // Create an array for both presents, there are 2 different types of present;
    Rectangle xPSourceRect[2]{}; // Create an array for both present rectangles, this aligns with the 2 presents;

    xmasPresent[0] = LoadTexture("resources/sprites/xmas_present1.png"); // initialise the 1st present as 0 in the array and import the texture
    xmasPresent[1] = LoadTexture("resources/sprites/xmas_present2.png"); // initialise the 2nd present as 1 in the array and import the texture

    float const xpScale = 0.015;       // initialise variable presents scale as 0.015 of the original imported texture scale. const as this scale does not change
    float const maxJumpHeight = 330.f; // initialise variable maxJumpHeight of the character. const as this max jump height does not change

    // Source and Dest Rectangles
    // the source rectangle is the portion of a spritesheet/texture i.e. for a spritesheet with multiple rows and columns, we would use coordinates to define a source rectangle for 1 specific sprite / frame of that spritesheet
    // the destination rectangle is where we will position that individual sprite on the screen in game (we apply a scale to this rectangle if necessary)
    // in the case of this game, each spritesheet will only have 1 row, however each spritesheet will have a differing amount of sprites/frames on each spritesheet
    for (int i = 0; i < 2; i++) // utilising the presents array, initialise variable i to 0 (initialised only once); boolean(test) expression, if i is less than 2 execute code within for loop; increment i by 1 and re-execute test expression until condition is met
    {
        xPSourceRect[i].x = 0;                          // initialise the postion on x to 0 for the source rectangle
        xPSourceRect[i].y = 0;                          // initialise the postion on y to 0 for the source rectangle
        xPSourceRect[i].height = xmasPresent[i].height; // set height of source rect to the xmas present height
        xPSourceRect[i].width = xmasPresent[i].width;   // set width of source rect to the xmas present width

        xPDestRect[i].x = 0;                                    // initialise the postion on x to 0 for the destination rectangle
        xPDestRect[i].y = 0;                                    // initialise the postion on y to 0 for the destination rectangle
        xPDestRect[i].height = xmasPresent[i].height * xpScale; // set present scale by multiplying the height of the original texture by the scale (scales the height)
        xPDestRect[i].width = xmasPresent[i].width * xpScale;   // set present scale by multiplying the width of the original texture by the scale (scales the width)

        std::cout << "testing value of i - for i < 2, i should return twice\n"; // debug the test expression above
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Load Textures
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    Texture2D bg_image = LoadTexture("resources/images/bg_image1.png");                   // load background texture
    Texture2D pressEnterToPlay = LoadTexture("resources/images/UI/pressEnterToPlay.png"); // load press Enter to Play texture
    Texture2D score = LoadTexture("resources/images/UI/score.png");                       // load the scoreboard texture

    float const scoreScale = 0.28; // initialise a variable for score texture scale
    float const enterScale = 0.7;  // initialise a variable for enter texture scale

    pressEnterToPlay.width = pressEnterToPlay.width * enterScale;   // calculate the scale for the Enter to Play texture width
    pressEnterToPlay.height = pressEnterToPlay.height * enterScale; // calculate the scale for the Enter to Play texture height

    score.width = score.width * scoreScale;   // calculate the scale for the scoreboard texture width
    score.height = score.height * scoreScale; // calculate the scale for the scoreboard texture height

    Rectangle bg_SourceRec = {}; // define background source rectangle but dont initialise a value
    Rectangle bg_DestRec = {};   // define background destination rectangle but dont initialise a value

    bg_SourceRec.x = 0;                    // background source rectangle position on x
    bg_SourceRec.y = 0;                    // background source rectangle position on y
    bg_SourceRec.width = bg_image.width;   // width of the background source rectangle (in this case we use the original imported texture width)
    bg_SourceRec.height = bg_image.height; // height of the background source rectangle (in this case we use the original imported texture height)

    bg_DestRec.x = 0;                 // background destination rectangle position on x
    bg_DestRec.y = 0;                 // background destination rectangle position on y
    bg_DestRec.width = screenWidth;   // width of the background destination rectangle (in this case we set it to the size of the screen width)
    bg_DestRec.height = screenHeight; // height of the background destination rectangle (in this case we set it to the size of the screen height)

    // Load spritesheets for each animation (there is no spritesheet for the hide animation as when we switch to this, we dont want any sprite to show e.g. this is the method we use in the main menu to hide the character sprite)
    Texture2D santa_Idle = LoadTexture("resources/sprites/idle_spritesheet_single.png");
    Texture2D santa_Run = LoadTexture("resources/sprites/run_spritesheet.png");
    Texture2D santa_Jump = LoadTexture("resources/sprites/jump_spritesheet_single.png");
    Texture2D santa_Dead = LoadTexture("resources/sprites/dead_spritesheet.png");

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Variables
    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------
    int numFramesIdle = 1;  // Declare how many frames will be used in the Idle spritesheet, in this case the spritesheet actually has 16 frames but we will only use 1 for simplicity during Idle state
    int numFramesRun = 11;  // Declare how many frames will be used in the Run spritesheet, in this case the spritesheet has 11 frames and as this is a primary animation running continuosly as the game begins, we will use all 11 frames
    int numFramesJump = 1;  // Declare how many frames will be used in the Jump spritesheet, in this case the spritesheet has 16 frames but we will only use 1 as the spritesheet is not 100% accurate and leads to out of sync animation. 1 frame is sufficent to demonstrate a change in animation for each jump
    int numFramesDead = 10; // Declare how many frames will be used in the Dead spritesheet, in this case the spritesheet has 10 frames and we will use all 10 frames. To ensure smooth animation when the player dies, we will use all 10 frames

    float idle_FrameWidth = (float)(santa_Idle.width / numFramesIdle); // cast all to a float as that is what is expected from the variable. We divide the width of the texture by the number of frames on that spritesheet to get 1 individual sprite from the sheet
    float run_FrameWidth = (float)(santa_Run.width / numFramesRun);    // cast all to a float as that is what is expected from the variable. We divide the width of the texture by the number of frames on that spritesheet to get 1 individual sprite from the sheet
    float jump_FrameWidth = (float)(santa_Jump.width / numFramesJump); // cast all to a float as that is what is expected from the variable. We divide the width of the texture by the number of frames on that spritesheet to get 1 individual sprite from the sheet
    float dead_FrameWidth = (float)(santa_Dead.width / numFramesDead); // cast all to a float as that is what is expected from the variable. We divide the width of the texture by the number of frames on that spritesheet to get 1 individual sprite from the sheet

    // as there is only 1 row on each spritesheet, to get the height of each individual sprite, we can use the original texture height and do not need to divide by the number of columns. This applies to all spritesheets. We could use the parameter for height without actually creating a variable but to keep everything clean and similar to the width, we created a similar type. Additionally, this prevents hardcoding values later in other sections of the code, we can just make an adjustment to this line to affect every other line it is used on
    float idle_FrameHeight = santa_Idle.height;
    float run_FrameHeight = santa_Run.height;
    float jump_FrameHeight = santa_Jump.height;
    float dead_FrameHeight = santa_Dead.height;

    int currentFrame = 0;         // create a variable for currentFrame and initialise to 0
    float frameCounter = 0.f;     // create a variable for frameCounter and intialise to 0
    float frameTime = 1.f / 15.f; // create a variable for frameTime (this is used to calculate the number of frames the animation will have i.e. rate of anim / speed up or slow down)
    int blinkFrames = 0;          // create a variable for blinkFrames (this will be used in the Pause screen to create a blinking effect of the pause text on screen by blinkFrames++)

    float const jumpSpeedUp = 800.f;   // initialise the speed for jumping up to 800
    float const jumpSpeedDown = 500.f; // initialise the fall down speed too 500
    // i.e. faster to jump up than fall down

    bool known[2];    // boolean array for known (this will be used in the game loop to determine if santa is above a present and when he has passed the present to award points to score)
    known[0] = false; // 1st present is not known
    known[1] = false; // 2nd present is not known

    JumpDirection jumpdirection; // initialise jump direction
    Rectangle tmpRect;           // create a temporary rectangle
    Rectangle tmpRect2;          // create a 2nd temporary rectangle

    // create the sprite destination rectangles. The dest recs are where each sprite will be positioned on screen
    Rectangle idle_DestRec = {screenWidth / 2.f, screenHeight / 2.f, idle_FrameWidth, idle_FrameHeight};
    Rectangle run_DestRec = {screenWidth / 2.f, screenHeight / 2.f, run_FrameWidth, run_FrameHeight};
    Rectangle jump_DestRec = {screenWidth / 2.f, screenHeight / 2.f, jump_FrameWidth, jump_FrameHeight};
    Rectangle dead_DestRec = {screenWidth / 2.f, screenHeight / 2.f, dead_FrameWidth, dead_FrameHeight};

    // source rectangles for each animation
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

    // position vectors for the centre of each sprite for each animation
    Vector2 santa_IdleCentre = {(idle_FrameWidth / 2), (idle_FrameHeight / 2)};
    Vector2 santa_RunCentre = {(run_FrameWidth / 2), (run_FrameHeight / 2)};
    Vector2 santa_JumpCentre = {(jump_FrameWidth / 2), (jump_FrameHeight / 2)};
    Vector2 santa_DeadCentre = {(dead_FrameWidth / 2), (dead_FrameHeight / 2)};

    SetTargetFPS(144); // Set the target frames per second to 144FPS
    //-----------------------------------------------------------------------------------------------------------------------

    // Main Game loop to keep the program active/repeating until all conditions have been met as defined within
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //-------------------------------------------------------------------------------------------------------------------
        // Update Variables
        //-------------------------------------------------------------------------------------------------------------------
        UpdateMusicStream(footsteps_sound); // initialise the music stream by using the Raylib function UpdateMusicStream(); this allows this sound to be utilised within the game loop and ensures the music buffer is updated
        deltaTime = GetFrameTime();         // define deltaTime as GetFrameTime() which is to get the time in seconds for last frame drawn. GetFrameTime() is a built in function of Raylib
        if (animations != Jump && !pause)   // if the animation is not equal to Jump and the game is not paused then execute the condition below
        {
            animations = Run;                 // set animation to Run
            PlayMusicStream(footsteps_sound); // play footsteps sound
        }
        else if (pause)        // if the game is paused then execute the condition below
            animations = Idle; // set animation to Idle

        if (gameOver) // if the game is over, execute condition below
        {
            animations = Dead; // set animation to dead
        }
        frameCounter += deltaTime; // update frame by adding deltaTime to the counter, this ensures the same frame is presented to the user regardless of monitor or system

        myTimer += deltaTime; // update timer, adding the timer to deltaTime ensures the same value is presented to the user regardless of monitor or system

        //-------------------------------------------------------------------------------------------------------------------
        // Sounds
        //-------------------------------------------------------------------------------------------------------------------
        UpdateMusicStream(main_sound);    // initialise the music stream by using the Raylib function UpdateMusicStream(); this allows this sound to be utilised within the game loop and ensures the music buffer is updated
        UpdateMusicStream(menu_voice);    // initialise the music stream by using the Raylib function UpdateMusicStream(); this allows this sound to be utilised within the game loop and ensures the music buffer is updated
        UpdateMusicStream(xmas_ambience); // initialise the music stream by using the Raylib function UpdateMusicStream(); this allows this sound to be utilised within the game loop and ensures the music buffer is updated

        if (!pause && !gameOver) // if the game is not paused and the game is not over then execute the below condition
        {
            PlayMusicStream(main_sound);    // play the main game sound
            PlayMusicStream(xmas_ambience); // play the amience sound
            // the block below is to create a loop for music while playing the game
            timePlayed = GetMusicTimePlayed(main_sound) / GetMusicTimeLength(main_sound); // measures the length of the track and gets the time played
            if (timePlayed >= 1.0f)                                                       // if the time played is greater than or equal to 1, where 1 is the entire length of the track then execute below condition
                SeekMusicStream(main_sound, 0);                                           // reset the track to the beginning i.e. 0
            StopMusicStream(menu_voice);                                                  // don't play the voiceover music i.e. stop this sound while the game is being played
        }

        if (pause) // if the game is paused, execute the below condition
        {
            PlayMusicStream(menu_voice);       // play the voiceover (menu_voice) sound
            PauseMusicStream(main_sound);      // pause the in game music
            PauseMusicStream(footsteps_sound); // pause the footsteps sound, the reason this is paused over stopped is so it will continue seamlessly when unpaused i.e. not restart
            PauseSound(fall_sound);            // if the game is paused during a collision, the fall sound will be paused. if this was not included the fall sound would play in the pause screen
        }

        if (gameOver) // if the game is over then execute the following conditions
        {
            StopMusicStream(main_sound);      // stop the main_sound playing. The update function at the beginning of the loop will ensure this is reset to 0 on retry
            StopMusicStream(footsteps_sound); // stop the footsteps_sound playing. The update function at the beginning of the loop will ensure this is reset to 0 on retry
            StopMusicStream(xmas_ambience);   // stop the xmas_ambience playing. The update function at the beginning of the loop will ensure this is reset to 0 on retry
            StopSound(fall_sound);            // stop the fall_sound playing. The update function at the beginning of the loop will ensure this is reset to 0 on retry
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Logic (detect user interaction) / key input
        //-------------------------------------------------------------------------------------------------------------------

        if (IsKeyPressed(KEY_P)) // if the P key is presssed execute the following
        {
            if (animations != Dead) // if the animation is not Dead execute the following (prior if statement must be met for this to be executed)
            {
                if (!gameOver) // if the game is not over execute the following condition (all prior if statements of (IsKeyPressed(KEY_P) must be met for this to be executed)
                {
                    pause = !pause; // initilise pause to not paused as this was initially defined as bool pause = true;
                    if (pause)      // if the game is paused execute the following (all prior if statements of (IsKeyPressed(KEY_P) must be met for this to be executed)

                        PauseMusicStream(main_sound);  // pause the main sound
                    PauseMusicStream(footsteps_sound); // pause the footsteps sound
                    PauseSound(fall_sound);            // pause the fall sound
                    animations = Idle;                 // set the animation to Idle
                }
            }
        }

        if (IsKeyPressed(KEY_SPACE)) // if the Space key is presssed execute the following
        {
            if (animations != Dead) // if the animation is not Dead execute the following (prior if statement must be met for this to be executed)
            {
                if (!pause) // if the game is not paused execute the following condition (all prior if statements of (IsKeyPressed(KEY_SPACE) must be met for this to be executed)
                {

                    if (animations != Jump) // if the animation is not Jumping i.e. running. execute the following (prior conditions must be met for this to be executed)
                    {
                        animations = Jump;                // set animation to Jump
                        StopMusicStream(footsteps_sound); // Stop the footsteps sound as the character is no longer running
                        jumpdirection = Up;               // set the direction of the jump to up
                        jump_DestRec.y = run_DestRec.y;   // set jump destination rectangle on Y to run destination rectangle on Y (this prevents a sudden transition to the jump animation i.e. allows for a smoother transition between both animations at the beginning of the game)
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_ENTER)) // if the Enter key is presssed execute the following
        {
            if (retry) // if retry is true (previous condition must be met) execute the following
            {
                for (int i = 0; i < 2; i++) // utilising the presents array, initialise variable i to 0 (initialised only once); boolean(test) expression, if i is less than 2 execute code within for loop {}; increment i by 1 and re-execute test expression until condition is met
                // the below is described on line 55 to 66
                {
                    xPDestRect[i].x = GetRandomValue(200, 1000) + screenWidth;
                    if (i == 1)
                    {
                        do
                        {
                            xPDestRect[i].x = GetRandomValue(200, 1000) + screenWidth;
                        } while (abs(xPDestRect[0].x - xPDestRect[i].x) < 400); // to ensure the absolute value is greater than 400px (distance between presents)
                    }
                }

                gameOver = false;                 // set game over to false
                pause = false;                    // set pause to false
                animations = Run;                 // set animations to Run
                PlayMusicStream(footsteps_sound); // Play footsteps sound
                StopMusicStream(menu_voice);      // play menu voice music
                PlayMusicStream(main_sound);      // play main sound
                PlayMusicStream(xmas_ambience);   // play xmas ambience sound
            }
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Game Logic (compute movement)
        //-------------------------------------------------------------------------------------------------------------------

        // background image scroll
        if (!pause)                                    // if the game is not paused
            bg_SourceRec.x += scrollSpeed * deltaTime; // background source rectangle on X moves/scrolls based on the scroll speed multiplied by deltatime
        else                                           // if game is paused
            blinkFrames++;                             // increment frames continuously for blinkFrames (this will be utilised in the Draw section)

        if (gameOver)                       // if the game is over
            bg_SourceRec.x = 0 * deltaTime; // set background to 0 i.e. stop scrolling

        if (bg_SourceRec.x >= bg_image.width) // if background source rectangle is greater / equal to background image texture width
            bg_SourceRec.x = 0;               // set background to 0 i.e. stop scrolling

        // scrolling of the xmas presents
        if (!gameOver) // if the game is not over
        {
            if (!pause) // if the game is not paused
            {
                for (int i = 0; i < 2; i++)
                {
                    initPresents(i); // call function declared on line 55 (rather than rewriting this function here, we can just call it to keep everything clean and it can be called as many times as necessary)

                    if (animations != Jump)     // if animation is not jumping
                        tmpRect = run_DestRec;  // create a temporary rectangle and set as the run destination rectangle (the temporary rectangle can be used multiple times to set several varying animations to the same rectangle is required)
                    else                        // if animation is jumping
                        tmpRect = jump_DestRec; // set the jump destination rectangle to the temporary rectangle
                    if (CheckCollisionCircles(Vector2{tmpRect.x, tmpRect.y}, tmpRect.width / 4,
                                              Vector2{xPDestRect[i].x + xPDestRect[i].width / 2, xPDestRect[i].y + xPDestRect[i].height / 2}, xPDestRect[i].width / 2)) // check collisions between 2 circles, we used circles here as the collision was more accurate than the rectangles method.
                    {
                        xPDestRect[i].x = GetRandomValue(200, 1000) + screenWidth; // reset the position of the presents off screen on the right when collision occurs

                        PlaySoundMulti(fall_sound);       // play fall sound
                        PlaySoundMulti(die_sound);        // play sound when player dies
                        StopMusicStream(footsteps_sound); // stop the footsteps sound as player is dead
                        animations = Dead;                // set animation to dead
                        gameOver = true;                  // game over is now set to true
                        pause = true;                     // pause is set to true
                        playerScore = 0;                  // player score is reset to 0
                        std::cout << "Game Over\n";       // debug game over output when collision occurs
                    }

                    if (animations == Jump) // if animation is jumping
                    {
                        // the collision check here is to know when to award the player points to the scoreboard. We need to detect when the player is over a present and when they have passed it.
                        tmpRect2 = jump_DestRec;                                             // create a second temporary rectangle, set it to the jump destination rectangle
                        tmpRect2.width *= 0.2f;                                              // scale the temporary rectangle down so the collision is more accurate
                        tmpRect2.height = ground - jump_DestRec.y + jump_DestRec.height / 2; // As jump height changes in relation to ground, the temp rect adjusts its height from ground
                        if (CheckCollisionCircleRec(Vector2{xPDestRect[i].x + xPDestRect[i].width / 2, xPDestRect[i].y + xPDestRect[i].height / 2}, xPDestRect[i].width / 2,
                                                    Rectangle{tmpRect2.x, tmpRect2.y, tmpRect2.width, tmpRect2.height})) // check collision between circle and rectangle
                        {
                            if (!known[i]) // If there is a collision ocurring per above and the presents are not known (as we are still in the block for "if (!gameOver)", we can utilise the array, using i for both presents)
                            {
                                known[i] = true;    // set presents known to true
                                playerScore += 100; // award the player +100 points for each collision
                                PlaySound(points);  // play sound when points are awarded
                            }
                        }
                        else
                            known[i] = false; // if there is no collision happening set the presents to unknown. This loop re-executes while the game is running
                                              // StopMusicStream(points);
                    }
                }
            }
        }

        if (animations == Jump) // if the animation is jumping
        {
            if (jumpdirection == Up) // check if the jump direction is up
            {
                if (jump_DestRec.y <= maxJumpHeight) // if the jump has reached max height
                    jumpdirection = Down;            // change the jump direction to down
                else                                 // if max jump height has not been reached then execute the following
                {
                    jump_DestRec.y = jump_DestRec.y - jumpSpeedUp * deltaTime; // continue jumping UP at a specific speed
                }
            }
            else // if the animation is not jumping
            {
                if (jump_DestRec.y >= ground) // check if the jump rectangle is on the ground
                    animations = Run;         // set the animation to Run
                else                          // if the jump rectangle is still no on the ground
                {
                    jump_DestRec.y = jump_DestRec.y + jumpSpeedDown * deltaTime; // continue jumping DOWN (falling down) at a specific speed
                }
            }
        }

        //-------------------------------------------------------------------------------------------------------------------
        // Switch Animations (Logic to update animation) / State Machine
        //-------------------------------------------------------------------------------------------------------------------
        switch (animations) // switch logic, all animation state conditions will be tested within each execution of the main game loop
        {
        case Animations::Idle:             // previously declared within the Animations enumeration, Idle state is updated here
            if (frameCounter >= frameTime) //  to ensure the Idle animation is executed at the correct FPS
            {
                frameCounter = 0.f; // initial frame presented to the player
                currentFrame++;     // increment frames by 1 and execute below condition in the loop, this will be done continuously until the condition is met

                if (currentFrame >= numFramesIdle) // per above, frames are incremented by 1 until this condition is met: if the current frame is >= numFramesIdle, reset frame to 1 per below
                {
                    currentFrame = 1; // reset frames to to this value, 1. The loop will continue to run until the condition is met again or the state is changed
                }
            }

            objectRectIdle.x = idle_FrameWidth * currentFrame; // The Idle source rectangle position on X, changes the position for each frame based on the current frame
            objectRectIdle.y = 0;                              // source rectangle position Y to 0
            objectRectIdle.width = idle_FrameWidth;
            idle_DestRec.y = ground; // set the destination rectangle position on the ground
            break;                   // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Run:              // previously declared within the Animations enumeration, Idle state is updated here
            if (frameCounter >= frameTime) //  to ensure the Run animation is executed at the correct FPS
            {
                frameCounter = 0.f; // initial frame presented to the player
                currentFrame++;     // increment frames by 1 and execute below condition in the loop, this will be done continuously until the condition is met

                if (currentFrame >= numFramesRun) // per above, frames are incremented by 1 until this condition is met: if the current frame is >= numFramesRun, reset frame to 1 per below
                {
                    currentFrame = 1; // reset frames to to this value, 1. The loop will continue to run until the condition is met again or the state is changed
                }
            }

            objectRectRun.x = run_FrameWidth * currentFrame; // The Run source rectangle position on X, changes the position for each frame based on the current frame
            objectRectRun.y = 0;                             // source rectangle position Y to 0
            objectRectRun.width = run_FrameWidth;
            run_DestRec.y = ground; // set the destination rectangle position on the ground
            break;                  // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Jump:             // previously declared within the Animations enumeration, Jump state is updated here
            if (frameCounter >= frameTime) //  to ensure the Jump animation is executed at the correct FPS
            {
                frameCounter = 0.f; // initial frame presented to the player
                currentFrame++;     // increment frames by 1 and execute below condition in the loop, this will be done continuously until the condition is met

                if (currentFrame >= numFramesJump) // per above, frames are incremented by 1 until this condition is met: if the current frame is >= numFramesJump, reset frame to 1 per below
                {
                    currentFrame = 1; // reset frames to to this value, 1. The loop will continue to run until the condition is met again or the state is changed
                }
            }

            objectRectJump.x = jump_FrameWidth * currentFrame; // The Jump source rectangle position on X, changes the position for each frame based on the current frame
            objectRectJump.y = 0;                              // source rectangle position Y to 0
            objectRectJump.width = jump_FrameWidth;
            break; // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Dead:             // previously declared within the Animations enumeration, Dead state is updated here
            if (frameCounter >= frameTime) //  to ensure the Dead animation is executed at the correct FPS
            {
                frameCounter = 0.f; // initial frame presented to the player
                currentFrame++;     // increment frames by 1 and execute below condition in the loop, this will be done continuously until the condition is met

                if (currentFrame >= numFramesDead) // per above, frames are incremented by 1 until this condition is met: if the current frame is >= numFramesDead, stop the animation per below condition
                {
                    currentFrame--; // do animation once and stop
                }
            }

            objectRectDead.x = dead_FrameWidth * currentFrame; // The Dead source rectangle position on X, changes the position for each frame based on the current frame
            objectRectDead.y = 0;                              // source rectangle position Y to 0
            objectRectDead.width = dead_FrameWidth;
            dead_DestRec.y = ground; // set the destination rectangle position on the ground
            break;                   // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Hide: // previously declared within the Animations enumeration, Hide state is updated here
            break;             // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)
        }

        //----------------------------------------------------------------------------------------------------------------------------------------------
        // Start Drawing
        //----------------------------------------------------------------------------------------------------------------------------------------------
        BeginDrawing();            // setup canvas to begin drawing
        ClearBackground(RAYWHITE); // clear canvas and paint it ray white i.e. clear white

        // Draw background image
        DrawTexturePro(bg_image, bg_SourceRec, bg_DestRec, Vector2Zero(), 0, WHITE);

        DrawFPS(1150, 10); // draw FPS on top right of screen

        // constant characters - each has its own unique string of text
        const char *msg1 = {"Naughty List"};
        const char *msg2 = {"by Rob N"};
        const char *msg3 = {"You Died! Press [ENTER] to Try Again"};
        const char *msg4 = {"Game Paused"};
        const char *msg5 = {"Press [P] or [ENTER] to Continue"};

        // measure the width of each string from the const char msg above
        const int textWidth_SnowforSanta = MeasureTextEx(SnowforSanta, msg1, 50, 15).x;  //(font type, message, font size, font spacing)
        const int textWidth_coolvetica_rg = MeasureTextEx(coolvetica_rg, msg2, 22, 2).x; //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta1 = MeasureTextEx(SnowforSanta, msg3, 40, 5).x;  //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta2 = MeasureTextEx(SnowforSanta, msg4, 46, 5).x;  //(font type, message, font size, font spacing)
        const int textWidth_SnowforSanta3 = MeasureTextEx(SnowforSanta, msg5, 40, 5).x;  //(font type, message, font size, font spacing)

        // Draw Xmas presents
        if (!gameOver) // if the game is not over
        {
            if (!pause) // if the game is not paused
            // Draw both Xmas presents on screen and change between each type continuously (2 present types - red and blue)
            {
                for (int i = 0; i < 2; i++)
                    DrawTexturePro(xmasPresent[i], xPSourceRect[i], xPDestRect[i], Vector2Zero(), 0, WHITE);
            }
        }

        if (!gameOver && !pause) // if the game is not over and is not paused
        {
            DrawTexture(score, 160 - score.width / 2, 120 - score.height / 2, RAYWHITE); // draw the scoreboard texture to the top left of screen
            DrawText(TextFormat("%4i", playerScore), 127, 130, 30, DARKBLUE);            // draw the score within the scorboard texture
            // DrawRectangle(tmpRect2.x, tmpRect2.y, tmpRect2.width, tmpRect2.height, RED); //Debug collision rectangle for jumping over presents (score points on collision)
        }

        if (gameOver) // if the game is over
        {
            DrawTextEx(SnowforSanta, msg3, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta1 / (float)2), screenHeight - screenHeight / 2}, 40, 5, RED); // draw text on screen "You Died! Press [ENTER] to Try Again"
        }

        if (pause && !gameOver && !bg_SourceRec.x == 0) // if game is paused and not over and the background is not at the intial starting position = 0 (starts at 0 but changes as it begins to scroll)
        {
            DrawTextEx(SnowforSanta, msg5, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta3 / (float)2), screenHeight - screenHeight / 2}, 40, 5, DARKGREEN); // draw text "Press [P] or [ENTER] to Continue"
            // only draw blinking "Game Paused" text in centre of screen when the game is paused
            if ((blinkFrames / 30) % 3)
            {
                DrawTextEx(SnowforSanta, msg4, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta2 / (float)2), screenHeight - screenHeight / 2 - 100}, 46, 5, ColorFromHSV(0, 100, 53));
            }
        }

        if (pause && !gameOver && bg_SourceRec.x == 0) // if game is paused and not over and the background texture is at the initial starting position = 0
        {
            DrawTexture(pressEnterToPlay, screenWidth / 2 - pressEnterToPlay.width / 2, screenHeight / 2 - pressEnterToPlay.height / 2 + 30, WHITE); // draw "press enter to play" texture on screen
            animations = Hide;                                                                                                                       // Hide the character sprite
        }

        // Testing a timer function to display text at a specific interval
        // if ((int)myTimer > 3)
        //{
        //    DrawText("TEST", screenWidth / 2, screenHeight / 2, 30, PURPLE);
        //   if (myTimer > 6)
        //       myTimer = 0;
        //}

        // Draw lines in centre of screen for position reference
        // DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, RED);
        // DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, YELLOW);

        switch (animations) // dependent on the switch logic, all animation states will be drawn individially per that logic
        {
        case Animations::Idle:                                                                    // Idle animation state
            DrawTexturePro(santa_Idle, objectRectIdle, idle_DestRec, santa_IdleCentre, 0, WHITE); // draw the Idle animation texture on screen
            break;                                                                                // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Run:                                                                 // Run animation state
            DrawTexturePro(santa_Run, objectRectRun, run_DestRec, santa_RunCentre, 0, WHITE); // draw the Run animation texture on screen
            break;                                                                            // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Jump:                                                                    // Jump animation state
            DrawTexturePro(santa_Jump, objectRectJump, jump_DestRec, santa_JumpCentre, 0, WHITE); // draw the Jump animation texture on screen
            break;                                                                                // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Dead:                                                                    // Dead animation state
            DrawTexturePro(santa_Dead, objectRectDead, dead_DestRec, santa_DeadCentre, 0, WHITE); // draw the Dead animation texture on screen
            break;                                                                                // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)

        case Animations::Hide: // Hide animation state
            break;             // break statement - ensures this particular animation state is stopped before moving to another state within the switch statement (state machine)
        }

        DrawTextEx(SnowforSanta, msg1, (Vector2){screenWidth / (float)2 - (textWidth_SnowforSanta / (float)2), screenHeight - screenHeight + 20}, 50, 15, GetColor(0x052c46ff)); // Draw text on top centre of screen "Naughty List"
        DrawTextEx(coolvetica_rg, msg2, (Vector2){screenWidth / (float)2 - (textWidth_coolvetica_rg / (float)2), screenHeight - screenHeight + 70}, 22, 2, WHITE);               // Draw text on top centre of screen below msg1 "by Rob N"

        EndDrawing(); // end drawing on canvas
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    // De-Initilisation
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    // unload textures from memory
    UnloadTexture(santa_Idle);
    UnloadTexture(santa_Run);
    UnloadTexture(santa_Jump);
    UnloadTexture(santa_Dead);
    UnloadTexture(bg_image);
    UnloadTexture(xmasPresent[0]);
    UnloadTexture(xmasPresent[1]);
    UnloadTexture(pressEnterToPlay);
    UnloadTexture(score);

    // unload fonts from memory
    UnloadFont(SnowforSanta);
    UnloadFont(coolvetica_rg);

    // unload music and sounds from memory
    UnloadMusicStream(main_sound);
    UnloadMusicStream(menu_voice);
    UnloadMusicStream(footsteps_sound);
    UnloadMusicStream(xmas_ambience);
    UnloadSound(fall_sound);
    UnloadSound(die_sound);
    UnloadSound(points);

    CloseWindow(); // Close window

    //-------------------------------------------------------------------------------------------------------------------------------------------------

    return 0; // program exit and terminates
}