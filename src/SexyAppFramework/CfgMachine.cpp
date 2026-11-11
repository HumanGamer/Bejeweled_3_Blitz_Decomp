#include "CfgMachine.h"

using namespace Sexy;

ICfgCompiler *ICfgCompiler::CompilerCreate()
{
    return new CfgCompiler();
}

ICfgMachine *CfgCompiler::CompilerCreateMachineFromFile(const std::string &file, std::string &error)
{
    return nullptr; // TODO: Implement
}
