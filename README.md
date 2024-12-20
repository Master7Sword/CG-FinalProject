# CG-FinalProject

 ![](https://img.shields.io/badge/platform-linux-brightgreen.svg)

## 环境配置

```
sudo apt install -y mesa-utils libgl1-mesa-dev libglew-dev libglfw3-dev build-essential libglm-dev libsfml-dev
export DISPLAY=:0

cd code
mkdir build
cd build
cmake ..
make
./Skybox_Project
```

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