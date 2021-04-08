#include <iostream>
#include "Decoder.h"
#include "Executor.h"
#include <assert.h>

// INSTRUCTION:
// bltu rs1, rs2, 12
// rs1 = 00001
// rs2 = 00010
// offset = 000000001100
// opcode = 1100011
// spec: imm[12|10:5] rs2 rs1 101 imm[4:1|11] 1100011

void test_decoder()
{
    Decoder decoder;
    Word raw_instr = 0b00000000001000001110011001100011;
    InstructionPtr decoded_instr = decoder.Decode(raw_instr);
    assert(decoded_instr->_type == IType::Br);
    assert(decoded_instr->_brFunc == BrFunc::Ltu);
    assert(decoded_instr->_src1.value() == 1);
    assert(decoded_instr->_src2.value() == 2);
    assert(decoded_instr->_imm.value() == 12);
    std::cout << "Decoder test passed.\n";
}

void test_executor()
{
    Decoder decoder;
    Executor executor;
    executor.InitExecutor();
    Word raw_instr = 0b00000000001000001110011001100011;
    // rs1 val = 0, rs2 val = 0
    InstructionPtr decoded_instr = decoder.Decode(raw_instr);
    executor.Execute(decoded_instr, 0);
    assert(decoded_instr->_nextIp == 4);
    // rs1 val = 0, rs2 val = 1
    decoded_instr = decoder.Decode(raw_instr);
    decoded_instr->_src1Val = 0;
    decoded_instr->_src2Val = 1;
    executor.Execute(decoded_instr, 0);
    assert(decoded_instr->_nextIp == 12);
    std::cout << "Executor test passed.\n";
}

void run_tests()
{
    test_decoder();
    test_executor();
}
