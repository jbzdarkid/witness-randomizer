#pragma once

#define MAJOR 3
#define MINOR 0
#define PATCH 1

#define VERSION			MAJOR, MINOR, PATCH, 0

#define TO_STRING(s) #s
#define VERSION_STR     TO_STRING(MAJOR) "." TO_STRING(MINOR) "." TO_STRING(PATCH) ".0"

#define PRODUCT_NAME "Witness Randomizer"
