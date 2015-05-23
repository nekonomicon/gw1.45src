Mdlviewer v2.1.0402, a Half-Life modelviewer. Released on 2nd of April 1999
---------------------------------------------------------------------------------------------

Programming by:
	Frans 'Otis' Bouma		
	Greg 'Ascent' Dunn		
	Volker 'Dark Yoda' Schöneveld

Additional credits:
	Core modelcode by Valve Software.

Released by: 
	The HalfLife Workshop. http://www.halflife.net/workshop

---------------------------------------------------------------------------------------------

CHANGES IN THIS VERSION.
========================

- Fixed numerous bugs and lame things
- Added texture importing and exporting functionality
- Added model save functionality
- Added bone rendering
- Added chrome effect toggle
- Added skinfamily selection
- Added new loadercode for a solid, failproof loading of files
- Added more errorrecovery
- Added new rendermode, ADDITIVE, so you can see semitransparent textures AND bones
- Added better wireframe rendercode
- Added numerious nice things in the gui so the user can work with the program easier without
        trapping a bug or closing the application instantaniously.
- Added new splashscreen
---------------------------------------------------------------------------------------------

This modelviewer uses the MFC4.2 library, which is a dll that is used in many
applications. If you don't have it, you can download it from Microsoft's ftp site.

This modelviewer can't read straight from a pakfile. You need a pakexplorer to extract
the models. 

This modelviewer uses OpenGL as render API. Therefore, if you don't have an OpenGL ICD installed
on your system, you won't see a thing and the viewer will probably crash. Look below to install
a software OpenGL library if you don't have it.

This is the second 2.x release. If you find bugs, please let us know. You can email a bugreport to:

	otis@halflife.net

Please be specific what went wrong and how to reproduce the problem. Please read this document
totally, because a lot of 'so called bugs' can be avoided if you do the right things :)

This modelviewer works on Windows 95/98 and NT 4.0. It's tested on windows2000 beta3 and performed fine.
(But note, it runs much better on NT than it does on Win9x)
---------------------------------------------------------------------------------------------

INFO
====
Everything is shown in an MDI window, which looks familliar if you work with Word or similar
programs. You can open as much models at once as you like, but it will be CPU intensive.

This version does not have optimized rendercode in it yet. Therefore, we recommend not to load more
than 2 or 3 models if you are on a slow system.  (again, it works better under NT or in wireframe mode)

The main controlling center is the Model Control Center (MCC). In there you can control
the current selected model. The current selected model is the model in the view that has
the focus. People who like keyboards more than mice can select the model in it's view
and use the following keys to control it.

- Left Mouse Button for positioning of model in front of camera
- Right Mouse Button for rotating the camera around the model
- Wheel on wheelmouse for zooming IN or OUT
- z for zooming IN
- x for zooming OUT
- n for next sequence
- p for previous sequence
- b for next body
- f for toggle fog on/off
- r for toggling rendermode (wireframe/textured/lightshademap/additive textured with bones)
- '-' on numpad to decrease fog distance (if fog enabled)
- '+' on numpad to increase fog distance (if fog enabled)

The MCC has some more options you'd like to use. First there are buttons as equivalents for the keys
plus there are info boxes that tell you more about the current status of the model you are viewing.

Addionally there are a few options you can use from the MCC that don't have keyboard equivalents. These
options allow you to select textures of the model, export these to a .bmp file, import .bmp files as
the texture instead of the currently selected texture, toggle the chrome effect on or off of the currently
selected texture and browse through the skinfamilies. See 'Customizing the model' below for details.

===============
IMPORTANT NOTE:
===============
You can't view files like hgrunt02.mdl, scientist02.mdl or files like scientistt.mdl. In 
general, don't load files like name0x.mdl and namet.mdl. These files are already loaded when you 
view name.mdl. The files like scientist0x.mdl are sequencegroup files and scientistt.mdl are 
texturefiles needed to view the model. So if you try to load these into the viewer, it will 
probably crash or won't view anything at all. So to view everything about the scientist, just 
load scientist.mdl. You won't miss any info. So don't email us about this, it's NOT a bug!

One tip about how you look at the model: if you pan or rotate inside the viewer, you are NOT 
moving or rotating the model itself, but the camera that you use to look at the model. This is 
essential different, and explaines why the chrome won't behave as expected when you pan or
rotate a model. 

---------------------------------------------------------------------------------------------
HOW TO GET STARTED
==================
- You'll need OpenGL libraries to run this, which can be downloaded here, if you don't already 
  have them:

	windows95, windows98:
		ftp://ftp.microsoft.com/softlib/mslfiles/opengl95.exe
	
	NT 4:
		Install service pack 3 or 4 for the latest OpenGL libraries. NT4 has native
		OpenGL support so it should run on native NT4 systems.

- You'll also need to unpack .mdl files from the half-life\valve\pak0.pak file. These two pak 
  utilities work, as well as others:

    http://asp.planetquake.com/dl/dl.asp?qped/qped211.exe
    ftp://ftp.cdrom.com/pub/quake/planetquake/chopshop/utilities/quake/pak_utils/pakexplr.zip

- With one of the 2 tools mentioned above, go to the half-life\valve dir on your harddisk
  and open pak0.pak. Go into the models dir in the pakfile (a pakfile is just a compressed
  directory structure). Extract all the files in that dir (except the 'player' dir) into
  a directory on your harddisk, for example c:\half-life\valve\models. Now you can
  view every model.mdl file in that dir (except for the special ones model0x.mdl and
  modelt.mdl as mentioned above). 

- To make a screenshot, simply press alt + prntscrn on your keyboard and paste it into your
  favorite paintprogram to resize it.
---------------------------------------------------------------------------------------------

CUSTOMIZING THE MODEL.
======================

We're not joking if we tell you it's now possible to put your own photo on the face of your
favorite DM model. In fact, you can change any texture on any model now. After loading the
model, no texture is selected. You can see that because the word 'none' is shown after 'Current
texture selected'. If you press the '-' or '+' BUTTONS that are next it in the MCC, you will see
that you can select a texture by simply hitting the '-' or '+' buttons. A texture is selected
when it's flickering between white and the texture it has. Sometimes you'll see that you hit the '+'
or '-' button and you don't see a texture selected. It's then not visible at the moment and you can
make it visible by moving on to another BODY by pressing 'next body' or another skin family (if
available). 

Exporting Textures.
-------------------
If you've selected the texture you want to work on, you can then export this texture to a .bmp file
by clicking 'export texture'. A dialog pops up that allows you to select the filename how to save the
texture, click Ok and the texture is saved to this file. You will be warned if the file already exists.
The texture is saved as a 256 color non compressed .bmp file. You can then edit this texture
in your favorite paint program. Please consider that textures can only have 256 colors and if you want
to re-import this texture at the same spot of the model, don't change the size of the bitmap.

Importing Textures.
-------------------
If you have for example exported a hgrunt.mdl's face to a bmp file and you have added your own photo
to that bmp, you want to get it onto the model ofcourse. Load the model and select the texture you want
to replace. Then click 'import texture' and a dialog pops up that allows you to select the .bmp file that
contains the new texture. Select the file and click Ok. If the bmp file is valid it will be put on the
model immediately. Click 'none' to deselect the texture and to watch the new texture live on the model.

Toggling chrome on textures.
----------------------------
Half-life introduced a neat feature on models: environment mapping. Some parts of models looked shiny
and reflective, like shiny chrome. Well, you can apply this chrome to all textures on all models now.
Simply select the texture as described, hit the 'Texture has chrome effect' checkbox and the chrome is
switched ON if it wasn't there, and switched OFF it was. Click 'none' to deselect all textures and to
watch the effect. As a cool example, try the chromeflag on the hgrunt.mdl 's pants for nylon pants!

Saving the model.
------------------
If you like your changes, you want to save the model for usage later. Simply go to the file menu and click
'save as'. Select a filename and the modelfiles will then be saved to this new file. It's not recommended
to overwrite sourcefiles, like a just downloaded DM model. You then can't undo the changes you've made.

I still have questions.
------------------------
Of course you do. But we encourage you to experiment with the buttons. You can't hurt anything. Just
experiment!

Why Is my BMP file not valid for importing?
-------------------------------------------
This can be of several reasons:
- The bitmap is compressed
- The bitmap doesn't fit onto the polygon. Because textures are not scaled by the engine, it will look ugly
  anyway to try to import a bitmap that is larger or smaller than the original bitmap.
- The file is not a Windows bitmap but a file in another format.
- The file contains more than 256 colors. The engine will display only 256 colors anyway, so it's of no use
  to import more than 256 colors, because it can't handle that.

---------------------------------------------------------------------------------------------

DISCLAIMER AND COPYRIGHTS.
==========================
This mdlviewer is not supported by Valve. It's based on the original code from Valve Software
for their own mdlviewer. Also that mdlviewer isn't supported by Valve. This complete new tool
is programmed by the people mentioned in the 'credits' part.
Because the original sourcecode is licensed to valve and iD software no-one may make money
from this tool. It's given to the public for the purpose where it's made for: viewing 
halflife models. 

If you want to include the sourcecode of this modelviewer in your commercial program, please 
contact Valve software. If you want to include the sourcecode of this modelviewer in your NON 
commercial program, contact us and we can discuss the co-operation, on the 
condition that you won't make money from the non-commercial program after you've included the 
sourcecode. 

Use this tool as-is. There is no warranty and there will also be no warranty. It's truely tested 
on Windows 95, Windows 98 and Windows NT. As with all free software: use it at your own risk. :)

The zipfile containing the modelviewer may not be modified in a way that the modelviewer is
separated from this textfile. No-one may alter this textfile. If you want to publish 
Mdlviewer on a website or other distribution medium, you are free to do so, unless you
won't charge money for the modelviewer or the ziparchive.

If you use a model made by others and change stuff on it and release it as your own, please consider
that the model you used is made by someone who wants copyright and credits for his/hers work. Always
include a note in a textfile with the model that states who made the original. Respect work of
others!

Mdlviewer is (c)1998-1999 Ascent, Dark Yoda & Otis
---------------------------------------------------------------------------------------------

KNOWN PROBLEMS AND MALFUNTIONS.
===============================
- Sometimes a sequence is reported to have a length > 0 milliseconds but nothing happens. Some
  gibmodels have this 'problem'. The sequence length is calculated from the number of frames
  that are reported in the modelfile. It's ok to use this length as a length for the sequence
  in a multimanager. The length is only mentioned as a key to have correct values for your
  multimanager to handle more than 1 scripted sequence, without having you tweak and tune
  the map by trial and error.
- On NT with a TNT and detonator drivers and a resolution of 1600x1200 it will lockup after loading
  more than 4 models after eachother. This is a driver problem and there is no solution at the moment
  for this.
- On NT sometimes the wheelmouse of logitech (mouseman) will crash the application when using the wheel
  for zooming purposes. This is a driver problem and you should get the latest mousedrivers to fix this
  (they used ugly code :) )
---------------------------------------------------------------------------------------------

CONTACT
===============================
If you're willing to contribute info or share your bugreports with us, please email to

	otis@halflife.net

Please keep your questions related to the tool. We're willing to help everyone with certain
mapping problems, but there are other resources on the net that will help you even more,
and probably faster :)

We work on mdlviewer still and will include more and more functionality in the near future. If you think
you have a neat feature that should be included, or better, you have coded some C++ code that can do
a function and should be included into the code, drop us a line and we will contact you.

Have fun and stay tuned!

	Ascent, Dark Yoda and Otis on 2nd of April 1999
	-----------------------------------------------
	Half-life Workshop: http://www.halflife.net/workshop

