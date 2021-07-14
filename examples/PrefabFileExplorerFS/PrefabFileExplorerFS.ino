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
#include <Commander.h>

#include <SD.h>
Commander cmd;

#include "PrefabFileNavigatorFS.h"

//FileNavigator myNavigator(SD, "SD:", SerialUSB1);
FileNavigator myNavigator(SD, "SD:");

//String for the top level commander prompt
String prompt = "CMD";

#include "Commands.h"

//SPI Chip select pin
const int cardSelect = BUILTIN_SDCARD;

void setup() {
  Serial.begin(115200);
  while(!Serial){yield();} //Wait for the serial port to open before running the example
  Serial.println("Commander prefab File navigator Example");

  SerialUSB1.begin(115200);
  SerialUSB1.println("(Debug) FatFs Command Line Interpreter");

  myNavigator.setup();

  //See if an SD card is inserted
  if(!SD.begin(cardSelect)) {
    Serial.println("SDCard Init Error");
  }else{
    myNavigator.setFilesystemOk(true);
    Serial.println("SDCard Started");
  }

  Serial.println("Starting Commander ... type help to see a command list");

  //tell the SD prefab what the top layer command list is called and how large it is, prompt);
  masterCollection.setList(masterCommands, sizeof(masterCommands), prompt);
  myNavigator.setTopLayer(masterCollection);

  cmd.endOfLineChar('\r');
  cmd.delimiters("= :,\t\\|"); // use defaults except for '/' which interferes with pathnames including root directory
  cmd.stripCR(OFF);
  cmd.setStreamingMode(1);
  cmd.setStreamingMethod(1);
  cmd.echo(true);
  cmd.begin(&Serial, &Serial, masterCollection.listPtr, masterCollection.numCmds);

  cmd.commandPrompt(ON);
  cmd.commanderName = prompt;
  cmd.printCommandPrompt();
}

void loop() {
  cmd.update();
}
