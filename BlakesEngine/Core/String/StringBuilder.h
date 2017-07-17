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
			StringElement(char *cp) : m_value(cp) {};
			StringElement(double d) : m_value(ToString<double>(d)) {};
			StringElement(float f) : m_value(ToString<float>(f)) {};
			StringElement(long l) : m_value(ToString<long>(l)) {};
			StringElement(int i) : m_value(ToString<int>(i)) {};
			StringElement(char c) : m_value(ToString<char>(c)) {};
			StringElement(short s) : m_value(ToString<short>(s)) {};
			StringElement(bool b) : m_value(ToString<bool>(b)) {};

			const std::string& ToString() const { return m_value; };

		private:
			template<typename T>
			std::string ToString(T &val)
			{
				std::stringstream ss;
				ss << val;
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

		private:
			std::string m_value;
	};
}