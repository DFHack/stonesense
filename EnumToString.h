#pragma once

#include "RemoteFortressReader.pb.h"

const char* TiletypeShapeToString(RemoteFortressReader::TiletypeShape input);
const char* TiletypeSpecialToString(RemoteFortressReader::TiletypeSpecial input);
const char* TiletypeMaterialToString(RemoteFortressReader::TiletypeMaterial input);
const char* TiletypeVariantToString(RemoteFortressReader::TiletypeVariant input);

RemoteFortressReader::TiletypeShape StringToTiletypeShape(const char* input);
RemoteFortressReader::TiletypeSpecial StringToTiletypeSpecial(const char* input);
RemoteFortressReader::TiletypeMaterial StringToTiletypeMaterial(const char* input);
RemoteFortressReader::TiletypeVariant StringToTiletypeVariant(const char* input);

