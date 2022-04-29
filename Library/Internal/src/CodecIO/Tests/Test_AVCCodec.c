#include "../../../../OVIA/include/CodecIO.h"

#include "../../../../../Dependencies/FoundationIO/Library/include/TestIO.h"
#include "../../../../../Dependencies/FoundationIO/Library/include/TextIO/CommandLineIO.h"

#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
extern "C" {
#endif
    
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

    /*
     What does TestIO need to look like?

     Small AVC stream embedded in code here, like 1kb max.

     main function that calls the AVCCodec functions to decode the stream and compare it via MediaIO to a reference image, probaby 8x8 MVC 3 channel, etc.

     TestIO fits in how? what does TestIO need to actually do here?

     Well, it needs to verify all the functions in AVCCodec work like they shoud, which means the functions need to be registered.
     */
    
#if (PlatformIO_Language == PlatformIO_LanguageIsCXX)
}
#endif
