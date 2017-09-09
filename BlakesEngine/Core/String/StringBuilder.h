#pragma once
// Adapted from https://www.codeproject.com/Articles/20491/A-C-StringBuilder-Class
#include <string>
#include <sstream>

namespace ExternalStringBuilder
{
	class StringElement
	{
		public:
			StringElement(const std::string& s) : m_value(s) {};
			StringElement(const char* cp) : m_value(cp) {};
			StringElement(double d) : m_value(ToString(d)) {};
			StringElement(float f) : m_value(ToString(f)) {};
			StringElement(long l) : m_value(ToString(l)) {};
			StringElement(int i) : m_value(ToString(i)) {};
			StringElement(char c) : m_value(ToString(c)) {};
			StringElement(short s) : m_value(ToString(s)) {};
			StringElement(bool b) : m_value(ToString(b)) {};

			const std::string& ToString() const { return m_value; };

		private:
			template<typename T>
			std::string ToString(T&& val)
			{
				std::stringstream ss;
				ss << std::forward<T>(val);
				return ss.str();
			};

			std::string m_value;
	};


	class StringBuilder
	{
		public:
			void Clear() { m_value.clear(); };
			void Append(StringElement element) { m_value.append(element.ToString()); };
			const std::string& ToString() const { return m_value; };
			const char* c_str() const { return m_value.c_str(); };
	
			StringBuilder &operator<<(StringElement se) {
				Append(se);
				return *this;
			};

			StringBuilder &operator<<(std::string& str)
			{
				m_value.append(str);
				return *this;
			};

			StringBuilder &operator<<(const char* str)
			{
				m_value.append(str);
				return *this;
			};

		private:
			std::string m_value;
	};
}