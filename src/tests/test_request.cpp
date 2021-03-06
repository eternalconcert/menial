#if TEST==1
#include "catch.h"
#include "utils.h"
#include "../request/request.h"


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

TEST_CASE("Testing 404", "[404]") {
    std::string headers = "GET /some/not/existing/page/ HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE(request->getVirtualHost() == "open:80");
    REQUIRE_THAT(request->getResponse(), Catch::Contains("404 Not Found"));
    REQUIRE_THAT(request->getResponse(), Catch::Contains("404: Not found"));
}

TEST_CASE("Testing standard request", "[200]") {
    std::string headers = "GET / HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE(request->getVirtualHost() == "open:80");
    REQUIRE_THAT(request->getResponse(), Catch::Contains("200 OK"));
    REQUIRE_THAT(request->getResponse(), Catch::Contains("If you see this page, the menial webserver is up and running."));
}

TEST_CASE("Testing standard request with GET params, content", "[200]") {
    std::string headers = "GET /?key1=value1&key2=value2 HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE(request->getVirtualHost() == "open:80");
    REQUIRE_THAT(request->getResponse(), Catch::Contains("200 OK"));
    REQUIRE_THAT(request->getResponse(), Catch::Contains("If you see this page, the menial webserver is up and running."));
}

TEST_CASE("Testing standard request with GET params, params", "[200]") {
    std::string headers = "GET /?key1=value1&key2=value2 HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE(request->getGetParams() == "?key1=value1&key2=value2");
}

TEST_CASE("Testing standard request no GET params", "[200]") {
    std::string headers = "GET / HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE(request->getGetParams() == "");
}

#endif
