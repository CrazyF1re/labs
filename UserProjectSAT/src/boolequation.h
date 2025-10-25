#ifndef BOOLEQUATION_H
#define BOOLEQUATION_H

#include "boolinterval.h"
#include "BBV.h"
#include "strategia.h"
#include <memory>

class BoolEquation {
public:
    BoolInterval **cnf;
    BoolInterval *root;
    int cnfSize;
    int count;
    BBV mask;
    int forcedBranchColumn;
    std::shared_ptr<BranchingStrategy> branchingStrategy; // Стратегия ветвления

    BoolEquation(BoolInterval **cnf, BoolInterval *root, int cnfSize, int count, BBV mask, std::shared_ptr<BranchingStrategy> strategy);
    BoolEquation(BoolEquation &equation);

    int CheckRules();
    // Метод для выбора индекса ветвления с использованием текущей стратегии
    int ChooseBranchingIndex();
    // Метод для изменения стратегии ветвления
    void SetBranchingStrategy(std::shared_ptr<BranchingStrategy> strategy);
    void Simplify(int ixCol, char value);

private:
    bool Rule2RowNull(BoolInterval *interval);
    bool Rule1Row1(BoolInterval *interval);
    void Rule3ColNull(BBV vector);
    bool Rule4Col0(BBV vector);
    bool Rule5Col1(BBV vector);
};

#endif // BOOLEQUATION_H
