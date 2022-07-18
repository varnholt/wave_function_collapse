#include "drawabletile.h"

DrawableTile::DrawableTile(int32_t tile_index, const std::array<std::set<int32_t>, 4>& compatible_tiles)
 : Tile(tile_index, compatible_tiles)
{
}
