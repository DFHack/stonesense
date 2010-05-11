#include <iostream>
#include <fstream>
#include <string>
#include "common.h"

using namespace std;

string parseStrFromLine( string keyword, string line ){
	string retVal = "";
	string trimString = "";
	trimString += "[";
	trimString += keyword;
	trimString += ":";
	int length = (int)trimString.length();


	if( line.compare(0,length, trimString) == 0){
		line.replace(0,length,"");
		line.replace(line.length()-1,1,"");
		retVal = line;
	}

	return retVal;
}

int parseIntFromLine( string keyword, string line ){
	int retVal = 0;
	string trimString = "";
	trimString += "[";
	trimString += keyword;
	trimString += ":";
	int length = (int)trimString.length();


	if( line.compare(0,length, trimString) == 0){
		line.replace(0,length,"");
		line.replace(line.length()-1,1,"");
		retVal = atoi( line.c_str() );
	}

	return retVal;
}


void parseConfigLine( string line ){
	char c = line[0];
	if( c != '[') return;

	//some systems don't remove the \r char as a part of the line change:
	if(line.size() > 0 &&  line[line.size() -1 ] == '\r' )
		line.resize(line.size() -1);

	c = line[ line.length() -1 ];
	if( c != ']' ) return;

	if( line.find("[WIDTH") != -1){
		int width = parseIntFromLine( "WIDTH", line );
		config.screenWidth = width;
	}
	if( line.find("[HEIGHT") != -1){
		int height = parseIntFromLine( "HEIGHT", line );
		config.screenHeight = height;
	}
	if( line.find("[WINDOWED") != -1){
		string result = parseStrFromLine( "WINDOWED", line );
		config.Fullscreen = (result == "NO");
	}
	if( line.find("[SEGMENTSIZE_XY") != -1){
		int value = parseIntFromLine( "SEGMENTSIZE_XY", line );
		if(value < 5) value = DEFAULT_SEGMENTSIZE;
		if(value > 100) value = 100;
		//plus 2 to allow edge readings
		config.segmentSize.x = value+2;
		config.segmentSize.y = value+2;
	}
	if( line.find("[SEGMENTSIZE_Z") != -1){
		int value = parseIntFromLine( "SEGMENTSIZE_Z", line );
		if(value < 1) value = DEFAULT_SEGMENTSIZE_Z;
		if(value > 255) value = 255;
		config.segmentSize.z = value;
	}

	if( line.find("[ALLCREATURES") != -1){
		string result = parseStrFromLine( "ALLCREATURES", line );
		config.show_all_creatures = (result == "YES");
	}
	if( line.find("[AUTO_RELOAD_STEP") != -1){
		int value = parseIntFromLine( "AUTO_RELOAD_STEP", line);
		if(value < 50) value = 50;
		config.automatic_reload_step = value;
	}
	if( line.find("[AUTO_RELOAD_TIME") != -1){
		int value = parseIntFromLine( "AUTO_RELOAD_TIME", line);
		if(value < 0) value = 0;
		config.automatic_reload_time = value;
	}
	if( line.find("[DEBUGMODE") != -1){
		string result = parseStrFromLine( "DEBUGMODE", line );
		config.debug_mode = (result == "YES");
	}
	if( line.find("[LIFTSEGMENT") != -1){
		int value = parseIntFromLine( "LIFTSEGMENT", line);
		config.lift_segment_offscreen = value;
	}
	if( line.find("[ANIMATION_RATE") != -1){
		int value = parseIntFromLine( "ANIMATION_RATE", line );
		if(value < 50) value = 50;
		config.animation_step = value;
	}
	if( line.find("[VERBOSE_LOGGING") != -1){
		string result = parseStrFromLine( "VERBOSE_LOGGING", line );
		config.verbose_logging = (result == "YES");
	}
	if( line.find("[TRACK_CENTER") != -1){
		string result = parseStrFromLine( "TRACK_CENTER", line );
		config.track_center = (result == "YES");
	}
	if( line.find("[FOLLOW_DF_SCREEN") != -1){
		string result = parseStrFromLine( "FOLLOW_DF_SCREEN", line );
		config.follow_DFscreen = (result == "YES");
	}
	if( line.find("[FOLLOW_DF_CURSOR") != -1){
		string result = parseStrFromLine( "FOLLOW_DF_CURSOR", line );
		config.follow_DFcursor = (result == "YES");
	}
	if( line.find("[SHOW_CREATURE_NAMES") != -1){
		string result = parseStrFromLine( "SHOW_CREATURE_NAMES", line );
		config.show_creature_names = (result == "YES");
	}
	if( line.find("[NAMES_USE_NICKNAME") != -1){
		string result = parseStrFromLine( "NAMES_USE_NICKNAME", line );
		config.names_use_nick = (result == "YES");
	}
	if( line.find("[NAMES_USE_SPECIES") != -1){
		string result = parseStrFromLine( "NAMES_USE_SPECIES", line );
		config.names_use_species = (result == "YES");
	}
	if( line.find("[SHOW_OSD") != -1){
		string result = parseStrFromLine( "SHOW_OSD", line );
		config.show_osd = (result == "YES");
	}
	if( line.find("[CACHE_IMAGES") != -1){
		string result = parseStrFromLine( "CACHE_IMAGES", line );
		config.cache_images = (result == "YES");
	}
	if( line.find("[SHOW_STOCKPILES") != -1){
		string result = parseStrFromLine( "SHOW_STOCKPILES", line );
		config.show_stockpiles = (result == "YES");
	}
	if( line.find("[SHOW_ZONES") != -1){
		string result = parseStrFromLine( "SHOW_ZONES", line );
		config.show_zones = (result == "YES");
	}
	if( line.find("[INTRO") != -1){
		string result = parseStrFromLine( "INTRO", line );
		config.show_intro = !(result == "OFF");
	}
	if( line.find("[FOG_RED") != -1){
		int value = parseIntFromLine( "FOG_RED", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.fogr = value;
	}
	if( line.find("[FOG_GREEN") != -1){
		int value = parseIntFromLine( "FOG_GREEN", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.fogg = value;
	}
	if( line.find("[FOG_BLUE") != -1){
		int value = parseIntFromLine( "FOG_BLUE", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.fogb = value;
	}
	if( line.find("[FOG_ALPHA") != -1){
		int value = parseIntFromLine( "FOG_ALPHA", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.foga = value;
	}
	if( line.find("[SHOW_FOG") != -1){
		string result = parseStrFromLine( "SHOW_FOG", line );
		config.fogenable = (result == "YES");
	}
	if( line.find("[BACK_RED") != -1){
		int value = parseIntFromLine( "BACK_RED", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.backr = value;
	}
	if( line.find("[BACK_GREEN") != -1){
		int value = parseIntFromLine( "BACK_GREEN", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.backg = value;
	}
	if( line.find("[BACK_BLUE") != -1){
		int value = parseIntFromLine( "BACK_BLUE", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.backb = value;
	}
	if( line.find("[FOLLOW_OFFSET_X") != -1){
		int value = parseIntFromLine( "FOLLOW_OFFSET_X", line);
		if(value > 30) value = 30;
		if(value < -30) value = -30;
		config.viewXoffset = value;
	}
	if( line.find("[FOLLOW_OFFSET_Y") != -1){
		int value = parseIntFromLine( "FOLLOW_OFFSET_Y", line);
		if(value > 30) value = 30;
		if(value < -30) value = -30;
		config.viewYoffset = value;
	}
	if( line.find("[FOLLOW_OFFSET_Z") != -1){
		int value = parseIntFromLine( "FOLLOW_OFFSET_Z", line);
		if(value > 30) value = 30;
		if(value < -30) value = -30;
		config.viewZoffset = value;
	}
	if( line.find("[LOG_IMAGECACHE") != -1){
		string result = parseStrFromLine( "LOG_IMAGECACHE", line );
		config.saveImageCache = (result == "YES");
	}
	if( line.find("[IMAGE_CACHE_SIZE") != -1){
		int value = parseIntFromLine( "IMAGE_CACHE_SIZE", line);
		config.imageCacheSize = value;
	}
	if( line.find("[FONTSIZE") != -1){
		int value = parseIntFromLine( "FONTSIZE", line);
		if(value < 1) value = 1;
		config.fontsize = value;
	}
	if( line.find("[FONT") != -1){
		string result = parseStrFromLine( "FONT", line );
		config.font = al_create_path(result.c_str());
	}
	if( line.find("[BLACK_R") != -1)
	{
		int value = parseIntFromLine( "BLACK_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.black_r = value;
	}
	if( line.find("[BLACK_G") != -1)
	{
		int value = parseIntFromLine( "BLACK_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.black_g = value;
	}
	if( line.find("[BLACK_B") != -1)
	{
		int value = parseIntFromLine( "BLACK_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.black_b = value;
	}
	if( line.find("[BLUE_R") != -1)
	{
		int value = parseIntFromLine( "BLUE_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.blue_r = value;
	}
	if( line.find("[BLUE_G") != -1)
	{
		int value = parseIntFromLine( "BLUE_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.blue_g = value;
	}
	if( line.find("[BLUE_B") != -1)
	{
		int value = parseIntFromLine( "BLUE_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.blue_b = value;
	}
	if( line.find("[GREEN_R") != -1)
	{
		int value = parseIntFromLine( "GREEN_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.green_r = value;
	}
	if( line.find("[GREEN_G") != -1)
	{
		int value = parseIntFromLine( "GREEN_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.green_g = value;
	}
	if( line.find("[GREEN_B") != -1)
	{
		int value = parseIntFromLine( "GREEN_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.green_b = value;
	}
	if( line.find("[CYAN_R") != -1)
	{
		int value = parseIntFromLine( "CYAN_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.cyan_r = value;
	}
	if( line.find("[CYAN_G") != -1)
	{
		int value = parseIntFromLine( "CYAN_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.cyan_g = value;
	}
	if( line.find("[CYAN_B") != -1)
	{
		int value = parseIntFromLine( "CYAN_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.cyan_b = value;
	}
	if( line.find("[RED_R") != -1)
	{
		int value = parseIntFromLine( "RED_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.red_r = value;
	}
	if( line.find("[RED_G") != -1)
	{
		int value = parseIntFromLine( "RED_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.red_g = value;
	}
	if( line.find("[RED_B") != -1)
	{
		int value = parseIntFromLine( "RED_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.red_b = value;
	}
	if( line.find("[MAGENTA_R") != -1)
	{
		int value = parseIntFromLine( "MAGENTA_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.magenta_r = value;
	}
	if( line.find("[MAGENTA_G") != -1)
	{
		int value = parseIntFromLine( "MAGENTA_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.magenta_g = value;
	}
	if( line.find("[MAGENTA_B") != -1)
	{
		int value = parseIntFromLine( "MAGENTA_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.magenta_b = value;
	}
	if( line.find("[BROWN_R") != -1)
	{
		int value = parseIntFromLine( "BROWN_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.brown_r = value;
	}
	if( line.find("[BROWN_G") != -1)
	{
		int value = parseIntFromLine( "BROWN_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.brown_g = value;
	}
	if( line.find("[BROWN_B") != -1){
		int value = parseIntFromLine( "BROWN_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.brown_b = value;
	}
	if( line.find("[LGRAY_R") != -1){
		int value = parseIntFromLine( "LGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgray_r = value;
	}
	if( line.find("[LGRAY_R") != -1){
		int value = parseIntFromLine( "LGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgray_g = value;
	}
	if( line.find("[LGRAY_R") != -1){
		int value = parseIntFromLine( "LGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgray_b = value;
	}
	if( line.find("[DGRAY_R") != -1){
		int value = parseIntFromLine( "DGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.dgray_r = value;
	}
	if( line.find("[DGRAY_R") != -1){
		int value = parseIntFromLine( "DGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.dgray_g = value;
	}
	if( line.find("[DGRAY_R") != -1){
		int value = parseIntFromLine( "DGRAY_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.dgray_b = value;
	}
	if( line.find("[LBLUE_R") != -1){
		int value = parseIntFromLine( "LBLUE_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lblue_r = value;
	}
	if( line.find("[LBLUE_G") != -1){
		int value = parseIntFromLine( "LBLUE_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lblue_g = value;
	}
	if( line.find("[LBLUE_B") != -1){
		int value = parseIntFromLine( "LBLUE_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lblue_b = value;
	}
	if( line.find("[LGREEN_R") != -1){
		int value = parseIntFromLine( "LGREEN_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgreen_r = value;
	}
	if( line.find("[LGREEN_G") != -1){
		int value = parseIntFromLine( "LGREEN_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgreen_g = value;
	}
	if( line.find("[LGREEN_B") != -1){
		int value = parseIntFromLine( "LGREEN_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lgreen_b = value;
	}
	if( line.find("[LCYAN_R") != -1){
		int value = parseIntFromLine( "LCYAN_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lcyan_r = value;
	}
	if( line.find("[LCYAN_G") != -1){
		int value = parseIntFromLine( "LCYAN_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lcyan_g = value;
	}
	if( line.find("[LCYAN_B") != -1){
		int value = parseIntFromLine( "LCYAN_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lcyan_b = value;
	}
	if( line.find("[LRED_R") != -1){
		int value = parseIntFromLine( "LRED_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lred_r = value;
	}
	if( line.find("[LRED_G") != -1){
		int value = parseIntFromLine( "LRED_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lred_g = value;
	}
	if( line.find("[LRED_B") != -1){
		int value = parseIntFromLine( "LRED_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lred_b = value;
	}
	if( line.find("[LMAGENTA_R") != -1){
		int value = parseIntFromLine( "LMAGENTA_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lmagenta_r = value;
	}
	if( line.find("[LMAGENTA_G") != -1){
		int value = parseIntFromLine( "LMAGENTA_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lmagenta_g = value;
	}
	if( line.find("[LMAGENTA_B") != -1){
		int value = parseIntFromLine( "LMAGENTA_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.lmagenta_b = value;
	}
	if( line.find("[YELLOW_R") != -1){
		int value = parseIntFromLine( "YELLOW_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.yellow_r = value;
	}
	if( line.find("[YELLOW_G") != -1){
		int value = parseIntFromLine( "YELLOW_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.yellow_g = value;
	}
	if( line.find("[YELLOW_B") != -1){
		int value = parseIntFromLine( "YELLOW_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.yellow_b = value;
	}
	if( line.find("[WHITE_R") != -1){
		int value = parseIntFromLine( "WHITE_R", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.white_r = value;
	}
	if( line.find("[WHITE_G") != -1){
		int value = parseIntFromLine( "WHITE_G", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.white_g = value;
	}
	if( line.find("[WHITE_B") != -1){
		int value = parseIntFromLine( "WHITE_B", line);
		if(value > 255) value = 255;
		if(value < 0) value = 0;
		config.colors.white_b = value;
	}
		if( line.find("[RENDERER") != -1){
		string result = parseStrFromLine( "RENDERER", line );
		config.opengl = (result == "OPENGL");
		config.software = (result == "SOFTWARE");
		config.directX = (result == "DIRECTX");
	}
}


bool loadConfigFile(){
	string line;
	ifstream myfile ("init.txt");
	if (myfile.is_open() == false)
	{
		cout << "Cannot find init file" << endl;
		return false;
	}

	while ( !myfile.eof() )
	{
		getline (myfile,line);
		cout << line << endl;
		parseConfigLine( line );
	}
	myfile.close();
	return true;
}
