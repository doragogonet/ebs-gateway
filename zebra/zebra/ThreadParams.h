#pragma once
#include "zebra.h"

struct ThreadParams {
    TagProcessingCallback callback;
    uint32_t timeout_ms;
    char id[MAX_PATH + 1];
    zebraReader* pReader;
};
