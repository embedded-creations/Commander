//All commands for 'master'

/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/
CommandCollection masterCollection;

//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool helloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool getHandler(Commander &Cmdr){
  //This handles commands that have a list of 'get' commands.
  //Sending the command, plus one of the 'get' commands will pass this command to the get command handler, 
  //the command will be carried out, and control will return to the master.
  //Sending just the 'get' command will transfer control down to the set commad list.
  //For example: Sending 'get help' will call gets help command, but if you then send 'help', it will call masters help command.
  //Sending 'get' will transfer control to the set command list, then sending 'help' will print gets command help
  //get must impliment its own function to return control, for exampe an 'exit' command.
  Cmdr.println("Passing control to get command handler");
  //Cmdr.attachCommands(getCommands, numOfGetCmds);
  if(Cmdr.transferTo(getCollection)){
    //commander returns true if it is passing back control;
    Cmdr.transferBack(masterCollection);
  }

  //if(Cmdr.hasPayload()){
  //  Serial.println("handing payload to get command list");
  //  retVal  = Cmdr.feed(Cmdr);
  //}
  return 0;
}

bool setHandler(Commander &Cmdr){
  //This handles commands that are sub commands of the 'set' command.
  //Sending the command, plus one of the 'set' commands will pass this command to the sub command handler, 
  //the command will be carried out, and control will return to the master.
  //Sending just the 'set' command will transfer control down to the set commad list.
  //For example: Sending 'set help' will call sets help command, but if you then send 'help', it will call masters help command.
  //Sending 'set' will transfer control to the set command list, then sending 'help' will print sets command help
  //set must impliment its own function to return control, for exampe an 'exit' command.
  bool retVal = 0;
  Cmdr.println("Passing to set command handler");
  if(Cmdr.transferTo(mySetCollection)){
    //Cmdr.commanderName = "set";
    //commander returns true if it is passing back control;
    Cmdr.transferBack(masterCollection);
  }
  return 0;
}

bool setHandler2(Commander &Cmdr){
  //This handles commands that are sub commands of the 'set' command.
  //Sending the command, plus one of the 'set' commands will pass this command to the sub command handler, 
  //the command will be carried out, and control will return to the master.
  //Sending just the 'set' command will transfer control down to the set commad list.
  //For example: Sending 'set help' will call sets help command, but if you then send 'help', it will call masters help command.
  //Sending 'set' will transfer control to the set command list, then sending 'help' will print sets command help
  //set must impliment its own function to return control, for exampe an 'exit' command.
  bool retVal = 0;
  Cmdr.println("Passing to set2 command handler");
  if(Cmdr.transferTo(mySetCollection2)){
    //Cmdr.commanderName = "set";
    //commander returns true if it is passing back control;
    Cmdr.transferBack(masterCollection);
  }
  return 0;
}

//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"hello",      helloHandler, "Say hello"},
  {"get",        getHandler,   "call get commands"},
  {"set",        setHandler,   "call set commands"},
  {"set2",        setHandler2,   "call set2 commands"},
};