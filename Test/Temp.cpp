#include "gtest/gtest.h"
#include "Randomizer.h"
#include <iostream>
#include <fstream>

class Temp : public testing::Test
{
protected:
    std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe");
    // std::vector<char> ReadSubtitles(int size) {
    //      Memory memory("witness64_d3d11.exe");
    //      std::vector<char> data;
    //      data.resize(size);
    //      ReadProcessMemory(memory._handle, (LPVOID)0x3D89F000, &data[0], sizeof(char) * size, nullptr);
    //      return data;
    // }
};

// TEST_F(Temp, Shipwreck) {
//     Randomizer randomizer(_memory);
//     int shipwreck = 0xAFB;
//     int thEntry = 0x288C;
//     int si1 = 0x00000022;
//     int bu1 = 0x6;
//     int td1 = 0x5D;
//     int ypp = 0x33EA;
//     int ramp_activation_shapers = 0x21D5;
//     int mill_upper_5 = 0x146C;
//     int mill_entry_left = 0x1E5A;
//     int mill_upper_7 = 0x03686;
// 
//     randomizer.SwapPanels(ypp, mill_upper_7, Randomizer::SWAP::LINES);
// }

/*
TEST_F(Temp, Extract) {
//    std::vector<byte> data = ReadSubtitles(166480);
    std::vector<char> data = ReadSubtitles(166480);
    std::ofstream file("raw.txt");
    ASSERT_TRUE(file.is_open());

    std::string hex = "0123456789ABCDEF";
    for (int i=0; i<data.size(); i++) {
        if (data[i] == '\r') continue;
        file << data[i];

    }
    file.close();
}
*/