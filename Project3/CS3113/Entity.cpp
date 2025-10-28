#include "Entity.h"
#include "raylib.h"
#include <cmath>


Entity::Entity()
    : mPosition{0, 0}, mMovement{0, 0}, mVelocity{0, 0}, mAcceleration{0, 0},
      mScale{DEFAULT_SIZE, DEFAULT_SIZE}, mColliderDimensions{DEFAULT_SIZE, DEFAULT_SIZE},
      mTexture{}, mOwnsTexture{false}, mTextureType{SINGLE}, mDirection{RIGHT},
      mFrameSpeed{0}, mSpeed{DEFAULT_SPEED}, mAngle{0}, mEntityType{NONE} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath, EntityType entityType)
    : mPosition{position}, mVelocity{0, 0}, mAcceleration{0, 0}, mScale{scale},
      mMovement{0, 0}, mColliderDimensions{scale}, mTexture{LoadTexture(textureFilepath)},
      mOwnsTexture{true}, mTextureType{SINGLE}, mDirection{RIGHT}, mFrameSpeed{0},
      mSpeed{DEFAULT_SPEED}, mAngle{0}, mEntityType{entityType} {}

Entity::Entity(Vector2 position, Vector2 scale, const char *textureFilepath,
               TextureType textureType, Vector2 spriteSheetDimensions,
               std::map<Direction, std::vector<int>> animationAtlas,
               EntityType entityType)
    : mPosition{position}, mVelocity{0, 0}, mAcceleration{0, 0}, mScale{scale},
      mColliderDimensions{scale}, mTexture{LoadTexture(textureFilepath)}, mOwnsTexture{true},
      mTextureType{textureType}, mSpriteSheetDimensions{spriteSheetDimensions},
      mAnimationAtlas{animationAtlas}, mDirection{RIGHT},
      mAnimationIndices{animationAtlas.at(RIGHT)}, mFrameSpeed{DEFAULT_FRAME_SPEED},
      mSpeed{DEFAULT_SPEED}, mAngle{0}, mEntityType{entityType} {}

Entity::~Entity() {
    if (mOwnsTexture && mTexture.id > 0)
        UnloadTexture(mTexture);
}


void Entity::setTexture(Texture2D tex) {
    mTexture = tex;
    mOwnsTexture = false; // shared texture
}


bool Entity::isColliding(Entity *other) const {
    if (!other->isActive() || other == this) return false;
    float xDist = fabs(mPosition.x - other->getPosition().x) -
                  ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDist = fabs(mPosition.y - other->getPosition().y) -
                  ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);
    return (xDist < 0 && yDist < 0);
}


void Entity::update(float deltaTime, Entity *player, Map *map,
                    Entity *collidableEntities, int collisionCheckCount) {
    if (mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.x += mVelocity.x * deltaTime;
    mPosition.y += mVelocity.y * deltaTime;

    // Clamp tiny velocities
    if (fabs(mVelocity.x) < 0.001f) mVelocity.x = 0;
    if (fabs(mVelocity.y) < 0.001f) mVelocity.y = 0;
}


void Entity::render() {
    if (mEntityStatus == INACTIVE) return;

    Rectangle src = {0, 0, (float)mTexture.width, (float)mTexture.height};
    Rectangle dst = {mPosition.x - mScale.x / 2.0f, mPosition.y - mScale.y / 2.0f, mScale.x, mScale.y};

    if (mTexture.id > 0)
        DrawTexturePro(mTexture, src, dst, {0, 0}, 0.0f, WHITE);
    else
        DrawRectangleV({mPosition.x - mScale.x / 2.0f, mPosition.y - mScale.y / 2.0f},
                       mScale, GRAY);
}

void Entity::displayCollider() {
    DrawRectangleLines(
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y,
        GREEN
    );
}
