#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

enum EntityType
{
    PLAYER,
    PLATFORM,
    HAZARD,
    GOAL
};

class Entity
{
  private:
    Vector2 mPosition;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    Vector2 mScale;
    Vector2 mColliderDimensions;
    Texture2D mTexture;
    float mAngle;
    EntityType mEntityType;
    bool mIsActive;
    bool mIsCollidingBottom;

    bool isColliding(const Entity *other) const;
    void resolveVerticalCollisions(Entity *collidables, int collidableCount);
    void resolveHorizontalCollisions(Entity *collidables, int collidableCount);

  public:
    static constexpr int DEFAULT_SIZE = 50;
    static constexpr float COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
        EntityType entityType);
    ~Entity();

    void update(float deltaTime, Entity *collidables, int collidableCount);
    void render() const;
    bool isTouching(const Entity *other) const
    {
        return isColliding(other);
    }

    Vector2 getPosition() const
    {
        return mPosition;
    }
    Vector2 getVelocity() const
    {
        return mVelocity;
    }
    Vector2 getAcceleration() const
    {
        return mAcceleration;
    }
    Vector2 getScale() const
    {
        return mScale;
    }
    Vector2 getColliderDimensions() const
    {
        return mColliderDimensions;
    }
    EntityType getEntityType() const
    {
        return mEntityType;
    }
    bool isActive() const
    {
        return mIsActive;
    }
    bool isCollidingBottom() const
    {
        return mIsCollidingBottom;
    }

    void setPosition(Vector2 position)
    {
        mPosition = position;
    }
    void setVelocity(Vector2 velocity)
    {
        mVelocity = velocity;
    }
    void setAcceleration(Vector2 acceleration)
    {
        mAcceleration = acceleration;
    }
    void setScale(Vector2 scale)
    {
        mScale = scale;
    }
    void setAngle(float angle)
    {
        mAngle = angle;
    }
    void setColliderDimensions(Vector2 dimensions)
    {
        mColliderDimensions = dimensions;
    }
    void setEntityType(EntityType entityType)
    {
        mEntityType = entityType;
    }
    void setTexture(const char *textureFilepath)
    {
        mTexture = LoadTexture(textureFilepath);
    }
    void deactivate()
    {
        mIsActive = false;
    }
};

#endif // ENTITY_H
