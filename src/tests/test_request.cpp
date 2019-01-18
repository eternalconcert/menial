#if TEST==1
#include "catch.h"
#include "utils.h"
#include "../request.h"


TEST_CASE("Testing authenticate open host", "[authenticate]") {
    std::string headers = "GET / HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);

    REQUIRE(request->getVirtualHost() == "open:80");
    REQUIRE(request->authenticate() == true);
    REQUIRE_THAT(request->getResponse(), Catch::Contains("200 OK"));
}


TEST_CASE("Testing authenticate closed host, authenticated", "[authenticate]") {
    std::string headers = "GET / HTTP/1.1\nHost: closed \nUser-Agent: Test Request Browser\nAuthorization: Basic aGVsbG86d29ybGQ=";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);

    REQUIRE(request->getVirtualHost() == "closed:80");
    REQUIRE(request->authenticate() == true);
    REQUIRE_THAT(request->getResponse(), Catch::Contains("200 OK"));
}


TEST_CASE("Testing authenticate closed host, no credentials provided", "[authenticate]") {
    std::string headers = "GET / HTTP/1.1\nHost: closed \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);

    REQUIRE(request->getVirtualHost() == "closed:80");
    REQUIRE(request->authenticate() == false);
    REQUIRE_THAT(request->getResponse(), Catch::Contains("401 Unauthorized"));
}

#endif
