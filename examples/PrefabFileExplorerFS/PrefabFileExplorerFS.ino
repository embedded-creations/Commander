/* Prefab command example
 *  Prefabs are command sets and command handlers that have been pre defined for specific scenarios
 *  This demonstrates the FileNavigator prefab that impliments an SDFat file navigation system.
 *  The prefab is implimented as a sub command and can be invoked with the SD command.
 *  
 *  IMPORTANT:
 *  The prefab includes a file write command that lets you stream text files to a file on the SD card or open a file and sent typed text to the file.
 *  When this command is activated the command processor will stop working until the ASCII value 4 is received.
 *  This can be sent from a terminal application such as coolTerm by pressing CONTROL+D
 *  The Arduino Serial terminal does NOT allow you to send this character and so you cannot terminate the file download when using the Arduino Serial terminal.
 */

#define USE_SD 1
#define USE_LITTLEFS 0
#include <Commander.h>
#include <prefabs/FileSystem/PrefabFileNavigatorFS.h>

Commander cmd;
FileNavigatorMainMenu myMainMenu("CMD");


#if (USE_SD == 1)
  #include <SD.h>
  FileNavigator myNavigator(SD, "SD:");
  //FileNavigator myNavigator(SD, "SD:", Serial2); // optionally include Stream in third argument to enable debug output
  const int cardSelect = 4;
#endif

#if (USE_LITTLEFS == 1)
  #include <LITTLEFS.h>
  FileNavigator myNavigatorLF(LITTLEFS, "LF:");
  //FileNavigator myNavigatorLF(LITTLEFS, "LF:", Serial2); // optionally include Stream in third argument to enable debug output
#endif

void setup() {
  Serial.begin(115200);
  while(!Serial){yield();} //Wait for the serial port to open before running the example
  Serial.println("Commander prefab File navigator Example");


#if (USE_SD == 1)

  //See if an SD card is inserted
  if(!SD.begin(cardSelect)) {
    Serial.println("SDCard Init Error");
  }else{
    myNavigator.setFilesystemOk(true);
    Serial.println("SDCard Started");
  }
  //tell the filenavigator prefabs the top layer command collection for when "exit" is used
  myNavigator.setTopLayer(myMainMenu);
  myMainMenu.addNavigator(myNavigator, "SD");
#endif

#if (USE_LITTLEFS == 1)
  if(!LITTLEFS.begin()) {
    Serial.println("LittleFS Init Error");
  }else{
    myNavigatorLF.setFilesystemOk(true);
    Serial.println("LittleFS Started");
  }

  //tell the filenavigator prefabs the top layer command collection for when "exit" is used
  myNavigatorLF.setTopLayer(myMainMenu);
  myMainMenu.addNavigator(myNavigatorLF, "LF");
#endif

  Serial.println("Starting Commander ... type help to see a command list");

  cmd.endOfLineChar('\r');
  cmd.delimiters("= :,\t\\|"); // use defaults except for '/' which interferes with pathnames including root directory
  cmd.stripCR(OFF);
  cmd.setStreamingMode(1);
  cmd.setStreamingMethod(1);
  cmd.echo(true);
  cmd.begin(&Serial, myMainMenu);
  cmd.commandPrompt(ON);
  cmd.commanderName = myMainMenu.name; // TODO: incorporate into begin?
  cmd.printCommandPrompt();
}

void loop() {
  cmd.update();
}
