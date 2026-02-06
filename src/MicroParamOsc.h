#include <MicroParam.h>
#include <MicroOsc.h>

// ======================= MicroParamBind =======================

class MicroParamBind {
  private:
  enum Type : uint8_t { INT = 'i' , FLOAT = 'f', ENUM = 'e' } type_;
  const char *key_;
  const uint32_t keyHash_;
  

  union {
    MicroParamInt   *i;
    MicroParamFloat *f;
    MicroParamEnum  *e;
  } ptr_;

  public:
  // --------- Constructors ---------
  MicroParamBind(const char *k, MicroParamInt &v)
      : type_(INT), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.i = &v;
  }

  MicroParamBind(const char *k, MicroParamFloat &v)
      : type_(FLOAT), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.f = &v;
  }

  MicroParamBind(const char *k, MicroParamEnum &v)
      : type_(ENUM), key_(k) , keyHash_(microHashFnv1a(k)) {
    ptr_.e = &v;
  }

  Type getType() const {
    return type_;
  }

  const char *getKey() const {
    return key_;
  }

  bool matches(const char *k) const {
    return strcmp(key_, k) == 0;
  }

  bool matches(uint32_t hash) const {
    return keyHash_ == hash;
  }

  bool matches(uint32_t hash, const char *addr) const {
        if (hash != keyHash_) return false;
        return strcmp(key_, addr) == 0; 
    }

  // --------- Access ---------
  void setInt(int32_t v) {
    switch (type_) {
      case INT:   ptr_.i->set(v); break;
      case FLOAT: ptr_.f->set((float)v); break;
      case ENUM:  ptr_.e->set(v); break;
    }
  }

  int32_t getInt() const {
    switch (type_) {
      case INT:   return ptr_.i->get();
      case FLOAT: return (int32_t)ptr_.f->get();
      case ENUM:  return ptr_.e->get();
    }
    return 0;
  }

  void setFloat(float v) {
    switch (type_) {
      case FLOAT: ptr_.f->set(v); break;
      case INT:   ptr_.i->set((int32_t)floor(v)); break;
      case ENUM:  ptr_.e->set((int32_t)floor(v)); break;
    }
  }

  float getFloat() const {
    switch (type_) {
      case FLOAT: return ptr_.f->get();
      case INT:   return (float)ptr_.i->get();
      case ENUM:  return (float)ptr_.e->get();
    }
    return 0.0f;
  }
};


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
