#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>
#include <FEHImages.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace FEHIcon;
using namespace std;

//CONSTANTS
#define deltaFrameTime 1/5.0 // fps of the game
#define playerMass 10 // initial player mass for collision physics
#define obMass 1 // initial obstacle mass for collision physics

//
//FUNCTIONS DECLARATIONS NOT ATTACHED TO A CLASS
//
void clearScreen();

//
//CLASSES
//

//Forward declaration of player class, so the obstacle class can refrence it in a method.
class Player;

//Projectile class, this class will not be constructed itself but will be used as the superclass for the obstacle and player classes.
class Projectile
{
    public:
        float screenXPos; //X Position of the projectile on the screen.
        float screenYPos; //Y Position of the projectile on the screen.
        float xVel;//X velocity of the projectile.
        float yVel;//Y velocity of the projectile.
        float acceleration = 1; //Acceleration of projectile, will start at 1 for all projectiles.
        float mass;// Mass of the projectile, will affect collisions between two projectiles.
        FEHImage image; //Image object that will be drawn at the projectiles position.
    public:
        Projectile(){}; //Default constructor.
        Projectile(float screenXPos, float screenYPos, float xVel, float yVel, float mass);//Parameterized constructor
        void draw(); //Draws the image object at the screen position.
};

class Obstacle : public Projectile
{
    private:
        bool hasCollided = false; //Boolean so each obstacle can only collide with the player once.
    public:
        Obstacle(){};//Default constructor
        Obstacle(float screenXPos, float screenYPos, float xVel, float yVel, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass){};//Parameterized constructor
        bool updateScreenPos(float pXVel, float PYVel); //Updates the screen position fields based on the relative velocity of the obstacle to the player.
        void collide(Player *player);//Checks if the obstacle is within a distance to the player and updates the player and obstacles velocity if so.
};

class Player : public Projectile
{
    protected:
        float xPos; //X Position of the player relative to where it started in the game world, not related to screen position.
        float yPos;//Y Position of the player relative to where it started in the game world, not related to screen position.
    public:
        Player() : Projectile(){}; // Default constructor.
        Player(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass){
            //Calls the superclass contructor, sets the subclass fields in this body.
            this->xPos = xPos;
            this->yPos = yPos;
        };//Parameterized constructor
        void init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass);//Sets all variables
        void startAnim();//Takes the player from the bottom left of the screen to the middle to act like a launch
        void updatePos(float deltaTime);//Calculates the new x and y coordinates of the player based on velocity, also updates velocity using acceleration
        void endAnim(vector<Obstacle> obs);//When the player is approaching the ground, draws player going from middle of screen to landing on ground.
        float getXVelocity();//Returns xVel
        float getYVelocity();//Returns yVel
        float getY();//Returns yPos
        void multiplyAccel(float);//Multiplies the accerlation by the argument.
        void setXVel(float xVel);//Sets the xVelocity, will be changed by collision
        void setYVel(float yVel);//Sets the yVelocity, will be changed by collision
};

class gameState
{
    private:
        Player player; //The game will contain one player object
        float startingVelocity; // initial speed of the launch
        int launches; // total number of times played during code running
        float arcLength = 0; // arc length of the current round
        float pointsTot = 0; // current number of total points
        float previousDistance = 0; // arc length of the previous round - to display during stats screen
        float maxDistance = 0; // max distance traveled - for stats screen
        float maxHeight = 0; // arc length for each run and total points collected
    public:
        gameState(int launches, float arcLength, float pointsTot, float startingVelocity, float previousDistance, float maxDistance, float maxHeight); // constructor function
        void initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass);
        void drawMenu(); // displays the main menu to the screen
        void displayGame(); // runs the actual game itself
        void runGame(); // actual launching projectile simulation
        void displayStats(); // displays the statistics page to the screen
        void displayInstructions(); // displays the instructions page to the screen
        void displayCredits(); // displays the credits page to the screen
        void returnToMainMenu(); // button for returning to the main menu - made to simplify code due to repetition
        void SetUpgrades(); // allow for extra upgrades
        void SetAngle(); // set initial angle for the current run
        void UpdatePosition(); // update position on screen
        void EndScreen(); // final results screen, prompts user to play again/return to menu
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

void clearScreen() // reset screen for frames, draw bordering outline
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
    player.image.Open("penguin.png");
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
    int x, y; // location of user's click
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
    initPlayer(0,0,0,0,20,220, playerMass); // start by intitializing projectile's values
    SetUpgrades(); // prompt user to enter upgrades/skip
    SetAngle(); // prompt user to set initial angle
    runGame(); // run projectile sim w/ provided values
    EndScreen(); // display final results, prompt to return to menu/play again
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
            obs.back().image.Open("flappy.png");
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
    // display stats - total distance of arc length, maximum arc length, total launches played, maximum distance/height
    LCD.WriteLine("Launches: ");
    LCD.WriteLine(launches);
    LCD.WriteLine("Previous Distance(m): ");
    LCD.WriteLine(previousDistance);
    LCD.WriteLine("Max Distance(m): ");
    LCD.WriteLine(maxDistance);
    LCD.WriteLine("Max Height(m): ");
    LCD.WriteLine(maxHeight);
    LCD.WriteLine("Current Points Saved: ");
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
    int x, y; // location of user's click
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
    LCD.WriteLine("Total points:");
    LCD.WriteLine(pointsTot);
    LCD.WriteLine("Choose a powerup:");

    // make buttons for each powerup available, display to screen
    Icon speedButton, gliderButton, skipButton;
    speedButton.SetProperties("x2 speed-5,000", 20, 78, 280, 26, WHITE, RED);
    speedButton.Draw();
    gliderButton.SetProperties("x0.75 gravity-10,000", 20, 130, 280, 26, WHITE, RED);
    gliderButton.Draw();
    skipButton.SetProperties("Skip Powerups", 20, 182, 280, 26, WHITE, RED);
    skipButton.Draw();
    // @ts-ignore
    LCD.Update();

    // check if/when one of the buttons are clicked
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y; // location of user's click
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
                pointsTot-=5000; // decrease points by given cost
                startingVelocity *= 2; // doubles the starting velocity
                choice = 1;
            }
        }
        if(gliderButton.Pressed(x,y,1)==1)
        {
            // decrease gravity by 25% - can increase several times?
            if (pointsTot >= 10000)
            {
                pointsTot-=10000; // decrease by given cost value
                player.multiplyAccel(.75); // 3/4 the projectile's acceleration due to gravity
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
    LCD.WriteLine("Choose a launch angle:");

    // create buttons for each of the angles used - 30/45/60/90
    Icon thirtyButton, fortyfiveButton, sixtyButton, ninetyButton; // buttons for each of the angles
    thirtyButton.SetProperties("30 degrees", 40, 26, 240, 26, WHITE, RED); // setting up buttons' values
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
    int x, y; // location of click
    int xtrash, ytrash; // throwaway values
    // check if the user has clicked, then check if the user clicked an angle button - fix x/y velocities by trigonometry
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(thirtyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(30), vy = v*sin(30) 
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
    previousDistance = arcLength;
    LCD.Clear();
    // tell the user how many points they've earned/how far they went and their current point total
    LCD.WriteLine("Distance Traveled: ");
    LCD.WriteLine(arcLength);
    LCD.Write("Total Points: ");
    LCD.WriteLine(pointsTot);
    arcLength = 0;

    // prompt the user to either play again or return to menu
    Icon menuButton, resetButton; // buttons for the main menu/playing again
    menuButton.SetProperties("Return to Menu", 40, 182, 240, 26, WHITE, RED);
    menuButton.Draw();
    resetButton.SetProperties("Play Again", 40, 130, 240, 26, WHITE, RED);
    resetButton.Draw();
    // @ts-ignore
    LCD.Update();

    // check if/when one of the buttons are clicked
    // define extra variables for the while loop
    int choice = -1; // checks if the user has clicked a button
    int x, y; // location of click
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
        if(resetButton.Pressed(x,y,1)==1)
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
    image.Draw(screenXPos,screenYPos);
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
    LCD.Write("X Position: ");
    LCD.WriteLine(xPos);
    LCD.Write("X Velocity:");
    LCD.WriteLine(xVel);
    LCD.Write("Y Position: ");
    LCD.WriteLine(-yPos);
    LCD.Write("Y Velocity:");
    LCD.WriteLine(-yVel);
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
        //Original veloity x direction for player(ogvxp)
        float ogvxp = player->getXVelocity();
        float ogvyp = player->getYVelocity();
        float ogvxo = -xVel;
        float ogvyo = -yVel;

        player->setXVel(((player->mass-mass)*ogvxp+2*mass*ogvxo)/(player->mass+mass));
        player->setYVel(((player->mass-mass)*ogvyp+2*mass*ogvyo)/(player->mass+mass));
        xVel = -((mass-player->mass)*ogvxo+2*player->mass*ogvxp)/(player->mass+mass);
        yVel = -((mass-player->mass)*ogvyo+2*player->mass*ogvyp)/(player->mass+mass);
        hasCollided = true;
    }
}
