#pragma once

class NonCopiable
{
	public:
		NonCopiable() = default;
	protected:
		NonCopiable(const NonCopiable& that) = default;
		NonCopiable& operator=(const NonCopiable& that){return *this;}
};

class NoCreate
{
	protected:
		NoCreate() = default;
		NoCreate(const NoCreate& that) = default;
		NoCreate& operator=(const NoCreate& that){return *this;}
};