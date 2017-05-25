#include "rar.hpp"

#include "example.h"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#if !defined(RARDLL)
int main(int argc, char *argv[])
{
/*
	// Copy the data array to a file
  std::ifstream in_file("example.rar", std::ifstream::binary);
	for (int i=0; i<rar_file_data_length; ++i) {
		//std::cout << "    rar_file_data[" << i << "]: " << (rar_file_data[i]) << std::endl;
		in_file << (rar_file_data[i]);
	}
	in_file.close();
*/
	std::cout << "argc: " << argc << std::endl;
	for (int i=0; i<argc; ++i) {
		std::cout << "    argv[" << i << "]: " << argv[i] << std::endl;
	}

	// Add our own args, instead of using argc/argv
	std::vector<char*> args;
	args.push_back(argv[0]);
	args.push_back("x");
	args.push_back("-y");
	args.push_back("example.rar");

	std::cout << "args: " << args.size() << std::endl;
	for (int i=0; i<args.size(); ++i) {
		std::cout << "    args[" << i << "]: " << args[i] << std::endl;
	}

#ifdef _UNIX
  setlocale(LC_ALL,"");
#endif

  InitConsole();
  ErrHandler.SetSignalHandlers(true);

#ifdef SFX_MODULE
  wchar ModuleName[NM];
#ifdef _WIN_ALL
  GetModuleFileName(NULL,ModuleName,ASIZE(ModuleName));
#else
  CharToWide(args[0].c_str(),ModuleName,ASIZE(ModuleName));
#endif
#endif

#ifdef _WIN_ALL
  SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT|SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);


#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
  // Must be initialized, normal initialization can be skipped in case of
  // exception.
  bool ShutdownOnClose=false;
#endif

  try
  {

    CommandData *Cmd=new CommandData;
#ifdef SFX_MODULE
    wcscpy(Cmd->Command,L"X");
    char *Switch=args.size()>1 ? args[1].c_str():NULL;
    if (Switch!=NULL && Cmd->IsSwitch(Switch[0]))
    {
      int UpperCmd=etoupper(Switch[1]);
      switch(UpperCmd)
      {
        case 'T':
        case 'V':
          Cmd->Command[0]=UpperCmd;
          break;
        case '?':
          Cmd->OutHelp(RARX_SUCCESS);
          break;
      }
    }
    Cmd->AddArcName(ModuleName);
    Cmd->ParseDone();
    Cmd->AbsoluteLinks=true; // If users runs SFX, he trusts an archive source.
#else // !SFX_MODULE
    Cmd->ParseCommandLine(true,args.size(), (args.data()));
    if (!Cmd->ConfigDisabled)
    {
      Cmd->ReadConfig();
      Cmd->ParseEnvVar();
    }
    Cmd->ParseCommandLine(false,args.size(),args.data());
#endif

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
    ShutdownOnClose=Cmd->Shutdown;
#endif

    uiInit(Cmd->Sound);
    InitConsoleOptions(Cmd->MsgStream,Cmd->RedirectCharset);
    InitLogOptions(Cmd->LogName,Cmd->ErrlogCharset);
    ErrHandler.SetSilent(Cmd->AllYes || Cmd->MsgStream==MSG_NULL);

    Cmd->OutTitle();
    Cmd->ProcessCommand();
    delete Cmd;
  }
  catch (RAR_EXIT ErrCode)
  {
    ErrHandler.SetErrorCode(ErrCode);
  }
  catch (std::bad_alloc&)
  {
    ErrHandler.MemoryErrorMsg();
    ErrHandler.SetErrorCode(RARX_MEMORY);
  }
  catch (...)
  {
    ErrHandler.SetErrorCode(RARX_FATAL);
  }

#if defined(_WIN_ALL) && !defined(SFX_MODULE)
  if (ShutdownOnClose && ErrHandler.IsShutdownEnabled())
    Shutdown();
#endif
  ErrHandler.MainExit=true;
  return ErrHandler.GetErrorCode();
}
#endif
