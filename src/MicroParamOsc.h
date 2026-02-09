#include <MicroParam.h>
#include <MicroOsc.h>




bool microParamOscDispatch(MicroOscMessage &message, MicroParamBindKey *bindings, size_t bindingsCount)
{
    const char *addr = message.getOscAddress();
    const uint32_t addrHash = MicroParamBindKey::generateHash(addr);

    for (size_t i = 0; i < bindingsCount; i++)
    {
        MicroParamBindKey & binding = bindings[i];

        if (binding.matches(addrHash, addr)) // Could be optimized with hash comparison only
        {
                switch (message.getTypeTag())
                {
                case 'i':
                    binding.getParam().setInt(message.nextAsInt());
                    break;
                case 'f':
                    binding.getParam().setFloat(message.nextAsFloat());
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

void microParamOscSend(MicroOsc &osc, MicroParamBindKey & binding)
{
    const char *key = binding.getKey();
    MicroParam & param = binding.getParam();

    switch (param.getType())
    {
    case MicroParam::Type::ENUM :
    case MicroParam::Type::INT :
        osc.sendInt(key, param.getInt());
        break;
    case MicroParam::Type::FLOAT:
        osc.sendFloat(key, param.getFloat());
        break;
    }
};

void microParamOscSend(MicroOsc &osc, MicroParamBindKey *bindings, size_t bindingsCount)
{
    for (size_t p = 0; p < bindingsCount; ++p)
    {
        MicroParamBindKey & binding = bindings[p];
        microParamOscSend(osc, binding);
    };
};
