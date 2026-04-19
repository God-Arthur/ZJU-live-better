#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
using namespace std;

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define _MY_WINDOWS_
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define _MY_LINUX_
#endif

#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#else
// 兼容C++11/14的旧版experimental filesystem
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

void maintain_history(fstream &history, vector<string> &records);

#ifdef _MY_WINDOWS_

#define POPEN _popen
#define PCLOSE _pclose

#include <conio.h>

#elif defined(_MY_LINUX_)

#define POPEN popen
#define PCLOSE pclose

#include <termios.h>
#include <unistd.h>

#endif

void read_script_name(string &script_name)
{
    
    char c;
    int i, idx, n, length, arrow;

    string tmp;

    fstream history;
    vector<string> records;

    #ifdef _MY_LINUX_
    struct termios old_attr, new_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    #endif

    read_script_name_again:

    if (!fs::exists(fs::path("./config"))) {
        fs::create_directory("./config");
    }
    
    history.open("./config/scriptHistory.txt", ios::in | ios::out | ios::app);
    if(!history.is_open()) {
        history.clear();
        history.open("./config/scriptHistory.txt", ios::in | ios::out | ios::app);
    }

    if(history.is_open()) {
        maintain_history(history, records);
    }

    #ifdef _MY_LINUX_
    // 关闭回显
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
    #endif

    arrow = 0;

    if(history.is_open()) {
        n = records.size();
        idx = n;
    }

    cout << "输入脚本路径：.js\b\b\b";
    script_name.clear();

    #ifdef _MY_WINDOWS_
    while((c = _getch()) != '\r')
    #elif defined(_MY_LINUX_)
    while((c = getchar()) != '\n')
    #endif

    {
        cout << "   \b\b\b";

        #ifdef _MY_WINDOWS_
        if(c == '\b')
        #elif defined(_MY_LINUX_)
        if(c == 127)/*退格键*/
        #endif

        {
            if(arrow) {
                idx = n;
                arrow = 0;
            }

            if(!script_name.empty()) {
                script_name.pop_back();
                cout << "\b \b";
            }
            cout << ".js\b\b\b";
        }
        else if(c>=32 && c<=126) {
            if(arrow) {
                idx = n;
                arrow = 0;
            }
            
            script_name += c;
            cout << c << ".js\b\b\b";
        }

        #ifdef _MY_WINDOWS_
        else if(c==-32 || c==0)
        #elif defined(_MY_LINUX_)
        else if(c == 27) /* Esc 键开始序列 */
        #endif
        
        {
            #ifdef _MY_LINUX_
            if((c = getchar()) == 91)
            #endif
            {
                #ifdef _MY_WINDOWS_
                c = _getch();
                #elif defined(_MY_LINUX_)
                c = getchar();
                #endif

                if(n == 0) {
                    cout << ".js\b\b\b";
                    continue;
                }

                length = script_name.size();
                
                #ifdef _MY_WINDOWS_
                if(c == 72)
                #elif defined(_MY_LINUX_)
                if(c == 65)
                #endif
                {//上箭头

                    if(idx==0) {
                        cout << ".js\b\b\b";
                        continue;
                    }
                    else if(idx == n) {
                        tmp.clear();
                        tmp = script_name;
                    }

                    for(i=0; i<length; i++)
                        cout << "\b \b";
                    script_name = records[--idx];
                    cout << script_name << ".js\b\b\b";

                    arrow = 1;
                }

                #ifdef _MY_WINDOWS_
                if(c == 80)
                #elif defined(_MY_LINUX_)
                if(c == 66)
                #endif

                {//下箭头

                    if(idx == n) {
                        cout << ".js\b\b\b";
                        continue;
                    }

                    for(i=0; i<length; i++)
                        cout << "\b \b";

                    if(idx == n-1) {
                        script_name = tmp;
                        idx++;
                    }
                    else script_name = records[++idx];
                    cout << script_name << ".js\b\b\b";

                    arrow = 1;
                }
            }
        }
        
        else if(c==3) {//Ctrl+C
            cout << "\n^C\n";
            #ifdef _MY_LINUX_
            tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
            #endif
            exit(0);
            
        }
    }
    cout << '\n';

    if(!fs::exists(fs::path(script_name + ".js"))) {
        cout << "脚本不存在，请检查输入\n";
        goto read_script_name_again;
    }

    if(history.is_open()) {
        for(size_t i = 0; i < records.size(); i++) {
            if(records[i] == script_name) {
                records.erase(records.begin() + i);
                break;
            }
        }
        records.push_back(script_name);
        history.close();
        history.open("./config/scriptHistory.txt", ios::out | ios::trunc);
        for(const auto& r : records) {
            history << r << '\n';
        }
        history.close();
    }//写入新历史记录，若存在则删除旧记录，相当于更新

    script_name = "\"" + script_name + ".js\"";

    #ifdef _MY_LINUX_
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
    #endif

    fstream arg_history;
    vector<string> arg_records;

    arg_history.open("./config/argHistory.txt", ios::in | ios::out | ios::app);
    if(!arg_history.is_open()) {
        arg_history.clear();
        arg_history.open("./config/argHistory.txt", ios::in | ios::out | ios::app);
    }

    if(arg_history.is_open()) {
        maintain_history(arg_history, arg_records);
    }

    #ifdef _MY_LINUX_
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
    #endif

    int arg_idx, arg_n, arg_length;
    string arg_tmp;
    int arg_arrow = 0;

    if(arg_history.is_open()) {
        arg_n = arg_records.size();
        arg_idx = arg_n;
    }

    cout << "以下命令将执行，可继续添加参数，直接回车则参数为空：\n";
    cout << "node " << script_name << ' ';

    string arg;
    arg.clear();

    #ifdef _MY_WINDOWS_
    while((c = _getch()) != '\r')
    #elif defined(_MY_LINUX_)
    while((c = getchar()) != '\n')
    #endif
    {
        #ifdef _MY_WINDOWS_
        if(c == '\b')
        #elif defined(_MY_LINUX_)
        if(c == 127)
        #endif

        {
            if(arg_arrow) {
                arg_idx = arg_n;
                arg_arrow = 0;
            }

            if(!arg.empty()) {
                arg.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            if(arg_arrow) {
                arg_idx = arg_n;
                arg_arrow = 0;
            }

            arg += c;
            cout << c;
        }

        #ifdef _MY_WINDOWS_
        else if(c==-32 || c==0)
        #elif defined(_MY_LINUX_)
        else if(c == 27)
        #endif
        {
            #ifdef _MY_LINUX_
            if((c = getchar()) == 91)
            #endif
            {
                #ifdef _MY_WINDOWS_
                c = _getch();
                #elif defined(_MY_LINUX_)
                c = getchar();
                #endif

                if(arg_n == 0) {
                    continue;
                }

                arg_length = arg.size();

                #ifdef _MY_WINDOWS_
                if(c == 72)
                #elif defined(_MY_LINUX_)
                if(c == 65)
                #endif
                {
                    if(arg_idx==0) {
                        continue;
                    }
                    else if(arg_idx == arg_n) {
                        arg_tmp.clear();
                        arg_tmp = arg;
                    }

                    for(i=0; i<arg_length; i++)
                        cout << "\b \b";
                    arg = arg_records[--arg_idx];
                    cout << arg;

                    arg_arrow = 1;
                }

                #ifdef _MY_WINDOWS_
                if(c == 80)
                #elif defined(_MY_LINUX_)
                if(c == 66)
                #endif
                {
                    if(arg_idx == arg_n) {
                        continue;
                    }

                    for(i=0; i<arg_length; i++)
                        cout << "\b \b";

                    if(arg_idx == arg_n-1) {
                        arg = arg_tmp;
                        arg_idx++;
                    }
                    else arg = arg_records[++arg_idx];
                    cout << arg;

                    arg_arrow = 1;
                }
            }
        }

        else if(c==3) {
            cout << "\n^C\n";
            #ifdef _MY_LINUX_
            tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
            #endif
            exit(0);
        }
    }
    cout << '\n';

    if(arg_history.is_open()) {
        for(size_t i = 0; i < arg_records.size(); i++) {
            if(arg_records[i] == arg) {
                arg_records.erase(arg_records.begin() + i);
                break;
            }
        }
        arg_records.push_back(arg);
        arg_history.close();
        arg_history.open("./config/argHistory.txt", ios::out | ios::trunc);
        for(const auto& r : arg_records) {
            arg_history << r << '\n';
        }
        arg_history.close();
    }

    #ifdef _MY_LINUX_
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
    #endif

    if(!arg.empty())
        script_name += ' ' + arg;
}
void read_password(string &password)
{
    #ifdef _MY_LINUX_
    struct termios old_attr, new_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    #endif

    read_password_again:

    cout << "输入密码：";
    password.clear();
    
    #ifdef _MY_LINUX_
    // 关闭回显
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
    #endif

    char c;

    #ifdef _MY_WINDOWS_
    while((c = _getch()) != '\r')
    #elif defined(_MY_LINUX_)
    while((c = getchar()) != '\n')
    #endif

    {
        #ifdef _MY_WINDOWS_
        if(c == '\b')
        #elif defined(_MY_LINUX_)
        if(c == 127)
        #endif

        {
            if(!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            cout << "*";
            password += c;
        }
        else if(c==3) {//Ctrl+C
            cout << "\n^C\n";
            #ifdef _MY_LINUX_
            tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
            #endif
            exit(0);
            
        }
    }

    #ifdef _MY_LINUX_
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
    #endif

    cout << '\n';


    invalid_password_check:

    cout << "确认?[Y/n] ";
    string tmp;
    cin >> tmp;
    cin.ignore();

    if(tmp == "n") {
        password.clear();
        goto read_password_again;
    }
    else if(tmp != "Y") {
        goto invalid_password_check;
    }
}

void read_num(string &num)
{
    char c;
    int i, idx, n, length, arrow;

    string tmp;

    fstream history;
    vector<string> records;

    #ifdef _MY_LINUX_
    struct termios old_attr, new_attr;
    tcgetattr(STDIN_FILENO, &old_attr);
    #endif

    read_num_again:

    if (!fs::exists(fs::path("./config"))) {
        fs::create_directory("./config");
    }

    history.open("./config/numHistory.txt", ios::in | ios::out | ios::app);
    if(!history.is_open()) {
        history.clear();
        history.open("./config/numHistory.txt", ios::in | ios::out | ios::app);
    }

    if(history.is_open()) {
        maintain_history(history, records);
    }

    #ifdef _MY_LINUX_
    // 关闭回显
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
    #endif

    arrow = 0;

    if(history.is_open()) {
        n = records.size();
        idx = n;
    }

    cout << "输入学号：";
    num.clear();

    #ifdef _MY_WINDOWS_
    while((c = _getch()) != '\r')
    #elif defined(_MY_LINUX_)
    while((c = getchar()) != '\n')
    #endif

    {
        #ifdef _MY_WINDOWS_
        if(c == '\b')
        #elif defined(_MY_LINUX_)
        if(c == 127)/*退格键*/
        #endif
        
        {
            if(arrow) {
                idx = n;
                arrow = 0;
            }

            if(!num.empty()) {
                num.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            if(arrow) {
                idx = n;
                arrow = 0;
            }

            cout << c;
            num += c;
        }

        #ifdef _MY_WINDOWS_
        else if(c==-32 || c==0)
        #elif defined(_MY_LINUX_)
        else if(c == 27)/* Esc 键开始序列 */
        #endif

        {
            #ifdef _MY_LINUX_
            if((c = getchar()) == 91)
            #endif
            {
                #ifdef _MY_WINDOWS_
                c = _getch();
                #elif defined(_MY_LINUX_)
                c = getchar();
                #endif

                if(n == 0) {
                    continue;
                }

                length = num.size();

                #ifdef _MY_WINDOWS_
                if(c == 72)
                #elif defined(_MY_LINUX_)
                if(c == 65)
                #endif
                
                {//上箭头
                    if(idx==0) {
                        continue;
                    }
                    else if(idx==n) {
                        tmp.clear();
                        tmp = num;
                    }

                    for(i=0; i<length; i++)
                        cout << "\b \b";
                    num = records[--idx];
                    cout << num;

                    arrow = 1;
                }

                #ifdef _MY_WINDOWS_
                if(c == 80)
                #elif defined(_MY_LINUX_)
                if(c == 66)
                #endif

                {//下箭头
                    if(idx==n) {
                        continue;
                    }

                    for(i=0; i<length; i++)
                        cout << "\b \b";

                    if(idx==n-1) {
                        num = tmp;
                        idx++;
                    }
                    else num = records[++idx];
                    cout << num;

                    arrow = 1;
                }
            }
        }
        
        else if(c==3) {//Ctrl+C
            cout << "\n^C\n";
            #ifdef _MY_LINUX_
            tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
            #endif
            exit(0);
        }
    }
    cout << '\n';

    #ifdef _MY_LINUX_
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
    #endif


    invalid_num_check:

    cout << "确认?[Y/n] ";
    string check;
    cin >> check;
    cin.ignore();

    if(check == "n") {
        num.clear();
        goto read_num_again;
    }
    else if(check != "Y") {
        if(num.empty())
            goto read_num_again;
        else
            goto invalid_num_check;
    }

    if(history.is_open()) {
        for(size_t i = 0; i < records.size(); i++) {
            if(records[i] == num) {
                records.erase(records.begin() + i);
                break;
            }
        }
        records.push_back(num);
        history.close();
        history.open("./config/numHistory.txt", ios::out | ios::trunc);
        for(const auto& r : records) {
            history << r << '\n';
        }
        history.close();
    }
}



//定义安全抹除函数
void secure_erase(string &s) {
    if (s.empty()) return;
    // 使用 volatile 指针防止编译器优化掉抹除操作
    volatile char* p = const_cast<volatile char*>(s.data());
    size_t n = s.size();
    while (n--) *p++ = '\0';
    s.clear(); 
}

int main(void)
{
    #ifdef _MY_WINDOWS_
    system("chcp 65001 > nul");//设置为UTF-8编码页
    #endif
    
    string script_name;
    read_script_name(script_name);

    string num;
    read_num(num);
    
    string password;
    read_password(password);

    // 1. 打开指向 Node.js 进程的管道 ("w" 表示我们要向它写入数据)
    FILE* node_pipe = POPEN(("node " + script_name).c_str(), "w");

    if(!node_pipe) {
        perror("无法启动Node.js进程");
        return 1;
    }

    // 2. 将学号和密码写入管道（传输给 Node.js）
    fprintf(node_pipe, "%s %s\n__END_OF_SECRET__\n", num.c_str(), password.c_str());
    fflush(node_pipe);
    cout << "\n[C++] 敏感数据安全传输至" << script_name << endl;
    
    // 3. 安全：抹除 C++ 内存中的学号和密码
    secure_erase(num);
    secure_erase(password);

    //4. 转发逻辑
    if(strstr(script_name.c_str(), "autosign.js") == nullptr) {
        cout << "[C++] 已进入交互模式" << endl;

        char ch;
        #ifdef _MY_WINDOWS_
            while (true) {
                if (_kbhit()) { // 检测按键
                    ch = _getch();
                    if (ch == 3) break; // Ctrl+C 退出
                    fputc(ch, node_pipe);
                    fflush(node_pipe);
                }
            }
        #elif defined(_MY_LINUX_)
            struct termios old_t, new_t;
            tcgetattr(STDIN_FILENO, &old_t);
            new_t = old_t;
            new_t.c_lflag &= ~(ICANON | ECHO); // 禁用行缓冲和回显
            tcsetattr(STDIN_FILENO, TCSANOW, &new_t);

            while (read(STDIN_FILENO, &ch, 1) > 0) {
                fputc(ch, node_pipe);
                fflush(node_pipe);
                // 简单的退出机制：如果管道断了或者收到特定信号
            }
            
            tcsetattr(STDIN_FILENO, TCSANOW, &old_t); // 恢复原始终端属性
        #endif
    }

    //5. 关闭管道
    PCLOSE(node_pipe);

    return 0;
}

void maintain_history(fstream &history, vector<string> &records)
{
    records.clear();
    history.clear();
    history.seekg(0);
    string line;
    while(getline(history, line)) {
        records.push_back(line);
    }
}