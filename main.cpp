#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>

void drawMenu();
void navigateMenu();
void displayGame();
void displayStats();
void displayInstructions();
void displayCredits();


int main() {
    bool running = true;
    while (running)
    {
        LCD.Update();
        drawMenu();
        navigateMenu();
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
    LCD.SetBackgroundColor(BLACK);
    LCD.SetFontColor(BLUE);
    LCD.Clear();
    LCD.FillRectangle(50,50,25,25);
    LCD.FillRectangle(50,100,25,25);
    LCD.FillRectangle(100,50,25,25);
    LCD.FillRectangle(100,100,25,25);
    LCD.WriteAt("Play",55,55);
    LCD.WriteAt("Stats",55,105);
    LCD.WriteAt("Instructions",105,55);
    LCD.WriteAt("Credits",105,105);
    LCD.Update();
}

void navigateMenu()
{
    //LCD.ClearBuffer();
    int choice = -1;
    int x;
    int y;
    int xtrash;
    int ytrash;
    while(choice < 0)
    {
        while(!LCD.Touch(&x,&y)){};
        while(LCD.Touch(&xtrash,&ytrash)){};
        if(x >= 50 && x <= 75 && y >= 50 && y <= 75)
        {
            choice = 1;
            displayGame();
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
    LCD.WriteLine("Instructions here.");
    LCD.Update();
}

void displayCredits()
{
    LCD.Clear();
    LCD.WriteLine("Avery Taylor");
    LCD.Write("Joe Quinn");
    LCD.Update();
}