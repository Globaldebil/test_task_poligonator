#pragma once

#include "CoreMinimal.h"
#include "Map.h"

class FOriginalMap
{
public:
	static FLayout CreateLayout();

private:
	static FCell ParseCell(TCHAR Symbol);
};
