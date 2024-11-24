#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>
#include <math.h>
using namespace FEHIcon;

//
//FUNCTIONS DECLARATIONS NOT ATTACHED TO A CLASS
//
void drawMenu();
void displayGame();
void runGame();
void displayStats();
void displayInstructions();
void displayCredits();
void returnToMainMenu();
void clearScreen();

//
//CLASSES
//
class gameVals
{
    private:
    int xPosition, yPosition; // current position of projectile
    float xVelocity, yVelocity; // behind the scenes velocity stuff
    float a = 9.8; // accelaration due to gravity
    float arcLength = 0, pointsTot = 0; // arc length for each run and total points collected
    public:
    void SetUpgrades();
    void UpdatePosition(); // code completed
    void EndScreen(); // code completed
};

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
        void setVals(float screenXPos, float screenYPos);
        void updateScreenPos(float xVel, float yVel);
        
};

class Player : public Projectile
{
    protected:
        float xPos;
        float yPos;
        float xVel;
        float yVel;
        float acceleration = -1;
    public:
        Player(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos) : Projectile(screenXPos, screenYPos){
            this->xPos = xPos;
            this->yPos = yPos;
            this->xVel = xVel;
            this->yVel = yVel;
        };
        void startAnim();
        void updatePos(float deltaTime);
        void endAnim();
        float getXVelocity();
        float getYVelocity();
};

//
//MAIN METHOD
//

int main() {
    bool running = true;
    while (running)
    {
        LCD.Update();
        drawMenu();
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

void drawMenu()
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
    LCD.WriteLine("Learn to Fly"); // display the game title
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

void displayGame() // runs the game itself
{
    Player player(0,0, 3, -5, 30, 180);
    player.startAnim();
    Obstacle obs[10];
    int numObs = 0;
    while(true)
    {
        double rand = std::rand()/(double)(RAND_MAX);
        clearScreen();
        player.updatePos(1.0/10);
        player.draw();
        LCD.Update();
    }

    gameVals runGame; // class for running the game itself
    runGame.SetUpgrades();
    
    returnToMainMenu();
}

void displayStats() // statistics menu
{
    LCD.Clear();
    // display stats - total distance of arc length, maximum arc length, and total launches played
    LCD.WriteLine("Total Distance: 0 meters.");
    LCD.WriteLine("Max Distance: 0 Meters.");
    LCD.WriteLine("Launches: 0");

    returnToMainMenu();
}

void displayInstructions() // instructions menu
{
    LCD.Clear();
    // have issues with text looping - must set text in different lines to work around
    LCD.WriteLine("Set the angle of the");
    LCD.WriteLine("initial launch; hit the");
    LCD.WriteLine("launch button when ready.");
    LCD.WriteLine("Collect points from");
    LCD.WriteLine("distance traveled. Buy");
    LCD.WriteLine("upgrades with your points and keep playing!");

    returnToMainMenu();
}

void displayCredits() // credits menu
{
    LCD.Clear();
    LCD.WriteLine("Avery Taylor");
    LCD.WriteLine("Joe Quinn");
    LCD.WriteLine("Inspired by Learn to Fly");

    returnToMainMenu();
}

void returnToMainMenu()
{
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

//
//FUNCTION DEFINITIONS TIED TO A CLASS
//

// -- definitions for functions used in game itself: --

// put your function definitions for the game here for ease of access
// also, modify the declarations I made above if you need inputs - we likely will for a lot of the main ones in the motion
// a lot of the specific ones (angle, launch, position/screen, collision, etc) will require pointers to permanently modify the values!
// need more formal discussion on the specific logistics of the game to figure out most of this

void gameVals::SetUpgrades() // prompt the user to either purchase a power-up or continue to game
{
    // display points/options to screen
    LCD.Clear();
    LCD.Write("Total points:");
    LCD.WriteLine(pointsTot);
    LCD.WriteLine("Choose a powerup:");

    // make buttons for each powerup available
    Icon speedButton, gliderButton;
    speedButton.SetProperties("Increase initial speed", 40, 182, 240, 26, WHITE, RED);
    speedButton.Draw();
    gliderButton.SetProperties("Reduce Gravity", 40, 130, 240, 26, WHITE, RED);
    gliderButton.Draw();
    gliderButton.SetProperties("Play Again", 40, 130, 240, 26, WHITE, RED);
    gliderButton.Draw();
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
            drawMenu(); // returns to menu
            choice = 1;
        }
        if(gliderButton.Pressed(x,y,1)==1)
        {
            displayGame(); // resets the game
            choice = 1;
        }
    }
}

void gameVals::UpdatePosition() // accepts the current position/velocity/total arclength, updates the values based on the change in time between frames
{
    // accelatarion is 9.8 m/s^2
    // assuming each pixel is a meter?
    // assuming 30fps
    float t = 1.0/30; // need to figure out how many seconds between frames!!!!

    // equations used based on kinematic equations - xf = xi + vt, vf = vi + at
    // calculate change in position
    xPosition+=(xVelocity*t);
    yPosition+=(yVelocity*t);

    // calculate the change in the vertical velocity due to gravity
    yVelocity+=(a*t);

    // calculate change in arclength, add to total arc length
    arcLength+=pow((pow(xPosition, 2))+(pow(yPosition, 2)), 0.5); // displacement equation

}

void gameVals::EndScreen() // displays the final results of the game, accepting the arclength value of that run and current point value
// don't need pointers since none of the values need to be modified
{
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

void Player :: startAnim()
{
    while(screenXPos < 160 && screenYPos > 120)
    {
        screenXPos += 3 * 1.0;
        xPos += 3 * 1.0/10;
        screenYPos -= 2 * 1.0;
        yPos -= 2 * 1.0;
        clearScreen();
        draw();
        LCD.Update();
    }
}

void Player :: updatePos(float deltaTime)
{
    xPos += xVel * deltaTime;
    yPos += yVel * deltaTime;
    yVel += acceleration * deltaTime;
    LCD.WriteLine(xPos);
    LCD.WriteLine(yPos);
}

void Player :: endAnim()
{

}

float Player :: getXVelocity()
{
    return xVel;
}

float Player :: getYVelocity()
{
    return yVel;
}

void Obstacle :: setVals(float screenXPos, float screenYPos)
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
}

void Obstacle :: updateScreenPos(float xVel, float yVel)
{
    screenXPos += xVel;
    screenYPos += yVel;
}
