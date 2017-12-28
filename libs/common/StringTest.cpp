#define BOOST_TEST_DYN_LINK

#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <common/String.hpp>

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(string)

BOOST_AUTO_TEST_CASE(SplitWordTest)
{
    {
        std::vector<std::string> tokens;
        komb::splitSpaces("f 1// 2// 3//\r\n", tokens);
        BOOST_CHECK_EQUAL(tokens.size(), 4u);
        BOOST_CHECK_EQUAL(tokens[0], "f");
        BOOST_CHECK_EQUAL(tokens[1], "1//");
        BOOST_CHECK_EQUAL(tokens[2], "2//");
        BOOST_CHECK_EQUAL(tokens[3], "3//");
    }

    {
        std::vector<std::string> tokens;
        komb::splitSlashes("1//", tokens);
        BOOST_CHECK_EQUAL(tokens.size(), 3u);
        BOOST_CHECK_EQUAL(tokens[0], "1");
        BOOST_CHECK_EQUAL(tokens[1], "");
        BOOST_CHECK_EQUAL(tokens[2], "");
    }
}

BOOST_AUTO_TEST_CASE(SnakeCasedTest)
{
    BOOST_CHECK_EQUAL(komb::snakeCased("Hello World"), "hello_world");
    BOOST_CHECK_EQUAL(komb::snakeCased("Hello world"), "hello_world");
    BOOST_CHECK_EQUAL(komb::snakeCased("HelloWorld"), "hello_world");
    BOOST_CHECK_EQUAL(komb::snakeCased("Hello world "), "hello_world_");
    BOOST_CHECK_EQUAL(komb::snakeCased("Hello  world"), "hello__world");
    BOOST_CHECK_EQUAL(komb::snakeCased(""), "");
    BOOST_CHECK_EQUAL(komb::snakeCased(" "), "_");
    BOOST_CHECK_THROW(komb::snakeCased("Åter till övers"), std::domain_error);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
