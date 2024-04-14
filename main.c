/*******************************************************************************************
*
*   Ludum Dare 55 Submission by Marco Puig
*
*   Created with raylib 5.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2021-2024 Ramon Santamaria (@raysan5)
*   Copyright (c) 2024 Marco Puig (@KazMaz)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <time.h>
#include <stdlib.h>

// THEME LD55: SUMMONING

/*
Game Idea, top down shooter style game.
Where you are a wizard that summons turrents to fight for you!
 */

// Defines
#define MAX_BULLETS 10
#define MAX_ENEMIES 5

//------------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720; 

    InitWindow(screenWidth, screenHeight, "S-Pull");

    GameScreen currentScreen = LOGO;

    int framesCounter = 0;          // Useful to count frames

    SetTargetFPS(60);               // Set desired fps
    //--------------------------------------------------------------------------------------
    
    // calc rng
    srand(time(NULL));
    int r = rand() % (2 + 1 - 0) + 0; // 2 = max, 0 = min
    
    // globals
    
    // Load Floor Sprite/Texture
    Texture2D floor = LoadTexture("resources/dungeon.png");
    
    // Load Enemy Sprite/Texture
    Texture2D enemySprite = LoadTexture("resources/spike-ball.png");
    
    // Animation variables
    Texture2D spriteSheet = LoadTexture("resources/wizard_idle.png");
    int currentFrame = 0;
    const int maxFrame = 5 * 5; // Total number of frames
    float frameCounter = 0;
    float updateTime = 1.0f / 12.0f; // Update the frame every 1/12 seconds, works well as 60 fps

    // Define the frame rectangle
    int frameWidth = spriteSheet.width / 5; // 5 columns
    int frameHeight = spriteSheet.height / 5; // 5 rows
    
    // initialize player
    Vector2 playerPos = { screenWidth / 2 - frameWidth / 2, screenHeight / 2 - frameHeight / 2 };
    
    // hud info 
    int score = 0;
    int manaCount = 0;
    
    // bullet class... I mean struct
    typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
    } Bullet;
    
    
    // Create 10 (Could say that these are the limit of bullets) - Default Bullets to Off
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    
    // Enemy struct
    typedef struct Enemy {
        Vector2 position;
        bool active;
    } Enemy;

    // Initialize enemies
    Enemy enemies[MAX_ENEMIES];
    
    // Modify the initialization code for enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].position = (Vector2){GetRandomValue(40, screenWidth), GetRandomValue(40, screenHeight)};
        enemies[i].active = true;
    }
    
    // audio
    InitAudioDevice();
    Sound music = LoadSound("resources/wizzy.mp3"); 
    PlaySound(music);
       
    // Main game loop
    while (WindowShouldClose() == false) // as long as window is open
    {
        // Update (LOGIC)
        switch(currentScreen)
        {
            case LOGO:
            {
                framesCounter++; 

                // Wait for 2 seconds
                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
                
            } break;
            case TITLE:
            {
                // TODO: Update TITLE screen variables here!
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                   currentScreen = GAMEPLAY; 
                }
                
            } break;
            case GAMEPLAY:
            { 
               // Core GameLoop func
               manaCount++;
               manaCount %= 6000;
               
               frameCounter += GetFrameTime();
               if (frameCounter >= updateTime) {
                   frameCounter = 0;
                   currentFrame++;

                   if (currentFrame >= maxFrame) {
                       currentFrame = 0;
                   }
               }
               
               // Player Logic
               int speed = 5;
                    
               // Command pattern for input
               if (IsKeyDown(KEY_D)) 
                    playerPos.x += speed;
               if (IsKeyDown(KEY_A)) 
                    playerPos.x -= speed;
               if (IsKeyDown(KEY_W)) 
                    playerPos.y -= speed;
               if (IsKeyDown(KEY_S))
                    playerPos.y += speed;
                
               // Enemies Logic (Follow Player)
               for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].active) {
                        // Simple AI to follow the player
                        enemies[i].position.x = Lerp(enemies[i].position.x, playerPos.x + (50 * r), (0.005 * r) + 0.005);
                        enemies[i].position.y = Lerp(enemies[i].position.y, playerPos.y + (50 * r), (0.005 * r) + 0.005);
                    }
                }
                                
               // Collision Handling   
               for (int i = 0; i < MAX_ENEMIES; i++) {
                   if (CheckCollisionCircles(playerPos, 15, enemies[i].position, 15) == true)
                   {
                       // if the player takes, damage, then they lose 
                        currentScreen = ENDING;
                   } 
               }

                // Player wand shooting logic
                if (IsKeyPressed(KEY_SPACE) && manaCount > 0) {
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (!bullets[i].active) {
                            bullets[i].active = true;
                            bullets[i].position = (Vector2){playerPos.x + 120, playerPos.y + 45};             
                            
                           // fire bullet in direction player is moving
                           if (IsKeyDown(KEY_D)) 
                                bullets[i].velocity = (Vector2){5.0f, 0}; 
                           else if (IsKeyDown(KEY_A)) 
                                 bullets[i].velocity = (Vector2){-5.0f, 0}; 
                           else if (IsKeyDown(KEY_W)) 
                                bullets[i].velocity = (Vector2){0, -5.0f}; 
                           else if (IsKeyDown(KEY_S))
                                bullets[i].velocity = (Vector2){0, 5.0f}; 
                            else
                                bullets[i].velocity = (Vector2){5.0f, 0}; 
                            
                            break; // break since we want to do one bullet at a time
                            // we are doing a for loop because we want to check all the bullets in mem that arent active 
                        }
                    }
                }
                
                // Update bullets pos
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        bullets[i].position = Vector2Add(bullets[i].position, bullets[i].velocity);
                        // Deactivate bullet if it goes off-screen
                        if (bullets[i].position.x < 0 || bullets[i].position.x > screenWidth ||
                            bullets[i].position.y < 0 || bullets[i].position.y > screenHeight) {
                            bullets[i].active = false;
                        }
                    }
                }
                
                // Check bullet collisions with enemies
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (bullets[i].active) {
                        for (int j = 0; j < MAX_ENEMIES; j++) {
                            if (enemies[j].active && CheckCollisionCircles(bullets[i].position, 5, enemies[j].position, 15)) {
                                enemies[j].active = false; // Enemy is "killed"
                                bullets[i].active = false; // Bullet disappears
                                score += 100; // Increase score for hitting an enemy

                                // Respawn enemy at a new random position
                                enemies[j].position = (Vector2){GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight)};
                                enemies[j].active = true;
                            }
                        }
                    }
                }
        
            } break;
            case ENDING:
            {
                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    // reset positions
                    playerPos.x = screenWidth / 2 - frameWidth / 2; 
                    playerPos.y = screenHeight/ 2 - frameHeight / 2;
                    
                    score = 0;
                    manaCount = 0;
                    
                    // go back to menu screen
                    currentScreen = TITLE;
                    
                    // recalc rng
                    r = rand() % (2 + 1 - 0) + 0;
                    
                    // reset enemy pos
                    for (int i = 0; i < MAX_ENEMIES; i++) {
                        enemies[i].position = (Vector2){
                        GetRandomValue(screenWidth - 100, screenWidth),
                        GetRandomValue(screenHeight - 100, screenHeight)};
                        enemies[i].active = true;
                    }
                }
            } break;
            default: break;
        }
        
        // Draw / Render
        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch(currentScreen)
            {
                case LOGO:
                {     
                    // show logos
                    ClearBackground(BLACK);
                    DrawText("Ludum Dare 55", 250, 100, 40, WHITE);
                    DrawText("Created by Marco Puig", 280, 220, 20, WHITE);
                    
                } break;
                case TITLE:
                {
                    // Draw TITLE screen here!
                    ClearBackground(BLUE);
                    DrawText("S-Pull", 280, 20, 60, WHITE);
                    DrawText("Click to Start!", 280, 220, 20, WHITE);

                } break;
                case GAMEPLAY:
                {                
                    // Draw Floor  
                    DrawTextureEx(floor, (Vector2){
                    screenWidth/2 - floor.width*1.7, 
                    screenHeight/2 - floor.height*1.5
                    }, 0.0f, 3.5f, WHITE);
                    
                    // Draw Enemies
                    for (int i = 0; i < MAX_ENEMIES; i++) {
                        if (enemies[i].active) {
                            DrawTextureEx(enemySprite, enemies[i].position, 0.0f, 0.3f, WHITE);
                        }
                    }  
                    
                    // Draw Player
                    
                    // Calculate the position of the rectangle on the spritesheet
                    int frameX = (currentFrame % 4) * frameWidth;
                    int frameY = (currentFrame / 4) * frameHeight;
                    Rectangle frameRec = { frameX, frameY, frameWidth, frameHeight };

                    // Draw the current frame
                    DrawTextureRec(spriteSheet, frameRec, playerPos, WHITE);
                    
                    // Draw bullets
                    for (int i = 0; i < MAX_BULLETS; i++) {
                        if (bullets[i].active) {
                            DrawCircleV(bullets[i].position, 5, PURPLE);
                        }
                    }                    
                   
                    // HUD
                    DrawText(TextFormat("SCORE: %i", score), 20, 20, 40, WHITE);
                    DrawText(TextFormat("Mana: %i", manaCount), 20, 100, 40, BLUE);

                    
                } break;
                case ENDING:
                {
                    const char *possibleEndMsgs[3];
                    possibleEndMsgs[0] = "You Lost!";
                    possibleEndMsgs[1] = "Nice Try!";
                    possibleEndMsgs[2] = "Try Again?";
                    
                    DrawText(possibleEndMsgs[r], 20, 20, 40, BLACK);
                    DrawText(TextFormat("SCORE: %i", score), 120, 100, 20, BLACK); // score
                    DrawText("Click to Restart", 120, 220, 20, BLACK);

                } break;
                default: break;
            }

        EndDrawing();
    }
        

    // De-Initialization

    // Unload all loaded data (textures, fonts, audio) here!
    UnloadSound(music);
    UnloadTexture(spriteSheet);
    UnloadTexture(enemySprite);
    UnloadTexture(floor);
    CloseAudioDevice();

    CloseWindow(); // Close window and OpenGL context
    
    return 0;
}

