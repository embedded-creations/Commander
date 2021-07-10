//All commands for 'set'

/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/
CommandCollection setCollection;
extern CommandCollection masterCollection;


//COMMAND HANDLERS ---------------------------------------------------------------------------

bool setHelloHandler(Commander &Cmdr){
  Cmdr.print("Hello! this is ");
  Cmdr.println(Cmdr.commanderName);
  return 0;
}

bool setIntVariable(Commander &Cmdr){
  
  if(Cmdr.getInt(myInt)){
    Cmdr.print("Setting my int to ");
    Cmdr.println(myInt);
  }else Cmdr.println("Error");
  return 0;
}

bool setfloatVariable(Commander &Cmdr){
  if(Cmdr.getFloat(myFloat)){
    Cmdr.print("Setting my float to ");
    Cmdr.println(myFloat);
  }else Cmdr.println("Error");
  return 0;
}

bool exitSet(Commander &Cmdr){
  //had over control to the sub commander
  Cmdr.println("Passing control back to main command handler");
  //transfer back to the master command list
  Cmdr.transferBack(masterCollection, "Cmd");
  return 0;
}

//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t setCommands[] = {
  {"hello",    setHelloHandler, "Say hello"},
  {"int",      setIntVariable,     "set my int variable"},
  {"float",    setfloatVariable,     "set my float variable"},
  {"exit",     exitSet,         "Exit sub command mode"},
};
