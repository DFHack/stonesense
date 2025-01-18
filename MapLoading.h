#pragma once

#include "VersionInfo.h"

#ifdef LINUX_BUILD
#define SUSPEND_DF DF.Suspend()
#define  RESUME_DF DF.Resume()
#else
#define SUSPEND_DF ;
#define  RESUME_DF ;
#endif

void reloadPosition();

void read_segment( void *arg);

extern const DFHack::VersionInfo *dfMemoryInfo;

struct SS_Item;
SS_Item ConvertItem(df::item * found_item, WorldSegment& segment);
