# progress2.inc

Progress bar library v2.0.1

A SA:MP UI library for rendering progress bars used to visualise all manner of data from health to a countdown timer.

Library originally written by Flávio Toribio (flavio_toibio@hotmail.com)
Now maintained by Southclaw in version 2+ with new features.


# Preview

![http://puu.sh/byvSQ/c512d56383.gif](http://puu.sh/byvSQ/c512d56383.gif)

![http://puu.sh/byxVW/f3d764d030.gif](http://puu.sh/byxVW/f3d764d030.gif)

![http://puu.sh/byBqe/6bf0e4e57c.gif](http://puu.sh/byBqe/6bf0e4e57c.gif)


# Resources


## Constants

- ```MAX_PLAYER_BARS```: Defaults to the textdraw limit divided by 3.
- ```INVALID_PLAYER_BAR_VALUE```: Invalid return value for interface functions.
- ```INVALID_PLAYER_BAR_ID```: Invalid bar ID value.
- ```BAR_DIRECTION_RIGHT```: Bar direction left-to-right: ```[> ]```
- ```BAR_DIRECTION_LEFT```: Bar direction right-to-left: ```[ <]```
- ```BAR_DIRECTION_UP```: Bar direction bottom to top: ```[/\]```
- ```BAR_DIRECTION_DOWN```: Bar direction top to bottom: ```[\/]```


## Functions

- ```PlayerBar:CreatePlayerProgressBar(playerid, Float:x, Float:y, Float:width = 55.5, Float:height = 3.2, colour, Float:max = 100.0, direction = BAR_DIRECTION_RIGHT)```:
  Creates a progress bar for a player.

- ```DestroyPlayerProgressBar(playerid, PlayerBar:barid)```:
  Destroys a player's progress bar.

- ```ShowPlayerProgressBar(playerid, PlayerBar:barid)```:
  Shows a player's progress bar to them.

- ```HidePlayerProgressBar(playerid, PlayerBar:barid)```:
  Hides a player's progress bar from them.

- ```IsValidPlayerProgressBar(playerid, PlayerBar:barid)```:
  Returns true if the input bar ID is valid and exists.

- ```GetPlayerProgressBarPos(playerid, PlayerBar:barid, &Float:x, &Float:y)```:
  Returns the on-screen position of the specified progress bar.

- ```SetPlayerProgressBarPos(playerid, PlayerBar:barid, Float:x, Float:y)```:
  *(NOTE: Function not written yet)*

- ```Float:GetPlayerProgressBarWidth(playerid, PlayerBar:barid)```:
  Returns the width of a progress bar.

- ```SetPlayerProgressBarWidth(playerid, PlayerBar:barid, Float:width)```:
  *(NOTE: Function not written yet)*

- ```Float:GetPlayerProgressBarHeight(playerid, PlayerBar:barid)```:
  Returns the height of a progress bar.

- ```SetPlayerProgressBarHeight(playerid, PlayerBar:barid, Float:height)```:
  *(NOTE: Function not written yet)*

- ```GetPlayerProgressBarColour(playerid, PlayerBar:barid)```:
  Returns the colour of a progress bar.

- ```SetPlayerProgressBarColour(playerid, PlayerBar:barid, colour)```:
  Sets the colour of a progress bar.

- ```Float:GetPlayerProgressBarMaxValue(playerid, PlayerBar:barid)```:
  Returns the maximum value of a progress bar.

- ```SetPlayerProgressBarMaxValue(playerid, PlayerBar:barid, Float:max)```:
  Sets the maximum value that a progress bar represents.

- ```Float:GetPlayerProgressBarValue(playerid, PlayerBar:barid)```:
  Returns the value a progress bar represents.

- ```SetPlayerProgressBarValue(playerid, PlayerBar:barid, Float:value)```:
  Sets the value a progress bar represents.

- ```GetPlayerProgressBarDirection(playerid, PlayerBar:barid)```:
  Returns the direction of a progress bar.

- ```SetPlayerProgressBarDirection(playerid, PlayerBar:barid, direction)```:
  *(NOTE: Function not written yet)*


## Hooked

- ```OnGameModeInit / OnFilterScriptInit```: When y_iterate is used, initialises iterators.
- ```OnPlayerDisconnect```: To automatically destroy bars when a player disconnects.
