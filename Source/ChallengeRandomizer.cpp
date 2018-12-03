#include "ChallengeRandomizer.h"
#include <iostream>

int find(const std::vector<byte> &data, const std::vector<byte>& search, size_t startIndex = 0) {
	for (size_t i=startIndex; i<data.size() - search.size(); i++) {
		bool match = true;
		for (size_t j=0; j<search.size(); j++) {
			if (data[i+j] == search[j]) {
				continue;
			}
			match = false;
			break;
		}
		if (match) return static_cast<int>(i);
	}
	return -1;
}

// Reads the (relative!) address of the RNG, then shifts it to point at RNG2
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

	int do_success_side_effects = -1;
	int reveal_exit_hall = -1;
	int begin_endgame_1 = -1;

	_memory->SigScan([&](int offset, const std::vector<byte>& data) {
		// This injection ensures that the seed is set every time the challenge is started.
		// We always do this sigscan since it affects the seed.
		if (do_success_side_effects == -1) {
			int index = find(data, {0xFF, 0xC8, 0x99, 0x2B, 0xC2, 0xD1, 0xF8, 0x8B, 0xD0});
			if (index != -1) {
				do_success_side_effects = offset + index + 0x3E;
				if (GLOBALS == 0x62A080) do_success_side_effects += 4; // There's an extra 4 opcodes in the new version
				_memory->WriteData<byte>({do_success_side_effects}, {
					0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,			// mov ecx, [0x00000000] ;This is going to be the address of the custom RNG
					0x67, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x00,	// mov dword ptr ds:[ecx], 0x00000000 ;This is going to be the seed value
					0x48, 0x83, 0xF8, 0x02,						// cmp rax, 0x2 ;This is the short solve on the record player (which turns it off)
					0x90, 0x90, 0x90							// nop nop nop
				});
				int target = (GLOBALS + 0x30) - (do_success_side_effects + 0x6); // +6 is for the length of the line
				_memory->WriteData<int>({do_success_side_effects + 0x2}, {target});
				_memory->WriteData<int>({do_success_side_effects + 0x9}, {seed});
			}
		}

		// BLEH.
		if (reveal_exit_hall == -1) {
			int index = find(data, {0x45, 0x8B, 0xF7, 0x48, 0x8B, 0x4D});
			if (index != -1) {
				reveal_exit_hall = offset + index;
				_memory->WriteData<byte>({reveal_exit_hall + 0x15}, {0xEB});
			}
		}
		if (begin_endgame_1 == -1) {
			int index = find(data, {0x83, 0x7C, 0x01, 0xD0, 0x04});
			if (index != -1) {
				begin_endgame_1 = offset + index;
				if (GLOBALS == 0x5B28C0) { // Version differences :/
					begin_endgame_1 += 0x75;
				} else if (GLOBALS == 0x62A080) {
					begin_endgame_1 += 0x86;
				}
				_memory->WriteData<byte>({begin_endgame_1}, {0xEB});
			}
		}
	});

	if (!alreadyInjected) HandleSigScans();
}

void ChallengeRandomizer::HandleSigScans() {
	static int shuffle_integers = -1;
	static int shuffle_int = -1;
	static int cut_random_edges = -1;
	static int get_empty_decoration_slot = -1;
	static int get_empty_dot_spot = -1;
	static int add_exactly_this_many_bisection_dots = -1;
	static int make_a_shaper = -1;
	static int init_pattern_data_lotus = -1;
	static int reroll_lotus_eater_stuff = -1;
	static int do_lotus_minutes = -1;
	static int do_lotus_tenths = -1;
	static int do_lotus_eighths = -1;

	_memory->SigScan([&](int offset, const std::vector<byte>& data) {
		if (shuffle_integers == -1) {
			int index = find(data, {0x48, 0x89, 0x5C, 0x24, 0x10, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x63, 0xDA, 0x48, 0x8B, 0xF1, 0x83, 0xFB, 0x01});
			if (index != -1) {
				shuffle_integers = offset + index;
				AdjustRng(shuffle_integers + 0x23);
			}
		}
		// shuffle<int>
		if (shuffle_int == -1) {
			int index = find(data, {0x33, 0xF6, 0x48, 0x8B, 0xD9, 0x39, 0x31, 0x7E, 0x51});
			if (index != -1) {
				shuffle_int = offset + index - 0x16;
				AdjustRng(shuffle_int + 0x12);
			}
		}
		if (cut_random_edges == -1) {
			int index = find(data, {0x89, 0x44, 0x24, 0x3C, 0x33, 0xC0, 0x85, 0xC0, 0x75, 0xFA});
			if (index != -1) {
				cut_random_edges = offset + index - 0x22;
				AdjustRng(cut_random_edges + 0x5D);
			}
		}
		if (get_empty_decoration_slot == -1) {
			int index = find(data, {0x42, 0x83, 0x3C, 0x80, 0x00, 0x75, 0xDF});
			if (index != -1) {
				get_empty_decoration_slot = offset + index - 0x2D;
				AdjustRng(get_empty_decoration_slot + 0x16);
			}
		}
		if (get_empty_dot_spot == -1) {
			int index = find(data, {0xF7, 0xF3, 0x85, 0xD2, 0x74, 0xEC});
			if (index != -1) {
				get_empty_dot_spot = offset + index - 0x2E;
				AdjustRng(get_empty_dot_spot + 0x23);
			}
		}
		if (add_exactly_this_many_bisection_dots == -1) {
			int index = find(data, {0x48, 0x8B, 0xB4, 0x24, 0xB8, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xBC, 0x24, 0xB0, 0x00, 0x00, 0x00});
			if (index != -1) {
				add_exactly_this_many_bisection_dots = offset + index - 0x20;
				AdjustRng(add_exactly_this_many_bisection_dots + 0x1C);
			}
		}
		if (make_a_shaper == -1) {
			int index = find(data, {0xF7, 0xE3, 0xD1, 0xEA, 0x8D, 0x0C, 0x52});
			if (index != -1) {
				make_a_shaper = offset + index - 0x19;
				AdjustRng(make_a_shaper + 0x9);
				AdjustRng(make_a_shaper + 0x35);
				AdjustRng(make_a_shaper + 0x62);
			}
		}
		if (/*Entity_Machine_Panel::*/init_pattern_data_lotus == -1) {
			int index = find(data, {0x40, 0x55, 0x56, 0x48, 0x8D, 0x6C, 0x24, 0xB1});
			if (index != -1) {
				init_pattern_data_lotus = offset + index;
				AdjustRng(init_pattern_data_lotus + 0x433);
				AdjustRng(init_pattern_data_lotus + 0x45B);
				AdjustRng(init_pattern_data_lotus + 0x5A7);
				AdjustRng(init_pattern_data_lotus + 0x5D6);
				AdjustRng(init_pattern_data_lotus + 0x6F6);
				AdjustRng(init_pattern_data_lotus + 0xD17);
				AdjustRng(init_pattern_data_lotus + 0xFDA);
			}
		}
		if (/*Entity_Record_Player::*/reroll_lotus_eater_stuff == -1) {
			int index = find(data, {0xB8, 0xAB, 0xAA, 0xAA, 0xAA, 0x41, 0xC1, 0xE8});
			if (index != -1) {
				reroll_lotus_eater_stuff = offset + index - 0x37;
				AdjustRng(reroll_lotus_eater_stuff + 0x24);
				AdjustRng(reroll_lotus_eater_stuff + 0x6B);
			}
		}
		// These disable the random locations on timer panels, which would otherwise increment the RNG.
		if (do_lotus_minutes == -1) {
			int index = find(data, {0x0F, 0xBE, 0x6C, 0x08, 0xFF, 0x45});
			if (index != -1) {
				do_lotus_minutes = offset + index - 0x2B;
				_memory->WriteData<byte>({do_lotus_minutes + 0x43B}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
			}
		}
		if (do_lotus_tenths == -1) {
			int index = find(data, {0x00, 0x04, 0x00, 0x00, 0x41, 0x8D, 0x50, 0x09});
			if (index != -1) {
				do_lotus_tenths = offset + index - 0x61;
				_memory->WriteData<byte>({do_lotus_tenths + 0x103}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
			}
		}
		if (do_lotus_eighths == -1) {
			int index = find(data, {0x75, 0xF5, 0x0F, 0xBE, 0x44, 0x08, 0xFF});
			if (index != -1) {
				do_lotus_eighths = offset + index - 0x39;
				_memory->WriteData<byte>({do_lotus_eighths + 0x1E7}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
			}
		}
	});
}