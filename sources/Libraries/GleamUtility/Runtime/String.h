#pragma once
#include<string>
#include <codecvt>
#include <vector>

namespace Gleam
{
    class String
    {
    public:
        static std::wstring ToWString(const std::string& input);
        static std::string ToString(const std::wstring& input);
        static void Split(const std::string& text, const std::string& splitWord, std::vector<std::string>& result);

        static int EncodingBase64(const unsigned char* in_data, int in_count, unsigned char* out_base64);
        static int DecodingBase64(const unsigned char* in_base64, int in_count, unsigned char* out_data);
        static void EncodingBase64(const std::vector<std::byte>& input, std::string& output);
        static void DecodingBase64(const std::string& input, std::vector<std::byte>& output);
    };
}