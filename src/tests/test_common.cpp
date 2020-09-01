#if TEST==1
#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "../common/common.h"
#include "../exceptions/exceptions.h"


TEST_CASE ("Testing readFile", "[readFile]") {
    REQUIRE(readFile("/dev/null") == "");
    REQUIRE(readFile("src/tests/testdata/test.txt") == "Hello world!\n");
    REQUIRE_THROWS_AS(readFile("/not/an/existing/file.txt"), FileNotFoundException);
}

TEST_CASE ("Testing logLevelByName", "[logLevelByName]") {
    REQUIRE(logLevelByName("debug") == DEBUG);
    REQUIRE(logLevelByName("info") == INFO);
    REQUIRE(logLevelByName("warning") == WARNING);
    REQUIRE(logLevelByName("error") == ERROR);
    REQUIRE(logLevelByName("critical") == CRITICAL);
    REQUIRE(logLevelByName("something") == NOTSET);
    REQUIRE(logLevelByName("") == NOTSET);
}


TEST_CASE ("Testing base64decode", "[base64encode]") {
    REQUIRE(base64decode("") == "");
    REQUIRE(base64decode("SGVsbG8gd29ybGQh") == "Hello world!");
    REQUIRE(base64decode("w5/DhMOkw5bDtsOcw7w=") == "ßÄäÖöÜü");
    REQUIRE(base64decode("MA==") == "0");
    REQUIRE(base64decode("MQ==") == "1");
}


TEST_CASE ("Testing sha256hash", "[sha256hash]") {
    REQUIRE(sha256hash("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    REQUIRE(sha256hash("Hello world!") == "c0535e4be2b79ffd93291305436bf889314e4a3faec05ecffcbb7df31ad9e51a");
    REQUIRE(sha256hash("ßÄäÖöÜü") == "6948527b88c4cf2fbcc96a4c677c79cea9b88091154bcecf99c4694fada14b96");
    REQUIRE(sha256hash("0") == "5feceb66ffc86f38d952786c6d696c79c2dbc239dd4e91b46729d73a27fb57e9");
    REQUIRE(sha256hash("1") == "6b86b273ff34fce19d6b804eff5a3f5747ada4eaa22f1d49c01e52ddb7875b4b");
}

#endif
