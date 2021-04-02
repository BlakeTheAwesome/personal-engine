export module beNonCopiable;

export class NonCopiable
{
	public:
		NonCopiable() = default;
	protected:
		NonCopiable(const NonCopiable& that) = default;
		NonCopiable& operator=(const NonCopiable& that) = default;
		NonCopiable& operator=(NonCopiable&& that) = default;
};

export class NoCreate
{
	protected:
		NoCreate() = default;
		NoCreate(const NoCreate& that) = default;
		NoCreate& operator=(const NoCreate& that) = default;
		NoCreate& operator=(NoCreate&& that) = default;
};