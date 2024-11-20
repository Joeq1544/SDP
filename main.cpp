#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>

void drawMenu();
//void navigateMenu();
void displayGame();
void displayStats();
void displayInstructions();
void displayCredits();


int main() {
    bool running = true;
    while (running)
    {
        LCD.Update();
        FEHIcon::Icon *buttons;
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
    using namespace FEHIcon;
    LCD.SetBackgroundColor(BLACK);
    LCD.SetFontColor(RED);
    LCD.Clear();
    Icon gameButton;
    Icon statButton;
    Icon instructionButton;
    Icon creditButton;
    gameButton.SetProperties("Play", 40, 26, 240, 26, WHITE, RED);
    statButton.SetProperties("Stats", 40, 78, 240, 26, WHITE, RED);
    instructionButton.SetProperties("Instructions", 40, 130, 240, 26, WHITE, RED);
    creditButton.SetProperties("Credits", 40, 182, 240, 26, WHITE, RED);
    gameButton.Draw();
    statButton.Draw();
    instructionButton.Draw();
    creditButton.Draw();
    LCD.Update();
    int choice = -1;
    int x;
    int y;
    int xtrash;
    int ytrash;
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(gameButton.Pressed(x,y,1)==1)
        {
            displayGame();
            choice = 1;
        }
        if(statButton.Pressed(x,y,1)==1)
        {
            displayStats();
            choice = 1;
        }
        if(instructionButton.Pressed(x,y,1)==1)
        {
            displayInstructions();
            choice = 1;
        }
        if(creditButton.Pressed(x,y,1)==1)
        {
            displayCredits();
            choice = 1;
        }
    }
}

void displayGame()
{
    LCD.Clear();
    LCD.Write("Play game here");
    LCD.Update();
}

void displayStats()
{
    LCD.Clear();
    LCD.WriteLine("Total Distance: 160 meters.");
    LCD.WriteLine("Max Distance: 63 Meters.");
    LCD.WriteLine("Launches: 7");
    LCD.Update();
}

void displayInstructions()
{
    LCD.Clear();
    LCD.WriteLine("Set the angle of the launch.");
    LCD.WriteLine("Hit the launch button when ready.");
    LCD.WriteLine("Collect points from distance traveled.");
    LCD.WriteLine("Buy upgrades with your points and keep playing!");
    LCD.Update();
}

void displayCredits()
{
    LCD.Clear();
    LCD.WriteLine("Avery Taylor");
    LCD.Write("Joe Quinn");
    LCD.Update();
}