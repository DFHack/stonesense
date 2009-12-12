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

  if( line.find("WIDTH") != -1){
    int width = parseIntFromLine( "WIDTH", line );
    config.screenWidth = width;
  }
  if( line.find("HEIGHT") != -1){
    int height = parseIntFromLine( "HEIGHT", line );
    config.screenHeight = height;
  }
  if( line.find("WINDOWED") != -1){
    string result = parseStrFromLine( "WINDOWED", line );
    config.Fullscreen = (result == "NO");
  }
  if( line.find("SEGMENTSIZE_XY") != -1){
    int value = parseIntFromLine( "SEGMENTSIZE_XY", line );
    if(value < 5) value = DEFAULT_SEGMENTSIZE;
    if(value > 100) value = 100;
    //plus 2 to allow edge readings
    config.segmentSize.x = value+2;
    config.segmentSize.y = value+2;
  }
  if( line.find("SEGMENTSIZE_Z") != -1){
    int value = parseIntFromLine( "SEGMENTSIZE_Z", line );
    if(value < 1) value = DEFAULT_SEGMENTSIZE_Z;
    if(value > 30) value = 30;
    config.segmentSize.z = value;
  }

  if( line.find("ALLCREATURES") != -1){
    string result = parseStrFromLine( "ALLCREATURES", line );
    config.show_all_creatures = (result == "YES");
  }
  if( line.find("AUTO_RELOAD_STEP") != -1){
    int value = parseIntFromLine( "AUTO_RELOAD_STEP", line);
    if(value < 50) value = 50;
    config.automatic_reload_step = value;
  }
   if( line.find("AUTO_RELOAD_TIME") != -1){
    int value = parseIntFromLine( "AUTO_RELOAD_TIME", line);
    if(value < 0) value = 0;
    config.automatic_reload_time = value;
  }
  if( line.find("DEBUGMODE") != -1){
    string result = parseStrFromLine( "DEBUGMODE", line );
    config.debug_mode = (result == "YES");
  }
  if( line.find("LIFTSEGMENT") != -1){
    int value = parseIntFromLine( "LIFTSEGMENT", line);
    config.lift_segment_offscreen = value;
  }
   if( line.find("ANIMATION_RATE") != -1){
    int value = parseIntFromLine( "ANIMATION_RATE", line );
    if(value < 50) value = 50;
    config.animation_step = value;
  }
  if( line.find("VERBOSE_LOGGING") != -1){
    string result = parseStrFromLine( "VERBOSE_LOGGING", line );
    config.verbose_logging = (result == "YES");
  }
  if( line.find("TRACK_CENTER") != -1){
    string result = parseStrFromLine( "TRACK_CENTER", line );
    config.track_center = (result == "YES");
  }
  if( line.find("FOLLOW_DF_SCREEN") != -1){
    string result = parseStrFromLine( "FOLLOW_DF_SCREEN", line );
    config.follow_DFscreen = (result == "YES");
  }
  if( line.find("SHOW_CREATURE_NAMES") != -1){
    string result = parseStrFromLine( "SHOW_CREATURE_NAMES", line );
    config.show_creature_names = (result == "YES");
  }
  if( line.find("SHOW_OSD") != -1){
    string result = parseStrFromLine( "SHOW_OSD", line );
    config.show_osd = (result == "YES");
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