#ifndef ENTITY_H
#define ENTITY_H

#include "Map.h"

enum Direction    { LEFT, UP, RIGHT, DOWN              }; 
enum EntityStatus { ACTIVE, INACTIVE                   };
enum EntityType   { PLAYER, BLOCK, PLATFORM, NPC, NONE };
enum AIType       { WANDERER, FOLLOWER                 };
enum AIState      { WALKING, IDLE, FOLLOWING           };

class Entity {
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D mTexture;
    bool mOwnsTexture = false;          // ✅ NEW FLAG
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;
    bool mIsJumping = false;
    float mJumpingPower = 0.0f;
    int mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;
    AIType  mAIType;
    AIState mAIState;

    bool isColliding(Entity *other) const;
    void checkCollisionY(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionY(Map *map);
    void checkCollisionX(Entity *collidableEntities, int collisionCheckCount);
    void checkCollisionX(Map *map);

    void resetColliderFlags() {
        mIsCollidingTop = mIsCollidingBottom = mIsCollidingRight = mIsCollidingLeft = false;
    }

    void animate(float deltaTime);
    void AIActivate(Entity *target);
    void AIWander();
    void AIFollow(Entity *target);

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
           TextureType textureType, Vector2 spriteSheetDimensions,
           std::map<Direction, std::vector<int>> animationAtlas,
           EntityType entityType);
    ~Entity();

    void update(float deltaTime, Entity *player, Map *map,
                Entity *collidableEntities, int collisionCheckCount);
    void render();
    void displayCollider();

    void normaliseMovement() { Normalise(&mMovement); }
    void jump() { mIsJumping = true; }
    void activate() { mEntityStatus = ACTIVE; }
    void deactivate() { mEntityStatus = INACTIVE; }

    bool isActive() const { return mEntityStatus == ACTIVE; }
    bool isCollidingTop() const { return mIsCollidingTop; }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    void moveUp()    { mMovement.y = -1; mDirection = UP; }
    void moveDown()  { mMovement.y =  1; mDirection = DOWN; }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT; }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }
    void resetMovement() { mMovement = {0.0f, 0.0f}; }

    // --- Getters ---
    Vector2 getPosition() const { return mPosition; }
    Vector2 getMovement() const { return mMovement; }
    Vector2 getVelocity() const { return mVelocity; }
    Vector2 getAcceleration() const { return mAcceleration; }
    Vector2 getScale() const { return mScale; }
    Vector2 getColliderDimensions() const { return mScale; }
    Texture2D getTexture() const { return mTexture; }
    EntityType getEntityType() const { return mEntityType; }

    // --- Setters ---
    void setPosition(Vector2 newPos) { mPosition = newPos; }
    void setAcceleration(Vector2 newAcc) { mAcceleration = newAcc; }
    void setColliderDimensions(Vector2 newDim) { mColliderDimensions = newDim; }

    void setTexture(const char *textureFilepath) {  // for single-use entities
        if (mOwnsTexture && mTexture.id > 0) UnloadTexture(mTexture);
        mTexture = LoadTexture(textureFilepath);
        mOwnsTexture = true;
    }

    void setTexture(Texture2D tex); // ✅ shared texture overload
};

#endif
