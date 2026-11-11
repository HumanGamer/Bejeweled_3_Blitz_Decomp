#pragma once

#include "Common.h"

namespace Sexy
{
    enum ECfgMachineValueType
    {
        CFGMVT_None,
        CFGMVT_Bool,
        CFGMVT_Int,
        CFGMVT_Double,
        CFGMVT_String,
        CFGMVT_COUNT,
    };

    class CfgMachineValue
    {
    public:
        ECfgMachineValueType mType;
        int mIntValue;
        double mDoubleValue;
        SexyString mStringValue; // TODO: might be std::wstring instead of SexyString
    };

    class ICfgMachine
    {
    public:
        virtual void MachineDestroy() = 0;
        virtual bool MachineExecuteFunction(const char* str, CfgMachineValue* value) = 0;
        virtual bool MachineDisassembleToFile(const std::string& file) = 0;
    };

    class ICfgCompiler
    {
        // TODO: Implement

    public:
        static ICfgCompiler* CompilerCreate();

    public:
        virtual ICfgMachine* CompilerCreateMachineFromFile(const std::string& file, std::string& error) = 0;
    };

    class CfgCompiler : public ICfgCompiler
    {
    public:
        ICfgMachine* CompilerCreateMachineFromFile(const std::string& file, std::string& error) override;
    };
}