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
  if( line.find("SEGMENTX") != -1){
    int value = parseIntFromLine( "SEGMENTX", line );
    if(value < 1) value = DEFAULT_SEGMENTSIZE_X;
    if(value > 100) value = 100;
    config.segmentSize.x = value;
  }
  if( line.find("SEGMENTY") != -1){
    int value = parseIntFromLine( "SEGMENTY", line );
    if(value < 1) value = DEFAULT_SEGMENTSIZE_Y;
    if(value > 100) value = 100;
    config.segmentSize.y = value;
  }
  if( line.find("SEGMENTZ") != -1){
    int value = parseIntFromLine( "SEGMENTZ", line );
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
  if( line.find("DEBUGMODE") != -1){
    string result = parseStrFromLine( "DEBUGMODE", line );
    config.debug_mode = (result == "YES");
  }
}


bool loadConfigFile(){
  string line;
  ifstream myfile ("init.txt");
  if (myfile.is_open() == false)
    return false;

  while ( !myfile.eof() )
  {
    getline (myfile,line);
    cout << line << endl;
    parseConfigLine( line );
  }
  myfile.close();
  
  
  return true;
}