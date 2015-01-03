//------------------------------------------------------------------------------

/*
	SA-MP progress bar 2 editor
*/

//------------------------------------------------------------------------------

#define FILTERSCRIPT

#include <a_samp>
#include "../include/progress2.inc"

//------------------------------------------------------------------------------

#define DIALOG_EDITOR		5356
#define DIALOG_BAREDIT		5457
#define DIALOG_CAPTION		"Progress Editor 0.1"
#define DIALOG_INFO			"1.\tCreate a Bar\n2.\tEdit a bar\n3.\tDelete all bars\n4.\tExport all bars"
#define DIALOG_BAR			"1.\tChange position\n2.\tChange size\n3.\tChange direction\n4.\tChange max value\n5.\tChange color\n6.\tDelete this bar\n7.\tExport this bar"

#define COLOR_WHITE			0xffffffff
#define COLOR_INFO			0x00b9e8ff
#define COLOR_ERROR			0xb6b4b4ff

#define PlaySelectSound(%0)	PlayerPlaySound(%0,1083,0.0,0.0,0.0)
#define PlayCancelSound(%0)	PlayerPlaySound(%0,1084,0.0,0.0,0.0)
#define PlayErrorSound(%0)	PlayerPlaySound(%0,1085,0.0,0.0,0.0)

//------------------------------------------------------------------------------

enum E_PE_PLAYER
{
	bool:E_PE_PLAYER_IS_EDITING,
	PlayerBar:E_PE_PLAYER_BAR_EDITING_ID,
	E_PE_PLAYER_BAR_EDITING_MODE
}
new gPlayerData[MAX_PLAYERS][E_PE_PLAYER];

//------------------------------------------------------------------------------

enum
{
	EDITING_NONE,
	EDITING_POSITION,
	EDITING_SIZE,
	EDITING_DIRECTION,
	EDITING_MAX_VALUE,
	EDITING_COLOR
}

//------------------------------------------------------------------------------

new PlayerText:g_p_txd_barsID[MAX_PLAYERS][MAX_PLAYER_BARS];

//------------------------------------------------------------------------------

public OnFilterScriptInit()
{
	printf("- Progress Bar Editor loaded.");
	SendClientMessageToAll(COLOR_WHITE, "* {00b9e8}/progress{ffffff} to open the editor.");
	for(new i; i < MAX_PLAYERS; i++)
	{
		if(!IsPlayerConnected(i))
			continue;

		ResetPlayerVars(i);
	}
	return 1;
}

//------------------------------------------------------------------------------

public OnFilterScriptExit()
{
	for(new i; i < MAX_PLAYERS; i++)
	{
		if(gPlayerData[i][E_PE_PLAYER_IS_EDITING])
			TogglePlayerControllable(i, true);

		for(new PlayerBar:j; _:j < MAX_PLAYER_BARS; _:j++)
		{
			DestroyPlayerProgressBar(i, j);
		}
	}
	return 1;
}

//------------------------------------------------------------------------------

public OnPlayerSpawn(playerid)
{
	SendClientMessage(playerid, COLOR_INFO, "* {00b9e8}/progress{ffffff} to open the editor.");
	return 1;
}

//------------------------------------------------------------------------------

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp(cmdtext, "/progress", true))
	{
		if(gPlayerData[playerid][E_PE_PLAYER_IS_EDITING])
			return SendClientMessage(playerid, COLOR_ERROR, "* You are editing a bar already!");

		TogglePlayerControllable(playerid, false);
		ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
		PlaySelectSound(playerid);
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch(dialogid)
	{
		case DIALOG_EDITOR:
		{
			if(!response)
			{
				PlayCancelSound(playerid);
				TogglePlayerControllable(playerid, true);
				return 1;
			}

			switch(listitem)
			{
				case 0: // Create a bar
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, 290.0, 103.0, 50.0, 10.0, 0x00b9e8ff, 100.0, BAR_DIRECTION_RIGHT);
					SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
					ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
					ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
					PlaySelectSound(playerid);
				}
				case 1: // Edit a bar
				{
					new dialogList[2048];

					for(new PlayerBar:i; _:i < MAX_PLAYER_BARS; _:i++)
					{
						if(!IsValidPlayerProgressBar(playerid, i))
							continue;

						new barData[40];
						format(barData, 40, "barID: %d\n", _:i);
						strins(dialogList, barData, strlen(dialogList));

						new Float:x, Float:y;
						format(barData, 40, "%d", _:i);
						GetPlayerProgressBarPos(playerid, i, x, y);

						if(GetPlayerProgressBarDirection(playerid, i) == BAR_DIRECTION_LEFT)
							g_p_txd_barsID[playerid][_:i] = CreatePlayerTextDraw(playerid, x-12, y+2, barData);
						else if(GetPlayerProgressBarDirection(playerid, i) == BAR_DIRECTION_UP)
							g_p_txd_barsID[playerid][_:i] = CreatePlayerTextDraw(playerid, x-10, y-10, barData);
						else if(GetPlayerProgressBarDirection(playerid, i) == BAR_DIRECTION_DOWN)
							g_p_txd_barsID[playerid][_:i] = CreatePlayerTextDraw(playerid, x-10, y+4, barData);
						else
							g_p_txd_barsID[playerid][_:i] = CreatePlayerTextDraw(playerid, x+2, y+2, barData);

						PlayerTextDrawColor(playerid, g_p_txd_barsID[playerid][_:i], 0xffffffff);
						PlayerTextDrawBackgroundColor(playerid, g_p_txd_barsID[playerid][_:i], 0x000000FF);
						PlayerTextDrawFont(playerid, g_p_txd_barsID[playerid][_:i], 2);
						PlayerTextDrawLetterSize(playerid, g_p_txd_barsID[playerid][_:i], 0.25, 0.5);
						PlayerTextDrawTextSize(playerid, g_p_txd_barsID[playerid][_:i], 0.5, 0.5);
						PlayerTextDrawSetShadow(playerid, g_p_txd_barsID[playerid][_:i], 0);
						PlayerTextDrawSetOutline(playerid, g_p_txd_barsID[playerid][_:i], 1);
						PlayerTextDrawShow(playerid, g_p_txd_barsID[playerid][_:i]);
					}

					if(strlen(dialogList) < 1)
					{
						SendClientMessage(playerid, COLOR_ERROR, "* No bars created!");
						ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
						PlayErrorSound(playerid);
						return 1;
					}

					ShowPlayerDialog(playerid, DIALOG_EDITOR+1, DIALOG_STYLE_LIST, DIALOG_CAPTION, dialogList, "Edit", "Back");
					PlaySelectSound(playerid);
					return 1;
				}
				case 2: // Delete all bars
				{
					for(new PlayerBar:i; _:i < MAX_PLAYER_BARS; _:i++)
					{
						if(!IsValidPlayerProgressBar(playerid, i))
							continue;

						DestroyPlayerProgressBar(playerid, i);
					}
					SendClientMessage(playerid, COLOR_WHITE, "* All {00b9e8}bars deleted{ffffff}.");
					ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
					PlaySelectSound(playerid);
				}
				case 3: // Export all bars
				{
					new count;
					for(new PlayerBar:i; _:i < MAX_PLAYER_BARS; _:i++)
					{
						if(!IsValidPlayerProgressBar(playerid, i))
							continue;

						count++;

						new Float:X, Float:Y;
						GetPlayerProgressBarPos(playerid, i, X, Y);

						new Float:width;
						width = GetPlayerProgressBarWidth(playerid, i);

						new Float:height;
						height = GetPlayerProgressBarHeight(playerid, i);

						new color;
						color = GetPlayerProgressBarColour(playerid, i);

						new direction;
						direction = GetPlayerProgressBarDirection(playerid, i);

						new directionName[32];
						switch(direction)
						{
							case BAR_DIRECTION_RIGHT:	directionName = "BAR_DIRECTION_RIGHT";
							case BAR_DIRECTION_LEFT:	directionName = "BAR_DIRECTION_LEFT";
							case BAR_DIRECTION_UP:		directionName = "BAR_DIRECTION_UP";
							case BAR_DIRECTION_DOWN:	directionName = "BAR_DIRECTION_DOWN";
						}

						new textToSave[128];
						new File:barFile = fopen("bars.txt", io_append);
						format(textToSave, 256, "CreatePlayerProgressBar(playerid, %f, %f, %f, %f, %d, %s);\n", X, Y, width, height, color, directionName);
						fwrite(barFile, textToSave);
						fclose(barFile);
					}

					if(count != 0)
					{
						PlaySelectSound(playerid);
						SendClientMessage(playerid, COLOR_WHITE, "* {00b9e8}All bars exported{ffffff} to scriptfiles/bars.txt");
					}
					else
					{
						PlayErrorSound(playerid);
						SendClientMessage(playerid, COLOR_ERROR, "* No bars created!");	
						ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
					}
					return 1;
				}
			}
		}
		case DIALOG_EDITOR+1: // Edit a bar 2
		{
			for(new PlayerBar:i; _:i < MAX_PLAYER_BARS; _:i++)
			{
				if(!IsValidPlayerProgressBar(playerid, i))
					continue;

				PlayerTextDrawDestroy(playerid, g_p_txd_barsID[playerid][_:i]);
			}

			if(!response)
			{
				ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
				PlayCancelSound(playerid);
				return 1;
			}

			new PlayerBar:barList[MAX_PLAYER_BARS], count;
			for(new PlayerBar:i; _:i < MAX_PLAYER_BARS; _:i++)
			{
				if(!IsValidPlayerProgressBar(playerid, i))
					continue;

				barList[count] = i;
				count++;
			}

			gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = barList[listitem];
			gPlayerData[playerid][E_PE_PLAYER_IS_EDITING] = true;

			ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
			PlaySelectSound(playerid);
			return 1;
		}
		case DIALOG_BAREDIT: // Bar editing
		{
			if(!response)
			{
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = INVALID_PLAYER_BAR_ID;
				gPlayerData[playerid][E_PE_PLAYER_IS_EDITING] = false;

				ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
				PlayCancelSound(playerid);
				return 1;
			}

			switch(listitem)
			{
				case 0:// Bar position
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_POSITION;
				}
				case 1: // Bar size
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_SIZE;
				}
				case 2: // Bar Direction
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_DIRECTION;
				}
				case 3: // Bar Max value
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_MAX_VALUE;
					ShowPlayerDialog(playerid, DIALOG_BAREDIT+1, DIALOG_STYLE_INPUT, DIALOG_CAPTION, "Write the new max level:", "Okay", "Back");
				}
				case 4: // Bar color
				{
					gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_COLOR;
					ShowPlayerDialog(playerid, DIALOG_BAREDIT+2, DIALOG_STYLE_LIST, DIALOG_CAPTION, "1.\tWhite\n2.\tBlack\n3.\tBlue\n4.\tRed\n5.\tGreen\n6.\tYellow\n7.\tOrange\n8.\tPink\n9.\tPurple\n10.\tCustom (HEX)", "Select", "Back");
				}
				case 5: // Delete this bar
				{
					DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
					ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
					PlaySelectSound(playerid);
				}
				case 6: // Export this bar
				{
					new Float:X, Float:Y;
					GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], X, Y);

					new Float:width;
					width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

					new Float:height;
					height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

					new color;
					color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

					new direction;
					direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

					new directionName[32];
					switch(direction)
					{
						case BAR_DIRECTION_RIGHT:	directionName = "BAR_DIRECTION_RIGHT";
						case BAR_DIRECTION_LEFT:	directionName = "BAR_DIRECTION_LEFT";
						case BAR_DIRECTION_UP:		directionName = "BAR_DIRECTION_UP";
						case BAR_DIRECTION_DOWN:	directionName = "BAR_DIRECTION_DOWN";
					}

					new textToSave[128];
					new File:barFile = fopen("bars.txt", io_append);
					format(textToSave, 256, "CreatePlayerProgressBar(playerid, %f, %f, %f, %f, %d, %s);\n", X, Y, width, height, color, directionName);
					fwrite(barFile, textToSave);
					fclose(barFile);

					ShowPlayerDialog(playerid, DIALOG_EDITOR, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_INFO, "Select", "Cancel");
					SendClientMessage(playerid, COLOR_WHITE, "* {00b9e8}Bar exported{ffffff} successful.");
					PlaySelectSound(playerid);
				}
			}
		}
		case DIALOG_BAREDIT+1:
		{
			if(!response)
			{
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlayCancelSound(playerid);
				return 1;
			}

			new bool:isnumeric = true;
			for(new i = 0, j = strlen(inputtext); i < j; i++)
			{
				if(inputtext[i] > '9' || inputtext[i] < '0')
				{
					isnumeric = false;
					break;
				}
			}

			if(strlen(inputtext) < 1 || isnumeric == false)
			{
				SendClientMessage(playerid, COLOR_ERROR, "* Invalid value.");
				ShowPlayerDialog(playerid, DIALOG_BAREDIT+1, DIALOG_STYLE_INPUT, DIALOG_CAPTION, "Write the new max level:", "Okay", "Back");
			}

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, floatstr(inputtext), direction);
			SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
			ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
			PlaySelectSound(playerid);

			new message[128];
			format(message, sizeof(message), "* Max value set to {00b9e8}%.2f{ffffff}.", floatstr(inputtext));
			SendClientMessage(playerid, COLOR_WHITE, message);
			return 1;
		}
		case DIALOG_BAREDIT+2:
		{
			if(!response)
			{
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlayCancelSound(playerid);
				return 1;				
			}

			switch(listitem)
			{
				case 0: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0xffffffff); // White
				case 1: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0x191919ff); // Black
				case 2: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0x4646eaff); // Blue
				case 3: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0xff0508ff); // Red
				case 4: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0x46ea46ff); // Green
				case 5: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0xeaea46ff); // Yellow
				case 6: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0xff4a00ff); // Orange
				case 7: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0xea46eaff); // Pink
				case 8: SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], 0x612759ff); // Purple
				case 9: // Custom (HEX)
				{

					ShowPlayerDialog(playerid, DIALOG_BAREDIT+3, DIALOG_STYLE_INPUT, DIALOG_CAPTION, "Insert the color (RRGGBBAA):\n(Recommended FF for alpha)", "Select", "Back");
					PlaySelectSound(playerid);
					return 1;
				}
			}

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:maxval;
			maxval = GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, direction);
			SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
			ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
			PlaySelectSound(playerid);
			return 1;
		}
		case DIALOG_BAREDIT+3:
		{
			if(!response)
			{
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlayCancelSound(playerid);
				return 1;				
			}

			if(strlen(inputtext) != 8)
			{
				SendClientMessage(playerid, COLOR_ERROR, "* Invalid format. (RRGGBBAA)");
				ShowPlayerDialog(playerid, DIALOG_BAREDIT+3, DIALOG_STYLE_INPUT, DIALOG_CAPTION, "Insert the color (RRGGBBAA):\n(Recommended FF for alpha)", "Select", "Back");
				PlayErrorSound(playerid);
				return 1;
			}

			SetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], HexToInt(inputtext));

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:maxval;
			maxval = GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, direction);
			SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
			ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
			PlaySelectSound(playerid);
		}
	}
	return 0;
}

//------------------------------------------------------------------------------

public OnPlayerUpdate(playerid)
{
	if(gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] == EDITING_NONE)
		return 1;

	new Keys, ud, lr;
	GetPlayerKeys(playerid, Keys, ud, lr);
	
	switch(gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE])
	{
		case EDITING_POSITION:
		{
			GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~n~~g~ARROWS ~w~TO MOVE, ~g~SPRINT ~w~TO BOOST, ~g~F ~w~TO FINISH", 1250, 3);

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:maxval;
			maxval = GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			if(ud == KEY_UP)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y-1, width, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(ud == KEY_DOWN)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y+1, width, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		 
			if(lr == KEY_LEFT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x-1, y, width, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(lr == KEY_RIGHT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x+1, y, width, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		}
		case EDITING_SIZE:
		{
			GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~n~~g~ARROWS ~w~TO CHANGE, ~g~SPRINT ~w~TO BOOST, ~g~F ~w~TO FINISH", 1250, 3);

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:maxval;
			maxval = GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			if(ud == KEY_UP)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height-1, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(ud == KEY_DOWN)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height+1, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		 
			if(lr == KEY_LEFT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width-1, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(lr == KEY_RIGHT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width+1, height, color, maxval, direction);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		}
		case EDITING_DIRECTION:
		{
			GameTextForPlayer(playerid, "~n~~n~~n~~n~~n~~n~~n~~n~~g~ARROWS ~w~TO CHANGE, ~g~F ~w~TO FINISH", 1250, 3);

			new Float:x, Float:y;
			GetPlayerProgressBarPos(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], x, y);

			new Float:width;
			width = GetPlayerProgressBarWidth(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:height;
			height = GetPlayerProgressBarHeight(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new color;
			color = GetPlayerProgressBarColour(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new direction;
			direction = GetPlayerProgressBarDirection(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			new Float:maxval;
			maxval = GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);

			if(ud == KEY_UP)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				if(direction == BAR_DIRECTION_UP || direction == BAR_DIRECTION_DOWN) gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, BAR_DIRECTION_UP);
				else gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, height, width, color, maxval, BAR_DIRECTION_UP);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(ud == KEY_DOWN)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				if(direction == BAR_DIRECTION_UP || direction == BAR_DIRECTION_DOWN) gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, BAR_DIRECTION_DOWN);
				else gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, height, width, color, maxval, BAR_DIRECTION_DOWN);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		 
			if(lr == KEY_LEFT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				if(direction == BAR_DIRECTION_LEFT || direction == BAR_DIRECTION_RIGHT) gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, BAR_DIRECTION_LEFT);
				else gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, height, width, color, maxval, BAR_DIRECTION_LEFT);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
			else if(lr == KEY_RIGHT)
			{
				DestroyPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
				if(direction == BAR_DIRECTION_LEFT || direction == BAR_DIRECTION_RIGHT) gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxval, BAR_DIRECTION_RIGHT);
				else gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID] = CreatePlayerProgressBar(playerid, x, y, height, width, color, maxval, BAR_DIRECTION_RIGHT);
				SetPlayerProgressBarValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID], floatdiv(GetPlayerProgressBarMaxValue(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]), 3));
				ShowPlayerProgressBar(playerid, gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]);
			}
		}
	}
	return 1;
}

//------------------------------------------------------------------------------

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	switch(gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE])
	{
		case EDITING_POSITION:
		{
			if(newkeys == KEY_SECONDARY_ATTACK)
			{
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_NONE;
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlaySelectSound(playerid);
			}
		}
		case EDITING_SIZE:
		{
			if(newkeys == KEY_SECONDARY_ATTACK)
			{
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_NONE;
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlaySelectSound(playerid);
			}
		}
		case EDITING_DIRECTION:
		{
			if(newkeys == KEY_SECONDARY_ATTACK)
			{
				gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE] = EDITING_NONE;
				ShowPlayerDialog(playerid, DIALOG_BAREDIT, DIALOG_STYLE_LIST, DIALOG_CAPTION, DIALOG_BAR, "Select", "Back");
				PlaySelectSound(playerid);
			}
		}
	}
	return 1;
}

//------------------------------------------------------------------------------

public OnPlayerConnect(playerid)
{
	ResetPlayerVars(playerid);
	return 1;
}

//------------------------------------------------------------------------------

public OnPlayerDisconnect(playerid, reason)
{
	ResetPlayerVars(playerid);
	return 1;
}

//------------------------------------------------------------------------------

ResetPlayerVars(playerid)
{
	gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_MODE]	= EDITING_NONE;	
	gPlayerData[playerid][E_PE_PLAYER_IS_EDITING]		= false;	
	gPlayerData[playerid][E_PE_PLAYER_BAR_EDITING_ID]	= INVALID_PLAYER_BAR_ID;
}

//------------------------------------------------------------------------------

stock HexToInt(string[])
{// Made by Dracoblue
	new i = 0;
	new cur = 1;
	new res = 0;
	for (i = strlen(string); i > 0; i--)
	{
		string[i-1] = toupper(string[i-1]);
		if (string[i-1] < 58) res = res + cur*(string[i-1] - 48); else res = res + cur*(string[i-1] - 65 + 10);
		cur = cur*16;
	}
	return res;
}
