<a name="colour_1"><h3 style="padding-top: 40px; margin-top: 40px;"></h3></a>
_____________________________
**colour:1("colour")**
This sets the colour of the button when it's value is 1.  See above for details on valid colours. 

>You can create an invisible button by setting the colour to something with an alpha value of 0. Images can then be placed under the button to create an image button. 

<!--UPDATE WIDGET_IN_CSOUND
    SIdent sprintf "colour:1(%d, 0, 255) ", rnd(255)
    SIdentifier strcat SIdentifier, SIdent
-->