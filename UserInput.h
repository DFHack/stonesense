//declarations of actions taken for user keystrokes

void action_incrrotation(int keymod);
void action_reloadsegment(int keymod);
void action_paintboard(int keymod);
void action_togglestockpiles(int keymod);
void action_togglezones(int keymod);
void action_toggleocclusion(int keymod);
void action_togglecreaturemood(int keymod);
void action_togglecreatureprof(int keymod);
void action_togglecreaturejob(int keymod);
void action_chopwall(int keymod);
void action_togglefollowdfcursor(int keymod);
void action_togglefollowdfscreen(int keymod);
void action_decrsegmentZ(int keymod);
void action_incrsegmentZ(int keymod);
void action_togglesinglelayer(int keymod);
void action_toggleshadehidden(int keymod);
void action_toggleshowhidden(int keymod);
void action_togglecreaturenames(int keymod);
void action_toggleosd(int keymod);
void action_incrzoom(int keymod);
void action_decrzoom(int keymod);
void action_screenshot(int keymod);
void action_incrreloadtime(int keymod);
void action_decrreloadtime(int keymod);
void action_credits(int keymod);

extern void doKey(int32_t keycode, uint32_t keymodcode);