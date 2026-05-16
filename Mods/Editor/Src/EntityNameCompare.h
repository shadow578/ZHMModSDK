#pragma once

#include <cctype>
#include <string>
#include <unordered_map>

struct EntityNameCompare {
    struct Parsed {
        std::string m_Base;
        bool m_HasNumber = false;
        int m_Number = 0;
    };

    static const Parsed &GetParsed(const std::string &p_sName) {
        thread_local std::unordered_map<std::string, Parsed> s_Cache;

        const auto s_It = s_Cache.find(p_sName);
        if (s_It != s_Cache.end())
        {
            return s_It->second;
        }

        auto s_Result = s_Cache.emplace(p_sName, Parse(p_sName));
        return s_Result.first->second;
    }

    static std::string StripSuffix(const std::string& p_String) {
        const size_t s_Position = p_String.find(" (");

        return (s_Position == std::string::npos) ? p_String : p_String.substr(0, s_Position);
    }

    static Parsed Parse(const std::string& p_String) {
        Parsed s_Parsed;
        const std::string s_Name = StripSuffix(p_String);

        const size_t s_End = s_Name.size();
        size_t s_Position = s_End;

        while (s_Position > 0 && std::isdigit(static_cast<unsigned char>(s_Name[s_Position - 1]))) {
            --s_Position;
        }

        if (s_Position < s_End) {
            s_Parsed.m_Base = s_Name.substr(0, s_Position);
            s_Parsed.m_Number = std::stoi(s_Name.substr(s_Position));
            s_Parsed.m_HasNumber = true;
        }
        else {
            s_Parsed.m_Base = s_Name;
        }

        return s_Parsed;
    }

    bool operator()(const std::string& p_A, const std::string& p_B) const {
        const Parsed &s_ParsedA = GetParsed(p_A);
        const Parsed &s_ParsedB = GetParsed(p_B);

        if (s_ParsedA.m_Base == s_ParsedB.m_Base) {
            if (s_ParsedA.m_HasNumber && s_ParsedB.m_HasNumber) {
                return s_ParsedA.m_Number < s_ParsedB.m_Number;
            }

            if (!s_ParsedA.m_HasNumber && s_ParsedB.m_HasNumber) {
                return true;
            }

            if (s_ParsedA.m_HasNumber && !s_ParsedB.m_HasNumber) {
                return false;
            }

            return p_A < p_B;
        }

        return s_ParsedA.m_Base < s_ParsedB.m_Base;
    }
};