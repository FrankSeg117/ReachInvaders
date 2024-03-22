#include "playerShip.h"
#include "EnemyManager.h"
#include "ShipBattle.h"

//Parametrized Constructor for the playerShip
Player::Player(int Xposition, int Yposition){
    pos.x = Xposition;
    pos.y = Yposition;
    health = 100;

    lives = 3;
    shield = 0;
    maxshield = 100;
    shieldtimer = 0;

    bombCount = 0;
    bombTimer = 0;

    velocity.set(0, 0);
    this->shipSprite.load("ShipModels/shipModel2.png");
    this->shipSprite1.load("ShipModels/ShipLost_1live.png");
    this->shipSprite2.load("ShipModels/ShipLost_2live.png");

    this->shieldsprite.load("CompressedImages/ForceShield.png");
    shieldIndicator.load("ShipModels/Shield.png");

    //////////////////////////////////////////////////////
    this->NewShip.load("ShipModels/secondShip.png");
    this->NewShipL1.load("ShipModels/Ship2_L1Live.png");
    this->NewShipL2.load("ShipModels/Ship2_L2Live.png");
    //////////////////////////////////////////////////////

    this->shipOrientation = 0;
    accelerationAmount = 5.0; // Adjust the value as needed
    score = 0;
    hitBox =  HitBox(pos, shipSprite.getWidth() * 0.25, shipSprite.getHeight() * 0.15);
            
    lastShotTime = 0;
    shotCooldown = 0.13;  // Set the cooldown duration to 0.5 seconds (adjust as needed)

}

Player::Player(){
    Player(ofGetWidth()/2, ofGetHeight()/2);
}

int Player::getScore() { return score; }
void Player::setScore(int score) { this->score = score; }

void Player::draw() {
        // Draw the ship sprite with the calculated rotation
            ofPushMatrix();
            ofTranslate(this->pos.x, this->pos.y);
            ofRotateDeg(shipOrientation);

        if(!firstbossdefeated){
            if(this->lives == 3){this->shipSprite.draw(-20, -20, 45, 45);}
            if(this->lives == 2){this->shipSprite1.draw(-20, -20, 45, 45);}
            if(this->lives == 1){this->shipSprite2.draw(-20, -20, 45, 45);}
        }
        if(firstbossdefeated){
            if(this->lives == 3){this->NewShip.draw(-20, -20, 45, 45);}
            if(this->lives == 2){this->NewShipL1.draw(-20, -20, 45, 45);}
            if(this->lives == 1){this->NewShipL2.draw(-20, -20, 45, 45);}
        }
            if(this->shieldactive){this->shieldsprite.draw(-20, -20, 50, 50);} //If shield is active draw shield around player
            ofPopMatrix();
                
        // Draw the hitbox around the player ship. Uncomment this line for testing purposes
            if(showHitbox)  this->hitBox.draw();

            if(shieldactive) shieldIndicator.draw(ofGetWidth() -145, 35, 40, 40);
}

void Player::update() {
    processPressedKeys();  // Process the pressed keys and calculate orientation change
    if(sprinting){
    velocity.limit(2*maxSpeed);  //If sprinting the velocity limit will double  
    }
    else{
    velocity.limit(maxSpeed); // Limit the velocity to the maximum speed
    }       
    pos += velocity; // Update position based on velocity
    this->hitBox.box.setPosition(pos.x - 15, pos.y - 15);
            
    velocity *= damping; // Apply damping to slow down the ship

    if(firstbossdefeated){
        shotCooldown = 0.1; //faster fire rate for new ship
    }

    draw();  // Draw the ship
    if (shieldactive){
        shieldtimer++;
        if(shieldtimer%10 == 0){
            shield--;
            shieldtimer = 0;
        }
        if (shield <=0){
            shieldactive = false;
            SoundManager::stopSong("Shieldsound");
        }
    }
}

void Player::shoot() { 
    // Calculate the current time
    float currentTime = ofGetElapsedTimef();

    // Check if enough time has passed since the last shot
        if (currentTime - lastShotTime >= shotCooldown) {
            if(firstbossdefeated){
                Projectiles p = Projectiles(ofPoint(this->pos.x, this->pos.y), this->shipOrientation, 19);
                p.setColors(ofColor::lightGoldenRodYellow, ofColor::whiteSmoke);
                this->bullets.push_back(p);
            }
            else {
                Projectiles p = Projectiles(ofPoint(this->pos.x, this->pos.y), this->shipOrientation);
                p.setColors(ofColor::azure, ofColor::blueViolet);
                this->bullets.push_back(p);
            }            
            // SoundManager::playSong("bulletSound", false);
            SoundManager::playSong("Beam", false);

            // Update the last shot time to the current time
            lastShotTime = currentTime;
        }
}

void Player::activateshield() { 
    if (shield == maxshield && !shieldactive){
        shieldactive = true;
        SoundManager::playSong("Shieldsound", true);
    }
}

void Player::activateBomb(){
    SoundManager::playSong("Bomb", false);
    EnemyManager::enemyList.clear();
}


void Player::setShotCooldown(float shotCooldown) { this->shotCooldown = shotCooldown; }

void Player::removeMarkedBullets(){
    bullets.erase(remove_if(bullets.begin(), bullets.end(), [](const Projectiles& p) { return p.markedForDeletion; }), bullets.end());
}

void Player::addPressedKey(int key) {
    key = tolower(key);

    keyMap[key] = true;
    isMoving = true; // Set the movement flagw
}

void Player::processPressedKeys() {
    if(keyMap['w']) movement('w');
    if(keyMap['s']) movement('s');
    if(keyMap['d']) movement('d');
    if(keyMap['a']) movement('a');

    if(keyMap[OF_KEY_SHIFT]) movement(OF_KEY_SHIFT);

    if(keyMap[' ']) shoot();

    if(keyMap['q']) activateshield();

    if(keyMap['e'] && bombCount != 0){
        activateBomb();
        bombCount--;
    }


    if (!isMoving) {
        // Apply damping to gradually slow down the ship when no keys are being pressed
        velocity *= damping; 
    }
}      

void Player::removePressedKey(int key) {
    key = tolower(key);
    keyMap[key] = false;
}

void Player::movement(char keyPressed) {
    ofVec2f acceleration;

    if (keyPressed == 'w') {
        // Apply acceleration in the direction of the ship's orientation
        acceleration.set(cos(ofDegToRad(shipOrientation)) * accelerationAmount, sin(ofDegToRad(shipOrientation)) * accelerationAmount);
        }
    if (keyPressed == 's') {
        // Apply acceleration in the opposite direction of the ship's orientation
        acceleration.set(-cos(ofDegToRad(shipOrientation)) * accelerationAmount, -sin(ofDegToRad(shipOrientation)) * accelerationAmount);
        }

    // Apply force to velocity
    if(keyPressed == OF_KEY_SHIFT && 'w'){
        sprinting = true;
        velocity += 2*acceleration;
    } 

    else {
    sprinting = false;
    velocity += acceleration;
    }

    
    if (keyPressed == 'a') {
        // Rotate the ship counterclockwise
        shipOrientation -= rotationSpeed;
        }
    if (keyPressed == 'd') {
        // Rotate the ship clockwise
        shipOrientation += rotationSpeed;
        }
    }   