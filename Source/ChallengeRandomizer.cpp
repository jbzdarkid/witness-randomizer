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

void ChallengeRandomizer::AdjustRng(int offset) {
	int currentRng = _memory->ReadData<int>({offset}, 0x1)[0];
	_memory->WriteData<int>({offset}, {currentRng + 0x20});
}

ChallengeRandomizer::ChallengeRandomizer(const std::shared_ptr<Memory>& memory, int seed) : _memory(memory)
{
	int RNG_ADDR = _memory->ReadData<int>({GLOBALS + 0x10}, 1)[0];
	int RNG2_ADDR = _memory->ReadData<int>({GLOBALS + 0x30}, 1)[0];
	_memory->WriteData<int>({GLOBALS + 0x30}, {RNG_ADDR + 4});
	if (RNG2_ADDR == RNG_ADDR + 4) return; // Already applied hack

	int shuffle_integers = -1;
	int cut_random_edges = -1;
	int get_empty_decoration_slot = -1;
	int get_empty_dot_spot = -1;
	int add_exactly_this_many_bisection_dots = -1;
	int make_a_shaper = -1;
	int init_pattern_data_lotus = -1;
	int reroll_lotus_eater_stuff = -1;
	int do_lotus_minutes = -1;
	int do_lotus_eighths = -1;
	int do_success_side_effects = -1;

	for (int i=0; i<0x200000; i+=0x1000) {
		std::vector<byte> data = _memory->ReadData<byte>({i}, 0x1100);
		std::cout << data.size() << std::endl;

		if (shuffle_integers == -1) {
			int index = find(data, {0x48, 0x89, 0x5C, 0x24, 0x10, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x63, 0xDA, 0x48, 0x8B, 0xF1, 0x83, 0xFB, 0x01});
			if (index != -1) {
				shuffle_integers = i + index;
				AdjustRng(shuffle_integers + 0x23);
			}
		}
		if (cut_random_edges == -1) {
			int index = find(data, {0x8B, 0x80, 0xBC, 0x03, 0x00, 0x00, 0x89, 0x44, 0x24, 0x3C, 0x33, 0xC0});
			if (index != -1) {
				cut_random_edges = i + index - 0x1C;
				AdjustRng(cut_random_edges + 0x5D);
			}
		}
		if (get_empty_decoration_slot == -1) {
			int index = find(data, {0x57, 0x48, 0x83, 0xEC, 0x20, 0x8B, 0xB9, 0x38, 0x04});
			if (index != -1) {
				get_empty_decoration_slot = i + index - 0x5;
				AdjustRng(get_empty_decoration_slot + 0x16);
			}
		}
		if (get_empty_dot_spot == -1) {
			int index = find(data, {0xF7, 0xF3, 0x85, 0xD2, 0x74, 0xEC});
			if (index != -1) {
				get_empty_dot_spot = i + index - 0x2E;
				AdjustRng(get_empty_dot_spot + 0x23);
			}
		}
		if (add_exactly_this_many_bisection_dots == -1) {
			int index = find(data, {0x48, 0x8B, 0xB4, 0x24, 0xB8, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xBC, 0x24, 0xB0, 0x00, 0x00, 0x00});
			if (index != -1) {
				add_exactly_this_many_bisection_dots = i + index - 0x20;
				AdjustRng(add_exactly_this_many_bisection_dots + 0x1C);
			}
		}
		if (make_a_shaper == -1) {
			int index = find(data, {0xF7, 0xE3, 0xD1, 0xEA, 0x8D, 0x0C, 0x52});
			if (index != -1) {
				make_a_shaper = i + index - 0x19;
				AdjustRng(make_a_shaper + 0x9);
				AdjustRng(make_a_shaper + 0x35);
				AdjustRng(make_a_shaper + 0x62);
			}
		}
		if (/*Entity_Machine_Panel::*/init_pattern_data_lotus == -1) {
			int index = find(data, {0x40, 0x55, 0x56, 0x48, 0x8D, 0x6C, 0x24, 0xB1});
			if (index != -1) {
				init_pattern_data_lotus = i + index;
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
				reroll_lotus_eater_stuff = i + index - 0x37;
				AdjustRng(reroll_lotus_eater_stuff + 0x24);
				AdjustRng(reroll_lotus_eater_stuff + 0x6B);
			}
		}
		// These disable the random locations on timer panels, which would otherwise increment the RNG.
		if (do_lotus_minutes == -1) {
			int index = find(data, {0x0F, 0xBE, 0x6C, 0x08, 0xFF, 0x45});
			if (index != -1) {
				do_lotus_minutes = i + index - 0x2B;
				_memory->WriteData<byte>({do_lotus_minutes + 0x43B}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
				_memory->WriteData<byte>({do_lotus_minutes + 0x5B3}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
			}
		}
		if (do_lotus_eighths == -1) {
			int index = find(data, {0x75, 0xF5, 0x0F, 0xBE, 0x44, 0x08, 0xFF});
			if (index != -1) {
				do_lotus_eighths = i + index - 0x39;
				_memory->WriteData<byte>({do_lotus_eighths + 0x1E7}, {0x31, 0xC0, 0x90, 0x90, 0x90}); // xor eax, eax ;RNG returns 0
			}
		}
		// This injection ensures that the seed is set every time the challenge is started.
		if (do_success_side_effects == -1) {
			int index = find(data, {0x85, 0xC0, 0x7E, 0x2C, 0x48, 0x6B, 0xC8, 0x34});
			if (index != -1) {
				do_success_side_effects = i + index;
				_memory->WriteData<byte>({do_success_side_effects}, {
					0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00,			// mov ecx, [] ;This is going to be the address of the custom RNG
					0x67, 0xC7, 0x01, 0x00, 0x00, 0x00, 0x00,	// mov dword ptr ds:[ecx], 0x0 ;This is going to be the seed value
					0x48, 0x83, 0xF8, 0x02,						// cmp rax, 0x2 ;This is the short solve on the record player (which turns it off)
					0x90, 0x90, 0x90							// nop nop nop
				});
				int target = (GLOBALS + 0x30) - (do_success_side_effects + 0x6); // +6 is for the length of the line
				_memory->WriteData<int>({do_success_side_effects + 0x2}, {target});
				_memory->WriteData<int>({do_success_side_effects + 0x9}, {seed});
			}
		}
	}
}
