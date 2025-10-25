 #ifndef BRANCHING_STRATEGY_H
 #define BRANCHING_STRATEGY_H
 
 #include "boolinterval.h"
 #include <vector>
 
 // Абстрактный класс стратегии ветвления
 class BranchingStrategy {
 public:
     virtual ~BranchingStrategy() = default;
     
     // Унифицируем сигнатуру (используем BBV& вместо const BBV&)
     virtual int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const = 0;
 };
 
 // Стратегия ветвления по столбцам
 class strategiaColumn : public BranchingStrategy {
 public:
     // Исправляем сигнатуру - должна точно совпадать с базовым классом
     int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const override;
 };
 
 // Стратегия ветвления по строкам
 class strategiaRow : public BranchingStrategy {
 public:
     // Исправляем сигнатуру - должна точно совпадать с базовым классом
     int chooseBranchingIndex(BoolInterval** cnf, int cnfSize, BBV& mask) const override;
 };
 
 #endif // BRANCHING_STRATEGY_H