#include "ISS.h"

ISS::ISS(int xpos, int ypos, string name) : Boss(xpos, ypos, 0.5, 400, name) {
    enemyShipSprite.load("ShipModels/Space_Station.png");
    enemyHitBox = new HitBox(pos.x, pos.y - 30, enemyShipSprite.getWidth() * 0.05, enemyShipSprite.getHeight() * 0.075);

}

void ISS::update(const ofPoint& playerPos) {
    shotTimer++;
    if (shotTimer % 30 == 0) {
        shoot();
    }
}

void ISS::draw() {
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    // ofRotateDeg(shipOrientation);
    enemyShipSprite.draw(ofGetWindowWidth()/2 - 200, ofGetWindowHeight()/2 - 200, 400, 400);
    ofPopMatrix();
    
    // Uncomment this if you want to see the hitbox for the enemy
    if(showHitboxes){
        enemyHitBox->draw();
    }
    
    showBossHealth();
}


void ISS::shoot() {
    const int numBullets = 18; // Increase the number of bullets for a smoother spiral
    const float angleStep = 360.0 / numBullets; // Ensure a full circle is covered

    for (int i = 0; i < numBullets; ++i) {
        // Calculate the angle for each bullet, incorporating the shootAngleOffset
        float angle = i * angleStep + shootAngleOffset;

        // Create and initialize the bullet at the boss's position with the given angle
        // Note: The angle is directly used in the Projectiles constructor
        Projectiles bullet(shootingPoint, angle);
        bullet.setSpeed(3);
        bullet.setColors(ofColor::blueSteel, ofColor::darkBlue); // Set desired colors
        enemyBullets.push_back(bullet);
    }   

    // Increment the shootAngleOffset for the next call to shoot() to continue the spiral
    shootAngleOffset += 5.0; // Adjust this value for the desired spiral rotation speed

    // Optional: Reset shootAngleOffset to keep it within 0-360 degrees to avoid overflow
    if(shootAngleOffset >= 360.0) {
        shootAngleOffset -= 360.0;
    }

    SoundManager::playSong("bulletSound", false);
}