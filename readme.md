![lazercore1](https://github.com/user-attachments/assets/e7b6cfb6-69a9-4dbc-b367-30854e5d35ae)
> [!CAUTION]
> This is a custom firmware that could break your keyboard.
>
> 🛑 **Use at your own risk!** 🛑

## Features  

### VIA Enabled  
Easily remap keys, configure layers, and customize RGB lighting using the [Via Configurator](https://usevia.app/).  

### Sentence Case
Toggle sentence case mode with `Fn + Caps Lock`. The feature is off by default and capitalizes the first letter of each sentence when enabled.

### Win Lock
Toggle the Windows key lock with `Fn + Win`. When enabled, both operating-system keys are blocked and the Win key glows red on the base layer.

### SOCD Mode Cycle
Hold `Fn + Right Shift`, then press `S` to cycle simultaneous-opposite cardinal direction (SOCD) handling between **Last Input Wins**, **Neutral**, and **First Input Wins**. Each selection triggers dedicated function-row lighting feedback so you always know which mode is active.

### NKRO Toggle
Hold `Fn + Right Shift`, then press `N` to toggle between the default **6KRO** and **NKRO** reporting. Lighting feedback confirms the currently selected mode.

### Layer Lighting
- **Layer 0**: Solid orange (RGB 255, 95, 64) by default—fully adjustable in VIA.
- **Layer 1 (Fn)**: All keys are dark except **Enter**, **Right Shift**, **Caps Lock**, and **Win**, which illuminate according to their functions and toggle states.
- **Layer 2**: Highlights the SOCD (`S`) and NKRO (`N`) controls in purple and orange respectively.
- **Layer 3**: Emphasizes the DFU (`Esc`) and EEPROM Clear (`E`) controls in red.

### Knob Controls
- **Layer 0**: Volume control
- **Layer 1**: Screen brightness
- **Layer 2**: Media fast-forward/rewind

### DFU Mode
Hold `Fn`, keep `Enter` pressed (momentary Layer 3), then tap `Esc`. The whole board flashes red for 0.5 s before entering the bootloader.

### Clear EEPROM
Hold `Fn`, keep `Enter` pressed, then tap `E`. The keys `{3, 4, R, F, C, X, S, W}` flash red for 0.5 s before the EEPROM is cleared and the board resets.

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
    - Enter DFU mode by holding `Fn`, keeping `Enter` pressed, then tapping `Esc`.
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
