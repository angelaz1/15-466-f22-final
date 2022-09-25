# (TODO: your game's title)

Author: Jason Xu, Ashley Kim, and Will Ozeas

Design: Our game is a puzzle-text adventure, where you must use information 
from previous runs of the game to survive a zombie-infested lab! Further spoiler details are in WALKTHROUGH.txt.

Text Drawing: (TODO: how does the text drawing in this game work? Is text precomputed? Rendered at runtime? What files or utilities are involved?)

Choices: Every screen is stored as a .txt file. Each file starts with the main text, then uses brackets to denote a choice, followed by
a file name in parentheses. Our code will parse this text automatically and gives us a vector of choices to render and a map of text entries
to file names, and if matching text is entered, the corresponding file is loaded.

Screen Shot:

![Screen Shot](screenshot.png)

How To Play:

At each screen, you may type in text and then hit enter to move rooms. Numbers indicate choices, but that's not all you can type...

Sources: Roboto Medium font from https://fonts.google.com/specimen/Roboto/about?query=roboto

Rubik Dirt font from rom https://fonts.google.com/specimen/Rubik+Dirt/about?query=rubik

Patua One font from https://fonts.google.com/specimen/Patua+One/about?query=patua+one

This game was built with [NEST](NEST.md).

