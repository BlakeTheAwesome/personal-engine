#pragma once
#include "beMath.h"
#include"BlakesEngine/External/fmt/format.h"

namespace fmt
{
	template <>
	struct formatter<beMath::Vec2>
	{
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(beMath::Vec2 const& vec, FormatContext &ctx)
		{
			return format_to(ctx.begin(), "{{{:.3f}, {:.3f}}}", vec.x, vec.y);
		}
	};

	template <>
	struct formatter<beMath::Vec3>
	{
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(beMath::Vec3 const& vec, FormatContext &ctx)
		{
			return format_to(ctx.begin(), "{{{:.3f}, {:.3f}, {:.3f}}}", vec.x, vec.y, vec.z);
		}
	};

	template <>
	struct formatter<beMath::Vec4>
	{
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(beMath::Vec4 const& vec, FormatContext &ctx)
		{
			return format_to(ctx.begin(), "{{{:.3f}, {:.3f}, {:.3f}, {:.3f}}}", vec.x, vec.y, vec.z, vec.w);
		}
	};
}

