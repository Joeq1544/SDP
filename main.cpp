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

//CONSTANTS - written by Joe Quinn
#define deltaFrameTime 1/5.0 // fps of the game
#define playerMass 10 // initial player mass for collision physics
#define obMass 1 // initial obstacle mass for collision physics

//
//FUNCTIONS DECLARATIONS NOT ATTACHED TO A CLASS
//
void clearScreen(); // resets the screen

//
//CLASSES
//

class Player; 

class Projectile // generic projectile stats used for player/obstacles- Written by Joe Quinn
{
    public:
        float screenXPos; // x/y position on the screen
        float screenYPos;
        float xVel; // behind the scenes - velocity for calculating change in values
        float yVel;
        float acceleration = 1; // base accelaration before powerups
        float mass; // for collision calculations
        FEHImage image; // visuals of the projectile
    public:
        Projectile(){}; // constructor function
        Projectile(float screenXPos, float screenYPos, float xVel, float yVel, float mass);
        void draw(); // draws the projectile to the screen
};

class Obstacle : public Projectile // values of each obstacle - written by Joe Quinn
{
    private:
        bool hasCollided = false; // checks if the user has collided with each projectile
    public:
        Obstacle(){}; // constructor function
        Obstacle(float screenXPos, float screenYPos, float xVel, float yVel, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass)
        {
            // this->screenXPos = screenXPos;
            // this->screenYPos = screenYPos;
            // this->xVel = xVel;
            // this->yVel = yVel;
        };
        bool updateScreenPos(float pXVel, float PYVel); // updates the projectile's position to the screen
        void collide(Player *player); // checks if the user has collided with the projectile
        friend class Player; // includes other variables from Projectile class to Obstacle
};

class Player : public Projectile // values of the player's projectile - Written by Avery Taylor, updated by Joe Quinn
{
    protected:
        float xPos; // player's current x position in air
        float yPos; // player's current y position in air
    public:
        Player() : Projectile(){}; // constructor function
        Player(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass) : Projectile(screenXPos, screenYPos, xVel, yVel, mass){
            this->xPos = xPos;
            this->yPos = yPos;
        };
        void init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass); // initialize values 
        void startAnim(); // runs the initial part of the launch before camera moves relative to projectile
        void updatePos(float deltaTime); // calculates changes in velocity/position, displays values to screen
        void endAnim(vector<Obstacle> obs); // runs the final part of the launch after camera moves relative to projectile
        float getXVelocity(); // saves values from calculations of x/y velocities/position
        float getYVelocity();
        float getY();
        void multiplyAccel(float); // modifies the accelaration due to gravity if user selects the low g powerup
        void setXVel(float xVel); // update x/y velocities with kinematic calculations
        void setYVel(float yVel);
};

class gameState // essential values of game - written by Avery Taylor
{
    private:
        Player player;
        float startingVelocity; // initial speed of the launch
        int launches; // total number of times played during code running
        float arcLength = 0; // arc length of the current round
        float pointsTot = 0; // current number of total points
        float previousDistance = 0; // arc length of the previous round - to display during stats screen
        float maxDistance = 0; // max distance traveled - for stats screen
        float maxHeight = 0; // arc length for each run and total points collected
    public:
        gameState(int launches, float arcLength, float pointsTot, float startingVelocity, float previousDistance, float maxDistance, float maxHeight); // constructor function
        void initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass); // initialize player's location
        void drawMenu(); // displays the main menu to the screen
        void displayGame(); // runs the actual game itself
        void runGame(); // actual launching projectile simulation
        void displayStats(); // displays the statistics page to the screen
        void displayInstructions(); // displays the instructions page to the screen
        void displayCredits(); // displays the credits page to the screen
        void returnToMainMenu(); // button for returning to the main menu - made to simplify code due to repetition
        void SetUpgrades(); // allow for extra upgrades
        void SetAngle(); // set initial angle for the current run
        void UpdatePosition(); // calculates arc length of current run
        void EndScreen(); // final results screen, prompts user to play again/return to menu
};

//
//MAIN METHOD
//

int main() { // written by Joe Quinn
    bool running = true;
    while (running)
    {
        // @ts-ignore
        LCD.Update();
        gameState game(0,0,0,10,0,0,0);
        game.drawMenu(); // opens the initial menu, runs the overall game due to nested functions
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

void clearScreen() // reset screen for frames, draw bordering outline - Written by Joe Quinn
{
    LCD.Clear();
    LCD.DrawRectangle(0,0,319,239);
}

//
//FUNCTION DEFINITIONS TIED TO A CLASS
//

gameState :: gameState(int launches, float arclength, float pointsTot, float startingVelocity, float maxDistance, float maxHeight, float previousDistance) // constructor function for gameState class - written by Avery Taylor, updated by Joe Quinn
{
    this->launches = launches;
    this->arcLength = arcLength;
    this->pointsTot = pointsTot;
    this->startingVelocity = startingVelocity;
    this->maxDistance = maxDistance;
    this->maxHeight = maxHeight;
    this->previousDistance = previousDistance;
}

void gameState :: initPlayer(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass) // initializes datat needed for player's projectile - written by Avery Taylor, updated by Joe Quinn
{
    player.init(xPos, yPos, xVel, yVel, screenXPos, screenYPos, mass);
    player.image.Open("penguin.png"); // opens the file for the player's character
}

void gameState :: drawMenu() // opens the initial main menu - Written by Joe Quinn
{
    // set standard values for menu
    LCD.SetBackgroundColor(BLACK);
    LCD.SetFontColor(RED);
    LCD.Clear();
    // define stats for main 4 buttons on menu, display to screen
    Icon gameButton; // play the game
    Icon statButton; // display statistics
    Icon instructionButton; // display instructions
    Icon creditButton; // display game credits
    gameButton.SetProperties("Play", 40, 26, 240, 26, WHITE, RED); // displays buttons to screen
    statButton.SetProperties("Stats", 40, 78, 240, 26, WHITE, RED);
    instructionButton.SetProperties("Instructions", 40, 130, 240, 26, WHITE, RED);
    creditButton.SetProperties("Credits", 40, 182, 240, 26, WHITE, RED);
    // draw values to the screen
    LCD.WriteLine("Learn to Fly 4"); // display the game title
    gameButton.Draw(); // displays buttons
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
        while(!LCD.Touch(&x,&y)){}; // while user has not clicked, do nothing
        while(LCD.Touch(&xtrash,&ytrash)){}; // user has clicked
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

void gameState :: displayGame() // runs the game itself - Written by Avery Taylor, updated by Joe Quinn
{
    initPlayer(0,0,0,0,20,220, playerMass); // start by intitializing projectile's values
    SetUpgrades(); // prompt user to enter upgrades/skip
    SetAngle(); // prompt user to set initial angle
    runGame(); // run projectile sim w/ provided values
    EndScreen(); // display final results, prompt to return to menu/play again
}

void gameState :: runGame() // runs the projectile motion sim - Written by Joe Quinn
{
    float runHeight = 0; // sets initial height to 0
    player.startAnim(); // runs the initial part of the launch before camera moves with the player
    vector<Obstacle> obs; // prepares randomized obstacles
    while(player.getY()<0) // while the player is still in the air, rum the simulation
    {
        if(player.yVel > 0 && runHeight ==0)
        {
            runHeight = -player.getY();
        }
        double rand = std::rand()/(double)(RAND_MAX); // creates the randomized projectiles
        if(rand < (10-obs.size())/40.0)
        {
            if(std::rand()/(double)(RAND_MAX)<.5) // if the random value is small enough, create a new projectile
            {
                if(player.getYVelocity()<0) // if user is descending, update values of position accordingly
                {
                    int x = std::rand()/(double)(RAND_MAX)*250+40; // where the obstacle spawns in
                    int y = std::rand()/(double)(RAND_MAX)*10+10;
                    Obstacle temp(x,y,0, 0, obMass); // creates a temporary class for the new projectile
                    obs.push_back(temp);
                }
                else // user is ascending - update obstacle stats accordingly
                {
                    int x = std::rand()/(double)(RAND_MAX)*250+40; // where the obstacle spawns in
                    int y = std::rand()/(double)(RAND_MAX)*5+210;
                    Obstacle temp(x,y,0, 0, obMass); // creates a temporary class for the new projectile
                    obs.push_back(temp);
                }
            }
            else // creating more obstacles
            {
                int x = std::rand()/(double)(RAND_MAX)*10+300;
                int y = std::rand()/(double)(RAND_MAX)*210+10;
                Obstacle temp(x,y,0, 0, obMass);
                obs.push_back(temp);
            }
            obs.back().image.Open("flappy.png"); // graphic for obstacles
        }
        clearScreen();
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(player.xVel,player.yVel) == true;}), obs.end()); // removes obstacles if went off screen
        for (int i = 0; i < obs.size(); i++) // draws each of the obstacles still on screen to updated values
        {
            obs.at(i).draw(); // draws obstacles to screen
            obs.at(i).collide(&player); // checks if collided with player
        }
        player.updatePos(1.0/10); // updates player's position/velocity, displays to screen
        UpdatePosition(); // updates arclength of current run
        player.draw(); // draw's player character
        // @ts-ignore
        LCD.Update();
    }
    player.endAnim(obs); // camera stops moving with player, runs final part of projectile motion
    maxHeight = max(runHeight,maxHeight); // sees if the height of the run was greater than the previously saved max height for stats
    maxDistance = max(arcLength, maxDistance); // sees if the arclength of the run was greater than the previously saved max arclength for stats
}

void gameState :: displayStats() // statistics menu - written by Avery Taylor
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

void gameState :: displayInstructions() // instructions menu - Written by Avery Taylor
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

void gameState :: displayCredits() // credits menu - Written by Avery Taylor
{
    LCD.Clear();
    LCD.WriteLine("Avery Taylor");
    LCD.WriteLine("Joe Quinn");
    LCD.WriteLine("Inspired by Learn to Fly");
    // prompt user to return to main menu
    returnToMainMenu();
}

void gameState :: returnToMainMenu() // simplify code on menus since each screen needed a button to return to main menu - Written by Avery Taylor, function made by Joe Quinn
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
    while(choice < 0) // while the user has not clicked a button, check for when the user clicks a button
    {
        while(!LCD.Touch(&x,&y)){}; // user has not clicked
        while(LCD.Touch(&xtrash,&ytrash)){}; // user has clicked
        if(menuButton.Pressed(x,y,1)==1)
        {
            drawMenu(); // returns to menu
            choice = 1; // confirm button press
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
                choice = 1; // confirm button press
            }
        }
        if(gliderButton.Pressed(x,y,1)==1)
        {
            // decrease gravity by 25% - can increase several times?
            if (pointsTot >= 10000)
            {
                pointsTot-=10000; // decrease by given cost value
                player.multiplyAccel(.75); // 3/4 the projectile's acceleration due to gravity
                choice = 1; // confirm button press
            }
        }
        if(skipButton.Pressed(x,y,1)==1)
        {
            // user has bought no powerups - no modification to values needed
            // do nothing
            choice = 1; // confirm button press
        }
    }
}

void gameState::SetAngle() // prompt the user to set the launch angle for the projectile - Written by Avery Taylor
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
            player.init(0,0,-startingVelocity*cos(60),startingVelocity*sin(60),20,220, playerMass); // Written by Joe Quinn
            choice = 1; // confirm button press
        }
        if(fortyfiveButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(45), vy = v*sin(45) - both sin/cos of 45 are the same so this one should be easy
            player.init(0,0,startingVelocity*cos(45),-startingVelocity*sin(45),20,220, playerMass); // Written by Joe Quinn
            choice = 1; // confirm button press
        }
        if(sixtyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(60), vy = v*sin(60) - just reversing sines/cosines of 30
            player.init(0,0,startingVelocity*cos(30), startingVelocity*sin(30),20,220, playerMass); // Written by Joe Quinn
            choice = 1; // confirm button press
        }
        if(ninetyButton.Pressed(x,y,1)==1)
        {
            // vx = v*cos(90), vy = v*sin(90) - vx will be zero, vy will be the full initial velocity
            player.init(0,0,0,-startingVelocity,20,220, playerMass); // Written by Joe Quinn
            choice = 1; // confirm button press
        }
    }

}

void gameState::UpdatePosition() // updates the arclength based on the change in motion/time between frames - written by Avery Taylor
{
    // calculate change in arclength, add to total arc length of current run
    arcLength+=pow((pow(player.getXVelocity(), 2))+(pow(player.getYVelocity(), 2)), 0.5); // displacement equation
}

void gameState::EndScreen() // displays the final results of the game, accepting the arclength value of that run and current point value - Written by Avery Taylor
{
    // increase total launches for stats screen, increase total points by given arclength
    launches++; // increase total number of launches/times played by one
    pointsTot+=arcLength; // add current arclength to total points
    previousDistance = arcLength; // saves previous run's arc length for stats screen
    LCD.Clear();
    // tell the user how many points they've earned/how far they went and their current point total
    LCD.WriteLine("Distance Traveled: ");
    LCD.WriteLine(arcLength);
    LCD.Write("Total Points: ");
    LCD.WriteLine(pointsTot);
    arcLength = 0; // resets arc length for next run - Joe Quinn

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
            choice = 1; // confirm button press
        }
        if(resetButton.Pressed(x,y,1)==1)
        {
            displayGame(); // resets the game
            choice = 1; // confirm button press
        }
    }
}

Projectile :: Projectile(float screenXPos, float screenYPos, float xVel, float yVel, float mass) // projectile constructor function - Written by Joe Quinn
{
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
    this->xVel = xVel;
    this->yVel = yVel;
    this->mass = mass;
}

void Projectile :: draw() // draws projectile(player/object)'s image to the screen at their current position
{
    image.Draw(screenXPos,screenYPos);
}

void Player :: init(float xPos, float yPos, float xVel, float yVel, float screenXPos, float screenYPos, float mass) // initialize values for the player's projectile to Player Class - Written by Joe Quinn
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->xVel = xVel;
    this->yVel = yVel;
    this->screenXPos = screenXPos;
    this->screenYPos = screenYPos;
    this->mass = mass;
}

void Player :: startAnim() // part of launch at beginning when camera is not following player yet - Written by Joe Quinn
{
    while(screenXPos < 160 && screenYPos > 120) // while still at starting part of screen, update the player's projectile to move
    {
        screenXPos += xVel * deltaFrameTime; // updating values to use on motion/display to stats
        xPos += xVel * deltaFrameTime;
        screenYPos += yVel * deltaFrameTime;
        yPos += yVel * deltaFrameTime;
        yVel += acceleration * deltaFrameTime*1.0/10;
        clearScreen();
        draw(); // draw the player's projectile
        LCD.DrawHorizontalLine(230, 0, 319); // create the "ground" on the screen
        // @ts-ignore
        LCD.Update();
    }
}

void Player :: updatePos(float deltaTime) // calculates the user's x/y velocity/position and display stats to screen - Written by Avery Taylor, updated by Joe Quinn
{
    // calculate position and y-velocity
    xPos += xVel * deltaFrameTime;
    yPos += yVel * deltaFrameTime;
    yVel += acceleration * deltaFrameTime * 1.0/10;
    // display stats to screen
    LCD.Write("X Position: ");
    LCD.WriteLine(xPos);
    LCD.Write("X Velocity:");
    LCD.WriteLine(xVel);
    LCD.Write("Y Position: ");
    LCD.WriteLine(-yPos);
    LCD.Write("Y Velocity:");
    LCD.WriteLine(-yVel);
}

void Player :: endAnim(vector<Obstacle> obs) // last part of projectile motion, camera stops moving with projectile again and floor gets closer - Written by Joe Quinn
{
    float floorPos = 239; // location of the floor line
    while(screenXPos < 300 && screenYPos < floorPos - 10) // while projectile has not hit the floor yet/ended the motion, keep updating the character's values
    {
        screenXPos += xVel * deltaFrameTime; // update position/velocity of player
        xPos += xVel * deltaFrameTime;
        screenYPos += yVel * deltaFrameTime;
        yPos += yVel * deltaFrameTime;
        yVel += acceleration * deltaFrameTime*1.0/10;
        clearScreen();
        draw(); // draw projectile to screen
        floorPos -= yVel * 1.0/5; // update floor position to move closer to player
        floorPos = min((int)floorPos, 230);
        LCD.DrawHorizontalLine(floorPos, 0, 319); // draws the floor
        obs.erase(remove_if(obs.begin(),obs.end(),[&](Obstacle& ob) {return ob.updateScreenPos(xVel, yVel) == true;}), obs.end()); // removes objects not on screen
        for (int i = 0; i < obs.size(); i++) // draws any obstacles still on screen
        {
            obs.at(i).draw();
        }
        // @ts-ignore
        LCD.Update();
    }
}

float Player :: getXVelocity() // saves current x velocity for in-code calculations - Written by Joe Quinn
{
    return xVel;
}

float Player :: getYVelocity() // saves current y velocity to the Player class for use in Player class functions - Written by Joe Quinn
{
    return yVel;
}

float Player :: getY() // saves current y position to the PLayer class for use in Player class functions - Written by Joe Quinn
{
    return yPos;
}

void Player :: multiplyAccel(float coef) // powerup for accelaration, decrease by given percent value - Written by Avery Taylor, function made by Joe Quinn
{
    acceleration *= coef;
}

void Player::setXVel(float xVel) // saves current x velocity to the Player class for use in Player class functions - Written by Joe Quinn
{
    this->xVel = xVel;
}

void Player::setYVel(float yVel) // saves current y velocity to the Player class for use in Player class functions - Written by Joe Quinn
{
    this->yVel = yVel;
}

bool Obstacle :: updateScreenPos(float pXVel, float pYVel) // updates obstacle's position to screen, checks if still on screen - Written by Joe Quinn
{
    screenXPos -= xVel * deltaFrameTime+pXVel*deltaFrameTime; // updates screen position
    screenYPos -= yVel * deltaFrameTime+pYVel*deltaFrameTime;
    //yVel += acceleration * deltaFrameTime * 1.0/10;
    if(screenXPos < 10 || screenYPos > 240 || screenXPos > 320 || screenYPos < 10) // check if the obstacle is off the screen, tells code to remove object if so
    {
        return true;
    }
    return false;
}

void Obstacle :: collide(Player *player) // if user has collided with an obstacle, the obstacle begins free fall and the user's/obstacle's velocity is updated from collision - Written by Joe Quinn
{
    float distance = pow(pow(player->screenXPos-screenXPos,2)+pow(player->screenYPos-screenYPos,2),.5); // hitbox between player/obstacle
    if(distance < 20 && !hasCollided)
    {
        //Original veloity x direction for player(ogvxp)
        float ogvxp = player->getXVelocity();
        float ogvyp = player->getYVelocity();
        float ogvxo = -xVel; // motion of obstacle is inverse of current player motion
        float ogvyo = -yVel;

        player->setXVel(((player->mass-mass)*ogvxp+2*mass*ogvxo)/(player->mass+mass)); // momentum collision equations
        player->setYVel(((player->mass-mass)*ogvyp+2*mass*ogvyo)/(player->mass+mass));
        xVel = -((mass-player->mass)*ogvxo+2*player->mass*ogvxp)/(player->mass+mass);
        yVel = -((mass-player->mass)*ogvyo+2*player->mass*ogvyp)/(player->mass+mass);
        hasCollided = true; // tells code that the user has collided with the given projectile
    }
}
