#include "catch2/catch.hpp"
#include "Core/beStringUtil.h"


TEST_CASE("StringFindFirst", "[string]")
{
	SECTION("Find char in string")
	{
		REQUIRE(beStringUtil::FindFirst("", 'M') == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", ' ') == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", 'M') == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", 'm') == -1);
		REQUIRE(beStringUtil::FindFirst("MMMMMMMMMM", 'S') == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", 'S') == 2);
		REQUIRE(beStringUtil::FindFirst("SSSSSSSSSS", 'S') == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", 'r') == 4);
	}

	SECTION("Find substring in string")
	{
		REQUIRE(beStringUtil::FindFirst("", "M") == -1);
		REQUIRE(beStringUtil::FindFirst("", "") == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "") == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", " ") == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "MyStr") == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "MyStrMyStr") == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "MyString") == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "M") == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "m") == -1);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "My") == 0);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "S") == 2);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "St") == 2);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "Str") == 2);
		REQUIRE(beStringUtil::FindFirst("MyStrMyStr", "r") == 4);
	}
}

TEST_CASE("StringFindLast", "[string]")
{
	SECTION("Find char in string")
	{
		REQUIRE(beStringUtil::FindLast("", 'M') == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", ' ') == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", 'M') == 5);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", 'm') == -1);
		REQUIRE(beStringUtil::FindLast("MMMMMMMMMM", 'S') == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", 'S') == 7);
		REQUIRE(beStringUtil::FindLast("SSSSSSSSSS", 'S') == 9);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", 'r') == 9);
	}

	SECTION("Find substring in string")
	{
		REQUIRE(beStringUtil::FindLast("", "M") == -1);
		REQUIRE(beStringUtil::FindLast("", "") == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "") == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", " ") == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "MyStr") == 5);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "MyStrMyStr") == 0);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "MyString") == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "M") == 5);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "m") == -1);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "My") == 5);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "S") == 7);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "St") == 7);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "Str") == 7);
		REQUIRE(beStringUtil::FindLast("MyStrMyStr", "r") == 9);
	}
}

TEST_CASE("StringBeginsWith", "[string]")
{
	SECTION("Case Sensitive")
	{
		REQUIRE(!beStringUtil::BeginsWith("", "M"));
		REQUIRE(beStringUtil::BeginsWith("", ""));
		REQUIRE(beStringUtil::BeginsWith("MyStrMyStr", ""));
		REQUIRE(beStringUtil::BeginsWith("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "y"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "my"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "r"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", " "));
		REQUIRE(beStringUtil::BeginsWith("MyStrMyStr", "M"));
		REQUIRE(beStringUtil::BeginsWith("MyStrMyStr", "My"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "MY"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "Y"));
		REQUIRE(!beStringUtil::BeginsWith("MyStrMyStr", "R"));
	}

	SECTION("Case Insensitive")
	{
		REQUIRE(!beStringUtil::BeginsWithI("", "M"));
		REQUIRE(beStringUtil::BeginsWithI("", ""));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", ""));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::BeginsWithI("MyStrMyStr", "y"));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "my"));
		REQUIRE(!beStringUtil::BeginsWithI("MyStrMyStr", "r"));
		REQUIRE(!beStringUtil::BeginsWithI("MyStrMyStr", " "));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "M"));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "My"));
		REQUIRE(beStringUtil::BeginsWithI("MyStrMyStr", "MY"));
		REQUIRE(!beStringUtil::BeginsWithI("MyStrMyStr", "Y"));
		REQUIRE(!beStringUtil::BeginsWithI("MyStrMyStr", "R"));
	}
}

TEST_CASE("StringEndsWith", "[string]")
{
	SECTION("Case Sensitive")
	{
		REQUIRE(!beStringUtil::EndsWith("", "M"));
		REQUIRE(beStringUtil::EndsWith("", ""));
		REQUIRE(beStringUtil::EndsWith("MyStrMyStr", ""));
		REQUIRE(beStringUtil::EndsWith("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "y"));
		REQUIRE(beStringUtil::EndsWith("MyStrMyStr", "r"));
		REQUIRE(beStringUtil::EndsWith("MyStrMyStr", "Str"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "STr"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", " "));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "M"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "Y"));
		REQUIRE(!beStringUtil::EndsWith("MyStrMyStr", "R"));
	}

	SECTION("Case Insensitive")
	{
		REQUIRE(!beStringUtil::EndsWithI("", "M"));
		REQUIRE(beStringUtil::EndsWithI("", ""));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", ""));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(!beStringUtil::EndsWithI("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::EndsWithI("MyStrMyStr", "y"));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "r"));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "Str"));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "STr"));
		REQUIRE(!beStringUtil::EndsWithI("MyStrMyStr", " "));
		REQUIRE(!beStringUtil::EndsWithI("MyStrMyStr", "M"));
		REQUIRE(!beStringUtil::EndsWithI("MyStrMyStr", "Y"));
		REQUIRE(beStringUtil::EndsWithI("MyStrMyStr", "R"));
	}
}


TEST_CASE("StringIsEqual", "[string]")
{
	SECTION("Case Sensitive")
	{
		REQUIRE(!beStringUtil::IsEqual("", "M"));
		REQUIRE(beStringUtil::IsEqual("", ""));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", ""));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", "M"));
		REQUIRE(beStringUtil::IsEqual("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", "Str"));
		REQUIRE(!beStringUtil::IsEqual("MyStrMyStr", "STR"));
	}

	SECTION("Case Insensitive")
	{
		REQUIRE(!beStringUtil::IsEqualI("", "M"));
		REQUIRE(beStringUtil::IsEqualI("", ""));
		REQUIRE(!beStringUtil::IsEqualI("MyStrMyStr", ""));
		REQUIRE(!beStringUtil::IsEqualI("MyStrMyStr", "m"));
		REQUIRE(!beStringUtil::IsEqualI("MyStrMyStr", "M"));
		REQUIRE(beStringUtil::IsEqualI("MyStrMyStr", "MyStrMyStr"));
		REQUIRE(beStringUtil::IsEqualI("MyStrMyStr", "MYSTRMYSTR"));
		REQUIRE(!beStringUtil::IsEqualI("MyStrMyStr", "Str"));
		REQUIRE(!beStringUtil::IsEqualI("MyStrMyStr", "STR"));
	}
}
