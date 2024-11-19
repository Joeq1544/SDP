#include "FEHLCD.h"
#include "FEHUtility.h"
#include <FEHRandom.h>

void AnimationTraining();

/*
    SDP Training Minigame #1
  
    Make the shapes race across the ​screen.​
    Change their colors when they reach​ the finish line.​
*/
void AnimationTraining() {
    int x1 = 50, y1 = 50;
    int x2 = 50, y2 = 100;
    int r = 10;

    // Draw Racer 1
    LCD.SetFontColor(RED);
    LCD.DrawCircle(x1, y1, r);
    LCD.FillCircle(x1, y1, r);

    // Draw Racer 2
    LCD.SetFontColor(BLUE);
    LCD.DrawCircle(x2, y2, r);
    LCD.FillCircle(x2, y2, r);

    LCD.Update();

    // Make racers move across the screen
    while (x1 < 300 && x2 < 300) {

        // Draw finish line
        LCD.SetFontColor(WHITE);
         LCD.DrawLine(300, 10, 300, 220); 
        
        
        // Clear previous racers by drawing black over them
        // Draw Racer 1
        LCD.SetFontColor(BLACK);
        LCD.DrawCircle(x1, y1, r);
        LCD.FillCircle(x1, y1, r);

        // Draw Racer 2
        LCD.SetFontColor(BLACK);
        LCD.DrawCircle(x2, y2, r);
        LCD.FillCircle(x2, y2, r);

        // Update racer coordinates
        x1 += Random.RandInt() / 10000;
        x2 += Random.RandInt() / 7000;

        
        // Redraw racers with different colors - don't forget to sleep!
        // Draw Racer 1
        LCD.SetFontColor(RED);
        LCD.DrawCircle(x1, y1, r);
        LCD.FillCircle(x1, y1, r);
        // Draw Racer 2
        LCD.SetFontColor(BLUE);
        LCD.DrawCircle(x2, y2, r);
        LCD.FillCircle(x2, y2, r);
        Sleep(100);
    }
    if(x1>=300)
    {
        // Draw Racer 1
        LCD.SetFontColor(YELLOW);
        LCD.DrawCircle(x1, y1, r);
        LCD.FillCircle(x1, y1, r);

        // Draw Racer 2
        LCD.SetFontColor(BURLYWOOD);
        LCD.DrawCircle(x2, y2, r);
        LCD.FillCircle(x2, y2, r);
    }
    else{
        // Draw Racer 1
        LCD.SetFontColor(BURLYWOOD);
        LCD.DrawCircle(x1, y1, r);
        LCD.FillCircle(x1, y1, r);

        // Draw Racer 2
        LCD.SetFontColor(YELLOW);
        LCD.DrawCircle(x2, y2, r);
        LCD.FillCircle(x2, y2, r);
    }
}

int main() {
    // Clear background
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    AnimationTraining();

    while (1) {
        LCD.Update();
        // Never end
    }
    return 0;
}