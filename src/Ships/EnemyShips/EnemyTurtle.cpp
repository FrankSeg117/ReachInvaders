#include "EnemyTurtle.h"

EnemyTurtle::EnemyTurtle(int xpos, int ypos) : EnemyShip(xpos, ypos, 0.5, 20, 150) {
    amIBoss = false;
    enemyShipSprite.load("ShipModels/Phantom.png");
    enemyShipSprite.rotate90(2);
    enemyHitBox = new HitBox(pos, enemyShipSprite.getWidth() * 0.1, enemyShipSprite.getHeight() * 0.08);
}

void EnemyTurtle::update(const ofPoint& playerPos) {
    // Calculate the angle to the player
        ofVec2f toPlayer = playerPos - pos;
        float angleToPlayer = ofRadToDeg(-atan2(toPlayer.x, toPlayer.y));

    // Update ship's orientation
        shipOrientation = angleToPlayer;

    // Move towards the player
        pos += toPlayer.getNormalized() * speed;
        enemyHitBox->box.setPosition(pos.x - 15, pos.y - 10);

    this->shotTimer++;
    // Check if the ship should shoot
        if (shotTimer % 100 == 0) {
            shoot();
            shotTimer = 0; //resets the timer
        }
}

void EnemyTurtle::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofRotateDeg(shipOrientation);
    enemyShipSprite.draw(-32, -32, 60, 60);
    ofPopMatrix();
    
    if(showHitboxes){
        enemyHitBox->draw();
    }
}

void EnemyTurtle::shoot() {
    // Create a new projectile and add it to the list of bullets
    Projectiles p = Projectiles(pos - 10, shipOrientation + 90);
    p.setColors(ofColor::limeGreen, ofColor::white);

    Projectiles q = Projectiles(pos + 10, shipOrientation + 90);
    q.setColors(ofColor::limeGreen, ofColor::white);

    enemyBullets.push_back(p);
    enemyBullets.push_back(q);

    SoundManager::playSong("bulletSound", false);
}