#if TEST==1
#include "catch.h"
#include "utils.h"
#include "../request.h"


TEST_CASE("Test Etag", "[Cache]") {
    std::string headers = "GET / HTTP/1.1\nHost: open \nUser-Agent: Test Request Browser";
    Request *request = new Request(headers, "127.0.0.2", false, config, logger);
    REQUIRE_THAT(request->getResponse(), Catch::Contains("ETag: "));
}

#endif
