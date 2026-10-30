#include "CS3113/Entity.h"

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
constexpr float GRAVITY = 30.0f;
constexpr float HORIZONTAL_ACCELERATION = 80.0f;

AppStatus gAppStatus = RUNNING;
GameStatus gGameStatus = PLAYING;
float gPreviousTime = 0.0f;
float gTimeAccumulator = 0.0f;
GameState gState;

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander");

    gState.player = new Entity(
        {100.0f, 100.0f}, {50.0f, 50.0f}, "assets/game/idle.png", PLAYER);
    gState.player->setAngle(-90.0f);
    gState.player->setColliderDimensions({50.0f, 50.0f});
    gState.player->setAcceleration({0.0f, GRAVITY});

    // these are the platforms the ship can land on
    gState.platforms = new Entity[NUMBER_OF_PLATFORMS];

    gState.platforms[0].setTexture("assets/game/pad_safe.png");
    gState.platforms[0].setEntityType(PLATFORM);
    gState.platforms[0].setScale({220.0f, 90.0f});
    gState.platforms[0].setColliderDimensions({220.0f, 90.0f});
    gState.platforms[0].setPosition({180.0f, SCREEN_HEIGHT - 50.0f});

    gState.platforms[1].setTexture("assets/game/pad_hazard.png");
    gState.platforms[1].setEntityType(HAZARD);
    gState.platforms[1].setScale({220.0f, 90.0f});
    gState.platforms[1].setColliderDimensions({220.0f, 90.0f});
    gState.platforms[1].setPosition({500.0f, SCREEN_HEIGHT - 50.0f});

    gState.platforms[2].setTexture("assets/game/pad_goal.png");
    gState.platforms[2].setEntityType(GOAL);
    gState.platforms[2].setScale({220.0f, 90.0f});
    gState.platforms[2].setColliderDimensions({220.0f, 90.0f});
    gState.platforms[2].setPosition({820.0f, SCREEN_HEIGHT - 50.0f});

    // platform moves by itself and is also a hazard
    gState.movingPlatform = new Entity({500.0f, 300.0f}, {220.0f, 90.0f},
        "assets/game/pad_hazard.png", HAZARD);
    gState.movingPlatform->setColliderDimensions({220.0f, 50.0f});
    gState.movingPlatform->setVelocity({70.0f, 0.0f});

    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose())
        gAppStatus = TERMINATED;

    if (gGameStatus != PLAYING)
        return;

    // reset accel every frame or it keeps speeding up forever
    gState.player->setAcceleration({0.0f, GRAVITY});

    // this changes acceleration
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        gState.player->setAcceleration({-HORIZONTAL_ACCELERATION, GRAVITY});
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        gState.player->setAcceleration({HORIZONTAL_ACCELERATION, GRAVITY});
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

        gTimeAccumulator -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(SKYBLUE);

    // draw the ship and platforms
    gState.player->render();
    for (int i = 0; i < NUMBER_OF_PLATFORMS; i++)
    {
        gState.platforms[i].render();
    }
    gState.movingPlatform->render();

    // show the result and stop the ship after landing
    if (gGameStatus == MISSION_FAILED)
        DrawText("Mission Failed", 350, 250, 40, RED);
    else if (gGameStatus == MISSION_ACCOMPLISHED)
        DrawText("Mission Accomplished", 300, 250, 40, GREEN);

    EndDrawing();
}

void shutdown()
{
    delete gState.player;
    delete[] gState.platforms;
    delete gState.movingPlatform;
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
