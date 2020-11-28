#include "../Dependencies/FoundationIO/Library/include/TestIO.h"
#include "../Library/include/CodecIO.h"
#include "../Dependencies/FoundationIO/Library/include/TextIO/CommandLineIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif

    /*
     Ok, so we can embed a JPEG file here and make sure it's decoded correctly
     */
    
    typedef enum Switches {
        Input       = 1,
        Output      = 2,
        Help        = 3,
        NumSwitches = Help,
    } Switches;

    int main(const int argc, const char *argv[]) {
        CommandLineIO *CLI = CommandLineIO_Init(NumSwitches);
        if (CLI != NULL) {
            CommandLineIO_SetMinOptions(CLI, 2);
            CommandLineIO_Switch_SetName(CLI, Input,  UTF32String("Input"));
            CommandLineIO_Switch_SetName(CLI, Output, UTF32String("Output"));
            CommandLineIO_Switch_SetName(CLI, Help,   UTF32String("Help"));
            CommandLineIO_Switch_SetType(CLI, Input,  SwitchType_Required);
            CommandLineIO_Switch_SetType(CLI, Output, SwitchType_Required);
            CommandLineIO_Switch_SetType(CLI, Help,   SwitchType_Standalone);
            CommandLineIO_SetHelpOption(CLI, Help);
        }
        return 0;
    }
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
