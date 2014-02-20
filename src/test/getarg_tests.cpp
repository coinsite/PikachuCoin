#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-PKC");
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", false));
    BOOST_CHECK(GetBoolArg("-PKC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-PKCo"));
    BOOST_CHECK(!GetBoolArg("-PKCo", false));
    BOOST_CHECK(GetBoolArg("-PKCo", true));

    ResetArgs("-PKC=0");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", false));
    BOOST_CHECK(!GetBoolArg("-PKC", true));

    ResetArgs("-PKC=1");
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", false));
    BOOST_CHECK(GetBoolArg("-PKC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noPKC");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", false));
    BOOST_CHECK(!GetBoolArg("-PKC", true));

    ResetArgs("-noPKC=1");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", false));
    BOOST_CHECK(!GetBoolArg("-PKC", true));

    ResetArgs("-PKC -noPKC");  // -PKC should win
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", false));
    BOOST_CHECK(GetBoolArg("-PKC", true));

    ResetArgs("-PKC=1 -noPKC=1");  // -PKC should win
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", false));
    BOOST_CHECK(GetBoolArg("-PKC", true));

    ResetArgs("-PKC=0 -noPKC=0");  // -PKC should win
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", false));
    BOOST_CHECK(!GetBoolArg("-PKC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PKC=1");
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", false));
    BOOST_CHECK(GetBoolArg("-PKC", true));

    ResetArgs("--noPKC=1");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", false));
    BOOST_CHECK(!GetBoolArg("-PKC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PKC", "eleven"), "eleven");

    ResetArgs("-PKC -bar");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PKC", "eleven"), "");

    ResetArgs("-PKC=");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PKC", "eleven"), "");

    ResetArgs("-PKC=11");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PKC", "eleven"), "11");

    ResetArgs("-PKC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PKC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PKC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PKC", 0), 0);

    ResetArgs("-PKC -bar");
    BOOST_CHECK_EQUAL(GetArg("-PKC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-PKC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-PKC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-PKC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PKC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PKC");
    BOOST_CHECK_EQUAL(GetBoolArg("-PKC"), true);

    ResetArgs("--PKC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-PKC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noPKC");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", true));
    BOOST_CHECK(!GetBoolArg("-PKC", false));

    ResetArgs("-noPKC=1");
    BOOST_CHECK(!GetBoolArg("-PKC"));
    BOOST_CHECK(!GetBoolArg("-PKC", true));
    BOOST_CHECK(!GetBoolArg("-PKC", false));

    ResetArgs("-noPKC=0");
    BOOST_CHECK(GetBoolArg("-PKC"));
    BOOST_CHECK(GetBoolArg("-PKC", true));
    BOOST_CHECK(GetBoolArg("-PKC", false));

    ResetArgs("-PKC --noPKC");
    BOOST_CHECK(GetBoolArg("-PKC"));

    ResetArgs("-noPKC -PKC"); // PKC always wins:
    BOOST_CHECK(GetBoolArg("-PKC"));
}

BOOST_AUTO_TEST_SUITE_END()
