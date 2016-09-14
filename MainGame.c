#include "myLib.h"
#include <stdlib.h>
#include <stdio.h>

#include "XWING.h"
#include "TIEFIGHTER.h"
#include "DEATHSTAR.h"
#include "MENU.h"
#include "WIN.h"
#include "GAMEOVER.h"


typedef struct {
  int row;
  int col;
  int rdel;
  int cdel;
  int width;
  int height;
  int health;
  int healthdel;
  const unsigned short *image;
  int alive;
  int canShoot;
  int canShootTwo;
} GAMEOBJ;

typedef struct {
  int row;
  int col;
  int rdel;
  int cdel;
  int width;
  int height;
  int shot;
  int delay;
  u16 color;
} BLAST;

int main() {
  REG_DISPCTL = MODE3 | BG2_ENABLE;

  while (1) {
    int canpress = 0;
    int over = 0;
    int won = 0;

    while (KEY_DOWN_NOW(BUTTON_A) == 0 && canpress == 0) {
      drawImage3(0, 0, MENU_WIDTH, MENU_HEIGHT, MENU);
      drawRect(148, 0, 12, SCREENWIDTH, GRAY);
    }

    canpress = 0;

    fillScreen(BLACK);
    GAMEOBJ player;
    player.rdel = 4;
    player.cdel = 4;
    player.width = XWING_WIDTH;
    player.height = XWING_HEIGHT;
    player.health = 100;
    player.healthdel = -1;
    player.image = XWING;
    player.alive = 1;
    player.canShoot = 1;
    player.canShootTwo = 1;
    player.row = (9 * (SCREENHEIGHT / 10)) - player.height;
    player.col = (SCREENWIDTH / 2) - (player.width / 2);
    GAMEOBJ *playerPointer = &player;

    BLAST leftShark;
    leftShark.row = 0;
    leftShark.col = 0;
    leftShark.rdel = -5;
    leftShark.cdel = 0;
    leftShark.width = 1;
    leftShark.height = 4;
    leftShark.shot = 0;
    leftShark.delay = 0;
    leftShark.color = CYAN;

    BLAST rightShark;
    rightShark.row = 0;
    rightShark.col = 0;
    rightShark.rdel = -5;
    rightShark.cdel = 0;
    rightShark.width = 1;
    rightShark.height = 4;
    rightShark.shot = 0;
    rightShark.delay = 0;
    rightShark.color = CYAN;

    int baseHealth = 100;
    int baseInc = -1;
    //unsigned int lives = 10;


    GAMEOBJ *enemyPointer;
    GAMEOBJ enemies[4];
    BLAST dam[(sizeof(enemies) / sizeof(enemies[0]))];
    int enemySpawnDelay = 0;
    int incrementDelay = 0;
    unsigned int numEnem = 0;
    unsigned int numObj = 1;
    GAMEOBJ oldObj[100];
    int bullspeed = 1;
    unsigned int numBlasts = 1;
    unsigned int killed = 0;
    unsigned int inDelay = 4;
    unsigned int spDelay = 100;
    unsigned int shDelay = 40;
    unsigned int spawned = 0;
    int killedAgain = 0;

    BLAST oldBlast[100];

    for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
      dam[i].row = 0;
      dam[i].col = 0;
      dam[i].rdel = bullspeed;
      dam[i].cdel = 0;
      dam[i].width = 1;
      dam[i].height = 4;
      dam[i].shot = 0;
      dam[i].delay = 0;
      dam[i].color = RED;
    }

    while (over == 0 && (KEY_DOWN_NOW(BUTTON_SELECT)) == 0) {
      //Key Bindings for Player
      if(KEY_DOWN_NOW(BUTTON_UP))
      {
        if (player.row != 0) {
          if (player.row - player.rdel < 0) {
            playerPointer -> row = 0;
          } else {
            playerPointer -> row -= player.rdel;
          }
        }
      }
      if(KEY_DOWN_NOW(BUTTON_DOWN))
      {
        if (player.row != 150 - player.height) {
          if (player.row + player.rdel >= 150 - player.height) {
            playerPointer -> row = 150 - player.height;
          } else {
            playerPointer -> row += player.rdel;
          }
        }
      }
      if(KEY_DOWN_NOW(BUTTON_LEFT))
      {
        if (player.col != 0) {
          if (player.col - player.cdel < 0) {
            playerPointer -> col = 0;
          } else {
            playerPointer -> col -= player.cdel;
          }
        }
      }
      if(KEY_DOWN_NOW(BUTTON_RIGHT))
      {
        if (player.col != SCREENWIDTH - player.width - 1) {
          if (player.col + player.cdel >= SCREENWIDTH - player.width) {
            playerPointer -> col = SCREENWIDTH - player.width - 1;
          } else {
            playerPointer -> col += player.cdel;
          }
        }
      }
      if(KEY_DOWN_NOW(BUTTON_A) && canpress == 1) {
        if (player.canShoot == 1) {
          leftShark.shot = 1;
          leftShark.row = player.row + leftShark.height;
          leftShark.col = player.col;
          player.canShoot = 0;
        }
      }
      if(KEY_DOWN_NOW(BUTTON_B)) {
        if (player.canShootTwo == 1) {
          rightShark.shot = 1;
          rightShark.row = player.row + leftShark.height;
          rightShark.col = player.col + player.width;
          player.canShootTwo = 0;
        }
      }

      if (KEY_DOWN_NOW(BUTTON_A) == 0) {
        canpress = 1;
      }

      //Enemy Spawn and Movement Control
      for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
        if (enemies[i].alive == 1) {
          enemyPointer = &enemies[i];
          if (enemies[i].row > SCREENHEIGHT) {
            enemyPointer -> alive = 0;
            baseHealth -= 5;
            numEnem--;
          }

          if (enemies[i].health == 0) {
            enemyPointer -> alive = 0;
            numEnem--;
            killed++;
            killedAgain = 1;
          }

          if (incrementDelay == 0) {
            enemyPointer -> row += enemies[i].rdel;
          }

          if (dam[i].shot == 1) {
            if (dam[i].row >= 150) {
              baseHealth += baseInc;
              enemies[i].canShoot = 1;
              dam[i].shot = 0;
            }

            if (dam[i].row >= player.row && dam[i].row <= player.row + player.height) {
              if (dam[i].col >= player.col && dam[i].col <= player.col + player.width) {
                player.health += player.healthdel;
                dam[i].shot = 0;
                enemies[i].canShoot = 1;
              }
            }
          }

          if (enemies[i].alive == 1 && enemies[i].canShoot == 1 && enemies[i].row >= 0 && dam[i].delay == 0 && killed >= 25)  {
            dam[i].shot = 1;
            dam[i].row = enemies[i].row + enemies[i].height;
            dam[i].col = enemies[i].col + ((enemies[i].width) / 2);
            enemies[i].canShoot = 0;
            dam[i].delay = shDelay;
          }

          if (dam[i].delay > 0) {
            dam[i].delay--;
          }
        }

        if (incrementDelay == 0) {
          incrementDelay = inDelay;
        }

        incrementDelay--;
      }

      if (enemySpawnDelay == 0 && numEnem < (sizeof(enemies) / sizeof(enemies[0]))) {
        int i = 0;
        while(enemies[i].alive == 1) {
          i++;
        }
        enemyPointer = &enemies[i];
        enemyPointer -> rdel = 1;
        enemyPointer -> cdel = 0;
        enemyPointer -> width = TIEFIGHTER_WIDTH;
        enemyPointer -> height = TIEFIGHTER_HEIGHT;
        enemyPointer -> health = 100;
        enemyPointer -> healthdel = -50;
        enemyPointer -> image = TIEFIGHTER;
        enemyPointer -> alive = 1;
        enemyPointer -> canShoot = 1;
        enemyPointer -> row = 0 - (enemyPointer -> height);
        enemyPointer -> col = rand()%(SCREENWIDTH - enemies[0].width);
        enemySpawnDelay = spDelay;
        numEnem++;
        spawned++;
      } else if (numEnem < (sizeof(enemies) / sizeof(enemies[0]))) {
        enemySpawnDelay--;
      } else {
        enemySpawnDelay = 0;
      }


      //Collisions for Shots for Player
      if (leftShark.shot == 1) {
        for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
          if (leftShark.row >= enemies[i].row && leftShark.row <= enemies[i].row + enemies[i].height) {
            if (leftShark.col >= enemies[i].col && leftShark.col <= enemies[i].col + enemies[i].width) {
              enemies[i].health += enemies[i].healthdel;
              leftShark.shot = 0;
              player.canShoot = 1;
            }
          }
        }
        if (leftShark.row < 0) {
          leftShark.shot = 0;
          player.canShoot = 1;
        }
      }

      if (rightShark.shot == 1) {
        for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
          if (rightShark.row >= enemies[i].row && rightShark.row <= enemies[i].row + enemies[i].height) {
            if (rightShark.col >= enemies[i].col && rightShark.col <= enemies[i].col + enemies[i].width) {
              enemies[i].health += enemies[i].healthdel;
              rightShark.shot = 0;
              player.canShootTwo = 1;
            }
          }
        }
        if (rightShark.row < 0) {
          rightShark.shot = 0;
          player.canShootTwo = 1;
        }
      }

      if (killed >= 25 && killed % 25 == 0 && (spDelay * shDelay * inDelay) > 0 && killedAgain == 1) {
        spDelay -= 25;
        shDelay -= 10;
        inDelay--;
        if (killed >= 25) {
          bullspeed++;
          for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
            dam[i].rdel = bullspeed;
          }
        }
        killedAgain = 0;
      }


      waitForVblank();
      //clear
      for (unsigned int i = 0; i < numObj; i++) {
        drawRect(oldObj[i].row,oldObj[i].col, oldObj[i].height, oldObj[i].width, BLACK);
      }
      for (unsigned int i = 0; i < numBlasts; i++) {
        drawRect(oldBlast[i].row,oldBlast[i].col, oldBlast[i].height, oldBlast[i].width, BLACK);
      }

      unsigned int objs = 0;
      unsigned int blasts = 0;

      drawImage3(20, 20, DEATHSTAR_WIDTH, DEATHSTAR_HEIGHT, DEATHSTAR);
      for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
        if (enemies[i].alive == 1) {
          drawImage3(enemies[i].col, enemies[i].row, enemies[i].width, enemies[i].height, enemies[i].image);
          if (dam[i].shot == 1) {
            drawRect(dam[i].row, dam[i].col, dam[i].height, dam[i].width, dam[i].color);
            oldBlast[blasts] = dam[i];
            dam[i].row += dam[i].rdel;
            blasts++;
          }
          oldObj[objs] = enemies[i];
          objs++;
        }
      }
      drawImage3(player.col, player.row, player.width, player.height, player.image);
      oldObj[objs] = player;
      objs++;
      char str[80];
      sprintf(str, "Ship:%d%%    Base:%d%%    Destroyed:%d", player.health, baseHealth, killed);
      if (leftShark.shot == 1) {
        drawRect(leftShark.row, leftShark.col, leftShark.height, leftShark.width, leftShark.color);
        oldBlast[blasts] = leftShark;
        leftShark.row += leftShark.rdel;
        blasts++;
      }
      if (rightShark.shot == 1) {
        drawRect(rightShark.row, rightShark.col, rightShark.height, rightShark.width, rightShark.color);
        oldBlast[blasts] = rightShark;
        rightShark.row += rightShark.rdel;
        blasts++;
      }
      drawRect(148, 0, 12, SCREENWIDTH, GRAY);
      drawString(150, 0, str, YELLOW);
      numObj = objs;
      numBlasts = blasts;

      if (killed == 100) {
        over = 1;
        won = 1;
      } else if (player.health * baseHealth <= 0) {
        over = 1;
        won = 0;
      }
    }

    for (unsigned int i = 0; i < (sizeof(enemies) / sizeof(enemies[0])); i++) {
      enemies[i].alive = 0;
    }

    fillScreen(BLACK);

    if (won == 0 && over == 1) {
      while (KEY_DOWN_NOW(BUTTON_SELECT) == 0) {
        drawImage3(0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, GAMEOVER);
        drawRect(148, 0, 12, SCREENWIDTH, GRAY);
      }
    } else if (won == 1 && over == 1){
      while (KEY_DOWN_NOW(BUTTON_SELECT) == 0) {
        drawImage3(0, 0, WIN_WIDTH, WIN_HEIGHT, WIN);
        drawRect(148, 0, 12, SCREENWIDTH, GRAY);
      }
    }

  }
  return 0;
}
