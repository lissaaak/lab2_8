#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

// Функция merge объединяет два отсортированных диапазона в один отсортированный
// first1, last1 - итераторы первого диапазона
// first2, last2 - итераторы второго диапазона
// out - итератор на выходной диапазон
// cmp - компаратор для сравнения элементов (по умолчанию less - "<")
template <class It, class Out, class Compare=std::less<>>
Out merge(It first1, It last1, It first2, It last2, Out out, Compare cmp=Compare{}){
    // Пока оба диапазона не пусты
    while (first1 != last1 && first2 != last2){
        // Сравниваем элементы из обоих диапазонов
        if (cmp(*first1, *first2)) {
            // Если элемент из первого диапазона меньше, записываем его
            *out = *first1;
            ++first1;
        } else {
            // Иначе записываем элемент из второго диапазона
            *out = *first2;
            ++first2;
        }
        ++out;
    }
    // Дописываем оставшиеся элементы из первого или второго диапазона
    out = std::copy(first1, last1, out);
    out = std::copy(first2, last2, out);

    return out;
}

// Реализация сортировки слиянием
// first, last - итераторы сортируемого диапазона
// out - итератор на выходной диапазон
// cmp - компаратор
template <class It, class Out, class Compare=std::less<>>
Out merge_sort(It first, It last, Out out, Compare cmp=Compare{}){
    // Вычисляем длину диапазона
    auto len = std::distance(first, last);

    // Базовый случай рекурсии - диапазон из 0 или 1 элемента
    if (len <= 1) {
        return std::copy(first, last, out);
    }

    // Находим середину диапазона
    auto mid = std::next(first, len / 2);
    // Создаем буферы для левой и правой частей
    std::vector<typename std::iterator_traits<It>::value_type> left_buf, right_buf;

    // Рекурсивно сортируем левую и правую части
    merge_sort(first, mid, std::back_inserter(left_buf), cmp);
    merge_sort(mid, last, std::back_inserter(right_buf), cmp);

    // Объединяем отсортированные части
    return merge(
            left_buf.begin(), left_buf.end(),
            right_buf.begin(), right_buf.end(),
            out, cmp
    );
}

// Реализация сортировки слиянием без дополнительной памяти (in-place)
template <class It, class Compare = std::less<>>
void inplace_merge_sort(It first, It last, Compare cmp = Compare{}) {
    // Лямбда для in-place слияния двух отсортированных частей
    auto inplace_merge = [&](It begin, It mid, It end) {
        if (begin == mid || mid == end) return;

        It left = begin;
        It right = mid;

        // Слияние с использованием rotate
        while (left != right && right != end) {
            if (cmp(*right, *left)) {
                std::rotate(left, right, std::next(right));
                ++right;
            }
            ++left;
        }
    };

    // Рекурсивная лямбда для сортировки
    auto sort = [&](auto& self, It start, It end) -> void {
        if (std::distance(start, end) <= 1)
            return;

        It mid = std::next(start, std::distance(start, end) / 2);
        // Сортируем левую и правую части
        self(self, start, mid);
        self(self, mid, end);
        // Сливаем их
        inplace_merge(start, mid, end);
    };

    sort(sort, first, last);
}

// Реализация сортировки кучей (пирамидальная сортировка)
template<class It, class Compare = std::less<>>
void heap_sort(It first, It last, Compare cmp = Compare{}) {
    using std::swap;

    auto heap_size = last - first;
    if (heap_size <= 1) return;
    
    // Функция для восстановления свойств кучи
    std::function<void(decltype(heap_size), decltype(heap_size))> heapify;
    heapify = [&first, &cmp, &heapify](auto heap_size, auto i) {
        auto largest = i;    // Индекс корня
        auto left = 2 * i + 1;  // Левый потомок
        auto right = 2 * i + 2; // Правый потомок

        // Если левый потомок больше корня
        if (left < heap_size && cmp(*(first + largest), *(first + left))) {
            largest = left;
        }

        // Если правый потомок больше текущего наибольшего
        if (right < heap_size && cmp(*(first + largest), *(first + right))) {
            largest = right;
        }

        // Если наибольший элемент не корень
        if (largest != i) {
            swap(*(first + i), *(first + largest));
            // Рекурсивно heapify поддерево
            heapify(heap_size, largest);
        }
    };

    // Построение кучи (перегруппируем массив)
    for (auto i = heap_size / 2 - 1; i >= 0; --i) {
        heapify(heap_size, i);
    }

    // Один за другим извлекаем элементы из кучи
    for (auto i = heap_size - 1; i > 0; --i) {
        // Перемещаем текущий корень в конец
        swap(*first, *(first + i));
        // Вызываем heapify на уменьшенной куче
        heapify(i, 0);
    }
}

// Реализация быстрой сортировки
template <class It, class Compare = std::less<>>
void quick_sort(It first, It last, Compare cmp = Compare{}) {
    // Базовый случай - пустой диапазон или 1 элемент
    if (first == last || std::next(first) == last) {
        return;
    }

    // Выбираем опорный элемент (середина диапазона)
    auto pivot = *std::next(first, std::distance(first, last) / 2);
    auto left = first;
    auto right = std::prev(last);

    // Разделение элементов относительно опорного
    while (left <= right) {
        while (cmp(*left, pivot)) {
            ++left;
        }
        while (cmp(pivot, *right)) {
            --right;
        }

        if (left <= right) {
            std::iter_swap(left, right);
            ++left;
            --right;
        }
    }

    // Рекурсивно сортируем поддиапазоны
    quick_sort(first, right + 1, cmp);
    quick_sort(left, last, cmp);
}

// Реализация сортировки вставками
template <class It, class Compare = std::less<>>
void insertion_sort(It first, It last, Compare cmp = Compare{}) {
    if (first == last) {
        return;
    }

    // Проходим по всем элементам, начиная со второго
    for (auto it = std::next(first); it != last; ++it) {
        auto key = std::move(*it);  // Текущий элемент
        auto j = it;

        // Сдвигаем элементы больше key вправо
        while (j != first && cmp(key, *std::prev(j))) {
            *j = std::move(*std::prev(j));
            --j;
        }

        // Вставляем key в правильную позицию
        *j = std::move(key);
    }
}

int main() {
    // Тестирование функции merge
    std::vector<int> a = {1, 3, 5, 9};
    std::vector<int> b = {2, 4, 6, 7, 8};
    std::vector<int> result;

    merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(result), std::less());
    for (int x: result) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Тестирование merge_sort
    std::vector<int> v = {5, 3, 1, 4, 2};
    std::vector<int> result2;
    merge_sort(v.begin(), v.end(), std::back_inserter(result2));
    for (int x: result2) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Тестирование inplace_merge_sort
    inplace_merge_sort(v.begin(), v.end());
    for (int x: v) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Тестирование heap_sort
    std::vector<int> v2 = {5, 3, 1, 4, 2};
    heap_sort(v2.begin(), v2.end());
    for (int x: v2) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Тестирование quick_sort
    std::vector<int> v3 = {5, 3, 1, 4, 2};
    quick_sort(v3.begin(), v3.end());
    for (int x: v3) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Тестирование insertion_sort
    std::vector<int> v4 = {5, 3, 1, 4, 2};
    insertion_sort(v4.begin(), v4.end());
    for (int x: v4) {
        std::cout << x << " ";
    }
}
