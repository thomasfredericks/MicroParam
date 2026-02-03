#include <MicroParam.h>
#include <MicroOsc.h>

bool microParamOscDispatch(MicroOscMessage &message, MicroParamBind *bindings, size_t bindingsCount)
{
    const char *addr = message.getOscAddress();
    const uint32_t addrHash = microHashFnv1a(addr);

    for (size_t p = 0; p < bindingsCount; ++p)
    {
        MicroParamBind &binding = bindings[p];

        if (binding.matches(addrHash, addr)) // Could be optimized with hash comparison only
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
    }
    return false;
};

void microParamOscSend(MicroOsc &osc, MicroParamBind &binding)
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

void microParamOscSend(MicroOsc &osc, MicroParamBind *bindings, size_t bindingsCount)
{
    for (size_t p = 0; p < bindingsCount; ++p)
    {
        MicroParamBind &binding = bindings[p];
        microParamOscSend(osc, binding);
    };
};
