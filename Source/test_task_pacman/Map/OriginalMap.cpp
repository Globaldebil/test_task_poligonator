#include "OriginalMap.h"

FLayout FOriginalMap::CreateLayout()
{
	static const TArray<FString> Rows =
	{
		TEXT("############################"),
		TEXT("#............##............#"),
		TEXT("#.####.#####.##.#####.####.#"),
		TEXT("#o####.#####.##.#####.####o#"),
		TEXT("#.####.#####.##.#####.####.#"),
		TEXT("#..........................#"),
		TEXT("#.####.##.########.##.####.#"),
		TEXT("#.####.##.########.##.####.#"),
		TEXT("#......##....##....##......#"),
		TEXT("######.##### ## #####.######"),
		TEXT("######.##### ## #####.######"),
		TEXT("######.##          ##.######"),
		TEXT("######.## ###--### ##.######"),
		TEXT("######.## #______# ##.######"),
		TEXT("      .   #______#   .      "),
		TEXT("######.## #______# ##.######"),
		TEXT("######.## ######## ##.######"),
		TEXT("######.##          ##.######"),
		TEXT("######.## ######## ##.######"),
		TEXT("######.## ######## ##.######"),
		TEXT("#............##............#"),
		TEXT("#.####.#####.##.#####.####.#"),
		TEXT("#.####.#####.##.#####.####.#"),
		TEXT("#o..##.......  .......##..o#"),
		TEXT("###.##.##.########.##.##.###"),
		TEXT("###.##.##.########.##.##.###"),
		TEXT("#......##....##....##......#"),
		TEXT("#.##########.##.##########.#"),
		TEXT("#.##########.##.##########.#"),
		TEXT("#..........................#"),
		TEXT("############################"),
	};

	FLayout Layout;
	Layout.Width = Rows[0].Len();
	Layout.Height = Rows.Num();

	for (int32 Y = 0; Y < Layout.Height; Y++)
	{
		for (int32 X = 0; X < Layout.Width; X++)
		{
			Layout.Cells.Add(ParseCell(Rows[Y][X]));
		}
	}

	Layout.PacmanStart = FIntPoint(13, 23);
	Layout.DiicstraStart = FIntPoint(13, 11);
	Layout.BfsStart = FIntPoint(12, 14);
	Layout.DfsStart = FIntPoint(15, 14);
	Layout.BellmanFordStart = FIntPoint(14, 14);
	Layout.InvisibleWallNode = FIntPoint(13, 12);
	Layout.HouseCenter = FIntPoint(13, 14);

	return Layout;
}

FCell FOriginalMap::ParseCell(TCHAR Symbol)
{
	FCell Cell;

	switch (Symbol)
	{
	case '.':
		Cell.Type = ECellType::EmptyCell;
		Cell.Pickup = ECellObjects::Score;
		break;
	case 'o':
		Cell.Type = ECellType::EmptyCell;
		Cell.Pickup = ECellObjects::Pill;
		break;
	case ' ':
		Cell.Type = ECellType::EmptyCell;
		break;
	case '-':
		Cell.Type = ECellType::Invisible_Wall;
		break;
	case '_':
		Cell.Type = ECellType::Respawn;
		break;
	default:
		Cell.Type = ECellType::Wall;
		break;
	}

	return Cell;
}
