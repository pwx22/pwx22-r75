![lazercore1](https://github.com/user-attachments/assets/e7b6cfb6-69a9-4dbc-b367-30854e5d35ae)
## ⚠️ Disclaimer
> [!CAUTION]
> This is a custom firmware that could break your keyboard.
>
> 🛑 **Use at your own risk!** 🛑

## Features  
- **VIA Enabled**: Easily remap keys, configure layers, and customize RGB lighting using the [Via configurator](https://usevia.app/).  
- **CapsWord**: Press `Left Shift + Right Shift` to type a word in all caps.  
- **Sentence Case**: Press `Fn + Caps Lock` to toggle sentence case mode. Automatically capitalizes the first letter of a sentence.  
- **Game Mode**: Press `Fn + Right Shift + G` to toggle game mode.
    - **SOCD Filtering**: It is a feature that prioritizes the latest input between two selected keys without the need to release the previous one, allowing for faster directional changes.
    - Custom RGB Matrix with highlighted WASD.
    - RGB Mode remains adjustable in Game Mode.
- **Audio Visualizer Mode**(layer 3):
  - Activate with `Fn + Right Shift + Volume Knob`.  
  - Real-time reactive audio visualizer powered by [Lazersync](https://github.com/pk-vishnu/lazersync).  
- **Light Indicators**: Visual indicators for active layers.
- **Knob controls**: Volume control on layer 0, Screen brightness on layer 1, Media FastForward/Rewind on layer 2.
- **DFU Mode**: Enter bootloader mode with `Fn + Shift + Esc` for flashing firmware.
- **Clear EEPROM**: Press `Fn + Space` to clear EEPROM.
#### RGB Controls  
- **Brightness**: `Fn + Up/Down Arrows`.  
- **Effect Speed**: `Fn + Left/Right Arrows`.  
- **RGB Mode**: `Fn + \` (Back Slash).
- Further Customization: Use the [Via Configurator](https://usevia.app/).
---

## Getting Started  

### Prerequisites  

#### To Flash Firmware  
1. Install [QMK Toolbox](https://github.com/qmk/qmk_toolbox) to flash firmware onto your keyboard.  

#### To Compile Firmware  
1. Set up your [QMK environment](https://docs.qmk.fm/newbs_getting_started) to compile and modify the firmware source code.  

---

### Flashing the Firmware  

1. **Download the Firmware**:  
   - Clone this repository or download the `.hex` file from the `Compiled` folder.  

2. **Flash the Firmware**:  
   - Enter DFU mode by pressing `Fn + Shift + Esc`.  
   - Alternatively, press the `ESC` key while plugging in the USB cable, or use the reset button located under the space bar.  
   - Use **QMK Toolbox** to flash the `lazercore.hex` file to your keyboard.  
   
---

### Compiling the Firmware  

1. Copy the `RK75` folder to the `keyboards` folder in your QMK firmware environment.  
2. Run the following command to compile the firmware:  
     ``` qmk compile -kb RK75 -km lazercore ``` (or) ```make RK75:lazercore -j``` 

## Contributing  

Contributions, bug reports, and feature requests are welcome!  

---

## Acknowledgments  
- Special thanks to [@sdk66](https://github.com/sdk66), [@snakkarike](https://github.com/snakkarike), [@irfanjmdn](https://github.com/irfanjmdn), and [@iamdanielv](https://github.com/iamdanielv) for their contributions to RK keyboards, providing a solid foundation for this project.
- Grateful to the [QMK Community](https://qmk.fm/) for their extensive documentation and support.  
---  
