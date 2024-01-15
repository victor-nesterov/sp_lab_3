// Вариант 3

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

using namespace std;

int quantityOfDigits;

int FindQuantityOfDigits(int a)
{
    if (a < 0) return 1 + FindQuantityOfDigits(-a); // В отрицательных на 1 больше (знак "-")
    if (a < 10) return 1; // Если меньше 10 (но при этом не отрицательное) - 1 цифра
    return 1 + FindQuantityOfDigits(a / 10); // Если >= 10, то / 10, тем самым отнимая по 1 цифре
}

string NumberWithQuantityOfDigits(int a)
{
    int spaces = quantityOfDigits - FindQuantityOfDigits(a);
    string r = "";
    for (int i = 0; i < spaces; ++i)
        r.push_back(' ');
    r += to_string(a);
    return r;
}

struct Lexeme
{
public:
    int start_symb;
    int start_str;
    int finish_symb;
    int finish_str;
    int type;
    Lexeme() {}
    Lexeme(int s_symb, int s_str, int f_symb, int f_str, int tpe)
    {
        define(s_symb, s_str, f_symb, f_str, tpe);
    }
    void define(int s_symb, int s_str, int f_symb, int f_str, int tpe)
    {
        start_symb = s_symb;
        start_str = s_str;
        finish_symb = f_symb;
        finish_str = f_str;
        type = tpe;
        /* cout << "start_symb = " << start_symb << endl;
        cout << "start_str = " << start_str << endl;
        cout << "finish_symb = " << finish_symb << endl;
        cout << "finish_str = " << finish_str << endl;
        cout << "type = " << type << endl; */
    }
};

vector<char> separators = { ';', ',', '{', '}', '(', ')' };
vector<char> additional_separators = { ' ', '\n' };
vector<char> operators_char = { '+', '-', '*', '/', '=', '%', '!', '>', '<', '!', '&', '|', '~', '^', '[', ']', ':' };
vector<string> operators = { "++", "!=", "&&", "<<", "+", "-", "*", "/", "=", "%", "!", ">", "<", "!", "&", "|", "~", "^", "[", "]", "[]", ":" };
vector<string> preprocessor_directives = { "#define", "#elif", "#else", "#endif", "#error", "#if", "#ifdef", "#ifndef", "#include" };
vector<string> reserved_words = { "int", "char", "bool", "float", "double", "long", "new", "delete", "const", "public", "private", "class", "struct", "for", "while", "return", "using", "namespace", "enum", "operator", "const_cast", "static_cast", "dynamic_cast", "reinterpret_cast", "auto", "break", "case", "continue", "default", "do", "else", "if", "extern", "goto", "register", "short", "signed", "unsigned", "sizeof", "static", "switch", "typedef", "union", "void", "volatile" };

int main()
{
    // Открываем файл:
    ifstream fin;
    fin.open("C:\\Users\\Victor\\source\\repos\\Lab1\\input.txt");
    if (!fin)
    {
        cout << "Cannot open the file" << endl;
        return 1;
    }
    // Находим количество строк (чтобы красиво выводить номер строк в консоли):
    int str_no = 1;
    char t, before_t = ' ';
    while (fin.get(t))
        if (t == '\n')
            ++str_no;
    quantityOfDigits = FindQuantityOfDigits(str_no);
    // Поиск лексем:
    fin.close();
    fin.open("C:\\Users\\Victor\\source\\repos\\Lab1\\input.txt");
    int symb_no = 1;
    str_no = 1;
    int symb_last = -1; // номер последнего символа предыдущей строки

    bool start_lexeme = true, parsing_lexeme = false;
    int current_symbol_state = 0; // 1 - сейчас комментарий однострочный, 2 - многострочный
    int current_symbol_start_symb, current_symbol_start_str;

    int start_current_lexeme_symb;
    int start_current_lexeme_str;
    string current_lexeme = "";

    vector<Lexeme> lexemes(0);
    while (fin.get(t))
    {
        // тут работаем с символом. str_no - номер строки, symb_no - номер символа
        if (current_symbol_state == 1 && t == '\n')
        {
            lexemes.push_back(Lexeme(current_symbol_start_symb, current_symbol_start_str, symb_no, str_no, 3));
            current_symbol_state = 0;
        }
        if (start_lexeme)
        {
            start_current_lexeme_symb = symb_no;
            start_current_lexeme_str = str_no;
            start_lexeme = false;
            parsing_lexeme = true;
        }
        if (parsing_lexeme)
            current_lexeme.push_back(t);

        bool need_check_current_lexeme = false;
        bool need_pushback_lexeme = false;
        Lexeme tmp_lexeme;
        // 1. Если разделительный знак - то смотрим на current_lexeme
        bool isOurLexeme = false;
        for (int i = 0; i < separators.size(); ++i)
            if (separators[i] == t)
            {
                isOurLexeme = true;
                break;
            }
        if (isOurLexeme)
        {
            need_check_current_lexeme = true;
            tmp_lexeme.define(symb_no, str_no, symb_no, str_no, 1);
            need_pushback_lexeme = true;
        }
        // 2. Если второй (дополнительный) разделитель
        isOurLexeme = false;
        for (int i = 0; i < additional_separators.size(); ++i)
            if (additional_separators[i] == t)
            {
                isOurLexeme = true;
                break;
            }
        if (isOurLexeme)
            need_check_current_lexeme = true;
        // 3. Если оператор разделяет лексемы, например a+b без всяких пробелов
        // Изм.: если этот и предыдущий символ: один - оператор, второй - не оператор, то смотрим current_lexeme
        int different_type = 0;
        for (int i = 0; i < operators_char.size(); ++i)
            if (operators_char[i] == t)
            {
                different_type += 2;
                break;
            }
        for (int i = 0; i < operators_char.size(); ++i)
            if (operators_char[i] == before_t)
            {
                different_type -= 2;
                break;
            }
        if ((t >= 'a' && t <= 'z') || (t >= 'A' && t <= 'Z') || (t >= '0' && t <= '9'))
            different_type += 5;
        if ((before_t >= 'a' && before_t <= 'z') || (before_t >= 'A' && before_t <= 'Z') || (before_t >= '0' && before_t <= '9'))
            different_type -= 5;
        if (different_type == 3 || different_type == -3)
        {
            // если different_type == -3, то надо добавить текущий символ

            need_check_current_lexeme = true;
            // tmp_lexeme.define(symb_no, str_no, symb_no, str_no, 6);
            // need_pushback_lexeme = true;
        }
        // 4. Проверка current_lexeme
        if (need_check_current_lexeme)
        {
            if (current_lexeme.size() > 1)
            {
                current_lexeme.pop_back();

                // TEST:

                bool already_found = false;
                // 4.1 Проверяем директивы препроцессора
                isOurLexeme = false;
                for (int i = 0; i < preprocessor_directives.size(); ++i)
                    if (preprocessor_directives[i] == current_lexeme)
                    {
                        isOurLexeme = true;
                        break;
                    }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 2));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 2));
                    already_found = true;

                    cout << "Lexeme: " << current_lexeme << " - preprocessor directive" << endl;
                }
                // 4.2 Проверяем зарезервированные слова
                isOurLexeme = false;
                for (int i = 0; i < reserved_words.size(); ++i)
                    if (reserved_words[i] == current_lexeme)
                    {
                        isOurLexeme = true;
                        break;
                    }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 5));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 5));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - reserved word" << endl;
                }
                // 4.3.1 Проверяем десятичные числа
                isOurLexeme = true;
                for (int i = 0; i < current_lexeme.size(); ++i)
                    if (current_lexeme[i] > '9' || current_lexeme[i] < '0')
                    {
                        isOurLexeme = false;
                        break;
                    }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 4));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 4));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - number" << endl;
                }
                // 4.3.2 Проверяем шестнадцатеричные числа
                isOurLexeme = false;
                if (current_lexeme.size() > 2)
                {
                    string tmp_str_lex = "";
                    for (int i = 0; i < 2; ++i)
                        tmp_str_lex.push_back(current_lexeme[i]);
                    if (tmp_str_lex == "0x")
                    {
                        isOurLexeme = true;
                        for (int i = 2; i < current_lexeme.size(); ++i)
                            if (!((current_lexeme[i] <= '9' && current_lexeme[i] >= '0') || (current_lexeme[i] <= 'f' && current_lexeme[i] >= 'a') || (current_lexeme[i] <= 'F' && current_lexeme[i] >= 'A')))
                            {
                                isOurLexeme = false;
                                break;
                            }
                    }
                }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 4));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 4));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - HEX number" << endl;
                }
                // 4.3.3 Проверяем десятичное с плавающей точкой
                isOurLexeme = false;
                if (current_lexeme.size() > 1 && current_lexeme[current_lexeme.size() - 1] != '.')
                {
                    bool hasDot = false; // есть ли точка в плавающем числе
                    isOurLexeme = true;
                    for (int i = 0; i < current_lexeme.size(); ++i)
                        if (current_lexeme[i] > '9' || current_lexeme[i] < '0')
                        {
                            if (current_lexeme[i] == '.')
                            {
                                if (!hasDot)
                                    hasDot = true;
                                else if (hasDot)
                                {
                                    isOurLexeme = false;
                                    break;
                                }
                            }
                            else
                            {
                                isOurLexeme = false;
                                break;
                            }
                        }
                    if (!hasDot)
                        isOurLexeme = false;
                }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 4));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 4));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - float/double number" << endl;
                }
                // 4.4 Проверяем идентификаторы/имена
                isOurLexeme = true;
                if (!((current_lexeme[0] >= 'a' && current_lexeme[0] <= 'z') || (current_lexeme[0] >= 'A' && current_lexeme[0] <= 'Z') || current_lexeme[0] == '_'))
                {
                    isOurLexeme = false;
                }
                else
                {
                    for (int i = 1; i < current_lexeme.size(); ++i)
                        if (!((current_lexeme[0] >= '0' && current_lexeme[0] <= '9') || (current_lexeme[0] >= 'a' && current_lexeme[0] <= 'z') || (current_lexeme[0] >= 'A' && current_lexeme[0] <= 'Z') || current_lexeme[0] == '_'))
                        {
                            isOurLexeme = false;
                            break;
                        }
                }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 7));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 7));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - identificator/name" << endl;
                }
                // 4.5 Проверяем операторы
                isOurLexeme = false;
                for (int i = 0; i < operators.size(); ++i)
                    if (operators[i] == current_lexeme)
                    {
                        isOurLexeme = true;
                        break;
                    }
                if (isOurLexeme && !already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 6));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 6));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - operator" << endl;
                }
                // 4.6 Проверяем комментарии (однострочный)
                if (current_lexeme == "//" && !already_found)
                {
                    current_symbol_state = 1;
                    current_symbol_start_symb = start_current_lexeme_symb;
                    current_symbol_start_str = start_current_lexeme_str;
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - comment" << endl;
                }
                // 4.7 ... error
                if (!already_found && current_symbol_state == 0)
                {
                    if (symb_no != 1)
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_no - 1, str_no, 8));
                    else
                        lexemes.push_back(Lexeme(start_current_lexeme_symb, start_current_lexeme_str, symb_last, str_no - 1, 8));
                    already_found = true;
                    cout << "Lexeme: " << current_lexeme << " - lexeme error" << endl;
                }
            }

            current_lexeme = "";
            if (different_type == 3 || different_type == -3)
            {
                current_lexeme.push_back(t);
                start_current_lexeme_symb = symb_no;
                start_current_lexeme_str = str_no;
                start_lexeme = false;
                parsing_lexeme = true;
            }
            else
            {
                start_lexeme = true;
                parsing_lexeme = false;
            }
            need_check_current_lexeme = false;
        }
        if (need_pushback_lexeme && current_symbol_state == 0)
        {
            lexemes.push_back(tmp_lexeme);
        }
        // записываем before_t
        before_t = t;
        // тут увеличиваем str_no & symb_no
        ++symb_no;
        if (t == '\n')
        {
            symb_last = symb_no;
            symb_no = 1;
            ++str_no;
        }
    }
    cout << endl;
    // Выводим текст кода:
    fin.close();
    fin.open("C:\\Users\\Victor\\source\\repos\\Lab1\\input.txt");
    symb_no = 1;
    str_no = 1;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << NumberWithQuantityOfDigits(str_no) << ". ";
    int it = 0;
    bool now_print_lexeme = false;
    while (fin.get(t))
    {
        // тут работаем с символом. str_no - номер строки, symb_no - номер символа
        if (it < lexemes.size() && lexemes[it].start_str == str_no && lexemes[it].start_symb == symb_no)
        {
            if (lexemes[it].type == 1) // Разделительные знаки  (лексема 1)
                SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY);
            else if (lexemes[it].type == 2) // Директивы препроцессора (лексема 2)
                SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_BLUE);
            else if (lexemes[it].type == 4) // Числа (лексема 4)
                SetConsoleTextAttribute(handle, FOREGROUND_GREEN | FOREGROUND_BLUE);
            else if (lexemes[it].type == 5) // Зарезервированные слова (лексема 5)
                SetConsoleTextAttribute(handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            else if (lexemes[it].type == 6) // Операторы (лексема 6)
                SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN /* | FOREGROUND_BLUE */);
            else if (lexemes[it].type == 3) // Комментарии (лексема 3)
                SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
            else if (lexemes[it].type == 8) // Error (лексема 8)
                SetConsoleTextAttribute(handle, FOREGROUND_RED);
        }
        cout << t;
        if (it < lexemes.size() && lexemes[it].finish_str == str_no && lexemes[it].finish_symb == symb_no)
        {
            SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            ++it;
        }
        // тут увеличиваем str_no & symb_no
        ++symb_no;
        if (t == '\n')
        {
            symb_no = 1;
            ++str_no;
            cout << NumberWithQuantityOfDigits(str_no) << ". ";
        }
    }
    while (1);
    return 0;
}