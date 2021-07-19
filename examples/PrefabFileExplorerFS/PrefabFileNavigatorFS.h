//Commander prefab: SD file navigator (based on SDfat and fatfscli example from XYmodem library)
#ifndef PrefabFileNavigatorSD_h
#define PrefabFileNavigatorSD_h

#include <Arduino.h>
#include <string.h>
#include <FS.h>

// Depends on forked version of XYmodem library: https://github.com/embedded-creations/XYmodem
#include <xymodem.h>

class FileNavigator : public CommandCollection {
  public:
    FileNavigator(FS &fs, String cmdName) {
      fsptr = &fs;
      this->cmdName = cmdName; // TODO: just use CommandCollection.name?

      setList(fileCommands, sizeof(fileCommands), cmdName);
    }

    FileNavigator(FS &fs, String cmdName, Stream &debugport)
    : rxymodem(&debugport) {
      fsptr = &fs;
      this->cmdName = cmdName; // TODO: just use CommandCollection.name?

      setList(fileCommands, sizeof(fileCommands), cmdName);
    }

    void setTopLayer(CommandCollection &topLayer){
      this->topLayer = &topLayer;
    }

    CC_METHOD(FileNavigator, receiveYmodemLoop, Cmdr) {
      if (XYmodemMode){
        // TODO:   if(Cmdr.isStreaming() == false){ //clean up close any open file } // are we guaranteed to receive one last call after isStreaming is set to false?

        if (rxymodem.loop() == 0) {
          XYmodemMode = false;
          Cmdr.stopStreaming();
          // TODO: store echo setting and restore, instead of assuming it should be on?
          Cmdr.echo(true);
          // wait for the terminal to wrap up the transfer and start printing again
          // TODO: figure out why command prompt isn't returning automatically consistently 
          delay(250);
        }
      }

      return 0;
    }

    const commandList_t fileCommands[6] = {
      //{"mkdir", makeDirectory, "make a sub directory"},
      {"cd", changeDirectory, "Change directory - use / for root"},
      {"ls", printDirectory, "Print directory"},
      //{"remove", removeFile, "Delete a file"},
      //{"removedir", removeDirectory, "Delete an empty directory"},
      //{"rename", renameFile, "rename a file or sub directory"},
      {"read", readFileContents, "Read file and send to terminal"},
      //{"write", writeToFileHandler, "Create file and open for writing - Send ASCII code 0x04 to terminate"},
      {"status", noFsError, "check filesystem status"},
      {"rb", receiveYmodem, "Start YMODEM Batch Receive"},
      {"exit", exitHandler, "Exit sub command"}
    };

    CC_METHOD(FileNavigator, noFsError, Cmdr) {
      if(!filesystemOK) Cmdr.println("ERR: No Filesystem Mounted");
      else  Cmdr.println("Filesystem Mounted");
      return 0;
    }

    CC_METHOD(FileNavigator, receiveYmodem, Cmdr) {
      if(!filesystemOK)  return noFsError(Cmdr);

      Cmdr.attachSpecialHandler(receiveYmodemLoop);
      // TODO: clear buffer before streaming?
      // TODO: store echo setting and restore, instead of assuming it should be on?
      Cmdr.echo(false);
      Cmdr.startStreaming();
      XYmodemMode = true;
      rxymodem.start_rb(Cmdr, *fsptr, true, true);

      return 0; 
    }

    CC_METHOD(FileNavigator, readFileContents, Cmdr) {
      char pathname[128+1];

      String fileString = Cmdr.getPayloadString(); // This works in one line without intermediate String on Teensy, but not on ESP32
      const char * filename = fileString.c_str();

      if(!filesystemOK)  return noFsError(Cmdr);

      if (make_full_pathname(Cmdr, filename, pathname, sizeof(pathname)) != 0) return 0;
      File readFile = fsptr->open(pathname, FILE_READ);
      if (!readFile) {
        Cmdr.println("Error, failed to open file for reading!");
        return 0;
      }
      readFile.setTimeout(0);
      while (readFile.available()) {
        char buf[512];
        size_t bytesIn = readFile.readBytes(buf, sizeof(buf));
        if (bytesIn > 0) {
          Cmdr.write(buf, bytesIn);
        }
        else {
          break;
        }
      }
      Cmdr.println();
      return 0;
    }

    bool changeDirectoryFS(Commander &Cmdr) {
      char pathname[128+1];
      bool returnVal = false;

      String dirString = Cmdr.getPayloadString(); // This works in one line without intermediate dirString on Teensy, but not on ESP32
      const char * dirname = dirString.c_str();


      if (make_full_pathname(Cmdr, dirname, pathname, sizeof(pathname)) != 0)
        return false;

      if ((strcmp(pathname, "/") != 0) && !fsptr->exists(pathname)) {
        //Cmdr.println("Directory does not exist.");
        return false;
      }
      File d = fsptr->open(pathname);
      if (d.isDirectory()) {
        strcpy(cwd, pathname);
        returnVal = true;
      }
      else {
        //Cmdr.println("Not a directory");
      }
      d.close();
      return returnVal;
    }

    CC_METHOD(FileNavigator, changeDirectory, Cmdr) {
      if(!filesystemOK)  return noFsError(Cmdr);

      if(changeDirectoryFS(Cmdr)) {
        Cmdr.print("In: ");
        Cmdr.println(Cmdr.getPayloadString());
        if(Cmdr.getPayloadString().charAt(0) == '/' ) Cmdr.commanderName = cmdName + Cmdr.getPayloadString();
        else Cmdr.commanderName = cmdName + " /" + Cmdr.getPayloadString();
      } else {
        Cmdr.println("Error - no such directory");
      }

      return 0;
    }

    CC_METHOD(FileNavigator, printDirectory, Cmdr) {
      if(!filesystemOK)  return noFsError(Cmdr);

      File dir = fsptr->open(cwd);
      if (!dir) {
        Cmdr.println("Directory open failed");
        return 0;
      }
      if (!dir.isDirectory()) {
        Cmdr.println("Not directory");
        dir.close();
        return 0;
      }
      File child = dir.openNextFile();
      while (child) {
        // Print the file name and mention if it's a directory.
        Cmdr.print(child.name());
        Cmdr.print(" "); Cmdr.print(child.size(), DEC);
        if (child.isDirectory()) {
          Cmdr.print(" <DIR>");
        }
        Cmdr.println();
        // Keep calling openNextFile to get a new file.
        // When you're done enumerating files an unopened one will
        // be returned (i.e. testing it for true/false like at the
        // top of this while loop will fail).
        child = dir.openNextFile();
      }

      return 0;
    }

    CC_METHOD(FileNavigator, exitHandler, Cmdr) {
      if(topLayer == NULL){
        Cmdr.println("Exit not functional");
        return 0;
      }
      //close any open files ...
      Cmdr.println("Closing SD Navigator");
      Cmdr.transferBack(*topLayer);
      return 0;
    }

    int make_full_pathname(Commander &Cmdr, const char *name, char *pathname, size_t pathname_len)
    {
      if (name == NULL || name == '\0') return -1;
      if (pathname == NULL || pathname_len == 0) return -1;

      // if dir name starts with '/', it is a full pathname so make it.
      // else form full pathname with current working directory.
      if (*name == '/') {
        strncpy(pathname, name, pathname_len);
        pathname[pathname_len-1] = '\0';
      } else {
        strcpy(pathname, cwd);
        if (cwd[strlen(cwd)-1] == '/') {
          if (strlen(cwd) + strlen(name) >= pathname_len) {
            Cmdr.println("pathname too long");
            return -2;
          }
        } else {
          if (strlen(cwd) + 1 + strlen(name) >= pathname_len) {
            Cmdr.println("pathname too long");
            return -2;
          }
          strcat(pathname, "/");
        }
        strcat(pathname, name);
      }
      return 0;
    }

    void setup() {
      strcpy(cwd, "/");
    }

    bool isFilesystemOk() {
      return filesystemOK;
    }

    void setFilesystemOk(bool ok) {
      filesystemOK = ok;
    }

  private:
    String cmdName = "";
    CommandCollection *topLayer;
    char cwd[128+1];     // Current Working Directory
    FS *fsptr;
    bool XYmodemMode = false;
    bool filesystemOK = false;
    XYmodem rxymodem; // TODO: make static?  Can constructor specifying debug port still work?
};

// TODO: migrate more Prefab Methods into FileNavigator
#if 0
//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function
bool makeDirectory(Commander &Cmdr){
  if(!SDOK)  return noFsError(Cmdr);
  if(SD.mkdir( Cmdr.getPayloadString().c_str() )){
    Cmdr.print("Created: ");
    Cmdr.println(Cmdr.getPayloadString());
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------

bool removeFile(Commander &Cmdr){
  if(!SDOK)  return noFsError(Cmdr);
	closeFiles();
  if(SD.remove( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool removeDirectory(Commander &Cmdr){
  if(!SDOK)  return noFsError(Cmdr);
  if(SD.rmdir( Cmdr.getPayloadString().c_str())){
    Cmdr.print("Removed: ");
    Cmdr.println(Cmdr.getPayloadString());
  }else{
    Cmdr.println("operation failed");
  }
  return 0;
}
//-----------------------------------------------------------------------------------------------
bool renameFile(Commander &Cmdr){
  if(!SDOK)  return noFsError(Cmdr);
	closeFiles();
  String pld = Cmdr.getPayloadString(); //get the payload without any newline
  int idx = pld.indexOf(" "); //find the first space
  if(idx == -1){
    Cmdr.println("Error, unable to handle command");
    return 0;
  }
  String sub1 = pld.substring(0, idx); //get the first word.
  String sub2 = pld.substring(idx+1, pld.length());
  Cmdr.print("Changing ");
  Cmdr.print(sub1);
  Cmdr.print(" to ");
  Cmdr.println(sub2);
  if(SD.rename(sub1.c_str(), sub2.c_str())){
    Cmdr.println("File or directory has been renamed");
  }else   Cmdr.println("Error: Operation failed");
  return 0;
}
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
bool writeToFileHandler(Commander &Cmdr){
	/* Open a file and write data to it.
		 Use the payload as the filename
		 Set Commander to Stream mode and wait for contents to be downloaded
	*/
	closeFiles();
	if(Cmdr.hasPayload()){
		//create file
		file1 = SD.open(Cmdr.getPayloadString().c_str(), O_WRITE | O_CREAT);
		Cmdr.print(F("Created file: "));
		Cmdr.println(Cmdr.getPayloadString());
		Cmdr.startStreaming();
	}else Cmdr.print(F("Error - Filename required"));
  return 0;
}
//-----------------------------------------------------------------------------------------------

bool streamToFileHandler(Commander &Cmdr){
	//special handler for file streams
	if(file1){
		//file is not open so tell the streamer to stop
		Cmdr.println("Error: Streaming to closed file");
		Cmdr.stopStreaming();
		return 0;
	}
	if(Cmdr.bufferString.length() > 0){
		//write to the file and flush the buffer.
		Cmdr.print("WRITING:");
		Cmdr.println(Cmdr.bufferString);
		file1.print(Cmdr.bufferString);
		file1.flush();
	}
	if(Cmdr.isStreaming() == false && file1){
		//File is open but streaming has stopped - indicated end of file and time to tidy up.
		Cmdr.println("Flushing and closing file");
		file1.flush();
		file1.close();

	}
	return 0;
}

#endif

#endif //PrefabFileNavigator_h