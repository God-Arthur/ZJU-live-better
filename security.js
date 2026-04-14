// security.js
import fs from 'fs';

export default function secureLoad(callback) {
    let buffer = Buffer.alloc(0);

    const onData = async (chunk) => {
        // 1. 持续拼接 C++ 传来的二进制数据
        buffer = Buffer.concat([buffer, chunk]);
        const content = buffer.toString();
        
        // 2. 检查是否收到了约定的“结束标志”
        if (content.includes("__END_OF_SECRET__")) {
            // 重要：拦截到秘密后，立刻停止监听 data 事件
            // 这样后续从管道进来的“按键数据”才会直接交给 inquirer 处理器
            process.stdin.removeListener('data', onData);

            // 解析：学号 密码\n__END_OF_SECRET__
            const lines = content.split('\n');
            const firstLine = lines[0].trim();
            const spaceIndex = firstLine.indexOf(' ');
            
            const studentId = firstLine.slice(0, spaceIndex);
            const password = firstLine.slice(spaceIndex + 1);

            try {
                // 3. 这里的 callback 就是 quizanswer.js 里的业务逻辑
                await callback(studentId, password);
            } finally {
                // 安全清理
                buffer.fill(0);
            }
        }
    };

    // 启动监听
    process.stdin.on('data', onData);

    // 确保流是活跃的，能够接收后续 C++ 转发的键盘按键
    process.stdin.resume();
};