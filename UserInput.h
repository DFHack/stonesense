//declarations of actions taken for user keystrokes

void action_incrrotation(uint32_t keymod);
void action_reloadsegment(uint32_t keymod);
void action_paintboard(uint32_t keymod);
void action_toggledesignations(uint32_t keymod);
void action_togglestockpiles(uint32_t keymod);
void action_togglezones(uint32_t keymod);
void action_toggleocclusion(uint32_t keymod);
void action_togglecreaturemood(uint32_t keymod);
void action_togglecreatureprof(uint32_t keymod);
void action_togglecreaturejob(uint32_t keymod);
void action_chopwall(uint32_t keymod);
void action_cycletrackingmode(uint32_t keymod);
void action_resetscreen(uint32_t keymod);
void action_decrsegmentX(uint32_t keymod);
void action_incrsegmentX(uint32_t keymod);
void action_decrsegmentY(uint32_t keymod);
void action_incrsegmentY(uint32_t keymod);
void action_decrsegmentZ(uint32_t keymod);
void action_incrsegmentZ(uint32_t keymod);
void action_togglesinglelayer(uint32_t keymod);
void action_toggleshadehidden(uint32_t keymod);
void action_toggleshowhidden(uint32_t keymod);
void action_togglecreaturenames(uint32_t keymod);
void action_toggleosd(uint32_t keymod);
void action_togglekeybinds(uint32_t keymod);
void action_incrzoom(uint32_t keymod);
void action_decrzoom(uint32_t keymod);
void action_screenshot(uint32_t keymod);
void action_incrreloadtime(uint32_t keymod);
void action_decrreloadtime(uint32_t keymod);
void action_credits(uint32_t keymod);

void action_decrY(uint32_t keymod);
void action_incrY(uint32_t keymod);
void action_decrX(uint32_t keymod);
void action_incrX(uint32_t keymod);
void action_decrZ(uint32_t keymod);
void action_incrZ(uint32_t keymod);

extern bool isRepeatable(int32_t keycode);
extern bool doKey(int32_t keycode, uint32_t keymodcode);
