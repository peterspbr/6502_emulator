/* 
 * Code written by Peter S
 * 2021 for educational purposes
 * This code was written following the Dave Poo tutorials on Youtube https://www.youtube.com/watch?v=qJgsuQoy9bc&list=PLLwK93hM93Z13TRzPx9JqTIn33feefl37
*/

#include <iostream>

// 8 Bit variables
using Byte = unsigned char;

// 16 Bit variables
using Word = unsigned short;

 // Unsigned int with 32 bytes
using u32 = __uint32_t;

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64; // Set the max memory size to 65536 bytes.
    Byte Data[MAX_MEM]; // Data is the where the instrucions will be stored.

    // Initialization process
    void Initialise()
    {
        for(u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    // Read 1 byte
    Byte operator[]( u32 Address ) const
    {
        return Data[Address];
    }

    // Write 1 byte
    Byte& operator[]( u32 Address )
    {
        return Data[Address];
    }

    // Write 2 bytes
    void WriteWord( Word Value, u32 Address, u32& Cycles )
    {
        Data[Address] = Value & 0xFF;
        Data[Address + 1] = (Value >> 8);
        Cycles += 2;
    }
};

// The CPU
struct CPU
{
    Word PC; // Program couter
    Word SP; // Stack pointer

    // 8 Bit registers: Accumulator, X register, Y register.
    Byte A, X, Y;

    // Flags
    Byte C : 1; // Carry flag
    Byte Z : 1; // Zero page flage
    Byte I : 1; // Interrupt disable
    Byte D : 1; // Decimal mode
    Byte B : 1; // Break command
    Byte V : 1; // Overflow flag
    Byte N : 1; // Negative flag

    // Reset process
    void Reset( Mem& memory )
    {
        PC = 0xFFFC; // Set the Program Counter to the Address FFFC like the Commodore 64 does.
        SP = 0x0000; // And then set the Stack pointer to 0.
        C = Z = I = D = B = V = N = 0; // Reset all flags ** Note this is not an accurate reset process of the Commodore64 ** 
        A = X = Y = 0; // Set all registers to 0.
        memory.Initialise(); // Then initialise the memory.
    }

    /* -- Immediate mode functions -- */

    // Fetch the bytes to be loaded into the memory
    Byte FetchByte( u32& Cycles, Mem& memory )
    {
        Byte Data = memory[PC]; // Check where the Program Counter is pointing to.
        PC++; // Then increment
        Cycles--; // Sacrifice the necessary cycle count
        return Data; // Then return the data
    }

    // Reads the bytes from memory
    Byte ReadByte( u32& Cycles, Byte Address, Mem& memory )
    {
        Byte Data = memory[Address]; // Get the memory address to be read
        Cycles--; // Sacrifice the necessary cycle count
        return Data; // Then return the data
    }

    /* -- Absolute mode functions -- */

    // Fetch the words (the 16 bit bad guys)
    Word FetchWord( u32& Cycles, Mem& memory )
    {
        Word Data = memory[PC]; // Gets the location from the Program Counter
        PC++; // Then increment it
        
        Data |= (memory[PC] << 8); // Shift left the Program Counter by 8
        PC++; // Then increment

        Cycles += 2; // It takes 2 cycles to be done

        return Data; // Then return the data
    }

    // Opcodes
    // It can be found at: http://www.obelisk.me.uk/6502/reference.html
    static constexpr Byte
            LDA_IM  = 0xA9,
            LDA_ZP  = 0xA5,
            LDA_ZPX = 0xB5,
            JMP     = 0x4C,
            JSR     = 0x20;

    // Execute the program
    void Execute( u32 Cycles, Mem& memory )
    {
        while (Cycles > 0)
        {
            // Get the Instruction to be handled
            Byte Instruction = FetchByte( Cycles, memory );

            // Switch between the instructions
            switch( Instruction )
            {
                // Load into the accumulator using the Immediate mode
                case LDA_IM:
                {
                    /* Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate. */
                    // Immediate mode

                    Byte Value = FetchByte( Cycles, memory );
                    A = Value;

                    Z = (A == 0);
                    N = (A & 0b10000000) > 0;
                } break;

                // Load into the accumulator with ZeroPage
                case LDA_ZP:
                {
                    /* Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate. */
                    // Immediate mode

                    Byte ZeroPageAddress = FetchByte( Cycles, memory );
                    A = ReadByte( Cycles, ZeroPageAddress, memory );

                    Z = (A == 0);
                    N = (A & 0b10000000) > 0;
                } break;

                // Load into the accumulator with ZeroPage in X register
                case LDA_ZPX:
                {
                    /* Loads a byte of memory into the accumulator setting the zero and negative flags as appropriate. */
                    // Immediate mode

                    Byte ZeroPageAddress = FetchByte( Cycles, memory );
                    ZeroPageAddress += X;
                    Cycles--;
                    A = ReadByte( Cycles, ZeroPageAddress, memory );

                    Z = (A == 0);
                    N = (A & 0b10000000) > 0;
                }

                // Jump between addresses
                case JMP:
                {
                    // Absolute mode

                    Word SubroutineAddress = FetchWord( Cycles, memory );
                    PC = SubroutineAddress;
                } break;

                // Jump into subroutine addresses
                case JSR:
                {
                    // Absolute mode

                    Word SubroutineAddress = FetchWord( Cycles, memory );
                    memory.WriteWord( PC - 1, SP, Cycles );
                    SP++;
                    PC = SubroutineAddress;
                    Cycles--;
                } break;

                default:
                {
                    printf("Instruction not handled: %d\n", Instruction); // If some instruction cannot be handled, then print into the console
                } break;
            }
        }
    }
};


int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset( mem );

    /* -- Start of hardcoded program -- */
    mem[0xFFFC] = CPU::LDA_IM;
    mem[0xFFFD] = 0x42;
    /* -- End of hardcoded program -- */

    cpu.Execute( 2, mem ); // Execute the program. In this case it only takes 2 cycles.

    return 0;
}