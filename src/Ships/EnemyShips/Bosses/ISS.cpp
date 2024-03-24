#include "ISS.h"

ISS::ISS(int xpos, int ypos, string name) : Boss(xpos, ypos, 0.5, 450, name) {
    enemyShipSprite.load("ShipModels/CovDestroyer.png");
    enemyHitBox = new HitBox(pos.x, pos.y - 30, enemyShipSprite.getWidth() * 0.475, enemyShipSprite.getHeight() * 0.475);

    shootingPoint = ofPoint(pos.x + enemyHitBox->box.getWidth()/2 - 10, pos.y + enemyHitBox->box.getHeight() / 2);
}

void ISS::update(const ofPoint& playerPos) {
    shotTimer++;
    if (shotTimer % 10 == 0) {
        shoot();
    }
}

void ISS::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    // ofRotateDeg(shipOrientation);
    enemyShipSprite.draw(-32, -32, 300, 230);
    ofPopMatrix();
    
    // Uncomment this if you want to see the hitbox for the enemy
    if(showHitboxes){
        enemyHitBox->draw();
    }
    
    showBossHealth();
}


void ISS::shoot() {

    // Increment the shootAngleOffset for the next call to shoot() to continue the spiral
    Projectiles bullet(shootingPoint, shootAngleOffset); // Bullet will shoot at the desired angle
    bullet.setColors(ofColor::blueSteel, ofColor::darkBlue);
    enemyBullets.push_back(bullet);
    shootAngleOffset += 15.0; // This value sets the desired spiral rotation speed

    // Optional: Reset shootAngleOffset to keep it within 0-360 degrees to avoid overflow
    if(shootAngleOffset >= 360.0) {
        shootAngleOffset -= 360.0;
    }

    SoundManager::playSong("bulletSound", false);
}