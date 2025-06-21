#include <iostream>      
#include <vector>         
#include <algorithm>      
#include <functional>     

// Функция merge сливает два отсортированных массива в один
// Принимает:
// - first1, last1: начало и конец первого диапазона
// - first2, last2: начало и конец второго диапазона
// - out: итератор для вывода результата
// - cmp: функция сравнения (по умолчанию less - по возрастанию)

template <class It, class Out, class Compare = std::less<>>
Out merge(It first1, It last1, It first2, It last2, Out out, Compare cmp = Compare{}) {
    // Пока оба массива не пусты
    while (first1 != last1 && first2 != last2) {
        // Сравниваем элементы и берём меньший
        if (cmp(*first1, *first2)) {
            *out = *first1;  // Записываем элемент из первого массива
            ++first1;        // Переходим к следующему элементу
        }
        else {
            *out = *first2;  // Иначе берём из второго
            ++first2;
        }
        ++out;  // Перемещаемся в выходном массиве
    }

    // Дописываем оставшиеся элементы (если один массив закончился раньше)
    out = std::copy(first1, last1, out);  // Копируем остаток первого
    out = std::copy(first2, last2, out);  // Или второго

    return out;  // Возвращаем итератор на конец
}

// Сортировка слиянием (merge sort)
// Рекурсивно делит массив пополам, сортирует и сливает
template <class It, class Out, class Compare = std::less<>>
Out merge_sort(It first, It last, Out out, Compare cmp = Compare{}) {
    auto len = std::distance(first, last);  // Длина массива

    // Базовый случай: массив из 0 или 1 элемента уже отсортирован
    if (len <= 1) {
        return std::copy(first, last, out);  // Просто копируем
    }

    // Находим середину
    auto mid = std::next(first, len / 2);

    // Временные буферы для левой и правой частей
    std::vector<typename std::iterator_traits<It>::value_type> left_buf, right_buf;

    // Рекурсивно сортируем левую и правую части
    merge_sort(first, mid, std::back_inserter(left_buf), cmp);
    merge_sort(mid, last, std::back_inserter(right_buf), cmp);

    // Сливаем отсортированные части
    return merge(
        left_buf.begin(), left_buf.end(),
        right_buf.begin(), right_buf.end(),
        out, cmp
    );
}

// In-place сортировка слиянием (без дополнительной памяти)
// Сложная для понимания, использует std::rotate
template <class It, class Compare = std::less<>>
void inplace_merge_sort(It first, It last, Compare cmp = Compare{}) {
    // Лямбда для слияния двух частей
    auto inplace_merge = [&](It begin, It mid, It end) {
        if (begin == mid || mid == end) return;

        It left = begin;
        It right = mid;

        // Сравниваем и вращаем при необходимости
        while (left != right && right != end) {
            if (cmp(*right, *left)) {
                std::rotate(left, right, std::next(right));  // Магия!
                ++right;
            }
            ++left;
        }
        };

    // Рекурсивная сортировка
    auto sort = [&](auto& self, It start, It end) -> void {
        if (std::distance(start, end) <= 1) return;

        It mid = std::next(start, std::distance(start, end) / 2);
        self(self, start, mid);  // Сортируем левую часть
        self(self, mid, end);   // Сортируем правую
        inplace_merge(start, mid, end);  // Сливаем
        };

    sort(sort, first, last);  // Запускаем
}

// Пирамидальная сортировка (Heap sort)
// Строим кучу и последовательно извлекаем максимумы
template<class It, class Compare = std::less<>>
void heap_sort(It first, It last, Compare cmp = Compare{}) {
    using std::swap;

    auto heap_size = last - first;
    if (heap_size <= 1) return;

    // Функция "просеивания" (heapify)
    std::function<void(decltype(heap_size), decltype(heap_size))> heapify;
    heapify = [&first, &cmp, &heapify](auto heap_size, auto i) {
        auto largest = i;
        auto left = 2 * i + 1;
        auto right = 2 * i + 2;

        // Ищем наибольший среди текущего и детей
        if (left < heap_size && cmp(*(first + largest), *(first + left))) {
            largest = left;
        }
        if (right < heap_size&& cmp(*(first + largest), *(first + right))) {
            largest = right;
        }

        // Если нашли большего - меняем и рекурсивно heapify
        if (largest != i) {
            swap(*(first + i), *(first + largest));
            heapify(heap_size, largest);
        }
        };

    // Построение кучи (переупорядочивание массива)
    for (auto i = heap_size / 2 - 1; i >= 0; --i) {
        heapify(heap_size, i);
    }

    // Извлечение элементов из кучи
    for (auto i = heap_size - 1; i > 0; --i) {
        swap(*first, *(first + i));  // Перемещаем максимум в конец
        heapify(i, 0);               // Восстанавливаем кучу
    }
}

// Быстрая сортировка (Quick sort)
// Выбираем опорный элемент и делим массив на две части
template <class It, class Compare = std::less<>>
void quick_sort(It first, It last, Compare cmp = Compare{}) {
    // Базовый случай: массив пуст или из одного элемента
    if (first == last || std::next(first) == last) return;

    // Выбираем опорный элемент (середина)
    auto pivot = *std::next(first, std::distance(first, last) / 2);
    auto left = first;
    auto right = std::prev(last);

    // Разделение массива
    while (left <= right) {
        while (cmp(*left, pivot)) ++left;   // Ищем элемент >= pivot
        while (cmp(pivot, *right)) --right; // Ищем элемент <= pivot

        if (left <= right) {
            std::iter_swap(left, right);  // Меняем местами
            ++left;
            --right;
        }
    }

    // Рекурсивно сортируем подмассивы
    quick_sort(first, right + 1, cmp);
    quick_sort(left, last, cmp);
}

// Сортировка вставками (Insertion sort)
// Проходим по массиву и вставляем каждый элемент на своё место
template <class It, class Compare = std::less<>>
void insertion_sort(It first, It last, Compare cmp = Compare{}) {
    if (first == last) return;

    // Начинаем со второго элемента
    for (auto it = std::next(first); it != last; ++it) {
        auto key = std::move(*it);  // Запоминаем текущий элемент
        auto j = it;

        // Сдвигаем элементы больше key вправо
        while (j != first && cmp(key, *std::prev(j))) {
            *j = std::move(*std::prev(j));
            --j;
        }

        *j = std::move(key);  // Вставляем key на правильное место
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    // Тестируем merge (слияние двух отсортированных массивов)
    std::vector<int> a = { 1, 3, 5, 9 };
    std::vector<int> b = { 2, 4, 6, 7, 8 };
    std::vector<int> result;

    merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result), std::less<int>());
    std::cout << "Merge result: ";
    for (int x : result) std::cout << x << " ";
    std::cout << std::endl;

    // Тестируем merge_sort
    std::vector<int> v = { 5, 3, 1, 4, 2 };
    std::vector<int> result2;
    merge_sort(v.begin(), v.end(), std::back_inserter(result2));
    std::cout << "Merge sort: ";
    for (int x : result2) std::cout << x << " ";
    std::cout << std::endl;

    // Тестируем inplace_merge_sort
    inplace_merge_sort(v.begin(), v.end());
    std::cout << "In-place merge sort: ";
    for (int x : v) std::cout << x << " ";
    std::cout << std::endl;

    // Тестируем heap_sort
    std::vector<int> v2 = { 5, 3, 1, 4, 2 };
    heap_sort(v2.begin(), v2.end());
    std::cout << "Heap sort: ";
    for (int x : v2) std::cout << x << " ";
    std::cout << std::endl;

    // Тестируем quick_sort
    std::vector<int> v3 = { 5, 3, 1, 4, 2 };
    quick_sort(v3.begin(), v3.end());
    std::cout << "Quick sort: ";
    for (int x : v3) std::cout << x << " ";
    std::cout << std::endl;

    // Тестируем insertion_sort
    std::vector<int> v4 = { 5, 3, 1, 4, 2 };
    insertion_sort(v4.begin(), v4.end());
    std::cout << "Insertion sort: ";
    for (int x : v4) std::cout << x << " ";
    std::cout << std::endl;

    return 0;
}