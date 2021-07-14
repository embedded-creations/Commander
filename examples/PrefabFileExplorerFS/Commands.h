//All commands for 'master'


 /*
  * This needs to be passed to the commander object so it knows how big the array of commands is, but this happens earlier in setup().
  * This has already been forward declared before setup() so the compiler knows it exists
  */
/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/


//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool sdStatusHandler(Commander &Cmdr){
  if(!myNavigator.isFilesystemOk()){
    Cmdr.println("No SD Card");
  }else{
    Cmdr.println("SD Card detected");
    //Cmdr.println(Cmdr.getPayloadString());
  }
  return 0;
}

CommandCollection masterCollection;

bool sdFileHandler(Commander &Cmdr){
  if(!myNavigator.isFilesystemOk()){
    Cmdr.println("No SD Card");
    return 0;
  }
  //For the moment - block commands if they are chained
  if(Cmdr.hasPayload()){
    Cmdr.println("Chained commands are disabled");
    //This is blocking when using coolterm ...?
    return 0;
  }
  Cmdr.println("Opening SD Navigator");
  //Cmdr.attachCommands(getCommands, numOfGetCmds);
  //cmdName = "SD:";
  //if(Cmdr.transferTo(fileCommands, numOfFileCommands, cmdName)){
  if(Cmdr.transferTo(myNavigator)){
    //commander returns true if it is passing back control;
    Cmdr.transferBack(masterCollection);
  }
  return 0;
}

//COMMAND ARRAY ------------------------------------------------------------------------------

const commandList_t masterCommands[] = {
  {"status", sdStatusHandler, "Check SD Card status"},
  {"SD", sdFileHandler, "Open SD explorer"},
};


