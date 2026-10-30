#include "CS3113/Entity.h"

#include <algorithm>

/**
 * Author: Janae Lewis
 * Assignment: Lunar Lander!
 * Date due: 10/30/2025
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 **/

enum AppStatus
{
    TERMINATED,
    RUNNING
};

struct GameState
{
    Entity *player;
    Entity *platforms;
    Entity *movingPlatform;
};

enum GameStatus
{
    PLAYING,
    MISSION_FAILED,
    MISSION_ACCOMPLISHED
};

constexpr int SCREEN_WIDTH = 1000;
constexpr int SCREEN_HEIGHT = 600;
constexpr int FPS = 120;
constexpr int NUMBER_OF_PLATFORMS = 3;
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr float GRAVITY = 15.0f;
constexpr float HORIZONTAL_ACCELERATION = 80.0f;
constexpr float MAX_FUEL = 100.0f;
constexpr float FUEL_COST_PER_INPUT = 0.4f;
constexpr int MOVE_ANIMATION_FRAMES = 6;
constexpr float MOVE_ANIMATION_SPEED = 12.0f;

AppStatus gAppStatus = RUNNING;
GameStatus gGameStatus = PLAYING;
float gPreviousTime = 0.0f;
float gTimeAccumulator = 0.0f;
float gFuel = MAX_FUEL;
bool gIsAccelerating = false;
Texture2D gMoveTexture{};
Texture2D gBackgroundTexture{};
GameState gState;

void restartGame()
{
    gGameStatus = PLAYING;
    gFuel = MAX_FUEL;
    gIsAccelerating = false;
    gTimeAccumulator = 0.0f;
    gPreviousTime = static_cast<float>(GetTime());

    gState.player->setPosition({100.0f, 100.0f});
    gState.player->setVelocity({0.0f, 0.0f});
    gState.player->setAcceleration({0.0f, GRAVITY});
    gState.movingPlatform->setPosition({500.0f, 300.0f});
    gState.movingPlatform->setVelocity({70.0f, 0.0f});
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");

    gState.player = new Entity(
        {100.0f, 100.0f}, {50.0f, 50.0f}, "assets/game/idle.png", PLAYER);
    gState.player->setAngle(-90.0f);
    gState.player->setColliderDimensions({50.0f, 50.0f});
    gState.player->setAcceleration({0.0f, GRAVITY});
    gMoveTexture = LoadTexture("assets/game/move.png");
    gBackgroundTexture = LoadTexture("assets/game/space_bg.png");

    // these are the platforms the ship can land on
    gState.platforms = new Entity[NUMBER_OF_PLATFORMS];

    gState.platforms[0].setTexture("assets/game/pad_hazard.png");
    gState.platforms[0].setEntityType(HAZARD);
    gState.platforms[0].setScale({220.0f, 110.0f});
    gState.platforms[0].setColliderDimensions({200.0f, 80.0f});
    gState.platforms[0].setPosition({180.0f, SCREEN_HEIGHT - 60.0f});

    gState.platforms[1].setTexture("assets/game/pad_safe.png");
    gState.platforms[1].setEntityType(PLATFORM);
    gState.platforms[1].setScale({220.0f, 90.0f});
    gState.platforms[1].setColliderDimensions({200.0f, 80.0f});
    gState.platforms[1].setPosition({500.0f, SCREEN_HEIGHT - 50.0f});

    gState.platforms[2].setTexture("assets/game/pad_goal.png");
    gState.platforms[2].setEntityType(GOAL);
    gState.platforms[2].setScale({220.0f, 90.0f});
    gState.platforms[2].setColliderDimensions({200.0f, 80.0f});
    gState.platforms[2].setPosition({820.0f, SCREEN_HEIGHT - 50.0f});

    // platform moves by itself and is also a hazard
    gState.movingPlatform = new Entity({500.0f, 300.0f}, {220.0f, 110.0f},
        "assets/game/pad_hazard.png", HAZARD);
    gState.movingPlatform->setColliderDimensions({200.0f, 80.0f});
    gState.movingPlatform->setVelocity({70.0f, 0.0f});

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;

    if (gGameStatus != PLAYING)
    {
        if (IsKeyPressed(KEY_R))
            restartGame();
        return;
    }

    if (gGameStatus != PLAYING)
        return;

    // reset accel every frame or it keeps speeding up forever
    gState.player->setAcceleration({0.0f, GRAVITY});
    gIsAccelerating = false;

    // this changes acceleration
    if (gFuel > 0.0f && (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)))
    {
        gState.player->setAcceleration({-HORIZONTAL_ACCELERATION, GRAVITY});
        gFuel = std::max(0.0f, gFuel - FUEL_COST_PER_INPUT);
        gIsAccelerating = true;
    }
    else if (gFuel > 0.0f && (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)))
    {
        gState.player->setAcceleration({HORIZONTAL_ACCELERATION, GRAVITY});
        gFuel = std::max(0.0f, gFuel - FUEL_COST_PER_INPUT);
        gIsAccelerating = true;
    }
}

void update()
{
    if (gGameStatus != PLAYING)
        return;

    // get delta time and add it to the fixed step accumulator
    float currentTime = static_cast<float>(GetTime());
    float deltaTime = currentTime - gPreviousTime;
    gPreviousTime = currentTime;
    gTimeAccumulator += deltaTime;

    // only update the ship in fixed time steps
    while (gTimeAccumulator >= FIXED_TIMESTEP)
    {
        gState.player->update(
            FIXED_TIMESTEP, gState.platforms, NUMBER_OF_PLATFORMS);
        gState.movingPlatform->update(FIXED_TIMESTEP, nullptr, 0);

        // make the moving platform go back and forth
        if (gState.movingPlatform->getPosition().x > 850.0f)
            gState.movingPlatform->setVelocity({-70.0f, 0.0f});
        else if (gState.movingPlatform->getPosition().x < 150.0f)
            gState.movingPlatform->setVelocity({70.0f, 0.0f});

        // check every platform after the ship moves
        for (int i = 0; i < NUMBER_OF_PLATFORMS; i++)
        {
            // if the ship touches a platform, check what kind it is
            if (gState.player->isTouching(&gState.platforms[i]))
            {
                if (gState.platforms[i].getEntityType() == HAZARD)
                    gGameStatus = MISSION_FAILED;
                else if (gState.platforms[i].getEntityType() == GOAL)
                    gGameStatus = MISSION_ACCOMPLISHED;
            }
        }

        // the moving platform is a hazard too
        if (gState.player->isTouching(gState.movingPlatform))
            gGameStatus = MISSION_FAILED;

        // Missing every platform means the ship falls out of the game world.
        Vector2 playerPosition = gState.player->getPosition();
        Vector2 playerScale = gState.player->getScale();
        if (playerPosition.y - playerScale.y / 2.0f > SCREEN_HEIGHT)
            gGameStatus = MISSION_FAILED;

        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle backgroundSource{0.0f, 0.0f,
        static_cast<float>(gBackgroundTexture.width),
        static_cast<float>(gBackgroundTexture.height)};
    Rectangle backgroundDestination{0.0f, 0.0f,
        static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};
    DrawTexturePro(gBackgroundTexture, backgroundSource, backgroundDestination,
        {0.0f, 0.0f}, 0.0f, WHITE);

    // draw the ship and platforms
    if (gIsAccelerating)
    {
        int frame = static_cast<int>(GetTime() * MOVE_ANIMATION_SPEED) %
                    MOVE_ANIMATION_FRAMES;
        gState.player->renderTexture(
            gMoveTexture, frame, MOVE_ANIMATION_FRAMES);
    }
    else
    {
        gState.player->render();
    }
    for (int i = 0; i < NUMBER_OF_PLATFORMS; i++)
    {
        gState.platforms[i].render();
    }
    gState.movingPlatform->render();
    DrawText(TextFormat("Fuel: %.0f", gFuel), 20, 20, 24, WHITE);

    // show the result and stop the ship after landing
    if (gGameStatus == MISSION_FAILED)
    {
        DrawText("press r to restart", 370, 215, 20, WHITE);
        DrawText("Mission Failed", 350, 250, 40, RED);
    }
    else if (gGameStatus == MISSION_ACCOMPLISHED)
    {
        DrawText("press r to restart", 370, 215, 20, WHITE);
        DrawText("Mission Accomplished", 300, 250, 40, GREEN);
    }

    EndDrawing();
}

void shutdown()
{
    delete gState.player;
    delete[] gState.platforms;
    delete gState.movingPlatform;
    UnloadTexture(gMoveTexture);
    UnloadTexture(gBackgroundTexture);
    CloseWindow();
}

int main()
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
