#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace FEHIcon;
using namespace std;

//CONSTANTS
#define deltaFrameTime 1/5.0
#define playerMass 10
#define obMass 1

//
//FUNCTIONS DECLARATIONS NOT ATTACHED TO A CLASS
//
void clearScreen();

//
//CLASSES
//

class Player;

class Projectile
{
    public:
        float screenXPos;
        float screenYPos;
        float xVel;
        float yVel;
        float acceleration = 1;
        float mass;
    public:
        Projectile(){};
        Projectile(float screenXPos, float screenYPos, float xVel, float yVel, float mass);
        void draw();
};

class Obstacle : public Projectile
{
    private:
        bool hasCollided = false;
    public:
        Obstacle(){};
        Obstacle(float screenXPos, float screenYPos, float xVel, float yVel, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass)
        {
            // this->screenXPos = screenXPos;
            // this->screenYPos = screenYPos;
            // this->xVel = xVel;
            // this->yVel = yVel;
        };
        bool updateScreenPos(float pXVel, float PYVel); 
        void collide(Player *player);
        friend class Player;
};

class Player : public Projectile
{
    protected:
        float xPos;
        float yPos;
    public:
        Player() : Projectile(){};
        Player(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass){
            this->xPos = xPos;
            this->yPos = yPos;
        };
        void init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass);
        void startAnim();
        void updatePos(float deltaTime);
        void endAnim(vector<Obstacle> obs);
        float getXVelocity();
        float getYVelocity();
        float getY();
        void multiplyAccel(float);
        void setXVel(float xVel);
        void setYVel(float yVel);
};

class gameState
{
    private:
        Player player;
        float startingVelocity;
        int launches; // total number of times played during code running
        float arcLength = 0;
        float pointsTot = 0;
        float previousDistance = 0;
        float maxDistance = 0;
        float maxHeight = 0; // arc length for each run and total points collected
    public:
        gameState(int launches, float arcLength, float pointsTot, float startingVelocity, float previousDistance, float maxDistance, float maxHeight);
        void initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass);
        void drawMenu();
        void displayGame();
        void runGame();
        void displayStats();
        void displayInstructions();
        void displayCredits();
        void returnToMainMenu();
        void SetUpgrades(); // code mostly completed
        void SetAngle(); // buttons dealt with, left x/y velocity calculations up to you like you said
        void UpdatePosition(); // code completed
        void EndScreen(); // code completed
};

//
//MAIN METHOD
//

int main() {
    bool running = true;
    while (running)
    {
        // @ts-ignore
        LCD.Update();
        gameState game(0,0,0,10,0,0,0);
        game.drawMenu();
        running = false;
    }
    while(1)
    {
        // @ts-ignore
        LCD.Update();
    }
    return 0;
}

//
//FUNCTION DEFINITIONS NOT ATTACHED TO A CLASS
//

void clearScreen()
{
    LCD.Clear();
    LCD.DrawRectangle(0,0,319,239);
}

//
//FUNCTION DEFINITIONS TIED TO A CLASS
//

gameState :: gameState(int launches, float arclength, float pointsTot, float startingVelocity, float maxDistance, float maxHeight, float previousDistance)
{
    this->launches = launches;
    this->arcLength = arcLength;
    this->pointsTot = pointsTot;
    this->startingVelocity = startingVelocity;
    this->maxDistance = maxDistance;
    this->maxHeight = maxHeight;
    this->previousDistance = previousDistance;
}

void gameState :: initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass)
{
    player.init(xPos, yPos, xVel, yVel, screenXPos, screenYPos, mass);
}

void gameState :: drawMenu()
{
    // set standard values for menu
    LCD.SetBackgroundColor(BLACK);
    LCD.SetFontColor(RED);
    LCD.Clear();
    // define stats for main 4 buttons on menu
    Icon gameButton; // play the game
    Icon statButton; // display statistics
    Icon instructionButton; // display instructions
    Icon creditButton; // display game credits
    gameButton.SetProperties("Play", 40, 26, 240, 26, WHITE, RED);
    statButton.SetProperties("Stats", 40, 78, 240, 26, WHITE, RED);
    instructionButton.SetProperties("Instructions", 40, 130, 240, 26, WHITE, RED);
    creditButton.SetProperties("Credits", 40, 182, 240, 26, WHITE, RED);
    // draw values to the screen
    LCD.WriteLine("Learn to Fly 4"); // display the game title
    gameButton.Draw();
    statButton.Draw();
    instructionButton.Draw();
    creditButton.Draw();
    // @ts-ignore
    LCD.Update();
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y;
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check which button - run respective menu/game
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(gameButton.Pressed(x,y,1)==1)
        {
            displayGame(); // starts running the game
            choice = 1;
        }
        if(statButton.Pressed(x,y,1)==1)
        {
            displayStats(); // opens the statistics menu
            choice = 1;
        }
        if(instructionButton.Pressed(x,y,1)==1)
        {
            displayInstructions(); // displays the instructions of the game
            choice = 1;
        }
        if(creditButton.Pressed(x,y,1)==1)
        {
            displayCredits(); // lists the credits for the game
            choice = 1;
        }
    }
}

void gameState :: displayGame() // runs the game itself
{
    initPlayer(0,0,0,0,20,220, playerMass);
    SetUpgrades();
    SetAngle();
    runGame();
    EndScreen();
}

void gameState :: runGame()
{
    float runHeight = 0;
    player.startAnim();
    vector<Obstacle> obs;
    while(player.getY()<0)
    {
        if(player.yVel > 0 && runHeight ==0)
        {
            runHeight = -player.getY();
        }
        double rand = std::rand()/(double)(RAND_MAX);
        if(rand < (10-obs.size())/40.0)
        {
            if(std::rand()/(double)(RAND_MAX)<.5)
            {
                if(player.getYVelocity()<0)
                {
                    int x = std::rand()/(double)(RAND_MAX)*250+40;
                    int y = std::rand()/(double)(RAND_MAX)*10+10;
                    Obstacle temp(x,y,0, 0, obMass);
                    obs.push_back(temp);
                }
                else
                {
                    int x = std::rand()/(double)(RAND_MAX)*250+40;
                    int y = std::rand()/(double)(RAND_MAX)*5+210;
                    Obstacle temp(x,y,0, 0, obMass);
                    obs.push_back(temp);
                }
            }
            else
            {
                int x = std::rand()/(double)(RAND_MAX)*10+300;
                int y = std::rand()/(double)(RAND_MAX)*210+10;
                Obstacle temp(x,y,0, 0, obMass);
                obs.push_back(temp);
            }
        }
        clearScreen();
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(player.xVel,player.yVel) == true;}), obs.end());
        for (int i = 0; i < obs.size(); i++)
        {
            obs.at(i).draw();
            obs.at(i).collide(&player);
        }
        player.updatePos(1.0/10);
        UpdatePosition();
        player.draw();
        // @ts-ignore
        LCD.Update();
    }
    player.endAnim(obs);
    maxHeight = max(runHeight,maxHeight);
    maxDistance = max(arcLength, maxDistance);
}

void gameState :: displayStats() // statistics menu
{
    LCD.Clear();
    // display stats - total distance of arc length, maximum arc length, and total launches played
    LCD.WriteLine("Launches: ");
    LCD.WriteLine(launches);
    LCD.WriteLine("Previous Distance(m): ");
    LCD.WriteLine(previousDistance);
    LCD.WriteLine("Max Distance(m): ");
    LCD.WriteLine(maxDistance);
    LCD.WriteLine("Max Height(m): ");
    LCD.WriteLine(maxHeight);
    LCD.WriteLine("Total Distance(m): ");
    LCD.WriteLine(pointsTot);
    // prompt user to return to main menu
    returnToMainMenu();
}

void gameState :: displayInstructions() // instructions menu
{
    LCD.Clear();
    // have issues with text looping - must set text in different lines to work around
    LCD.WriteLine("Set the angle of the");
    LCD.WriteLine("initial launch; hit the");
    LCD.WriteLine("launch button when ready.");
    LCD.WriteLine("Collect points from");
    LCD.WriteLine("distance traveled. Buy");
    LCD.WriteLine("upgrades with your points and keep playing!");
    // prompt user to return to main menu
    returnToMainMenu();
}

void gameState :: displayCredits() // credits menu
{
    LCD.Clear();
    LCD.WriteLine("Avery Taylor");
    LCD.WriteLine("Joe Quinn");
    LCD.WriteLine("Inspired by Learn to Fly");
    // prompt user to return to main menu
    returnToMainMenu();
}

void gameState :: returnToMainMenu()
{
    // create the button to return to main menu
    Icon menuButton;
    menuButton.SetProperties("Return to Menu", 40, 182, 240, 26, WHITE, RED);
    menuButton.Draw();
    // @ts-ignore
    LCD.Update();
    // check if/when menu button is clicked
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y;
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check if menu button was clicked
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(menuButton.Pressed(x,y,1)==1)
        {
            drawMenu(); // returns to menu
            choice = 1;
        }
    }
}

// -- definitions for functions used in game itself: --

// put your function definitions for the game here for ease of access
// also, modify the declarations I made above if you need inputs - we likely will for a lot of the main ones in the motion
// a lot of the specific ones (angle, launch, position/screen, collision, etc) will require pointers to permanently modify the values!
// need more formal discussion on the specific logistics of the game to figure out most of this

void gameState::SetUpgrades() // prompt the user to either purchase a power-up or continue to game
{
    // display points/options to screen
    LCD.Clear();
    LCD.Write("Total points:");
    LCD.WriteLine(pointsTot);
    LCD.WriteLine("Choose a powerup:");

    // make buttons for each powerup available, display to screen
    Icon speedButton, gliderButton, skipButton;
    speedButton.SetProperties("2x speed-5,000", 20, 78, 280, 26, WHITE, RED);
    speedButton.Draw();
    gliderButton.SetProperties("x0.75gravity-10,000", 20, 130, 280, 26, WHITE, RED);
    gliderButton.Draw();
    skipButton.SetProperties("Skip Powerups", 20, 182, 280, 26, WHITE, RED);
    skipButton.Draw();
    // @ts-ignore
    LCD.Update();

    // check if/when one of the buttons are clicked
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y;
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check which button - run respective function
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(speedButton.Pressed(x,y,1)==1)
        {
            // doubles the initial velocity - powerup can increase several times? need to discuss maybe
            // check if user has enough points - if yes, run function/reduce points otherwise do nothing
            if (pointsTot >=5000)
            {
                pointsTot-=5000;
                startingVelocity *= 2;
                choice = 1;
            }
        }
        if(gliderButton.Pressed(x,y,1)==1)
        {
            // decrease gravity by 25% - can increase several times?
            if (pointsTot >= 10000)
            {
                pointsTot-=10000;
                player.multiplyAccel(.75);
                choice = 1;
            }
        }
        if(skipButton.Pressed(x,y,1)==1)
        {
            // user has bought no powerups - no modification to values needed
            // do nothing
            choice = 1;
        }
    }
}

void gameState::SetAngle() // prompt the user to set the launch angle for the projectile
{
    LCD.Clear();
    // prompt user to set the initial angle
    LCD.WriteLine("Choose an initial launch angle:");

    // create buttons for each of the angles used - 30/45/60/90
    Icon thirtyButton, fortyfiveButton, sixtyButton, ninetyButton;
    thirtyButton.SetProperties("30 degrees", 40, 26, 240, 26, WHITE, RED);
    fortyfiveButton.SetProperties("45 degrees", 40, 78, 240, 26, WHITE, RED);
    sixtyButton.SetProperties("60 degrees", 40, 130, 240, 26, WHITE, RED);
    ninetyButton.SetProperties("90 degrees", 40, 182, 240, 26, WHITE, RED);
    // draw the buttons
    thirtyButton.Draw();
    fortyfiveButton.Draw();
    sixtyButton.Draw();
    ninetyButton.Draw();
    // @ts-ignore
    LCD.Update();
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y;
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check if the user clicked an angle button - fix x/y velocities by trigonometry
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(thirtyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(30), vy = v*sin(30) - just remember we can't use functions, so check what the values are
            player.init(0,0,-startingVelocity*cos(60),startingVelocity*sin(60),20,220, playerMass);
            choice = 1;
        }
        if(fortyfiveButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(45), vy = v*sin(45) - both sin/cos of 45 are the same so this one should be easy
            player.init(0,0,startingVelocity*cos(45),-startingVelocity*sin(45),20,220, playerMass);
            choice = 1;
        }
        if(sixtyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(60), vy = v*sin(60) - just reversing sines/cosines of 30
            player.init(0,0,startingVelocity*cos(30), startingVelocity*sin(30),20,220, playerMass);
            choice = 1;
        }
        if(ninetyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(90), vy = v*sin(90) - vx will be zero, vy will be the full initial velocity
            player.init(0,0,0,-startingVelocity,20,220, playerMass);
            choice = 1;
        }
    }

}

void gameState::UpdatePosition() // accepts the current position/velocity/total arclength, updates the values based on the change in time between frames
{
    // calculate change in arclength, add to total arc length
    arcLength+=pow((pow(player.getXVelocity(), 2))+(pow(player.getYVelocity(), 2)), 0.5); // displacement equation
}

void gameState::EndScreen() // displays the final results of the game, accepting the arclength value of that run and current point value
// don't need pointers since none of the values need to be modified
{
    // increase total launches for stats screen, increase total points by given arclength
    launches++;
    pointsTot+=arcLength;
    cout << arcLength <<" al "<< pointsTot<<" pt\n";
    previousDistance = arcLength;
    LCD.Clear();
    // tell the user how many points they've earned/how far they went and their current point total
    LCD.Write("Distance Traveled: ");
    LCD.WriteLine(arcLength);
    LCD.Write("Total Points: ");
    LCD.WriteLine(pointsTot);
    arcLength = 0;

    // prompt the user to either play again or return to menu
    Icon menuButton, resetButton;
    menuButton.SetProperties("Return to Menu", 40, 182, 240, 26, WHITE, RED);
    menuButton.Draw();
    resetButton.SetProperties("Play Again", 40, 130, 240, 26, WHITE, RED);
    resetButton.Draw();
    // @ts-ignore
    LCD.Update();

    // check if/when one of the buttons are clicked
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y;
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check which button - run respective function
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(menuButton.Pressed(x,y,1)==1)
        {
            drawMenu(); // returns to menu
            choice = 1;
        }
        if(menuButton.Pressed(x,y,1)==1)
        {
            displayGame(); // resets the game
            choice = 1;
        }
    }
}

Projectile :: Projectile(float screenXPos, float screenYPos, float xVel, float yVel, float mass)
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
    this->xVel = xVel;
    this->yVel = yVel;
    this->mass = mass;
}

void Projectile :: draw()
{
    LCD.DrawCircle(screenXPos, screenYPos, 10);
}

void Player :: init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->xVel = xVel;
    this->yVel = yVel;
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
    this->mass = mass;
}

void Player :: startAnim()
{
    while(screenXPos < 160 && screenYPos > 120)
    {
        screenXPos += xVel * deltaFrameTime;
        xPos += xVel * deltaFrameTime;
        screenYPos += yVel * deltaFrameTime;
        yPos += yVel * deltaFrameTime;
        yVel += acceleration * deltaFrameTime*1.0/10;
        clearScreen();
        draw();
        LCD.DrawHorizontalLine(230, 0, 319);
        // @ts-ignore
        LCD.Update();
    }
}

void Player :: updatePos(float deltaTime)
{
    xPos += xVel * deltaFrameTime;
    yPos += yVel * deltaFrameTime;
    yVel += acceleration * deltaFrameTime * 1.0/10;
    LCD.WriteLine(xVel);
    LCD.WriteLine(yVel);
}

void Player :: endAnim(vector<Obstacle> obs)
{
    float floorPos = 239;
    while(screenXPos < 300 && screenYPos < floorPos - 10)
    {
        screenXPos += xVel * deltaFrameTime;
        xPos += xVel * deltaFrameTime;
        screenYPos += yVel * deltaFrameTime;
        yPos += yVel * deltaFrameTime;
        yVel += acceleration * deltaFrameTime*1.0/10;
        clearScreen();
        draw();
        floorPos -= yVel * 1.0/5;
        floorPos = min((int)floorPos, 230);
        LCD.DrawHorizontalLine(floorPos, 0, 319);
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(xVel, yVel) == true;}), obs.end());
        for (int i = 0; i < obs.size(); i++)
        {
            obs.at(i).draw();
        }
        // @ts-ignore
        LCD.Update();
    }
}

float Player :: getXVelocity()
{
    return xVel;
}

float Player :: getYVelocity()
{
    return yVel;
}

float Player :: getY()
{
    return yPos;
}

void Player :: multiplyAccel(float coef)
{
    acceleration *= coef;
}

void Player::setXVel(float xVel)
{
    this->xVel = xVel;
}

void Player::setYVel(float yVel)
{
    this->yVel = yVel;
}

bool Obstacle :: updateScreenPos(float pXVel, float pYVel)
{
    screenXPos -= xVel * deltaFrameTime+pXVel*deltaFrameTime;
    screenYPos -= yVel * deltaFrameTime+pYVel*deltaFrameTime;
    //yVel += acceleration * deltaFrameTime * 1.0/10;
    if(screenXPos < 10 || screenYPos > 240 || screenXPos > 320 || screenYPos < 10)
    {
        return true;
    }
    return false;
}

void Obstacle :: collide(Player *player)
{
    float distance = pow(pow(player->screenXPos-screenXPos,2)+pow(player->screenYPos-screenYPos,2),.5);
    if(distance < 20 && !hasCollided)
    {
        cout<<player->mass <<" Player mass\n";
        cout<<mass<<" ob mass\n";
        //Original veloity x direction for player(ogvxp)
        float ogvxp = player->getXVelocity();
        float ogvyp = player->getYVelocity();
        float ogvxo = -xVel;
        float ogvyo = -yVel;

        player->setXVel(((player->mass-mass)*ogvxp+2*mass*ogvxo)/(player->mass+mass));
        player->setYVel(((player->mass-mass)*ogvyp+2*mass*ogvyo)/(player->mass+mass));
        xVel = -((mass-player->mass)*ogvxo+2*player->mass*ogvxp)/(player->mass+mass);
        yVel = -((mass-player->mass)*ogvyo+2*player->mass*ogvyp)/(player->mass+mass);
        cout << ogvxp << " - " << player->xVel << "\n";
        cout << ogvyp << " - " << player->yVel << "\n";
        cout << ogvxo << " - " << xVel << "\n";
        cout << ogvyo << " - " << yVel << "\n";
        hasCollided = true;
    }
}
