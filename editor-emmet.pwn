/*
 *  Progress bar editor by Emmet_
 *  Made for Southclaw's Progress Bars v2.
 *
 *  Features:
 *  - Ability to adjust all settings, including direction, color, and size.
 *  - Uses SQLite for better organization.
 *  - Supports multiple progress bars.
*/

#include <a_samp>
#include <progress2>
#include <zcmd>

#define COLOR_INFO  (0xFFDD00FF)
#define COLOR_ERROR (0xFF5030FF)

#define PROJECT_DB_PATH     "projects.db" // Path of the file that stores all of the project names.
#define PROJECT_EXPORT_PATH "%s.pwn" // Path of the export file location.
#define ADMIN_RESTRICTION   false  // Restrict the progress bar editor to RCON admins only?
#define MAX_PROJECT_NAME    24     // Maximum amount of characters in a project name.

#define DIALOG_MAIN_MENU        18300
#define DIALOG_LOAD_PROJECT     18301
#define DIALOG_LOAD_CUSTOM      18302
#define DIALOG_DELETE_PROJECT   18303
#define DIALOG_DELETE_CUSTOM    18304
#define DIALOG_DELETE_CONFIRM   18305
#define DIALOG_CREATE_PROJECT   18306
#define DIALOG_PROJECT_MENU     18307
#define DIALOG_BAR_MENU         18308
#define DIALOG_COLOR_INPUT      18309
#define DIALOG_DIRECTION_LIST   18310
#define DIALOG_MAX_VALUE        18311

#define LIST_TYPE_LOAD     1
#define LIST_TYPE_DELETE   2

#define UPDATE_BAR_POSITION   1
#define UPDATE_BAR_SIZE       2
#define UPDATE_BAR_DIRECTION  3
#define UPDATE_BAR_MAX_VALUE  4
#define UPDATE_BAR_COLOR      5

#define EDIT_TYPE_POSITION   1
#define EDIT_TYPE_SIZE       2

enum e_ProjectData
{
	// Indicates if a project is active.
	e_ProjectActive,

	// Name of the project.
	e_ProjectName[MAX_PROJECT_NAME],

	// Name of the project to delete.
	e_ProjectToDelete[MAX_PROJECT_NAME],

	// Editing type. (1) Position (2) Size
	e_EditType,

	// Index of the selected progress bar, relative to "ProjectBars"
	e_SelectedBar
};

new Project[MAX_PLAYERS][e_ProjectData];

enum e_ProjectBars
{
	// Database ID of the progress bar's record.
	e_DatabaseID,

	// ID of the progress bar.
	PlayerBar:e_ProgressBarID
};

new ProjectBars[MAX_PLAYERS][MAX_PLAYER_BARS][e_ProjectBars];

new DB:g_iDatabase;

new DBResult:g_iDatabaseResult;

public OnFilterScriptInit()
{
	for (new i = 0; i < MAX_PLAYERS; i ++)
	{
		if (IsPlayerConnected(i)) ResetProjectData(i);
	}
	g_iDatabase = db_open(PROJECT_DB_PATH);

	db_query(g_iDatabase, "CREATE TABLE IF NOT EXISTS `projects` (name VARCHAR("#MAX_PROJECT_NAME"), creator VARCHAR(24))");
	db_query(g_iDatabase, "CREATE TABLE IF NOT EXISTS `bars` (bar_id INTEGER PRIMARY KEY AUTOINCREMENT, project VARCHAR("#MAX_PROJECT_NAME"), x_pos FLOAT, y_pos FLOAT, width FLOAT, height FLOAT, max_value FLOAT, color INTEGER, direction INTEGER, FOREIGN KEY (project) REFERENCES projects(name))");

	print("\nProgress Bar Editor loaded!");
	print("Use the /bar command to begin editing.\n");
	return 1;
}

public OnFilterScriptExit()
{
	for (new i = 0; i < MAX_PLAYERS; i ++)
	{
		if (Project[i][e_ProjectActive])
		{
			Project_Close(i);
			ShowPlayerDialog(i, -1, DIALOG_STYLE_LIST, " ", " ", " ", "");
		}
	}
	db_close(g_iDatabase);

	print("Progress Bar Editor unloaded!");
	return 1;
}

public OnPlayerConnect(playerid)
{
	ResetProjectData(playerid);
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	if (Project[playerid][e_ProjectActive])
	{
		Project_Close(playerid);
	}
	return 1;
}

public OnPlayerSpawn(playerid)
{
	#if ADMIN_RESTRICTION == true
		if (IsPlayerAdmin(playerid)) SendClientMessage(playerid, COLOR_INFO, "Use the /bar command to begin editing!");
	#elseif ADMIN_RESTRICTION == false
		SendClientMessage(playerid, COLOR_INFO, "Use the /bar command to begin editing!");
	#endif

	return 1;
}

public OnPlayerUpdate(playerid)
{
	new
		keys,
		ud,
		lr;

	if (Project[playerid][e_ProjectActive] && Project[playerid][e_SelectedBar] != -1)
	{
		new
			PlayerBar:barid = ProjectBars[playerid][Project[playerid][e_SelectedBar]][e_ProgressBarID],
			string[128],
			Float:x,
			Float:y;

		GetPlayerKeys(playerid, keys, ud, lr);

		GetPlayerProgressBarPos(playerid, barid, x, y);

		if (keys & KEY_SECONDARY_ATTACK)
		{
			switch (Project[playerid][e_EditType])
			{
				case EDIT_TYPE_POSITION:
				{
					UpdateProgressBarData(playerid, Project[playerid][e_SelectedBar], UPDATE_BAR_POSITION);
					SetTimerEx("ShowProgressBarMenu", 300, false, "dd", playerid, _:barid);

					format(string, sizeof(string), "You have set the position of bar #%d.", Project[playerid][e_SelectedBar]);
					SendClientMessage(playerid, COLOR_INFO, string);
				}
				case EDIT_TYPE_SIZE:
				{
					UpdateProgressBarData(playerid, Project[playerid][e_SelectedBar], UPDATE_BAR_SIZE);
					SetTimerEx("ShowProgressBarMenu", 300, false, "dd", playerid, _:barid);

					format(string, sizeof(string), "You have set the size of bar #%d.", Project[playerid][e_SelectedBar]);
					SendClientMessage(playerid, COLOR_INFO, string);
				}
			}
			TogglePlayerControllable(playerid, 1);
			SetPlayerProgressBarValue(playerid, barid, 50.0);

			Project[playerid][e_EditType] = 0;
		}
		if (ud == KEY_UP)
		{
			if (Project[playerid][e_EditType] == EDIT_TYPE_POSITION)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarPos(playerid, barid, x, y - 5.0);
				else
					SetPlayerProgressBarPos(playerid, barid, x, y - 1.0);
			}
			else if (Project[playerid][e_EditType] == EDIT_TYPE_SIZE)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarHeight(playerid, barid, GetPlayerProgressBarHeight(playerid, barid) - 1.0);
				else
					SetPlayerProgressBarHeight(playerid, barid, GetPlayerProgressBarHeight(playerid, barid) - 0.5);
			}
		}
		if (ud == KEY_DOWN)
		{
			if (Project[playerid][e_EditType] == EDIT_TYPE_POSITION)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarPos(playerid, barid, x, y + 5.0);
				else
					SetPlayerProgressBarPos(playerid, barid, x, y + 1.0);
			}
			else if (Project[playerid][e_EditType] == EDIT_TYPE_SIZE)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarHeight(playerid, barid, GetPlayerProgressBarHeight(playerid, barid) + 1.0);
				else
					SetPlayerProgressBarHeight(playerid, barid, GetPlayerProgressBarHeight(playerid, barid) + 0.5);
			}
		}
		if (lr == KEY_LEFT)
		{
			if (Project[playerid][e_EditType] == EDIT_TYPE_POSITION)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarPos(playerid, barid, x - 5.0, y);
				else
					SetPlayerProgressBarPos(playerid, barid, x - 1.0, y);
			}
			else if (Project[playerid][e_EditType] == EDIT_TYPE_SIZE)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarWidth(playerid, barid, GetPlayerProgressBarWidth(playerid, barid) - 1.0);
				else
					SetPlayerProgressBarWidth(playerid, barid, GetPlayerProgressBarWidth(playerid, barid) - 0.5);
			}
		}
		if (lr == KEY_RIGHT)
		{
			if (Project[playerid][e_EditType] == EDIT_TYPE_POSITION)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarPos(playerid, barid, x + 5.0, y);
				else
					SetPlayerProgressBarPos(playerid, barid, x + 1.0, y);
			}
			else if (Project[playerid][e_EditType] == EDIT_TYPE_SIZE)
			{
				if (keys == KEY_SPRINT)
					SetPlayerProgressBarWidth(playerid, barid, GetPlayerProgressBarWidth(playerid, barid) + 1.0);
				else
					SetPlayerProgressBarWidth(playerid, barid, GetPlayerProgressBarWidth(playerid, barid) + 0.5);
			}
		}
	}
	return 1;
}

/*
	Function:
		ResetProjectData
	Parameters:
		playerid - The player to reset the data for.
	Returns:
		No significant value.
*/

stock ResetProjectData(playerid)
{
	for (new i = 0; i < MAX_PLAYER_BARS; i ++)
	{
		ProjectBars[playerid][i][e_DatabaseID] = 0;
		ProjectBars[playerid][i][e_ProgressBarID] = INVALID_PLAYER_BAR_ID;
	}
	Project[playerid][e_ProjectActive] = 0;
	Project[playerid][e_ProjectName] = 0;
	Project[playerid][e_EditType] = 0;
	Project[playerid][e_SelectedBar] = -1;
}

/*
	Function:
		Project_ExportPath
	Parameters:
		name[] - Name of the project.
	Returns:
		The export file path for the specified project name.
*/

stock Project_ExportPath(name[])
{
	new
		path[MAX_PROJECT_NAME + 14];

	format(path, sizeof(path), PROJECT_EXPORT_PATH, name);
	return path;
}

/*
	Function:
		Project_Exists
	Parameters:
		name[] - Name of the project.
	Returns:
		Returns 1 if the project file exists.
*/

stock Project_Exists(name[])
{
	new
		string[128],
		rows;

	format(string, sizeof(string), "SELECT `name` FROM `projects` WHERE `name` = '%s'", name);

	g_iDatabaseResult = db_query(g_iDatabase, string);

	rows = db_num_rows(g_iDatabaseResult);

	db_free_result(g_iDatabaseResult);

	return (rows > 0);
}

/*
	Function:
		Project_Remove
	Parameters:
		name[] - Name of the project.
	Returns:
		Returns 1 if the project was removed.
*/

stock Project_Remove(name[])
{
	if (Project_Exists(name))
	{
		new
			string[128];

		format(string, sizeof(string), "DELETE FROM projects WHERE name = '%s'", name);
		db_query(g_iDatabase, string);

		format(string, sizeof(string), "DELETE FROM bars WHERE project = '%s'", name);
		db_query(g_iDatabase, string);

		return 1;
	}
	return 0;
}

/*
	Function:
		Project_IsOpen
	Parameters:
		name[] - Name of the project.
	Returns:
		Returns 1 if the project is opened by another player.
*/

stock Project_IsOpen(name[])
{
	for (new i = 0; i < MAX_PLAYERS; i ++)
	{
		if (!IsPlayerConnected(i)) continue;

		if (Project[i][e_ProjectActive] && !strcmp(Project[i][e_ProjectName], name, true))
		{
			return 1;
		}
	}
	return 0;
}

/*
	Function:
		Project_Open
	Parameters:
		playerid - The player that is editing the project.
		name[] - Name of the project.
	Returns:
		1: if the project was successfully opened.
		0: if another player is already editing that project.
*/

stock Project_Open(playerid, name[])
{
	new
		str[128],
		rows
	;
	if (!Project_Exists(name))
	{
		// If it doesn't exist in the database, then add it in there.
		GetPlayerName(playerid, str, sizeof(str));

		format(str, sizeof(str), "INSERT INTO projects (name, creator) VALUES('%s', '%s')", name, str);
		db_query(g_iDatabase, str);
	}
	else if (Project_IsOpen(name)) // Another player is already editing this project.
	{
		return 0;
	}

	new
		Float:x,
		Float:y,
		Float:width,
		Float:height,
		Float:maxvalue,
		color,
		direction
	;

	format(str, sizeof(str), "SELECT * FROM `bars` WHERE `project` = '%s'", name);

	g_iDatabaseResult = db_query(g_iDatabase, str);

	rows = db_num_rows(g_iDatabaseResult);

	for (new i = 0; i < rows; i ++)
	{
		if (i >= MAX_PLAYER_BARS)
		{
			printf("Warning: Project \"%s\" contains %d bars; limit is %d.", name, rows, MAX_PLAYER_BARS);
			break;
		}
		db_get_field_assoc(g_iDatabaseResult, "bar_id", str, sizeof(str));
		ProjectBars[playerid][i][e_DatabaseID] = strval(str);

		db_get_field_assoc(g_iDatabaseResult, "x_pos", str, sizeof(str));
		x = floatstr(str);

		db_get_field_assoc(g_iDatabaseResult, "y_pos", str, sizeof(str));
		y = floatstr(str);

		db_get_field_assoc(g_iDatabaseResult, "width", str, sizeof(str));
		width = floatstr(str);

		db_get_field_assoc(g_iDatabaseResult, "height", str, sizeof(str));
		height = floatstr(str);

		db_get_field_assoc(g_iDatabaseResult, "max_value", str, sizeof(str));
		maxvalue = floatstr(str);

		db_get_field_assoc(g_iDatabaseResult, "color", str, sizeof(str));
		color = strval(str);

		db_get_field_assoc(g_iDatabaseResult, "direction", str, sizeof(str));
		direction = strval(str);

		ProjectBars[playerid][i][e_ProgressBarID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, maxvalue, direction);

		SetPlayerProgressBarValue(playerid, ProjectBars[playerid][i][e_ProgressBarID], 50.0);
		ShowPlayerProgressBar(playerid, ProjectBars[playerid][i][e_ProgressBarID]);

		db_next_row(g_iDatabaseResult);
	}
	db_free_result(g_iDatabaseResult);

	Project[playerid][e_ProjectActive] = 1;

	return strcat(Project[playerid][e_ProjectName], name, MAX_PROJECT_NAME);
}

/*
	Function:
		ShowProjectMainMenu
	Parameters:
		playerid - The player to show the main menu for.
	Returns:
		1: if the menu was successfully shown.
		0: if the player is not editing any projects.
*/

stock ShowProjectMainMenu(playerid)
{
	if (Project[playerid][e_ProjectActive])
	{
		ShowProjectMenu(playerid);
	}
	else
	{
		ShowPlayerDialog(playerid, DIALOG_MAIN_MENU, DIALOG_STYLE_LIST, "Main Menu", "Create project...\nLoad project...\nDelete project...", "Select", "Cancel");
	}
	return 1;
}

/*
	Function:
		ShowProjectMenu
	Parameters:
		playerid - The player to show the menu for.
	Returns:
		1: if the menu was successfully shown.
		0: if the player is not editing any projects.
*/

forward ShowProjectMenu(playerid);
public ShowProjectMenu(playerid)
{
	if (Project[playerid][e_ProjectActive])
	{
		new
			string[1024];

		format(string, sizeof(string), "Create progress bar...\nExport project...\nClose project...");

		for (new i = 0; i < MAX_PLAYER_BARS; i ++)
		{
			if (ProjectBars[playerid][i][e_ProgressBarID] == INVALID_PLAYER_BAR_ID)
				continue;

			switch (GetPlayerProgressBarDirection(playerid, ProjectBars[playerid][i][e_ProgressBarID]))
			{
				case BAR_DIRECTION_LEFT:
					format(string, sizeof(string), "%s\n- Bar #%d - Direction: Left", string, i);
				case BAR_DIRECTION_RIGHT:
					format(string, sizeof(string), "%s\n- Bar #%d - Direction: Right", string, i);
				case BAR_DIRECTION_UP:
					format(string, sizeof(string), "%s\n- Bar #%d - Direction: Up", string, i);
				case BAR_DIRECTION_DOWN:
					format(string, sizeof(string), "%s\n- Bar #%d - Direction: Down", string, i);
			}
		}
		ShowPlayerDialog(playerid, DIALOG_PROJECT_MENU, DIALOG_STYLE_LIST, "Project Menu", string, "Select", "Cancel");
		return 1;
	}
	return 0;
}

/*
	Function:
		Project_ShowList
	Parameters:
		playerid - The player to show the list to.
		type - The type of the list.
	Returns:
		1: if the list was successfully shown.
		0: if there wasn't anything to display.
*/

stock Project_ShowList(playerid, type)
{
	new
		string[MAX_PROJECT_NAME + 3],
		buffer[1024],
		rows;

	strcat(buffer, "Custom name...\n");

	g_iDatabaseResult = db_query(g_iDatabase, "SELECT `name` FROM projects");

	rows = db_num_rows(g_iDatabaseResult);

	for (new i = 0; i < rows; i ++)
	{
		db_get_field(g_iDatabaseResult, 0, string, sizeof(string));
		db_next_row(g_iDatabaseResult);

		strcat(buffer, string);
		strcat(buffer, "\n");
	}
	db_free_result(g_iDatabaseResult);

	if (isnull(buffer))
	{
		return 0;
	}
	switch (type)
	{
		case LIST_TYPE_LOAD:
		{
			ShowPlayerDialog(playerid, DIALOG_LOAD_PROJECT, DIALOG_STYLE_LIST, "Load project...", buffer, "Load", "Cancel");
		}
		case LIST_TYPE_DELETE:
		{
			ShowPlayerDialog(playerid, DIALOG_DELETE_PROJECT, DIALOG_STYLE_LIST, "Delete project...", buffer, "Delete", "Cancel");
		}
	}
	return 1;
}

/*
	Function:
		Project_Close
	Parameters:
		playerid - The player to close the project for.
	Returns:
		1: if the project was closed.
		0: if there wasn't any project loaded.
*/

stock Project_Close(playerid)
{
	if (Project[playerid][e_ProjectActive])
	{
		if (Project[playerid][e_EditType] > 0)
		{
			TogglePlayerControllable(playerid, 1);
		}
		for (new i = 0; i < MAX_PLAYER_BARS; i ++)
		{
			if (ProjectBars[playerid][i][e_ProgressBarID] != INVALID_PLAYER_BAR_ID)
			{
				HidePlayerProgressBar(playerid, ProjectBars[playerid][i][e_ProgressBarID]);
				DestroyPlayerProgressBar(playerid, ProjectBars[playerid][i][e_ProgressBarID]);
			}
		}
		ResetProjectData(playerid);
		return 1;
	}
	return 0;
}

/*
	Function:
		ShowProgressBarMenu
	Parameters:
		playerid - The player to show the menu to.
		PlayerBar:barid - The ID of the player bar.
	Returns:
		1: if the menu was shown.
		0: if the player bar doesn't exist.
*/

forward ShowProgressBarMenu(playerid, PlayerBar:barid);
public ShowProgressBarMenu(playerid, PlayerBar:barid)
{
	if (Project[playerid][e_ProjectActive])
	{
		new
			string[512],
			title[24];

		Project[playerid][e_SelectedBar] = GetProgressBarInternalID(playerid, barid);
		format(title, sizeof(title), "Bar #%d", Project[playerid][e_SelectedBar]);

		format(string, sizeof(string), "Change position\nChange size\nChange color\nChange direction (%s)\nChange max value (%.4f)\nDuplicate bar\nDelete this bar", GetDirectionFromType(GetPlayerProgressBarDirection(playerid, barid)), GetPlayerProgressBarMaxValue(playerid, barid));
		ShowPlayerDialog(playerid, DIALOG_BAR_MENU, DIALOG_STYLE_LIST, title, string, "Select", "Back");
	}
	return 1;
}

/*
	Function:
		GetProgressBarInternalID
	Parameters:
		playerid - The player ID of the progress bar.
		PlayerBar:barid - The ID of the progress bar.
	Returns:
		The index of "ProjectBars" that contains the progress bar ID.
*/

stock GetProgressBarInternalID(playerid, PlayerBar:barid)
{
	for (new i = 0; i < MAX_PLAYER_BARS; i ++)
	{
		if (ProjectBars[playerid][i][e_ProgressBarID] == barid)
		{
			return i;
		}
	}
	return -1;
}

/*
	Function:
		DuplicateProgresssBar
	Parameters:
		playerid - The player ID to duplicate the progress bar for.
		index - The index of the bar, relative to "ProjectBars".
	Returns:
		The index relative to "ProjectBars" that contains the new progress bar ID.
*/

stock DuplicateProgressBar(playerid, index)
{
	if (Project[playerid][e_ProjectActive])
	{
		new
			PlayerBar:barid = ProjectBars[playerid][index][e_ProgressBarID],
			Float:x,
			Float:y;

		GetPlayerProgressBarPos(playerid, barid, x, y);
		return AddBarToProject(playerid, x, y, GetPlayerProgressBarWidth(playerid, barid), GetPlayerProgressBarHeight(playerid, barid), GetPlayerProgressBarMaxValue(playerid, barid), GetPlayerProgressBarColour(playerid, barid), GetPlayerProgressBarDirection(playerid, barid));
	}
	return -1;
}

/*
	Function:
		AddBarToProject
	Parameters:
		playerid - The player ID to add the bar for.
		... - Parameters. Check "CreatePlayerProgressBar" for more details.
	Returns:
		The index relative to "ProjectBars" that contains the new progress bar ID.
*/

stock AddBarToProject(playerid, Float:x = 280.0, Float:y = 200.0, Float:width = 55.5, Float:height = 3.2, Float:max_value = 100.0, color = -1429936641, direction = BAR_DIRECTION_RIGHT)
{
	new
		string[160];

	if (!Project[playerid][e_ProjectActive])
	{
		return -1;
	}
	for (new i = 0; i < MAX_PLAYER_BARS; i ++)
	{
		if (ProjectBars[playerid][i][e_ProgressBarID] != INVALID_PLAYER_BAR_ID) continue;

		format(string, sizeof(string), "INSERT INTO `bars` VALUES(null, '%s', %.6f, %.6f, %.6f, %.6f, %.4f, %d, %d)", Project[playerid][e_ProjectName], x, y, width, height, max_value, color, direction);
		db_query(g_iDatabase, string);

		ProjectBars[playerid][i][e_ProgressBarID] = CreatePlayerProgressBar(playerid, x, y, width, height, color, max_value, direction);
		ProjectBars[playerid][i][e_DatabaseID] = db_last_insert_id(g_iDatabase, "bars");

		SetPlayerProgressBarValue(playerid, ProjectBars[playerid][i][e_ProgressBarID], 50.0);
		ShowPlayerProgressBar(playerid, ProjectBars[playerid][i][e_ProgressBarID]);
		return i;
	}
	return -1;
}

/*
	Function:
		UpdateProgressBarData
	Parameters:
		playerid - The player ID to add the bar for.
		index - Index of the progress bar, relative to "ProjectBars".
		type - Type of data to update in the database.
	Returns:
		Returns 1 if the data was successfully updated.
*/

stock UpdateProgressBarData(playerid, index, type)
{
	new
		PlayerBar:barid = ProjectBars[playerid][index][e_ProgressBarID],
		string[128];

	if (IsValidPlayerProgressBar(playerid, barid))
	{
		switch (type)
		{
			case UPDATE_BAR_POSITION:
			{
				new
					Float:x,
					Float:y;

				GetPlayerProgressBarPos(playerid, barid, x, y);

				format(string, sizeof(string), "UPDATE `bars` SET `x_pos` = %.6f, `y_pos` = %.6f WHERE `bar_id` = %d", x, y, ProjectBars[playerid][index][e_DatabaseID]);
				db_query(g_iDatabase, string);
			}
			case UPDATE_BAR_SIZE:
			{
				format(string, sizeof(string), "UPDATE `bars` SET `width` = %.6f, `height` = %.6f WHERE `bar_id` = %d", GetPlayerProgressBarWidth(playerid, barid), GetPlayerProgressBarHeight(playerid, barid), ProjectBars[playerid][index][e_DatabaseID]);
				db_query(g_iDatabase, string);
			}
			case UPDATE_BAR_DIRECTION:
			{
				format(string, sizeof(string), "UPDATE `bars` SET `direction` = %d WHERE `bar_id` = %d", GetPlayerProgressBarDirection(playerid, barid), ProjectBars[playerid][index][e_DatabaseID]);
				db_query(g_iDatabase, string);
			}
			case UPDATE_BAR_MAX_VALUE:
			{
				format(string, sizeof(string), "UPDATE `bars` SET `max_value` = %.6f WHERE `bar_id` = %d", GetPlayerProgressBarMaxValue(playerid, barid), ProjectBars[playerid][index][e_DatabaseID]);
				db_query(g_iDatabase, string);
			}
			case UPDATE_BAR_COLOR:
			{
				format(string, sizeof(string), "UPDATE `bars` SET `color` = %d WHERE `bar_id` = %d", GetPlayerProgressBarColour(playerid, barid), ProjectBars[playerid][index][e_DatabaseID]);
				db_query(g_iDatabase, string);
			}
		}
		return 1;
	}
	return 0;
}

stock GetDirectionFromType(direction)
{
	new
		str[6];

	if (direction == BAR_DIRECTION_LEFT)
		str = "Left";
	else if (direction == BAR_DIRECTION_RIGHT)
		str = "Right";
	else if (direction == BAR_DIRECTION_UP)
		str = "Up";
	else if (direction == BAR_DIRECTION_DOWN)
		str = "Down";

	return str;
}

stock StrToHex(str[])
{
	// Credits to Y_Less.

	new
		i,
		value;

	if (str[0] == '0' && (str [1] == 'x' || str [1] == 'X'))
		i = 2;

	while (str[i])
	{
		value <<= 4;

		switch (str[i])
		{
			case '0'..'9':
				value |= str [i] - '0';

			case 'A'..'F':
				value |= str [i] - 'A' + 10;

			case 'a'..'f':
				value |= str [i] - 'a' + 10;

			default:
				return 0;
		}
		++ i;
	}
	return value;
}

stock db_last_insert_id(DB:database, const table[])
{
	new
		DBResult:result,
		string[64];

	format(string, sizeof(string), "SELECT last_insert_rowid() FROM %s", table);

	result = db_query(database, string);

	db_get_field(result, 0, string, sizeof(string));

	db_free_result(result);

	return strval(string);
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	new
		string[255],
		PlayerBar:barid,
		index = -1;

	if (Project[playerid][e_ProjectActive] && (index = Project[playerid][e_SelectedBar]) != -1)
	{
		barid = ProjectBars[playerid][Project[playerid][e_SelectedBar]][e_ProgressBarID];
	}

	switch (dialogid)
	{
		case DIALOG_MAIN_MENU:
		{
			if (response)
			{
				switch (listitem)
				{
					case 0: // Create new project
					{
						ShowPlayerDialog(playerid, DIALOG_CREATE_PROJECT, DIALOG_STYLE_INPUT, "Create new project...", "Please enter the name of your new project below:\nThe name must contain only letters, numbers and spaces.", "Submit", "Back");
					}
					case 1: // Load existing project
					{
						Project_ShowList(playerid, LIST_TYPE_LOAD);
					}
					case 2: // Delete project
					{
						new
							success = Project_ShowList(playerid, LIST_TYPE_DELETE);

						if (!success) return SendClientMessage(playerid, COLOR_ERROR, "There are no projects to delete.");
					}
				}
			}
		}
		case DIALOG_CREATE_PROJECT:
		{
			if (response)
			{
				if (isnull(inputtext))
				{
					return ShowPlayerDialog(playerid, DIALOG_CREATE_PROJECT, DIALOG_STYLE_INPUT, "Create new project...", "Please enter the name of your new project below:\nThe name must contain only letters, numbers and spaces.", "Submit", "Back");
				}
				else if (strlen(inputtext) > 32)
				{
					return ShowPlayerDialog(playerid, DIALOG_CREATE_PROJECT, DIALOG_STYLE_INPUT, "Create new project...", "Please enter the name of your new project below:\nThe name must contain only letters, numbers and spaces.", "Submit", "Back");
				}
				else if (Project_Exists(inputtext))
				{
					return ShowPlayerDialog(playerid, DIALOG_CREATE_PROJECT, DIALOG_STYLE_INPUT, "Create new project...", "The specified project name is already in use!\n\nPlease enter the name of your new project below:\nThe name must contain only letters, numbers and spaces.", "Submit", "Back");
				}
				else
				{
					for (new i = 0, l = strlen(inputtext); i < l; i ++)
					{
						switch (inputtext[i])
						{
							case '\\', '/', ':', '*', '"', '?', '<', '>', '|', '\'':
							{
								return ShowPlayerDialog(playerid, DIALOG_CREATE_PROJECT, DIALOG_STYLE_INPUT, "Create new project...", "You have entered invalid characters. Please remove them and try again.\n\nPlease enter the name of your new project below:\nThe name must contain only letters, numbers and spaces.", "Submit", "Back");
							}
						}
					}
					format(string, sizeof(string), "You have created project \"%s\".", inputtext);
					SendClientMessage(playerid, COLOR_INFO, string);

					Project_Open(playerid, inputtext);
					ShowProjectMenu(playerid);
				}
			}
			else
			{
				ShowProjectMainMenu(playerid);
			}
		}
		case DIALOG_LOAD_PROJECT:
		{
			if (response)
			{
				if (!listitem) // Custom name...
				{
					ShowPlayerDialog(playerid, DIALOG_LOAD_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "Please enter the name of the project you wish to load:", "Submit", "Back");
				}
				else
				{
					new
						pos = -1;

					// strip extra characters
					if ((pos = strfind(inputtext, "\r")) != -1) strdel(inputtext, pos, pos + 1);
					if ((pos = strfind(inputtext, "\n")) != -1) strdel(inputtext, pos, pos + 1);

					format(string, sizeof(string), "You have loaded project \"%s\".", inputtext);
					SendClientMessage(playerid, COLOR_INFO, string);

					Project_Open(playerid, inputtext);
					ShowProjectMenu(playerid);
				}
			}
			else
			{
				ShowProjectMainMenu(playerid);
			}
		}
		case DIALOG_LOAD_CUSTOM:
		{
			if (response)
			{
				if (isnull(inputtext) || strlen(inputtext) > 32)
				{
					ShowPlayerDialog(playerid, DIALOG_LOAD_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "Please enter the name of the project you wish to load:", "Submit", "Back");
				}
				else if (!Project_Exists(inputtext))
				{
					ShowPlayerDialog(playerid, DIALOG_LOAD_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "The specified project name doesn't exist.\n\nPlease enter the name of the project you wish to load:", "Submit", "Back");
				}
				else if (Project_IsOpen(inputtext))
				{
					ShowPlayerDialog(playerid, DIALOG_LOAD_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "The specified project is being edited by another player.\n\nPlease enter the name of the project you wish to load:", "Submit", "Back");
				}
				else
				{
					format(string, sizeof(string), "You have loaded project \"%s\".", inputtext);
					SendClientMessage(playerid, COLOR_INFO, string);

					Project_Open(playerid, inputtext);
					ShowProjectMenu(playerid);
				}
			}
			else
			{
				Project_ShowList(playerid, LIST_TYPE_LOAD);
			}
		}
		case DIALOG_DELETE_PROJECT:
		{
			if (response)
			{
				if (!listitem) // Custom name...
				{
					ShowPlayerDialog(playerid, DIALOG_DELETE_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "Please enter the name of the project you wish to delete:", "Submit", "Back");
				}
				else
				{
					new
						pos = -1;

					// strip extra characters
					if ((pos = strfind(inputtext, "\r")) != -1) strdel(inputtext, pos, pos + 1);
					if ((pos = strfind(inputtext, "\n")) != -1) strdel(inputtext, pos, pos + 1);

					Project[playerid][e_ProjectToDelete] = 0;
					strcat(Project[playerid][e_ProjectToDelete], inputtext, MAX_PROJECT_NAME);

					format(string, sizeof(string), "You are about to delete project \"%s\"!\nYou cannot recover a project once it is deleted.", inputtext);
					ShowPlayerDialog(playerid, DIALOG_DELETE_CONFIRM, DIALOG_STYLE_MSGBOX, "Delete project...", string, "Yes", "No");
				}
			}
			else
			{
				ShowProjectMainMenu(playerid);
			}
		}
		case DIALOG_DELETE_CUSTOM:
		{
			if (response)
			{
				if (isnull(inputtext) || strlen(inputtext) > 32)
				{
					ShowPlayerDialog(playerid, DIALOG_DELETE_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "Please enter the name of the project you wish to delete:", "Submit", "Back");
				}
				else if (!Project_Exists(inputtext))
				{
					ShowPlayerDialog(playerid, DIALOG_DELETE_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "The specified project name doesn't exist.\n\nPlease enter the name of the project you wish to delete:", "Submit", "Back");
				}
				else if (Project_IsOpen(inputtext))
				{
					ShowPlayerDialog(playerid, DIALOG_DELETE_CUSTOM, DIALOG_STYLE_INPUT, "Custom name...", "The specified project is being edited by another player.\n\nPlease enter the name of the project you wish to delete:", "Submit", "Back");
				}
				else
				{
					Project[playerid][e_ProjectToDelete] = 0;
					strcat(Project[playerid][e_ProjectToDelete], inputtext, MAX_PROJECT_NAME);

					format(string, sizeof(string), "You are about to delete project \"%s\"!\nYou cannot recover a project once it is deleted.", inputtext);
					ShowPlayerDialog(playerid, DIALOG_DELETE_CONFIRM, DIALOG_STYLE_MSGBOX, "Delete project...", string, "Yes", "No");
				}
			}
			else
			{
				Project_ShowList(playerid, LIST_TYPE_LOAD);
			}
		}
		case DIALOG_DELETE_CONFIRM:
		{
			if (response)
			{
				Project_Remove(Project[playerid][e_ProjectToDelete]);

				format(string, sizeof(string), "You have deleted project \"%s\".", Project[playerid][e_ProjectToDelete]);
				SendClientMessage(playerid, COLOR_INFO, string);

				Project[playerid][e_ProjectToDelete] = 0;
			}
			else
			{
				ShowProjectMainMenu(playerid);
			}
		}
		case DIALOG_PROJECT_MENU:
		{
			if (response)
			{
				switch (listitem)
				{
					case 0: // Create progress bar...
					{
						new
							id = AddBarToProject(playerid);

						if (id != -1)
						{
							format(string, sizeof(string), "You have created progress bar #%d.", id);
							SendClientMessage(playerid, COLOR_INFO, string);

							ShowProjectMenu(playerid);
							return 1;
						}
						else
						{
							SendClientMessage(playerid, COLOR_ERROR, "Please adjust the \"MAX_PLAYER_BARS\" setting in \"progress2.inc\" to add more progress bars.");
							ShowProjectMenu(playerid);
						}
					}
					case 1: // Export project...
					{
						new
							File:file = fopen(Project_ExportPath(Project[playerid][e_ProjectName]), io_write),
							Float:x,
							Float:y,
							date[6]
						;

						if (file)
						{
							getdate(date[0], date[1], date[2]);
							gettime(date[3], date[4], date[5]);

							format(string, sizeof(string), "/*\r\n * Project Name: %s\r\n * Date: %02d/%02d/%d @ %02d:%02d:%02d\r\n\r\n * The code below is to be used with the Progress Bar V2 include.\r\n *\r\n*/\r\n\r\n", Project[playerid][e_ProjectName], date[2], date[1], date[0], date[3], date[4], date[5]);
							fwrite(file, string);

							fwrite(file, "#include <a_samp>\r\n#include <progress2>\r\n\r\n");

							for (new i = 0; i < MAX_PLAYER_BARS; i ++)
							{
								if (ProjectBars[playerid][i][e_ProgressBarID] != INVALID_PLAYER_BAR_ID)
								{
									format(string, sizeof(string), "new PlayerBar:Bar%d[MAX_PLAYERS];\r\n", i);
									fwrite(file, string);
								}
							}
							fwrite(file, "\r\npublic OnPlayerConnect(playerid)\r\n{\r\n");

							for (new i = 0; i < MAX_PLAYER_BARS; i ++)
							{
								if (ProjectBars[playerid][i][e_ProgressBarID] == INVALID_PLAYER_BAR_ID)
									continue;

								GetPlayerProgressBarPos(playerid, ProjectBars[playerid][i][e_ProgressBarID], x, y);

								format(string, sizeof(string), "    Bar%d[playerid] = CreatePlayerProgressBar(playerid, %.6f, %.6f, %.6f, %.6f, %d, %.4f, %d);\r\n",
									i,
									x,
									y,
									GetPlayerProgressBarWidth(playerid, ProjectBars[playerid][i][e_ProgressBarID]),
									GetPlayerProgressBarHeight(playerid, ProjectBars[playerid][i][e_ProgressBarID]),
									GetPlayerProgressBarColour(playerid, ProjectBars[playerid][i][e_ProgressBarID]),
									GetPlayerProgressBarMaxValue(playerid, ProjectBars[playerid][i][e_ProgressBarID]),
									GetPlayerProgressBarDirection(playerid, ProjectBars[playerid][i][e_ProgressBarID])
								);

								fwrite(file, string);
							}
							fwrite(file, "\r\n    return 1;\r\n}\r\n\r\npublic OnPlayerSpawn(playerid)\r\n{\r\n");

							for (new i = 0; i < MAX_PLAYER_BARS; i ++)
							{
								if (ProjectBars[playerid][i][e_ProgressBarID] == INVALID_PLAYER_BAR_ID)
									continue;

								format(string, sizeof(string), "    ShowPlayerProgressBar(playerid, Bar%d[playerid]);\r\n", i);
								fwrite(file, string);
							}
							fwrite(file, "\r\n    return 1;\r\n}\r\n");
							fclose(file);

							format(string, sizeof(string), "Project has been exported to \"%s\".", Project_ExportPath(Project[playerid][e_ProjectName]));
							SendClientMessage(playerid, COLOR_INFO, string);

							ShowProjectMenu(playerid);
						}
					}
					case 2: // Close project...
					{
						format(string, sizeof(string), "You have closed project \"%s\".", Project[playerid][e_ProjectName]);
						SendClientMessage(playerid, COLOR_INFO, string);

						Project_Close(playerid);
						ShowProjectMainMenu(playerid);
					}
					default: // Player selected a bar
					{
						index = strval(inputtext[7]);

						if (!IsValidPlayerProgressBar(playerid, ProjectBars[playerid][index][e_ProgressBarID]))
						{
							SendClientMessage(playerid, COLOR_ERROR, "Whoops! There seems to be a problem here... Please contact Emmet.");
						}
						else
						{
							ShowProgressBarMenu(playerid, ProjectBars[playerid][index][e_ProgressBarID]);
						}
					}
				}
			}
		}
		case DIALOG_BAR_MENU:
		{
			if (response)
			{
				switch (listitem)
				{
					case 0: // Change position
					{
						Project[playerid][e_EditType] = EDIT_TYPE_POSITION;
						TogglePlayerControllable(playerid, 0);

						format(string, sizeof(string), "You are changing bar #%d's position. Use the arrow keys to move the bar and press Enter when done.", index);
						SendClientMessage(playerid, COLOR_INFO, string);
					}
					case 1: // Change size
					{
						Project[playerid][e_EditType] = EDIT_TYPE_SIZE;
						TogglePlayerControllable(playerid, 0);

						format(string, sizeof(string), "You are changing bar #%d's size. Use the arrow keys to adjust the size and press Enter when done.", index);
						SendClientMessage(playerid, COLOR_INFO, string);
					}
					case 2: // Change color
					{
						ShowPlayerDialog(playerid, DIALOG_COLOR_INPUT, DIALOG_STYLE_INPUT, "Change color", "Please enter the new color for this progress bar below:\nYou must enter a hexadecimal value. The hex color for white is 0xFFFFFFFF.", "Submit", "Back");
					}
					case 3: // Change direction
					{
						ShowPlayerDialog(playerid, DIALOG_DIRECTION_LIST, DIALOG_STYLE_LIST, "Change direction", "Right\nLeft\nUp\nDown", "Select", "Back");
					}
					case 4: // Change max value
					{
						format(string, sizeof(string), "Please enter the new maximum value for this bar below (current: %.4f):", GetPlayerProgressBarMaxValue(playerid, barid));
						ShowPlayerDialog(playerid, DIALOG_MAX_VALUE, DIALOG_STYLE_INPUT, "Change max value", string, "Submit", "Back");
					}
					case 5: // Duplicate bar
					{
						new
							id = DuplicateProgressBar(playerid, index);

						if (id != -1)
						{
							format(string, sizeof(string), "You have duplicated progress bar #%d (new ID: #%d).", index, id);
							SendClientMessage(playerid, COLOR_INFO, string);

							ShowProjectMenu(playerid);
						}
						else
						{
							SendClientMessage(playerid, COLOR_ERROR, "Please adjust the \"MAX_PLAYER_BARS\" setting in \"progress2.inc\" to add more progress bars.");
							ShowProjectMenu(playerid);
						}
					}
					case 6: // Delete this bar
					{
						format(string, sizeof(string), "DELETE FROM `bars` WHERE `bar_id` = %d", ProjectBars[playerid][index][e_DatabaseID]);
						db_query(g_iDatabase, string);

						HidePlayerProgressBar(playerid, barid);
						DestroyPlayerProgressBar(playerid, barid);

						format(string, sizeof(string), "You have deleted progress bar #%d.", index);
						SendClientMessage(playerid, COLOR_INFO, string);

						ProjectBars[playerid][index][e_ProgressBarID] = INVALID_PLAYER_BAR_ID;
						ProjectBars[playerid][index][e_DatabaseID] = 0;

						Project[playerid][e_SelectedBar] = -1;

						return ShowProjectMenu(playerid);
					}
				}
			}
			else
			{
				ShowProjectMenu(playerid);
			}
		}
		case DIALOG_COLOR_INPUT:
		{
			if (response)
			{
				if (isnull(inputtext) || (strlen(inputtext) != 6 && strlen(inputtext) != 8))
				{
					ShowPlayerDialog(playerid, DIALOG_COLOR_INPUT, DIALOG_STYLE_INPUT, "Change color", "Please enter the new color for this progress bar below:\nYou must enter a hexadecimal value. The hex color for white is 0xFFFFFFFF.", "Submit", "Back");
				}
				else
				{
					new color = StrToHex(inputtext);

					SetPlayerProgressBarColour(playerid, barid, color);

					ShowPlayerProgressBar(playerid, barid);
					UpdateProgressBarData(playerid, index, UPDATE_BAR_COLOR);

					format(string, sizeof(string), "You have set bar #%d's color to {%06x}%s.", index, color >>> 8, inputtext);
					SendClientMessage(playerid, COLOR_INFO, string);

					ShowProgressBarMenu(playerid, barid);
				}
			}
			else
			{
				ShowProgressBarMenu(playerid, barid);
			}
		}
		case DIALOG_DIRECTION_LIST:
		{
			if (response)
			{
				SetPlayerProgressBarDirection(playerid, barid, listitem);
				UpdateProgressBarData(playerid, index, UPDATE_BAR_DIRECTION);

				format(string, sizeof(string), "You have changed bar #%d's direction to %s.", index, GetDirectionFromType(listitem));
				SendClientMessage(playerid, COLOR_INFO, string);

				ShowProgressBarMenu(playerid, barid);
			}
			else
			{
				ShowProgressBarMenu(playerid, barid);
			}
		}
		case DIALOG_MAX_VALUE:
		{
			if (response)
			{
				if (isnull(inputtext))
				{
					format(string, sizeof(string), "Please enter the new maximum value for this bar below (current: %.4f):", GetPlayerProgressBarMaxValue(playerid, barid));
					ShowPlayerDialog(playerid, DIALOG_MAX_VALUE, DIALOG_STYLE_INPUT, "Change max value", string, "Submit", "Back");
				}
				else
				{
					new
						Float:max_value = floatstr(inputtext);

					SetPlayerProgressBarMaxValue(playerid, barid, max_value);
					UpdateProgressBarData(playerid, index, UPDATE_BAR_MAX_VALUE);

					format(string, sizeof(string), "You have changed bar #%d's maximum value to %.4f.", index, max_value);
					SendClientMessage(playerid, COLOR_INFO, string);

					ShowProgressBarMenu(playerid, barid);
				}
			}
			else
			{
				ShowProgressBarMenu(playerid, barid);
			}
		}
	}
	return 0;
}

CMD:bar(playerid, params[])
{
	#if ADMIN_RESTRICTION == true
		if (!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, COLOR_ERROR, "You must be an RCON admin to use the editor.");
	#endif

	if (Project[playerid][e_EditType] > 0)
	{
		return SendClientMessage(playerid, COLOR_ERROR, "You must finish editing before you can use this command.");
	}
	ShowProjectMainMenu(playerid);
	return 1;
}