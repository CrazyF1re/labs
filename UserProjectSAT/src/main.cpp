// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stack>
#include <memory>
#include <algorithm>
#include <chrono> 
#include "NodeBoolTree.h"
#include "boolinterval.h"
#include "boolequation.h"
#include "BBV.h"
#include <Allocator.h>

// cmake -DUSE_CUSTOM_ALLOCATOR=ON -B build 
// cmake --build build
// /home/aevameow/SAT_DPLL/build/lab2 $(pwd)/../SatExamples/Sat_ex12_3.pla
// find src Allocator -name "*.cpp" -o -name "*.h" | xargs clang-tidy -p .
// для тестов -
// cmake -DUSE_CUSTOM_ALLOCATOR=ON -DENABLE_ASAN=ON -DENABLE_UBSAN=ON  -B build 

// Создаем специализированные аллокаторы для разных типов классов
// Каждый аллокатор оптимизирован под размер конкретного класса
Allocator equationAllocator(sizeof(BoolEquation), 1000, NULL, "EquationAllocator");  // Для BoolEquation
Allocator nodeAllocator(sizeof(NodeBoolTree), 2000, NULL, "NodeAllocator");          // Для NodeBoolTree
Allocator intervalAllocator(sizeof(BoolInterval), 500, NULL, "IntervalAllocator");   // Для BoolInterval
Allocator bbvAllocator(sizeof(BBV), 1000, NULL, "BBVAllocator");                     // Для BBV

// Функция для удаления символов переноса строки
std::string trimNewlines(const std::string& str) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

// Функция для удаления лидирующих и замыкающих пробелов
std::string trim(const std::string& str) {
    // Создаем итератор, указывающий на начало строки
    auto start = str.begin();
    
    // Пропускаем все лидирующие пробелы
    while (start != str.end() && std::isspace(*start)) {
        start++;  // Сдвигаем итератор вправо, пока не найдем первый ненулевой символ
    }

    // Создаем итератор, указывающий на конец строки
    auto end = str.end();
    
    // Пропускаем все замыкающие пробелы
    do {
        end--;  // Сдвигаем итератор влево, пока не найдем первый ненулевой символ с конца
    } while (std::distance(start, end) > 0 && std::isspace(*end));  // Убедимся, что итераторы не пересеклись и символ является пробельным

    // Возвращаем подстроку, которая начинается с первого ненулевого символа и заканчивается на последнем
    return std::string(start, end + 1);  // end + 1, так как end указывает на символ за последним ненулевым символом
}
// Вспомогательные функции для работы с пользовательскими аллокаторами
#ifdef USE_CUSTOM_ALLOCATOR
// Специализированные функции для разных типов классов с соответствующими аллокаторами
BoolEquation* allocateEquation(BoolInterval** cnf, BoolInterval* root, int cnfSize, int count, BBV mask, std::shared_ptr<BranchingStrategy> strategy) {
    void* memory = equationAllocator.Allocate(sizeof(BoolEquation));
    return new(memory) BoolEquation(cnf, root, cnfSize, count, mask, strategy);
}

BoolEquation* allocateEquationCopy(BoolEquation& equation) {
    void* memory = equationAllocator.Allocate(sizeof(BoolEquation));
    return new(memory) BoolEquation(equation);
}

NodeBoolTree* allocateNode(BoolEquation* eq) {
    void* memory = nodeAllocator.Allocate(sizeof(NodeBoolTree));
    return new(memory) NodeBoolTree(eq);
}

BoolInterval* allocateInterval(BBV& vec, BBV& dnc) {
    void* memory = intervalAllocator.Allocate(sizeof(BoolInterval));
    return new(memory) BoolInterval(vec, dnc);
}

BoolInterval* allocateInterval(const char* str) {
    void* memory = intervalAllocator.Allocate(sizeof(BoolInterval));
    return new(memory) BoolInterval(str);
}

BBV* allocateBBV(const char* str) {
    void* memory = bbvAllocator.Allocate(sizeof(BBV));
    return new(memory) BBV(str);
}

// Функции для деаллокации разных типов объектов
void deallocateNode(NodeBoolTree* node) {
    if (node) {
        // Вызываем деструктор явно
        node->~NodeBoolTree();
        // Освобождаем память через соответствующий аллокатор
        nodeAllocator.Deallocate(node);
    }
}

void deallocateEquation(BoolEquation* eq) {
    if (eq) {
        // Вызываем деструктор явно
        eq->~BoolEquation();
        // Освобождаем память через соответствующий аллокатор
        equationAllocator.Deallocate(eq);
    }
}

void deallocateInterval(BoolInterval* interval) {
    if (interval) {
        interval->~BoolInterval();
        intervalAllocator.Deallocate(interval);
    }
}

void deallocateBBV(BBV* bbv) {
    if (bbv) {
        bbv->~BBV();
        bbvAllocator.Deallocate(bbv);
    }
}
#endif

int main(int argc, char *argv[])
{
    std::vector<std::string> full_file_list;
    std::string filepath;
    
    // Настройка стратегии ветвления (по умолчанию - по столбцам)
    std::shared_ptr<BranchingStrategy> strategy = std::make_shared<strategiaColumn>();
    
    // Разбор аргументов командной строки
    if (argc > 1) {
        filepath = argv[1];
        
        if (argc > 2) {
            std::string strategyArg = argv[2];
            
        }
    } else {
        // Hardcode input если нет аргументов
        filepath = "../data/Sat_ex11_3.pla";
    }
    
    #ifdef USE_CUSTOM_ALLOCATOR
    std::cout << "Используются пользовательские аллокаторы для разных типов классов\n";
    #else
    std::cout << "Используется стандартный аллокатор\n";
    #endif
    
    std::ifstream file(filepath);

    // Для измерения времени
    auto start_time = std::chrono::high_resolution_clock::now();

    // Считываем весь файл
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            full_file_list.push_back(trimNewlines(line));
        }
        file.close();

        int cnfSize = full_file_list.size(); // Получаем количество строк, считанных из файла
        BoolInterval **CNF = new BoolInterval*[cnfSize]; // Выделяем массив указателей на объекты BoolInterval (по количеству строк)
        int rangInterval = -1; // Инициализируем переменную, которая будет хранить длину первой строки (по умолчанию -1, как ошибка)

        if (cnfSize) {
            rangInterval = trim(full_file_list[0]).length();
        }

        for (int i = 0; i < cnfSize; i++) { // Заполняем массив
            std::string strv = trim(full_file_list[i]);
            #ifdef USE_CUSTOM_ALLOCATOR
            CNF[i] = allocateInterval(strv.c_str());
            #else
            CNF[i] = new BoolInterval(strv.c_str());
            #endif
        }

        std::string rootvec = ""; // Строка для хранения вектора корня 
        std::string rootdnc = ""; // Строка для хранения компонента корня 

        // Строим интервал в котором все компоненты принимают значение '-',
        // который представляет собой корень уравнения, пока пустой.
        // В процессе поиска корня, компоненты интервала будут заменены на конкретные значения.

        for (int i = 0; i < rangInterval; i++) {
            rootvec += "0";
            rootdnc += "1";
        }

        #ifdef USE_CUSTOM_ALLOCATOR
        BBV* vec_ptr = allocateBBV(rootvec.c_str());
        BBV* dnc_ptr = allocateBBV(rootdnc.c_str());
        BBV& vec = *vec_ptr;
        BBV& dnc = *dnc_ptr;
        
        // Создаем пустой корень уравнения
        BoolInterval *root = allocateInterval(vec, dnc);
        #else
        BBV vec(rootvec.c_str());  // Создаем объект BBV для представления вектора значений корня, инициализируем его строкой rootvec
        BBV dnc(rootdnc.c_str());  // Создаем второй объект BBV для представления компонента корня, инициализируем его строкой rootdnc
        
        // Создаем пустой корень уравнения
        BoolInterval *root = new BoolInterval(vec, dnc);
        #endif
        // Алгоритм поиска корня. Работаем всегда с верхушкой стека.
		// Шаг 1. Правила выполняются? Нет - Ветвление Шаг 5. Да - Упрощаем Шаг 2.
		// Шаг 2. Строки закончились? Нет - Шаг1, Да - Корень найден? Да - Успех КОНЕЦ, Нет - Шаг 3.
		// Шаг 3. Кол-во узлов в стеке > 1? Нет - Корня нет КОНЕЦ, Да - Шаг 4.
		// Шаг 4. Текущий узел выталкиваем из стека, попадаем в новый узел. У нового узла lt rt отличны от NULL? Нет - Шаг 1. Да - Шаг 3.
		// Шаг 5. Выбор компоненты ветвления, создание двух новых узлов, добавление их в стек сначала с 1 потом с 0. Шаг 1.

		// Алгоритм CheckRules.
		// Цикл по строкам КНФ.
		// 1. Проверка правила 2. Выполнилось? Да - Корня нет, Нет - Идем дальше.
		// 2. Проверка правила 1. Выполнилось? Да - Упрощаем, Нет - Идем дальше.

        BoolEquation *boolequation;
        NodeBoolTree *startNode;
        
        #ifdef USE_CUSTOM_ALLOCATOR
        // Используем специализированные аллокаторы
        boolequation = allocateEquation(CNF, root, cnfSize, cnfSize, vec, strategy);
        startNode = allocateNode(boolequation);
        #else
        // Используем стандартный new
        boolequation = new BoolEquation(CNF, root, cnfSize, cnfSize, vec, strategy);
        startNode = new NodeBoolTree(boolequation);
        #endif

        // Алгоритм поиска корня. Работаем всегда с верхушкой стека.
        bool rootIsFinded = false; // Флаг, который будет указывать, найден ли корень уравнения
        std::stack<NodeBoolTree *> BoolTree; // Стек для хранения узлов булевого дерева
        BoolTree.push(startNode); // Добавляем начальный узел в стек (первоначальный узел дерева)

        do {
            NodeBoolTree *currentNode(BoolTree.top());

            if (currentNode->lt == nullptr && currentNode->rt == nullptr) { 
                // Если вернулись в обработанный узел
                BoolEquation *currentEquation = currentNode->eq;
                bool flag = true;

                // Цикл для упрощения по правилам.
                while (flag) {
                    int a = currentEquation->CheckRules(); // Проверка выполнения правил

                    switch (a) {
                        case 0: { // Корня нет.
                            BoolTree.pop();
                            flag = false;
                            break;
                        }

                        case 1: { // Правило выполнилось, корень найден или продолжаем упрощать.
                            if (currentEquation->count == 0 ||
                                    currentEquation->mask.getWeight() ==
                                    currentEquation->mask.getSize()) { // Если кончились строки или столбцы, корень найден.
                                flag = false;
                                rootIsFinded = true; // Полагаем, что корень найден, выполняем проверку корня

                                for (int i = 0; i < cnfSize; i++) {
                                    if (!CNF[i]->isEqualComponent(*currentEquation->root)) {
                                        rootIsFinded = false; // Корень не найден. Продолжаем искать дальше.
                                        BoolTree.pop();
                                        break;
                                    }
                                }
                            }
                            break;
                        }

                        case 2: { // Правила не выполнились, ветвление
                            // Теперь используем общий метод для выбора индекса ветвления
                            int indexBranching = currentEquation->ChooseBranchingIndex();

                            // Создаем новые уравнения и узлы в зависимости от типа аллокации
                            BoolEquation *Equation0, *Equation1;
                            NodeBoolTree *Node0, *Node1;
                            
                            #ifdef USE_CUSTOM_ALLOCATOR
                            // Используем специализированные аллокаторы
                            Equation0 = allocateEquationCopy(*currentEquation);
                            Equation1 = allocateEquationCopy(*currentEquation);
                            
                            Equation0->Simplify(indexBranching, '0');
                            Equation1->Simplify(indexBranching, '1');
                            
                            Node0 = allocateNode(Equation0);
                            Node1 = allocateNode(Equation1);
                            #else
                            // Используем стандартный new
                            Equation0 = new BoolEquation(*currentEquation);
                            Equation1 = new BoolEquation(*currentEquation);
                            
                            Equation0->Simplify(indexBranching, '0');
                            Equation1->Simplify(indexBranching, '1');
                            
                            Node0 = new NodeBoolTree(Equation0);
                            Node1 = new NodeBoolTree(Equation1);
                            #endif

                            currentNode->lt = Node0;
                            currentNode->rt = Node1;

                            BoolTree.push(Node1);
                            BoolTree.push(Node0);

                            flag = false;
                            break;
                        }
                    }
                }
            } else {
                BoolTree.pop();
            }

        } while (BoolTree.size() > 1 && !rootIsFinded);

        // Измеряем время выполнения
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

        if (rootIsFinded) {
            std::cout << "Root is:\n ";
            BoolInterval *finded_root = BoolTree.top()->eq->root;
            std::cout << std::string(*finded_root) << std::endl;
        } else {
            std::cout << "Root is not exists!" << std::endl;
        }

        std::cout << "Время выполнения: " << duration << " мкс" << std::endl;

        // Выводим статистику по аллокаторам
        #ifdef USE_CUSTOM_ALLOCATOR
        std::cout << "\n===== Статистика аллокаторов =====\n";
        std::cout << "Аллокатор для BoolEquation:\n";
        std::cout << "  Размер блока: " << equationAllocator.GetBlockSize() << " байт\n";
        std::cout << "  Всего блоков: " << equationAllocator.GetBlockCount() << "\n";
        std::cout << "  Блоков в использовании: " << equationAllocator.GetBlocksInUse() << "\n";
        std::cout << "  Аллокаций: " << equationAllocator.GetAllocations() << "\n";
        std::cout << "  Деаллокаций: " << equationAllocator.GetDeallocations() << "\n";
        
        std::cout << "\nАллокатор для NodeBoolTree:\n";
        std::cout << "  Размер блока: " << nodeAllocator.GetBlockSize() << " байт\n";
        std::cout << "  Всего блоков: " << nodeAllocator.GetBlockCount() << "\n";
        std::cout << "  Блоков в использовании: " << nodeAllocator.GetBlocksInUse() << "\n";
        std::cout << "  Аллокаций: " << nodeAllocator.GetAllocations() << "\n";
        std::cout << "  Деаллокаций: " << nodeAllocator.GetDeallocations() << "\n";

        std::cout << "\nАллокатор для BoolInterval:\n";
        std::cout << "  Размер блока: " << intervalAllocator.GetBlockSize() << " байт\n";
        std::cout << "  Всего блоков: " << intervalAllocator.GetBlockCount() << "\n";
        std::cout << "  Блоков в использовании: " << intervalAllocator.GetBlocksInUse() << "\n";
        std::cout << "  Аллокаций: " << intervalAllocator.GetAllocations() << "\n";
        std::cout << "  Деаллокаций: " << intervalAllocator.GetDeallocations() << "\n";

        std::cout << "\nАллокатор для BBV:\n";
        std::cout << "  Размер блока: " << bbvAllocator.GetBlockSize() << " байт\n";
        std::cout << "  Всего блоков: " << bbvAllocator.GetBlockCount() << "\n";
        std::cout << "  Блоков в использовании: " << bbvAllocator.GetBlocksInUse() << "\n";
        std::cout << "  Аллокаций: " << bbvAllocator.GetAllocations() << "\n";
        std::cout << "  Деаллокаций: " << bbvAllocator.GetDeallocations() << "\n";

        #endif

        // Освобождаем память
        #ifdef USE_CUSTOM_ALLOCATOR
        // При использовании пользовательских аллокаторов освобождаем память вручную
        while (!BoolTree.empty()) {
            NodeBoolTree* node = BoolTree.top();
            BoolTree.pop();
            
            // Освобождаем память для уравнения
            if (node->eq) {
                deallocateEquation(node->eq);
            }
            
            // Освобождаем память для узла
            deallocateNode(node);
        }
        
        // Освобождаем память для CNF
        for (int i = 0; i < cnfSize; i++) {
            deallocateInterval(CNF[i]);
        }
        
        // Освобождаем память для root и BBV
        deallocateInterval(root);
        deallocateBBV(vec_ptr);
        deallocateBBV(dnc_ptr);
        #else
        // При использовании стандартного аллокатора нужно явно освободить всю память
        while (!BoolTree.empty()) {
            NodeBoolTree* node = BoolTree.top();
            BoolTree.pop();
            delete node->eq;
            delete node;
        }
        
        // Освобождаем память для CNF и root
        for (int i = 0; i < cnfSize; i++) {
            delete CNF[i];
        }
        delete root;
        #endif
        
        // В обоих случаях освобождаем массив указателей
        delete[] CNF;

    } else {
        std::cout << "File does not exists.\n";
    }

    return 0;
}