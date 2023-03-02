#include "AcqirisD1Import.h"
#include <iostream>

int main()
{
    // Abrir el dispositivo
    ViSession vi;
    AcqrsD1_acquire("PXI1Slot2", &vi);

    // Configuración del canal
    ViInt32 channel = 1;
    AcqrsD1_configChannel(vi, channel, 0, 0, 8, 0); // Canal diferencial, acoplamiento DC, 8 bits de resolución

    // Configuración del desencadenador interno
    AcqrsD1_configTrigClass(vi, 0x8000, 0x2000, 0, 0); // Desencadenador interno
    AcqrsD1_configTrigSource(vi, channel, 0, 0.5, 0.0); // Nivel de desencadenamiento en 0.5V

    // Configuración del modo de adquisición
    ViInt32 samplesPerRecord = 1000; // Número de muestras por registro
    ViInt32 recordsPerAcquisition = T/dt; // Número de registros por adquisición
    ViInt32 segmentIndex = 0;
    AcqrsD1_configMode(vi, 1, samplesPerRecord, recordsPerAcquisition, segmentIndex); // Modo de adquisición de registro único

    // Configuración del tiempo de adquisición
    ViReal64 delayTime = 0.0; // Tiempo de espera antes de comenzar la adquisición
    ViReal64 acquisitionTime = T; // Tiempo de adquisición en segundos
    AcqrsD1_configAcqTiming(vi, delayTime, acquisitionTime, 0, 0); // Tiempo de adquisición

    // Inicio de la adquisición
    AcqrsD1_acquire(vi);

    // Lectura de los datos adquiridos
    ViInt32 dataBuffer[1000];
    for (int i = 0; i < recordsPerAcquisition; i++)
    {
        AcqrsD1_readData(vi, channel, 0, samplesPerRecord, dataBuffer, &actualNumberOfPoints, NULL, NULL);
        std::cout << "Registro " << i+1 << ":" << std::endl;
        for (int j = 0; j < actualNumberOfPoints; j++)
        {
            std::cout << dataBuffer[j] << " ";
        }
        std::cout << std::endl;
    }

    // Cerrar el dispositivo
    AcqrsD1_close(vi);
    return 0;
}