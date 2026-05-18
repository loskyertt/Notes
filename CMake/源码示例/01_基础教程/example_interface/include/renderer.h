#pragma once

// 接口定义 - 不包含实现
class IRenderer {
public:
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void cleanup() = 0;
};

// 工厂函数声明 - 实现由使用者提供
extern IRenderer* createRenderer();
