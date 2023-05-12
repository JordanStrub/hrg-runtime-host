#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <assert.h>
#include <locale.h>
namespace aristocrat
{
	////////////////////////////////////////////////////////////////////////////////////
	// String utilities used internally in runtime host
	////////////////////////////////////////////////////////////////////////////////////
	namespace sutils
	{
		////////////////////////////////////////////////////////////////////////////////////
		/// Conversion between std::string and std::wstring, with UTF-8 / ANSI supports
		////////////////////////////////////////////////////////////////////////////////////
		namespace detail
		{
			inline std::string _to_string_internal(const std::wstring& input, UINT codePage = CP_ACP)
			{
				std::string output;
				if (!input.empty())
				{
					const int sz = static_cast<int>(input.size());
					int len = WideCharToMultiByte(codePage, 0, input.c_str(), sz, NULL, 0, NULL, NULL);
					assert(len > 0 && "WideCharToMultiByte failed.");
					output.resize(len);
					len = WideCharToMultiByte(codePage, 0, input.c_str(), sz, &output[0], len, 0, 0);
					assert(len == output.size() && "WideCharToMultiByte failed.");
				}
				return output;
			}

			inline std::wstring _to_wstring_internal(const std::string& input, UINT codePage = CP_ACP)
			{
				std::wstring output;
				if (!input.empty())
				{
					const int sz = static_cast<int>(input.size());
					int len = MultiByteToWideChar(codePage, 0, input.c_str(), sz, NULL, 0);
					assert(len > 0 && "MultiByteToWideChar failed.");
					output.resize(len);
					len = MultiByteToWideChar(codePage, 0, input.c_str(), sz, &output[0], len);
					assert(len == output.size() && "MultiByteToWideChar failed.");
				}
				return output;
			}
		}

		/// Convertion between std::string and std::wstring (ANSI compatible)
		///@{
		inline std::string to_string(const std::wstring &wideChars)
		{
			return detail::_to_string_internal(wideChars);
		}
		inline std::wstring to_wstring(const std::string &multiBytes)
		{
			return detail::_to_wstring_internal(multiBytes);
		}
		///@}

		/// Convertion between UTF-8 multibytes std::string and std::wstring
		///@{
		inline std::string wstring_to_utf8(const std::wstring &utf16)
		{
			return detail::_to_string_internal(utf16, CP_UTF8);
		}
		inline std::wstring utf8_to_wstring(const std::string &utf8)
		{
			return detail::_to_wstring_internal(utf8, CP_UTF8);
		}
		///@}

		////////////////////////////////////////////////////////////////////////////////////
		/// Utilities to obtain a string with all alphabets converted to lowercase/uppercase
		////////////////////////////////////////////////////////////////////////////////////
		namespace detail
		{
			inline char _to_lower(char c) { return static_cast<char>(::tolower(c)); }
			inline char _to_upper(char c) { return static_cast<char>(::toupper(c)); }
			inline wchar_t _to_lower(wchar_t c) { return ::towlower(c); }
			inline wchar_t _to_upper(wchar_t c) { return ::towupper(c); }
		}

		//template<typename CharT>
		//inline std::basic_string<CharT> to_lower_case(const std::basic_string<CharT>& str)
		//{
		//	std::basic_string<CharT> result;
		//	std::transform(str.begin(), str.end(), std::back_inserter(result),
		//					[](CharT c) { return detail::_to_lower(c); });
		//	return result;
		//}

		//template<typename CharT>
		//inline std::basic_string<CharT> to_upper_case(const std::basic_string<CharT>& str)
		//{
		//	std::basic_string<CharT> result;
		//	std::transform(str.begin(), str.end(), std::back_inserter(result),
		//					[](CharT c) { return detail::_to_upper(c); });
		//	return result;
		//}

		////////////////////////////////////////////////////////////////////////////////////
		/// Case-insensitive string comparison
		////////////////////////////////////////////////////////////////////////////////////
		template<typename CharT>
		inline int compare_i(const std::basic_string<CharT>& s1, const std::basic_string<CharT>& s2)
		{
			auto iter1 = s1.begin();
			auto iter2 = s2.begin();
			for (; iter1 != s1.end() && iter2 != s2.end(); ++iter1, ++iter2)
			{
				int upper1 = detail::_to_upper(*iter1);
				int upper2 = detail::_to_upper(*iter2);
				if (upper1 < upper2)
					return -1;
				else if (upper1 > upper2)
					return 1;
			}

			if (iter1 != s1.end() && iter2 == s2.end())
				return 1;
			if (iter1 == s1.end() && iter2 != s2.end())
				return -1;
			return 0;
		}

		template<typename CharT>
		inline int compare_i(const std::basic_string<CharT>& s1, const CharT *s2)
		{
			return compare_i(s1, std::basic_string<CharT>(s2));
		}

		////////////////////////////////////////////////////////////////////////////////////
		/// String generator that accepts C standard string formatter (e.g., "%d, %s\n")
		////////////////////////////////////////////////////////////////////////////////////
		namespace detail
		{
			constexpr size_t kBufferSize = 1024;
		}

		inline std::string format_va_list(const char *s, va_list args)
		{
			char buf[detail::kBufferSize];
			vsnprintf(buf, detail::kBufferSize, s, args);
			return std::string{ buf };
		}

		inline std::wstring format_va_list(const wchar_t *s, va_list args)
		{
			wchar_t buf[detail::kBufferSize];
			vswprintf(buf, detail::kBufferSize, s, args);
			return std::wstring{ buf };
		}

		template<typename CharT>
		inline std::basic_string<CharT> format(const CharT *s, ...)
		{
			va_list args;
			va_start(args, s);
			auto res = format_va_list(s, args);
			va_end(args);
			return res;
		}

		////////////////////////////////////////////////////////////////////////////////////
		/// String tokenizing utilities to split a string into substrings on delimiters.
		///
		/// If verbatimSymbols is specified, everything enclosed within a pair of
		/// verbatimSymbols cannot be split into separate tokens even though token
		/// separator is part of the substring.
		/// e.g. if we tokenize the string "'20,000', '30,000', '40,000'" using " ,"
		/// (space and comma) as token separator and ' (single quote) as verbatim symbol,
		/// we will split it into three strings: ['20,000', '30,000', '40,000']
		////////////////////////////////////////////////////////////////////////////////////
		template<typename StringVector>
		inline void tokenize_string(
			const typename StringVector::value_type& str,
			StringVector& tokens,
			const typename StringVector::value_type& delimiters,
			const typename StringVector::value_type& verbatimSymbols = typename StringVector::value_type())
		{
			typedef typename StringVector::value_type string_type;
			typedef typename string_type::size_type size_type;

			assert(delimiters.compare(verbatimSymbols));

			size_type startPos = str.find_first_not_of(delimiters, 0);
			size_type endPos = str.find_first_of(delimiters, startPos);

			size_type verbatimStart = str.find_first_of(verbatimSymbols);
			size_type verbatimEnd = str.find_first_of(verbatimSymbols, verbatimStart + 1);
			// make sure verbatim symbols come in pairs
			assert(verbatimStart == string_type::npos || verbatimEnd != verbatimStart);

			while (startPos != endPos)
			{
				if (verbatimStart != string_type::npos)
				{
					assert(startPos <= verbatimStart);
					if (endPos > verbatimStart&& endPos < verbatimEnd)
					{ // in verbatim mode
						endPos = str.find_first_of(delimiters, endPos + 1);
						continue;
					}
					if (endPos > verbatimEnd)
					{ // current verbatim mode ended

						// prepare for the next verbatim
						verbatimStart = str.find_first_of(verbatimSymbols, verbatimEnd + 1);
						verbatimEnd = str.find_first_of(verbatimSymbols, verbatimStart + 1);
						assert(verbatimStart == string_type::npos || verbatimEnd != verbatimStart);
					}
				}
				if (verbatimStart == string_type::npos || endPos < verbatimStart)
				{
					// found a token, add it to the vector.
					tokens.push_back(str.substr(startPos, endPos - startPos));

					startPos = str.find_first_not_of(delimiters, endPos);
					endPos = str.find_first_of(delimiters, startPos);
				}
			}
		}

		inline std::vector<std::string> tokenize_string(
			const std::string &str,
			const std::string& delimiters,
			const std::string& verbatimSymbols = std::string())
		{
			std::vector<std::string> tokens;
			tokenize_string(str, tokens, delimiters, verbatimSymbols);
			return tokens;
		}

		inline std::vector<std::wstring> tokenize_string(
			const std::wstring &str,
			const std::wstring& delimiters,
			const std::wstring& verbatimSymbols = std::wstring())
		{
			std::vector<std::wstring> tokens;
			tokenize_string(str, tokens, delimiters, verbatimSymbols);
			return tokens;
		}

		////////////////////////////////////////////////////////////////////////////////////
		/// Base64 encoding/decoding utilities
		////////////////////////////////////////////////////////////////////////////////////
		inline bool is_base64(unsigned char c) {
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		inline std::string base64_encode(const char* bytes_to_encode, unsigned int in_len) {
			std::string ret;
            ret.reserve(4 * (in_len / 3));
			int i = 0;
			int j = 0;
			unsigned char char_array_3[3];
			unsigned char char_array_4[4];
			const std::string base64_chars =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

			while (in_len--) {
				char_array_3[i++] = *(bytes_to_encode++);
				if (i == 3) {
					char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
					char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
					char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
					char_array_4[3] = char_array_3[2] & 0x3f;

					for (i = 0; (i < 4); i++)
						ret += base64_chars[char_array_4[i]];
					i = 0;
				}
			}

			if (i)
			{
				for (j = i; j < 3; j++)
					char_array_3[j] = '\0';

				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (j = 0; (j < i + 1); j++)
					ret += base64_chars[char_array_4[j]];

				while ((i++ < 3))
					ret += '=';

			}

			return ret;

		}

		inline std::string base64_encode(const std::vector<char>& bytes_to_encode, unsigned int in_len) {
			return base64_encode(&bytes_to_encode[0], in_len);
		}

		inline std::vector<char> base64_decode(const char* encoded_string, int in_len)
        {
            std::vector<char> outdata;
            outdata.reserve(3 * (in_len / 4));

			int i = 0;
			int j = 0;
			int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
			const std::string base64_chars =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

			while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
				char_array_4[i++] = encoded_string[in_]; in_++;
				if (i == 4) {
					for (i = 0; i < 4; i++)
						char_array_4[i] = (unsigned char)base64_chars.find(char_array_4[i]);

					char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
					char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
					char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

					for (i = 0; (i < 3); i++)
						outdata.push_back(char_array_3[i]);
					i = 0;
				}
			}

			if (i) {
				for (j = i; j < 4; j++)
					char_array_4[j] = 0;

				for (j = 0; j < 4; j++)
					char_array_4[j] = (unsigned char)base64_chars.find(char_array_4[j]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (j = 0; (j < i - 1); j++) outdata.push_back(char_array_3[j]);
			}
            return outdata;
		}
	};

	/************************************************************************/
	/* Comparator for case-insensitive comparison in STL assos. containers  */
	/************************************************************************/
	struct ci_less
	{
		bool operator() (const std::string & s1, const std::string & s2) const
		{
			return sutils::compare_i(s1, s2) < 0;
		}
	};
};
