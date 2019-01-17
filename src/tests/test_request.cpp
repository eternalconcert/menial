#if TEST==1
#include "catch.h"
#include "../request.h"
#include "../config.h"
#include "../logger.h"

static Config *config = Config::getConfig("testdata/unittests/menial.json");
static Logger *logger = Logger::getLogger(config);


TEST_CASE ("Testing authenticate open host", "[authenticate]") {
    std::string headers = "GET / HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);

    REQUIRE(request->getVirtualHost() == "open:80");
    REQUIRE(request->authenticate() == true);
}

TEST_CASE ("Testing authenticate closed host", "[authenticate]") {
    std::string headers = "GET / HTTP/1.1\nHost: closed \nUser-Agent: Test Request Browser\nAuthorization: Basic aGVsbG86d29ybGQ=";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);

    REQUIRE(request->getVirtualHost() == "closed:80");
    REQUIRE(request->authenticate() == true);
}

#endif
