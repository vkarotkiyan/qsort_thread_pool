#include "requesthandler.h"
#include <iostream>
using namespace std;

bool make_thread = true;
RequestHandler rh;

void quickSort(int *array, long left, long right) {
    if(left >= right) return;
    long left_bound = left;
    long right_bound = right;
    long middle = array[(left_bound + right_bound) / 2];
    do {
        while(array[left_bound] < middle)
            left_bound++;
        while(array[right_bound] > middle)
            right_bound--;
        if (left_bound <= right_bound) { //Меняем элементы местами
            swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);
    if (make_thread && (right_bound - left > 10000)) {
        auto f = rh.pushRequest(quickSort, array, left, right_bound);
        quickSort(array, left_bound, right);
        while (f.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
            rh.pushRequest2();
        }
    } else {
        quickSort(array, left, right_bound);
        quickSort(array, left_bound, right);
    }
}


int main(int argc, char* argv[]) {
    srand(0);
    long arr_size = 100000000;
    int* array = new int[arr_size];
    for (long i = 0; i < arr_size; i++) // инициализация массива
        array[i] = rand() % 500000;

    // многопоточный запуск
    auto start = chrono::high_resolution_clock::now(); // сохраняем время начала работы алгоритма
    quickSort(array, 0, arr_size - 1);
    auto finish = chrono::high_resolution_clock::now(); // сохраняем время конца работы алгоритма
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(finish - start);
    printf("Count of array elemets: %d \n", arr_size);
    printf("Count of hardware supported threads: %d \n", thread::hardware_concurrency());
    printf("The time (with multythreads): %lld msec\n", elapsed.count());

    for (long i = 0; i < arr_size - 1; i++) // Проверка отсортированного массива
        if (array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }

    for (long i = 0; i < arr_size; i++) // инициализация массива
        array[i] = rand() % 500000;
    
    // однопоточный запуск
    make_thread = false;
    start = chrono::high_resolution_clock::now();
    quickSort(array, 0, arr_size - 1);
    finish = chrono::high_resolution_clock::now();
    elapsed = chrono::duration_cast<chrono::milliseconds>(finish - start);
    printf("The time (without multythreads): %lld msec\n", elapsed.count());

    delete[] array;
    return 0;
}
