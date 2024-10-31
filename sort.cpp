// BubbleSort.cpp
#include "BubbleSort.h"

void BubbleSort::sort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1]) {
                // 交换元素
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}