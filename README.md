

## Update OFFSETS
1. Make sure you have properly formatted offsets.h and offsets.ini.
2. Run the python script like this.
    ```shell
    py update.py offsets.h offsets.ini
    ```

## Getting Started

There are really only two steps:

1. Run the game on a windows guest in a kvm virtual machine.
2. Run the compiled apex_dma program on the Linux host.
    ```shell
    sudo ./apex_dma
    ```



Additional information:


1. Please put the overlay window on the top of the VM screen after start. For example, on top of the looking-glass window.
2. The window can't get the focus at the same time with the VM screen,  please passthrough some keyboard, mouse or joystick into the VM,  otherwise you can't operate the game.
3. Press insert key on the overlay window to open the overlay menu.
4. If you are using a resolution other than 1080p, save the configuration and then modify the `screen_width` and `screen_height` in *settings.toml* and reload the configuration.



Click on *[Actions](https://github.com/Jotalz/apex_dma_kvm_pub/actions)* to download the auto-built artifacts.

Or compile it yourself.



## Build from source

**Requirements:**

* C++ toolchain
* Rust toolchain
* CMake
* Git

**Install Rust:**

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

**Install Build Dependencies (Ubuntu):**

```bash
sudo apt install cmake clang libzstd-dev libglfw3-dev libfreetype6-dev libvulkan-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev wayland-protocols libwayland-dev libxkbcommon-dev
```

**Build:**

```shell
git clone --recurse https://github.com/Jotalz/apex_dma_kvm_pub
cd apex_dma_kvm_pub
git submodule update --init --recursive
cd apex_dma
bash ./build.sh
```



