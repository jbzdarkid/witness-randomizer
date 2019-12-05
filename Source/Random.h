#pragma once

class Random
{
public:
    static void SetSeed(int seed);
    static int RandInt(int min, int max);

    template <typename T>
    static std::vector<T> SelectFromSet(std::vector<T> set, size_t count) {
        size_t setSize = set.size();
        assert(count < setSize);
        std::vector<T> selection;
        for (int i=0; i<count && i<setSize; i++) {
            int index = Random::RandInt(0, static_cast<int>(setSize - 1));
            selection.emplace_back(set[index]);
            set[index] = set[setSize-1];
            setSize--;
        }
        return selection;
    }

private:
    static uint32_t s_seed;
};
