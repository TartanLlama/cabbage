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

#ifndef CabbageCustomProcessor_H_INCLUDED
#define CabbageCustomProcessor_H_INCLUDED

#include "JuceHeader.h"
#include "../../Widgets/CabbageWidgetData.h"
#include "../../CabbageIds.h"
#include "../../Widgets/CabbageXYPad.h"

class CabbagePluginParameter;

class CabbageCustomProcessor : public Timer
{
public:

    class CabbageJavaClass  : public DynamicObject
    {

        CabbageCustomProcessor* owner;
    public:

        explicit CabbageJavaClass (CabbageCustomProcessor* thisOwner): owner (thisOwner)
        {
            setMethod ("print", print);
        }

        static Identifier getClassName()   { return "Cabbage"; }

        static var print (const var::NativeFunctionArgs& args)
        {
            if (args.numArguments > 0)
                if (CabbageJavaClass* thisObject = dynamic_cast<CabbageJavaClass*> (args.thisObject.getObject()))
                    thisObject->owner->cabbageScriptGeneratedCode.add (args.arguments[0].toString());

            return var::undefined();
        }



        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageJavaClass)
    };

    struct PlantImportStruct
    {
        String nsp, name, csoundCode;
        StringArray cabbageCode;
    };

	CabbageCustomProcessor (File inputFile, BusesProperties IOBuses);
	void createCsound(File inputFile, bool shouldCreateParameters = true);
    ~CabbageCustomProcessor() override;

    ValueTree cabbageWidgets;
    CachedValue<var> cachedValue;
    void getChannelDataFromCsound() override;
    void getIdentifierDataFromCsound() override;
    void triggerCsoundEvents() override;
    void setWidthHeight();
    int chnsetGestureMode = 1;
    CabbageWidgetIdentifiers** pd;
    CabbageWidgetIdentifiers* identData;
    int autoUpdateCount = 0;
    bool autoUpdateIsOn = false;

    
    //save and restore user plugin presets
    void addPluginPreset(String presetName, String fileName, bool remove);
    void restorePluginPreset(String presetName, String filename);
    
    bool addImportFiles (StringArray& lineFromCsd);
    void parseCsdFile (StringArray& linesFromCsd);
    // use this instead of AudioProcessor::addParameter
    void addCabbageParameter(std::unique_ptr<CabbagePluginParameter> parameter);
    void createCabbageParameters();
    void updateWidgets (String csdText);
    void handleXmlImport (XmlElement* xml, StringArray& linesFromCsd);
    void getMacros (const StringArray& csdText);
    void generateCabbageCodeFromJS (PlantImportStruct& importData, String text);
    void insertUDOCode (PlantImportStruct importData, StringArray& linesFromCsd);
    void insertPlantCode (StringArray& linesFromCsd);
    bool isWidgetPlantParent (StringArray& linesFromCsd, int lineNumber);
    bool shouldClosePlant (StringArray& linesFromCsd, int lineNumber);
    void setPluginName (String name) {    pluginName = name;  }
    String getPluginName() { return pluginName;  }
    void expandMacroText (String &line, ValueTree wData);
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void setCabbageParameter(String& channel, float value, ValueTree& wData);
    CabbagePluginParameter* getParameterForXYPad (StringRef name);
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool editorIsOpen = false;
    void releaseResources() override;
    bool hasEditor() const override;
    //===== XYPad methods =========
    void addXYAutomator (CabbageXYPad* xyPad, ValueTree wData);
    void enableXYAutomator (String name, bool enable, Line<float> dragLine);
    void disableXYAutomators();
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void setParametersFromXml (XmlElement* e);
    XmlElement savePluginState (String tag);
    void restorePluginState (XmlElement* xmlElement);
    //==============================================================================
    StringArray cabbageScriptGeneratedCode;
    Array<PlantImportStruct> plantStructs;

    int64 csdLastModifiedAt;
    void timerCallback() override;
	//uid needed for Cabbage host
	AudioProcessorGraph::NodeID nodeId;

	File getCsdFile()
	{
		return csdFile;
	}

    static BusesProperties readBusesPropertiesFromXml(File csdFile)
    {
        BusesProperties buses;

        String csdString = csdFile.loadFileAsString();

        const int numOutChannels = CabbageUtilities::getHeaderInfo(csdString, "nchnls");
        int numInChannels = numOutChannels;
        if (CabbageUtilities::getHeaderInfo(csdString, "nchnls_i") != -1 && CabbageUtilities::getHeaderInfo(csdString, "nchnls_i") != 0)
            numInChannels = CabbageUtilities::getHeaderInfo(csdString, "nchnls_i") ;

        // repeat this for every bus in the xml file
        for (int i = 0, cnt = 1; i < numOutChannels; i+=2, cnt++)
            buses.addBus(false, "Output #" + String(cnt), AudioChannelSet::stereo());

        for (int i = 0, cnt = 1; i < numInChannels; i+=2, cnt++)
            buses.addBus(true, "n'Input #" + String(cnt), AudioChannelSet::stereo());

        return buses;
    }

	StringArray getCurrentCsdFileAsStringArray()
	{
		StringArray csdArray;
		csdArray.addLines(csdFile.loadFileAsString());
		return csdArray;
	}
    
    Font getCustomFont(){        return customFont;    }
    File getCustomFontFile(){        return customFontFile;    }
    
    // use this instead of AudioProcessor::getParameters
    const OwnedArray<CabbagePluginParameter>& getCabbageParameters() const { return parameters; }
    int currentPluginScale = -1;
    String currentPresetName = "";
    

private:
#if !Cabbage_IDE_Build
    PluginHostType pluginType;
#endif
    controlChannelInfo_s* csoundChanList;
    int numberOfLinesInPlantCode = 0;
    String pluginName;
    File csdFile;
    int linesToSkip = 0;
    NamedValueSet macroText;
    var macroNames;
    var macroStrings;
    bool xyAutosCreated = false;
    OwnedArray<XYPadAutomator> xyAutomators;
	int samplingRate = 44100;
    int samplesInBlock = 64;
	int screenWidth, screenHeight;
	bool isUnityPlugin = false;
    int automationMode = 0;
    OwnedArray<CabbagePluginParameter> parameters;
    Font customFont;
    File customFontFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabbageCustomProcessor)

};


#endif CabbageCustomProcessor_H_INCLUDED
