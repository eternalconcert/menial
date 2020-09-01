#if TEST==1
#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "../config/config.h"
#include "../logger/logger.h"

static Config *config = Config::getConfig("src/tests/testdata/menial.json");
static Logger *logger = Logger::getLogger(config);

#endif
