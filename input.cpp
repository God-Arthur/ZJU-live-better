#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#include <filesystem>

#if __cplusplus >= 201703L
#include <filesystem>
namespace fs = std::filesystem;
#elif defined(_WIN32) || defined(__linux__)
// 兼容C++11/14的旧版experimental filesystem
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#ifdef _MY_WINDOWS_

#define POPEN _popen
#define PCLOSE _pclose

#include <conio.h>

void set_utf8_codepage() {
    system("chcp 65001 > nul");
}

void read_script_name(string &script_name)
{
    
    char c;

    read_script_name_again:
    cout << "输入脚本路径：.js\b\b\b";
    script_name.clear();

    while((c = _getch()) != '\r') {
        cout << "   \b\b\b";
        if(c == '\b') {
            if(!script_name.empty()) {
                script_name.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            script_name += c;
            cout << c << ".js\b\b\b";
        }
    }
    script_name += ".js";
    cout << '\n';

    if(!fs::exists(fs::path(script_name))) {
        cout << "脚本不存在，请检查输入\n";
        script_name.clear();
        goto read_script_name_again;
    }


    cout << "以下命令将执行，可继续添加参数，直接回车则参数为空：\n";
    cout << "node " << script_name << ' ';

    string arg;
    arg.clear();

    while((c = getchar()) != '\n'){
        arg += c;
    }
    if(!arg.empty())
        script_name += " " + arg;
}
void read_password(string &password)
{
    read_password_again:
    cout << "输入密码：";
    password.clear();
    char c;
    while((c = _getch()) != '\r') {
        if(c == '\b') {
            if(!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            cout << "*";
            password += c;
        }
    }
    cout << '\n';

    invalid_password_check:
    cout << "确认?[y/n] ";
    string tmp;
    cin >> tmp;
    cin.ignore();
    if(tmp == "n") {
        password.clear();
        goto read_password_again;
    }
    else if(tmp != "y") {
        goto invalid_password_check;
    }
}

#elif defined(_MY_LINUX_)

#define POPEN popen
#define PCLOSE pclose

#include <termios.h>
#include <unistd.h>
void read_script_name(string &script_name)
{
    
    struct termios old_attr, new_attr;
    tcgetattr(STDIN_FILENO, &old_attr);

    // 关闭回显
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);

    char c;

    read_script_name_again:
    cout << "输入脚本路径：.js\b\b\b";
    script_name.clear();

    while((c = getchar()) != '\n') {
        cout << "   \b\b\b";
        if(c == 127)/*退格键*/ {
            if(!script_name.empty()) {
                script_name.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            script_name += c;
            cout << c;
        }
        cout << ".js\b\b\b";
    }
    script_name += ".js";
        
    cout << '\n';

    if(!fs::exists(fs::path(script_name))) {
        cout << "脚本不存在，请检查输入\n";
        goto read_script_name_again;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);


    cout << "以下命令将执行，可继续添加参数，直接回车则参数为空：\n";
    cout << "node " << script_name << ' ';

    string arg;
    arg.clear();

    while((c = getchar()) != '\n'){
        arg += c;
    }
    if(!arg.empty())
        script_name += " " + arg;
}
void read_password(string &password)
{
    read_password_again:

    cout << "输入密码：";
    password.clear();
    struct termios old_attr, new_attr;
    tcgetattr(STDIN_FILENO, &old_attr);

    // 关闭回显
    new_attr = old_attr;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);

    char c;
    while((c = getchar()) != '\n') {
        if(c == 127)/*退格键*/ {
            if(!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else if(c>=32 && c<=126) {
            password += c;
            cout << "*";
        }
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);

    cout << '\n';

    invalid_password_check:
    cout << "确认?[y/n] ";
    string tmp;
    cin >> tmp;
    cin.ignore();
    if(tmp == "n") {
        password.clear();
        goto read_password_again;
    }
    else if(tmp != "y") {
        goto invalid_password_check;
    }
}
#endif

int main()
{
    #ifdef _MY_WINDOWS_
    set_utf8_codepage();
    #endif
    
    string script_name;
    read_script_name(script_name);

    read_num_again:

    cout << "输入学号：";
    string str;
    cin >> str;
    str = str.substr(0, 10);

    cin.ignore();

    invalid_num_check:
    cout << "确认?[y/n] ";
    string tmp;
    cin >> tmp;
    cin.ignore();
    if(tmp == "n") {
        str.clear();
        goto read_num_again;
    }
    else if(tmp != "y") {
        goto invalid_num_check;
    }

    string password;
    read_password(password);

    // 1. 打开指向 Node.js 进程的管道 ("w" 表示我们要向它写入数据)
    FILE* node_pipe = POPEN(("node " + script_name).c_str(), "w");

    if(!node_pipe) {
        perror("无法启动Node.js进程");
        return 1;
    }

    // 2. 将学号和密码写入管道（传输给 Node.js）
    fprintf(node_pipe, "%s %s\n__END_OF_SECRET__\n", str.c_str(), password.c_str());
    fflush(node_pipe);
    cout << "\n[C++] 敏感数据安全传输至" << script_name << endl;
    
    // 3. 安全：抹除 C++ 内存中的学号和密码
    str.assign(str.size(), '\0');
    password.assign(password.size(), '\0');

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