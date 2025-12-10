#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
using namespace std;

const int MaxRec = 30; // Количество записей в массиве
const int DaysInMonth[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 }; // Календарь для проверки дат

struct Expense
{
    unsigned int Id;   // числовой ключ
    string Category;   // краткая категория
    string Note;       // короткое описание
    unsigned int Day;
    unsigned int Month;
    unsigned int Year;
    double Amount;     // сумма > 0
    bool Deleted;      // пометка удаления
};

struct TreeNode
{
    string Key;      // ключ сортировки (категория)
    int RecIndex;    // номер записи в массиве
    TreeNode *Left;
    TreeNode *Right;
    TreeNode *Next;  // цепочка для одинаковых ключей
};

Expense ExpArr[MaxRec];
int RecCount = 0;

TreeNode *Root = NULL;

void ClearInput()
{
    cin.clear();
    while (cin && cin.get() != '\n') {}
    printf("Неверный ввод\n");
}

bool ReadUInt(const char *prompt, unsigned int &val)
{
    string tmp;
    while (true)
    {
        printf("%s (или # - выход): ", prompt);
        if (!(cin >> tmp))
        {
            ClearInput();
            continue;
        }
        if (tmp == "#")
        {
            printf("Отмена\n");
            return false;
        }
        try
        {
            val = static_cast<unsigned int>(stoul(tmp));
            return true;
        }
        catch (...)
        {
            printf("Неверный ввод\n");
        }
    }
}

bool ReadDouble(const char *prompt, double &val)
{
    string tmp;
    while (true)
    {
        printf("%s (или # - выход): ", prompt);
        if (!(cin >> tmp))
        {
            ClearInput();
            continue;
        }
        if (tmp == "#")
        {
            printf("Отмена\n");
            return false;
        }
        try
        {
            val = stod(tmp);
            return true;
        }
        catch (...)
        {
            printf("Неверный ввод\n");
        }
    }
}

bool ReadPosDouble(const char *prompt, double &val)
{
    while (true)
    {
        if (!ReadDouble(prompt, val)) return false;
        if (val > 0) return true;
        printf("Значение должно быть > 0\n");
    }
}

bool ReadString(const char *prompt, string &val)
{
    while (true)
    {
        printf("%s (или # - выход): ", prompt);
        getline(cin >> ws, val);
        if (val == "#")
        {
            printf("Отмена\n");
            return false;
        }
        if (val.find(';') != string::npos || val.find(' ') != string::npos)
        {
            printf("Символ ';' и пробелы запрещены\n");
            continue;
        }
        return true;
    }
}

bool ReadDate(unsigned int &y, unsigned int &m, unsigned int &d)
{
    if (!ReadUInt("Год", y)) return false;
    while (true)
    {
        if (!ReadUInt("Месяц", m)) return false;
        if ((m >= 1) && (m <= 12)) break;
        printf("Месяц должен быть 1..12\n");
    }
    while (true)
    {
        if (!ReadUInt("День", d)) return false;
        int maxd = DaysInMonth[m];
        if ((d >= 1) && (d <= static_cast<unsigned int>(maxd))) break;
        printf("В этом месяце %d дней\n", maxd);
    }
    return true;
}

// Работа с деревом
TreeNode* MakeNode(string key, int recIndex)
{
    TreeNode *p = new TreeNode;
    p->Key = key;
    p->RecIndex = recIndex;
    p->Left = NULL;
    p->Right = NULL;
    p->Next = NULL;
    return p;
}

void InsertNode(TreeNode* &root, string key, int recIndex)
{
    if (root == NULL)
    {
        root = MakeNode(key, recIndex);
        return;
    }
    if (key < root->Key)
    {
        InsertNode(root->Left, key, recIndex);
    }
    else if (key > root->Key)
    {
        InsertNode(root->Right, key, recIndex);
    }
    else
    {
        TreeNode *t = root;
        while (t->Next != NULL)
        {
            t = t->Next;
        }
        t->Next = MakeNode(key, recIndex);
    }
}

void FreeTree(TreeNode* node)
{
    if (node == NULL) return;
    FreeTree(node->Left);
    FreeTree(node->Right);
    TreeNode *dup = node->Next;
    while (dup != NULL)
    {
        TreeNode *tmp = dup->Next;
        delete dup;
        dup = tmp;
    }
    delete node;
}

void BuildTree()
{
    FreeTree(Root);
    Root = NULL;
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            InsertNode(Root, ExpArr[i].Category, i);
        }
    }
}

void PrintOne(Expense E, int Num)
{
    printf("%d) Id: %u; Категория: %s; Описание: %s; Дата: %u.%u.%u; Сумма: %.2f",
           Num, E.Id, E.Category.c_str(), E.Note.c_str(), E.Day, E.Month, E.Year, E.Amount);
    if (E.Deleted)
    {
        printf(" [удалена]");
    }
    printf("\n");
}

void PrintNodeRecords(TreeNode *node)
{
    TreeNode *t = node;
    while (t != NULL)
    {
        int idx = t->RecIndex;
        if (!ExpArr[idx].Deleted)
        {
            PrintOne(ExpArr[idx], idx + 1);
        }
        t = t->Next;
    }
}

void TraverseAsc(TreeNode *node)
{
    if (node == NULL) return;
    TraverseAsc(node->Left);
    PrintNodeRecords(node);
    TraverseAsc(node->Right);
}

void TraverseDesc(TreeNode *node)
{
    if (node == NULL) return;
    TraverseDesc(node->Right);
    PrintNodeRecords(node);
    TraverseDesc(node->Left);
}

TreeNode* SearchRec(TreeNode *node, string key)
{
    if (node == NULL) return NULL;
    if (key == node->Key) return node;
    if (key < node->Key) return SearchRec(node->Left, key);
    return SearchRec(node->Right, key);
}

TreeNode* SearchIter(TreeNode *node, string key)
{
    TreeNode *cur = node;
    while (cur != NULL)
    {
        if (key == cur->Key) return cur;
        if (key < cur->Key)
            cur = cur->Left;
        else
            cur = cur->Right;
    }
    return NULL;
}

void InputOne()
{
    if (RecCount >= MaxRec)
    {
        printf("Массив заполнен\n");
        return;
    }
    printf("Введите данные расхода номер %d\n", RecCount + 1);
    printf("Id - только число, строки без пробелов, дата с учётом дней в месяце\n");
    if (!ReadUInt("Id", ExpArr[RecCount].Id)) return;
    if (!ReadString("Категория (строка)", ExpArr[RecCount].Category)) return;
    if (!ReadString("Описание (строка)", ExpArr[RecCount].Note)) return;
    if (!ReadDate(ExpArr[RecCount].Year, ExpArr[RecCount].Month, ExpArr[RecCount].Day)) return;
    if (!ReadPosDouble("Сумма", ExpArr[RecCount].Amount)) return;
    ExpArr[RecCount].Deleted = false;
    RecCount++;
    printf("Запись добавлена\n");
    BuildTree();
}

void PrintAll()
{
    printf("Текущие записи (по порядку ввода):\n");
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            PrintOne(ExpArr[i], i + 1);
        }
    }
    printf("\n");
}

void SaveToFile()
{
    string FileName;
    char Mode;
    string modeStr;
    printf("Введите имя файла (или # - выход): > ");
    if (!(cin >> FileName) || FileName == "#")
    {
        if (!cin) ClearInput();
        printf("Отмена\n");
        return;
    }
    printf("Режим: 1 - новый файл, 2 - добавить в конец (или # - выход): > ");
    if (!(cin >> modeStr))
    {
        ClearInput();
        return;
    }
    if (modeStr.size() != 1)
    {
        printf("Неверный режим\n");
        return;
    }
    Mode = modeStr[0];
    if (Mode == '#')
    {
        printf("Отмена\n");
        return;
    }
    if ((Mode != '1') && (Mode != '2'))
    {
        printf("Неверный режим\n");
        return;
    }
    ofstream f;
    if (Mode == '2')
        f.open(FileName, ios::app);
    else
        f.open(FileName, ios::out);
    if (!f.is_open())
    {
        printf("Файл не открыт\n");
        return;
    }
    for (int i = 0; i < RecCount; i++)
    {
        f << ExpArr[i].Id << ";" << ExpArr[i].Category << ";" << ExpArr[i].Note << ";";
        f << ExpArr[i].Day << ";" << ExpArr[i].Month << ";" << ExpArr[i].Year << ";";
        f << ExpArr[i].Amount << ";" << (ExpArr[i].Deleted ? 1 : 0) << endl;
    }
    f.close();
    printf("Запись в файл выполнена\n");
}

string NextField(string S, size_t &pos)
{
    // Формат строки: id;cat;note;day;month;year;amount;deleted
    size_t next = S.find(';', pos);
    if (next == string::npos)
    {
        string part = S.substr(pos);
        pos = S.length();
        return part;
    }
    string part = S.substr(pos, next - pos);
    pos = next + 1;
    return part;
}

void LoadFromFile()
{
    string FileName;
    printf("Имя файла для чтения (или # - выход): > ");
    if (!(cin >> FileName) || FileName == "#")
    {
        if (!cin) ClearInput();
        printf("Отмена\n");
        return;
    }
    ifstream f;
    f.open(FileName);
    if (!f.is_open())
    {
        printf("Файл не открыт\n");
        return;
    }
    string line;
    while (!f.eof())
    {
        getline(f, line);
        if (line.length() == 0)
            continue;
        if (RecCount >= MaxRec)
        {
            printf("Массив заполнен, чтение остановлено\n");
            break;
        }
        size_t pos = 0;
        string fid = NextField(line, pos);
        string fcat = NextField(line, pos);
        string fnote = NextField(line, pos);
        string fday = NextField(line, pos);
        string fmonth = NextField(line, pos);
        string fyear = NextField(line, pos);
        string famount = NextField(line, pos);
        string fdel = NextField(line, pos);
        ExpArr[RecCount].Id = stoi(fid);
        ExpArr[RecCount].Category = fcat;
        ExpArr[RecCount].Note = fnote;
        ExpArr[RecCount].Day = stoi(fday);
        ExpArr[RecCount].Month = stoi(fmonth);
        ExpArr[RecCount].Year = stoi(fyear);
        ExpArr[RecCount].Amount = stod(famount);
        ExpArr[RecCount].Deleted = (fdel == "1");
        RecCount++;
    }
    f.close();
    BuildTree();
    printf("Чтение из файла завершено\n");
}

void ShowByTree(bool Asc)
{
    if (Root == NULL)
    {
        printf("Индекс пуст\n");
        return;
    }
    if (Asc)
        TraverseAsc(Root);
    else
        TraverseDesc(Root);
    printf("\n");
}

void SearchByCategoryRec()
{
    if (Root == NULL)
    {
        printf("Сначала постройте дерево\n");
        return;
    }
    string key;
    if (!ReadString("Введите категорию для поиска (рекурсивно)", key)) return;
    TreeNode *node = SearchRec(Root, key);
    if (node == NULL)
    {
        printf("Не найдено\n\n");
        return;
    }
    PrintNodeRecords(node);
    printf("\n");
}

void SearchByCategoryIter()
{
    if (Root == NULL)
    {
        printf("Сначала постройте дерево\n");
        return;
    }
    string key;
    if (!ReadString("Введите категорию для поиска (итеративно)", key)) return;
    TreeNode *node = SearchIter(Root, key);
    if (node == NULL)
    {
        printf("Не найдено\n\n");
        return;
    }
    PrintNodeRecords(node);
    printf("\n");
}

void EditById()
{
    unsigned int id;
    printf("Id - только число\n");
    if (!ReadUInt("Введите Id записи для редактирования", id)) return;
    for (int i = 0; i < RecCount; i++)
    {
        if ((!ExpArr[i].Deleted) && (ExpArr[i].Id == id))
        {
            if (!ReadString("Новая категория (строка)", ExpArr[i].Category)) return;
            if (!ReadString("Новое описание (строка)", ExpArr[i].Note)) return;
            if (!ReadDate(ExpArr[i].Year, ExpArr[i].Month, ExpArr[i].Day)) return;
            if (!ReadPosDouble("Новая сумма", ExpArr[i].Amount)) return;
            printf("Запись изменена\n");
            BuildTree();
            return;
        }
    }
    printf("Запись не найдена\n");
}

void DeleteByCategory()
{
    if (Root == NULL)
    {
        printf("Сначала постройте дерево\n");
        return;
    }
    string key;
    if (!ReadString("Категория для удаления", key)) return;
    bool found = false;
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted && ExpArr[i].Category == key)
        {
            ExpArr[i].Deleted = true;
            found = true;
        }
    }
    if (found)
        printf("Все записи категории %s помечены как удалённые\n", key.c_str());
    else
        printf("Не найдено\n");
    BuildTree();
}

void DeleteById()
{
    unsigned int id;
    printf("Id - только число\n");
    if (!ReadUInt("Id для удаления", id)) return;
    bool found = false;
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted && ExpArr[i].Id == id)
        {
            ExpArr[i].Deleted = true;
            found = true;
            printf("Запись с Id %u помечена как удалённая\n", id);
            break;
        }
    }
    if (!found) printf("Запись с таким Id не найдена\n");
    BuildTree();
}

void RestoreById()
{
    unsigned int id;
    printf("Id - только число\n");
    if (!ReadUInt("Id записи для восстановления", id)) return;
    for (int i = 0; i < RecCount; i++)
    {
        if (ExpArr[i].Id == id && ExpArr[i].Deleted)
        {
            ExpArr[i].Deleted = false;
            printf("Запись восстановлена\n");
            BuildTree();
            return;
        }
    }
    printf("Удалённая запись не найдена\n");
}

void PackDeleted()
{
    int j = 0;
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            ExpArr[j] = ExpArr[i];
            j++;
        }
    }
    RecCount = j;
    BuildTree();
    printf("Удалённые записи вычищены\n");
}

void BuildIndexFromMenu()
{
    BuildTree();
    if (Root == NULL)
        printf("Индекс пуст\n");
    else
        printf("Индекс построен\n");
}

int main()
{
    char ch = '0';
    string cmd;

    do
    {
        printf("Меню:\n");
        printf("1 - ввод новой записи\n");
        printf("2 - вывод записей по вводу\n");
        printf("3 - запись в файл\n");
        printf("4 - чтение из файла\n");
        printf("5 - построить индекс-дерево по категории\n");
        printf("6 - вывод по дереву (возрастание)\n");
        printf("7 - вывод по дереву (убывание)\n");
        printf("8 - поиск по категории (рекурсивно)\n");
        printf("9 - поиск по категории (итеративно)\n");
        printf("c - редактировать по Id\n");
        printf("d - удалить по категории\n");
        printf("h - удалить по Id\n");
        printf("e - восстановить по Id\n");
        printf("f - физическое удаление помеченных\n");
        printf("0 - выход\n");
        if (!(cin >> cmd))
        {
            ClearInput();
            continue;
        }
        if (cmd.size() != 1)
        {
            printf("Команда не распознана\n\n");
            continue;
        }
        ch = cmd[0];
        printf("\n");

        switch (ch)
        {
        case '1':
            InputOne();
            break;
        case '2':
            PrintAll();
            break;
        case '3':
            SaveToFile();
            break;
        case '4':
            LoadFromFile();
            break;
        case '5':
            BuildIndexFromMenu();
            break;
        case '6':
            ShowByTree(true);
            break;
        case '7':
            ShowByTree(false);
            break;
        case '8':
            SearchByCategoryRec();
            break;
        case '9':
            SearchByCategoryIter();
            break;
        case 'c':
            EditById();
            break;
        case 'd':
            DeleteByCategory();
            break;
        case 'e':
            RestoreById();
            break;
        case 'f':
            PackDeleted();
            break;
        case 'h':
            DeleteById();
            break;
        case '0':
            break;
        default:
            printf("Нет такой команды\n");
            break;
        }
        printf("\n");
    } while (ch != '0');

    FreeTree(Root);
    return 0;
}
