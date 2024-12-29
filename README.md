# CG-FinalProject

 ![](https://img.shields.io/badge/platform-linux-brightgreen.svg)

## 环境配置

for Linux:
```
sudo apt install -y mesa-utils libgl1-mesa-dev libglew-dev libglfw3-dev build-essential libglm-dev libsfml-dev
export DISPLAY=:0

cd code
mkdir build
cd build
cmake ..
make
./firework
```

for Mac:
```shell
brew install llvm libomp glew glfw3 sfml glm
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
```

运行 python 脚本请使用低于 3.13 版本 python, 并安装 `opencv-python`：

```shell
pip install python-opencv
# mamba install py-opencv
# conda install py-opencv
```

## 烟花

`ParticleRenderer` 是烟花发射器


## 常见问题

1. 如果运行出现报错：
    ```
    MESA: error: ZINK: failed to choose pdev
    glx: failed to create drisw screen
    ```

    可以尝试将MESA驱动更新到最新版本：
    ```
    sudo add-apt-repository ppa:kisak/kisak-mesa
    sudo apt update
    sudo apt upgrade
    ```

## Code Review 标准（暂定）
1. 格式化

    VSCode Pritter插件，默认配置

2. 命名 
    - 变量、函数名等，用词准确
    - 类名以及相关的头文件名和源文件名，统一用大驼峰命名法
    - 其余的变量名、函数名、文件名，统一用小驼峰命名法，专有名词除外。

3. 注释
    - 统一使用`//`以及中文进行注释
    - 类成员变量在声明时添加注释
    - 类成员函数及其他函数在实现的开头添加注释
    - 函数内部，每一个逻辑块之前添加注释

4. 全局变量以及函数的抽象

    视情况进行抽象，单个函数不宜过长