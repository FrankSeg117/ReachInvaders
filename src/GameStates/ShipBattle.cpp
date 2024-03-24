#include "ShipBattle.h"

// ====================================
// Constructor & Destructor Section
// ====================================

ShipBattle::ShipBattle() {
    this->player = new Player(ofGetWidth() / 2, ofGetHeight() / 2);
    this->playerScore = 0;
    this->killspreeTimer = 0;
    
    font.load("Fonts/Halo.ttf", 20, true);
    indicatorFont.load("Fonts/Halo.ttf", 10, true);
    backgroundImage.load("Menu_Images/In-game.jpg");

    // int w = 1200;
    // int h = 800;

    // backgroundImage.resize(w, h);

    BombSprite.load("ShipModels/Bomb.png");             //Bomb sprite for the bomb indicator
}

// ====================================
// Core Functionality Section
// ====================================


// Update Method
void ShipBattle::update() {
    // Boss spawn logic
    if (EnemyManager::isBossSpawning()) {
        displayBossWarning = true;
        SoundManager::stopSong("battle");
    } 
    else {
        displayBossWarning = false;
    }

    // Update enemies and player
    EnemyManager::updateEnemies(player);
    this->player->processPressedKeys();
    this->player->update();
    wrapCoords(this->player->pos);

    // Update scoring and kill spree timer
    playerScore += EnemyManager::getPointsScored() * scoreMultiplier() * killSpreeMode();
    this->player->setScore(playerScore);
    killspreeTimer = EnemyManager::getKillSpreeTimer();

    // Update player projectiles
    if (!this->player->bullets.empty()) {
        updateBullets();
    }

    // State switching logic for when the player dies
    if (this->player->health <= 0) {
        if(this->player->lives != 0){//si quedan vidas, se resta una y se pone vida al max
            player->health = 100;//Health back to max
            player->lives--;//One life is deducted from counter
            SoundManager::playSong("ShipWentBoom", false);

        }       
        if(this->player->lives == 0){ //If no lives remaining
        SoundManager::playSong("ShipWentBoom", false); 
        this->setNextState("GameOverState");
        SoundManager::stopSong("battle");
        if(EnemyManager::getSpawningBossType() != ""){
            SoundManager::stopSong(EnemyManager::getSpawningBossType());
        }
            // Write the current score to a file
            ofstream scoreFile("currentScore.txt");
            if (scoreFile.is_open()) {
                scoreFile << playerScore;
                scoreFile.close();
            }
            this->setFinished(true);
        }
    }
}

//====== Draw Method ====== 
void ShipBattle::draw() {
    ofSetBackgroundColor(ofColor::black);
    backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    // Draw the score
    ofSetColor(ofColor::white);
    font.drawString("SCORE " + to_string(playerScore), ofGetWidth() / 2 - 50, 50);

    // Draw enemies and player
    EnemyManager::drawEnemies();
    player->draw();
    draw_bullets();
    
    // Draw boss warning if needed
    if (displayBossWarning) {
        // Semi-transparent red screen tint
        ofSetColor(255, 0, 0, 128);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

        // Warning text
        ofSetColor(255);
        font.drawString("BOSS APPROACHING!", ofGetWidth() / 2 - 100, ofGetHeight() / 2);
    }

    // Draw UI elements
    healthBar(player->health, 100);
    killSpreeTimer(this->killspreeTimer, 150);
    shieldbar(player->shield, player->maxshield);
    if(player->bombTimer > 0){
    bombCooldown(player->bombTimer, 600);
    }
    //Draw a mini box for the bomb. Make sure to draw the bomb inside this box.
        ofNoFill(); 
        ofDrawRectangle(ofGetWidth() - 150, 30, 50, 50); 
        ofFill();

        ofNoFill(); 
        ofDrawRectangle(ofGetWidth() -150, 85, 50, 50); 
        ofFill();
        
        if (player->bombCount > 0){
            BombSprite.draw(ofGetWidth() - 165, 15, 80, 80);
        }
    //Debugger
    font.drawString("Lives: " + to_string(player->lives), 100, 30);
    font.drawString("Bombs: " + to_string(player->bombCount), 250, 30); 
    
    if(playerScore <= 25000) font.drawString("Covenant ships inbound!", 10, 220);
    else if(playerScore > 25000 && playerScore <= 75000) font.drawString("Current Mission... \nFIGHT THEM AWAY!", 10, 220);
    else if(playerScore > 75000) font.drawString("Current Mission... \nSURVIVE NOBLE", 10, 220);

}

// ====================================
// Input Handling Section
// ====================================

void ShipBattle::keyPressed(int key) {
    player->addPressedKey(key);
    
    //DEBUG KEYS - Press these keys to ease the debugging with the game
    if(key == 'k') {
        EnemyManager::toggleHitBoxes = !EnemyManager::toggleHitBoxes; 
        player->showHitbox = !player->showHitbox;
    }
    if(key == 'o')  player->health = 100;
    if(key == 'p')  playerScore += 1000; 
}

void ShipBattle::keyReleased(int key) {
    key = tolower(key);
    this->player->removePressedKey(key);
}

void ShipBattle::mousePressed(int x, int y, int button) {
    // Implementation for mousePressed event in case you want to do something with this
}

// ====================================
// Helper Methods Section
// ====================================

void ShipBattle::wrapCoords(ofPoint &currentPos) {
    // Screen wrapping logic for player ship
    if (currentPos.x < 0.0) currentPos.x = ofGetWidth() - 10;
    if (currentPos.x >= ofGetWidth()) currentPos.x = 10;
    if (currentPos.y < 0.0) currentPos.y = ofGetHeight() - 10;
    if (currentPos.y >= ofGetHeight()) currentPos.y = 10;
}

void ShipBattle::draw_bullets() {
    // Draw all player bullets
    for (auto &bullet : this->player->bullets) {
        bullet.draw();
    }
}

void ShipBattle::updateBullets() {
    // Update logic for player bullets
    for (auto &bullet : this->player->bullets) {
        bullet.update();
    }
}

// ====================================
// UI and Feedback Methods Section
// ====================================
void ShipBattle::healthBar(int currHealth, int maxHealth) {
    indicatorFont.drawString("HEALTH", 10, 30);
    ofNoFill();
    ofDrawRectangle(10, 40, maxHealth *2, 20);
    ofFill();
    ofSetColor(ofColor::green);
    ofDrawRectangle(10, 40, currHealth *2, 20);
    ofSetColor(ofColor::white);
}

void ShipBattle::killSpreeTimer(int currTimer, int maxTimer) {
    indicatorFont.drawString("KILL SPREE", 10, 80);
    ofNoFill();
    ofDrawRectangle(10, 90, maxTimer, 10);
    ofFill();
    ofSetColor(ofColor::red);
    ofDrawRectangle(10, 90, currTimer, 10);
    ofSetColor(ofColor::white);
}

void ShipBattle::shieldbar(int currShield, int maxShield){
    indicatorFont.drawString("SHIELD", 10, 130);
    ofNoFill();
    ofDrawRectangle(10, 140, maxShield *1.8, 30);
    ofFill();
    ofSetColor(ofColor::blue);
    ofDrawRectangle(10, 140, currShield *1.8, 30);
    ofSetColor(ofColor::white);
}

void ShipBattle::bombCooldown(int cooldownduration, int maxduration) {
    indicatorFont.drawString("Cooling Down", 10, 190);
    ofNoFill();
    ofDrawRectangle(10, 200, maxduration *.25, 10);
    ofFill();
    ofSetColor(ofColor::orangeRed);
    ofDrawRectangle(10, 200, cooldownduration *.25, 10);
    ofSetColor(ofColor::white);
}

// ====================================
// Game Mechanics Methods Section
// ====================================

double ShipBattle::killSpreeMode() {
    // Logic for kill spree mode bonus
    return this->killspreeTimer > 0 ? 1.5 : 1.0;
}

double ShipBattle::scoreMultiplier() {
    // Logic for score multiplier based on current score
    if (playerScore >= 75000){
        return 3.50;
    }
    else if (playerScore >= 25000) {
        return 2.25;
    }
    else if (playerScore >= 5000) {
        return 1.75;
    }
    else return 1.0; // Default multiplier
}

void ShipBattle::reset(){
    setFinished(false);
    setNextState("");
}


                                        
