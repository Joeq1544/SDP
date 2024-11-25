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
#define deltaFrameTime 1/10.0

//
//FUNCTIONS DECLARATIONS NOT ATTACHED TO A CLASS
//
void clearScreen();

//
//CLASSES
//

class Projectile
{
    protected:
        float screenXPos;
        float screenYPos;
        
    public:
        Projectile(){};
        Projectile(float screenXPos, float screenYPos);
        void draw();
};

class Obstacle : public Projectile
{
    public:
        Obstacle(){};
        Obstacle(float screenXPos, float ScreenYPos);
        void setVals(float screenXPos, float screenYPos);
        bool updateScreenPos(float xVel, float yVel, float deltaTime); 
};

class Player : public Projectile
{
    protected:
        float xPos;
        float yPos;
        float xVel;
        float yVel;
        float acceleration = 1;
    public:
        Player() : Projectile(){};
        Player(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos) : Projectile(screenXPos, screenYPos){
            this->xPos = xPos;
            this->yPos = yPos;
            this->xVel = xVel;
            this->yVel = yVel;
        };
        void init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos);
        void startAnim();
        void updatePos(float deltaTime);
        void endAnim(vector<Obstacle> obs);
        float getXVelocity();
        float getYVelocity();
        float getY();
        void multiplyAccel(float);
        void multiplyXVel(float);
        void multiplyYVel(float);
};

class gameState
{
    private:
        Player player;
        int launches; // total number of times played during code running
        float arcLength, pointsTot; // arc length for each run and total points collected
    public:
        gameState(int launches, float arcLength, float pointsTot);
        void initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos);
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
        LCD.Update();
        gameState game(0,0,0);
        game.initPlayer(0,0,10,-10,20,20);
        game.drawMenu();
        running = false;
    }
    while(1)
    {
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

gameState :: gameState(int launches, float arclength, float pointsTot)
{
    this->launches = launches;
    this->arcLength = arcLength;
    this->pointsTot = pointsTot;
}

void gameState :: initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos)
{
    player.init(xPos, yPos, xVel, yVel, screenXPos, screenYPos);
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

    //SetUpgrades();
    runGame();
    returnToMainMenu();
}

void gameState :: runGame()
{
    player.startAnim();
    vector<Obstacle> obs;
    while(player.getY()<0)
    {
        double rand = std::rand()/(double)(RAND_MAX);
        if(rand < (10-obs.size())/40.0)
        {
            //cout << obs.size() << "\n";
            if(std::rand()/(double)(RAND_MAX)<.5)
            {
                if(player.getYVelocity()<0)
                {
                    obs.emplace_back(std::rand()/(double)(RAND_MAX)*250+40,std::rand()/(double)(RAND_MAX)*10+10);
                }
                else
                {
                    obs.emplace_back(std::rand()/(double)(RAND_MAX)*250+40,std::rand()/(double)(RAND_MAX)*5+220);
                }
            }
            else
            {
                obs.emplace_back(std::rand()/(double)(RAND_MAX)*10+300,std::rand()/(double)(RAND_MAX)*220+10);
            }
        }
        clearScreen();
        player.updatePos(1.0/10);
        player.draw();
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(player.getXVelocity(), player.getYVelocity(), deltaFrameTime) == true;}), obs.end());
        for (int i = 0; i < obs.size(); i++)
        {
            obs.at(i).draw();
            //cout << i <<" - "<< obs.at(i).screenXPos<<" - "<<obs.at(i).screenYPos<<"\n";
        }
        LCD.Update();
    }
    player.endAnim(obs);
}

void gameState :: displayStats() // statistics menu
{
    LCD.Clear();
    // display stats - total distance of arc length, maximum arc length, and total launches played
    LCD.WriteLine("Previous Distance(m): ");
    LCD.WriteLine(arcLength);
    LCD.WriteLine("Max Distance(m): ");
    LCD.WriteLine(pointsTot);
    LCD.WriteLine("Launches: ");
    LCD.WriteLine(launches);
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
    speedButton.SetProperties("Double initial speed", 40, 78, 240, 26, WHITE, RED);
    speedButton.Draw();
    gliderButton.SetProperties("Low gravity", 40, 130, 240, 26, WHITE, RED);
    gliderButton.Draw();
    skipButton.SetProperties("Skip Powerups", 40, 182, 240, 26, WHITE, RED);
    skipButton.Draw();
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
            player.multiplyXVel(2);
            player.multiplyYVel(2);
            choice = 1;
        }
        if(gliderButton.Pressed(x,y,1)==1)
        {
            // decrease gravity by 25% - can increase several times?
            player.multiplyAccel(.75);
            choice = 1;
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
            choice = 1;
        }
        if(fortyfiveButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(45), vy = v*sin(45) - both sin/cos of 45 are the same so this one should be easy
            choice = 1;
        }
        if(sixtyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(60), vy = v*sin(60) - just reversing sines/cosines of 30
            choice = 1;
        }
        if(ninetyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(90), vy = v*sin(90) - vx will be zero, vy will be the full initial velocity
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
    using namespace FEHIcon; // for the button inputs
    LCD.Clear();
    // tell the user how many points they've earned/how far they went and their current point total
    LCD.Write("Distance Traveled: ");
    LCD.WriteLine(arcLength);
    LCD.Write("Total Points: ");
    LCD.WriteLine(pointsTot);

    // prompt the user to either play again or return to menu
    Icon menuButton, resetButton;
    menuButton.SetProperties("Return to Menu", 40, 182, 240, 26, WHITE, RED);
    menuButton.Draw();
    resetButton.SetProperties("Play Again", 40, 130, 240, 26, WHITE, RED);
    resetButton.Draw();
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

Projectile :: Projectile(float screenXPos, float screenYPos)
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
}

void Projectile :: draw()
{
    LCD.DrawCircle(screenXPos, screenYPos, 10);
}

void Player :: init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->xVel = xVel;
    this->yVel = yVel;
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
}

void Player :: startAnim()
{
    while(screenXPos < 160 && screenYPos > 120)
    {
        screenXPos += xVel * 1.0/5;
        xPos += xVel * 1.0/5;
        screenYPos += yVel * 1.0/5;
        yPos += yVel * 1.0/5;
        yVel += acceleration * 1.0/50;
        clearScreen();
        draw();
        LCD.DrawHorizontalLine(230, 0, 319);
        LCD.Update();
    }
}

void Player :: updatePos(float deltaTime)
{
    xPos += xVel * 1.0/5;
    yPos += yVel * 1.0/5;
    yVel += acceleration * 1.0/50;
    LCD.WriteLine(xVel);
    LCD.WriteLine(yVel);
}

void Player :: endAnim(vector<Obstacle> obs)
{
    float floorPos = 239;
    while(screenXPos < 300 && screenYPos < floorPos - 10)
    {
        screenXPos += xVel * 1.0/5;
        xPos += xVel * 1.0/5;
        screenYPos += yVel * 1.0/5;
        yPos += yVel * 1.0/5;
        yVel += acceleration * 1.0/50;
        clearScreen();
        draw();
        floorPos -= yVel * 1.0/5;
        floorPos = min((int)floorPos, 230);
        LCD.DrawHorizontalLine(floorPos, 0, 319);
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(getXVelocity(), getYVelocity(), deltaFrameTime) == true;}), obs.end());
        for (int i = 0; i < obs.size(); i++)
        {
            obs.at(i).draw();
            //cout << i <<" - "<< obs.at(i).screenXPos<<" - "<<obs.at(i).screenYPos<<"\n";
        }
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

void Player :: multiplyXVel(float coef)
{
    xVel *= coef;
}

void Player :: multiplyYVel(float coef)
{
    yVel *= coef;
}

Obstacle :: Obstacle(float screenXPos, float screenYPos)
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
}
void Obstacle :: setVals(float screenXPos, float screenYPos)
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
}

bool Obstacle :: updateScreenPos(float xVel, float yVel, float deltaTime)
{
    screenXPos -= xVel * deltaTime;
    screenYPos -= yVel * deltaTime;
    // LCD.WriteAt(screenXPos,screenXPos,screenYPos);
    // LCD.WriteAt(screenYPos,screenXPos,screenYPos+10);
    if(screenXPos < 15 || screenYPos > 225 || screenXPos > 320 || screenYPos < 0)
    {
        cout << "remove\n";
        return true;
    }
    return false;
}
