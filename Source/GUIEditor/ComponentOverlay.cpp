/*
  ==============================================================================

    ComponentOverlay.cpp
    Created: 18 Nov 2016 1:41:26pm
    Author:  rory

  ==============================================================================
*/
#include "ComponentOverlay.h"
#include "../Audio/Plugins/CabbagePluginEditor.h"

ComponentOverlay::ComponentOverlay (Component* targetChild, ComponentLayoutEditor* owner)
    :   target (targetChild), lookAndFeel(), layoutEditor (owner)
{
    resizeContainer.reset (new ComponentBoundsConstrainer());
    resizeContainer->setMinimumSize (4, 2); //set minimum size so objects cant be resized too small
    resizer = new ResizableBorderComponent (this, resizeContainer.get());
    addAndMakeVisible (resizer);
    resizer->addMouseListener (this, false);
    constrainer.reset (new ComponentBoundsConstrainer());
    interest = "none";
    userAdjusting = false;
    updateFromTarget ();
    setLookAndFeel (&lookAndFeel);
    setRepaintsOnMouseActivity (true);
    addKeyListener (this);

}

ComponentOverlay::~ComponentOverlay ()
{
    setLookAndFeel (nullptr);
    delete resizer;
}

void ComponentOverlay::resized ()
{
    setWantsKeyboardFocus (true);

    resizer->setBounds (0, 0, getWidth(), getHeight());

    if (resizer->isMouseButtonDown ())
    {
        applyToTarget ();
    }
}

void ComponentOverlay::paint (Graphics& g)
{

    Colour c = Colours::white;

    if (interest == "selected")
    {
        Path selectedRect;
        selectedRect.addRectangle (getLocalBounds().reduced (2));
        g.setColour (c);


        const float dashLengths[] = { 10.0f, 10.0f };
        PathStrokeType stroke (1.0, PathStrokeType::mitered);
        stroke.createDashedStroke (selectedRect, selectedRect, dashLengths, 2);
        g.strokePath (selectedRect, stroke);
    }

    g.setColour (c);
    g.drawRect (0, 0, getWidth(), getHeight(), 1);
}


const Component* ComponentOverlay::getTargetChild ()
{
    return target.getComponent ();
}
//===========================================================================
void ComponentOverlay::updateFromTarget ()
{
    if (target != NULL)
        //if (!target.hasBeenDeleted ())
    {
        setBounds ( target.getComponent ()->getBounds () );
    }
}

void ComponentOverlay::applyToTarget ()
{
    if (target != NULL)
    {

        Component* c = (Component*) target.getComponent ();
        c->setBounds (getBounds ());

        if (startBounds.getTopLeft() == c->getBounds().getTopLeft())
        {
            for (int i = 0; i < c->getNumChildComponents(); i++)
            {
                Component* comp = c->getChildComponent (i);
                const float x = ((float)c->getWidth() / (float)startBounds.getWidth());
                const float y = ((float)c->getHeight() / (float)startBounds.getHeight());
                const float left = childBounds[i].getX() * x;
                const float top = childBounds[i].getY() * y;
                const float width = childBounds[i].getWidth() * x;
                const float height = childBounds[i].getHeight() * y;
                comp->setBounds (left, top, width, height);

            }
        }

        userChangedBounds ();
    }
}
//===========================================================================
void ComponentOverlay::userChangedBounds ()
{
    //update minimum onscreen amounts so that object can't be resized past the screen area
    resizeContainer->setMinimumOnscreenAmounts (getHeight() + target.getComponent()->getHeight(), getWidth() + target.getComponent()->getWidth(),
                                                getHeight() + target.getComponent()->getHeight(), getWidth() + target.getComponent()->getWidth());
}

bool ComponentOverlay::boundsChangedSinceStart ()
{
    return startBounds != getBounds ();
}
//===========================================================================
void ComponentOverlay::mouseDown (const MouseEvent& e)
{
    layoutEditor->updateSelectedComponentBounds();

    mouseDownSelectStatus = layoutEditor->getLassoSelection().addToSelectionOnMouseDown (this, e.mods);

    if (e.eventComponent != resizer)
    {
        //added a constrainer so that components can't be dragged off-screen
        constrainer->setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
        dragger.startDraggingComponent (this, e);
    }


    setBoundsForChildren();

    userAdjusting = true;
    startBounds = getBounds ();
    userStartedChangingBounds ();
    layoutEditor->updateCodeEditor();

    if (layoutEditor->getLassoSelection().getNumSelected() == 1)
        layoutEditor->resetAllInterest();

    interest = "selected";
    repaint();

    if (e.mods.isPopupMenu())
    {
        PopupMenu menu;
        menu.setLookAndFeel (&this->getLookAndFeel());
        menu.addItem (100, "Delete");

        menu.showMenuAsync(juce::PopupMenu::Options(), [this](int r) {

        if (r == 100)
        {
            if (this->layoutEditor->getLassoSelection().getNumSelected() > 1)
            {
                CabbageUtilities::showMessage ("Multiple widgets cannot be deleted. Either deselect and delete one by one, or delete the widgets from the Cabbage code section of your .csd file", &this->getPluginEditor()->getLookAndFeel());;
            }
            else
                this->layoutEditor->getPluginEditor()->sendActionMessage ("delete:" + target->getProperties().getWithDefault ("linenumber", -1).toString());
        }
        });
    }

}

void ComponentOverlay::mouseUp (const MouseEvent& e)
{

    if (e.eventComponent != resizer)
    {
        //add this to reset MainComponent to have keyboard focus so that keyboard shortcuts (eg. lock/unlock) still work / intercept the messages
        getTopLevelComponent()->getChildComponent (0)->grabKeyboardFocus();
    }

    if (userAdjusting) userStoppedChangingBounds ();

    userAdjusting = false;

    layoutEditor->getLassoSelection().addToSelectionOnMouseUp (this, e.mods, true, mouseDownSelectStatus);
    layoutEditor->updateSelectedComponentBounds();
    updateBoundsDataForTarget();
}

void ComponentOverlay::mouseDrag (const MouseEvent& e)
{
    if (e.eventComponent != resizer)
    {
        bool multipleSelection = false;

        if (!e.mods.isPopupMenu())
        {
            for ( ComponentOverlay* child : layoutEditor->getLassoSelection() )
            {
                const int gridSize = 2;
                const int selectedCompsPosX = ((int (child->getProperties().getWithDefault ("originalX", 1)) + e.getDistanceFromDragStartX() ) / gridSize) * gridSize;
                const int selectedCompsPosY = ((int (child->getProperties().getWithDefault ("originalY", 1)) + e.getDistanceFromDragStartY() ) / gridSize) * gridSize;
                child->setTopLeftPosition (selectedCompsPosX, selectedCompsPosY);
                child->applyToTarget();
                multipleSelection = true;
            }

            if (multipleSelection == false)
            {
                dragger.dragComponent (this, e, constrainer.get());
                applyToTarget ();
            }

        }

    }
}

void ComponentOverlay::mouseEnter (const MouseEvent& e)
{
    if (layoutEditor->getLassoSelection().getNumSelected() > 1)
        resizer->setVisible (false);
    else
        resizer->setVisible (true);

    repaint ();
}

void ComponentOverlay::mouseExit (const MouseEvent& e)
{
    repaint ();
}

//==================================================================================================================
bool ComponentOverlay::keyPressed (const KeyPress& key, Component* originatingComponent)
{
    bool multipleSelection = false;
    const int gridSize =  2;
    

    for (ComponentOverlay* child : layoutEditor->getLassoSelection())
    {
        if (key == KeyPress::leftKey || key == KeyPress::rightKey)
            child->setTopLeftPosition (child->getPosition().getX() + (key == KeyPress::leftKey ? -gridSize : gridSize), child->getPosition().getY());

        else if (key == KeyPress::upKey || key == KeyPress::downKey)
            child->setTopLeftPosition (child->getPosition().getX(), child->getPosition().getY() + (key == KeyPress::upKey ? -gridSize : gridSize));

        else if (key == KeyPress (KeyPress::leftKey, ModifierKeys::shiftModifier, 0))
            child->setTopLeftPosition (child->getPosition().getX() - gridSize * 4, child->getPosition().getY());

        else if (key == KeyPress (KeyPress::rightKey, ModifierKeys::shiftModifier, 0))
            child->setTopLeftPosition (child->getPosition().getX() + gridSize * 4, child->getPosition().getY());

        else if (key == KeyPress (KeyPress::upKey, ModifierKeys::shiftModifier, 0))
            child->setTopLeftPosition (child->getPosition().getX(), child->getPosition().getY() - gridSize * 4);

        else if (key == KeyPress (KeyPress::downKey, ModifierKeys::shiftModifier, 0))
            child->setTopLeftPosition (child->getPosition().getX(), child->getPosition().getY() + gridSize * 4);


        child->applyToTarget();
        multipleSelection = true;
        layoutEditor->updateSelectedComponentBounds();
    }

    if (multipleSelection == false)
    {
        setTopLeftPosition (getPosition().getX() - gridSize, getPosition().getY());
        applyToTarget();
    }

    updateBoundsDataForTarget();
    layoutEditor->updateCodeEditor();

    return true;
}
//===========================================================================
CabbagePluginEditor* ComponentOverlay::getPluginEditor()
{
    return layoutEditor->getPluginEditor();
}

void ComponentOverlay::setBoundsForChildren()
{
    Component* c = (Component*) target.getComponent ();
    childBounds.clear();

    for (int i = 0; i < c->getNumChildComponents(); i++)
    {
        childBounds.add (c->getChildComponent (i)->getBounds());
    }

}
//===========================================================================

void ComponentOverlay::updateBoundsDataForTarget()
{
    for ( ComponentOverlay* child : layoutEditor->getLassoSelection() )
    {
        ValueTree valueTree = CabbageWidgetData::getValueTreeForComponent (layoutEditor->widgetData, child->target.getComponent()->getName());
        CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::left, child->target.getComponent()->getX());
        CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::top, child->target.getComponent()->getY());
        CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::width, child->target.getComponent()->getWidth());
        CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::height, child->target.getComponent()->getHeight());
    }


    Component* c = (Component*) target.getComponent ();

    for (int i = 0; i < c->getNumChildComponents(); i++)
    {
        const Component* child = target.getComponent()->getChildComponent (i);
        ValueTree valueTree = CabbageWidgetData::getValueTreeForComponent (layoutEditor->widgetData, child->getName());

        if (CabbageWidgetData::getStringProp (valueTree, CabbageIdentifierIds::parentcomponent).isNotEmpty()) //now deal with plants, all child widgets must have theirs bounds updated..
        {
            CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::left, child->getX());
            CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::top, child->getY());
            CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::width, child->getWidth());
            CabbageWidgetData::setNumProp (valueTree, CabbageIdentifierIds::height, child->getHeight());
        }
    }

    getPluginEditor()->sendChangeMessage();
}
