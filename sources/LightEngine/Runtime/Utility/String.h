#pragma once
#include<string>
#include <codecvt>
#include <vector>

namespace Light
{
    class String
    {
    public:
        static std::wstring ToWString(const std::string& input);
        static std::string ToString(const std::wstring& input);
        static void Split(const std::string& text, const std::string& splitWord, std::vector<std::string>& result);

        static int EncodingBase64(const unsigned char* in_data, int in_count, unsigned char* out_base64);
        static int DecodingBase64(const unsigned char* in_base64, int in_charcount, unsigned char* out_data);
        static std::string EncodingBase64(const std::vector<std::byte>& input)
        {
            std::string output;
            output.resize((input.size() + 2) / 3 * 4); //每3个普通字节转为4个base64字节储存，所以数量必须为3的倍数
            output.resize(EncodingBase64(
                    reinterpret_cast<const unsigned char*>(input.data()),
                    static_cast<int>(input.size()),
                    reinterpret_cast<unsigned char*>(output.data()))
            );
            return output;
        }
        static std::vector<std::byte> DecodingBase64(const std::string& input)
        {
            std::vector<std::byte> output;
            output.resize(input.size() / 4 * 3); //base64字节数量一定是4的倍数，4个base64字节还原成3个普通字节
            output.resize(DecodingBase64(
                    reinterpret_cast<const unsigned char*>(input.data()),
                    static_cast<int>(input.size()),
                    reinterpret_cast<unsigned char*>(output.data()))
            );
            return output;
        }
    };
}