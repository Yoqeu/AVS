
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"
#include <vector>
#include <functional>
#include <map>

class Executor
{
public:
    void Execute(InstructionPtr& instr, Word ip)
    {
        // ALU evaluations block
        if (instr->_src1.has_value() && (instr->_imm.has_value() || instr->_src2.has_value()))
        {  
            Word op1 = instr->_src1Val;        
            Word op2 = instr->_imm.value_or(instr->_src2Val);
            Word result = _aluFuncs[instr->_aluFunc](op1, op2);  

            switch (instr->_type) 
            {
                case IType::Alu:
                    instr->_data = result; 
                    break;
                case IType::St:
                    instr->_data = instr->_src2Val; 
                    instr->_addr = result;  
                    break;
                case IType::Ld:
                    instr->_addr = result;  
                    break;
            }
        }
        switch (instr->_type) 
        {
                case IType::Csrr:
                    instr->_data = instr->_csrVal;  
                    break;
                case IType::Csrw:
                    instr->_data = instr->_src1Val;  
                    break;
                case IType::J:
                case IType::Jr:
                    instr->_data = ip + 4; 
                    break;
                case IType::Auipc:
                    instr->_data = ip + instr->_imm.value(); 
                    break;
        }
        // branching block
        Word next_addr;
        bool branch = false;
        if (instr->_src1.has_value() && instr->_src2.has_value())
        {
            switch (instr->_type)
            {
                case IType::Br:
                case IType::J:
                    next_addr = ip + instr->_imm.value();
                    break;
                case IType::Jr:
                    next_addr = instr->_src1Val + instr->_imm.value();
                    break;
            }
            Word op1 = instr->_src1Val;
            Word op2 = instr->_src2Val;
            branch = _brFuncs[instr->_brFunc](op1, op2);
        }
        if (branch)
            instr->_nextIp = next_addr;
        else
            instr->_nextIp = ip + 4;
    }

    void InitExecutor()
    {
        // init ALU functions
        _aluFuncs[AluFunc::Add] = [this] (Word op1, Word op2) { return Add(op1, op2); };
        _aluFuncs[AluFunc::Sll] = [this] (Word op1, Word op2) { return Sll(op1, op2); };
        _aluFuncs[AluFunc::Slt] = [this] (Word op1, Word op2) { return Slt(op1, op2); };
        _aluFuncs[AluFunc::Sltu] = [this] (Word op1, Word op2) { return Sltu(op1, op2); };
        _aluFuncs[AluFunc::Xor] = [this] (Word op1, Word op2) { return Xor(op1, op2); };
        _aluFuncs[AluFunc::And] = [this] (Word op1, Word op2) { return And(op1, op2); };
        _aluFuncs[AluFunc::Or] = [this] (Word op1, Word op2) { return Or(op1, op2); };
        _aluFuncs[AluFunc::Sub] = [this] (Word op1, Word op2) { return Sub(op1, op2); };
        _aluFuncs[AluFunc::Sra] = [this] (Word op1, Word op2) { return Sra(op1, op2); };
        _aluFuncs[AluFunc::Srl] = [this] (Word op1, Word op2) { return Srl(op1, op2); };
        // init branch functions
        _brFuncs[BrFunc::Eq] = [this] (Word op1, Word op2) { return Eq(op1, op2); };
        _brFuncs[BrFunc::Neq] = [this] (Word op1, Word op2) { return Neq(op1, op2); };
        _brFuncs[BrFunc::Lt] = [this] (Word op1, Word op2) { return Lt(op1, op2); };
        _brFuncs[BrFunc::Ltu] = [this] (Word op1, Word op2) { return Ltu(op1, op2); };
        _brFuncs[BrFunc::Ge] = [this] (Word op1, Word op2) { return Ge(op1, op2); };
        _brFuncs[BrFunc::Geu] = [this] (Word op1, Word op2) { return Geu(op1, op2); };
        _brFuncs[BrFunc::AT] = [this] (Word op1, Word op2) { return AT(op1, op2); };
        _brFuncs[BrFunc::NT] = [this] (Word op1, Word op2) { return NT(op1, op2); };
    }

private:
    std::map<AluFunc, std::function<Word(Word, Word)>> _aluFuncs;
    std::map<BrFunc, std::function<Word(Word, Word)>> _brFuncs;

    Word Add(Word op1, Word op2)
    {
        return op1 + op2;
    }

    Word Sub(Word op1, Word op2)
    {
        return op1 - op2;
    }

    Word And(Word op1, Word op2)
    {
        return op1 & op2;
    }

    Word Or(Word op1, Word op2)
    {
        return op1 | op2;
    }

    Word Xor(Word op1, Word op2)
    {
        return op1 ^ op2;
    }

    Word Slt(Word op1, Word op2)
    {
        return (Word)((int16_t)op1 < (int16_t)op2);
    }

    Word Sltu(Word op1, Word op2)
    {
        return (Word)(op1 < op2);
    }
    
    Word Sll(Word op1, Word op2)
    {
        return op1 << (op2 % 32);
    }

    Word Srl(Word op1, Word op2)
    {
        return op1 >> (op2 % 32);
    }

    Word Sra(Word op1, Word op2)
    {
        return (Word)((int16_t)op1 >> (op2 % 32));
    }

    Word Eq(Word op1, Word op2)
    {
        return (Word)(op1 == op2);
    }

    Word Neq(Word op1, Word op2)
    {
        return (Word)(op1 != op2);
    }

    Word Lt(Word op1, Word op2)
    {
        return (Word)((int16_t)op1 < (int16_t)op2);
    }

    Word Ltu(Word op1, Word op2)
    {
        return (Word)(op1 < op2);
    }

    Word Ge(Word op1, Word op2)
    {
        return (Word)((int16_t)op1 >= (int16_t)op2);
    }

    Word Geu(Word op1, Word op2)
    {
        return (Word)(op1 >= op2);
    }

    Word AT(Word op1, Word op2)
    {
        return 1;
    }

    Word NT(Word op1, Word op2)
    {
        return 0;
    }
};

#endif // RISCV_SIM_EXECUTOR_H
