#include "pch.h"
#include "ChallengeRandomizer.h"
#include "Memory.h"

// Modify an opcode to use RNG2 instead of main RNG
void ChallengeRandomizer::AdjustRng(int offset) {
    int currentRng = _memory->ReadData<int>({offset}, 0x1)[0];
    _memory->WriteData<int>({offset}, {currentRng + 0x20});
}

// Overwrite the pointer for the lightmap_generator (which is unused, afaict) to point to a secondary RNG.
// Then, adjust all the RNG functions in challenge/doors to use this RNG.
ChallengeRandomizer::ChallengeRandomizer(const std::shared_ptr<Memory>& memory, int seed) : _memory(memory)
{
    RNG_ADDR = _memory->ReadData<int>({GLOBALS + 0x10}, 1)[0];
    RNG2_ADDR = _memory->ReadData<int>({GLOBALS + 0x30}, 1)[0];
    bool alreadyInjected = (RNG2_ADDR == RNG_ADDR + 4);

    if (!alreadyInjected) _memory->WriteData<int>({GLOBALS + 0x30}, {RNG_ADDR + 4});
    _memory->WriteData<int>({GLOBALS + 0x30, 0}, {seed});

    // do_success_side_effects
    _memory->AddSigScan({0xFF, 0xC8, 0x99, 0x2B, 0xC2, 0xD1, 0xF8, 0x8B, 0xD0}, [&](int index) {
        if (GLOBALS == 0x5B28C0) { // Version differences.
            index += 0x3E;
        } else if (GLOBALS == 0x62D0A0) {
            index += 0x42;
        }
        // Overwritten bytes start just after the movsxd rax, dword ptr ds:[rdi + 0x230]
        // aka test eax, eax; jle 2C; imul rcx, rax, 34
        _memory->WriteData<byte>({index}, {
            0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,            // mov ecx, [0x00000000] ;This is going to be the address of the custom RNG
            0x67, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x00,    // mov dword ptr ds:[ecx], 0x00000000 ;This is going to be the seed value
            0x48, 0x83, 0xF8, 0x02,                        // cmp rax, 0x2 ;This is the short solve on the record player (which turns it off)
            0x90, 0x90, 0x90                            // nop nop nop
        });
        int target = (GLOBALS + 0x30) - (index + 0x6); // +6 is for the length of the line
        _memory->WriteData<int>({index + 0x2}, {target});
        _memory->WriteData<int>({index + 0x9}, {seed}); // Because we're resetting seed every challenge, we need to run this injection every time.
    });

    if (!alreadyInjected) {
        // shuffle_integers
        _memory->AddSigScan({0x48, 0x89, 0x5C, 0x24, 0x10, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x63, 0xDA, 0x48, 0x8B, 0xF1, 0x83, 0xFB, 0x01}, [&](int index) {
            AdjustRng(index + 0x23);
        });
        // shuffle<int>
        _memory->AddSigScan({0x33, 0xF6, 0x48, 0x8B, 0xD9, 0x39, 0x31, 0x7E, 0x51}, [&](int index) {
            AdjustRng(index - 0x4);
        });
        // cut_random_edges
        _memory->AddSigScan({0x89, 0x44, 0x24, 0x3C, 0x33, 0xC0, 0x85, 0xC0, 0x75, 0xFA}, [&](int index) {
            AdjustRng(index + 0x3B);
        });
        // get_empty_decoration_slot
        _memory->AddSigScan({0x42, 0x83, 0x3C, 0x80, 0x00, 0x75, 0xDF}, [&](int index) {
            AdjustRng(index - 0x17);
        });
        // get_empty_dot_spot
        _memory->AddSigScan({0xF7, 0xF3, 0x85, 0xD2, 0x74, 0xEC}, [&](int index) {
            AdjustRng(index - 0xB);
        });
        // add_exactly_this_many_bisection_dots
        _memory->AddSigScan({0x48, 0x8B, 0xB4, 0x24, 0xB8, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xBC, 0x24, 0xB0, 0x00, 0x00, 0x00}, [&](int index) {
            AdjustRng(index - 0x4);
        });
        // make_a_shaper
        _memory->AddSigScan({0xF7, 0xE3, 0xD1, 0xEA, 0x8D, 0x0C, 0x52}, [&](int index) {
            AdjustRng(index - 0x10);
            AdjustRng(index + 0x1C);
            AdjustRng(index + 0x49);
        });
        // Entity_Machine_Panel::init_pattern_data_lotus
        _memory->AddSigScan({0x40, 0x55, 0x56, 0x48, 0x8D, 0x6C, 0x24, 0xB1}, [&](int index) {
            AdjustRng(index + 0x433);
            AdjustRng(index + 0x45B);
            AdjustRng(index + 0x5A7);
            AdjustRng(index + 0x5D6);
            AdjustRng(index + 0x6F6);
            AdjustRng(index + 0xD17);
            AdjustRng(index + 0xFDA);
        });
        // Entity_Record_Player::reroll_lotus_eater_stuff
        _memory->AddSigScan({0xB8, 0xAB, 0xAA, 0xAA, 0xAA, 0x41, 0xC1, 0xE8}, [&](int index) {
            AdjustRng(index - 0x13);
            AdjustRng(index + 0x34);
        });

        // These disable the random locations on timer panels, which would otherwise increment the RNG.
        // I'm writing 31 C0 (xor eax, eax), then 3 NOPs, which pretends the RNG returns 0.
        // do_lotus_minutes
        _memory->AddSigScan({0x0F, 0xBE, 0x6C, 0x08, 0xFF, 0x45}, [&](int index) {
            _memory->WriteData<byte>({index + 0x410}, {0x31, 0xC0, 0x90, 0x90, 0x90});
        });
        // do_lotus_tenths
        _memory->AddSigScan({0x00, 0x04, 0x00, 0x00, 0x41, 0x8D, 0x50, 0x09}, [&](int index) {
            _memory->WriteData<byte>({index + 0xA2}, {0x31, 0xC0, 0x90, 0x90, 0x90});
        });
        // do_lotus_eighths
        _memory->AddSigScan({0x75, 0xF5, 0x0F, 0xBE, 0x44, 0x08, 0xFF}, [&](int index) {
            _memory->WriteData<byte>({index + 0x1AE}, {0x31, 0xC0, 0x90, 0x90, 0x90});
        });
    }

    int failed = _memory->ExecuteSigScans();
    if (failed != 0) {
        std::cout << "Failed " << failed << " sigscans";
    }
}
