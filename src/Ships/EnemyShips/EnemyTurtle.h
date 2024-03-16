#pragma once
#include "enemy.h"

class EnemyTurtle : public EnemyShip {

    public:
        
        EnemyTurtle(int xpos, int ypos);
        void update(const ofPoint& playerPos) override;
        void draw() override;
        void shoot() override;
};