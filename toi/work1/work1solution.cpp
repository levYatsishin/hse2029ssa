#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <limits>
#include <cstdlib>

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

struct CatIndexElement
{
    string CatKey;
    int RecN;
};

struct DateIndexElement
{
    int DateKey;
    int RecN;
};

Expense ExpArr[MaxRec];
int RecCount = 0;

CatIndexElement CatIndex[MaxRec + 1]; // с барьером
int CatIndexCount = 0;

DateIndexElement DateIndexArr[MaxRec + 1]; // с барьером
int DateIndexCount = 0;

void ClearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
            // stoul переводит строку в число; затем приводим к unsigned int
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
    // Только положительное значение
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
    if (!ReadUInt("Год", y)) return false; // сначала год
    while (true)
    {
        if (!ReadUInt("Месяц", m)) return false; // проверяем 1..12
        if ((m >= 1) && (m <= 12)) break;
        printf("Месяц должен быть 1..12\n");
    }
    while (true)
    {
        if (!ReadUInt("День", d)) return false; // проверяем дни в месяце
        int maxd = DaysInMonth[m];
        if ((d >= 1) && (d <= static_cast<unsigned int>(maxd))) break;
        printf("В этом месяце %d дней\n", maxd);
    }
    return true;
}

int CalcDateKey(Expense E)
{
    // Вычисляем ключ даты вида YYYYMMDD
    return (E.Year * 10000 + E.Month * 100 + E.Day);
}

string Key(CatIndexElement IE)
{
    return (IE.CatKey);
}

int Key(DateIndexElement IE)
{
    return (IE.DateKey);
}

int OrderCatByInsert(CatIndexElement *A, int AN)
{
    int C = 0; // пересылки
    CatIndexElement x;
    int i, j;

    for (i = 2; i < AN; i++)
    {
        x = A[i];
        A[0] = x;
        j = i - 1;
        while (Key(x) < Key(A[j]))
        {
            C++;
            A[j + 1] = A[j];
            j = j - 1;
        }
        A[j + 1] = x;
    }
    return (C);
}

int OrderDateBySelection(DateIndexElement A[], int AN)
{
    int C = 0; // пересылки
    DateIndexElement x;
    int i, j;
    int MinEl;
    int MinIndex;

    for (i = 1; i < AN - 1; i++)
    {
        MinEl = Key(A[i]);
        MinIndex = i;
        for (j = i + 1; j <= AN - 1; j++)
        {
            if (Key(A[j]) < MinEl)
            {
                MinEl = Key(A[j]);
                MinIndex = j;
                C++;
            }
        }
        x = A[MinIndex];
        A[MinIndex] = A[i];
        A[i] = x;
        C++;
    }

    return (C);
}

int RecBinarySearchCat(CatIndexElement Arr[], int L, int R, string K)
{
    int M;

    if (L <= R)
    {
        M = ((R - L) / 2) + L;
        if (Key(Arr[M]) == K)
        {
            return M;
        }
        else
        {
            if (Key(Arr[M]) > K)
                R = M - 1;
            else
                L = M + 1;
            return RecBinarySearchCat(Arr, L, R, K);
        }
    }
    return -1;
}

int IterBinarySearchDate(DateIndexElement Arr[], int AN, int K)
{
    int L = 1;
    int R = AN;
    int M;

    if ((AN > 0) && (K >= Key(Arr[L])) && (K <= Key(Arr[R])))
    {
        while (L <= R)
        {
            M = ((R - L) / 2) + L;
            if (Key(Arr[M]) == K)
            {
                return M;
            }
            else
            {
                if (Key(Arr[M]) > K)
                    R = M - 1;
                else
                    L = M + 1;
            }
        }
    }
    return -1;
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
        // Сохраняем все записи, включая помеченные, с флагом deleted
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
    printf("Чтение из файла завершено\n");
}

void BuildCatIndex()
{
    CatIndexCount = 0; // пересоздаём индекс
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            CatIndexCount++;
            CatIndex[CatIndexCount].CatKey = ExpArr[i].Category;
            CatIndex[CatIndexCount].RecN = i;
        }
    }
    if (CatIndexCount > 1)
    {
        OrderCatByInsert(CatIndex, CatIndexCount + 1);
    }
}

void BuildDateIndex()
{
    DateIndexCount = 0; // пересоздаём индекс
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            DateIndexCount++;
            DateIndexArr[DateIndexCount].DateKey = CalcDateKey(ExpArr[i]);
            DateIndexArr[DateIndexCount].RecN = i;
        }
    }
    if (DateIndexCount > 1)
    {
        OrderDateBySelection(DateIndexArr, DateIndexCount + 1);
    }
}

void ShowByCatIndex(bool Asc)
{
    if (CatIndexCount == 0)
    {
        printf("Индекс пуст\n");
        return;
    }
    if (Asc)
    {
        for (int i = 1; i <= CatIndexCount; i++)
        {
            PrintOne(ExpArr[CatIndex[i].RecN], CatIndex[i].RecN + 1);
        }
    }
    else
    {
        for (int i = CatIndexCount; i >= 1; i--)
        {
            PrintOne(ExpArr[CatIndex[i].RecN], CatIndex[i].RecN + 1);
        }
    }
    printf("\n");
}

void ShowByDateIndex(bool Asc)
{
    if (DateIndexCount == 0)
    {
        printf("Индекс пуст\n");
        return;
    }
    if (Asc)
    {
        for (int i = 1; i <= DateIndexCount; i++)
        {
            PrintOne(ExpArr[DateIndexArr[i].RecN], DateIndexArr[i].RecN + 1);
        }
    }
    else
    {
        for (int i = DateIndexCount; i >= 1; i--)
        {
            PrintOne(ExpArr[DateIndexArr[i].RecN], DateIndexArr[i].RecN + 1);
        }
    }
    printf("\n");
}

void ListCategories()
{
    if (CatIndexCount == 0)
    {
        printf("Категории отсутствуют, постройте индекс по категории\n");
        return;
    }
    printf("Список категорий (по индексу):\n");
    for (int i = 1; i <= CatIndexCount; i++)
    {
        printf("%s\n", CatIndex[i].CatKey.c_str());
    }
    printf("\n");
}

void SearchByCategory()
{
    if (CatIndexCount == 0)
    {
        printf("Сначала постройте индекс по категории\n");
        return;
    }
    string key;
    if (!ReadString("Введите категорию для поиска", key)) return;
    int pos = RecBinarySearchCat(CatIndex, 1, CatIndexCount, key);
    if (pos == -1)
    {
        printf("Не найдено\n");
    }
    else
    {
        PrintOne(ExpArr[CatIndex[pos].RecN], CatIndex[pos].RecN + 1);
    }
    printf("\n");
}

void SearchByDate()
{
    if (DateIndexCount == 0)
    {
        printf("Сначала постройте индекс по дате\n");
        return;
    }
    unsigned int d, m, y;
    if (!ReadDate(y, m, d)) return;
    int key = y * 10000 + m * 100 + d;
    int pos = IterBinarySearchDate(DateIndexArr, DateIndexCount, key);
    if (pos == -1)
    {
        printf("Не найдено\n");
    }
    else
    {
        PrintOne(ExpArr[DateIndexArr[pos].RecN], DateIndexArr[pos].RecN + 1);
    }
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
            BuildCatIndex();
            BuildDateIndex();
            return;
        }
    }
    printf("Запись не найдена\n");
}

void DeleteByCategory()
{
    if (CatIndexCount == 0)
    {
        printf("Сначала постройте индекс по категории\n");
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
    BuildCatIndex();
    BuildDateIndex();
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
    BuildCatIndex();
    BuildDateIndex();
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
            BuildCatIndex();
            BuildDateIndex();
            return;
        }
    }
    printf("Удалённая запись не найдена\n");
}

void RestoreByCategory()
{
    if (CatIndexCount == 0)
    {
        printf("Сначала постройте индекс по категории\n");
        return;
    }
    string key;
    if (!ReadString("Категория для восстановления", key)) return;
    bool found = false;
    for (int i = 0; i < RecCount; i++)
    {
        if (ExpArr[i].Deleted && ExpArr[i].Category == key)
        {
            ExpArr[i].Deleted = false;
            found = true;
        }
    }
    if (found)
    {
        printf("Записи категории %s восстановлены\n", key.c_str());
        BuildCatIndex();
        BuildDateIndex();
    }
    else
    {
        printf("Удалённых записей с такой категорией нет\n");
    }
}

void PackDeleted()
{
    int j = 0; // новая позиция не удаленных записей
    for (int i = 0; i < RecCount; i++)
    {
        if (!ExpArr[i].Deleted)
        {
            ExpArr[j] = ExpArr[i];
            j++;
        }
    }
    RecCount = j;
    BuildCatIndex();
    BuildDateIndex();
    printf("Удалённые записи вычищены\n");
}

void BuildAllIndexes()
{
    // строятся оба индекса
    BuildCatIndex(); 
    BuildDateIndex();
}

int main()
{
    setlocale(LC_ALL, "Russian");
    char ch = '0';
    string cmd;

    do
    {
        printf("Меню:\n");
        printf("1 - ввод новой записи\n");
        printf("2 - вывод записей по вводу\n");
        printf("3 - запись в файл\n");
        printf("4 - чтение из файла\n");
        printf("5 - построить индексы\n");
        printf("6 - вывод по индексу категории (возрастание)\n");
        printf("7 - вывод по индексу категории (убывание)\n");
        printf("8 - вывод по индексу даты (возрастание)\n");
        printf("9 - вывод по индексу даты (убывание)\n");
        printf("a - поиск по категории (рекурсивно)\n");
        printf("b - поиск по дате (итеративно)\n");
        printf("g - показать все категории\n");
        printf("c - редактировать по Id\n");
        printf("d - удалить по категории\n");
        printf("h - удалить по Id\n");
        printf("e - восстановить по Id\n");
        printf("i - восстановить по категории\n");
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
            BuildAllIndexes();
            printf("Индексы построены\n");
            break;
        case '6':
            ShowByCatIndex(true);
            break;
        case '7':
            ShowByCatIndex(false);
            break;
        case '8':
            ShowByDateIndex(true);
            break;
        case '9':
            ShowByDateIndex(false);
            break;
        case 'a':
            SearchByCategory();
            break;
        case 'b':
            SearchByDate();
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
        case 'g':
            ListCategories();
            break;
        case 'h':
            DeleteById();
            break;
        case 'i':
            RestoreByCategory();
            break;
        case '0':
            break;
        default:
            printf("Нет такой команды\n");
            break;
        }
        printf("\n");
    } while (ch != '0');

    return 0;
}
