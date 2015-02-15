#include "common.h"
#include "Config.h"
#include "UserInput.h"

//should match allegrow/keycodes.h
string keynames[] = {
    "INVALID",

    "KEY_A",
    "KEY_B",
    "KEY_C",
    "KEY_D",
    "KEY_E",
    "KEY_F",
    "KEY_G",
    "KEY_H",
    "KEY_I",
    "KEY_J",
    "KEY_K",
    "KEY_L",
    "KEY_M",
    "KEY_N",
    "KEY_O",
    "KEY_P",
    "KEY_Q",
    "KEY_R",
    "KEY_S",
    "KEY_T",
    "KEY_U",
    "KEY_V",
    "KEY_W",
    "KEY_X",
    "KEY_Y",
    "KEY_Z",

    "KEY_0",
    "KEY_1",
    "KEY_2",
    "KEY_3",
    "KEY_4",
    "KEY_5",
    "KEY_6",
    "KEY_7",
    "KEY_8",
    "KEY_9",

    "KEYPAD_0",
    "KEYPAD_1",
    "KEYPAD_2",
    "KEYPAD_3",
    "KEYPAD_4",
    "KEYPAD_5",
    "KEYPAD_6",
    "KEYPAD_7",
    "KEYPAD_8",
    "KEYPAD_9",

    "KEYF_1",
    "KEYF_2",
    "KEYF_3",
    "KEYF_4",
    "KEYF_5",
    "KEYF_6",
    "KEYF_7",
    "KEYF_8",
    "KEYF_9",
    "KEYF_10",
    "KEYF_11",
    "KEYF_12",

    "KEYS_ESCAPE",
    "KEYS_TILDE",
    "KEYS_MINUS",
    "KEYS_EQUALS",
    "KEYS_BACKSPACE",
    "KEYS_TAB",
    "KEYS_OPENBRACE",
    "KEYS_CLOSEBRACE",
    "KEYS_ENTER",
    "KEYS_SEMICOLON",
    "KEYS_QUOTE",
    "KEYS_BACKSLASH",
    "KEYS_BACKSLASH2", /* DirectInput calls this DIK_OEM_102: "< > | on UK/Germany KEYSboards" */
    "KEYS_COMMA",
    "KEYS_FULLSTOP",
    "KEYS_SLASH",
    "KEYS_SPACE",

    "KEYS_INSERT",
    "KEYS_DELETE",
    "KEYS_HOME",
    "KEYS_END",
    "KEYS_PGUP",
    "KEYS_PGDN",
    "KEYS_LEFT",
    "KEYS_RIGHT",
    "KEYS_UP",
    "KEYS_DOWN",

    "KEYPAD_SLASH",
    "KEYPAD_ASTERISK",
    "KEYPAD_MINUS",
    "KEYPAD_PLUS",
    "KEYPAD_DELETE",
    "KEYPAD_ENTER",

    "KEYS_PRINTSCREEN",
    "KEYS_PAUSE",

    "KEYS_ABNT_C1",
    "KEYS_YEN",
    "KEYS_KANA",
    "KEYS_CONVERT",
    "KEYS_NOCONVERT",
    "KEYS_AT",
    "KEYS_CIRCUMFLEX",
    "KEYS_COLON2",
    "KEYS_KANJI",

    "KEYPAD_EQUALS",    /* MacOS X */
    "KEYS_BACKQUOTE",    /* MacOS X */
    "KEYS_SEMICOLON2",    /* MacOS X */
    "KEYS_COMMAND"    /* MacOS X */

    //KEY_UNKNOWN
};

int getKeyCode(string& keyName){
    for(int i=0; i<ALLEGRO_KEY_UNKNOWN; i++) {
        if(keynames[i] == keyName) {
            return i;
        }
    }
    return ALLEGRO_KEY_UNKNOWN;
}

////should match allegrow/keycodes.h
//string keymodnames[] = {
//    "KEYMOD_SHIFT",
//    "KEYMOD_CTRL",
//    "KEYMOD_ALT",
//    "KEYMOD_LWIN",
//    "KEYMOD_RWIN",
//    "KEYMOD_MENU",
//    "KEYMOD_ALTGR",
//    "KEYMOD_COMMAND",
//    "KEYMOD_SCROLLLOCK",
//    "KEYMOD_NUMLOCK",
//    "KEYMOD_CAPSLOCK",
//    "KEYMOD_INALTSEQ",
//    "KEYMOD_ACCENT1",
//    "KEYMOD_ACCENT2",
//    "KEYMOD_ACCENT3",
//    "KEYMOD_ACCENT4"
//};
//
//int getKeymodCode(string& keymodName){
//    for(int i=0; i<ALLEGRO_KEY_UNKNOWN; i++) {
//        if(keymodnames[i] == keymodName) {
//            return (0x01 << i);
//        }
//    }
//    return KEYMOD_NONE;
//}

void action_noop(uint32_t keymod){
#ifdef DEBUG
    PrintMessage("unbound key pressed\n");
#endif
}

void action_invalid(uint32_t keymod){
    PrintMessage("invalid action\n");
}

action_name_mapper actionnamemap[] = {
    {"NOOP", action_noop},
    {"ROTATE", action_incrrotation},
    {"RELOAD_SEGMENT", action_reloadsegment},
    {"PAINT", action_paintboard},
    {"TOGGLE_DESIGNATIONS", action_toggledesignations},
    {"TOGGLE_STOCKS", action_togglestockpiles},
    {"TOGGLE_ZONES", action_togglezones},
    {"TOGGLE_OCCLUSION", action_toggleocclusion},
    {"TOGGLE_CREATURE_MOODS", action_togglecreaturemood},
    {"TOGGLE_CREATURE_PROFS", action_togglecreatureprof},
    {"TOGGLE_CREATURE_JOBS", action_togglecreaturejob},
    {"TOGGLE_CREATURE_NAMES", action_togglecreaturenames},
    {"CHOP_WALLS", action_chopwall},
    {"CYCLE_TRACKING_MODE", action_cycletrackingmode},
    {"RESET_VIEW_OFFSET", action_resetscreen},
    {"TOGGLE_SINGLE_LAYER", action_togglesinglelayer},
    {"TOGGLE_SHADE_HIDDEN_TILES", action_toggleshadehidden},
    {"TOGGLE_SHOW_HIDDEN_TILES", action_toggleshowhidden},
    {"TOGGLE_OSD", action_toggleosd},
    {"TOGGLE_KEYBINDS", action_togglekeybinds},
    {"INCR_ZOOM", action_incrzoom},
    {"DECR_ZOOM", action_decrzoom},
    {"SCREENSHOT", action_screenshot},
    {"INCR_RELOAD_TIME", action_incrreloadtime},
    {"DECR_RELOAD_TIME", action_decrreloadtime},
    {"CREDITS", action_credits},

    {"DECR_SEGMENT_X", action_decrsegmentX},
    {"INCR_SEGMENT_X", action_incrsegmentX},
    {"DECR_SEGMENT_Y", action_decrsegmentY},
    {"INCR_SEGMENT_Y", action_incrsegmentY},
    {"DECR_SEGMENT_Z", action_decrsegmentZ},
    {"INCR_SEGMENT_Z", action_incrsegmentZ},

    {"DECR_Z", action_decrZ},
    {"INCR_Z", action_incrZ},

    {"DECR_Y", action_decrY},
    {"INCR_Y", action_incrY},
    {"DECR_X", action_decrX},
    {"INCR_X", action_incrX},
    //add extra action here!

    {"INVALID", action_invalid}//this is the stop condition
};

void (*actionkeymap[ALLEGRO_KEY_UNKNOWN])(uint32_t);
bool actionrepeatmap[ALLEGRO_KEY_UNKNOWN];

void parseKeymapLine( string line )
{
    if(line.empty()) {
        return;
    }
    char c = line[0];
    if( c != '[') {
        return;
    }

    //some systems don't remove the \r char as a part of the line change:
    if(line.size() > 0 &&  line[line.size() -1 ] == '\r' ) {
        line.resize(line.size() -1);
    }

    c = line[ line.length() -1 ];
    if( c != ']' ) {
        return;
    }

    //second-last character should tell us if this is a repeating action
    c = line[ line.length() -2 ];

    for(int i=0; actionnamemap[i].func != action_invalid; i++) {
        if(line.find(actionnamemap[i].name)!=-1) {
            for(int j=0; j<ALLEGRO_KEY_UNKNOWN; j++){
                if(line.find(keynames[j])!=-1) {
                    actionkeymap[j] = actionnamemap[i].func;
                    if( c == '*' ) {
                        actionrepeatmap[j] = true;
#ifdef DEBUG
                    PrintMessage("successfully mapped: op:%i key:%i repeatable\n",i,j);
                    } else {
                    PrintMessage("successfully mapped: op:%i key:%i\n",i,j);
#endif
                    }
                    break;
                }
            }
            break;
        }
    }
}

bool loadKeymapFile(){
    string line;
    ALLEGRO_PATH * p =al_create_path("stonesense/keybinds.txt");
    const char * path = al_path_cstr(p,ALLEGRO_NATIVE_PATH_SEP);
    ifstream myfile (path);
    if (myfile.is_open() == false) {
        LogError( "cannot find keybinds file\n" );
        al_destroy_path(p);
        return false;
    }

    //initialize the keymap to all noops
    for(int i=0; i<ALLEGRO_KEY_UNKNOWN; i++) {
        actionkeymap[i] = action_noop;
        actionrepeatmap[i] = false;
    }

    while ( !myfile.eof() ) {
        getline (myfile,line);
        parseKeymapLine( line );
    }
    //close file, etc.
    myfile.close();
    al_destroy_path(p);
    return true;
}

bool isRepeatable(int32_t keycode)
{
    return actionrepeatmap[keycode];
}

bool doKey(int32_t keycode, uint32_t keymodcode)
{
    if(keycode>0 && keycode<ALLEGRO_KEY_UNKNOWN) {
        actionkeymap[keycode](keymodcode);
        return true;
    }
    return false;
}

bool getKeyStrings(int32_t keycode, string*& keyname, string*& actionname){
    keyname = actionname = NULL;
    if(keycode>0 && keycode<ALLEGRO_KEY_UNKNOWN) {
        keyname = &keynames[keycode];
        void* action = (void*) actionkeymap[keycode];
        if(action == (void*) action_noop){
            return false;
        }
        for(int i=0; actionnamemap[i].func != action_invalid; i++) {
            if(action == (void*) actionnamemap[i].func) {
                actionname = &(actionnamemap[i].name);
                return true;
            }
        }
    }
    return false;
}
