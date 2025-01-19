#pragma once

void reloadPosition();

void read_segment( void *arg);

struct SS_Item;
SS_Item ConvertItem(df::item * found_item, WorldSegment& segment);

std::optional<DFHack::t_matglossPair> getDyeMaterialFromItem(df::item* item);
