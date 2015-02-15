#pragma once

#ifdef LINUX_BUILD
#define SUSPEND_DF DF.Suspend()
#define  RESUME_DF DF.Resume()
#else
#define SUSPEND_DF ;
#define  RESUME_DF ;
#endif

void reloadPosition();

void DisconnectFromDF();
bool IsConnectedToDF();

void read_segment( void *arg);

extern const VersionInfo *dfMemoryInfo;

struct SS_Item;
SS_Item ConvertItem(df::item * found_item, WorldSegment& segment);