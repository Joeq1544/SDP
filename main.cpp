#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>
#include <math.h>
using namespace FEHIcon;

// define functions used
void drawMenu();
//void navigateMenu();
void displayGame();
void displayStats();
void displayInstructions();
void displayCredits();
void returnToMainMenu();
// functions used for game

void UpdatePosition(int *x, int *y, float *vx, float *vy, float *al); // still need time between frames to finish

void EndScreen(float arclength, float points); // code completed


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
    LCD.Clear();
    LCD.WriteLine("Play game here");
    
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

// -- definitions for functions used in game itself: --

// put your function definitions for the game here for ease of access
// also, modify the declarations I made above if you need inputs - we likely will for a lot of the main ones in the motion
// a lot of the specific ones (angle, launch, position/screen, collision, etc) will require pointers to permanently modify the values!
// need more formal discussion on the specific logistics of the game to figure out most of this


void UpdatePosition(int *xPosition, int *yPosition, float *xVelocity, float *yVelocity, float *arcLength) // accepts the current position/velocity/total arclength, updates the values based on the change in time between frames
{
    // accelatarion is 9.8 m/s^2
    // assuming each pixel is a meter?
    // time between frames = ??????????????????????????????????????????????????????????????
    float t; // need to figure out how many seconds between frames!!!!
    float a = 9.8; // accelaration due to gravity

    // equations used based on kinematic equations - xf = xi + vt, vf = vi + at
    // calculate change in position
    *xPosition+=((*xVelocity)*t);
    *yPosition+=((*yVelocity)*t);

    // calculate the change in the vertical velocity due to gravity
    *yVelocity+=(a*t);

    // calculate change in arclength, add to total arc length
    *arcLength+=pow((pow(*xPosition, 2))+(pow(*yPosition, 2)), 0.5); // displacement equation

}

void EndScreen(float runLen, float pointTot) // displays the final results of the game, accepting the arclength value of that run and current point value
// don't need pointers since none of the values need to be modified
{
    using namespace FEHIcon; // for the button inputs
    LCD.Clear();
    // tell the user how many points they've earned/how far they went and their current point total
    LCD.Write("Distance Traveled: ");
    LCD.WriteLine(runLen);
    LCD.Write("Total Points: ");
    LCD.WriteLine(pointTot);

    // prompt the user to either play again or return to menu
    Icon menuButton, resetButton;
    menuButton.SetProperties("Return to Menu", 40, 182, 240, 26, WHITE, RED);
    menuButton.Draw();
    resetButton.SetProperties("Play Again", 40, 182, 240, 26, WHITE, RED);
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
