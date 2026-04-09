#!/bin/bash

# Clear screen
echo -e "\033[H\033[2J"

# Draw pseudo-graphic box starting at row 10, col 5, size 8x8
# Enter alternate charset: \033(0
# Exit alternate charset:  \033(B
# VT100 line-drawing chars: l=top-left, k=top-right, m=bottom-left, j=bottom-right
# q=horizontal, x=vertical

# Top border (row 10, cols 5-12)
echo -e "\033[10;5H\033(0lqqqqqqk\033(B"

# Side borders (rows 11-16)
for row in 11 12 13 14 15 16; do
    echo -e "\033[${row};5H\033(0x      x\033(B"
done

# Bottom border (row 17, cols 5-12)
echo -e "\033[17;5H\033(0mqqqqqqj\033(B"

# Draw digit 8 (last digit of birth day 18) inside the box
# using ACS_CKBOARD character ('a' in alternate charset = checkerboard block)
# Inner space: rows 11-16, cols 6-11 (6 wide x 6 tall)
echo -e "\033[11;6H\033(0aaaaaa\033(B"   # top bar
echo -e "\033[12;6H\033(0a    a\033(B"   # left and right sides
echo -e "\033[13;6H\033(0aaaaaa\033(B"   # middle bar
echo -e "\033[14;6H\033(0a    a\033(B"   # left and right sides
echo -e "\033[15;6H\033(0aaaaaa\033(B"   # bottom bar
# row 16 stays empty

# Move cursor below the drawing
echo -e "\033[20;1H"
