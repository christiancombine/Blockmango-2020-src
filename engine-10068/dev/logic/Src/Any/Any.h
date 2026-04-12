#pragma once

#ifndef __ANY_HEADER__
#define __ANY_HEADER__

#include <algorithm>
#include <typeinfo>
#include "Core.h"

using namespace LORD;

namespace BLOCKMAN
{
	class bad_any_cast : public std::bad_cast
	{
	public:
		virtual const char* what() const noexcept
		{
			return "bad_any_cast: failed conversion using any_cast";
		}
	};

	class Any final
	{
#pragma region //Holder
	private:
		class PlaceHolder : public ObjectAlloc
		{
		public:
			virtual ~PlaceHolder() = default;
			virtual PlaceHolder* clone() = 0;
			virtual const std::type_info& type() const noexcept = 0;
		};

		template<typename ValueType>
		class Holder final : public PlaceHolder
		{
		public:
			Holder(const ValueType& value) : m_held(value) {}
			Holder(ValueType&& value) : m_held(std::move(value)) {}
			Holder* clone() override { return LordNew Holder{ m_held }; }
			const std::type_info& type() const noexcept override { return typeid(ValueType); }
		public:
			ValueType m_held;
		};
#pragma endregion

	public:
#pragma region //constructor
		Any() = default;

		template<typename ValueType>
		Any(const ValueType& value) : m_content(LordNew Holder<std::remove_cv_t<ValueType>>{value}) {}
		template<typename ValueType>
		Any(ValueType&& value) : m_content(LordNew Holder<std::remove_cv_t<ValueType>>{std::move(value)}) {}
#pragma endregion

#pragma region
		Any(const Any& other) : m_content(other.m_content ? other.m_content->clone() : nullptr) {}

		// Move constructor
		Any(Any&& other) noexcept
			: m_content(other.m_content)
		{
			other.m_content = 0;
		}

		Any& operator=(const Any& rhs)
		{
			Any(rhs).swap(*this);
			return *this;
		}

		// move assignment
		Any& operator=(Any&& rhs) noexcept
		{
			rhs.swap(*this);
			Any().swap(rhs);
			return *this;
		}

		// Perfect forwarding of ValueType
		template <class ValueType>
		Any& operator=(ValueType&& rhs)
		{
			Any(static_cast<ValueType&&>(rhs)).swap(*this);
			return *this;
		}
#pragma endregion

		~Any()
		{
			LordSafeDelete(m_content);
		}

	public:
		bool empty() const noexcept
		{
			return !m_content;
		}

		const std::type_info& type() const
		{
			return m_content->type();
		}

		Any& swap(Any & rhs) noexcept
		{
			std::swap(m_content, rhs.m_content);
			return *this;
		}

	private:
		template<typename ValueType>
		friend ValueType any_cast(Any& operand);

		PlaceHolder* m_content{nullptr};
	};

#pragma region
	template<typename ValueType>
	static void castCheck(const Any& operand)
	{
		if (operand.empty())
		{
			throw bad_any_cast();
		}

		if (operand.type() != typeid(ValueType))
		{
			throw bad_any_cast();
		}
	}

	template<typename ValueType>
	ValueType any_cast(Any& operand)
	{
		using HoldedType = std::remove_cv_t<std::remove_reference_t<ValueType>>;
		castCheck<HoldedType>(operand);
		return static_cast<const Any::Holder<HoldedType>*>(operand.m_content)->m_held;
	}

	template<typename ValueType>
	ValueType any_cast(const Any& operand)
	{
		return any_cast<ValueType>(const_cast<Any&>(operand));
	}

	template<typename ValueType>
	ValueType any_cast(Any&& operand)
	{
		return any_cast<ValueType>(operand);
	}
#pragma endregion
}

#endif
