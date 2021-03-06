/**
 * \file  4dt_menu.c
 * \brief Game menu handling and drawing module.
 */
 
/*------------------------------------------------------------------------------
   INCLUDE FILES
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include "4dt_m3d.h"
#include "4dt_m4d.h"
#include "4dt_eng.h"
#include "4dt_scn.h"
#include "4dt_g3d.h"
#include "4dt_gtxt.h"
#include "4dt_g4d.h"
#include "4dt_ai.h"
#include "4dt_hst.h"

#include "4dt_menu.h"

/*------------------------------------------------------------------------------
   MACROS
------------------------------------------------------------------------------*/

/** Maximal number of submenu items per menu item. */
#define MENUMAXITEM 10
/** Number of lines in some menu's multiline text */
#define TEXTLINENUM 8

/*------------------------------------------------------------------------------
   TYPES
------------------------------------------------------------------------------*/

/** String type for menu caption. */
typedef char* tMenuString;

/** Menu item descriptor struct */
typedef struct
{
  char        enabled;               /**< Flag for enable menu item */
  tMenuString caption;               /**< Caption of the menu item */
  void        (*activate)(void);     /**< Callback called when menu item
                                          activates */
  void        (*deactivate)(void);   /**< Callback called when menu item
                                          activates */
  eMenuItem   parent;                /**< Parent menu item */
  eMenuItem   submenus[MENUMAXITEM]; /**< (last item must be 'eNull') */
} tMenuItem;

/*------------------------------------------------------------------------------
   PROTOTYPES
------------------------------------------------------------------------------*/

static void menuNew(void);
static void menuSound(void);
static void menuMusic(void);
static void menuAuto(void);
static void menuLevel(void);
static void menuControls(void);
static void menuHelp(void);
static void menuScores(void);
static void menuAbout(void);
static void menuBack(void);
static void menuQuit(void);
static void menuAnimation(void);
static void menuGameOver(void);
static void menuGameOverOff(void);
static void menuDrawmode(void);
static void menuGrid(void);
static void menuStereo(void);

/*------------------------------------------------------------------------------
   CONSTANTS
------------------------------------------------------------------------------*/

/** Menu item descriptor array. Must be same order then Menu item enums! */
static tMenuItem menuItems[eMenuItemNum] =
{
/* en, caption,         activate,      deact., parent,            submenus */
  {0, "",               NULL,          NULL,   eMenuNull,         {eMenuNull}},
  {0, "",               NULL,          NULL,   eMenuOFF,          {eMenuRoot, eMenuNull}},
  {1, "Main menu",      NULL,          NULL,   eMenuOFF,          {eMenuNewGame, eMenuOptions, eMenuHelp, eMenuBackToGame, eMenuQuit, eMenuNull}},
  {1, "New Game",       menuNew,       NULL,   eMenuRoot,         {eMenuNull} },
  {1, "Options",        NULL,          NULL,   eMenuRoot,         {eMenuVideoOptions, eMenuAudioOptions, eMenuGameOptions, eMenuNull} },
  {1, "Video Options",  NULL,          NULL,   eMenuOptions,      {eMenuAnimation, eMenuDrawmode, eMenuGrid, eMenuStereo, eMenuNull} },
  {1, "Animation - ON", menuAnimation, NULL,   eMenuVideoOptions, {eMenuNull} },
  {1, "Draw connected hypercubes",menuDrawmode,NULL,eMenuVideoOptions,{eMenuNull} },
  {1, "Grid - OFF",     menuGrid,      NULL,   eMenuVideoOptions, {eMenuNull} },
  {1, "View mode - Mono", menuStereo, NULL,   eMenuVideoOptions, {eMenuNull} },
  {0, "Audio Options",  NULL,          NULL,   eMenuOptions,      {eMenuSound, eMenuMusic, eMenuNull} },
  {0, "Sound",          menuSound,     NULL,   eMenuAudioOptions, {eMenuNull}  },
  {0, "Music",          menuMusic,     NULL,   eMenuAudioOptions, {eMenuNull}  },
  {1, "Game Options",   NULL,          NULL,   eMenuOptions,      {eMenuAutoPlayer, eMenuDiffLevel, eMenuControls, eMenuNull} },
  {1, "Auto Player - ON", menuAuto,    NULL,   eMenuGameOptions,  {eMenuNull}  },
  {1, "Diff. Level - Hard", menuLevel, NULL,   eMenuGameOptions,  {eMenuNull}  },
  {0, "Controls",       menuControls,  NULL,   eMenuGameOptions,  {eMenuNull}  },
  {1, "Help",           NULL,          NULL,   eMenuRoot,         {eMenuHelppage, eMenuHighScores, eMenuAbout, eMenuNull} },
  {1, "Help Page",      menuHelp,      NULL,   eMenuHelp,         {eMenuNull}  },
  {1, "High Scores",    menuScores,    NULL,   eMenuHelp,         {eMenuNull}  },
  {1, "About",          menuAbout,     NULL,   eMenuHelp,         {eMenuNull}  },
  {0, "Back To Game",   menuBack,      NULL,   eMenuRoot,         {eMenuNull}  },
  {1, "Quit",           menuQuit,      NULL,   eMenuRoot,         {eMenuNull}  },
  {1, "",               menuGameOver, menuGameOverOff, eMenuRoot, {eMenuNewGame, eMenuHighScores, eMenuRoot, eMenuNull}  },
};

/** Text of help page */
static tMenuString menuHelpText[TEXTLINENUM] =
{
  "Game Controls:",
  " - arrow keys, backspace - rotate/turn the view port",
  " - z, x, c, v - turn the blocks around the 4 axis",
  " - 1, 2, 3 - select rotation axles",
  " - +, -, pgdn, pgup - rotate around selected axle",
  " - home, end - rot. around selected and 4th axle",
  " - space, enter - step down/drop the object",
  " - F1, F2 - switch view mode"
};

/** Text of help page */
static tMenuString menuAboutText[TEXTLINENUM] =
{
  "4DTRIS",
  "Tetris like game in four dimension",
  "Copyright (C) 2008-2010 Simon, Laszlo",
  "<laszlo.simon@gmail.com>",
  "http://dtris.sourceforge.net/",
  "https://launchpad.net/4dtris",
  "",
  ""
};

/** Text of help page */
static tMenuString menuGameOverText[TEXTLINENUM] =
{
  "", "", "","", "", "",
  "Game over.",
  ""
};


/*------------------------------------------------------------------------------
   GLOBAL VARIABLES
------------------------------------------------------------------------------*/

static eMenuItem menuActItem = eMenuRoot; /**< Actual active menu item.           */
static int menuSelItems[eMenuItemNum];    /**< Last selected submenu in each menuitem */
static char **menuText = NULL;        /**< Actual menu text for display       */

static tEngGame *pMenuEngGame;
static tScnSet *pMenuScnSet;

/*------------------------------------------------------------------------------
   FUNCTIONS
------------------------------------------------------------------------------*/

void menuInit(tEngGame *pEngGame, tScnSet *pScnSet)
{
  pMenuEngGame = pEngGame;
  pMenuScnSet  = pScnSet;
}

/** Get function for query menu active state. */
int menuIsActived(void)
{
  return(menuActItem != eMenuOFF);
}

/** Counts sub menu items of a menu item. */
static int menuSubNum(eMenuItem item)
{
  int i = 0;

  while( (menuItems[item].submenus[i] != eMenuNull) && (i < MENUMAXITEM) )
  {
    i++;
  }

  return(i);
}

/** Set menu item active */
void menuGotoItem(eMenuItem menuItem)
{
  if (   (menuItem != menuActItem)
      && (menuItem != eMenuNull) )
  {
    if (NULL != menuItems[menuActItem].deactivate)
    {
      (*menuItems[menuActItem].deactivate)();
    }

    menuActItem = menuItem;

    if (NULL != menuItems[menuActItem].activate)
    {
      (*menuItems[menuActItem].activate)();
    }
  }
}

/** Menu state machine. */
void menuNavigate(eMenuEvent event)
{
  int subMenuNum;    /*  number of submenus in actual menu item */

  subMenuNum = menuSubNum(menuActItem);

  switch(event)
  {
    case eMenuUp:
      /*  Go up in the submenu list */
      if (subMenuNum != 0)
      {
        menuSelItems[menuActItem] = (menuSelItems[menuActItem] + subMenuNum - 1)
                                    % subMenuNum;
      }
      break;
    case eMenuDown:
      /*  Go down in the submenu list */
      if (subMenuNum != 0)
      {
        menuSelItems[menuActItem] = (menuSelItems[menuActItem] + 1)
                                    % subMenuNum;
      }
      break;
    case eMenuForward:
    {
      /*  Step in to a menu item */
      eMenuItem subItem = menuItems[menuActItem].submenus[menuSelItems[menuActItem]];

      if (   (subItem != eMenuNull)
          && (1 == menuItems[subItem].enabled))
      {
        menuGotoItem(subItem);
      }
      break;
    }
    case eMenuBack:
      /*  Step out from a menu item */
      if (1 == menuItems[menuItems[menuActItem].parent].enabled)
      {
        menuGotoItem(menuItems[menuActItem].parent);
        menuText = NULL;
      }
      break;
    default:
      exit(1); /*  Control should not reach this point */
  }
}

static void menuColor(int selected,
                      int disabled,
                      float color[4])
{
  static eMenuItem lastActItem = eMenuNull;
  int i;
  float colors[2][4] = { {0.8, 0.8, 0.9, 1.0} /* normal */,
                         {0.3, 0.3, 1.0, 1.0} /* selected */};

  static float alpha = 0.0f;

  alpha = (lastActItem != menuActItem)
          ? 0.0f
          : (alpha < 1.0f) ? alpha + 0.20f : 1.0f;

  lastActItem = menuActItem;

  for(i = 0; i < 4; i++) { color[i] = (colors[selected][i]); }

  if (disabled) { color[3] = 0.5; }

  color[3] *= alpha;
}

/** Menu drawing procedure */
void menuDraw(void)
{
  int i = 0;
  eMenuItem subItem;

  float linespace = 0.1; /*  portion of vert. display size between lines */

  /*  colors for different menu item states */
  float color[4];

  /*  for each submenu in active menu item */
  for (i = 0; i < menuSubNum(menuActItem); i++)
  {
    subItem = menuItems[menuActItem].submenus[i];

    menuColor((i == menuSelItems[menuActItem]),
              (0 == menuItems[subItem].enabled),
              color);

    /*  Render the menu caption. */
    g3dRenderString(0.1, 1 - (i+1.5) * linespace,
                    color,
                    menuItems[subItem].caption);
  }

  /*  If multiline text selected to display */
  if (menuText != NULL)
  {

    menuColor(0, 0, color);

    /*  display the text */
    g3dRenderText(0.1, 1 - 1.5 * linespace, color,
                  menuText, TEXTLINENUM, linespace);
  }

  return;
}

/*------------------------------------------------------------------------------
   MENUITEM CALLBACK FUNCTIONS
------------------------------------------------------------------------------*/

static void menuGameOver(void)
{
  g4dSwitchAutoRotation(1);
  menuText = menuGameOverText;
  menuItems[eMenuBackToGame].enabled = 0;
  menuItems[eMenuOFF].enabled = 0;
}

static void menuGameOverOff(void)
{
  menuText = NULL;
  engResetGame(pMenuEngGame);
  aiSetActive(1, pMenuEngGame);
}

static void menuNew(void)
{
  /*  reset game engine */
  aiSetActive(0, pMenuEngGame);
  engResetGame(pMenuEngGame);
  pMenuEngGame->activeUser = 1;

  menuItems[eMenuAutoPlayer].caption = "Auto Player - OFF";

  menuItems[eMenuBackToGame].enabled = 1;
  menuItems[eMenuOFF].enabled = 1;

  g4dReset();
  g4dSwitchAutoRotation(0);

  menuNavigate(eMenuBack);
  menuNavigate(eMenuBack);
}

static void menuSound(void)
{
  /*  Not implemented yet */
}

static void menuMusic(void)
{
  /*  Not implemented yet */
}

static void menuAuto(void)
{
  aiSetActive(aiIsActive() ? 0 : 1, pMenuEngGame);

  menuItems[eMenuAutoPlayer].caption = (aiIsActive())
                                   ? "Auto Player - ON"
                                   : "Auto Player - OFF";
  menuNavigate(eMenuBack);
}

static void menuLevel(void)
{
  const tMenuString captions[DIFFLEVELS] =
  {
    "Diff. Level - Easy",
    "Diff. Level - Medium",
    "Diff. Level - Hard"
  };

  pMenuEngGame->game_opts.diff = (pMenuEngGame->game_opts.diff+1) % DIFFLEVELS;

  menuItems[eMenuDiffLevel].caption = captions[pMenuEngGame->game_opts.diff];

  menuNavigate(eMenuBack);
}

static void menuControls(void)
{
  /*  Not implemented yet */
}

static void menuHelp(void)
{
  menuText = menuHelpText;
}

static void menuScores(void)
{
  hstGetScoreTab(&menuText, TEXTLINENUM);
}

static void menuAbout(void)
{
  menuText = menuAboutText;
}

static void menuBack(void)
{
  menuNavigate(eMenuBack);
  menuNavigate(eMenuBack);
}

static void menuQuit(void)
{
  exit(EXIT_SUCCESS);
}

static void menuAnimation(void)
{
  pMenuEngGame->animation.enable = !pMenuEngGame->animation.enable;

  menuItems[eMenuAnimation].caption = (pMenuEngGame->animation.enable)
                                   ? "Animation - ON"
                                   : "Animation - OFF";
  menuNavigate(eMenuBack);
}

static void menuDrawmode(void)
{
  const struct
  {
    tMenuString caption;
    int hypercubedraw;
    int separateBlockDraw;
  }
  states[3] =
  {
    { "Draw connected hypercubes", 1, 0},
    { "Draw separate hypercubes",  1, 1},
    { "Draw separate cubes",       0, 1}
  };

  static int state = 0;

  state = (state + 1) % 3;

  scnSetEnableHypercubeDraw(states[state].hypercubedraw, pMenuScnSet);
  scnSetEnableSeparateBlockDraw(states[state].separateBlockDraw, pMenuScnSet);

  menuItems[eMenuDrawmode].caption = states[state].caption;
  menuNavigate(eMenuBack);
}

static void menuGrid(void)
{
  scnSetEnableGridDraw(!scnGetEnableGridDraw(pMenuScnSet), pMenuScnSet);

  menuItems[eMenuGrid].caption = scnGetEnableGridDraw(pMenuScnSet)
                                 ? "Grid - ON"
                                 : "Grid - OFF";
  menuNavigate(eMenuBack);
}

static void menuStereo(void)
{
  char* captions[eScnViewModeNum] =
  {
    "View mode - Mono",
    "View mode - Stereogramm",
    "View mode - Anaglyph"
  };

  scnSetViewMode( (scnGetViewMode(pMenuScnSet)+1) % eScnViewModeNum,
                  pMenuScnSet);

  menuItems[eMenuStereo].caption = captions[scnGetViewMode(pMenuScnSet)];

  menuNavigate(eMenuBack);
}
