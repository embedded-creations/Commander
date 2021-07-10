/*Commander example - single object, multi layer command structure
 * Demonstrates how to use one commander object with multiple command arrays to create a layerd command structure
 * Top level commands can be invoked by typing them
 * Lower commands can be directly invoked from the top layer by typing the top command followed by the lower command
 * For example 'get help' calls the 'help' function in the command set called 'get'
 * Lower command structures can be entered by typing their command, and then the lower level commands can be directly invoked
 * An exit command will return control to the top level
 * For example, 'get' will transfer control to the 'get' command set. 'help' will then call the help function for the 'get' commands.
 */
#include <Commander.h>

//portSettings_t savedSettings;
Commander cmd;
//Variables we can set or get
int myInt = 0;
float myFloat = 0.0;

// include header files with commands after declaring any variables used by the commands
#include "getCommands.h"
#include "setCommands.h"
#include "masterCommands.h"
//SETUP ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  masterCollection.setList(masterCommands, sizeof(masterCommands));
  setCollection.setList(setCommands, sizeof(setCommands));
  getCollection.setList(getCommands, sizeof(getCommands));
  cmd.begin(&Serial, masterCollection.listPtr, masterCollection.numCmds);
  cmd.commanderName = "Cmd";
  cmd.commandPrompt(ON);
  cmd.echo(true);
  while(!Serial){;}
  Serial.println("Hello: Type 'help' to get help");
  cmd.printCommandPrompt();
}

//MAIN LOOP ---------------------------------------------------------------------------
void loop() {
  //Call the update functions using the activeCommander pointer
  cmd.update();
}
