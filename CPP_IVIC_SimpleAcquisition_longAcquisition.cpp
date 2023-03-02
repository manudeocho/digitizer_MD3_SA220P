///
/// Acqiris IVI-C Driver Example Program
///
/// Initializes the driver, reads a few Identity interface properties, and performs a
/// simple record acquisition.
///
/// For additional information on programming with IVI drivers in various IDEs, please see
/// http://www.ivifoundation.org/resources/
///
/// Runs in simulation mode without an instrument.
///

#include "AqMD3.h"

#include <iostream>
#include <fstream>
using std::cout;
using std::cerr;
using std::hex;
#include <vector>
using std::vector;
#include <stdexcept>
using std::runtime_error;
using std::ofstream;
using std::ios_base;

#define checkApiCall( f ) do { ViStatus s = f; testApiCall( s, #f ); } while( false )

// Edit resource and options as needed. Resource is ignored if option has Simulate=true.
// An input signal is necessary if the example is run in non simulated mode, otherwise
// the acquisition will time out.
ViChar resource[] = "PXI157::0::0::INSTR";
ViChar options[]  = "Simulate=false, DriverSetup= Model=SA220P";

// Acquisition configuration parameters
ViReal64 const sampleRate = 2.0e9;
ViReal64 const sampleInterval = 1.0 / sampleRate;
ViInt64 recordSize = 1000;
ViInt64 const numRecords = 1;
ViInt32 const acquisitionMode = AQMD3_VAL_ACQUISITION_MODE_NORMAL;

// Channel configuration parameters
ViReal64 const range = 2.5;
ViReal64 const offset = 0.0;
ViInt32 const coupling = AQMD3_VAL_VERTICAL_COUPLING_DC;

// Trigger configuration parameters
ViConstString triggerSource = "External1";
ViReal64 const triggerLevel = 0.5;
ViInt32 const triggerSlope = AQMD3_VAL_TRIGGER_SLOPE_POSITIVE;

const char DataFileName[] = "SA220P_Ch1.bin";


// Utility function to check status error during driver API call.
void testApiCall( ViStatus status, char const * functionName )
{
    ViInt32 ErrorCode;
    ViChar ErrorMessage[512];

    if( status>0 ) // Warning occurred.
    {
        AqMD3_GetError( VI_NULL, &ErrorCode, sizeof( ErrorMessage ), ErrorMessage );
        cerr << "** Warning during " << functionName << ": 0x" << hex << ErrorCode << ", " << ErrorMessage << '\n';

    }
    else if( status<0 ) // Error occurred.
    {
        AqMD3_GetError( VI_NULL, &ErrorCode, sizeof( ErrorMessage ), ErrorMessage );
        cerr << "** ERROR during " << functionName << ": 0x" << hex << ErrorCode << ", " << ErrorMessage << '\n';
        throw runtime_error( ErrorMessage );
    }
}


int main()
{
    cout << "SimpleAcquisition\n\n";

    // Initialize the driver. See driver help topic "Initializing the IVI-C Driver" for additional information.
    ViSession session;
    ViBoolean const idQuery = VI_FALSE;
    ViBoolean const reset   = VI_FALSE;
    checkApiCall( AqMD3_InitWithOptions( resource, idQuery, reset, options, &session ) );

    cout << "Driver initialized \n";

    // Read and output a few attributes.
    ViChar str[128];
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_SPECIFIC_DRIVER_PREFIX,               sizeof( str ), str ) );
    cout << "Driver prefix:      " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_SPECIFIC_DRIVER_REVISION,             sizeof( str ), str ) );
    cout << "Driver revision:    " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_SPECIFIC_DRIVER_VENDOR,               sizeof( str ), str ) );
    cout << "Driver vendor:      " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_SPECIFIC_DRIVER_DESCRIPTION,          sizeof( str ), str ) );
    cout << "Driver description: " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_INSTRUMENT_MODEL,                     sizeof( str ), str ) );
    cout << "Instrument model:   " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_INSTRUMENT_INFO_OPTIONS,              sizeof( str ), str ) );
    cout << "Instrument options: " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_INSTRUMENT_FIRMWARE_REVISION,         sizeof( str ), str ) );
    cout << "Firmware revision:  " << str << '\n';
    checkApiCall( AqMD3_GetAttributeViString( session, "", AQMD3_ATTR_INSTRUMENT_INFO_SERIAL_NUMBER_STRING, sizeof( str ), str ) );
    cout << "Serial number:      " << str << '\n';

    ViBoolean simulate;
    checkApiCall( AqMD3_GetAttributeViBoolean( session, "", AQMD3_ATTR_SIMULATE, &simulate ) );
    cout << "\nSimulate:           " << ( simulate?"True":"False" ) << '\n';

     // Configure the channels.
    cout << "\nConfiguring Channel1\n";
    cout << "  Range:              " << range << '\n';
    cout << "  Offset:             " << offset << '\n';
    cout << "  Coupling:           " << (coupling ? "DC" : "AC") << '\n';
    checkApiCall(AqMD3_ConfigureChannel(session, "Channel1", range, offset, coupling, VI_TRUE));


   // Configure the acquisition
    cout << "\nConfiguring acquisition\n";
    cout << "  SampleRate:         " << sampleRate << '\n';
    cout << "  Number of records:  " << numRecords << '\n';
    cout << "  Acquisition mode:   " << acquisitionMode << '\n';
    checkApiCall(AqMD3_SetAttributeViReal64(session, "", AQMD3_ATTR_SAMPLE_RATE, sampleRate));
    checkApiCall(AqMD3_SetAttributeViInt32(session, "", AQMD3_ATTR_ACQUISITION_MODE, acquisitionMode));
    checkApiCall(AqMD3_SetAttributeViInt64(session, "", AQMD3_ATTR_NUM_RECORDS_TO_ACQUIRE, numRecords));
    checkApiCall(AqMD3_GetAttributeViInt64(session, "", AQMD3_ATTR_MAX_SAMPLES_PER_CHANNEL, &recordSize));
    cout << "  Record size:        " << recordSize << '\n';
    checkApiCall(AqMD3_SetAttributeViInt64(session, "", AQMD3_ATTR_RECORD_SIZE, recordSize));


    // Configure the trigger.
    cout << "\nConfiguring Trigger\n";
    cout << "  ActiveSource:       " << triggerSource << '\n';
    cout << "  Level:              " << triggerLevel << "\n";
    cout << "  Slope:              " << (triggerSlope ? "Positive" : "Negative") << "\n";
    checkApiCall(AqMD3_SetAttributeViString(session, "", AQMD3_ATTR_ACTIVE_TRIGGER_SOURCE, triggerSource));
    checkApiCall(AqMD3_SetAttributeViReal64(session, triggerSource, AQMD3_ATTR_TRIGGER_LEVEL, triggerLevel));
    checkApiCall(AqMD3_SetAttributeViInt32(session, triggerSource, AQMD3_ATTR_TRIGGER_SLOPE, triggerSlope));

    // Calibrate the instrument.
    cout << "\nPerforming self-calibration\n";
    checkApiCall( AqMD3_SelfCalibrate( session ) );


    ofstream fch1;
    fch1.open(DataFileName, ios_base::out | ios_base::trunc | ios_base::binary);

    // Define the readout parameters.
    ViInt64 arraySize = 0;
    checkApiCall( AqMD3_QueryMinWaveformMemory( session, 16, numRecords, 0, recordSize, &arraySize ) );

    vector<ViInt16> dataArray( arraySize );
    
    ViInt64 actualPoints[numRecords], firstValidPoint[numRecords];
    ViReal64 initialXOffset[numRecords], initialXTimeSeconds[numRecords], initialXTimeFraction[numRecords];
    ViReal64 xIncrement = 0.0, scaleFactor = 0.0, scaleOffset = 0.0;
    ViInt64 actualRecords = 0;


    // Perform the acquisition.
    ViInt32 const timeoutInMs = 1000;
    cout << "\nPerforming acquisition\n";
    checkApiCall( AqMD3_InitiateAcquisition( session ) );
    //checkApiCall(AqMD3_SendSoftwareTrigger(session));
    checkApiCall( AqMD3_WaitForAcquisitionComplete( session, timeoutInMs ) );
    cout << "Acquisition completed\n";

    // Fetch the acquired data in array.
    checkApiCall(AqMD3_FetchMultiRecordWaveformInt16(session, "Channel1", 0, 1, 0, recordSize, arraySize,
        &dataArray[0], &actualRecords, actualPoints, firstValidPoint, initialXOffset,
        initialXTimeSeconds, initialXTimeFraction, &xIncrement, &scaleFactor, &scaleOffset));


    cout << "\nProcessing data\n";
    //saving the data of the first record in a binary file
    fch1.write(reinterpret_cast<char const*>(&dataArray.at(firstValidPoint[0])), sizeof(ViInt16) * actualPoints[0]);
    fch1.close();

    // Convert data to Volts.  
    //for( ViInt64 currentRecord = 0; currentRecord<numRecords; ++currentRecord )
    //{
    //    for( ViInt64 currentPoint = 0; currentPoint<actualPoints[currentRecord]; ++currentPoint )
    //    {
    //        ViReal64 valueInVolts = dataArray[firstValidPoint[currentRecord]+currentPoint]*scaleFactor + scaleOffset;
    //    }
    //}

    cout << "Processing completed\n";

    // Close the driver.
    checkApiCall( AqMD3_close( session ) );
    cout << "\nDriver closed\n";

    return 0;
}

