#include "String.h"
#include <sstream>

namespace Light
{
    std::wstring String::ToWString(const std::string& input)
    {
        using namespace std;
        const char* source = input.c_str();
        const size_t charNum = input.size() + 1;

        auto* dest = new wchar_t[charNum];
        const size_t errno_t = mbstowcs_s(nullptr, dest, charNum, source, _TRUNCATE);
        if (errno_t != 0)throw std::exception("转换字符串失败");

        wstring result = {dest};
        delete[] dest;
        return result;
    }
    std::string String::ToString(const std::wstring& input)
    {
        using namespace std;
        const wchar_t* wcs = input.c_str();
        const size_t wcharNum = sizeof(wchar_t) * input.size() + 1;

        auto* dest = new char[wcharNum];
        const size_t errno_t = wcstombs_s(nullptr, dest, wcharNum, wcs, _TRUNCATE);
        if (errno_t != 0)throw std::exception("转换字符串失败");

        string result = {dest};
        delete[] dest;
        return result;
    }
    void String::Split(const std::string& text, const std::string& splitWord, std::vector<std::string>& result)
    {
        size_t begin = 0;
        const size_t splitLength = splitWord.size();

        result.clear();

        while (true)
        {
            const size_t fragmentLength = text.find(splitWord, begin) - begin;
            if (fragmentLength == std::string::npos - begin)
            {
                result.emplace_back(text.substr(begin));
                break;
            }

            result.emplace_back(text.substr(begin, fragmentLength));
            begin += fragmentLength;

            begin += splitLength;
        }
    }

    int String::EncodingBase64(const unsigned char* in_data, int in_count, unsigned char* out_base64)
    {
        static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789""+/"; // -_
        const int cnt = in_count / 3 * 3;
        for (int i = 0; i < cnt; i += 3, out_base64 += 4)
        {
            const unsigned char a0 = in_data[i];
            const unsigned char a1 = in_data[i + 1];
            const unsigned char a2 = in_data[i + 2];
            out_base64[0] = base64_chars[a0 >> 2];
            out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
            out_base64[2] = base64_chars[((a1 & 0x0f) << 2) + (a2 >> 6)];
            out_base64[3] = base64_chars[a2 & 0x3f];
        }

        if (cnt + 1 == in_count)
        {
            const unsigned char a0 = in_data[cnt];
            out_base64[0] = base64_chars[a0 >> 2];
            out_base64[1] = base64_chars[(a0 & 0x03) << 4];
            out_base64[2] = '=';
            out_base64[3] = '=';
        }
        else if (cnt + 2 == in_count)
        {
            const unsigned char a0 = in_data[cnt];
            const unsigned char a1 = in_data[cnt + 1];
            out_base64[0] = base64_chars[a0 >> 2];
            out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
            out_base64[2] = base64_chars[(a1 & 0x0f) << 2];
            out_base64[3] = '=';
        }
        return (in_count + 2) / 3 * 4;
    }
    int String::DecodingBase64(const unsigned char* in_base64, int in_count, unsigned char* out_data)
    {
        static const unsigned char pm[] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 30 
            0, 0, 0, 62, 0, 62, 0, 63, 52, 53, // 40 
            54, 55, 56, 57, 58, 59, 60, 61, 0, 0, // 50 
            0, 0, 0, 0, 0, 0, 1, 2, 3, 4, // 60 
            5, 6, 7, 8, 9, 10, 11, 12, 13, 14, // 70 
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // 80 
            25, 0, 0, 0, 0, 63, 0, 26, 27, 28, // 90 
            29, 30, 31, 32, 33, 34, 35, 36, 37, 38, // 100 
            39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 110 
            49, 50, 51, 0, 0, 0, 0, 0 // 120 
        };

        const int cnt = in_count - 4;
        for (int i = 0; i < cnt; i += 4, out_data += 3)
        {
            const unsigned char a0 = pm[in_base64[i]];
            const unsigned char a1 = pm[in_base64[i + 1]];
            const unsigned char a2 = pm[in_base64[i + 2]];
            const unsigned char a3 = pm[in_base64[i + 3]];
            out_data[0] = static_cast<unsigned char>((a0 << 2) + (a1 >> 4));
            out_data[1] = static_cast<unsigned char>((a1 << 4) + (a2 >> 2));
            out_data[2] = static_cast<unsigned char>((a2 << 6) + a3);
        }

        int out_cnt = cnt / 4 * 3;
        if (in_base64[cnt + 2] == '=')
        {
            const unsigned char a0 = pm[in_base64[cnt]];
            const unsigned char a1 = pm[in_base64[cnt + 1]];
            out_data[0] = static_cast<unsigned char>((a0 << 2) + (a1 >> 4));
            out_cnt += 1;
        }
        else if (in_base64[cnt + 3] == '=')
        {
            const unsigned char a0 = pm[in_base64[cnt]];
            const unsigned char a1 = pm[in_base64[cnt + 1]];
            const unsigned char a2 = pm[in_base64[cnt + 2]];
            out_data[0] = static_cast<unsigned char>((a0 << 2) + (a1 >> 4));
            out_data[1] = static_cast<unsigned char>((a1 << 4) + (a2 >> 2));
            out_cnt += 2;
        }
        else
        {
            const unsigned char a0 = pm[in_base64[cnt]];
            const unsigned char a1 = pm[in_base64[cnt + 1]];
            const unsigned char a2 = pm[in_base64[cnt + 2]];
            const unsigned char a3 = pm[in_base64[cnt + 3]];
            out_data[0] = static_cast<unsigned char>((a0 << 2) + (a1 >> 4));
            out_data[1] = static_cast<unsigned char>((a1 << 4) + (a2 >> 2));
            out_data[2] = static_cast<unsigned char>((a2 << 6) + a3);
            out_cnt += 3;
        }
        return out_cnt;
    }
    void String::EncodingBase64(const std::vector<std::byte>& input, std::string& output)
    {
        output.resize((input.size() + 2) / 3 * 4); //每3个普通字节转为4个base64字节储存，所以数量必须为3的倍数
        output.resize(EncodingBase64(
            reinterpret_cast<const unsigned char*>(input.data()),
            static_cast<int>(input.size()),
            reinterpret_cast<unsigned char*>(output.data())
        ));
    }
    void String::DecodingBase64(const std::string& input, std::vector<std::byte>& output)
    {
        output.resize(input.size() / 4 * 3); //base64字节数量一定是4的倍数，4个base64字节还原成3个普通字节
        output.resize(DecodingBase64(
            reinterpret_cast<const unsigned char*>(input.data()),
            static_cast<int>(input.size()),
            reinterpret_cast<unsigned char*>(output.data())
        ));//存在为凑3字节的空字节，故需二次重置大小来确定
    }
}
