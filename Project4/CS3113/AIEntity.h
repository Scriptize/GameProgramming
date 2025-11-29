#ifndef AIENTITY_H
#define AIENTITY_H
#include "Entity.h"

// Unique name to avoid clashing with existing AIType
enum EnemyBehavior { ENEMY_WANDERER, ENEMY_FOLLOWER, ENEMY_FLYER };

class AIEntity : public Entity {
private:
    EnemyBehavior mBehavior;
    float mSpeed;
    float mTimer;

public:
    AIEntity(Vector2 pos, Vector2 scale, const char *texture,
             EnemyBehavior behavior, float speed)
        : Entity(pos, scale, texture, SINGLE, {1,1}, {}, NPC),
          mBehavior(behavior), mSpeed(speed), mTimer(0.0f) {}

    void updateAI(float dt, Entity *player) {
        mTimer += dt;
        Vector2 pos = getPosition();

        switch (mBehavior) {
            case ENEMY_WANDERER:
                pos.x += sinf(mTimer * 2.0f) * mSpeed * dt;
                break;

            case ENEMY_FOLLOWER:
                if (player) {
                    float px = player->getPosition().x;
                    pos.x += (px > pos.x ? mSpeed : -mSpeed) * dt;
                }
                break;

            case ENEMY_FLYER:
                pos.x += cosf(mTimer * 2.0f) * mSpeed * 0.5f * dt;
                pos.y += sinf(mTimer * 3.0f) * mSpeed * 0.5f * dt;
                break;
        }

        setPosition(pos);
    }

    Rectangle getAABB() const {
        Vector2 pos = getPosition();
        Vector2 scale = getScale();
        return {
            pos.x - scale.x / 2.0f,
            pos.y - scale.y / 2.0f,
            scale.x,
            scale.y
        };
    }
};
#endif
