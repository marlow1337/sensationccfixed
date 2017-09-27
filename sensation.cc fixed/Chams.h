/*
Rest In Peace ApocalypseCheats
*/

#pragma once
#include "SDK.h"

void InitKeyValues(KeyValues* keyValues, char* name);

void LoadFromBuffer(KeyValues* keyValues, char const *resourceName, const char *pBuffer);

void ForceMaterial(IMaterial* material, Color color, bool useColor = true, bool forceMaterial = true);

IMaterial *CreateMaterial(bool shouldIgnoreZ, bool isLit = true, bool isWireframe = false);