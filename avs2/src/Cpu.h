
#ifndef RISCV_SIM_CPU_H
#define RISCV_SIM_CPU_H

#include "Memory.h"
#include "Decoder.h"
#include "RegisterFile.h"
#include "CsrFile.h"
#include "Executor.h"
#include <iostream>

class Cpu
{
public:
    Cpu(Memory& mem)
        : _mem(mem)
    {
        _exe.InitExecutor();
    }

    void ProcessInstruction()
    {
        // reading raw instruction from memory
        Word raw_instr = _mem.Request(_ip);
        // decoding raw instruction
        InstructionPtr decoded_instr = _decoder.Decode(raw_instr);
        // reading data from registers
        _rf.Read(decoded_instr);
        _csrf.Read(decoded_instr);
        // executing decoded instruction
        _exe.Execute(decoded_instr, _ip);
        // writing evaluation result to register
        _mem.Request(decoded_instr);
        _rf.Write(decoded_instr);
        _csrf.Write(decoded_instr);
        _csrf.InstructionExecuted();
        // setting ip to next instruction
	    _ip = decoded_instr->_nextIp;
    }

    void Reset(Word ip)
    {
        _csrf.Reset();
        _ip = ip;
    }

    std::optional<CpuToHostData> GetMessage()
    {
        return _csrf.GetMessage();
    }

private:
    Reg32 _ip;
    Decoder _decoder;
    RegisterFile _rf;
    CsrFile _csrf;
    Executor _exe;
    Memory& _mem;
};


#endif //RISCV_SIM_CPU_H
