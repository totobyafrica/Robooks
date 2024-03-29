# Robooks
Robooks .ino soure

<b>Color Coding</b><br>
Due to the model being based on LED illuminated color input, we will only be considering the upper HSV [V] values (Volume is at least 70%)
<i>Special states</i> such as <b>BLACK</b> and <b>WHITE</b> are managed as their own exclusive colors corresponding to the "reset" and "run memory" states respectively.<br>
Currently the states are as follows:<br>
<b>RED</b> - move forward (save to memory)<br>
<b>GREEN</b> - move left (save to memory)<br>
<b>BLUE</b> - move right (save to memory)<br>
<b>YELLOW</b> - move back (save to memory)<br>
<b>WHITE</b> - Run memory / sequence array<br>
<b>BLACK</b> - Reset memory --> empty array<br>
<p align="center">
  <img src="https://www.mediafire.com/convkey/1457/jjxbcmp9b6k35uwzg.jpg?size_id=5" width="525" alt="accessibility text">
</p>
<br>

<b>ISL29125 Calibration</b><br>
We are working with the following values<br>
LOWS:<br>
Red: 675<br>
Green: 863<br>
Blue: 628<br>

HIGHS:<br>
Red: 4230<br>
Green: 5123<br>
Blue: 3389<br>

<b>Command Voice IDs</b><br>
Each voice / sound file located in the SD card is ordered and stored in an encoded file in the AD4 format. These files are numbered and ordered from 0, with there being 11 files for each language (the currently supported languages are: Hungarian, Romanian, Slovak, English) with an average size of around 15kb.
The voicelines are generated using standard TTS software.
Below is a graphical representation of the system of storing the voicelines on the SD cards.
<p align="center">
  <img src="https://www.mediafire.com/convkey/8b11/fu8jbq6ju75ju239g.jpg" width="525" alt="accessibility text">
</p>
