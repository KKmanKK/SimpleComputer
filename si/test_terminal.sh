#!/bin/bash

# Clear screen
echo -e "\033[H\033[2J"

# Row 5, col 10: name — red text on black background
echo -e "\033[5;10H\033[31m\033[40mДаниил\033[0m"

# Row 6, col 8: group — green text on white background
echo -e "\033[6;8H\033[32m\033[47mЗП-31\033[0m"

# Move cursor to row 10, col 1 and reset colors
echo -e "\033[10;1H\033[0m"
