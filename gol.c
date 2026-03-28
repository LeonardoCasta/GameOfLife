#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"

#define CELLSIZE 30
#define CELLNUMBER 40
#define GRID_DIMENTION (CELLSIZE*CELLNUMBER)
#define BOTTOM 80
#define BUTTON_HEIGHT 60
#define BUTTON_WIDTH 150

short state = 0;
char *buttonText[2] = { "Pause", "Continue" };

typedef void (*action)();                                                                                                                                                                                                                   
                                                                                                                                                                                                                                                        
typedef struct                                                                                                                                                                                                                                          
{                                                                                                                                                                                                                                                       
    Rectangle rect;                                                                                                                                                                                                                                     
    Color color;                                                                                                                                                                                                                                        
    char* text;                                                                                                                                                                                                                                         
    int fontSize;                                                                                                                                                                                                                                       
    action action;                                                                                                                                                                                                                                      
}Button;     

typedef struct {
    int state;
    int nextState;
    Rectangle rect;
    Color color; 
} Cell;

void CreateGrid(Cell (*grid)[CELLNUMBER]){
    for (int i = 0; i < CELLNUMBER; i++){
        for (int j = 0; j < CELLNUMBER; j++){
            struct Rectangle rect = {.x = i*CELLSIZE, .y = j*CELLSIZE, .height = CELLSIZE, .width = CELLSIZE}; 
            grid[i][j] = (Cell){.state = 0, .nextState = 0, rect, WHITE};
        }
    }
}

static const int moves[8][2] = {{-1,1},{0,-1},{1,-1},{-1,0},{1,0},{-1,-1},{0,1},{1,1}};

bool IsValid(int x, int y, int i, int j){
    return i + x >= 0 && i + x < CELLNUMBER &&
           j + y >= 0 && j + y < CELLNUMBER;
}

void DrawButton(Button* btns, Color col){                                                                                                                                                                                                               
    Font font = GetFontDefault();                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                        
    DrawRectangleRec((btns)->rect, col);                                                                                                                                                                                                                
                                                                                                                                                                                                                                                        
    // Get the size of the text to draw                                                                                                                                                                                                                 
    Vector2 textSize = MeasureTextEx(font,                                                                                                                                                                                                              
        btns->text,                                                                                                                                                                                                                                     
        btns->fontSize,                                                                                                                                                                                                                                 
        btns->fontSize*.1f);                                                                                                                                                                                                                            
                                                                                                                                                                                                                                                        
    // Calculate the top-left text position based on the rectangle and alignment                                                                                                                                                                        
    Vector2 textPos = (Vector2){                                                                                                                                                                                                                        
        btns->rect.x + Lerp(0.0f, btns->rect.width  - textSize.x, 0.5),                                                                                                                                                                                 
        btns->rect.y + Lerp(0.0f, btns->rect.height - textSize.y, 0.5)                                                                                                                                                                                  
    };                                                                                                                                                                                                                                                  
                                                                                                                                                                                                                                                        
    // Draw the text                                                                                                                                                                                                                                    
    DrawTextEx(font, btns->text,                                                                                                                                                                                                                        
        textPos,                                                                                                                                                                                                                                        
        btns->fontSize,                                                                                                                                                                                                                                 
        btns->fontSize*.1f, RAYWHITE);                                                                                                                                                                                                                  
} 

void Action(Button *btn){ 
    /*
    0 ^ 1 -> 1
    1 ^ 1 -> 0
    */
    state = state ^ 1;
    btn->text = buttonText[state];
}                                                                                                                                                                                                                                                       
                                                                                                                                                                                                                                                        
void ManageButtons(Button* btns){                                                                                                                                                                                                                       
    Vector2 mousePoint = GetMousePosition();                                                                                                                                                                                                            
                                                                                                                                                                                                                                                        
    //change color if mouse over button                                                                                                                                                                                                             
    Color col = (btns)->color;                                                                                                                                                                                                                    
    int isReleased = false;                                                                                                                                                                                                                         
    if (CheckCollisionPointRec(mousePoint, (btns)->rect)){                                                                                                                                                                                        
        col.a *= 1.5;                                                                                                                                                                                                                               
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) isReleased = true;                                                                                                                                                                            
    }                                                                                                                                                                                                                                               
    DrawButton(btns, col);                                                                                                                                                                                                                        
    if(isReleased){                                                                                                                                                                                                                                 
        (btns)->action(btns);                                                                                                                                                                                               
    }                                                                                                                                                                                                                                               
} 

void createButtons(Button btns[]) {                                                                                                                                                                                                                     
    struct Rectangle rect1 = {
        .x = ((GRID_DIMENTION)/2) - (BUTTON_WIDTH/2), 
        .y = (GRID_DIMENTION) + ((BOTTOM-BUTTON_HEIGHT)/2), 
        .height = BUTTON_HEIGHT, 
        .width = BUTTON_WIDTH
    };                                                                                                                                                                            
    btns[0] = (Button){ rect1, BLUE, "pause", 25, &Action};                                                                                                                                                                                          
}

/*
Any live cell with fewer than two live neighbours dies, as if by underpopulation
Any live cell with two or three live neighbours lives on to the next generation
Any live cell with more than three live neighbours dies, as if by overpopulation
Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction
*/
void SetState(Cell (*grid)[CELLNUMBER], int i, int j){
    int count = 0;
    for(int x = 0; x < 8; x++){
        int a = (i+moves[x][0])%CELLNUMBER;
        if (a == -1) a = 29;
        int b = (j+moves[x][1])%CELLNUMBER;
        if (b == -1) b = 29;
        if(grid[a][b].state == 1){
            count++;
        }
    }
    switch(grid[i][j].state){
        case 1:
            if(count >= 2 && count <= 3){
                grid[i][j].nextState = 1;
            }else {
                grid[i][j].nextState = 0;
            }
            break;
        case 0:
            if(count == 3)
                grid[i][j].nextState = 1;
            break;
    }
    //exit(0);
    //printf("[%d %d] %d %d %d\n",i,j,grid[i][j].state ,grid[i][j].nextState, count);
}

void ManageCells(Cell* cell){                                                                                                                                                                                                                       
    Vector2 mousePoint = GetMousePosition();                                                                                                                                                                                                            
                                                                                                                                                                                                                                                        
    //change color if mouse over button                                                                                                                                                                                   
    int isReleased = false;                                                                                                                                                                                                                         
    if (CheckCollisionPointRec(mousePoint, (cell)->rect)){                                                                                                                                                                                        
        cell->color = GRAY;                                                                                                                                                                                                                               
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isReleased = true;                                                                                                                                                                            
    }  

    if(isReleased){        
        cell->state = cell->state ^ 1;
        cell->nextState = cell->state; 
    }                                                                                                                                                                                                                                               
} 

//update grid
void UpdateGrid(Cell (*grid)[CELLNUMBER]){
    for (int i = 0; i < CELLNUMBER; i++){
        for (int j = 0; j < CELLNUMBER; j++){
            SetState(grid, i, j);
        }
    }
}

//draw grid
void DrawGridR(Cell (*grid)[CELLNUMBER]){
    for (int i = 0; i < CELLNUMBER; i++){
        for (int j = 0; j < CELLNUMBER; j++){
            //set color
            if (grid[i][j].state){
                grid[i][j].color = WHITE;
            }else{
                grid[i][j].color = BLACK;
            }

            if(state) ManageCells(&grid[i][j]);

            DrawRectangleRec(
                grid[i][j].rect,
                grid[i][j].color            
            );
            grid[i][j].state = grid[i][j].nextState;
        }
    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GRID_DIMENTION, GRID_DIMENTION+BOTTOM, "Game of Life");
    SetTargetFPS(20);
    
    //initialize buttons
    Button* btns = (Button*)malloc(sizeof(Button));
    createButtons(btns);

    //using brackets cos i want a pointer to a array of cells on an array or pointers 
    Cell (*grid)[CELLNUMBER] = malloc(sizeof(Cell[CELLNUMBER][CELLNUMBER]));
    CreateGrid(grid);

    grid[0][2].state = 1;
    grid[1][0].state = 1;
    grid[1][2].state = 1;
    grid[2][1].state = 1;
    grid[2][2].state = 1;

    while (!WindowShouldClose())
    {
	
        BeginDrawing();
            switch (state)
            {
                case 0:
                    UpdateGrid(grid);
                    break;
                case 1:
                    break;
            }
            DrawGridR(grid);
            ManageButtons(btns);
            ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
