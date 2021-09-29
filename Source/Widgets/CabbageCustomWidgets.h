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

#ifndef CABBAGECUSTOMWIDGETS_H_INCLUDED
#define CABBAGECUSTOMWIDGETS_H_INCLUDED

#include "../CabbageCommonHeaders.h"
#include "CabbageWidgetBase.h"

class CabbagePluginEditor;

// Add any new custom widgets here to avoid having to edit makefiles and projects
// Each Cabbage widget should inherit from ValueTree listener, and CabbageWidgetBase
class DemoCabbageWidget : public Component, public ValueTree::Listener, public CabbageWidgetBase
{
public:

    explicit DemoCabbageWidget (ValueTree wData);
    ~DemoCabbageWidget() override {}

    //ValueTree::Listener virtual methods....
    void valueTreePropertyChanged (ValueTree& valueTree, const Identifier&) override;
    void valueTreeChildAdded (ValueTree&, ValueTree&)override {}
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override {}
    void valueTreeChildOrderChanged (ValueTree&, int, int) override {}
    void valueTreeParentChanged (ValueTree&) override {}

    ValueTree widgetData;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoCabbageWidget)
};

//add any new class declarations below this line..
class CabbageMeter : public Component, public ValueTree::Listener, public CabbageWidgetBase
{
    CabbagePluginEditor* owner;
    float level = 0;
    Array<Colour> gradientColours;
    ColourGradient colourGradient;
    Colour outlineColour;
    float outlineThickness = 1;
    int corners = 2;
    bool isVertical = true;

    class Overlay : public Component
    {
    public:
        Colour colour;
        explicit Overlay (Colour inColour): Component(), colour (inColour) {}
        void paint (Graphics& g) {    g.fillAll (colour);  }
    };

    Overlay overlayRect;
public:

    CabbageMeter (ValueTree wData, CabbagePluginEditor* _owner);
    ~CabbageMeter() override {}

    void paint (Graphics& g)  override;
    void resized()  override;

    //ValueTree::Listener virtual methods....
    void valueTreePropertyChanged (ValueTree& valueTree, const Identifier&) override;
    void valueTreeChildAdded (ValueTree&, ValueTree&)override {}
    void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override {}
    void valueTreeChildOrderChanged (ValueTree&, int, int) override {}
    void valueTreeParentChanged (ValueTree&) override {}
    void setValue (ValueTree& valueTree);
    ValueTree widgetData;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMeter)
};
#endif  // CABBAGECUSTOMWIDGETS_H_INCLUDED
