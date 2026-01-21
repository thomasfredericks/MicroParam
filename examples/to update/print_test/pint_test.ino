
#include <Arduino.h>
#include <MicroParam.h>

MicroParamInt amp_attack(2, 0, 128);
MicroParamInt amp_decay(400, 1, 2000);
MicroParamInt filter_attack(0, 0, 128);
MicroParamInt filter_decay(256, 1, 2000);
MicroParamInt filter_mod(32, 0, 256);
MicroParamInt filter_cutoff(64, 0, 128);
MicroParamInt filter_resonance(128, 0, 256);


MicroParamBind bindings[] = {
    {"/vca/atk", &amp_attack},
    {"/vca/dcy", &amp_decay},
    {"/vcf/atk", &filter_attack},
    {"/vcf/dcy", &filter_decay},
    {"/vcf/mod", &filter_mod},
    {"/vcf/cut", &filter_cutoff},
    {"/vcf/res", &filter_resonance}
};
MicroParamBinder binder(bindings, sizeof(bindings) / sizeof(MicroParamBind));

unsigned long chrono;

void setup()
{
    Serial.begin(115200);

    chrono = millis();
}


void loop()
{

    if (millis() - chrono >= 1000)
    {
        chrono = millis();

        size_t count = binder.getCount();

        for (size_t i = 0; i < count; ++i)
        {
            MicroParam *param = binder.getPointer(i);
            Serial.print(binder.getKey(i));
            Serial.print(' ');
            Serial.print(param->getType());
            Serial.print(' ');
            param->printValue(Serial);
            Serial.print(' ');
            param->printDefinition(Serial,' ');
            Serial.println();
        }

    }
}
