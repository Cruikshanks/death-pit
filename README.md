# Death Pit

> Do not use this as a current reference. This code is now almost 20 years old!

<img src="/screenshots/title_screen.gif" alt="Screenshot of Death Pit title screen" />

<img src="/screenshots/game_screen.jpg" alt="Screenshot of Death Pit gameplay" />

This was my final foray into using DirectX and C++ back in 2002 after [Pongy II](https://github.com/Cruikshanks/pongy2).

I first created the game in [Blitz BASIC](https://en.wikipedia.org/wiki/Blitz_BASIC) and then converted it to DirectX 8 and C++.

'Death Pit' involves the player moving a sprite around the screen, collecting gold coins whilst avoiding the sides of the playing area and 'instant' death obstacles placed within it. Both the coins and the obstacles are randomly placed for each game and level, although the program will not overlay them as this is seen as unfair to the player.

The difficulty comes in the fact that with each coin collected, the sprite moves faster, by a factor set depending on the current level. Once in motion, the sprite cannot be stopped.

This was the foundation of the game and from it other features evolved. For example, with enough time most people will complete each of the levels involved. So how do you instil a sense of panic and show a distinction between someone who completes a level in 30 seconds, and one who does it in 30 minutes? This was solved by adding a time limit of one minute to each level, one in which the game will end, lives or no lives when the counter reaches 0. A persons score is also based on how quickly they complete a level, both factors forcing the player to be conscious of time and thus adding an element of pace to proceedings.

### Text input

From my notes the one thing I failed to solve at the time was getting text input from the user

> 'Death Pit' features a high score table, which can read, or create a high score table when first started, then save it disk when finished. However one failing in my planning this conversion, was how to you get text input once your in full screen mode? I can think of one way, but until I grasp a more subtle solution, players scores are saved under the default name of PLAYER 1.

## Pre-requisites

- C++
- DirectX 8

## User Guide

Use the arrow keys to control your sprites direction. Collect all 20 coins on each level to move to the next. You have sixty seconds to complete each level. If the timer runs out its game over.

You have three lives per game, hit a troll or the sides and you loose a life.

## License

The gem is available as open source under the terms of the [MIT License](http://opensource.org/licenses/MIT).

> If you don't add a license it's neither free or open!