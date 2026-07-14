#!/usr/bin/env python3
"""
SCBB Menuconfig - Configuration tool for AiPi-SCBB
Based on Kconfig-style configuration
"""

import os
import sys
import json
import argparse
from pathlib import Path

# Default configuration
DEFAULT_CONFIG = {
    "SCBB_CH224A": False,
    "SCBB_SHT3X": False,
    "SCBB_WS2812": False,
    "SCBB_HXD039B2": False,
    "SCBB_USE_BSP": False,
    "SCBB_BSP_I2C_HEADER": "stm32f10x_bsp_i2c.h",
    "SCBB_BSP_UART_HEADER": "stm32f10x_bsp_uart.h",
    "SCBB_BSP_SPI_HEADER": "stm32f10x_bsp_spi.h",
    "SCBB_BSP_GPIO_HEADER": "stm32f10x_bsp_gpio.h",
    "SCBB_BSP_DELAY_HEADER": "stm32f10x_delay.h",
}

# Menu structure
MENUStructure = {
    "title": "AiPi-SCBB Configuration",
    "items": [
        {
            "type": "menu",
            "title": "Modules",
            "items": [
                {"type": "bool", "key": "SCBB_CH224A", "title": "Enable CH224A USB-PD driver", "desc": "I2C protocol"},
                {"type": "bool", "key": "SCBB_SHT3X", "title": "Enable SHT3x temperature/humidity sensor", "desc": "I2C protocol"},
                {"type": "bool", "key": "SCBB_WS2812", "title": "Enable WS2812 LED strip driver", "desc": "PWM+DMA protocol"},
                {"type": "bool", "key": "SCBB_HXD039B2", "title": "Enable HXD039B2 IR encoder/decoder", "desc": "UART protocol"},
            ]
        },
        {
            "type": "menu",
            "title": "BSP Configuration",
            "items": [
                {"type": "bool", "key": "SCBB_USE_BSP", "title": "Use built-in STM32F10x BSP", "desc": "Disable for custom BSP"},
                {
                    "type": "menu",
                    "title": "Custom BSP Headers",
                    "condition": "SCBB_USE_BSP==False",
                    "items": [
                        {"type": "string", "key": "SCBB_BSP_I2C_HEADER", "title": "I2C BSP header file", "desc": "Header filename for I2C"},
                        {"type": "string", "key": "SCBB_BSP_UART_HEADER", "title": "UART BSP header file", "desc": "Header filename for UART"},
                        {"type": "string", "key": "SCBB_BSP_SPI_HEADER", "title": "SPI BSP header file", "desc": "Header filename for SPI"},
                        {"type": "string", "key": "SCBB_BSP_GPIO_HEADER", "title": "GPIO BSP header file", "desc": "Header filename for GPIO"},
                        {"type": "string", "key": "SCBB_BSP_DELAY_HEADER", "title": "Delay BSP header file", "desc": "Header filename for delay"},
                    ]
                }
            ]
        }
    ]
}


class MenuConfig:
    def __init__(self, config_file="scbb_config.json"):
        self.config_file = Path(config_file)
        self.config = DEFAULT_CONFIG.copy()
        self.load_config()
    
    def load_config(self):
        if self.config_file.exists():
            with open(self.config_file, 'r') as f:
                saved = json.load(f)
                self.config.update(saved)
    
    def save_config(self):
        with open(self.config_file, 'w') as f:
            json.dump(self.config, f, indent=2)
    
    def display_menu(self, menu, indent=0):
        prefix = "  " * indent
        print(f"\n{prefix}=== {menu['title']} ===")
        
        for item in menu['items']:
            if item['type'] == 'menu':
                # Check condition
                if 'condition' in item:
                    if not self.check_condition(item['condition']):
                        continue
                self.display_menu(item, indent + 1)
            elif item['type'] == 'bool':
                value = self.config.get(item['key'], False)
                status = "[*]" if value else "[ ]"
                print(f"{prefix}  {status} {item['title']}")
                print(f"{prefix}      {item['desc']}")
            elif item['type'] == 'string':
                value = self.config.get(item['key'], "")
                print(f"{prefix}  {item['title']}: {value}")
                print(f"{prefix}      {item['desc']}")
    
    def check_condition(self, condition):
        """Simple condition checker"""
        if "==" in condition:
            key, value = condition.split("==")
            key = key.strip()
            value = value.strip()
            if value == "True":
                return self.config.get(key, False) == True
            elif value == "False":
                return self.config.get(key, False) == False
        return True
    
    def interactive_config(self):
        print("\n" + "="*50)
        print("AiPi-SCBB Menuconfig")
        print("="*50)
        
        while True:
            self.display_menu(MENUStructure)
            print("\n" + "-"*50)
            print("Commands:")
            print("  t <num>  - Toggle boolean option")
            print("  s <num> <value> - Set string option")
            print("  save     - Save configuration")
            print("  quit     - Exit without saving")
            print("  q        - Save and quit")
            
            choice = input("\nEnter command: ").strip()
            
            if choice == 'quit':
                return False
            elif choice == 'q' or choice == 'save':
                self.save_config()
                print("Configuration saved!")
                return True
            elif choice.startswith('t '):
                try:
                    num = int(choice.split()[1])
                    self.toggle_option(num)
                except (ValueError, IndexError):
                    print("Invalid command")
            elif choice.startswith('s '):
                try:
                    parts = choice.split(maxsplit=2)
                    num = int(parts[1])
                    value = parts[2] if len(parts) > 2 else ""
                    self.set_string_option(num, value)
                except (ValueError, IndexError):
                    print("Invalid command")
    
    def get_all_options(self):
        """Get flat list of all configurable options"""
        options = []
        self._collect_options(MENUStructure, options)
        return options
    
    def _collect_options(self, menu, options):
        for item in menu['items']:
            if item['type'] == 'menu':
                if 'condition' in item:
                    if not self.check_condition(item['condition']):
                        continue
                self._collect_options(item, options)
            else:
                options.append(item)
    
    def toggle_option(self, num):
        options = self.get_all_options()
        bool_options = [o for o in options if o['type'] == 'bool']
        if 0 <= num < len(bool_options):
            key = bool_options[num]['key']
            self.config[key] = not self.config.get(key, False)
        else:
            print("Invalid option number")
    
    def set_string_option(self, num, value):
        options = self.get_all_options()
        string_options = [o for o in options if o['type'] == 'string']
        if 0 <= num < len(string_options):
            key = string_options[num]['key']
            self.config[key] = value
        else:
            print("Invalid option number")
    
    def generate_cmake(self):
        """Generate CMake cache entries from config"""
        lines = []
        lines.append("# Auto-generated from menuconfig")
        for key, value in self.config.items():
            if isinstance(value, bool):
                lines.append(f"set({key} {'ON' if value else 'OFF'} CACHE BOOL \"\" FORCE)")
            else:
                lines.append(f'set({key} "{value}" CACHE STRING "" FORCE)')
        return "\n".join(lines)
    
    def generate_header(self):
        """Generate C header from config"""
        lines = []
        lines.append("// Auto-generated from menuconfig")
        lines.append("#ifndef SCBB_CONFIG_H")
        lines.append("#define SCBB_CONFIG_H")
        lines.append("")
        for key, value in self.config.items():
            if isinstance(value, bool):
                if value:
                    lines.append(f"#define {key}_ENABLED 1")
            else:
                lines.append(f'#define {key} "{value}"')
        lines.append("")
        lines.append("#endif // SCBB_CONFIG_H")
        return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description="AiPi-SCBB Menuconfig")
    parser.add_argument("--config", default="scbb_config.json", help="Config file path")
    parser.add_argument("--cmake", action="store_true", help="Generate CMake file")
    parser.add_argument("--header", action="store_true", help="Generate C header file")
    parser.add_argument("--output", help="Output file path")
    args = parser.parse_args()
    
    menuconfig = MenuConfig(args.config)
    
    if args.cmake:
        output = args.output or "scbb_config.cmake"
        with open(output, 'w') as f:
            f.write(menuconfig.generate_cmake())
        print(f"CMake config written to {output}")
    elif args.header:
        output = args.output or "scbb_config.h"
        with open(output, 'w') as f:
            f.write(menuconfig.generate_header())
        print(f"Header written to {output}")
    else:
        menuconfig.interactive_config()


if __name__ == "__main__":
    main()
