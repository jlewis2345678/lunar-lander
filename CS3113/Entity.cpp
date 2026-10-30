#include "Entity.h"

#include <cmath>

Entity::Entity()
    : mPosition{0.0f, 0.0f}, mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
      mScale{DEFAULT_SIZE, DEFAULT_SIZE},
      mColliderDimensions{DEFAULT_SIZE, DEFAULT_SIZE}, mTexture{}, mAngle{0.0f},
      mEntityType{PLATFORM}, mIsActive{true}, mIsCollidingBottom{false}
{
}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
    EntityType entityType)
    : mPosition{position}, mVelocity{0.0f, 0.0f}, mAcceleration{0.0f, 0.0f},
      mScale{scale}, mColliderDimensions{scale},
      mTexture{LoadTexture(textureFilepath)}, mAngle{0.0f},
      mEntityType{entityType}, mIsActive{true}, mIsCollidingBottom{false}
{
}

Entity::~Entity()
{
    UnloadTexture(mTexture);
}

bool Entity::isColliding(const Entity *other) const
{
    if (!other->isActive() || other == this)
        return false;

    float xDistance =
        std::fabs(mPosition.x - other->mPosition.x) -
        (mColliderDimensions.x + other->mColliderDimensions.x) / 2.0f;
    float yDistance =
        std::fabs(mPosition.y - other->mPosition.y) -
        (mColliderDimensions.y + other->mColliderDimensions.y) / 2.0f;

    return xDistance <= 0.0f && yDistance <= 0.0f;
}

void Entity::resolveVerticalCollisions(Entity *collidables, int collidableCount)
{
    for (int i = 0; i < collidableCount; i++)
    {
        Entity *collidable = &collidables[i];

        if (!isColliding(collidable))
            continue;

        float yDistance = std::fabs(mPosition.y - collidable->mPosition.y);
        float yOverlap = std::fabs(yDistance - mColliderDimensions.y / 2.0f -
                                   collidable->mColliderDimensions.y / 2.0f);

        if (mVelocity.y > 0.0f)
        {
            mPosition.y -= yOverlap;
            mVelocity.y = 0.0f;
            mIsCollidingBottom = true;
        }
        else if (mVelocity.y < 0.0f)
        {
            mPosition.y += yOverlap;
            mVelocity.y = 0.0f;
        }
    }
}

void Entity::resolveHorizontalCollisions(
    Entity *collidables, int collidableCount)
{
    for (int i = 0; i < collidableCount; i++)
    {
        Entity *collidable = &collidables[i];

        if (!isColliding(collidable))
            continue;

        float yDistance = std::fabs(mPosition.y - collidable->mPosition.y);
        float yOverlap = std::fabs(yDistance - mColliderDimensions.y / 2.0f -
                                   collidable->mColliderDimensions.y / 2.0f);

        if (yOverlap < COLLISION_THRESHOLD)
            continue;

        float xDistance = std::fabs(mPosition.x - collidable->mPosition.x);
        float xOverlap = std::fabs(xDistance - mColliderDimensions.x / 2.0f -
                                   collidable->mColliderDimensions.x / 2.0f);

        if (mVelocity.x > 0.0f)
            mPosition.x -= xOverlap;
        else if (mVelocity.x < 0.0f)
            mPosition.x += xOverlap;

        mVelocity.x = 0.0f;
    }
}

void Entity::update(float deltaTime, Entity *collidables, int collidableCount)
{
    if (!mIsActive)
        return;

    mIsCollidingBottom = false;
    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.y += mVelocity.y * deltaTime;
    resolveVerticalCollisions(collidables, collidableCount);

    mPosition.x += mVelocity.x * deltaTime;
    resolveHorizontalCollisions(collidables, collidableCount);
}

void Entity::render() const
{
    if (!mIsActive)
        return;

    renderTexture(mTexture, 0, 1);
}

void Entity::renderTexture(Texture2D texture, int frame, int frameCount) const
{
    if (!mIsActive || frameCount <= 0)
        return;

    Rectangle destination{mPosition.x, mPosition.y, mScale.x, mScale.y};
    Vector2 origin{mScale.x / 2.0f, mScale.y / 2.0f};
    float frameWidth = static_cast<float>(texture.width) / frameCount;
    Rectangle source{frameWidth * frame, 0.0f, frameWidth,
        static_cast<float>(texture.height)};

    DrawTexturePro(texture, source, destination, origin, mAngle, WHITE);
}
