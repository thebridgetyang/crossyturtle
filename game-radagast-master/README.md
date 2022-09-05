**Work Log**<br>
Wednesday, May 18; 8:00-12:00PM <br>
Who: Bridget, Pranay, Jay, Joe <br>
What: Ported over files; discussed and started skeleton.<br>
Bugs: N/A.<br>
<br>
Monday, May 23<br>
Who: Bridget and Pranay <br>
What: Worked on draw_sprite function for sdl wrapper<br>
Bugs: Images not working.<br>
<br>
Tuesday, May 24; 6:00-10:00PM <br>
Who: Bridget, Pranay, Joe <br>
What: Continued on draw_sprite and making scenes for game.<br>
Bugs: N/A.<br>
<br>
Tuesday, May 24; 10:00-12:00PM <br>
Who: Jay, Joe <br>
What: Cleaned up sdl_wrapper and fixed bugs.<br>
Bugs: N/A.<br>
<br>
Wednesday, May 25; 6:00-9:00PM <br>
Who: Jay, Bridget <br>
What: Tried to figure out mouse click.<br>
Bugs: Couldn't get the screen to change on mouse click.<br>
<br>
Thursday, May 26; 6:00-8:00PM <br>
Who: Joe <br>
What: Implemented audio. Added background music and sound effects.<br>
Bugs: N/A.<br>
<br>
Thursday, May 26; 8:00-9:00PM, 11:00-1:00 AM <br>
Who: Jay, Bridget <br>
What: Working on mouse click for start screen. Added the car obstacles to the scene and trying to get them to move.<br>
Bugs: N/A.<br>
<br>
Monday, May 30; 10:00-1:00AM<br>
Who: Jay, Bridget, Joe, Pranay <br>
What: Jay and Bridget designed more graphics, added more random color cars to the road, and implemented the car/turtle collision. Joe helped debug this and worked on the audios. Pranay started implementing the coin/point system.<br>
Bugs: N/A.<br>
<br>
Tuesday, May 31; 6:00-10:00PM <br>
Who: Bridget, Pranay, Joe <br>
What: Implemented screen pan and body types and worked on randomly spawning grass and road background strips, rather than always alternating between the two. Joe also reeorganized the code/made data structures.<br>
Bugs: N/A.<br>
<br>
Tuesday, May 31; 10:00-12:00PM <br>
Who: Joe, Jay <br>
What: Fixed/improved car spawning and modified turtle movement so that the turtle moved fully from one strip to the next.<br>
Bugs: N/A.<br>
<br>
Wednesday, June 1; 6:00-10:00PM <br>
Who: Jay, Bridget, Pranay <br>
What: Fixed the background so that there was no longer a little white line between each strip of grass/road. Got the screen to display the lives and coin counter using SDL_TTF. Pranay continued working on the coins feature. Using the same logic/framework as the coins feature code, we then worked on the lives feature.<br>
Bugs: Had a hard time figuring out how to work with SDL_TTF. Had to consult numerous online resources.<br>
<br>
Thursday, June 2 ; 12:00PM-4:00PM <br>
Who: Joe<br>
What: Fixed bugs and implemented handler for going below screen.<br>
Bugs: N/A.<br>
<br>
Thursday, June 2 ; 8:00PM-5:00AM <br>
Who: Joe, Bridget, Jay, Pranay <br>
What: Joe modified the collision of the turtle with the cars; he made the turtle spin when it collided with the cars instead of just getting pushed out of the scene. He also made the turtle die/lives go down by 1 if it didn't move forward for too long and got absorbed by the screen. We added more obstacles (rocks to the grass) that the user has to go around to move forward and worked on the endscreen. Jay also added the feature that makes the turtle "invisible" for 2 seconds after it dies.<br>
Bugs: Final coin points were not displaying on the end screen.<br>
<br>
Thursday, June 2; 8:00-9:00PM, 11:00-2:30AM <br>
Who: Jay, Bridget, Pranay <br>
What: Pranay finished up the lives powerup. Bridget added the ghost invincibility powerup to the game. Jay, Bridget, and Pranay tried to debug the powerups and audio memory leak.<br>
Bugs: For some reason, the ghost powerup was not showing up even though the logic of the code was correct and there were no errors. Jay, Bridget, and Pranay all tried to debug it together; Jay figured out that it was because of the order we put the GHOST body type in in the typedef enum struct. The audio was throwing an "Uncaught RuntimeERROR: table index is out of bounds" error in the console if we ran the game for 5 minutes or greater. After scouring the Internet and wracking all our brains, we still could not figure it out, so we asked the TA's for help via Discord.<br>
<br>
Friday, June 3; 11:00AM-1:30PM <br>
Who: Jay, Bridget, Pranay <br>
What: Pranay fixed the audio memory leak. Jay designed the directions screen; Bridget coded it into game. Bridget also finally got the end screen to display the number of coins collected and made other minor changes to the aesthetics of the game. Jay and Bridget also fixed the car spawning, so the cars spawn more frequently and randomly.<br>
Bugs: N/A.<br>
<br>
Friday, June 3; 4:30PM-5:30PM <br>
Who: Joe <br>
What: Fixed bugs related to object spawning, audio, mouse clicking, and memory leaks.<br>
Bugs: Problem in which final screen words are handled; error when attempted to be freed.<br>

