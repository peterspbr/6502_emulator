# 6502_emulator
This is a (not yet) accurate 6502 emulator for a Commodore64 emulator project.

## Why to make a Commodore64 emulator from zero?
I'm at my university vacations and why not? This is a good way to become a better programmer. When you are making the code for emulating a bare metal process and learning about machine code and whatever, it makes you feel better and you can understand what you are making while coding.
Some programmers just know how to make the things works, but if i ask him: "What does this printf() function actualy runs on the machine?", and his answer will be really poor or inexistent, that's the bad programmer.

## Todo
### More accurate reset process
According to the C64 Wiki the Commodore64 reset process is like as follows:
```asm
; MOS 6510 System Reset routine[3]
; Reset vector (Kernal address $FFFC) points here.
; 
; If cartridge is detected then cartridge cold start routine is activated.
; If no cartridge is detected then I/O and memory are initialised and BASIC cold start routine is activated.

FCE2   A2 FF      LDX #$FF        ; 
FCE4   78         SEI             ; set interrupt disable
FCE5   9A         TXS             ; transfer .X to stack
FCE6   D8         CLD             ; clear decimal flag
FCE7   20 02 FD   JSR $FD02       ; check for cart
FCEA   D0 03      BNE $FCEF       ; .Z=0? then no cart detected
FCEC   6C 00 80   JMP ($8000)     ; direct to cartridge cold start via vector
FCEF   8E 16 D0   STX $D016       ; sets bit 5 (MCM) off, bit 3 (38 cols) off
FCF2   20 A3 FD   JSR $FDA3       ; initialise I/O
FCF5   20 50 FD   JSR $FD50       ; initialise memory
FCF8   20 15 FD   JSR $FD15       ; set I/O vectors ($0314..$0333) to kernal defaults
FCFB   20 5B FF   JSR $FF5B       ; more initialising... mostly set system IRQ to correct value and start
FCFE   58         CLI             ; clear interrupt flag
FCFF   6C 00 A0   JMP ($A000)     ; direct to BASIC cold start via vector
```
I want to make exactly how it is, but my processor is missing a lot of instructions, like ```STX```, ```CLI```, ```SEI```, ```TXS```, etc.
---

I need to emulate cartridges, video output, audio output, kernel loading, basic loading, character set loading... hmmm... a lot of things.
