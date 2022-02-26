/*
 Copyright (C) 2016 Rory Walsh
 
 Cabbage is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 Cabbage is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU General Public
 License along with Csound; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 02111-1307 USA
 */

#include "CabbageLicenseDialogue.h"
#include "../Audio/Plugins/CabbagePluginEditor.h"


CabbageLicenseDialogue::CabbageLicenseDialogue (CabbagePluginEditor* _owner)
: owner(_owner),
CabbageWidgetBase(_owner)
{   
    setOpaque (false);
    widgetData.addListener (this);              //add listener to valueTree so it gets notified when a widget's property changes
}

void CabbageLicenseDialogue::valueTreePropertyChanged (ValueTree& valueTree, const Identifier& prop)
{

}
