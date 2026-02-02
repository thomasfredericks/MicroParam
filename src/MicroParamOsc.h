#include <MicroParam.h>
#include <MicroOsc.h>


    bool microParamOscDispatch(MicroOscMessage &message, MicroBind &binding)
    {

        if (binding.checkKey(message.getOscAddress()))
        {
            switch (message.getTypeTag())
            {
            case 'i':
                binding.setInt(message.nextAsInt());
                break;
            case 'f':
                binding.setFloat(message.nextAsFloat());
                break;
            default:
                // unsupported type
                break;
            }
            return true;
        }
        return false;
    };  

    bool microParamOscDispatch(MicroOscMessage &message, MicroBind *bindings, size_t bindingsCount)
    {

        for (size_t p = 0; p < bindingsCount; ++p)
        {
            MicroBind &binding = bindings[p];

            if ( microParamOscDispatch (message, binding) )
            {
                return true;
            };
        }
        return false;
    };

    void microParamOscSend(MicroOsc &osc, MicroBind &binding)
    {
        const char *key = binding.getKey();

        switch (binding.getType())
        {
        case 'e':
        case 'i':
            osc.sendInt(key, binding.getInt());
            break;
        case 'f':
            osc.sendFloat(key, binding.getFloat());
            break;
        }
    };

    void microParamOscSend(MicroOsc &osc, MicroBind *bindings, size_t bindingsCount)
    {
        for (size_t p = 0; p < bindingsCount; ++p)
        {
            MicroBind &binding = bindings[p];
            microParamOscSend(osc, binding);
        };
    };
