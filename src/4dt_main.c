/**
   \mainpage 4DTRIS - Tetris like game in four dimension
   \image html 4DTRIS_icon_128.png

   Copyright (C) 2008 Simon, Laszlo

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   \warning You should have received a copy of the GNU General Public License
            along with this program.  If not, see <http://www.gnu.org/licenses/>.

   \author Simon, Laszlo <laszlo.simon@gmail.com>

   \include README
 */
/**
 * \file 4dt_main.c
 * \brief This file is the main unit of the application.
 */

/*
--------------------------------------------------------------------------------
   INCLUDES
--------------------------------------------------------------------------------
*/

#include <GL/glut.h>


#include "4dt_ai.h"
#include "4dt_eng.h"
#include "4dt_g3d.h"
#include "4dt_g4d.h"
#include "4dt_scn.h"
#include "4dt_ui.h"
#include "4dt_menu.h"
#include "4dt_hst.h"

/*
--------------------------------------------------------------------------------
   GLOBAL VARIABLES
--------------------------------------------------------------------------------
*/

/*
--------------------------------------------------------------------------------
   PROTOTYPES
--------------------------------------------------------------------------------
*/

static void specialKeyPress(int key, int x, int y);
static void keyPress(unsigned char key, int x, int y);
static void engineTimerCallback(int value);
static void autoplayTimerCallback(int value);

/*
--------------------------------------------------------------------------------
   FUNCTIONS
--------------------------------------------------------------------------------
*/

/*------------------------------------------------------------------------------
    Event handlers
*/

/** Timer function for Game engine. */
static void engineTimerCallback(int value)
{
  if (engGE.gameOver == 0)
  {
    if (!menuActived() || aiAutoGamerON)
    {
      engPrintSpace();

      // lower the solid and
      engLowerSolid();
    }
  }
  else
  {
    aiAutoGamerON = 1;
    menuGotoItem(eMenuHighScores);
    engResetGame();
  }

  // Force drawing
  glutPostRedisplay();

  // set call back again
  glutTimerFunc(engGetTimestep(), engineTimerCallback, value);
}

/** Timer function for Autoplayer. */
static void autoplayTimerCallback(int value)
{
  if ((engGE.gameOver == 0) && (aiAutoGamerON))
  {
    engPrintSpace();
    aiDoStep();
  }

  // Force drawing
  glutPostRedisplay();

  // set call back again
  glutTimerFunc(150, autoplayTimerCallback, value);
}

/** Eventhandler of special key pressing. */
static void specialKeyPress(int key, int x, int y)
{
  int uiKey;

  switch (key)
  {
  case GLUT_KEY_UP:    uiKey = UI_KEY_UP;    break;
  case GLUT_KEY_LEFT:  uiKey = UI_KEY_LEFT;  break;
  case GLUT_KEY_DOWN:  uiKey = UI_KEY_DOWN;  break;
  case GLUT_KEY_RIGHT: uiKey = UI_KEY_RIGHT; break;
  }

  uiKeyPress(uiKey);

  // Refresh the display.
  glutPostRedisplay();
}

/** Eventhandler of key pressing. */
static void keyPress(unsigned char key, int x, int y)
{
  uiKeyPress(key);

  // Refresh the display.
  glutPostRedisplay();
}


/*------------------------------------------------------------------------------
    M A I N
*/

/** Main function of the software */
int main(int argc, char *argv[])
{
  // Initialize/load High Score table
  hstInit();

  // Set random colors for game levels
  scnInit();

  // Initialize the game engine.
  engInitGame();

  // Set the size of the window.
  glutInitWindowSize(640,480);

  // Set the position of the window's top left corner.
  glutInitWindowPosition(50,50);

  // Initialise Glut.
  glutInit(&argc, argv);

  // Set Glut display mode.
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

  // Create the window with the specified title.
  glutCreateWindow("4DTris");

  // Set the reshape function.
  glutReshapeFunc(g3dResize);

  // initialise 3D drawing modul
  g3dInit();

  // initialise 4D drawing modul
  g4dCalibrate(SPACELENGTH);

  // Set the display function.
  glutDisplayFunc(scnDisplay);

  // Set the keypress event handler function.
  glutKeyboardFunc(keyPress);

  // Set the special keypress event handler function.
  glutSpecialFunc(specialKeyPress);

  // set timer callbacks
  glutTimerFunc(engGetTimestep(), engineTimerCallback, 0);
  glutTimerFunc(150, autoplayTimerCallback, 0);

  // Start glut's main loop.
  glutMainLoop();

  // Return with successed exit constant.
  return EXIT_SUCCESS;
}
