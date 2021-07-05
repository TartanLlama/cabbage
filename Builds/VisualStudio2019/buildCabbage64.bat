
REM "Build Script for Cabbage on Windows. 
REM "Please edit this file to point to the location of your Projucer application"
REM "And change the variable MYPATH to the directory of projucer.exe"
REM "Asio driver and VST SDK should be placed under : C:\SDKs\ASIOSDK2.3\   and C:\SDKs\VST_SDK"
REM "If not, you have to modify the jucer files in the Cabbage root directory to ensure all paths are correct."
REM "Then run this script"
REM "exe and dll will be stored under .\x64"

PAUSE
 
::C:\\Users\\rory\\Documents\\sourcecode\\JUCE\\extras\\Projucer\\Builds\\VisualStudio2017\\x64\\Debug\\Projucer.exe
SET PROJUCER="C:\\Users\\rory\\sourcecode\\JUCE\\extras\\Projucer\\Builds\\VisualStudio2017\\x64\\Debug\\App\\Projucer.exe"


REM "Build Script for Cabbage on Windows. 
REM "Please edit this file to point to the location of your Projucer application"
REM "And change the variable MYPATH to the directory of projucer.exe"
REM "Asio driver and VST SDK should be placed under : C:\SDKs\ASIOSDK2.3\   and C:\SDKs\VST_SDK"
REM "If not, you have to modify the jucer files in the Cabbage root directory to ensure all paths are correct."
REM "Then run this script"
REM "exe and dll will be stored under .\x64"

PAUSE
 
::C:\\Users\\rory\\Documents\\sourcecode\\JUCE\\extras\\Projucer\\Builds\\VisualStudio2017\\x64\\Debug\\Projucer.exe
SET PROJUCER="C:\\Users\\rory\\sourcecode\\JUCE\\extras\\Projucer\\Builds\\VisualStudio2017\\x64\\Debug\\App\\Projucer.exe"

REM "BUILDING THE COMMAND LI FOR CSOUND"
 %PROJUCER% --resave ../../CabbageCsoundCLI.jucer
  msbuild CabbageCsoundCLI.sln /p:Configuration=Release /property:Platform=x64 /m
 copy ".\x64\Release\ConsoleApp\CabbageCsoundCLI.exe" ".\x64\CabbageCsoundCLI.exe"

REM "BUILDING THE MAIN APP"
 %PROJUCER% --resave ../../CabbageIDE.jucer
  msbuild Cabbage.sln /p:Configuration=Release /property:Platform=x64 /m
 copy ".\x64\Release\App\Cabbage.exe" ".\x64\Cabbage.exe"

 
REM "BUILDING EFFECT PLUGIN"
 %PROJUCER% --resave ../../CabbagePlugin.jucer
 msbuild CabbagePlugin.sln /p:Configuration=Release /property:Platform=x64 /m
 copy ".\x64\Release\Standalone Plugin\CabbagePlugin.exe" ".\x64\CabbagePlugin.exe"
 copy ".\x64\Release\VST\CabbagePlugin.dll" ".\x64\CabbagePluginEffect.dll"
 copy ".\x64\Release\VST3\CabbagePlugin.vst3" ".\x64\CabbagePluginEffect.vst3"


REM "BUILDING VST PLUGIN"
 %PROJUCER% --resave ../../CabbagePluginSynth.jucer
 msbuild CabbagePlugin.sln /p:Configuration=Release /property:Platform=x64 /m

 copy ".\x64\Release\VST\CabbagePlugin.dll" ".\x64\CabbagePluginSynth.dll"
 copy ".\x64\Release\VST3\CabbagePlugin.vst3" ".\x64\CabbagePluginSynth.vst3"

REM "BUILDING CABBAGE LITE"
 %PROJUCER% --resave ../../CabbageLite.jucer
  msbuild CabbageLite.sln /p:Configuration=Release /property:Platform=x64 /m
 copy ".\x64\Release\App\CabbageLite.exe" ".\x64\CabbageLite.exe"


for /f %%i in ('%PROJUCER% --get-version ../../CabbageIDE.jucer') do set EXECUTABLE=Cabbage-%%i.exe
ren ".\x64\Cabbage.exe" %EXECUTABLE%
