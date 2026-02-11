#include <MicroParam.h>
#include <MicroOsc.h>

// ======================= MicroParamBindOsc =======================
class MicroParamBindOsc
{
private:
    const char *address_;
    uint32_t addressHash_;

    const char *typeTags_;
    uint32_t typeTagsHash_;

    MicroParam *singleParam_[1]; // storage for single param
    MicroParam **params_;        // array of params
    size_t count_;

public:
    static uint32_t generateHash(const char *s)
    {
        return microHashFnv1a(s);
    }

    // -------- Single param ----------
    MicroParamBindOsc(const char *address,
                      const char *typeTags,
                      MicroParam &param)
        : address_(address),
          addressHash_(generateHash(address)),
          typeTags_(typeTags),
          typeTagsHash_(generateHash(typeTags)),
          count_(1)
    {
        singleParam_[0] = &param;
        params_ = singleParam_;
    }

    // -------- Array of params ----------
    MicroParamBindOsc(const char *address,
                      const char *typeTags,
                      MicroParam **paramArray)
        : address_(address),
          addressHash_(generateHash(address)),
          typeTags_(typeTags),
          typeTagsHash_(generateHash(typeTags)),
          params_(paramArray)
    {
        count_ = strlen(typeTags);
    }

    // -------- Accessors ----------
    const char *getAddress() const { return address_; }
    uint32_t getAddressHash() const { return addressHash_; }

    const char *getTypeTags() const { return typeTags_; }
    uint32_t getTypeTagsHash() const { return typeTagsHash_; }

    size_t getCount() const { return count_; }

    MicroParam &getParam() const { return *params_[0]; }
    MicroParam &getParam(size_t index) const { return *params_[index]; }

    // -------- Matching ----------
    bool matchesAddress(const char *addr) const { return strcmp(address_, addr) == 0; }

    bool matchesAddress(uint32_t hash) const { return addressHash_ == hash; }

    bool matchesTypeTags(uint32_t hash) const { return typeTagsHash_ == hash; }

    bool matchesTypeTags(const char *tags) const { return (strcmp(typeTags_, tags) == 0); }
};

// ======================= MicroParam OSC Utilities =======================

bool microParamOscDispatch(MicroOscMessage &message, MicroParamBindOsc *bindings, size_t bindingsCount)
{
    const char *address = message.getOscAddress();

    const uint32_t addrHash = MicroParamBindOsc::generateHash(address);
    const char *typeTags = message.getTypeTags();
    const uint32_t typeHash = MicroParamBindOsc::generateHash(typeTags);

    for (size_t i = 0; i < bindingsCount; i++)
    {
        MicroParamBindOsc &binding = bindings[i];

        if (binding.matchesAddress(addrHash) && binding.matchesTypeTags(typeHash)) // Address and type tags match
        {
            for (size_t i = 0; i < binding.getCount(); i++)
            {
                switch (typeTags[i])
                {
                case 'i':
                    binding.getParam(i).setInt(message.nextAsInt());
                    break;

                case 'f':
                    binding.getParam(i).setFloat(message.nextAsFloat());
                    break;

                case 's':
                    binding.getParam(i).setString(message.nextAsString());
                    break;

                default:
                    return false;
                }
            }
            return true;
        }
    }
    return false;
};

void microParamOscSend(MicroOsc &osc, MicroParamBindOsc &binding)
{
    const char *address = binding.getAddress();
    size_t count = binding.getCount();
    const char *tags = binding.getTypeTags();

    // Loop over params according to typetags
    for (size_t i = 0; i < count; ++i)
    {
        char tag = tags[i]; // get the expected OSC typetag
        MicroParam &param = binding.getParam(i);

        switch (tag)
        {
        case 'i': // integer
            osc.sendInt(address, param.getInt());
            break;

        case 'f': // float
            osc.sendFloat(address, param.getFloat());
            break;

        case 's': // string
        {
            osc.sendString(address, param.getString());
            break;
        }

        default:
            // unsupported type tag
            break;
        }
    }
};

void microParamOscSend(MicroOsc &osc, MicroParamBindOsc *bindings, size_t bindingsCount)
{
    for (size_t p = 0; p < bindingsCount; ++p)
    {
        MicroParamBindOsc &binding = bindings[p];
        microParamOscSend(osc, binding);
    };
};
