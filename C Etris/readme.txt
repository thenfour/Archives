///////////////////////////////////////////////////////////////////////////////////////
          ______    ________
         /              /                    *
        /___           /    ____ ____  __     ___
       /              /    /___/  /   /  \ /   //
      /______        /    |___   /   /    / //__

ETetriz (Elemental Tetriz)
1998 James corcoran

Resources used:
---------------
Borland C++ 5.01
MSVC 6
MSDN
Adobe Photoshop 5.0
3d Studio MAX R2.5
Microsoft DirectX 5 SDK

///////////////////////////////////////////////////////////////////////////////////////

12-12-98 11:36am Saturday

After nearly completing this program and then having it completely deleted off my HD,
it's time to start over again.

I don't expect to complete this quite as fast as I did at first because of school...it's
finals week and Christmas break is right around the corner.  The first version of this
took nearly 2 weeks to complete and I thought that was pretty quick.

But now that I've done it once, I guess that it will be more organized the second time.
I took a break from it for a while, though, so I have to dust off my DirectX chops and
even my C++ chops.

So now down to business.  This is a game of tetris with an interesting twist.  Instead of
your regular tetris game (they all say this), ETetriz stands for Elemental tetris.
Each block will be composed of an element, or something close.  In other words, there are
different kinds of blocks.  The blocks are Gravity, Fire, Earth, Ice, Water, Destruct,
Flammable.  Each block, besides simply being a tetris block, performs an effect on blocks
around it.

The gravity block makes all the blocks in that particular row "fall", leaving no white
space in it.  This block disappears after it is set down.

The fire block catches flammable blocks on fire, and melts Ice.  This block doesn't
disappear.  Fire also is put out by water (so when you melt ice, the resulting water
extinguishes the flame as well).

Earth - Does nothing.  Solid.
Ice - melts on fire, solid.
Water - Not solid, melts fire.
Destruct - Destroys all the blocks in the row in which you lay it.
Flammable - Sits there until fire hits it and turns into flames.  Solid.

Now this may seem ridiculous; there's no point to some blocks.  Like what's the
difference between earth, flammable, and fire?  They all just sit there like good little
pieces.  Well, since it's incredibly hard to keep a flammable block from catching on
fire somehow, the points are significantly higher for those pieces.  That's why.  So
the blocks listed in order of preciousness are as follows:

Flammable, Ice, Earth, Fire.

Now here's mre about the program.  It was written entirely in C++ under Win95 using
DirectDraw and DirectInput.  It was comprised of like 20 code files...10 cpp files and
10 .h files.  They were (if I can remember correctly)

graphics library
tetris main
tetris settings screen
tetris title screen
tetris game play
tetris playfield object
directinput library

and maybe some others, but that was the basic idea.

So today I'm going to try to rewrite the graphics library and get some stuff on the
screen maybe if I feel like it.



12-16-98 Wednesday 10:28 pm
Finals are over adn everyone is having fun; I'm going to program.  Right now the program
just sets up a window on the screen and now I'm going to set up the directX draw screen.
11:38pm I'm getting the DirectX stuff to work and it just locked up; I'll continue tomorrow.
It's something to do with the surfaces.
(1:30)

12-17-98 Thursday 12:32 pm
I'm back; I don't know what the problem was; it works now and I don't think I changed anything.
I just set up error trapping stuff, commented the suspect code, and slowly un-commented it,
and finally I was left with what I had before, but it worked this time.

1-11-99 Tuesday 10:00am.
Well finally I'm back here to work more on my tetriz project.  I just spent one minute figuring
out what went wrong - I forgot to set dwsize before messing with a DDSCAPS struct.  It's amazing
that I remember these things after being away from a computer for a month.  Well I guess it works
now and I will continue figuring out how to get stuff working.  On to bitmaps and surfaces and
such.

10:10 am I'm going to take a shower
12:00 pm Im' back now to get things working.  I have bitmaps displayed on the screen.  Next I'm
going to switch operation to the game portion of Tetriz and start the game.
12:47 I just got done typing in a bunch of data and stuff...I haven't actually started the game
yet like I said I would.
I also encountered a problem with the way I'm organizing this.  I can't pass variables between
modules as I wanted.  So I will have to go back to my old method (I hate trying new things only
to find it was better before).  That is where I simply do a #include "*.cpp" for all my code

One trip on IRC and I found a pretty good solution.  A man told me that it's common practice to
avoid making things global.  He said to pass it in the function.  I said I didn't want to have
to pass all those variables in each function.  He said make a STRUCT and I heard a DING and that
was it.

1:54 pm I just made a complete compile of my newly organized code.  About 2 hours today so far
It worked just fine; I have now a struct PASSVARS that includes the necessary DD stuff and some
extra things...works great.  One slight problem with not using any global variables is that
if I need to access variables within WNDPROC functions, They can't use them.  And you can't pass
anything to them.

6:16 pm I have tried to solve the problem by going on IRC once again.  I found out that you can
pass a pointer to a WNDPROC function by passing it in CreateWindow.  Then under the WM_CREATE:
message, LPARAM is the pointer.

9:18pm I'll give myself 5 hours today; 99% of that was all research on my thing.  I hate to admit
it but I gave up.  I finally just decided to make my passing struct global.  Didn't like doing
that at all, though.

(6:30)

1-13-99, 11:18am
I'm at the point where I can start programming the setup of the game.  I need to write the code
for a couple classes (Animation, Playfield).  Animation is for the animation of the blocks, and
playfield is basically the whole gameplay.  It will have members CheckForLines, DoEffects, etc..

I also have to start writing the DirectInput routines today.  I figure that now that I have the
organization down (hopefully), I will be able to move more smoothly.  I'm glad I learned so much
yesterday about extern, #ifndef, and I had more practice with pointers (as if I dont have enough
already, with DirecX).

1-14-98 5:43 pm
Maybe it was a bad idea, but I took yesterday off this project and wrote a database in VB...I
wrote it this time using the standard MSData controls.  I took a little break today and now
I'm here to program once again.  It's amazing how much I research when I program; I probably do
more research into different things than actual programming.

So where I am now is this:  The DirectDraw screen gets completely setup.  Now I am going to go
straight into the game.  I have to simultaneously write the game.cpp and playfield.cpp code...

6:27 I wrote a lot of the foundations for the playfield class, and now I am realizing that I have
a problem with my CreateOtherSurface function; I'm debugging that now.

7:29 Figured it out...when I would try to CreateSurface, I didn't have the DDSCAPS struct defined
correctly.  And I started to do more writing on the Animation class, and now I'm going to rest.

(8:30)

1-15-99 2:22pm
Well I've worked on it for 2 hours today.  I started where there was a blank slate in my new
classes PLAYFIELD and ANIMATION and I've written some code in there, written completely the
constructor for playfield, I've set up a message loop and now have a Frame() function in teh
game.cpp file...now we're almost ready for some real business.  I have a temporary thing set up
right now in Frame() that blits some and flips the screens.  I seem to have a strange problem,
though - when it blits a large area it's REALLY slow.  I don't know even where to begin on that,
but I'll get it soon.  Then I'm going to write some routines in the PLAYFIELD object to draw
shapes on the screen and stuff.
3:00 I figured it out; Flip() would return an error, because I had to set the DDFLIP_WAIT flag
to supress it.  Why it made it really slow, I really don't know, but it works now like it should.

(2:30 today)
4:30 now I'm going to write more of the Animation class and soon have everything working.

5:30 Progress.  I have written the DrawNext routine so I should have a shape on the screen.  I
like the way I've organized things, too.  The only problem is that I need a different way to
create my ANIMATION and PLAYFIELD objects.  I want to create them right in GameMain() but I
wanted them to be global.  I now realize that I can't do that and make it work, so I'm going to
create yet another PASSVARS struct for all 7 animations.  I'll do it later, after I rest a
little.
(3:30 today)

11:30 I just spent only 30 minutes and I cleaned up all that mess, cleaned up the header
files to make them #included more efficiently (a better way of making sure nothing is
included more than once), and finished writing a routine that draws a shape on the screen.
In other words, that was probably teh most productive half hour of this project.

I think that with that, I'll call it a night.

(12:30)

1-17-99 10:06 pm
I haven't worked on it in two days because I was writing a program I promised my friend.  It
was an ActiveDesktop HTML file creator.  It creates HTML code for links to paths on your computer.

But now I'm done with that; I'm pretty happy with it.

So now where will I begin here?  I need first to write a routine called DrawShape() that will
be like DrawNext() but will draw any shape anywhere...it will work like DrawNext, though...I'm
actually going to have DrawNext call it to draw the shape; I just have to determine how it will
know where to put it.  I want to pass the GRID coords to DrawShape, not the screen coords.  I
think it will be easier that way.  Maybe I'll make a routine called DrawShapeAnywhere() that
draws a shape anywhere on the screen.  DrawShape() will just call that and convert the coords
to screen coords.  DrawNext will just call DrawShapeAnywhere() with teh screen coords.   Yep...
10:26 OK; that's done,and I did a little bit of fixing-up and stuff...

Wha'ts next?  Well I guess it's time to start writing DirectInput routines, but I've programmed
for like 6 hours straight and that's definitely my limit; I'm going to rest now...I'll work on
this tomorrow.

1-19-99 7:02 pm
My project tonight is to get the directInput stuff going.  Just like DD, I'm going to have
routines in the library file...CreateDirectInputObject, and whatever else...I'll have to do a
little review with the sample SDK files.

CreateDirectInput
CreateDirectInputObject
SetDataFormat
SetBehavior
GetAccess

(13:30)
1-20-99 4:22 pm
Well alright, I quit last night after writing the stuff for like an hour and then finding out
I kept getting linker errors because it couldn't find the GUID stuff.  I don't know what to do.
I'll try to figure that out now.

4:29 I got it to compile, but I'm kinda suspicious.  I used this stuff before, and I never had to
do this to get it to work; I don't know why.  I had to put a #define INITGUID in my source.  Hmmm..

Well now I'll have to re-familiarize myself with how to read keyboard data and stuff.  Wait;
maybe I need to get some more stuff straight.  I need a printer to print out my H files so I can
see what my declarations; I can't stand going back and forth between files and cutting and pasting.

Let's outline how the game will be played:

First, the playfield is initialized.  It's given a random NEXT shape, and a random CURRENT shape.
GRID[][] is cleared.

Each frame that passes, we redraw the field after blitting a copy of the clean board on the screen.
On the frames that are a 0 % of a certain number, SPEED or something, we have to drop the
CURRENT shape down one step after checking for a collision.

To drop the shape, we just move the coordinates of the CURRENT shape.  I'll have to add that.
Also, the current shape is not evident in the GRID[][].  Only after a collision do we finally
add it's properties to the grid[][].

After it's entered into the grid[][], we do effects.  This gets complicated because of the
different effects.  I can't just scan through and do the effects; I will scan through because
that's our only apparent option, but it's not a matter of a simple switch() statement and a
command.  We do the effect by changing the status of a block.  We go until we'v scanned through
completely and if we made any changes, we need to go back and do it again until the board is
completely at a stand-still.  In other words, if you have an entire row of flammable blocks and
you lay down one fire block, the whole row must turn to fire; not just one block, as would be the
case if we were only to do one pass.

So let's generalize this:  We need a routine to setup initially in a game (clear the field and
set up the current/next shapes.

We need a routine to do one frame of the playfield.  It will redraw everything, pretty much.
There will be a routine to
We need a routine to go to the next shape

- Add coords to the CURRENT shape.

(14:30)
1-21-99 8:30pm
I'm working ont eh DirectInput routines and gameplay right now...I've been doing more talk than
programming because things around school are always coming up (like classes, damn them).

Well I've worked on getting a MoveRight() and MoveLeft() routine for the playfield object.  they
 are done, and I'm doing Advance frame.  I've fixed up some stuff and it's almost ready to go.

by ready to go I mean that I'll be seeing some shapes dropping and I'll be conrolling them.  I
still have a lot to go...

Let's see...
-------------
The game effects
DirectSound stuff
A text-display routine
a command button custom control class
the settings screen
Graphics
Title
Credits
Music/SFX

Well that should keep me busy...really, I should write a font creator and a bitmap convertor.
That way I can have my own standard for graphics and I will be able to make fonts - but actually,
I'm not making a private developing business, so maybe that's unnecessary.

11:39 I worked probably 2 hours or so tonight; I am using the keyboard to cycle through the
shapes that I see on the screen.  I have a Current shape and a Next shape; we're close now to
game play, except that the shapes are somehow screwed up...I have no idea.  Some are correct,
some are incorrect, but there isn't any pattern to it...I'll figure it out tomorrow.

(17:00)

1-22-99 12:59 pm
I'm pretty happy; one more hour of work and everything's apparantly bug-free and I'm actually
almost playing the game. Piecs are dropping, I'm controlling them, and they get to the bottom
and I start with another piece again.  It's really cool that I'm flying on this, too...the next
thing I have to do is figure out how I'm going to do the effects.

I think that when I "glue" the current piece to Grid[][] (after it it set down), I'll do the
effects.  I don't know why I am doubting this will work correctly.  First, I will scan through
to find any gravity or destruct blocks...they'll be handled on their own, completing their cycle.
Then I'll just do the old scan through and modify blocks.

after that cycle is complete, I'll check for lines.  I guess this way, if you have a line of
9 fire blocks in a row adn you set a Flammable block in there, the flammable block will turn to
fire before you get the line; I guess that's OK, but I'll think about making it so you get
the line before hand.  That wouldn't be technically correct, but I'll think about it.  I think
the player would like it better.  It would be one of those nice things that programmers do for
the player.

so a recap:  Glue the piece(done), Do Effecs, check for lines, update statistics, DONE.

(21:00)

1-23-99 11:07 am
I painted my keybard so that I can't see any of the keys; it surprises me every once in a while
when I find myself looking at the keyboard to find the "%" sign or something.

Yesterday I would say I spent 4 hours working on it; I got the effects done and I refined some
of the artwork to make it a little less epileptic.

I am now at the point where I write the code for the gravity/destruct effects.  Then I will
move on to bigger things.

Amazingly enough, in 15 minutes I modified some stuff and wrote the entire Gravity() code and
tested it, and it workes perfectly, apparantly.

7 minutes later, once again, flawless (hopefully).

Well now I've got the basic game play FINISHED.  I have decided to hold off on the statistics
thing until after a while; that shouldn't be that hard to do - just plug in some variables in
a struct named STATS or something and poke them in the code in various places.  When you hit
pause, you can see them.  So I'll need to write the text displaying thing first because I
WANT TO.

Oh, I need to write the check for lines thing.

I'm on a damn roll...20 minutes and I wrote it; works again just like clockwork.

I'm now going to start writing a text library.  This is for displaying text on the screen.
(duh).  Well I have to write a Font Editor in VB to do it, so I'll see you back here in C++ land
probably tomorrow, if not later.

Duh; what was I thinking.  I will just have a .bmp file that has the letters/numbers in it,
all fixed-width.  Then I will read them the same as the animation frames.  Then I can have them
be multi-color, or whatever I want.  and i won't have to do direct-pixel access stuff; that
is too slow for c++ (well, maybe not, but it was a good argument).

ABCDEFGHIJKLMNOPQRSTUVWXYZ
1234567890

12:09 am I have worked about 3 hours tonight on it for about 3.5 hours today...
I have text displaying, the game ending and a bunch of things fixed up, all the effects working,
line checking, and the beginnings of statistics checking.

tomorrow I'll integrate the rest of the statistics stuff (the display, pause), level stuff/
speed correction, and a problem I'm having with random numbers.  They're not being random,
even with #include <time.h> and randomize();   I don't know why...
(24:30)

1-24-99 1:30pm
Well I just wrote really quickly like 20 minutes the frame rate counter, and the displaynumber()
routine.  Pretty happy; no flaws.

I also show the number of lines now on the screen (this text library is pretty handy now, I have
my own form of Print)

Now I am going to integrate a Pause function.
Today I will:
* Pause function
- Display all statistic information
- Random number fix
* Speed correction

And if I get the time:
- .ini file library (for settings)
- Settings screen
- Levels

Approximately 4 hours of programming yet today and I'm going to start working on some graphics.
(28:30)

1-25-99
7.5 hours on graphics...
(35:00)
Another 2 hours on graphics - who would have known that the graphics would take this long?
While testing (I like playing this game...it's fun), I have found that there are some errors
in the gravity block...well, I never said it was perfect...also, I am going to change some of
the effects because they are incredibly hard to predict.

Today I spent another 2 hours or so on programming - I can't figure out why in the world it
does this - when I use DrawNumber() to display a number that's a multiple of 4 digits long,
the game gets screwy.  It's in the pfPlayField.Frames, too, I think.  I narrowed the problem
down to the windows string functions like lstrlen, lstrcpy, and atoi.

(37:00)
So now do more with scoring, incorporate lines above 4.

1-27-99 4:45 I took yesterday off, I guess, and today I will work on it a little more.  The
game is now complete, I guess...oh and I fixed that problem I had before; it was because I
declared a string as char far* str; instead of char Str[100];  that didn't make sense; but
oh well...

I will now finish the statistics par tof the code in the playfield file...
(38:00)

1-29-99 9:32 am I am here once again; I took off yesterday just because of things are busy.
I am ready to start work on a high scores section; I'll put this in the settings screen, so
I'll be making a new .cpp/.h for file access and another one for the settings screen and
probably another one for high score display and certainly another one for changing the
controls.  I'll after that support joysticks.  Then sound.  Then....then....the.....th.....t..

I'm also going to have to create a few custom controls for my settings screen like a
command button, a slider (for volume, etc...).  Well for now I'll just do the command button.

I have one particular problem with the command buttons - that is that I need a way to tell
the class COMMANDBUTTON that when it's clicked, it should go to a particular function. I
just did some research on the internet and found how to do pointers to functions.
(39:00)

1-31-99 10:49 am
Well as it is very evident I haven't been spending much time on this lately; I have been doing
some other projects that I am in the midst of doing, such as recording songs multitrack for
fun and stuff.

But I got up today and still had the programming bug biting me, so I completed the implement-
ation of the command-buttons.  Now they work great; I just have to make some graphics for
that screen (and while I'm at it I should make the pause screen) (and while I'm at it I should
make the title screen) (and while I'm at it I should make the 10-minute full-screen with sound
cinematic at the beginning with full orchestrated score}.

2-2-99 12:32am
I haven't been documenting on it as detailed as before simply because it's hard to.  I would
estimate that I have spent 3 hours today and yesterday on it.  I have the high scores saving
routines done; I am trying to work out bugs in the exiting of the program (like hitting Alt-f4;
the program doesn't respond well to it; the process is still active).  But my next session
(note I do'nt specifically say tomorrow) I will try to create a place where you type in your
name for the high score.  Also a place where it displays it.  My goal is to display all the
information about the high scores (all 31 different fields of information) and because it will
be impossible to display it all on the screen at once, be able to scroll through it.

I realized that I am pretty far in this game right now.

Well let's see...
1) Termination thingy
2) Command Button for Exit on pause screen
3) Another control for a text box for user entry.

1) DONE
2) DONE
3)

2-4-99 12:08am
I just bought Sim City 300 and Odd World Abe's Oddysee, so I haven't done anything recently.
My next task is to create another control for the keyboard entry box.

I want to document something:  I understand that it would be much nicer to have creted an
entire OS for my game.  To have an environment act like windows and have my game use it to
make it look all nice and everything, but for the fact that I am not a personal game develop-
ing company, I have no need to write an entire "OS" for a Tetris game.  Maybe someday,
because it would be good practice, though.

2-5-99
I'll say 43:00 for now; that's a bare estimate, but the highest one becuse I don't want to
flatter myself and say I did this in under two hours or something.

I've started the textentrybox control,but I'm realizing that I did it stupidly; I should have
made the class for it to hold information; that will be easy; it will still be called in
the same fashion.

2-6-99 1:14pm
I've decided to finally sit and finish this text edit thingy at whatever the cost.  I am going
to have to turn it into a class, but I still want to make input a simple function call.  So
there is a function GetInput() I guess and a class TEXTEDITBOX.  GetInput will setup a
TEXTEDITBOX object and get the input from it and return the string.

So for what will the TEXTEDITBOX class be good?  It will hold all information about the "window"
I create with TEXTEDITBOX::GetInput and everything....So ::GetInput() will be called and it will
create a TEXTEDITBOX, adn then just simply call TEXTEDITBOX::GetInput(); and return the return
value.

It gets kinda confusing because I have to put a messageloop in there somewhere and the main
proc function will have to call another proc function; hmmm.....well I think I can figure it out
even though it might get a little spicy.

2-8-99 4:44pm
(45:00)
Well I believe the high scores thing is all done; some touching up on teh text entry thing will
be nice, but over all it's time to move on.

I want to take a paragraph to explain something I could have improved to start.  I have been
giving a lot of thought lately about the OS of my game.  Very small, indeed.  I have just
some routines that do stuff and that stuff is only very specifically game-related.  I don't have
modal window classes and control classes really; well I guess I kind of do, but they are very
specific and not broadly-based like I would like, but I believe that for the nature of this game
and the fact that it won't be used again, it's necessary.  So although I might not have changed
it if I had gone back and had to start over, it isn't the most efficient way of doing thigns.

There is one particular thing that I DO wished I had done differently and that is the pause
screen.  Every screen consists of it's own PF_ declaration for Pass.ProgramFlow, and it has
it's own .cpp and .h file, adn includes it's very own Proc() and message loop, making it very
organized and easy to follow.  Pause isn't handled that way; I'm not the biggest fan of that,
but it works just fine.

This problem came to my thinking because my next project is to create the screen for displaying
high scores.  This shouldn't be that big of a deal; the only thing that might be tough is the
display and the implimentation of it.  Beings tha there are like 100 high scores and 30-some
different stats for each, it would be impossible to display them all.  So the user will have
to hit arrow keys to scroll (either side-to-side or up/down; who knows?) or I could have it
be shown in screens.  Possibly I could have it be a time-thing, where the user just waits for
it to scroll down.

I think I will just go with the scrolling idea; but not like the credits of a movie; it will be
controlled by the user.  THey can PageUp/Down, and arrow keys.

2-12-99 4:49 pm
(48:00)
I have finished all the high score stuff (with tweaking to be done when I'm cleaning up my
program.

I'm now going to delve into something into which I've never delved before:  DirectSound....
I'm reading right now; don't expect me back until I've gotten a sound to play.

*Joystick input
*Sound
*Some more graphics

2-13-99 11:07 pm
Well sound looks incredibly complicated, so it will take some concentration and studying
before I can delve into it.  I've read some today and I've learned a lot..  I'll try to
just jot down some stuff I've learned (it will help me sort it out)

DirectSound is going to be pretty simple, although streaming the background score may not
be.  Playing a WAV file is my task as of now, using DirectSound.  API would simply mean
sndPlaySound, but unfortunately the standard Win32 API doesn't include a mixer, and thusly
I can not play more than one sound at a time, and that would make for a boring game.  My
solution:  DirectSound.  It will mix it, and also introduce me to another aspect of DirectX.
It's relatively simple, but it brings up another problem:  WAV format.  Sure, it's easy to
set up a secondary buffer and play it in DirectSound, but how about getting a WAV file into
a buffer?  I'll need to (and have already started to) write a library for WAV file routines
that will load them into buffers.  This involves using mmsystem.h or whatever it's called
(just learned about it today).  mmciOpen(); and stuff like that.

Now this process should be simple, but because of the way everything under standard Windows
works, it's not.  Windows WAV files are RIFF files.  Under the RIFF chunk, we need to descend
to the WAVE chunk, then get data from descending yet again to the "fmt " chunk, and finally
get to teh wave data after descending to the "data" chunk.

I have researched how to do all this and because it's so fresh in my memory, it's very loose
and I will forget it.  It's another one of those times when I'll have to stare at the help
file a few times to be able to be comfortable with calling those API functions.

I will need to create a function in dsWAVlib.cpp that will load a .WAV file into a buffer.
I will still have to research streaming buffers, but for now that should do.

My next task I guess is to figure out how to stick the data into the buffer; I can get to
the part of the WAV file for the data, but I don't know how to put it in the buffer; I think
you just copy it like the file on disk (that would make perfect sense; there's no reason
that the disk copy of a WAV file should be different than the memory resident version).

2-17-99 6:22 pm
I have sound.  I know, it's a bit late in coming, but hey, I got it.  I got WAV files to play
in DirectX.  Woo hoo!  Turns out I have been doing eveything just fine, I just need to stick
with it and keep going.

So I can easily now play sound files; I'm prtty happy with how that goes.  Now I need to create
a lib for Streaming, though.  I will create a class for this; I kinda planned this out in my
mind.  I will just create an audiostream class, and when it's initialized, it will create a
sound buffer for it, and open the wave file for reading.

I have to figure out now how this is going to work.  For something like this, that's not so
much the lingo of the interface, like DirectX stuff, or windows API, but it's more like a
procedure...like a sort function...that I will have to create and I want o create it myself.

OK.  I have decided that I'll have a 1500 ms buffer for my stream.  When DSSTREAM::Play() is
called, it will start to play the looped buffer, and after 1000 ms, it will fill the first
500 ms with new sound.  From then on, every 500 ms it will keep doing that, forever, until the
music stops.  That way, the cursor will always be 1/2 second behind and in front of the play-
ing cursor.  Now the only thing that worries me is that when I lock the surface, when I get
a pointer to the place in memory, where does THAT start?

Ahh...I guess you specify the portion to lock...not the Lock method....cool...I guess my
method will work fine....

(50:00)

2-18-99
Well I have a dilemma.  I wrote the entirety of my DSSTREAM class and haven't change any of the
code.  It's very well done, or so I beleive, but I can't test it.  timeSetEvent() seems to
lock up my computer under any circumstances, so I am forced to be very frustrated.  I had a
creative time passing values and functions and things, but nothing works.  All day I have tried
this, and I'm going to be forced to stick valuable hours on my clock.  I don't know what to do;
there are few situations in which I am completely stuck, but there seems to me that there is
nothing wrong with this.

The second plan is to set up a notify event.  That might even be more accurate, too.
(59:00)

2-19-99 12:22pm
I hate things like this.  Borland should now die, I've concluded.  The reason timeSetEvent
wasn't wroking is because the debugger can't handle it.  So if I run it without the debugger,
it works fine.  Now I have to write the rest of the streaming stuff.

2-21-99 3:15 pm
I wrote the .ini file library today...it's pretty nice.  I love simple stuff.  I mean, you just
create a INIFILE object, and then ::Open(filename) it, and then you can either READVALUE or
WRITEVALUE.  They can be of any type and any length.
(62:00)

I've decided to go back and make this game more interesting.  It seems like normal tetris;
you don't have to worry about anything.  The only pieces that are truly differnt are
gravity adn  destruct; the others are just static blokcs.

So my idea is to make FIRE a "bad" block.  When you get one, it's not good, and you lose
points for continuing to get them.  Putting out fire blocks is the best thing to do; they
will disappear maybe.  I have to think about this; I need to make is to that each kind of
block is good for things and some bad.  Otherwise, it's just plain old tetris

And thusly I have thought about changing the graphics, too.  I got a second opinion on the
block graphics, and they said that was the weakest part of the graphics.  So I will have to
change all the graphics, including EVERYTHING.  And I wnt to make an opening cinematic
because that way the bulk of this thing won't just be music; it will be graphics, too.

3-7-99 2:23pm
I haven't written in here for a while because I am not using MSVC and I don't include this
.txt file in the project.  I got done with the .ini file library, changed the effects so that
water disappears sometimes, redid some graphics, and then I changed everything to 16-bit.

I had written fadein/out routines, but they were for 256 color, so I changed that to 16-bit.
Not easy.  That code went from 10 lines to 100.  I finally got it done though, and I am
ever learning DirectDraw.

My next project is to go back and clean up everything.

3-8-99 10:48 pm
Well, I'm polishing stuff up.  Here's a list of things I need to be able to do:

* Finish the font .bmps (990309)
	* Fix font code to handle fonts better. (990309)
* Finish On/Off Control. (990310)
* Fix controls to act the same (990312)
* Add controls to the options screen and finish that (990312)
* Add code for the .ini file again(990312)
* Add controls in Highscores screen(990314)
* add sorting in high scores screen(990314)
* Destroy all DirectX objects completely upon exiting
* Program the sounds effects into the game (990315)
* Add credits screen (990315)
* Cheat codes (create a buffer for all keyboard input and every keypress, check the buffer (990315)

* Fade problem(990316)
* Fix DSSTREAM so that it checks to make sure it's OK (990316)
* Restore screen when switched(990316)
* Clean up text edit box(990316)
* Fix the volume problem. (990316)
* Fix Slider values problem.(990316)
* Fix high scores sort by date(990316)
^ Rest of cheats
/////////////////////////////////////////////////////////
MEDIA:
- Create the sound effects
- BMPs
/////////////////////////////////////////////////////////
And I'm sure I'll come up with some more

- Check for compatibility
- Installation program (ugh)
- Help files
- Packaging to a CD

I have set a deadline for March 22nd, when my girlfriend comes up and I won't have time to do 
anything on this.

3-12-99 4:04pm
I've done a bit on this today; as you can also see I gave up on teh hours thing...I lost count
and things got too complicated when I switched from Borland to MSVC.  I'm currently writing the
code for where you choose the controls for the game in the options screen.

To do this, I am going to clean up a lot of my code.  I want to change all my controls (onoff/
slider, commandbutton) to all work the same as far as drawing to the screen goes.

3-14-99 2:57 pm
It's spring break and I'm flying on this.  I should get people testing it pretty soon; my biggest
leap now will be the media.  Then to make an install program and I'm all good.  I'm happy that I
have decided to stay out of the system registry with this.

7:43 pm
Well I want to start figuring out how I'm going to program the sound effects; but I'll wait till
tomorrow; I'm beat.

3-15-99 1:19 pm
OK.  Sounds.  I can't load all the sounds into memory; that's simply inefficient.  I will have to
have an array of soundbuffers and a function to play a sound file.  The buffer stuff will all be
handled by DSOUNDLIB.cpp.  Say there are 3 buffers (LPDIRECTSOUNDBUFFER DSBuffer[3]).  When the
program starts, there will be a "pointer" (not a *, but a general pointer, from 0-2) that will
be 0.  When we play a wave file, it will be played in DSBuffer[0] and the pointer will then be
advanced to 1.  See how this works?  That way we will be able to have 3 sounds playing at the same
time, and memory managed more efficiently.

I've been doing a lot of interesting cleaning up today - I am improving how the program exits.
for a while I couldn't get a lot of stuff to work, but not I'm safe again phew....
