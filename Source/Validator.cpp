#include "Validator.h"
#include "Puzzle.h"

void Validator::Validate(Puzzle& p) {
    // console.log('Validating', puzzle);
    p.valid = true; // Assume valid until we find an invalid element
    p.invalidElements.clear();
    p.negations.clear();

    bool puzzleHasSymbols = false;
    bool puzzleHasStart = false;
    bool puzzleHasEnd = false;
  // Validate gap failures as an early exit.
    for (int x = 0; x < p.width; x++) {
        for (int y = 0; y < p.height; y++) {
            Cell cell = p.grid[x][y];
            auto decoration = cell.decoration;
            if (decoration) {
                if (decoration->type == Type::Stone ||
                    decoration->type == Type::Star ||
                    decoration->type == Type::Nega ||
                    decoration->type == Type::Poly ||
                    decoration->type == Type::Ylop) {
                    puzzleHasSymbols = true;
                    continue;
                }
                if (decoration->type == Type::Triangle) {
                    int actualCount = 0;
                    if (p.GetLine(x - 1, y) != Cell::Color::NONE) actualCount++;
                    if (p.GetLine(x + 1, y) != Cell::Color::NONE) actualCount++;
                    if (p.GetLine(x, y - 1) != Cell::Color::NONE) actualCount++;
                    if (p.GetLine(x, y + 1) != Cell::Color::NONE) actualCount++;
                    if (decoration->count != actualCount) {
                        // console.log('Triangle at grid['+x+']['+y+'] has', actualCount, 'borders')
                        p.invalidElements.emplace_back(Pos{x, y});
                    }
                }
            }
            if (cell.gap != Cell::Gap::NONE && cell.color != Cell::Color::NONE) {
                // console.log('Gap at', x, y, 'is covered')
                p.valid = false;
            }
            if (cell.dot != Cell::Dot::NONE) {
                if (cell.color == Cell::Color::NONE) {
                    // console.log('Dot at', x, y, 'is not covered')
                    p.invalidElements.emplace_back(Pos{x, y});
                } else if (cell.color == Cell::Color::BLUE && cell.dot == Cell::Dot::YELLOW) {
                    // console.log('Yellow dot at', x, y, 'is covered by blue line')
                    p.valid = false;
                } else if (cell.color == Cell::Color::YELLOW && cell.dot == Cell::Dot::BLUE) {
                    // console.log('Blue dot at', x, y, 'is covered by yellow line')
                    p.valid = false;
                }
            }
            if (cell.color != Cell::Color::NONE) {
                if (cell.start == true) puzzleHasStart = true;
                if (cell.end != Cell::Dir::NONE) puzzleHasEnd = true;
            }
        }
    }
    if (!puzzleHasStart || !puzzleHasEnd) {
        // console.log('There is no covered start or endpoint')
        p.valid = false;
    }

    // Perf optimization: We can skip computing regions if the grid has no symbols.
    if (!puzzleHasSymbols) { // No additional symbols, and we already checked dots & gaps
        p.valid &= (p.invalidElements.size() == 0);
    } else { // Additional symbols, so we need to discard dots & divide them by region
        /*
        p.invalidElements.clear();
        std::vector<Region> regions = p.GetRegions();
        // console.log('Found', regions.length, 'regions');
        // console.debug(regions);

        for (const Region& region : regions) {
          std::string key = region.grid.ToString();
          auto regionData = puzzle.regionCache[key];
          if (regionData == undefined) {
            console.log('Cache miss for region', region, 'key', key);
            regionData = _regionCheckNegations(puzzle, region);
            // Entirely for convenience
            regionData.valid = (regionData.invalidElements.size() == 0)
            // console.log('Region valid:', regionData.valid);

            if (!DISABLE_CACHE) {
              p.regionCache[key] = regionData;
            }
          }
          p.negations = p.negations.concat(regionData.negations);
          p.invalidElements = p.invalidElements.concat(regionData.invalidElements);
          p.valid = p.valid && regionData.valid;
        }
        */
    }
    // console.log('Puzzle has', puzzle.invalidElements.length, 'invalid elements')
}
