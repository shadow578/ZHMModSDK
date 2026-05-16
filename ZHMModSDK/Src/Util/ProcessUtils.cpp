#include "ProcessUtils.h"

#include <TlHelp32.h>
#include <unordered_set>
#include <cstring>

#include "Logging.h"

using namespace Util;

uintptr_t ProcessUtils::SearchPattern(
    uintptr_t p_pBaseAddress, size_t p_nScanSize, const uint8_t* p_pPattern, const char* p_szMask
) {
    const size_t s_PatternSize = strlen(p_szMask);

    if (s_PatternSize <= 1) {
        return 0;
    }

    if (p_nScanSize < s_PatternSize) {
        return 0;
    }

    if (p_szMask[0] == '?') {
        return 0;
    }

    const uint8_t* s_pSearchStart = reinterpret_cast<const uint8_t*>(p_pBaseAddress);
    const uint8_t* const s_pSearchEnd = s_pSearchStart + p_nScanSize - s_PatternSize;

    while (s_pSearchStart <= s_pSearchEnd)
    {
        size_t s_nRemainingBytes = static_cast<size_t>(s_pSearchEnd - s_pSearchStart) + 1;
        const uint8_t* s_pCanidate = static_cast<const uint8_t*>(memchr(s_pSearchStart, p_pPattern[0], s_nRemainingBytes));
        if (!s_pCanidate) {
            break;
        }

        if (IsPatternMatch(s_pCanidate, p_pPattern, p_szMask, s_PatternSize)) {
            return reinterpret_cast<uintptr_t>(s_pCanidate);
        }

        s_pSearchStart = s_pCanidate + 1;
    }

    return 0;
}

bool ProcessUtils::IsPatternMatch(
    const uint8_t* p_pAddress, const uint8_t* p_pPattern, const char* p_szMask, const size_t p_nPatternSize
) {
    for (size_t i = 0; i < p_nPatternSize; ++i) {
        if (p_szMask[i] == '?') {
            continue;
        }

        if (p_pAddress[i] != p_pPattern[i]) {
            return false;
        }
    }

    return true;
}

uint32_t ProcessUtils::GetSizeOfCode(HMODULE p_Module) {
    PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
    PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) +
        s_DOSHeader->e_lfanew);

    if (!s_NTHeader)
        return 0;

    return static_cast<uint32_t>(s_NTHeader->OptionalHeader.SizeOfCode);
}

uintptr_t ProcessUtils::GetBaseOfCode(HMODULE p_Module) {
    PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
    PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) +
        s_DOSHeader->e_lfanew);

    if (!s_NTHeader)
        return 0;

    return static_cast<uintptr_t>(s_NTHeader->OptionalHeader.BaseOfCode);
}

uint32_t ProcessUtils::GetSizeOfImage(HMODULE p_Module) {
    PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
    PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) +
        s_DOSHeader->e_lfanew);

    if (!s_NTHeader)
        return 0;

    return static_cast<uint32_t>(s_NTHeader->OptionalHeader.SizeOfImage);
}

std::tuple<uintptr_t, uintptr_t> ProcessUtils::GetSectionStartAndEnd(
    HMODULE p_Module, const std::string& p_SectionName
) {
    PIMAGE_DOS_HEADER s_DOSHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(p_Module);
    PIMAGE_NT_HEADERS s_NTHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(p_Module) +
        s_DOSHeader->e_lfanew);

    if (!s_NTHeader)
        return std::make_tuple<uintptr_t, uintptr_t>(0, 0);

    PIMAGE_SECTION_HEADER s_Section = IMAGE_FIRST_SECTION(s_NTHeader);

    for (int i = 0; i < s_NTHeader->FileHeader.NumberOfSections; ++i) {
        if (strcmp(reinterpret_cast<const char*>(s_Section->Name), p_SectionName.c_str()) == 0) {
            uintptr_t s_RDataSectionStart = s_Section->VirtualAddress;
            s_RDataSectionStart += reinterpret_cast<uintptr_t>(p_Module);

            uintptr_t s_RDataSectionEnd = s_RDataSectionStart + s_Section->SizeOfRawData;

            return std::make_tuple(s_RDataSectionStart, s_RDataSectionEnd);
        }

        ++s_Section;
    }

    return std::make_tuple<uintptr_t, uintptr_t>(0, 0);
}

uintptr_t ProcessUtils::GetRelativeAddr(uintptr_t p_Base, int32_t p_Offset) {
    uintptr_t s_RelAddrPtr = p_Base + p_Offset;
    int32_t s_RelAddr = *reinterpret_cast<int32_t*>(s_RelAddrPtr);

    return s_RelAddrPtr + s_RelAddr + sizeof(int32_t);
}