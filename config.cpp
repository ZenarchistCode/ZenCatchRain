// This mod is a slightly improved version of CatchRain by PoPo - all credit to him: https://steamcommunity.com/sharedfiles/filedetails/?id=2648040518

class CfgPatches
{
	class ZenCatchRain
	{
		requiredVersion = 0.1;
		units[] =
		{
			""
		};
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Gear_Books",
		};
	};
};

class CfgMods
{
	class ZenCatchRain
	{
		dir = "ZenCatchRain";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "ZenCatchRain";
		credits = "PoPo";
		author = "Zenarchist";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game","World","Mission" };
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = { "ZenCatchRain/scripts/3_game" };
			};
			class worldScriptModule
			{
				value = "";
				files[] = { "ZenCatchRain/scripts/4_world" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "ZenCatchRain/scripts/5_mission" };
			};
		};
	};
};