#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <string>
#include <map>
#include <vector>

using namespace std;

vector<string> split_str(string& str, char separator)
{
    vector<string> output;
    string::size_type prev_pos = 0, pos = 0;

    while ((pos = str.find(separator, pos)) != string::npos)
    {
        string substr = str.substr(prev_pos, pos - prev_pos);
        output.push_back(substr);
        prev_pos = ++pos;
    }

    output.push_back(str.substr(prev_pos, pos - prev_pos));

    return output;
}

map <string, map<string, string>> read_csv_file(string file_name)
{
    ifstream file(file_name);
    string line;
    char sep = ',';

    map <string, map<string, string>> table;

    if (file.is_open())
    {
        vector<string> first;
        bool is_first_row = true;

        while (getline(file, line))
        {
            vector<string> values = split_str(line, sep);

            if (is_first_row)
            {
                first = values;
                is_first_row = false;
                continue;
            }

            string row_name;

            for (int i = 0; i < values.size(); i++)
            {
                if (i == 0)
                {
                    row_name = values[i];
                    continue;
                }

                string col_name = first[i];
                table[row_name][col_name] = values[i];
            }
        }

        file.close();
    }

    return table;
}

pair<string, string> parse_cell_address(string str_val, int& i)
{
    pair<string, string> arg;
    while (str_val[i] >= 65 && str_val[i] <= 90 || str_val[i] >= 97 && str_val[i] <= 122)
    {
        arg.second.push_back(str_val[i++]);
    }
    while (str_val[i] >= 48 && str_val[i] <= 57)
    {
        arg.first.push_back(str_val[i++]);
    }

    return arg;
}

string resolve_cell(map <string, map<string, string>>& table, string row_name, string col_name)
{
    string str_val = table[row_name][col_name];

    if (str_val[0] == '=')
    {
        int i = 1;
        pair<string, string> arg1, arg2;
        char op;

        arg1 = parse_cell_address(str_val, i);
        op = str_val[i++];
        arg2 = parse_cell_address(str_val, i);

        string resolved_1 = resolve_cell(table, arg1.first, arg1.second);
        string resolved_2 = resolve_cell(table, arg2.first, arg2.second);

        int int_val_1 = atoi(resolved_1.c_str());
        int int_val_2 = atoi(resolved_2.c_str());

        int res = 0;

        switch (op)
        {
        case '+':
            res = int_val_1 + int_val_2;
            break;

        case '-':
            res = int_val_1 - int_val_2;
            break;

        case '*':
            res = int_val_1 * int_val_2;
            break;

        case '/':
            res = int_val_1 / int_val_2;
            break;
        }

        return to_string(res);
    }
    else
    {
        return str_val;
    }
}

void resolve_table(map <string, map<string, string>>& table)
{
    for (auto row : table)
    {
        for (auto col : row.second)
        {
            table[row.first][col.first] = resolve_cell(table, row.first, col.first);
        }
    }
}

void print_table(map <string, map<string, string>>& table)
{
    bool is_first_row = true;

    for (auto row : table)
    {
        if (is_first_row)
        {
            for (auto col : row.second)
            {
                cout << "\t" << col.first;
            }
            cout << endl;
            is_first_row = false;
        }

        cout << row.first;
        for (auto col : row.second)
        {
            cout << "\t" << col.second;
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "Invalid arguments" << endl;
        return EXIT_FAILURE;
    }

    auto table = read_csv_file(argv[1]);
    resolve_table(table);
    print_table(table);

    return 0;
}