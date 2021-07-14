//All commands for 'set'

/* Command handler template
bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
}
*/

extern CommandCollection masterCollection;

class SetCollection : public CommandCollection {
  public:
    SetCollection(int setNumber) {this->setNumber = setNumber;}

    //COMMAND ARRAY ------------------------------------------------------------------------------

    const commandList_t setCommands[4] = {
      {"hello",    setHelloHandler, "Say hello"},
      {"int",      setIntVariable,     "set my int variable"},
      {"float",    setfloatVariable,     "set my float variable"},
      {"exit",     exitSet,         "Exit sub command mode"},
    };

    //COMMAND HANDLERS ---------------------------------------------------------------------------

    // this method uses setNumber which is non-static, and so CC_METHOD wrapper must be used
    CC_METHOD(SetCollection, setHelloHandler, Cmdr) {
      Cmdr.print("Hello! this is ");
      Cmdr.println(Cmdr.commanderName);

      Cmdr.print("My setNumber is ");
      Cmdr.println(setNumber);

      return 0;
    }

    static bool setIntVariable(Commander &Cmdr){
      if(Cmdr.getInt(myInt)){
        Cmdr.print("Setting my int to ");
        Cmdr.println(myInt);
      }else Cmdr.println("Error");
      return 0;
    }

    static bool setfloatVariable(Commander &Cmdr){
      if(Cmdr.getFloat(myFloat)){
        Cmdr.print("Setting my float to ");
        Cmdr.println(myFloat);
      }else Cmdr.println("Error");
      return 0;
    }

    static bool exitSet(Commander &Cmdr){
      //had over control to the sub commander
      Cmdr.println("Passing control back to main command handler");
      //transfer back to the master command list
      Cmdr.transferBack(masterCollection);
      return 0;
    }

  private:
    int setNumber;
};

SetCollection mySetCollection(1);
SetCollection mySetCollection2(2);

